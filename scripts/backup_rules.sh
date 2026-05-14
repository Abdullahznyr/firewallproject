#!/usr/bin/env bash
set -euo pipefail

BACKUP_DIR="${1:-./data/backups}"
mkdir -p "$BACKUP_DIR"

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

BACKUP_FILE="$BACKUP_DIR/nftables-$(date +%Y%m%d-%H%M%S).nft"
sudo "$NFT_BIN" list ruleset > "$BACKUP_FILE"
echo "$BACKUP_FILE"
