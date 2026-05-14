#ifndef FIREWALLPROJECT_UTILS_H
#define FIREWALLPROJECT_UTILS_H

#include <stddef.h>

void trim_newline(char *text);
void trim_spaces(char *text);
void to_lowercase(char *text);
int is_empty_or_comment(const char *text);
int open_data_file_path(const char *filename, char *buffer, size_t buffer_size);
int file_contains_line(const char *filename, const char *value);
int append_unique_line(const char *filename, const char *value);

#endif
