#ifndef FIREWALLPROJECT_PORT_CHECK_H
#define FIREWALLPROJECT_PORT_CHECK_H

typedef enum {
    PORT_RISK_LOW = 0,
    PORT_RISK_MEDIUM,
    PORT_RISK_HIGH
} PortRisk;

int validate_port(int port);
const char *check_port_service(int port);
PortRisk check_port_risk(int port);
const char *port_risk_to_string(PortRisk risk);
int port_is_allowed(int port);
int port_is_blocked(int port);

#endif
