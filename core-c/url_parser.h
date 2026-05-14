#ifndef FIREWALLPROJECT_URL_PARSER_H
#define FIREWALLPROJECT_URL_PARSER_H

#include <stddef.h>

typedef struct {
    char scheme[8];
    char domain[256];
    char path[512];
} ParsedUrl;

int parse_url(const char *url, ParsedUrl *parsed);
int url_is_blacklisted(const char *url);

#endif
