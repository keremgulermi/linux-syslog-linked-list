#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SATIR_UZUNLUGU 1024

/* * YAPI: SyslogNode (Çift Yönlü Baðlý Liste Düðümü)
 * Amaç: Her bir syslog satýrýný bellekte tutmak ve 
 * bir önceki/sonraki log kayýtlarýna baðlantý saðlamak.
 */
typedef struct SyslogNode {
    char logMesaji[MAX_SATIR_UZUNLUGU]; // Log satýrýnýn tutulduðu metin dizisi
    struct SyslogNode* onceki;          // Bir önceki log düðümüne iþaretçi
    struct SyslogNode* sonraki;         // Bir sonraki log düðümüne iþaretçi
} SyslogNode;

/*
 * FONKSÝYON: yeniDugumOlustur
 * Amaç: Bellekte (Heap) yeni bir log düðümü için yer ayýrmak.
 * Parametre: eklenecek_mesaj (Kayýt edilecek log satýrý)
 * Dönüþ: Oluþturulan yeni düðümün bellek adresi
 */
SyslogNode* yeniDugumOlustur(const char* eklenecek_mesaj) {
    SyslogNode* yeni_dugum = (SyslogNode*)malloc(sizeof(SyslogNode));
    if (yeni_dugum == NULL) {
        printf("Hata: Bellek tahsisi basarisiz oldu!\n");
        exit(1);
    }
    // Gelen log mesajýný düðümün içine kopyala
    strncpy(yeni_dugum->logMesaji, eklenecek_mesaj, MAX_SATIR_UZUNLUGU - 1);
    yeni_dugum->logMesaji[MAX_SATIR_UZUNLUGU - 1] = '\0'; // Güvenlik için null-terminator
    
    yeni_dugum->onceki = NULL;
    yeni_dugum->sonraki = NULL;
    
    return yeni_dugum;
}

/*
 * FONKSÝYON: logEkle
 * Amaç: Çift yönlü baðlý listenin sonuna yeni bir log düðümü eklemek.
 * ZAMAN KARMAÞIKLIÐI: Kuyruk (tail) iþaretçisi kullandýðýmýz için $O(1)$ sürede ekleme yapar.
 * Parametreler: bas (listenin baþý), son (listenin sonu), mesaj (log içeriði)
 */
void logEkle(SyslogNode** bas, SyslogNode** son, const char* mesaj) {
    SyslogNode* yeni_dugum = yeniDugumOlustur(mesaj);

    if (*bas == NULL) {
        // Liste boþsa, ilk düðüm hem baþ hem de sondur.
        *bas = yeni_dugum;
        *son = yeni_dugum;
    } else {
        // Liste doluysa, mevcut son düðümün arkasýna ekle
        (*son)->sonraki = yeni_dugum;
        yeni_dugum->onceki = *son;
        *son = yeni_dugum; // Yeni son düðüm artýk bu
    }
}

/*
 * FONKSÝYON: loglariBastirma (Eskiden Yeniye / Kronolojik)
 * Amaç: Listeyi baþtan sona okuyarak ekrana yazdýrmak.
 */
void loglariBastirma(SyslogNode* bas) {
    printf("\n--- SYSLOG KAYITLARI (KRONOLOJIK) ---\n");
    SyslogNode* gecici = bas;
    while (gecici != NULL) {
        printf("%s", gecici->logMesaji);
        gecici = gecici->sonraki;
    }
    printf("-------------------------------------\n");
}

/*
 * FONKSÝYON: loglariTersBastirma (Yeniden Eskiye / Ters Kronolojik)
 * Amaç: Listeyi sondan baþa okuyarak ekrana yazdýrmak. Bu sistem yöneticilerinin 
 * en son oluþan hatalarý hýzlýca görmesi için önemlidir.
 */
void loglariTersBastirma(SyslogNode* son) {
    printf("\n--- SYSLOG KAYITLARI (EN GUNCELDEN ESKIYE) ---\n");
    SyslogNode* gecici = son;
    while (gecici != NULL) {
        printf("%s", gecici->logMesaji);
        gecici = gecici->onceki;
    }
    printf("-------------------------------------\n");
}

/*
 * FONKSÝYON: bellekTemizle
 * Amaç: Program sonunda sýzýntýlarý (memory leak) önlemek için listeyi yok etmek.
 */
void bellekTemizle(SyslogNode* bas) {
    SyslogNode* gecici;
    while (bas != NULL) {
        gecici = bas;
        bas = bas->sonraki;
        free(gecici);
    }
}

int main() {
    SyslogNode* listeBasi = NULL;
    SyslogNode* listeSonu = NULL;
    
    // Linux'ta gercek syslog dosyasi genellikle /var/log/syslog altindadir.
    // Ancak yetki sorunlari yasamamak icin videoda local bir dosya kullanilabilir.
    FILE *dosya = fopen("ornek_syslog.txt", "r");
    
    if (dosya == NULL) {
        printf("Hata: Syslog dosyasi acilamadi. Lutfen 'ornek_syslog.txt' dosyasini olusturun.\n");
        return 1;
    }

    char satir_tamponu[MAX_SATIR_UZUNLUGU];
    
    // Dosyayý satýr satýr okuyup baðlý listeye ekleme
    while (fgets(satir_tamponu, sizeof(satir_tamponu), dosya) != NULL) {
        logEkle(&listeBasi, &listeSonu, satir_tamponu);
    }
    fclose(dosya);

    // DÝKKAT: Demo sýrasýnda bu iki fonksiyon çaðrýlacaktýr
    printf("1. Bagli Listeyi Bastan Sona Okuma Ismi (Normal Akis)\n");
    loglariBastirma(listeBasi);

    printf("\n2. Bagli Listeyi Sondan Basa Okuma Ismi (Hata Ayiklama Modu)\n");
    loglariTersBastirma(listeSonu);

    // Belleði sisteme iade et
    bellekTemizle(listeBasi);

    return 0;
}
