#ifndef TOKENS_H
#define TOKENS_H

#include "const.h"

typedef struct {
    char token_strings[MAX_INPUT_SIZE + 1];
    char * token_string_ptrs[MAX_INPUT_SIZE + 1];
    int num_tokens;
} Tokens;

/*
typedef struct {
    char tokenStrings[MAX_INPUT_SIZE + 1];
    char * Token[MAX_INPUT_SIZE + 1];
    int numTokens;
} Tokens;
*/


typedef enum {
    TOKEN_WORD,
    TOKEN_AND
} TokenType;

typedef struct {
    char * text;
    TokenType type;
} Token;


Tokens create_tokens(char * s);
char * index_tokens(Tokens * t, int index);
int num_tokens(Tokens * t);
void test_tokens(Tokens * t);

#endif