#include "cstring.h"
#include <stdio.h>

#include <ctype.h>

void print_hidden_string(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];

        // Explicitly map common hidden whitespace characters
        switch (c) {
            case '\n': printf("\\n"); break;
            case '\t': printf("\\t"); break;
            case '\r': printf("\\r"); break;
            // case '\0': printf("\\0"); break; // In case of mid-buffer exploration
            default:
                if (isprint(c)) {
                    // It's a standard, visible character
                    putchar(c);
                } else {
                    // For any other hidden control characters, print their Hex code
                    printf("\\x%02X", c);
                }
                break;
        }
    }
    printf("\n");
}

size_t c_strlen(const char *s) {
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
        // print_hidden_char(s[start]);
        left++;
    }

    // Move to first non-whitespace character right to left
    while (right >= left && c_isspace((unsigned char)s[right])) {
        // print_hidden_char(s[end]);
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
        // printf("%c", s[i]);
        s[left - i] = s[i];
    }

    size_t lenght = (right - left + 1);
    s[lenght] = '\0'; // re-add null terminator at end
    // printf("left %d, right %d, len(%ld) ->", left, right, lenght);
    // print_hidden_string(s);
    return (lenght);
}

int c_strcmp(const char * s1, const char * s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}