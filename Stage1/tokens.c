#include "tokens.h"
#include "const.h"
#include "cstring.h"
#include <stddef.h>
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                      CONSTANTS, ENUMS, HELPER HEADERS                      */
/* -------------------------------------------------------------------------- */

#define PEEK        (tz->input[tz->r])
#define ADVANCE     (tz->r++)
#define CONSUME     (tz->input[tz->r++])
#define WRITE(c)    (tz->output[tz->w++] = (c))

typedef enum {
    STATE_START_TOKEN,
    STATE_QUOTE_WRITE_TOKEN,
    STATE_NORMAL_WRITE_TOKEN,
    STATE_SPECIAL_TOKEN,
    STATE_EXIT,
} State;

typedef struct {
    int r;              // read head
    int w;              // write head
    const char * input; // read from
    char * output;      // write to
    Tokens * tokens;    // token container
    char quote;          
    int at_start;       // used to for a specific condition found in special token
} Tokenizer;

/**
 * @brief finds the first none whitespace character, init the next token
 */
static State handle_start_token(Tokenizer * tz);

/**
 * @brief writes all following non-whitespace, non-terminating, non-special characters, handles closing the current token
 */
static State handle_normal_write_token(Tokenizer * tz);

/**
 * @brief writes all following character up to the next matching quote
 */
static State handle_quote_write_token(Tokenizer * tz);

/**
 * @brief attempts to init a new token based on the tokens defined in special_tokens.c,
 * either fills the token out with the special token, or just write the current char as 
 * it wasn't a token
 */
static State handle_special_token(Tokenizer * tz);

static void write_token(Token * token, TokenType type, const char * text);

static inline int is_quote(const char c) { return (c == '\'' || c =='"'); }
static inline int is_digit(const char c) { return (c >= '0' && c <='9'); }
static inline int is_space(const char c) { return c_isspace((unsigned char) c); }
static inline int is_nullc(const char c) { return c == '\0'; }
static inline int is_special_candidate(const char c) { 
    for (int i = 0; i < NUMB_SPECIAL_TOKENS; i++) { 
        if (c == SPECIAL_TOKENS[i][0]) return 1; 
    } 
    return 0; 
}

/* -------------------------------------------------------------------------- */
/*                            FUNCTION DEFINITIONS                            */
/* -------------------------------------------------------------------------- */

Token * index_tokens(Tokens * t, int i) {
    if (i < 0 || i >= t->num_tokens) return NULL;
    return &(t->token_array[i]);
}

int num_tokens(Tokens * t) {
    return t->num_tokens;
}

void create_tokens(Tokens * tokens, const char * input_buffer) {
    
    // Init
    tokens->num_tokens = 0;
    if (input_buffer == NULL || input_buffer[0] == '\0') return;

    Tokenizer tz = {
        .r = 0,
        .w = 0,
        .input = input_buffer,
        .output = tokens->cstring_storage,
        .tokens = tokens,
        .quote = '\0',
        .at_start = 0
    };

    State state = STATE_START_TOKEN;
    while (state != STATE_EXIT) {

        switch (state) {
        case STATE_START_TOKEN:
            state = handle_start_token(&tz);
            break;
        case STATE_QUOTE_WRITE_TOKEN:
            state = handle_quote_write_token(&tz);
            break;
        case STATE_NORMAL_WRITE_TOKEN:
            state = handle_normal_write_token(&tz);
            break;
        case STATE_SPECIAL_TOKEN:
            state = handle_special_token(&tz);
            break;
        case STATE_EXIT:
            break;
        }
    }
    return;
}

void test_tokens(Tokens * t) {
    printf("------TOKEN TEST------\n");
    printf("Number of Tokens: %d\n", num_tokens(t));
    int i;
    for (i = 0; i < num_tokens(t); i++) {
        Token * token = index_tokens(t, i);
        printf("Token: %s (%ld), type: %d\n", token->text, c_strlen(token->text), (int) token->type);
    }
    printf("----------------------\n");
}

/* -------------------------------------------------------------------------- */
/*                         HELPER FUNCTION DEFINITIONS                        */
/* -------------------------------------------------------------------------- */

static State handle_start_token(Tokenizer * tz) {
    Tokens * tokens = tz->tokens;

    // move to the right until non-ws character (Beginning of the token)
    while (is_space(PEEK)) ADVANCE;
    if (is_nullc(PEEK)) return STATE_EXIT;

    // init the next available token
    Token * token = &(tokens->token_array[tokens->num_tokens]);
    const char * str_location = tz->output + (tz->w); // set the next available string location
    write_token(token, TOKEN_WORD, str_location);

    if (is_quote(PEEK)) {
        tz->quote = CONSUME; // Ignore quote
        return STATE_QUOTE_WRITE_TOKEN;
        
    } else if (set_special_candidates(PEEK)) {
        tz->at_start = 1;
        return STATE_SPECIAL_TOKEN;
    }

    return STATE_NORMAL_WRITE_TOKEN;
}

static State handle_normal_write_token(Tokenizer * tz) {
    Tokens * tokens = tz->tokens;


    // move to the right until ws character, quote, or operator
    while ( !is_space(PEEK) && 
            !is_nullc(PEEK) && 
            !is_quote(PEEK) && 
            !is_special_candidate(PEEK)
        ) { WRITE(CONSUME); }

    
    if (is_quote(PEEK)) {
        tz->quote = CONSUME; 
        return STATE_QUOTE_WRITE_TOKEN; // if quote found, skip pass string pointer, l"s" -> ls, cstring pointer to l from previous
    } else if (set_special_candidates(PEEK)) {
        tz->at_start = 0;
        return STATE_SPECIAL_TOKEN;
    }
    
    // End current token's string
    WRITE('\0'); 
    tokens->num_tokens++;

    // Reached the end of the input buffer
    if (is_nullc(PEEK)) return STATE_EXIT;
    return STATE_START_TOKEN;
}

static State handle_quote_write_token(Tokenizer * tz) {
    while (PEEK != tz->quote && !is_nullc(PEEK)) { WRITE(CONSUME); }
    if (is_nullc(PEEK)) {
        WRITE('\0'); 
        tz->tokens->num_tokens++;
        return STATE_EXIT;
    }
    // safe to pass to handle_normal_write_token to deal with \0
    tz->quote = '\0';
    ADVANCE;
    return STATE_NORMAL_WRITE_TOKEN;
}

static State handle_special_token(Tokenizer * tz) {
    Tokens * tokens = tz->tokens;
    TokenType token_type;
    int token_size;
    validate_special_candidates(tz->input + tz->r, &token_type, &token_size); 

    if (token_type != TOKEN_ERROR) {
        
        //special case a|, finish up previous token
        if (!(tz->at_start)) {
            WRITE('\0');
            tokens->num_tokens++;
        }

        tz->r += token_size;
        
        Token * token = &(tokens->token_array[tokens->num_tokens]);
        write_token(token, token_type, SPECIAL_TOKENS[token_type]);
        tokens->num_tokens++;
        return STATE_START_TOKEN;
    }

    // otherwise just write like normal character
    WRITE(CONSUME);
    return STATE_NORMAL_WRITE_TOKEN;
}

static void write_token(Token * token, TokenType type, const char * text) {
    token->type = type;
    token->text = text;
}

#undef PEEK
#undef ADVANCE
#undef CONSUME
#undef WRITE