#ifndef TOKENS_H
#define TOKENS_H

#include "const.h"

typedef enum {
    // Error type
    TOKEN_ERROR = -1,

    // Special type
    TOKEN_PIPE = 0,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_OUT_APPEND,
    TOKEN_BACKGROUND,
    
    // Counter for special types
    NUMB_SPECIAL_TOKENS,

    // Generic type
    TOKEN_WORD,
} TokenType;

typedef struct {
    const char * text;
    TokenType type;
} Token;

typedef struct {
    char cstring_storage[MAX_INPUT_SIZE + 1]; // DO NOT MODIFY OUTSIDE
    Token token_array[MAX_INPUT_SIZE + 1];
    int num_tokens;
} Tokens;

extern const char * SPECIAL_TOKENS[NUMB_SPECIAL_TOKENS];

extern const int SPECIAL_TOKENS_SIZE[NUMB_SPECIAL_TOKENS];

Token * index_tokens(Tokens * t, int index);

/**
 * @brief retrieves the token count of tokens
 * @param t a pointer to a tokens struct
 * @return the count of tokens
 */
int num_tokens(Tokens * t);

void create_tokens(Tokens * tokens, const char * input_buffer);

void test_tokens(Tokens * t);

/**
 * @brief sets the global candidates
 * @param c the character to be qualified
 * @return number of qualifying candidates
 */
int set_special_candidates(const char c);

/**
 * @brief used in conjuction with set_special_candidates.\n
 * reads from the global candidates, determines wether or not that from a pointer to the character's starting position, it is a valid special tokens.
 * @param starting a pointer from a cstring in which the character appeared
 * @param return_type the returning type of the token, TOKEN_INVALID if invalid.
 * @param return_length the length of the token found, 1 if not found
 */
void validate_special_candidates(const char * starting, TokenType * return_type, int * return_length);


#endif