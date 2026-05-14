#include "ip_rules.h"

#include <arpa/inet.h>

#include "utils.h"

int validate_ipv4(const char *ip)
{
    struct in_addr parsed;

    if (ip == NULL || ip[0] == '\0') {
        return 0;
    }

    return inet_pton(AF_INET, ip, &parsed) == 1;
}

int ip_is_whitelisted(const char *ip)
{
    /*
     * TODO: CIDR desteği eklendiğinde 192.168.1.0/24 gibi ağ blokları da
     * bu fonksiyonda kontrol edilebilir.
     */
    if (!validate_ipv4(ip)) {
        return 0;
    }

    return file_contains_line("ip_whitelist.txt", ip);
}

int ip_is_blacklisted(const char *ip)
{
    /*
     * TODO: CIDR desteği için satırdaki değerde '/' var mı kontrol edilip
     * IP'nin o ağ bloğuna düşüp düşmediği hesaplanabilir.
     */
    if (!validate_ipv4(ip)) {
        return 0;
    }

    return file_contains_line("ip_blacklist.txt", ip);
}
