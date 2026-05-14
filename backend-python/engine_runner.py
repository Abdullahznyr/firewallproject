"""C firewall engine çalıştırma yardımcıları."""

import subprocess

from config import CORE_DIR, CORE_ENGINE


def build_engine():
    result = subprocess.run(
        ["make"],
        cwd=CORE_DIR,
        text=True,
        capture_output=True,
        check=False,
    )

    return {
        "ok": result.returncode == 0,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "returncode": result.returncode,
    }


def run_engine_test(ip="", url="", domain="", port="", scheme=""):
    build = build_engine()
    if not build["ok"]:
        return {"ok": False, "build": build, "stdout": "", "stderr": "C engine derlenemedi"}

    command = [str(CORE_ENGINE), "--test"]
    if ip:
        command.extend(["--ip", ip])
    if url:
        command.extend(["--url", url])
    if domain:
        command.extend(["--domain", domain])
    if port:
        command.extend(["--port", str(port)])
    if scheme:
        command.extend(["--scheme", scheme])

    result = subprocess.run(
        command,
        cwd=CORE_DIR,
        text=True,
        capture_output=True,
        check=False,
    )

    return {
        "ok": result.returncode in (0, 2),
        "returncode": result.returncode,
        "stdout": result.stdout.strip(),
        "stderr": result.stderr.strip(),
        "build": build,
    }


def run_engine_command(args):
    build = build_engine()
    if not build["ok"]:
        return {"ok": False, "build": build, "stdout": "", "stderr": "C engine derlenemedi"}

    result = subprocess.run(
        [str(CORE_ENGINE), *args],
        cwd=CORE_DIR,
        text=True,
        capture_output=True,
        check=False,
    )

    return {
        "ok": result.returncode == 0,
        "returncode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
        "build": build,
    }
