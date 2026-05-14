# nftables Integration

firewallproject nftables entegrasyonunu iki seviyede ele alır:

1. C core içindeki `nftables_adapter.c`
2. Backend ve `scripts/` altındaki güvenli operasyon komutları

## Simulation Mode

Varsayılan mod simulation modudur. Bu modda apply işlemi `--dry-run` olarak çalışır ve nftables komutları ekrana yazılır; gerçek sistem kuralı değişmez.

## Live Mode

Live mode gerçek nftables ruleset üzerinde işlem yapar. Root yetkisi veya uygun sudoers yapılandırması gerekir.

## Rollback

Rollback için önce `scripts/backup_rules.sh` ile mevcut ruleset dosyaya alınmalıdır. `scripts/restore_rules.sh` en yeni backup dosyasını veya parametre olarak verilen backup dosyasını geri yükler.

## Emergency Reset

`scripts/emergency_reset.sh` tüm nftables ruleset'i temizler. Bu komut erişim kesintisi yaratabileceği için yalnızca kontrollü ortamda kullanılmalıdır.

## Not

Şu an C adapter base table/chain kurar. IP/domain/URL/port listelerini nftables kurallarına tam otomatik çevirmek için sonraki adım backend tarafında atomik nft script üretimidir.
