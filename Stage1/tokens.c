#include <stddef.h>

#include "tokens.h"
#include "const.h"
#include "cstring.h"

/* --------------------------------- MACROS --------------------------------- */

#define PEEK        (tz->input[tz->r])
#define ADVANCE     (tz->r++)
#define CONSUME     (tz->input[tz->r++])
#define WRITE(c)    (tz->output[tz->w++] = (c))

/* ----------------- STATE MACHINE ENUM, STRUCT, PROTOTYPES ----------------- */

typedef enum {
    STATE_START_TOKEN,
    STATE_QUOTE_WRITE_TOKEN,
    STATE_NORMAL_WRITE_TOKEN,
    STATE_SPECIAL_TOKEN,
    STATE_EXIT,
} State;

typedef struct {
    size_t r;               // Read index
    size_t w;               // Write index
    const char * input;     // Source string
    char * output;          // Destination buffer
    Tokens * tokens;        // Tokens struct
    char quote;             // Current opening quote char
    int at_start;           // Special token condition flag
} Tokenizer;

/** @brief Skips whitespace and starts a new word token at the write position. */
static State handle_start_token(Tokenizer * tz);

/** @brief Copies an unquoted token, handing off to the quote or special token states when one is hit. */
static State handle_normal_write_token(Tokenizer * tz);

/** @brief Copies a quoted section verbatim until the matching closing quote. */
static State handle_quote_write_token(Tokenizer * tz);

/** @brief Emits a special token if one starts here, otherwise writes the char as normal text. */
static State handle_special_token(Tokenizer * tz);

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

static inline int is_quote(const char c) { 
    return (c == '\'' || c == '"'); 
}

static inline int is_digit(const char c) { 
    return (c >= '0' && c <= '9'); 
}

static inline int is_space(const char c) { 
    return c_isspace((unsigned char) c); 
}

static inline int is_nullc(const char c) { 
    return c == '\0'; 
}

static inline int is_special_candidate(const char c) {
    for (int i = 0; i < NUMB_SPECIAL_TOKENS; i++) if (c == SPECIAL_TOKENS[i][0]) return 1;
    return 0;
}

static void write_token(Token * token, TokenType type, const char * text) {
    token->type = type;
    token->text = text;
}

/* -------------------------- FUNCTION DEFINITIONS -------------------------- */

Token * index_tokens(Tokens * tokens, int index) {
    if (index < 0 || index >= tokens->num_tokens) return NULL;
    return &(tokens->token_array[index]);
}

int num_tokens(Tokens * tokens) {
    return tokens->num_tokens;
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

/* ------------------- STATE MACHINE FUNCTION DEFINITIONS ------------------- */

static State handle_start_token(Tokenizer * tz) {
    Tokens * tokens = tz->tokens;

    // Move to the right until non-ws character (Beginning of the token)
    while (is_space(PEEK)) ADVANCE;
    if (is_nullc(PEEK)) return STATE_EXIT;

    // Init the next available token
    Token * token = &(tokens->token_array[tokens->num_tokens]);
    const char * str_location = tz->output + (tz->w); // Set the next available string location
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

    // Move to the right until ws character, quote, or operator
    while ( !is_space(PEEK) && 
            !is_nullc(PEEK) && 
            !is_quote(PEEK) && 
            !is_special_candidate(PEEK)
        ) { WRITE(CONSUME); }

    
    if (is_quote(PEEK)) {
        tz->quote = CONSUME; 
        return STATE_QUOTE_WRITE_TOKEN; // l"s" -> ls
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

    // Safe to pass to handle_normal_write_token to deal with \0
    tz->quote = '\0';
    ADVANCE;
    return STATE_NORMAL_WRITE_TOKEN;
}

static State handle_special_token(Tokenizer * tz) {
    Tokens * tokens = tz->tokens;
    TokenType token_type;
    size_t token_size;

    validate_special_candidates(tz->input + tz->r, &token_type, &token_size); 

    if (token_type != TOKEN_ERROR) {
        
        // Special case a|, finish up previous token
        if (!(tz->at_start)) {
            WRITE('\0');
            tokens->num_tokens++;
        }

        tz->r += token_size;
        
        // Create new token for special case
        Token * token = &(tokens->token_array[tokens->num_tokens]);
        write_token(token, token_type, SPECIAL_TOKENS[token_type]);
        tokens->num_tokens++;
        return STATE_START_TOKEN;
    }

    // Otherwise just write like normal character
    WRITE(CONSUME);
    return STATE_NORMAL_WRITE_TOKEN;
}