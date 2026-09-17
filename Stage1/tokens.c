#include "tokens.h"
#include "const.h"
#include "cstring.h"
#include <stdio.h>

// char tokens[MAX_INPUT_SIZE + 1] = {"\0"};
// char * tokenPointers[MAX_INPUT_SIZE + 1] = {NULL};
// int tokenFill = 0;


// char * t
// chat ** t
//printf("set Pointer (%d)\n", i);
//printf("%d, %d ->E:\n", s[i], c_isspace((unsigned char)s[i]));

Tokens createTokens(char * s) {

    Tokens t = {
        .tokenStrings = {'\0'},
        .tokenStringPtrs = {NULL},
        .numTokens = 0
    };

    if (s[0] == '\0' || s == NULL) {
        return t;
    }

    int i = 0;
    while (1) {

        // move to the right until non-ws character
        while(c_isspace((unsigned char)s[i])) {t.tokenStrings[i] = s[i]; i++;}
        if (s[i] == '\0') break;

        
        // set pointer to start of string
        t.tokenStringPtrs[t.numTokens] = t.tokenStrings + i;
        t.numTokens ++;

        // move to the right until non-ws
        while(!c_isspace((unsigned char)s[i]) && s[i] != '\0') {t.tokenStrings[i] = s[i]; i++;}
        if (s[i] == '\0') break; // if character was null terminator, end.
        t.tokenStrings[i] = '\0'; // otherwise, add a \0 seperator
        i++;
    }
    return t;
}

char * indexTokens(Tokens * t, int i) {
    if (i < 0 || i >= t->numTokens) return NULL;
    return t->tokenStringPtrs[i];
}

void testTokens(Tokens * t) {
    printf("Number of Tokens: %d\n", t->numTokens);
    int i;
    for (i = 0; i < t->numTokens; i++) {
        printf("Token: %s\n", indexTokens(t, i));
    }
}
