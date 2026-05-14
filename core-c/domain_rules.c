#include "domain_rules.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

static void normalize_domain(const char *input, char *output, size_t output_size)
{
    if (output_size == 0) {
        return;
    }

    output[0] = '\0';
    if (input == NULL) {
        return;
    }

    snprintf(output, output_size, "%s", input);
    trim_newline(output);
    trim_spaces(output);
    to_lowercase(output);

    if (strncmp(output, "www.", 4) == 0) {
        memmove(output, output + 4, strlen(output + 4) + 1);
    }
}

static int domain_matches_rule(const char *domain, const char *rule)
{
    size_t domain_len = strlen(domain);
    size_t rule_len = strlen(rule);

    if (strcmp(domain, rule) == 0) {
        return 1;
    }

    if (domain_len > rule_len && strcmp(domain + domain_len - rule_len, rule) == 0) {
        return domain[domain_len - rule_len - 1] == '.';
    }

    return 0;
}

static int domain_in_file(const char *filename, const char *domain)
{
    char normalized_domain[256];
    char normalized_rule[256];
    char path[256];
    char line[512];
    FILE *file = NULL;

    normalize_domain(domain, normalized_domain, sizeof(normalized_domain));
    if (!validate_domain_name(normalized_domain)) {
        return 0;
    }

    open_data_file_path(filename, path, sizeof(path));
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

        normalize_domain(line, normalized_rule, sizeof(normalized_rule));
        if (domain_matches_rule(normalized_domain, normalized_rule)) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int validate_domain_name(const char *domain)
{
    size_t i = 0;
    int has_dot = 0;

    if (domain == NULL || domain[0] == '\0' || strlen(domain) > 253) {
        return 0;
    }

    for (i = 0; domain[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)domain[i];
        if (ch == '.') {
            has_dot = 1;
            continue;
        }

        if (!(isalnum(ch) || ch == '-')) {
            return 0;
        }
    }

    return has_dot;
}

int domain_is_whitelisted(const char *domain)
{
    return domain_in_file("domain_whitelist.txt", domain);
}

int domain_is_blacklisted(const char *domain)
{
    return domain_in_file("domain_blacklist.txt", domain);
}
