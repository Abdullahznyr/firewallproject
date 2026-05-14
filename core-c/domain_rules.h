#ifndef FIREWALLPROJECT_DOMAIN_RULES_H
#define FIREWALLPROJECT_DOMAIN_RULES_H

int validate_domain_name(const char *domain);
int domain_is_whitelisted(const char *domain);
int domain_is_blacklisted(const char *domain);

#endif
