#include <stdio.h>
#include "tokens.h"
#include <string.h>
#include "const.h"
#include "cstring.h"

void test_tokens(Tokens * t);

#define expected_tk_oneword {.text = "Hello", .type = TOKEN_WORD}
#define expected_tk_pipe {.text = "|", .type = TOKEN_PIPE}
#define expected_tk_redirect_in {.text = "<", .type = TOKEN_REDIRECT_IN}
#define expected_tk_redirect_out {.text = ">", .type = TOKEN_REDIRECT_OUT}
#define expected_tk_background {.text = "&", .type = TOKEN_BACKGROUND}
#define expected_tk_redirect_out_append {.text = ">>", .type = TOKEN_REDIRECT_OUT_APPEND}

Tokens expected_hello_token = {
    .cstring_storage = {0},
    .token_array = {
        [0] = expected_tk_oneword
    },
    .num_tokens = 1
};
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

Tokens expected_max_words_token = {
    .cstring_storage = {0},
    .token_array = {
                [0] = expected_tk_oneword // initialized in main
    },
    .num_tokens = MAX_INPUT_SIZE/6+1
};
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

Tokens expected_redirect_in_token = {
    .cstring_storage = {0},
    .token_array = {
        [0] = expected_tk_oneword,
        [1] = expected_tk_redirect_in,
        [2] = expected_tk_oneword
    },
    .num_tokens = 3
};

Tokens expected_redirect_out_token = {
    .cstring_storage = {0},
    .token_array = {
        [0] = expected_tk_oneword,
        [1] = expected_tk_redirect_out,
        [2] = expected_tk_oneword
    },
    .num_tokens = 3
};

Tokens expected_redirect_out_append_token = {
    .cstring_storage = {0},
    .token_array = {
                [0] = expected_tk_oneword,
        [1] = expected_tk_redirect_out_append,
        [2] = expected_tk_oneword
    },
    .num_tokens = 3
};

Tokens expected_background_token = {
    .cstring_storage = {0},
    .token_array = {
                [0] = expected_tk_oneword,
        [1] = expected_tk_background
    },
    .num_tokens = 2
};

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
        [15] = expected_tk_redirect_out,
        [16] = expected_tk_oneword,
        [17] = expected_tk_background,
        [18] = expected_tk_oneword,
        [19] = expected_tk_pipe,
        [20] = expected_tk_oneword
    },
    .num_tokens = 21
};
static void initialize_test_token_data() {
    for(int i = 0; i < MAX_INPUT_SIZE/6+1; i++){
        expected_max_words_token.token_array[i] = expected_hello_token.token_array[0];
    }
};