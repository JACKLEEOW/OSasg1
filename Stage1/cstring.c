#include <stddef.h>

#include "cstring.h"

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
    if (s == NULL)
        return 0;

    int left = 0;
    int right = c_strlen(s) - 1;

    // Move to first non-whitespace character left to right
    while (s[left] && c_isspace((unsigned char)s[left])) {
        left++;
    }

    // Move to first non-whitespace character right to left
    while (right >= left && c_isspace((unsigned char)s[right])) {
        right--;
    }

    // handle case where entire string is whitespace
    if (left > right){
        s[0] = '\0';
        return 0;
    }

    // Shift all elements, removeing whitespace
    int i;
    for (i = left; i <= right; i++) {
        s[i - left] = s[i];
    }

    size_t length = (right - left + 1);
    s[length] = '\0'; // re-add null terminator at end
    return (length);
}

int c_strcmp(const char * s1, const char * s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char * c_strcpy(char * dest, const char * src) {
    if (src == NULL) return dest;

    int i = 0;
    do {
        dest[i] = src[i];
    } while (src[i++] != '\0');
    return dest;
}

char * c_strncpy(char * dest, const char * src, size_t n) {
    if (src == NULL) return dest;

    int i = 0;
    do {
        if (i==n) break;
        dest[i] = src[i];
    } while (src[i++] != '\0' || i);
    return dest;
}

char * c_strcat(char * dest, const char * src) {
    char *ptr = dest;

    while (*ptr != '\0') {
        ptr++;
    }

    while (*src != '\0') {
        *ptr = *src;
        ptr++;
        src++;
    }

    *ptr = '\0';

    return src;
}