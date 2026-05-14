# Security Policy

firewallproject ağ trafiğini yönetmeyi hedefleyen bir güvenlik aracıdır. Bu nedenle güvenlik hatalarını açık issue olarak paylaşmadan önce dikkatli olun.

## Bildirim

Güvenlik açığı, yetki atlama, yanlış nftables kuralı üretimi veya live mode kaynaklı risk bulursanız:

- Mümkünse önce maintainer ile özel kanaldan paylaşın.
- Açık issue açacaksanız sömürü adımlarını zararlı biçimde detaylandırmayın.
- Hatanın etkisini, beklenen davranışı ve önerilen düzeltmeyi yazın.

## Kapsam

- C core engine
- Flask backend API
- nftables apply/rollback/reset akışları
- Dashboard üzerinden rule yönetimi
- Raspberry Pi gateway deployment senaryosu

## Güvenli Kullanım

Projeyi yalnızca kendi cihazlarınızda, laboratuvar ortamında veya açık izin verilen ağlarda kullanın.
