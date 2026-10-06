#include "parser.h"
#include <stdio.h>
#include "tokens.h"
#include <string.h>
#include "const.h"

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
    // test_parse(&p);
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
    Tokens full_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword
            },
        .num_tokens = MAX_INPUT_SIZE,
    };
    for(int i = 0; i < MAX_INPUT_SIZE; i++){
        full_token.token_array[i] = tk_oneword;
    }Tokens fullpipeline_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_pipe,
            },
        .num_tokens = MAX_INPUT_SIZE,
    };
    for(int i = 0; i < MAX_INPUT_SIZE; i++){
        if(i % 2 == 0) {
            fullpipeline_token.token_array[i] = tk_oneword;
        } else {
            fullpipeline_token.token_array[i] = tk_pipe;
        }
        full_token.token_array[i] = tk_oneword;
    }
    Tokens redirect_in_fail_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_redirect_in,
            [2] = tk_pipe,
            [3] = tk_oneword
        },
        .num_tokens = 4
    };
    Tokens redirect_out_fail_token1 = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_redirect_out,
        },
        .num_tokens = 2
    };Tokens redirect_out_fail_token2 = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_redirect_out,
            [2] = tk_redirect_out,
        },
        .num_tokens = 4
    };

    Pipeline expected_word = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 0,
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
        .num_stages = 1,
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
        .num_stages = 0,
        .background = 0,
        .infile = "Hello",
        .outfile = "Hello",
        .append = 0
    };
    Pipeline expected_full = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = MAX_ARGS
            },
        },
        .num_stages = 0,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    for(int i = 0; i < MAX_ARGS; i++){
        expected_full.stages[0].argv[i] = "Hello";
    }
    expected_full.stages[0].argv[MAX_ARGS+1] = NULL;
    Pipeline expected_fullpipeline = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 0
            },
        },
        .num_stages = MAX_PIPELINE_STAGES,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    for (int i = 0; i < MAX_PIPELINE_STAGES; i++) {
        expected_fullpipeline.stages[i].argv[0] = "Hello";
        expected_fullpipeline.stages[i].argc++;
        expected_fullpipeline.stages[i].argv[1] = NULL;
    }
    Pipeline expected_redirect_in_fail = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 0,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    Pipeline expected_redirect_out_fail = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 0,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
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
    if(test_parser(&full_token, &expected_full) == PAR_ERROR){
        printf("Full test failed\n");
        errors++;
    }
    //printf("Testing full pipeline with %d stages\n", MAX_PIPELINE_STAGES);
    //test_tokens(&fullpipeline_token);
    //test_parse(&expected_fullpipeline);
    // IDK why this test is failing.
    if(test_parser(&fullpipeline_token, &expected_fullpipeline) == PAR_ERROR){
        printf("Full pipeline test failed\n"); 
        errors++;
    }
    if(test_parser(&redirect_in_fail_token, &expected_redirect_in_fail) == PAR_ERROR){
        printf("Redirect fail test failed\n");
        errors++;
    }
    if(test_parser(&redirect_out_fail_token1, &expected_redirect_out_fail) == PAR_ERROR){
        printf("Redirect out fail test 1 failed\n");
        errors++;
    }if(test_parser(&redirect_out_fail_token2, &expected_redirect_out_fail) == PAR_ERROR){
        printf("Redirect out fail test 2 failed\n");
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