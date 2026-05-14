/*
 * port_check.c
 *
 * Bu modül port doğrulama ve port-servis eşleştirme işlemleri için hazırlanır.
 */

#include "port_check.h"

#include <stdio.h>

#include "utils.h"

int validate_port(int port)
{
    if (port >= 1 && port <= 65535) {
        return 1;
    }

    return 0;
}

const char *check_port_service(int port)
{
    switch (port) {
    case 21:
        return "FTP";
    case 22:
        return "SSH";
    case 25:
        return "SMTP";
    case 53:
        return "DNS";
    case 80:
        return "HTTP";
    case 443:
        return "HTTPS";
    case 3306:
        return "MYSQL";
    case 5432:
        return "POSTGRESQL";
    default:
        return "UNKNOWN";
    }
}

PortRisk check_port_risk(int port)
{
    switch (port) {
    case 21:
    case 22:
    case 25:
    case 3306:
    case 5432:
        return PORT_RISK_HIGH;
    case 53:
    case 80:
    case 443:
        return PORT_RISK_MEDIUM;
    default:
        return PORT_RISK_LOW;
    }
}

const char *port_risk_to_string(PortRisk risk)
{
    switch (risk) {
    case PORT_RISK_HIGH:
        return "HIGH";
    case PORT_RISK_MEDIUM:
        return "MEDIUM";
    default:
        return "LOW";
    }
}

static int port_in_file(const char *filename, int port)
{
    char value[16];

    if (!validate_port(port)) {
        return 0;
    }

    snprintf(value, sizeof(value), "%d", port);
    return file_contains_line(filename, value);
}

int port_is_allowed(int port)
{
    return port_in_file("allowed_ports.txt", port);
}

int port_is_blocked(int port)
{
    return port_in_file("blocked_ports.txt", port);
}
