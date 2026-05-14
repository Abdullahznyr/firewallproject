"""Basit local dashboard auth yardımcıları.

Bu proje local ağ/lab kullanımı için tasarlandı. Üretim ortamında gerçek kullanıcı
yönetimi, HTTPS ve session hardening eklenmelidir.
"""

from functools import wraps

from flask import request


def require_local_request(view):
    @wraps(view)
    def wrapper(*args, **kwargs):
        # Local geliştirme için izin veriyoruz. İleride buraya token/session eklenebilir.
        _ = request.remote_addr
        return view(*args, **kwargs)

    return wrapper
