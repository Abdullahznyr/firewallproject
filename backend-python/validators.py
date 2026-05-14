"""Input validation yardımcıları."""

import ipaddress
import re
from urllib.parse import urlparse


DOMAIN_RE = re.compile(r"^(?=.{1,253}$)([a-zA-Z0-9-]{1,63}\.)+[a-zA-Z]{2,63}$")


def validate_ipv4(value):
    try:
        return str(ipaddress.IPv4Address(str(value).strip()))
    except ValueError as exc:
        raise ValueError("Geçersiz IPv4 adresi") from exc


def validate_domain(value):
    domain = str(value).strip().lower()
    if domain.startswith("www."):
        domain = domain[4:]

    if not DOMAIN_RE.match(domain):
        raise ValueError("Geçersiz domain")

    return domain


def validate_url(value):
    url = str(value).strip()
    parsed = urlparse(url)

    if parsed.scheme not in ("http", "https"):
        raise ValueError("URL http veya https ile başlamalı")
    if not parsed.netloc:
        raise ValueError("URL domain içermeli")

    domain = validate_domain(parsed.netloc)
    path = parsed.path or "/"
    return f"{parsed.scheme}://{domain}{path}"


def validate_port(value):
    try:
        port = int(value)
    except (TypeError, ValueError) as exc:
        raise ValueError("Port sayı olmalı") from exc

    if port < 1 or port > 65535:
        raise ValueError("Port 1-65535 aralığında olmalı")

    return port


def validate_category(category):
    allowed = {
        "ip_blacklist",
        "ip_whitelist",
        "domain_blacklist",
        "domain_whitelist",
        "url_blacklist",
        "blocked_ports",
        "allowed_ports",
    }

    if category not in allowed:
        raise ValueError("Geçersiz kural kategorisi")

    return category


def normalize_rule_value(category, value):
    category = validate_category(category)

    if category.startswith("ip_"):
        return validate_ipv4(value)
    if category.startswith("domain_"):
        return validate_domain(value)
    if category == "url_blacklist":
        return validate_url(value)
    if category in ("blocked_ports", "allowed_ports"):
        return validate_port(value)

    raise ValueError("Geçersiz kural")
