#include <stddef.h>

#include "cstring.h"

/* -------------------------- FUNCTION DEFINITIONS -------------------------- */

size_t c_strlen(const char *s) {
    if (s == NULL) return 0;

    size_t i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

int c_isspace(int c) {
    return (c == ' '  ||  // Space
            c == '\t' ||  // Horizontal tab
            c == '\n' ||  // Newline
            c == '\r' ||  // Carriage return
            c == '\v' ||  // Vertical tab
            c == '\f');   // Form feed
}

size_t c_trim(char *s) {
    if (s == NULL) return 0;

    size_t start = 0;
    size_t end = c_strlen(s);

    // Skip leading whitespace
    while (s[start] && c_isspace((unsigned char) s[start])) {
        start++;
    }

    // Skip trailing whitespace
    while (end > start && c_isspace((unsigned char) s[end - 1])) {
        end--;
    }

    // Shift the remaining characters to the front (handles all-whitespace too)
    size_t length = end - start;
    for (size_t i = 0; i < length; i++) {
        s[i] = s[start + i];
    }
    s[length] = '\0';

    return length;
}

int c_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *) s1 - *(const unsigned char *) s2;
}

const char *c_strcpy(char *dest, const char *src) {
    if (src == NULL) return dest;

    size_t i = 0;
    while ((dest[i] = src[i]) != '\0') {
        i++;
    }
    return dest;
}

const char *c_strncpy(char *dest, const char *src, size_t n) {
    if (src == NULL) return dest;

    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

const char *c_strcat(char *dest, const char *src) {
    char *ptr = dest + c_strlen(dest);

    while (*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';

    return dest;
}

const char *uint_to_cstr(char *buffer, size_t n, unsigned int num) {
    if (buffer == NULL || n == 0) return NULL;

    char tmp[20];
    size_t len = 0;

    do {
        tmp[len++] = (char)('0' + (num % 10));
        num /= 10;
    } while (num != 0);

    if (len + 1 > n) {
        buffer[0] = '\0';
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        buffer[i] = tmp[len - 1 - i];
    }
    buffer[len] = '\0';

    return buffer;
}

unsigned int cstr_to_uint(const char *s) {
    unsigned int result = 0;
    while (s && *s >= '0' && *s <= '9') {
        result = result * 10 + (*s++ - '0');
    }
    return result;
}