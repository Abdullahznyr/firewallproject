#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

void trim_newline(char *text)
{
    if (text == NULL) {
        return;
    }

    text[strcspn(text, "\r\n")] = '\0';
}

void trim_spaces(char *text)
{
    char *start = text;
    char *end = NULL;

    if (text == NULL || text[0] == '\0') {
        return;
    }

    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != text) {
        memmove(text, start, strlen(start) + 1);
    }

    end = text + strlen(text);
    while (end > text && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';
}

void to_lowercase(char *text)
{
    size_t i = 0;

    if (text == NULL) {
        return;
    }

    for (i = 0; text[i] != '\0'; i++) {
        text[i] = (char)tolower((unsigned char)text[i]);
    }
}

int is_empty_or_comment(const char *text)
{
    if (text == NULL) {
        return 1;
    }

    while (isspace((unsigned char)*text)) {
        text++;
    }

    return *text == '\0' || *text == '#';
}

int open_data_file_path(const char *filename, char *buffer, size_t buffer_size)
{
    const char *prefixes[] = {
        "../data/",
        "data/",
        "./data/"
    };
    size_t i = 0;
    FILE *file = NULL;

    if (filename == NULL || buffer == NULL || buffer_size == 0) {
        return 0;
    }

    for (i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); i++) {
        snprintf(buffer, buffer_size, "%s%s", prefixes[i], filename);
        file = fopen(buffer, "r");
        if (file != NULL) {
            fclose(file);
            return 1;
        }
    }

    snprintf(buffer, buffer_size, "../data/%s", filename);
    return 0;
}

int file_contains_line(const char *filename, const char *value)
{
    char path[256];
    char line[512];
    FILE *file = NULL;

    if (value == NULL || value[0] == '\0') {
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

        if (strcmp(line, value) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

int append_unique_line(const char *filename, const char *value)
{
    char path[256];
    FILE *file = NULL;

    if (value == NULL || value[0] == '\0') {
        return 0;
    }

    if (file_contains_line(filename, value)) {
        return 1;
    }

    open_data_file_path(filename, path, sizeof(path));
    file = fopen(path, "a");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "%s\n", value);
    fclose(file);
    return 1;
}
