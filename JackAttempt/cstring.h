#ifndef CSTRING_H
#define CSTRING_H

#include <stddef.h>

/**
 * retrieve the length of the cstring excluding null terminator
 * @param s A pointer to a cstyle string
 * @return length of cstring
 */
size_t c_strlen(const char * s);

/**
 * removes all whitespace characters in-place
 * @param s A pointer to a c style string
 * @return length of modified cstring excluding null terminator
 */
size_t c_trim(char * s);

/**
 * evaluates if the character is a whitespace character ('\t' '\\n' '\r' '\v' '\f')
 * @param c the character to be evaluated 
 * @note c must either be an unsigned char or equal to EOF (-1)
 * @return number of bytes of modified cstring
 */
int c_isspace(int c);

/**
 * compares two cstrings
 * @param s1 cstring 1
 * @param s2 cstring 2
 * @return >0 if cstring 1 is larger, <0 if cstring 2 is larger, 0 if equal
 */
int c_strcmp(const char * s1, const char * s2);

void print_hidden_string(const char *str);

#endif