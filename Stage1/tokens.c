#include "tokens.h"
#include "const.h"
#include "cstring.h"
#include <stdio.h>


// const char * SPECIAL_TOKENS[_NUMB_SPECIAL_TOKENS] = {
//     [TOKEN_PIPE] = "||",
//     [TOKEN_OR] = "|||",
//     [TOKEN_REDIRECT_IN] = "<",
//     [TOKEN_REDIRECT_OUT] = ">",
//     [TOKEN_BACKGROUND] = "&"
// };

// const int SPECIAL_TOKENS_SIZE[_NUMB_SPECIAL_TOKENS] = {
//     [TOKEN_PIPE] = 2,
//     [TOKEN_OR] = 3,
//     [TOKEN_REDIRECT_IN] = 1,
//     [TOKEN_REDIRECT_OUT] = 1,
//     [TOKEN_BACKGROUND] = 1
// };


//printf("set Pointer (%d)\n", i);
//printf("%d, %d ->E:\n", s[i], c_isspace((unsigned char)s[i]));
int is_quote(char c);
int is_digit(char c);

int special_token_helper(int * i, int * j, char * s, Tokens * t, bool start);
// int is_special_candidate(char c);
// void retrieve_special_token(char * starting, TokenType * return_type, int * return_length);

void create_tokens(Tokens * t, char * s) {

    // Tokens t = {
    //     .token_strings = {'\0'},
    //     .token_string_ptrs = {NULL},
    //     .num_tokens = 0
    // };
    
    t->num_tokens = 0;

    if (s[0] == '\0' || s == NULL) return;

    int i = 0; // iterator for input string
    int j = 0; // iterator for copied string
    char curr_quote = '\0';

    // loops runs once for a word
    while (1) {
        TokenType type = TOKEN_WORD;
        

        // move to the right until non-ws character (Beginning of the token)
        while(c_isspace((unsigned char)s[i])) {/*t->token_strings[j] = s[i];*/ i++; /*j++;*/}

        if (s[i] == '\0') break;
        if (is_quote(s[i])) {curr_quote = s[i]; i++;} // skip pass quote
        if (set_special_candidates(s[i])) {
            if (special_token_helper(&i, &j, s, t, true)) {
                goto End;
            }
        } else {
            // set pointer to start of string
            t->token_string_ptrs[t->num_tokens] = t->token_strings + j;
            t->num_tokens ++;
        }

        // quote mode
        if (curr_quote) {
            QuoteMode:
            // move to the right until closing quote character (read anything)
            while(s[i] != curr_quote && s[i] != '\0') {t->token_strings[j] = s[i]; i++; j++;}
            if (s[i] == '\0') {t->token_strings[j] = '\0'; break;} // maybe set a flag
            curr_quote = '\0'; // reset quote
            i++;
        }
            
        GoNext:
        // move to the right until ws character, quote, or operator
        while(!c_isspace((unsigned char)s[i]) && s[i] != '\0' && !is_quote(s[i]) && !is_special_candidate(s[i])) { t->token_strings[j] = s[i]; i++; j++;}
        if (s[i] == '\0') {t->token_strings[j] = '\0'; break;} // if character was null terminator, end

        // found in between (operators)
        if (is_quote(s[i])) {curr_quote = s[i]; i++; goto QuoteMode;} // if quote found, skip pass string pointer, l"s" -> ls, cstring pointer to l from previous
        if (set_special_candidates(s[i])) {
            // int offset;

            // retrieve_special_token(s + i, &type, &offset); 
            // i+=(offset - 1);
            // t->token_string_ptrs[t->num_tokens] = SPECIAL_TOKENS[type];
            // t->num_tokens ++;
            if (!special_token_helper(&i, &j, s, t, false)) {
                goto GoNext;
            }
        }
        End:
        t->token_strings[j] = '\0'; // otherwise, add a \0 separator
        i++;
        j++;
    }

    return;
}

char * index_tokens(Tokens * t, int i) {
    if (i < 0 || i >= t->num_tokens) return NULL;
    return t->token_string_ptrs[i];
}

int num_tokens(Tokens * t) {
    return t->num_tokens;
}

void test_tokens(Tokens * t) {
    printf("Number of Tokens: %d\n", t->num_tokens);
    int i;
    for (i = 0; i < num_tokens(t); i++) {
        char * s = index_tokens(t, i);
        printf("Token: %s (%ld)\n", s, c_strlen(s));
    }
}

int is_quote(char c) {
    return (c == '\'' || c =='"');
}

int is_digit(char c) {
    return (c >= '0' && c <='9');
}

int is_special_candidate(const char c) {
    int i;
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) { if (c == SPECIAL_TOKENS[i][0]) return 1; }
    return 0;
}

/**
 * i points to the LAST char of the alleged special token
 * if the token was found to be invalid, add it to the buffer using j
 */
int special_token_helper(int * i, int * j, char * s, Tokens * t, bool start) {
    TokenType token_type;
    int token_size;
    validate_special_candidates(s + *i, &token_type, &token_size); 

    if (token_type != TOKEN_INVALID) {
        *i += (token_size - 1);
        t->token_string_ptrs[t->num_tokens] = SPECIAL_TOKENS[token_type];
        t->num_tokens ++;
        return 1;
    }

    if (start) {
        t->token_string_ptrs[t->num_tokens] = t->token_strings + (*j);
        t->num_tokens ++;
    }
    int k;
    for (k = 0; k < token_size; k++) {
        t->token_strings[*(j)] = s[*(i)];
        (*j)++;
        (*i)++;

    }

    // printf("i: %d j:%d\n", *i,*j);
    // printf("char: %c\n", s[(*i)]);
    // printf("Size: %d\n", token_size);

    return 0;  
}
/*
void retrieve_special_token(char * starting, TokenType * return_type, int * return_length) {
    int i;
    // reset candidates 
    bool selected_candidates[_NUMB_SPECIAL_TOKENS];
    bool verified_candidates[_NUMB_SPECIAL_TOKENS];
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) selected_candidates[i] = true;
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) verified_candidates[i] = false;
    int numb_verified_candidates = 0;

    TokenType type = TOKEN_INVALID;
    // find next candidate
    int offset = 0;

    while (numb_verified_candidates < _NUMB_SPECIAL_TOKENS) {
        // printf("t");
        for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) {
            if (verified_candidates[i]) continue;
            else if (!selected_candidates[i]) continue;
            else if (SPECIAL_TOKENS[i][offset] == '\0') { 
                verified_candidates[i] = true; 
                numb_verified_candidates++;
            }

            else if (starting[offset] != SPECIAL_TOKENS[i][offset]) { 
                selected_candidates[i] = false; 
                verified_candidates[i] = true;
                numb_verified_candidates++;
            }
            // printf("(#%d, t: %d )-> ",numb_verified_candidates, i);
        }
        if (starting[offset] == '\0') break;
        offset++;
    }

    int max_size = 0;
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) {
        if (!(selected_candidates[i] && verified_candidates[i])) continue;
        if (SPECIAL_TOKENS_SIZE[i] < max_size) continue;
        type = i;
        max_size = SPECIAL_TOKENS_SIZE[i];
    }

    *(return_length) = (max_size != 0) ? max_size : offset;
    *(return_type) = type;
    // printf("Special token found (t: %d, s: %d, str : %s)\n", (int) i, max_size, SPECIAL_TOKENS[type]);
}
*/