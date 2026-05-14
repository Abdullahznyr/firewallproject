# Contributing

firewallproject öğrenme ve geliştirme amaçlı açık bir Linux firewall yönetim projesidir. Hata bulursanız, eksik gördüğünüz bir özellik varsa veya daha iyi bir çözüm öneriniz olursa issue açabilir ya da pull request gönderebilirsiniz.

## Katkı Alanları

- C core engine geliştirmeleri
- nftables rule üretimi ve rollback güvenliği
- Flask backend API iyileştirmeleri
- Dashboard UI/UX geliştirmeleri
- Raspberry Pi gateway deployment testleri
- Dokümantasyon, kurulum ve hata giderme notları

## Geliştirme Akışı

1. Repoyu fork edin.
2. Yeni bir branch oluşturun.
3. Değişikliği küçük ve anlaşılır tutun.
4. C tarafını derleyin.
5. Python dosyalarını sözdizimi açısından kontrol edin.
6. Pull request açıklamasına neyi neden değiştirdiğinizi yazın.

## Kontrol Komutları

```bash
cd core-c
make clean && make
```

```bash
python3 -m py_compile backend-python/*.py
```

## Güvenlik Notu

Live mode ve emergency reset gerçek nftables ruleset üzerinde işlem yapabilir. Bu alanlarda değişiklik yaparken simulation mode ile test edin ve dokümantasyonda riskleri açıkça belirtin.
