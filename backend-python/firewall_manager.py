"""nftables, live mode ve rollback yönetimi."""

import json
import os
import shutil
import subprocess
from datetime import datetime

from config import SCRIPTS_DIR, STATE_FILE, ensure_data_files
from engine_runner import run_engine_command
from log_manager import audit
from rules_manager import load_rules, sync_text_files


def resolve_nft_binary():
    candidates = [
        shutil.which("nft"),
        "/usr/sbin/nft",
        "/sbin/nft",
        "/usr/bin/nft",
        "/bin/nft",
    ]

    for candidate in candidates:
        if candidate and os.path.exists(candidate) and os.access(candidate, os.X_OK):
            return candidate

    return None


def _load_state():
    ensure_data_files()
    try:
        return json.loads(STATE_FILE.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return {
            "nftables_applied": False,
            "last_apply": None,
            "last_rollback": None,
            "last_error": "state file yeniden oluşturuldu",
        }


def _save_state(state):
    ensure_data_files()
    STATE_FILE.write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def get_status():
    rules = load_rules()
    state = _load_state()
    nft_path = resolve_nft_binary()

    if nft_path is None:
        return {
            "mode": rules["mode"],
            "live_mode": rules["live_mode"],
            "nft_available": False,
            "nft_accessible": False,
            "nft_returncode": None,
            "nft_path": None,
            "nft_error": "nft binary bulunamadı. Debian için: sudo apt install nftables",
            "state": state,
        }

    nft_status = subprocess.run(
        [nft_path, "list", "ruleset"],
        text=True,
        capture_output=True,
        check=False,
    )

    return {
        "mode": rules["mode"],
        "live_mode": rules["live_mode"],
        "nft_available": True,
        "nft_accessible": nft_status.returncode == 0,
        "nft_path": nft_path,
        "nft_returncode": nft_status.returncode,
        "nft_error": nft_status.stderr.strip(),
        "state": state,
    }


def apply_rules():
    rules = load_rules()
    sync_text_files(rules)
    dry_run = not rules["live_mode"] or rules["mode"] == "simulation"
    args = ["--apply"]

    if dry_run:
        args.append("--dry-run")

    result = run_engine_command(args)
    state = _load_state()
    now = datetime.now().isoformat(timespec="seconds")
    state["last_apply"] = now
    state["nftables_applied"] = result["ok"] and not dry_run
    state["last_error"] = None if result["ok"] else result.get("stderr")
    _save_state(state)

    audit("FIREWALL_APPLY", f"dry_run={dry_run} ok={result['ok']}")
    return {"dry_run": dry_run, "result": result, "state": state}


def rollback():
    script = SCRIPTS_DIR / "restore_rules.sh"
    result = subprocess.run(
        [str(script)],
        text=True,
        capture_output=True,
        check=False,
    )
    state = _load_state()
    state["last_rollback"] = datetime.now().isoformat(timespec="seconds")
    state["last_error"] = None if result.returncode == 0 else result.stderr.strip()
    _save_state(state)
    audit("FIREWALL_ROLLBACK", f"ok={result.returncode == 0}")

    return {
        "ok": result.returncode == 0,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "returncode": result.returncode,
        "state": state,
    }


def emergency_reset():
    script = SCRIPTS_DIR / "emergency_reset.sh"
    result = subprocess.run(
        [str(script)],
        text=True,
        capture_output=True,
        check=False,
    )
    state = _load_state()
    state["nftables_applied"] = False
    state["last_error"] = None if result.returncode == 0 else result.stderr.strip()
    _save_state(state)
    audit("FIREWALL_EMERGENCY_RESET", f"ok={result.returncode == 0}")

    return {
        "ok": result.returncode == 0,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "returncode": result.returncode,
        "state": state,
    }


def list_interfaces():
    interfaces = []
    sys_class_net = "/sys/class/net"

    if not os.path.isdir(sys_class_net):
        return interfaces

    for name in sorted(os.listdir(sys_class_net)):
        if name:
            interfaces.append({
                "name": name,
                "loopback": name == "lo",
            })

    return interfaces
