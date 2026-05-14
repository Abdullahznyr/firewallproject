"""Log ve audit dosyası yönetimi."""

from datetime import datetime

from config import AUDIT_FILE, LOGS_FILE, ensure_data_files


def read_lines(path, limit=200):
    ensure_data_files()

    if not path.exists():
        return []

    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    return lines[-limit:]


def get_logs(limit=200):
    return read_lines(LOGS_FILE, limit)


def get_audit_logs(limit=200):
    return read_lines(AUDIT_FILE, limit)


def audit(action, detail, actor="dashboard"):
    ensure_data_files()
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    with AUDIT_FILE.open("a", encoding="utf-8") as file:
        file.write(f"[{timestamp}] ACTOR={actor} ACTION={action} DETAIL={detail}\n")
