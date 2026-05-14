# firewallproject Architecture

firewallproject; C çekirdek motoru, Flask backend, nftables adaptörü ve modern dashboard katmanlarından oluşan modüler Linux firewall yönetim sistemidir.

## Katmanlar

- `core-c`: IP, domain, URL, port ve scheme bilgilerini değerlendirir. Karar sonucu `ALLOW`, `BLOCK` veya `UNKNOWN` olabilir. Risk seviyesi `LOW`, `MEDIUM`, `HIGH` olarak üretilir.
- `backend-python`: Flask API ve dashboard route katmanıdır. JSON kuralları yönetir, C motorunu çalıştırır, nftables apply/rollback/reset akışlarını tetikler.
- `dashboard`: Local ağ kullanımına uygun responsive dark UI sağlar.
- `data`: JSON kurallar, aktif text rule dosyaları, traffic log, audit log ve firewall state dosyasını tutar.
- `scripts`: Emergency reset, backup ve restore işlemlerini kapsar.

## Karar Sırası

1. URL parse edilir ve scheme/domain/path ayrılır.
2. IP whitelist kontrol edilir.
3. IP blacklist kontrol edilir.
4. Domain whitelist kontrol edilir.
5. Domain blacklist kontrol edilir.
6. URL blacklist kontrol edilir.
7. Port allow/block listesi ve risk seviyesi kontrol edilir.
8. Eşleşme yoksa default davranış uygulanır.

Whitelist kararları blacklist öncesinde kısa devre yapar. Bu davranış kontrollü ortamlar için pratiktir; üretimde politika ihtiyacına göre değiştirilebilir.
