"""JSON tabanlı kural yönetimi."""

import json
from copy import deepcopy
from json import JSONDecodeError

from config import RULE_CATEGORIES, RULES_FILE, ensure_data_files
from log_manager import audit
from validators import normalize_rule_value, validate_category


DEFAULT_RULES = {
    "mode": "simulation",
    "live_mode": False,
    "default_action": "ALLOW",
    "rules": {
        "ip_blacklist": [],
        "ip_whitelist": [],
        "domain_blacklist": [],
        "domain_whitelist": [],
        "url_blacklist": [],
        "blocked_ports": [],
        "allowed_ports": [],
    },
}


def _normalize_entry(entry):
    if isinstance(entry, dict):
        return {
            "value": entry.get("value"),
            "enabled": bool(entry.get("enabled", True)),
            "description": entry.get("description", ""),
        }

    return {
        "value": entry,
        "enabled": True,
        "description": "",
    }


def normalize_rules(data):
    normalized = deepcopy(DEFAULT_RULES)

    if isinstance(data, dict):
        normalized["mode"] = data.get("mode", normalized["mode"])
        normalized["live_mode"] = bool(data.get("live_mode", normalized["live_mode"]))
        normalized["default_action"] = data.get("default_action", normalized["default_action"])

        rules = data.get("rules", {})
        if isinstance(rules, dict):
            for category in normalized["rules"]:
                raw_entries = rules.get(category, [])
                if isinstance(raw_entries, list):
                    normalized["rules"][category] = [_normalize_entry(item) for item in raw_entries]

    if normalized["mode"] not in ("simulation", "live"):
        normalized["mode"] = "simulation"

    normalized["live_mode"] = normalized["mode"] == "live" and normalized["live_mode"]
    return normalized


def load_rules():
    ensure_data_files()

    try:
        data = json.loads(RULES_FILE.read_text(encoding="utf-8"))
    except (FileNotFoundError, JSONDecodeError):
        data = deepcopy(DEFAULT_RULES)

    normalized = normalize_rules(data)
    save_rules(normalized, sync=False)
    sync_text_files(normalized)
    return normalized


def save_rules(data, sync=True):
    ensure_data_files()
    RULES_FILE.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if sync:
        sync_text_files(data)


def sync_text_files(data=None):
    if data is None:
        data = load_rules()

    for category, path in RULE_CATEGORIES.items():
        entries = data["rules"].get(category, [])
        active_values = []

        for entry in entries:
            normalized = _normalize_entry(entry)
            if normalized["enabled"] and normalized["value"] not in (None, ""):
                active_values.append(str(normalized["value"]))

        path.write_text("\n".join(active_values) + ("\n" if active_values else ""), encoding="utf-8")


def add_rule(category, value, description=""):
    category = validate_category(category)
    normalized_value = normalize_rule_value(category, value)
    data = load_rules()

    entries = data["rules"][category]
    for entry in entries:
        if str(entry.get("value")) == str(normalized_value):
            entry["enabled"] = True
            entry["description"] = description or entry.get("description", "")
            save_rules(data)
            audit("RULE_REENABLE", f"{category}:{normalized_value}")
            return entry

    entry = {
        "value": normalized_value,
        "enabled": True,
        "description": description,
    }
    entries.append(entry)
    save_rules(data)
    audit("RULE_ADD", f"{category}:{normalized_value}")
    return entry


def set_rule_enabled(category, value, enabled):
    category = validate_category(category)
    data = load_rules()

    for entry in data["rules"][category]:
        if str(entry.get("value")) == str(value):
            entry["enabled"] = bool(enabled)
            save_rules(data)
            audit("RULE_ENABLE" if enabled else "RULE_DISABLE", f"{category}:{value}")
            return entry

    raise ValueError("Kural bulunamadı")


def delete_rule(category, value):
    category = validate_category(category)
    data = load_rules()
    before = len(data["rules"][category])
    data["rules"][category] = [
        entry for entry in data["rules"][category]
        if str(entry.get("value")) != str(value)
    ]

    if len(data["rules"][category]) == before:
        raise ValueError("Kural bulunamadı")

    save_rules(data)
    audit("RULE_DELETE", f"{category}:{value}")


def set_mode(mode, live_mode=None):
    if mode not in ("simulation", "live"):
        raise ValueError("Mode simulation veya live olmalı")

    data = load_rules()
    data["mode"] = mode
    data["live_mode"] = bool(live_mode) if live_mode is not None else mode == "live"

    if mode == "simulation":
        data["live_mode"] = False

    save_rules(data)
    audit("MODE_CHANGE", f"mode={data['mode']} live_mode={data['live_mode']}")
    return data


def stats():
    data = load_rules()
    total = 0
    active = 0

    for entries in data["rules"].values():
        total += len(entries)
        active += sum(1 for entry in entries if entry.get("enabled", True))

    return {
        "total_rules": total,
        "active_rules": active,
        "blocked_ip_count": len(data["rules"]["ip_blacklist"]),
        "mode": data["mode"],
        "live_mode": data["live_mode"],
    }
