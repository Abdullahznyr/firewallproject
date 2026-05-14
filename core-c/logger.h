#ifndef FIREWALLPROJECT_LOGGER_H
#define FIREWALLPROJECT_LOGGER_H

void log_event(
    const char *action,
    const char *rule_id,
    const char *ip,
    const char *domain,
    const char *url,
    int port,
    const char *service,
    const char *reason
);

void audit_event(const char *actor, const char *action, const char *detail);

#endif
