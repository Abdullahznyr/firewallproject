#!/usr/bin/env bash
set -euo pipefail

BACKUP_DIR="./data/backups"
BACKUP_FILE="${1:-}"

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

if [[ -z "$BACKUP_FILE" ]]; then
  BACKUP_FILE="$(ls -1t "$BACKUP_DIR"/*.nft 2>/dev/null | head -n 1 || true)"
fi

if [[ -z "$BACKUP_FILE" || ! -f "$BACKUP_FILE" ]]; then
  echo "Geri yüklenecek backup bulunamadı." >&2
  exit 1
fi

sudo "$NFT_BIN" flush ruleset
sudo "$NFT_BIN" -f "$BACKUP_FILE"
echo "[firewallproject] Restore tamamlandı: $BACKUP_FILE"
