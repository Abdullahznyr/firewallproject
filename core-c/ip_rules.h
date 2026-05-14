#ifndef FIREWALLPROJECT_IP_RULES_H
#define FIREWALLPROJECT_IP_RULES_H

int validate_ipv4(const char *ip);
int ip_is_whitelisted(const char *ip);
int ip_is_blacklisted(const char *ip);

#endif
