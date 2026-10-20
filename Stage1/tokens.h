#ifndef TOKENS_H
#define TOKENS_H

#include <stddef.h>

#include "const.h"

/* ---------------------------- STRUCTS AND ENUMS --------------------------- */

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

/* --------------------------------- GLOBALS -------------------------------- */

extern const char * SPECIAL_TOKENS[NUMB_SPECIAL_TOKENS];
extern const size_t SPECIAL_TOKENS_SIZE[NUMB_SPECIAL_TOKENS];

/* --------------------------- FUNCTION PROTOTYPES -------------------------- */

// Tokens

Token * index_tokens(Tokens * tokens, int index);

/**
 * @brief Retrieves the token count of tokens
 * @param t A pointer to a tokens struct
 * @return The count of tokens
 */
int num_tokens(Tokens * tokens);

/**
 * @brief Initialize the tokens struct from input_buffer
 * @param tokens Tokens struct
 * @param input_buffer Null-terminated input_buffer
 * @warning input_buffer must be the same length as tokens cstring_storage.
 * 
 */
void create_tokens(Tokens * tokens, const char * input_buffer);


// Special token helpers

/**
 * @brief Sets the global candidates
 * @param c The character to be qualified
 * @return Number of qualifying candidates
 * 
 */
int set_special_candidates(const char c);

/**
 * @brief Used in conjuction with set_special_candidates.
 * reads from the global candidates, determines wether or not that from a pointer to the character's starting position, it is a valid special tokens.
 * @param starting A pointer from a cstring in which the character appeared
 * @param return_type The returning type of the token, TOKEN_INVALID if not found.
 * @param return_length The length of the token string found, 1 if not found
 */
void validate_special_candidates(const char * starting, TokenType * return_type, size_t * return_length);


#endif