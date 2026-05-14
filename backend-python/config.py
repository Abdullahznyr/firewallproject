"""firewallproject backend ayarları."""

from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
DATA_DIR = PROJECT_ROOT / "data"
CORE_DIR = PROJECT_ROOT / "core-c"
CORE_ENGINE = CORE_DIR / "firewallproject-core"
SCRIPTS_DIR = PROJECT_ROOT / "scripts"

RULES_FILE = DATA_DIR / "rules.json"
STATE_FILE = DATA_DIR / "firewall_state.json"
LOGS_FILE = DATA_DIR / "logs.txt"
AUDIT_FILE = DATA_DIR / "audit.log"

IP_BLACKLIST_FILE = DATA_DIR / "ip_blacklist.txt"
IP_WHITELIST_FILE = DATA_DIR / "ip_whitelist.txt"
DOMAIN_BLACKLIST_FILE = DATA_DIR / "domain_blacklist.txt"
DOMAIN_WHITELIST_FILE = DATA_DIR / "domain_whitelist.txt"
URL_BLACKLIST_FILE = DATA_DIR / "url_blacklist.txt"

PORT_BLOCK_FILE = DATA_DIR / "blocked_ports.txt"
PORT_ALLOW_FILE = DATA_DIR / "allowed_ports.txt"

RULE_CATEGORIES = {
    "ip_blacklist": IP_BLACKLIST_FILE,
    "ip_whitelist": IP_WHITELIST_FILE,
    "domain_blacklist": DOMAIN_BLACKLIST_FILE,
    "domain_whitelist": DOMAIN_WHITELIST_FILE,
    "url_blacklist": URL_BLACKLIST_FILE,
    "blocked_ports": PORT_BLOCK_FILE,
    "allowed_ports": PORT_ALLOW_FILE,
}


def ensure_data_files():
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    LOGS_FILE.touch(exist_ok=True)
    AUDIT_FILE.touch(exist_ok=True)

    for path in RULE_CATEGORIES.values():
        path.touch(exist_ok=True)

    if not RULES_FILE.exists():
        RULES_FILE.write_text(
            '{\n'
            '  "mode": "simulation",\n'
            '  "live_mode": false,\n'
            '  "default_action": "ALLOW",\n'
            '  "rules": {\n'
            '    "ip_blacklist": [],\n'
            '    "ip_whitelist": [],\n'
            '    "domain_blacklist": [],\n'
            '    "domain_whitelist": [],\n'
            '    "url_blacklist": [],\n'
            '    "blocked_ports": [],\n'
            '    "allowed_ports": []\n'
            '  }\n'
            '}\n',
            encoding="utf-8",
        )

    if not STATE_FILE.exists():
        STATE_FILE.write_text(
            '{\n'
            '  "nftables_applied": false,\n'
            '  "last_apply": null,\n'
            '  "last_rollback": null,\n'
            '  "last_error": null\n'
            '}\n',
            encoding="utf-8",
        )
