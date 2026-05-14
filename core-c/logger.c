#include <stdio.h>
#include <time.h>

#include "logger.h"
#include "utils.h"

static void timestamp(char *buffer, size_t buffer_size)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (local == NULL) {
        snprintf(buffer, buffer_size, "unknown-time");
        return;
    }

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", local);
}

static FILE *open_log_file(const char *filename)
{
    char path[256];
    open_data_file_path(filename, path, sizeof(path));
    return fopen(path, "a");
}

void log_event(
    const char *action,
    const char *rule_id,
    const char *ip,
    const char *domain,
    const char *url,
    int port,
    const char *service,
    const char *reason
)
{
    char time_text[32];
    FILE *file = open_log_file("logs.txt");

    if (file == NULL) {
        printf("Log dosyasi acilamadi.\n");
        return;
    }

    timestamp(time_text, sizeof(time_text));
    fprintf(
        file,
        "[%s] ACTION=%s RULE=%s IP=%s DOMAIN=%s URL=%s PORT=%d SERVICE=%s REASON=%s\n",
        time_text,
        action != NULL ? action : "UNKNOWN",
        rule_id != NULL ? rule_id : "-",
        ip != NULL && ip[0] != '\0' ? ip : "-",
        domain != NULL && domain[0] != '\0' ? domain : "-",
        url != NULL && url[0] != '\0' ? url : "-",
        port,
        service != NULL ? service : "UNKNOWN",
        reason != NULL ? reason : "-"
    );

    fclose(file);
}

void audit_event(const char *actor, const char *action, const char *detail)
{
    char time_text[32];
    FILE *file = open_log_file("audit.log");

    if (file == NULL) {
        printf("Audit log dosyasi acilamadi.\n");
        return;
    }

    timestamp(time_text, sizeof(time_text));
    fprintf(
        file,
        "[%s] ACTOR=%s ACTION=%s DETAIL=%s\n",
        time_text,
        actor != NULL ? actor : "system",
        action != NULL ? action : "UNKNOWN",
        detail != NULL ? detail : "-"
    );
    fclose(file);
}
