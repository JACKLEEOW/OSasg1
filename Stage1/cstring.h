#ifndef CSTRING_H
#define CSTRING_H

#include <stddef.h>

/**
 * @brief Returns the length of a C string, excluding the null terminator.
 * @param s Null-terminated string, or NULL
 * @return Length of s, or 0 if s is NULL
 */
size_t c_strlen(const char *s);

/**
 * @brief Removes leading and trailing whitespace from a string in-place.
 *
 * Whitespace in the middle of the string is left untouched.
 * @param s Null-terminated string to modify, or NULL
 * @return Length of the trimmed string, excluding the null terminator
 *          (0 if s is NULL or contains only whitespace)
 */
size_t c_trim(char *s);

/**
 * @brief Checks whether a character is whitespace: ' ', '\t', '\n', '\r', '\v', or '\f'.
 * @param c Character to test
 * @note c must be representable as an unsigned char or equal EOF, as with
 *          the standard isspace()
 * @return Nonzero if c is whitespace, 0 otherwise
 */
int c_isspace(int c);

/**
 * @brief Compares two strings lexicographically, treating characters as unsigned.
 * @param s1 First null-terminated string
 * @param s2 Second null-terminated string
 * @return <0 if s1 < s2, 0 if equal, >0 if s1 > s2
 */
int c_strcmp(const char *s1, const char *s2);

/**
 * @brief Copies src into dest, including the null terminator.
 * @param dest Destination buffer; must be large enough to hold src
 * @param src Null-terminated source string. If NULL, dest is left unchanged
 * @return dest
 * @note The buffers must not overlap
 */
const char *c_strcpy(char *dest, const char *src);

/**
 * @brief Copies at most n characters from src into dest.
 *
 * If src is shorter than n, the rest of dest is padded with '\0'.
 * @param dest Destination buffer; must have room for n characters
 * @param src Null-terminated source string. If NULL, dest is left unchanged
 * @param n Maximum number of characters to write
 * @return dest
 * @note If src has n or more characters, dest is NOT null-terminated
 */
const char *c_strncpy(char *dest, const char *src, size_t n);

/**
 * @brief Appends src to the end of dest, including the null terminator.
 * @param dest Null-terminated string with enough room for the result
 * @param src Null-terminated string to append
 * @return dest
 * @note The buffers must not overlap
 */
const char *c_strcat(char *dest, const char *src);

/**
 * @brief Writes num as a null-terminated decimal string into buffer.
 * @param buffer Destination buffer; must have room for the digits plus the null terminator
 * @param n Size of buffer in bytes, including space for the null terminator
 * @param num Value to convert
 * @return buffer on success, or NULL if buffer is NULL, n is 0, or buffer is too small
 */
const char *uint_to_cstr(char *buffer, size_t n, unsigned int num);


/**
 * @brief Parses a string into an unsigned int.
 *
 * Reads digits until the first non-digit character or the end of the string.
 * No sign, whitespace, or error reporting is supported.
 * @param s Null-terminated source string. If NULL, returns 0
 * @return The parsed value, or 0 if s is NULL or does not start with a digit
 */
unsigned int cstr_to_uint(const char *s);

#endif