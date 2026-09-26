#include "tokens.h"
#include <stdio.h>

/* ---------------------------- Defined operators --------------------------- */
char * SPECIAL_TOKENS[_NUMB_SPECIAL_TOKENS] = {
    [TOKEN_PIPE] = "|",
    [TOKEN_OR] = "||",
    [TOKEN_REDIRECT_IN] = "<",
    [TOKEN_REDIRECT_OUT] = "&",
    [TOKEN_REDIRECT_OUT_APPEND] = ">>",
    [TOKEN_BACKGROUND] = "|&"
};

int SPECIAL_TOKENS_SIZE[_NUMB_SPECIAL_TOKENS] = {
    [TOKEN_PIPE] = 1,
    [TOKEN_OR] = 2,
    [TOKEN_REDIRECT_IN] = 1,
    [TOKEN_REDIRECT_OUT] = 1,
    [TOKEN_REDIRECT_OUT_APPEND] = 2,
    [TOKEN_BACKGROUND] = 2,
};

/* --------------------------------- Globals -------------------------------- */
TokenType _special_stack[_NUMB_SPECIAL_TOKENS];
int _stack_fill = 0;

/* -------------------------- Function Definitions -------------------------- */
int set_special_candidates(const char c) {
    _stack_fill = 0;
    int i;
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) { 
        if (c != SPECIAL_TOKENS[i][0]) continue;
        _special_stack[_stack_fill] = (TokenType) i;
        _stack_fill++;
        // printf("candidate: %d\n", i);
    }
    return _stack_fill;
}

// int is_special_candidate(const char c) {
//     int i;
//     for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) { if (c == SPECIAL_TOKENS[i][0]) return 1; }
//     return 0;
// }

void validate_special_candidates(const char * starting, TokenType * return_type, int * return_length) {
    int i;
    // reset candidates 
    TokenType candidates_states[_NUMB_SPECIAL_TOKENS];
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) candidates_states[i] = TOKEN_INPROGRESS;
    int numb_verified_candidates = 0;

    TokenType type = TOKEN_INVALID;
    // find next candidate
    int offset = 0;
    int curr_stack_fill = _stack_fill;
    while (curr_stack_fill > 0) {
        //printf("t");
        for (i = curr_stack_fill - 1; i >= 0; i--) {
            TokenType curr_token = (TokenType) _special_stack[i];
            
            if (SPECIAL_TOKENS[curr_token][offset] == '\0') { 
                //printf(" NULL! ");
                TokenType temp = _special_stack[i];
                _special_stack[i] = _special_stack[curr_stack_fill - 1];
                _special_stack[curr_stack_fill - 1] = temp;
                curr_stack_fill--;

                candidates_states[curr_token] = curr_token;
                numb_verified_candidates++;
            }

            else if (starting[offset] != SPECIAL_TOKENS[curr_token][offset]) { 
                //printf(" MISMATCH! ");
                TokenType temp = _special_stack[i];
                _special_stack[i] = _special_stack[curr_stack_fill - 1];
                _special_stack[curr_stack_fill - 1] = temp;
                curr_stack_fill--;

                candidates_states[curr_token] = TOKEN_INVALID;
                numb_verified_candidates++;
            }
            //printf("(#%d, i: %d, token : %d) -> ",curr_stack_fill, i, curr_token);
        }
        if (starting[offset] == '\0') break;
        offset++;
    }

    int max_size = 0;
    for (i = 0; i < _stack_fill; i++) {
        TokenType curr_token = (TokenType) _special_stack[i];
        if (candidates_states[curr_token] == TOKEN_INVALID || candidates_states[curr_token] == TOKEN_INPROGRESS) continue;
        if (SPECIAL_TOKENS_SIZE[curr_token] < max_size) continue;
        type = curr_token;
        max_size = SPECIAL_TOKENS_SIZE[curr_token];
    }

    *(return_length) = (max_size != 0) ? max_size : 1;
    *(return_type) = type;
    // printf("len: %d, type: %d\n", *(return_length), *(return_type));
}