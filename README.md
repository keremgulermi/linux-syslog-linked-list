# Linux Syslog Okuyucu - Çift Yönlü Bağlı Liste Uygulaması

Bu proje, Linux sistem günlüklerini (syslog) okuyup bellek üzerinde **Çift Yönlü Bağlı Liste (Doubly Linked List)** veri yapısı kullanarak saklayan ve işleyen bir C programıdır. Bu uygulama, bağlı listelerin dinamik bellek yönetimi avantajlarını kullanarak büyüklüğü önceden bilinmeyen log dosyalarını verimli bir şekilde yönetmeyi amaçlamaktadır.

## Neden Çift Yönlü Bağlı Liste Kullanıldı?
Log dosyaları genellikle çok büyüktür ve sürekli olarak yeni kayıtlar eklenir. Sistem yöneticileri bazen logları kronolojik sırada (eskiden yeniye), bazen ise anlık oluşan sorunları tespit etmek için **ters kronolojik sırada (en yeniden eskiye)** incelemek isterler. Çift yönlü bağlı liste, `onceki` (prev) ve `sonraki` (next) işaretçileri (pointer) barındırdığı için loglar arasında her iki yönde de $O(n)$ karmaşıklığında hızlı gezinme imkanı sunar.

## Özellikler
* Log dosyalarının dinamik olarak Heap bellekte düğümler (nodes) halinde saklanması.
* Kuyruk (tail) işaretçisi kullanımı sayesinde $O(1)$ karmaşıklığında listeye yeni log ekleme performansı.
* İleriye ve geriye doğru log listeleme seçenekleri.
* İşlem bitiminde `free()` kullanılarak bellek sızıntılarının (memory leak) önlenmesi
