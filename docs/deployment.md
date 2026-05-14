# Deployment

## Debian Kurulum

```bash
sudo apt update
sudo apt install -y build-essential python3 python3-venv python3-pip nftables
```

## C Engine

```bash
cd core-c
make
./firewallproject-core --test --ip 192.168.1.10 --url https://example.com --port 443
```

## Flask Backend

```bash
cd backend-python
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python app.py
```

Dashboard varsayılan olarak `http://0.0.0.0:5000` üzerinde çalışır.

## Raspberry Pi Gateway Notları

- Raspberry Pi üzerinde iki ağ arayüzü kullanılması önerilir: WAN ve LAN.
- IP forwarding kontrollü şekilde açılmalıdır.
- nftables kuralları önce simulation mode ile test edilmelidir.
- Live mode sadece fiziksel erişim veya alternatif SSH erişimi varken açılmalıdır.

## İstemci Trafiğini Gerçekten Engelleme

Bir telefonun internet trafiğini engellemek için telefonun varsayılan ağ geçidi firewallproject çalışan Debian/Raspberry Pi cihazı olmalıdır. Telefon doğrudan ev modemini gateway olarak kullanıyorsa, firewallproject sadece dashboard makinesi üzerinde kural yazar; telefonun router'a giden trafiği bu makineden geçmediği için kesilmez.

Gateway senaryosunda genel akış:

1. Telefon LAN tarafında Raspberry Pi/Debian cihazını gateway olarak kullanır.
2. Debian/Raspberry Pi üzerinde IP forwarding açılır.
3. WAN tarafı modem/router'a çıkar.
4. nftables `forward` chain üzerindeki blacklist drop kuralı istemci trafiğini keser.
