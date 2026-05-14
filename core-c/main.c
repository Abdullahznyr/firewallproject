#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "logger.h"
#include "nftables_adapter.h"

static void usage(const char *program)
{
    printf("Kullanim:\n");
    printf("  %s --test --ip 1.2.3.4 --url https://example.com/a --port 443\n", program);
    printf("  %s --apply --dry-run\n", program);
    printf("  %s --apply\n", program);
    printf("  %s --list\n", program);
    printf("  %s --rollback backup.nft\n", program);
    printf("  %s --emergency-reset --dry-run\n", program);
}

static int parse_request_args(int argc, char **argv, FirewallRequest *request)
{
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            snprintf(request->ip, sizeof(request->ip), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--domain") == 0 && i + 1 < argc) {
            snprintf(request->domain, sizeof(request->domain), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--url") == 0 && i + 1 < argc) {
            snprintf(request->url, sizeof(request->url), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--scheme") == 0 && i + 1 < argc) {
            snprintf(request->scheme, sizeof(request->scheme), "%s", argv[++i]);
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            request->port = atoi(argv[++i]);
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    FirewallRequest request;
    FirewallResult result;
    int dry_run = 0;
    int i = 0;

    if (argc == 1) {
        usage(argv[0]);
        return 0;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--dry-run") == 0) {
            dry_run = 1;
        }
    }

    if (strcmp(argv[1], "--apply") == 0) {
        return nftables_apply_rules(dry_run);
    }

    if (strcmp(argv[1], "--list") == 0) {
        return nftables_list_rules();
    }

    if (strcmp(argv[1], "--rollback") == 0 && argc >= 3) {
        return nftables_rollback(argv[2], dry_run);
    }

    if (strcmp(argv[1], "--emergency-reset") == 0) {
        return nftables_emergency_reset(dry_run);
    }

    if (strcmp(argv[1], "--test") == 0) {
        engine_init_request(&request);
        parse_request_args(argc, argv, &request);
        engine_evaluate(&request, &result);

        printf(
            "DECISION=%s RISK=%s SERVICE=%s REASON=%s\n",
            decision_to_string(result.decision),
            risk_to_string(result.risk),
            result.service,
            result.reason
        );

        log_event(
            decision_to_string(result.decision),
            "manual-test",
            request.ip,
            request.domain[0] != '\0' ? request.domain : result.parsed_url.domain,
            request.url,
            request.port,
            result.service,
            result.reason
        );
        return result.decision == FIREWALL_BLOCK ? 2 : 0;
    }

    usage(argv[0]);
    return 0;
}
