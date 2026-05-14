"""firewallproject Flask dashboard uygulaması."""

import os

from flask import Flask, jsonify, render_template, request

import firewall_manager
import log_manager
import rules_manager
from auth import require_local_request
from config import PROJECT_ROOT, ensure_data_files
from engine_runner import run_engine_test


app = Flask(
    __name__,
    template_folder=str(PROJECT_ROOT / "dashboard" / "templates"),
    static_folder=str(PROJECT_ROOT / "dashboard" / "static"),
)


@app.before_request
def prepare_files():
    ensure_data_files()


@app.route("/")
@require_local_request
def index():
    return render_template("index.html", stats=rules_manager.stats(), logs=log_manager.get_logs(5), status=firewall_manager.get_status())


@app.route("/rules")
@require_local_request
def rules_page():
    return render_template("rules.html", rules=rules_manager.load_rules())


@app.route("/logs")
@require_local_request
def logs_page():
    return render_template("logs.html", logs=log_manager.get_logs(), audit_logs=log_manager.get_audit_logs())


@app.route("/interfaces")
@require_local_request
def interfaces_page():
    return render_template("interfaces.html", interfaces=firewall_manager.list_interfaces(), status=firewall_manager.get_status())


@app.route("/live-mode")
@require_local_request
def live_mode_page():
    return render_template("live_mode.html", rules=rules_manager.load_rules(), status=firewall_manager.get_status())


@app.route("/test")
@require_local_request
def test_page():
    return render_template("test.html")


@app.get("/api/rules")
@require_local_request
def api_rules_get():
    return jsonify(rules_manager.load_rules())


@app.post("/api/rules")
@require_local_request
def api_rules_post():
    payload = request.get_json(force=True, silent=True) or {}
    try:
        entry = rules_manager.add_rule(
            payload.get("category", ""),
            payload.get("value", ""),
            payload.get("description", ""),
        )
        return jsonify({"ok": True, "entry": entry})
    except ValueError as exc:
        return jsonify({"ok": False, "error": str(exc)}), 400


@app.patch("/api/rules")
@require_local_request
def api_rules_patch():
    payload = request.get_json(force=True, silent=True) or {}
    try:
        entry = rules_manager.set_rule_enabled(
            payload.get("category", ""),
            payload.get("value", ""),
            payload.get("enabled", True),
        )
        return jsonify({"ok": True, "entry": entry})
    except ValueError as exc:
        return jsonify({"ok": False, "error": str(exc)}), 400


@app.delete("/api/rules")
@require_local_request
def api_rules_delete():
    payload = request.get_json(force=True, silent=True) or {}
    try:
        rules_manager.delete_rule(payload.get("category", ""), payload.get("value", ""))
        return jsonify({"ok": True})
    except ValueError as exc:
        return jsonify({"ok": False, "error": str(exc)}), 400


@app.get("/api/logs")
@require_local_request
def api_logs():
    return jsonify({"logs": log_manager.get_logs(), "audit": log_manager.get_audit_logs()})


@app.get("/api/interfaces")
@require_local_request
def api_interfaces():
    return jsonify({"interfaces": firewall_manager.list_interfaces(), "status": firewall_manager.get_status()})


@app.post("/api/test")
@require_local_request
def api_test():
    payload = request.get_json(force=True, silent=True) or {}
    result = run_engine_test(
        ip=payload.get("ip", ""),
        url=payload.get("url", ""),
        domain=payload.get("domain", ""),
        port=payload.get("port", ""),
        scheme=payload.get("protocol", ""),
    )
    return jsonify(result)


@app.post("/api/apply")
@require_local_request
def api_apply():
    return jsonify(firewall_manager.apply_rules())


@app.post("/api/rollback")
@require_local_request
def api_rollback():
    return jsonify(firewall_manager.rollback())


@app.post("/api/emergency-reset")
@require_local_request
def api_emergency_reset():
    return jsonify(firewall_manager.emergency_reset())


@app.post("/api/live-mode")
@require_local_request
def api_live_mode():
    payload = request.get_json(force=True, silent=True) or {}
    enabled = bool(payload.get("enabled", False))
    data = rules_manager.set_mode("live" if enabled else "simulation", enabled)
    return jsonify({"ok": True, "rules": data})


@app.get("/favicon.ico")
def favicon():
    return "", 204


if __name__ == "__main__":
    ensure_data_files()
    host = os.environ.get("FIREWALLPROJECT_HOST", "127.0.0.1")
    port = int(os.environ.get("FIREWALLPROJECT_PORT", "5000"))
    debug = os.environ.get("FIREWALLPROJECT_DEBUG", "0") == "1"
    app.run(host=host, port=port, debug=debug)
