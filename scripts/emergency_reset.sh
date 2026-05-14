#!/usr/bin/env bash
set -euo pipefail

NFT_BIN="$(command -v nft || true)"
if [[ -z "$NFT_BIN" && -x /usr/sbin/nft ]]; then
  NFT_BIN="/usr/sbin/nft"
fi
if [[ -z "$NFT_BIN" && -x /sbin/nft ]]; then
  NFT_BIN="/sbin/nft"
fi

if [[ -z "$NFT_BIN" ]]; then
  echo "nft komutu bulunamadı. nftables kurulu olmalı." >&2
  exit 1
fi

echo "[firewallproject] Emergency reset başlıyor."
sudo "$NFT_BIN" flush ruleset
echo "[firewallproject] Tüm nftables ruleset temizlendi."
