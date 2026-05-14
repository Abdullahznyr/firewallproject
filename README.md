# firewallproject

firewallproject, Debian/Linux üzerinde çalışan ve Raspberry Pi gateway cihazına taşınabilecek modüler firewall yönetim platformudur.

Proje; C ile yazılmış core decision engine, Python/Flask backend, nftables entegrasyonu, JSON tabanlı kural sistemi ve responsive dashboard katmanlarından oluşur.

## Özellikler

- C core firewall decision engine
- Flask backend ve dashboard
- JSON tabanlı rule yönetimi
- IP/domain/URL/port allow-block listeleri
- HTTP/HTTPS ayrımı
- Simulation mode ve live mode
- nftables adapter
- Rollback ve emergency reset scriptleri
- Traffic log ve audit log
- Responsive dark dashboard
- Raspberry Pi gateway senaryosuna uygun mimari

## Proje Yapısı

```text
firewallproject/
├── core-c/              # C core engine ve nftables adapter
├── backend-python/      # Flask backend ve API
├── dashboard/           # HTML/CSS/JS dashboard
├── data/                # JSON rules, state ve runtime data
├── scripts/             # backup, restore, emergency reset
├── docs/                # mimari ve deployment notları
└── README.md
```

## Kurulum

Debian/Raspberry Pi OS:

```bash
sudo apt update
sudo apt install -y build-essential python3 python3-venv python3-pip nftables
```

Repoyu klonlayın:

```bash
git clone https://github.com/<kullanici-adi>/firewallproject.git
cd firewallproject
```

## C Engine Derleme

```bash
cd core-c
make
```

Test:

```bash
./firewallproject-core --test --ip 192.168.1.10 --url https://example.com/login --port 443
```

Dry-run nftables apply:

```bash
./firewallproject-core --apply --dry-run
```

Temizleme:

```bash
make clean
```

## Dashboard Çalıştırma

```bash
cd backend-python
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python app.py
```

Tarayıcıdan:

```text
http://localhost:5000
```

LAN üzerinden erişmek için:

```bash
FIREWALLPROJECT_HOST=0.0.0.0 python app.py
```

Debug açmak için:

```bash
FIREWALLPROJECT_DEBUG=1 python app.py
```

Debug modunu LAN’a açık şekilde kullanmayın.

## Live Mode

Varsayılan mod `simulation` modudur. Bu mod gerçek nftables kurallarını değiştirmez. Dashboard üzerinden apply işlemi dry-run olarak yürür.

Live mode gerçek nftables ruleset üzerinde işlem yapabilir. Root yetkisi gerekir:

```bash
cd backend-python
sudo -E .venv/bin/python app.py
```

Dashboard üzerinden:

1. Rules sayfasından IP/domain/URL/port kuralı ekleyin.
2. Live Mode sayfasından live mode açın.
3. Apply Rules butonuna basın.
4. Sistemde kuralı kontrol edin:

```bash
sudo nft list table inet firewallproject
```

## Gateway Notu

Bir telefonun veya başka bir istemcinin internetini kesmek için Debian/Raspberry Pi cihazı o istemcinin gateway'i olmalıdır.

Eğer telefon doğrudan modem/router üzerinden internete çıkıyorsa, aynı LAN’daki firewallproject makinesi o trafiği göremez ve engelleyemez.

Doğru senaryo:

```text
Telefon / IoT cihaz
        |
Raspberry Pi / Debian firewallproject
        |
Modem / Router
        |
İnternet
```

Aktif IP blacklist kuralları nftables tarafında şu mantığa çevrilir:

```text
inet firewallproject input   ip saddr <IP> drop
inet firewallproject forward ip saddr <IP> drop
inet firewallproject output  ip daddr <IP> drop
```

## Rollback

Backup almak:

```bash
./scripts/backup_rules.sh
```

Restore:

```bash
./scripts/restore_rules.sh
```

## Emergency Reset

```bash
./scripts/emergency_reset.sh
```

Bu komut tüm nftables ruleset'i temizler. Uzak bağlantı üzerinden kullanırken dikkatli olun.

## Katkı ve Geri Bildirim

Hata bulursanız, eksik gördüğünüz bir özellik varsa veya daha iyi bir çözüm öneriniz olursa issue açabilir ya da pull request gönderebilirsiniz.

Özellikle şu alanlarda katkılar değerlidir:

- Raspberry Pi gateway testleri
- nftables rule üretimi
- Dashboard geliştirmeleri
- Güvenli live mode akışları
- Dokümantasyon ve hata giderme notları

Katkı rehberi için [CONTRIBUTING.md](CONTRIBUTING.md) dosyasına bakın.

## Güvenli Kullanım

Bu proje öğrenme, laboratuvar ve kendi cihazlarınızı koruma amacıyla hazırlanmıştır. Başkasına ait ağlarda veya sistemlerde izinsiz kullanım yasal değildir.

Live mode, rollback ve emergency reset özelliklerini kullanmadan önce fiziksel erişiminiz veya alternatif bağlantı yolunuz olduğundan emin olun.
