#include "parser.h"
#include <stdio.h>
#include "tokens.h"
#include <string.h>

// ParseStatus parse_tokens(Tokens *tokens, Pipeline *pipeline);
// typedef struct {
//     char * text;
//     TokenType type;
// } Token;

// typedef struct {
//     char cstring_storage[MAX_INPUT_SIZE + 1];
//     Token token_array[MAX_INPUT_SIZE + 1];
//     int num_tokens;
// } Tokens;

int is_pipelines_equal(Pipeline * p1, Pipeline * p2) {
    if (p1->num_stages != p2->num_stages) {
        printf("num_stages not equal: %d vs %d\n", p1->num_stages, p2->num_stages);
        return 0;
    }
    for (int i = 0; i < p1->num_stages; i++) {
        if (p1->stages[i].argc != p2->stages[i].argc) {
            printf("argc not equal at stage %d: %d vs %d\n", i, p1->stages[i].argc, p2->stages[i].argc);
            return 0;
        }
        for (int j = 0; j < p1->stages[i].argc; j++) {
            if (strcmp(p1->stages[i].argv[j], p2->stages[i].argv[j]) != 0) {
                printf("argv not equal at stage %d, arg %d: %s vs %s\n", i, j, p1->stages[i].argv[j], p2->stages[i].argv[j]);
                return 0;
            }
        }
    }
    return 1;
}

int test_parser(Tokens * t, Pipeline * expected) {
    Pipeline p;
    parse_tokens(t, &p);
    if (expected == NULL) {
        return PAR_OK;
    }
    if(is_pipelines_equal(&p, expected)){
        return PAR_OK;
    }

    return PAR_ERROR;
}

int main() {
    Token tk_oneword = {
        .text = "Hello",
        .type = TOKEN_WORD
    };
    Token tk_pipe = {
        .text = "|",
        .type = TOKEN_PIPE
    };
    Token tk_redirect_in = {
        .text = "<",
        .type = TOKEN_REDIRECT_IN
    };
    Token tk_redirect_out = {
        .text = ">",
        .type = TOKEN_REDIRECT_OUT
    };
    Tokens hello_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword
        },
        .num_tokens = 1
    };
    Tokens empty_token = {
        .cstring_storage = {0},
        .token_array = {0},
        .num_tokens = 0
    };
    Tokens pipe_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_pipe,
            [2] = tk_oneword
        },
        .num_tokens = 3
    };
    Tokens redirect_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_redirect_in,
            [2] = tk_oneword,
            [3] = tk_redirect_out,
            [4] = tk_oneword
        },
        .num_tokens = 5
    };
    Pipeline expected_word = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 1,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    Pipeline expected_empty = {
        .stages = {0},
        .num_stages = 0,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    Pipeline expected_pipe = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            },
            [1] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 2,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    Pipeline expected_redirect = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 1,
        .background = 0,
        .infile = "Hello",
        .outfile = "Hello",
        .append = 0
    };
    int errors = 0;
    if(test_parser(&empty_token, &expected_empty) == PAR_ERROR){
        printf("Empty test failed\n");
        errors++;
    }
    if(test_parser(&hello_token, &expected_word) == PAR_ERROR){
        printf("Single word test failed\n");
        errors++;
    }
    if(test_parser(&pipe_token, &expected_pipe) == PAR_ERROR){
        printf("Pipe test failed\n");
        errors++;
    }
    if(test_parser(&redirect_token, &expected_redirect) == PAR_ERROR){
        printf("Redirect test failed\n");
        errors++;
    }
    printf("Total errors: %d\n", errors);

/*
    Tokens t;
    create_tokens(&t, "1 2 | > <<<<< 3");
    Pipeline p;
    printf("%d\n",parse_tokens(&t,&p));
    create_tokens(&t, "hi");
    printf("%d\n",parse_tokens(&t,&p));*/
    return 0;
}