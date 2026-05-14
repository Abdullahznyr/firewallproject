#ifndef FIREWALLPROJECT_ENGINE_H
#define FIREWALLPROJECT_ENGINE_H

#include "url_parser.h"

typedef enum {
    FIREWALL_ALLOW = 0,
    FIREWALL_BLOCK,
    FIREWALL_UNKNOWN
} FirewallDecision;

typedef enum {
    RISK_LOW = 0,
    RISK_MEDIUM,
    RISK_HIGH
} RiskLevel;

typedef struct {
    char ip[64];
    char domain[256];
    char url[768];
    char scheme[8];
    int port;
} FirewallRequest;

typedef struct {
    FirewallDecision decision;
    RiskLevel risk;
    char service[32];
    char reason[256];
    ParsedUrl parsed_url;
} FirewallResult;

void engine_init_request(FirewallRequest *request);
void engine_evaluate(const FirewallRequest *request, FirewallResult *result);
const char *decision_to_string(FirewallDecision decision);
const char *risk_to_string(RiskLevel risk);

#endif
