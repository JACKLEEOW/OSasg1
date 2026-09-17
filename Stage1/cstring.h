#ifndef CSTRING_H
#define CSTRING_H
#include <stddef.h>

size_t c_strlen(const char * s);
size_t c_trim(char * s);
int c_isspace(int c);
int c_strcmp(const char * s1, const char * s2);
void print_hidden_string(const char *str);

#endif