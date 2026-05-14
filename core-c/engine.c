#include "engine.h"

#include <stdio.h>
#include <string.h>

#include "domain_rules.h"
#include "ip_rules.h"
#include "port_check.h"
#include "url_parser.h"

void engine_init_request(FirewallRequest *request)
{
    if (request == NULL) {
        return;
    }

    memset(request, 0, sizeof(*request));
    request->port = 0;
}

const char *decision_to_string(FirewallDecision decision)
{
    switch (decision) {
    case FIREWALL_ALLOW:
        return "ALLOW";
    case FIREWALL_BLOCK:
        return "BLOCK";
    default:
        return "UNKNOWN";
    }
}

const char *risk_to_string(RiskLevel risk)
{
    switch (risk) {
    case RISK_HIGH:
        return "HIGH";
    case RISK_MEDIUM:
        return "MEDIUM";
    default:
        return "LOW";
    }
}

static RiskLevel merge_risk(RiskLevel current, RiskLevel next)
{
    return next > current ? next : current;
}

static void block(FirewallResult *result, RiskLevel risk, const char *reason)
{
    result->decision = FIREWALL_BLOCK;
    result->risk = merge_risk(result->risk, risk);
    snprintf(result->reason, sizeof(result->reason), "%s", reason);
}

void engine_evaluate(const FirewallRequest *request, FirewallResult *result)
{
    const char *domain_to_check = NULL;
    PortRisk port_risk = PORT_RISK_LOW;

    if (request == NULL || result == NULL) {
        return;
    }

    memset(result, 0, sizeof(*result));
    result->decision = FIREWALL_ALLOW;
    result->risk = RISK_LOW;
    snprintf(result->service, sizeof(result->service), "UNKNOWN");
    snprintf(result->reason, sizeof(result->reason), "Kural eslesmedi, varsayilan izin");

    if (request->url[0] != '\0') {
        if (!parse_url(request->url, &result->parsed_url)) {
            block(result, RISK_MEDIUM, "URL formati gecersiz");
            return;
        }

        domain_to_check = result->parsed_url.domain;
    } else if (request->domain[0] != '\0') {
        domain_to_check = request->domain;
    }

    if (request->port > 0) {
        if (!validate_port(request->port)) {
            block(result, RISK_MEDIUM, "Port gecersiz");
            return;
        }

        snprintf(result->service, sizeof(result->service), "%s", check_port_service(request->port));
        port_risk = check_port_risk(request->port);
        result->risk = merge_risk(result->risk, (RiskLevel)port_risk);
    }

    if (request->ip[0] != '\0') {
        if (!validate_ipv4(request->ip)) {
            block(result, RISK_MEDIUM, "IPv4 formati gecersiz");
            return;
        }

        if (ip_is_whitelisted(request->ip)) {
            result->decision = FIREWALL_ALLOW;
            result->risk = RISK_LOW;
            snprintf(result->reason, sizeof(result->reason), "IP whitelist icinde");
            return;
        }

        if (ip_is_blacklisted(request->ip)) {
            block(result, RISK_HIGH, "IP blacklist icinde");
            return;
        }
    }

    if (domain_to_check != NULL && domain_to_check[0] != '\0') {
        if (domain_is_whitelisted(domain_to_check)) {
            result->decision = FIREWALL_ALLOW;
            result->risk = RISK_LOW;
            snprintf(result->reason, sizeof(result->reason), "Domain whitelist icinde");
            return;
        }

        if (domain_is_blacklisted(domain_to_check)) {
            block(result, RISK_HIGH, "Domain blacklist icinde");
            return;
        }
    }

    if (request->url[0] != '\0' && url_is_blacklisted(request->url)) {
        block(result, RISK_HIGH, "URL blacklist icinde");
        return;
    }

    if (request->port > 0) {
        if (port_is_allowed(request->port)) {
            result->decision = FIREWALL_ALLOW;
            snprintf(result->reason, sizeof(result->reason), "Port allow list icinde");
            return;
        }

        if (port_is_blocked(request->port)) {
            block(result, RISK_HIGH, "Port block list icinde");
            return;
        }
    }
}
