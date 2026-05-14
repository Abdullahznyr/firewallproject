#include "url_parser.h"

#include <stdio.h>
#include <string.h>

#include "domain_rules.h"
#include "utils.h"

int parse_url(const char *url, ParsedUrl *parsed)
{
    const char *cursor = url;
    const char *scheme_end = NULL;
    const char *path_start = NULL;
    size_t domain_len = 0;

    if (url == NULL || parsed == NULL || url[0] == '\0') {
        return 0;
    }

    memset(parsed, 0, sizeof(*parsed));

    scheme_end = strstr(cursor, "://");
    if (scheme_end != NULL) {
        size_t scheme_len = (size_t)(scheme_end - cursor);
        if (scheme_len >= sizeof(parsed->scheme)) {
            return 0;
        }
        snprintf(parsed->scheme, sizeof(parsed->scheme), "%.*s", (int)scheme_len, cursor);
        cursor = scheme_end + 3;
    } else {
        snprintf(parsed->scheme, sizeof(parsed->scheme), "http");
    }

    to_lowercase(parsed->scheme);
    if (strcmp(parsed->scheme, "http") != 0 && strcmp(parsed->scheme, "https") != 0) {
        return 0;
    }

    path_start = strchr(cursor, '/');
    if (path_start == NULL) {
        domain_len = strlen(cursor);
        snprintf(parsed->path, sizeof(parsed->path), "/");
    } else {
        domain_len = (size_t)(path_start - cursor);
        snprintf(parsed->path, sizeof(parsed->path), "%s", path_start);
    }

    if (domain_len == 0 || domain_len >= sizeof(parsed->domain)) {
        return 0;
    }

    snprintf(parsed->domain, sizeof(parsed->domain), "%.*s", (int)domain_len, cursor);
    to_lowercase(parsed->domain);

    if (strncmp(parsed->domain, "www.", 4) == 0) {
        memmove(parsed->domain, parsed->domain + 4, strlen(parsed->domain + 4) + 1);
    }

    return validate_domain_name(parsed->domain);
}

int url_is_blacklisted(const char *url)
{
    char normalized_url[1024];
    char without_www[1024];
    ParsedUrl parsed;

    if (!parse_url(url, &parsed)) {
        return 0;
    }

    snprintf(normalized_url, sizeof(normalized_url), "%s://%s%s", parsed.scheme, parsed.domain, parsed.path);
    if (file_contains_line("url_blacklist.txt", normalized_url)) {
        return 1;
    }

    snprintf(without_www, sizeof(without_www), "%s%s", parsed.domain, parsed.path);
    return file_contains_line("url_blacklist.txt", without_www);
}
