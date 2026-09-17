#ifndef TOKENS_H
#define TOKENS_H

#include "const.h"
typedef char ** tokenptrs_t;

typedef struct {
    char tokenStrings[MAX_INPUT_SIZE + 1];
    char * tokenStringPtrs[MAX_INPUT_SIZE + 1];
    int numTokens;
} Tokens;

enum TokenType{
    WORD,
    AND
};


Tokens createTokens(char * s);
char * indexTokens(Tokens * t, int index);
void testTokens(Tokens * t);

#endif