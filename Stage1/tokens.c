#include "tokens.h"
#include "const.h"
#include "cstring.h"
#include <stdio.h>

// Look into inline
int is_quote(char c);
int is_digit(char c);
int is_space(char c);
int is_nullc(char c);

void write_token(Token * token, TokenType type, char * s);

int special_token_helper(int * i, int * j, char * s, Tokens * t, bool start);

void create_tokens(Tokens * t, char * s) {
    // init 
    t->num_tokens = 0;

    if (s[0] == '\0' || s == NULL) return;

    int i = 0; // iterator for input string
    int j = 0; // iterator for copied string
    char curr_quote = '\0';

    // loops runs once for a word
    while (1) {
        
        // move to the right until non-ws character (Beginning of the token)
        while ( is_space(s[i]) ) { i++; }

        // exit if end of buffer
        if ( is_nullc(s[i]) ) break;

        if ( set_special_candidates(s[i]) ) {
            // will attempt to tokenize the alleged special token, otherwise **P as normal character
            if ( special_token_helper(&i, &j, s, t, true) ) {
                goto End;
            }
        } else {
            if ( is_quote(s[i]) ) { curr_quote = s[i]; i++; } // skip pass quote

            Token * token_ptr = &(t->token_array[t->num_tokens]);
            char * cstring_location = t->cstring_storage + (j);
            write_token(token_ptr, TOKEN_WORD, cstring_location);
            t->num_tokens ++;
        }

        // quote mode
        if (curr_quote) {
            QuoteMode:
            // move to the right until closing quote character (read anything)
            while( s[i] != curr_quote && s[i] != '\0') {t->cstring_storage[j] = s[i]; i++; j++;}
            if (s[i] == '\0') {t->cstring_storage[j] = '\0'; break;} // maybe set a flag
            curr_quote = '\0'; // reset quote
            i++;
        }
            
        GoNext:
        // move to the right until ws character, quote, or operator
        while ( !is_space(s[i]) && 
                !is_nullc(s[i]) && 
                !is_quote(s[i]) && 
                !is_special_candidate(s[i])
            ) { t->cstring_storage[j++] = s[i++]; }

        if ( is_nullc(s[i]) ) {t->cstring_storage[j] = '\0'; break;} // if character was null terminator, end

        // found in between (operators)
        if ( is_quote(s[i]) ) {
            curr_quote = s[i]; 
            i++; 
            goto QuoteMode; // if quote found, skip pass string pointer, l"s" -> ls, cstring pointer to l from previous
        } 

        if (set_special_candidates(s[i])) {
            if (!special_token_helper(&i, &j, s, t, false)) goto GoNext;
            else goto End;
        }
        End:
        t->cstring_storage[j] = '\0'; // otherwise, add a \0 separator
        i++;
        j++;
    }

    return;
}

Token * index_tokens(Tokens * t, int i) {
    if (i < 0 || i >= t->num_tokens) return NULL;
    return &(t->token_array[i]);
}

int num_tokens(Tokens * t) {
    return t->num_tokens;
}

void test_tokens(Tokens * t) {
    printf("Number of Tokens: %d\n", num_tokens(t));
    int i;
    for (i = 0; i < num_tokens(t); i++) {
        Token * token = index_tokens(t, i);
        printf("Token: %s (%ld), type: %d\n", token->text, c_strlen(token->text), (int) token->type);
    }
}

int is_quote(char c) {
    return (c == '\'' || c =='"');
}

int is_digit(char c) {
    return (c >= '0' && c <='9');
}

int is_space(char c) {
    return c_isspace((unsigned char) c);
}

int is_nullc(char c) {
    return c == '\0';
}

int is_special_candidate(const char c) {
    int i;
    for (i = 0; i < _NUMB_SPECIAL_TOKENS; i++) { if (c == SPECIAL_TOKENS[i][0]) return 1; }
    return 0;
}

/**
 * 
 * i points to the LAST char of the alleged special tokencstring_storage
 * if the token was found to be invalid, add it to the buffer using j
 */
int special_token_helper(int * i, int * j, char * s, Tokens * t, bool start) {
    TokenType token_type;
    int token_size;
    validate_special_candidates(s + *i, &token_type, &token_size); 

    if (token_type != TOKEN_INVALID) {
        *i += (token_size - 1);

        Token * token_ptr = &(t->token_array[t->num_tokens]);
        write_token(token_ptr, token_type, SPECIAL_TOKENS[token_type]);
        t->num_tokens ++;
        return 1;
    }

    if (start) {
        // t->token_string_ptrs[t->num_tokens] = t->cstring_storage + (*j);
        // t->num_tokens ++;

        Token * token_ptr = &(t->token_array[t->num_tokens]);
        char * cstring_location = t->cstring_storage + (*j);
        write_token(token_ptr, token_type, cstring_location);
        t->num_tokens ++;
    }
    // int k;
    // for (k = 0; k < token_size; k++) {
    //     t->cstring_storage[*(j)] = s[*(i)];
    //     (*j)++;
    //     (*i)++;

    // }

    // write character to storage
    t->cstring_storage[*(j)] = s[*(i)];
    (*j)++;
    (*i)++;

    // printf("i: %d j:%d\n", *i,*j);
    // printf("char: %c\n", s[(*i)]);
    // printf("Size: %d\n", token_size);

    return 0;  
}

void write_token(Token * token, TokenType type, char * s) {
    token->type = type;
    token->text = s;
}