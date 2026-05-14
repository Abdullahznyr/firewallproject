#include "nftables_adapter.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"

static const char *nft_binary(void)
{
    if (access("/usr/sbin/nft", X_OK) == 0) {
        return "/usr/sbin/nft";
    }

    if (access("/sbin/nft", X_OK) == 0) {
        return "/sbin/nft";
    }

    if (access("/usr/bin/nft", X_OK) == 0) {
        return "/usr/bin/nft";
    }

    return "nft";
}

static int run_command(char *const argv[], int dry_run, int allow_failure)
{
    pid_t pid;
    int status = 0;
    int i = 0;

    if (dry_run) {
        printf("DRY-RUN:");
        for (i = 0; argv[i] != NULL; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n");
        return 0;
    }

    pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    }

    if (waitpid(pid, &status, 0) < 0) {
        return 1;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return 0;
    }

    return allow_failure ? 0 : 1;
}

static int valid_ipv4_line(const char *line)
{
    struct in_addr parsed;

    if (line == NULL || line[0] == '\0') {
        return 0;
    }

    return inet_pton(AF_INET, line, &parsed) == 1;
}

static int apply_ip_blacklist_rules(int dry_run)
{
    char path[256];
    char line[128];
    FILE *file = NULL;
    int rc = 0;

    open_data_file_path("ip_blacklist.txt", path, sizeof(path));
    file = fopen(path, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        trim_newline(line);
        trim_spaces(line);

        if (is_empty_or_comment(line) || !valid_ipv4_line(line)) {
            continue;
        }

        /*
         * input: cihazın firewall makinesine erişimini keser.
         * forward: firewall makinesi gateway ise cihazın internet trafiğini keser.
         * output: firewall makinesinin bu cihaza trafik göndermesini keser.
         */
        char *drop_input[] = {
            (char *)nft_binary(), "add", "rule", "inet", "firewallproject", "input",
            "ip", "saddr", line, "drop", NULL
        };
        char *drop_forward[] = {
            (char *)nft_binary(), "add", "rule", "inet", "firewallproject", "forward",
            "ip", "saddr", line, "drop", NULL
        };
        char *drop_output[] = {
            (char *)nft_binary(), "add", "rule", "inet", "firewallproject", "output",
            "ip", "daddr", line, "drop", NULL
        };

        if (run_command(drop_input, dry_run, 0) != 0) {
            rc = 1;
        }
        if (run_command(drop_forward, dry_run, 0) != 0) {
            rc = 1;
        }
        if (run_command(drop_output, dry_run, 0) != 0) {
            rc = 1;
        }
    }

    fclose(file);
    return rc;
}

static int apply_blocked_port_rules(int dry_run)
{
    char path[256];
    char line[64];
    FILE *file = NULL;
    int rc = 0;

    open_data_file_path("blocked_ports.txt", path, sizeof(path));
    file = fopen(path, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        trim_newline(line);
        trim_spaces(line);

        if (is_empty_or_comment(line)) {
            continue;
        }

        char *drop_tcp[] = {
            (char *)nft_binary(), "add", "rule", "inet", "firewallproject", "forward",
            "tcp", "dport", line, "drop", NULL
        };
        char *drop_udp[] = {
            (char *)nft_binary(), "add", "rule", "inet", "firewallproject", "forward",
            "udp", "dport", line, "drop", NULL
        };

        if (run_command(drop_tcp, dry_run, 0) != 0) {
            rc = 1;
        }
        if (run_command(drop_udp, dry_run, 0) != 0) {
            rc = 1;
        }
    }

    fclose(file);
    return rc;
}

int nftables_remove_rules(int dry_run)
{
    char *delete_table[] = {(char *)nft_binary(), "delete", "table", "inet", "firewallproject", NULL};
    int rc = run_command(delete_table, dry_run, 1);
    audit_event("core-c", "NFT_DELETE_TABLE", dry_run ? "dry-run" : "live");
    return rc;
}

int nftables_apply_rules(int dry_run)
{
    /*
     * Atomic yaklaşım:
     * 1. Eski tabloyu kaldırmayı dener.
     * 2. Yeni tablo ve base chain'leri oluşturur.
     * 3. Aktif text rule dosyalarından gerçek nft drop kurallarını üretir.
     */
    char *add_table[] = {(char *)nft_binary(), "add", "table", "inet", "firewallproject", NULL};
    char *add_input_chain[] = {
        (char *)nft_binary(), "add", "chain", "inet", "firewallproject", "input",
        "{", "type", "filter", "hook", "input", "priority", "0", ";",
        "policy", "accept", ";", "}", NULL
    };
    char *add_forward_chain[] = {
        (char *)nft_binary(), "add", "chain", "inet", "firewallproject", "forward",
        "{", "type", "filter", "hook", "forward", "priority", "0", ";",
        "policy", "accept", ";", "}", NULL
    };
    char *add_output_chain[] = {
        (char *)nft_binary(), "add", "chain", "inet", "firewallproject", "output",
        "{", "type", "filter", "hook", "output", "priority", "0", ";",
        "policy", "accept", ";", "}", NULL
    };

    if (nftables_remove_rules(dry_run) != 0) {
        return 1;
    }

    if (run_command(add_table, dry_run, 0) != 0) {
        return 1;
    }

    if (run_command(add_input_chain, dry_run, 0) != 0) {
        return 1;
    }

    if (run_command(add_forward_chain, dry_run, 0) != 0) {
        return 1;
    }

    if (run_command(add_output_chain, dry_run, 0) != 0) {
        return 1;
    }

    if (apply_ip_blacklist_rules(dry_run) != 0) {
        return 1;
    }

    if (apply_blocked_port_rules(dry_run) != 0) {
        return 1;
    }

    audit_event("core-c", "NFT_APPLY_BASE_RULES", dry_run ? "dry-run" : "live");
    return 0;
}

int nftables_list_rules(void)
{
    char *list_rules[] = {(char *)nft_binary(), "list", "ruleset", NULL};
    return run_command(list_rules, 0, 0);
}

int nftables_backup(const char *backup_path)
{
    char command[512];

    /*
     * Basitlik için backup shell script tarafında daha güçlü yapılır.
     * C tarafında kullanıcıya hangi komutun çalıştırılacağını gösteriyoruz.
     */
    snprintf(command, sizeof(command), "%s list ruleset > %s", nft_binary(), backup_path);
    printf("Backup komutu: %s\n", command);
    audit_event("core-c", "NFT_BACKUP_HINT", command);
    return 0;
}

int nftables_rollback(const char *backup_path, int dry_run)
{
    char *flush_ruleset[] = {(char *)nft_binary(), "flush", "ruleset", NULL};
    char *restore_ruleset[] = {(char *)nft_binary(), "-f", (char *)backup_path, NULL};

    if (backup_path == NULL || backup_path[0] == '\0') {
        return 1;
    }

    if (run_command(flush_ruleset, dry_run, 0) != 0) {
        return 1;
    }

    if (run_command(restore_ruleset, dry_run, 0) != 0) {
        return 1;
    }

    audit_event("core-c", "NFT_ROLLBACK", backup_path);
    return 0;
}

int nftables_emergency_reset(int dry_run)
{
    char *flush_ruleset[] = {(char *)nft_binary(), "flush", "ruleset", NULL};
    int rc = run_command(flush_ruleset, dry_run, 0);
    audit_event("core-c", "NFT_EMERGENCY_RESET", dry_run ? "dry-run" : "live");
    return rc;
}
