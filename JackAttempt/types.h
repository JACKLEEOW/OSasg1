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

void print_hidden_char(const char a) {

    unsigned char c = (unsigned char) a;
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

    printf("\n");
}