#ifndef TOKENS_H
#define TOKENS_H

#include "const.h"
#include <stdbool.h>

typedef struct {
    char token_strings[MAX_INPUT_SIZE + 1];
    char * token_string_ptrs[MAX_INPUT_SIZE + 1];
    int num_tokens;
} Tokens;


// typedef struct {
//     char token_strings[MAX_INPUT_SIZE + 1];
//     Token tokens[MAX_INPUT_SIZE + 1];
//     int num_tokens;
// } Tokens;


typedef enum {
    
    TOKEN_PIPE,
    TOKEN_OR,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_BACKGROUND,
    TOKEN_TEST,
    _NUMB_SPECIAL_TOKENS,

    TOKEN_WORD,

    // A special token types used for the special token nfa's
    TOKEN_INPROGRESS,
    TOKEN_INVALID 
} TokenType;

typedef struct {
    char * text;
    TokenType type;
} Token;

extern const char * SPECIAL_TOKENS[_NUMB_SPECIAL_TOKENS];

extern const int SPECIAL_TOKENS_SIZE[_NUMB_SPECIAL_TOKENS];

void create_tokens(Tokens * t, char * s);
char * index_tokens(Tokens * t, int index);
int num_tokens(Tokens * t);
void test_tokens(Tokens * t);

/**
 * @brief sets the global candidates
 * @param c the character to be qualified
 * @return number of qualifying candidates
 */
int set_special_candidates(const char c);

/**
 * @brief reads from the global candidates, compares 
 * @param starting the location at which a token could appear
 */
void validate_special_candidates(const char * starting, TokenType * return_type, int * return_length);

// /**
//  * 
//  */
int is_special_candidate(const char c);

#endif