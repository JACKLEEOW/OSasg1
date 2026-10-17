#include <stdio.h>
#include "tokens.h"
#include <string.h>
#include "const.h"
#include "cstring.h"
#include "test_token.h"

int is_tokens_equal(Tokens *t1, Tokens *t2) {
    if (t1->num_tokens != t2->num_tokens) {
        printf("Number of tokens not equal: %d vs %d\n", t1->num_tokens, t2->num_tokens);
        return 1;
    }
    for (int i = 0; i < t1->num_tokens; i++) {
        Token *token = index_tokens(t1, i);
        Token *t2_token = index_tokens(t2, i);
        if (c_strcmp(token->text, t2_token->text) != 0) {
            printf("Token text not equal at index %d: %s vs %s\n", i, token->text, t2_token->text);
            return 1;
        }
        if (token->type != t2_token->type) {
            printf("Token type not equal at index %d: %d vs %d\n", i, token->type, t2_token->type);
            return 1;
        }
    }
    return 0;
}

int test_tokenizar(char *input, Tokens *expected) {
    Tokens t;
    create_tokens(&t, input);
    int failed = is_tokens_equal(&t, expected);
    if(failed){
        printf("Token result:\n");
        test_tokens(&t);
        printf("Token expected:\n");
        test_tokens(expected);
        return 1;
    }
    return 0;
}

int main() {
    initialize_test_token_data();
    int errors = 0;
    
    char * word_input = "Hello";
    if(test_tokenizar(word_input, &expected_hello_token)) {
        printf("Test one word failed\n");
        errors++;
    }

    char * many_words_input = "Hello Hello Hello Hello Hello";
    if(test_tokenizar(many_words_input, &expected_many_words_token)) {
        printf("Test many words failed\n");
        errors++;
    }
    
    char * whitespace_words_input = "       Hello        Hello       Hello      Hello        \n    Hello       ";
    if(test_tokenizar(whitespace_words_input, &expected_many_words_token)) {
        printf("Test whitespace words failed\n");
        errors++;
    }

    char max_words_input[MAX_INPUT_SIZE];
    for(int i = 0; i < MAX_INPUT_SIZE; i = i + 6) {
        max_words_input[i] = 'H';
        max_words_input[i + 1] = 'e';
        max_words_input[i + 2] = 'l';
        max_words_input[i + 3] = 'l';
        max_words_input[i + 4] = 'o';
        max_words_input[i + 5] = ' ';
    }
    max_words_input[MAX_INPUT_SIZE+1] = '\0';
    if(test_tokenizar(max_words_input, &expected_max_words_token)) {
        printf("Test max words failed\n");
        errors++;
    }
    
    char * pipe_input = "Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello";
    if(test_tokenizar(pipe_input, &expected_pipe_token)) {
        printf("Test pipe failed\n");
        errors++;
    }

    char * redirect_in_input = "Hello < Hello ";
    if(test_tokenizar(redirect_in_input, &expected_redirect_in_token)) {
        printf("Test redirect in failed\n");
        errors++;
    }

    char * redirect_out_input = "Hello > Hello ";
    if(test_tokenizar(redirect_out_input, &expected_redirect_out_token)) {
        printf("Test redirect out failed\n");
        errors++;
    }
    char * redirect_out_append_input = "Hello >>> Hello ";
    if(test_tokenizar(redirect_out_append_input, &expected_redirect_out_append_token)) {
        printf("Test redirect out append failed\n");
        errors++;
    }

    char * background_input = "Hello & ";
    if(test_tokenizar(background_input, &expected_background_token)) {
        printf("Test background failed\n");
        errors++;
    }

    char * special_combo_input = "   Hello   <   Hello   >   Hello   >>>   Hello   &   | | Hello<<Hello>>>Hello&Hello|Hello   ";
    if(test_tokenizar(special_combo_input, &expected_special_combo_token)) {
        printf("Test special combo failed\n");
        errors++;
    }

    if (errors > 0) {
        printf("Total errors: %d\n", errors);
    }else {
        printf("All token tests passed\n");
    }
    return 0;
}