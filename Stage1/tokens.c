#include "tokens.h"
#include "const.h"
#include "cstring.h"
#include <stdio.h>

//printf("set Pointer (%d)\n", i);
//printf("%d, %d ->E:\n", s[i], c_isspace((unsigned char)s[i]));
int is_quote(char c);

Tokens create_tokens(char * s) {

    // Should be done with malloc
    Tokens t = {
        .token_strings = {'\0'},
        .token_string_ptrs = {NULL},
        .num_tokens = 0
    };

    if (s[0] == '\0' || s == NULL) {
        return t;
    }

    int i = 0; // iterator for input string
    int j = 0; // iterator for copied string
    char curr_quote = '\0';
    while (1) {

        // move to the right until non-ws character
        while(c_isspace((unsigned char)s[i])) {t.token_strings[j] = s[i]; i++; j++;}
        if (s[i] == '\0') break;
        if (is_quote(s[i])) {curr_quote = s[i]; i++;} // skip pass quote

        // set pointer to start of string
        t.token_string_ptrs[t.num_tokens] = t.token_strings + j;
        t.num_tokens ++;

        
        // quote mode
        if (curr_quote) {
            QuoteMode:
            // move to the right until closing quote character (read anything)
            while(s[i] != curr_quote && s[i] != '\0') {t.token_strings[j] = s[i]; i++; j++;}
            if (s[i] == '\0') {t.token_strings[j] = '\0'; break;} // maybe set a flag
            curr_quote = '\0'; // reset quote
            i++;
        }
            
        // move to the right until ws character, quote, or operator
        while(!c_isspace((unsigned char)s[i]) && s[i] != '\0' && !is_quote(s[i])) {t.token_strings[j] = s[i]; i++; j++;}
        if (s[i] == '\0') {t.token_strings[j] = '\0'; break;} // if character was null terminator, end

        // found in between (operators)
        if (is_quote(s[i])) {curr_quote = s[i]; i++; goto QuoteMode;} // if quote found, skip pass string pointer, l"s" -> ls, cstring pointer to l from previous

        t.token_strings[j] = '\0'; // otherwise, add a \0 separator
        i++;
        j++;
    }
    return t;
}

char * index_tokens(Tokens * t, int i) {
    if (i < 0 || i >= t->num_tokens) return NULL;
    return t->token_string_ptrs[i];
}

int num_tokens(Tokens * t) {
    return t->num_tokens;
}

void test_tokens(Tokens * t) {
    printf("Number of Tokens: %d\n", t->num_tokens);
    int i;
    for (i = 0; i < num_tokens(t); i++) {
        char * s = index_tokens(t, i);
        printf("Token: %s (%ld)\n", s, c_strlen(s));
    }
}

int is_quote(char c) {
    return (c == '\'' || c =='"');
}
