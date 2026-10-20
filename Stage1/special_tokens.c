#include <stddef.h>

#include "tokens.h"

/* ----------------------------- SPECIAL TOKENS ----------------------------- */

const char * SPECIAL_TOKENS[NUMB_SPECIAL_TOKENS] = {
    [TOKEN_PIPE] = "|",
    [TOKEN_REDIRECT_IN] = "<",
    [TOKEN_REDIRECT_OUT] = ">",
    [TOKEN_REDIRECT_OUT_APPEND] = ">>",
    [TOKEN_BACKGROUND] = "&"
};

const size_t SPECIAL_TOKENS_SIZE[NUMB_SPECIAL_TOKENS] = {
    [TOKEN_PIPE] = 1,
    [TOKEN_REDIRECT_IN] = 1,
    [TOKEN_REDIRECT_OUT] = 1,
    [TOKEN_REDIRECT_OUT_APPEND] = 2,
    [TOKEN_BACKGROUND] = 1,
};

/* --------------------------------- GLOBALS -------------------------------- */

static TokenType special_stack[NUMB_SPECIAL_TOKENS];
static int stack_fill = 0;

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

int set_special_candidates(const char c) {
    stack_fill = 0;
    for (int i = 0; i < NUMB_SPECIAL_TOKENS; i++) { 
        if (c != SPECIAL_TOKENS[i][0]) continue;
        special_stack[stack_fill] = (TokenType) i;
        stack_fill++;
    }
    return stack_fill;
}

void validate_special_candidates(const char * starting, TokenType * return_type, size_t * return_length) {
    
    // Reset candidates 
    TokenType candidates_states[NUMB_SPECIAL_TOKENS];
    for (int i = 0; i < NUMB_SPECIAL_TOKENS; i++) candidates_states[i] = TOKEN_ERROR;
    int numb_verified_candidates = 0;

    // Find next candidate
    size_t offset = 0;
    int curr_stack_fill = stack_fill;
    while (curr_stack_fill > 0) {
        for (int i = curr_stack_fill - 1; i >= 0; i--) {
            TokenType curr_token = (TokenType) special_stack[i];
            
            // Candidate token successfully match
            if (SPECIAL_TOKENS[curr_token][offset] == '\0') { 
                TokenType temp = special_stack[i];
                special_stack[i] = special_stack[curr_stack_fill - 1];
                special_stack[curr_stack_fill - 1] = temp;
                curr_stack_fill--;

                candidates_states[curr_token] = curr_token;
                numb_verified_candidates++;
            }

            // Candidate token did not match
            else if (starting[offset] != SPECIAL_TOKENS[curr_token][offset]) { 
                TokenType temp = special_stack[i];
                special_stack[i] = special_stack[curr_stack_fill - 1];
                special_stack[curr_stack_fill - 1] = temp;
                curr_stack_fill--;
                numb_verified_candidates++;
            }

        }

        // Input ended
        if (starting[offset] == '\0') break;
        offset++;
    }

    // Get longest successful candidate token
    TokenType type = TOKEN_ERROR;
    size_t max_size = 0;
    for (int i = 0; i < stack_fill; i++) {
        TokenType curr_token = (TokenType) special_stack[i];
        if (candidates_states[curr_token] == TOKEN_ERROR) continue;
        if (SPECIAL_TOKENS_SIZE[curr_token] < max_size) continue;
        type = curr_token;
        max_size = SPECIAL_TOKENS_SIZE[curr_token];
    }

    *(return_length) = (max_size != 0) ? max_size : 1;
    *(return_type) = type;
}