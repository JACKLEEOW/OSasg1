#include <stdio.h>
#include "tokens.h"
#include <string.h>
#include "const.h"
#include "cstring.h"

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
    int errors = 0;
    Token expected_tk_oneword = {
        .text = "Hello",
        .type = TOKEN_WORD
    };
    Token expected_tk_pipe = {
        .text = "|",
        .type = TOKEN_PIPE
    };
    Token expected_tk_redirect_in = {
        .text = "<",
        .type = TOKEN_REDIRECT_IN
    };
    Token expected_tk_redirect_out = {
        .text = ">",
        .type = TOKEN_REDIRECT_OUT
    };
    Token expected_tk_redirect_out_append = {
        .text = ">>",
        .type = TOKEN_REDIRECT_OUT_APPEND
    };
    Token expected_tk_background = {
        .text = "&",
        .type = TOKEN_BACKGROUND
    };


    Tokens expected_hello_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword
        },
        .num_tokens = 1
    };
    char * word_input = "Hello";
    if(test_tokenizar(word_input, &expected_hello_token)) {
        printf("Test one word failed\n");
        errors++;
    }

    char * many_words_input = "Hello Hello Hello Hello Hello";
    Tokens expected_many_words_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_oneword,
            [2] = expected_tk_oneword,
            [3] = expected_tk_oneword,
            [4] = expected_tk_oneword
        },
        .num_tokens = 5
    };
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
    Tokens expected_max_words_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = {0}
        },
        .num_tokens = MAX_INPUT_SIZE/6+1
    };
    for(int i = 0; i < MAX_INPUT_SIZE/6+1; i++){
        expected_max_words_token.token_array[i] = expected_tk_oneword;
    }
    if(test_tokenizar(max_words_input, &expected_max_words_token)) {
        printf("Test max words failed\n");
        errors++;
    }
    
    char * pipe_input = "Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello | Hello";
    Tokens expected_pipe_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_pipe,
            [2] = expected_tk_oneword,
            [3] = expected_tk_pipe,
            [4] = expected_tk_oneword,
            [5] = expected_tk_pipe,
            [6] = expected_tk_oneword,
            [7] = expected_tk_pipe,
            [8] = expected_tk_oneword,
            [9] = expected_tk_pipe,
            [10] = expected_tk_oneword,
            [11] = expected_tk_pipe,
            [12] = expected_tk_oneword,
            [13] = expected_tk_pipe,
            [14] = expected_tk_oneword,
            [15] = expected_tk_pipe,
            [16] = expected_tk_oneword,
            [17] = expected_tk_pipe,
            [18] = expected_tk_oneword
        },
        .num_tokens = 19
    };

    if(test_tokenizar(pipe_input, &expected_pipe_token)) {
        printf("Test pipe failed\n");
        errors++;
    }

    char * redirect_in_input = "Hello < Hello ";
    Tokens expected_redirect_in_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_redirect_in,
            [2] = expected_tk_oneword
        },
        .num_tokens = 3
    };
    if(test_tokenizar(redirect_in_input, &expected_redirect_in_token)) {
        printf("Test redirect in failed\n");
        errors++;
    }

    char * redirect_out_input = "Hello > Hello ";
    Tokens expected_redirect_out_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_redirect_out,
            [2] = expected_tk_oneword
        },
        .num_tokens = 3
    };
    if(test_tokenizar(redirect_out_input, &expected_redirect_out_token)) {
        printf("Test redirect out failed\n");
        errors++;
    }
    char * redirect_out_append_input = "Hello >> Hello ";
    Tokens expected_redirect_out_append_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_redirect_out_append,
            [2] = expected_tk_oneword
        },
        .num_tokens = 3
    };
    if(test_tokenizar(redirect_out_append_input, &expected_redirect_out_append_token)) {
        printf("Test redirect out append failed\n");
        errors++;
    }
    char * background_input = "Hello & ";
    Tokens expected_background_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_background
        },
        .num_tokens = 2
    };
    if(test_tokenizar(background_input, &expected_background_token)) {
        printf("Test background failed\n");
        errors++;
    }
    char * special_combo_input = "   Hello   <   Hello   >   Hello   >>>   Hello   &   | | Hello<<Hello>>>Hello&Hello|Hello   ";
    Tokens expected_special_combo_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = expected_tk_oneword,
            [1] = expected_tk_redirect_in,
            [2] = expected_tk_oneword,
            [3] = expected_tk_redirect_out,
            [4] = expected_tk_oneword,
            [5] = expected_tk_redirect_out_append,
            [6] = expected_tk_oneword,
            [7] = expected_tk_background,
            [8] = expected_tk_pipe,
            [9] = expected_tk_pipe,
            [10] = expected_tk_oneword,
            [11] = expected_tk_redirect_in,
            [12] = expected_tk_redirect_in,
            [13] = expected_tk_oneword,
            [14] = expected_tk_redirect_out_append,
            [15] = expected_tk_oneword,
            [16] = expected_tk_background,
            [17] = expected_tk_oneword,
            [18] = expected_tk_pipe,
            [19] = expected_tk_oneword
        },
        .num_tokens = 20
    };
    if(test_tokenizar(special_combo_input, &expected_special_combo_token)) {
        printf("Test special combo failed\n");
        errors++;
    }

    if (errors > 0) {
        printf("Total errors: %d\n", errors);
    }
    return 0;
}