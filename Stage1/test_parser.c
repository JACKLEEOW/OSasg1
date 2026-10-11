#include "parser.h"
#include <stdio.h>
#include "tokens.h"
#include <string.h>
#include "const.h"

int is_pipelines_equal(Pipeline * p1, Pipeline * p2) {
    if (p1->num_stages != p2->num_stages) {
        printf("num_stages not equal: %d vs %d\n", p1->num_stages, p2->num_stages);
        return 1;
    }
    for (int i = 0; i < p1->num_stages; i++) {
        if (p1->stages[i].argc != p2->stages[i].argc) {
            printf("argc not equal at stage %d: %d vs %d\n", i, p1->stages[i].argc, p2->stages[i].argc);
            return 1;
        }
        for (int j = 0; j < p1->stages[i].argc; j++) {
            if (strcmp(p1->stages[i].argv[j], p2->stages[i].argv[j]) != 0) {
                printf("argv not equal at stage %d, arg %d: %s vs %s\n", i, j, p1->stages[i].argv[j], p2->stages[i].argv[j]);
                return 1;
            }
        }
    }
    if (p1->background != p2->background) {
        printf("background not equal: %d vs %d\n", p1->background, p2->background);
        return 1;
    }
    if (p1->infile != p2->infile) {
        printf("infile not equal: %s vs %s\n", p1->infile, p2->infile);
        return 1;
    }
    if (p1->outfile != p2->outfile) {
        printf("outfile not equal: %s vs %s\n", p1->outfile, p2->outfile);
        return 1;
    }
    if (p1->append != p2->append) {
        printf("append not equal: %d vs %d\n", p1->append, p2->append);
        return 1;
    }
    
    return 0;
}

int test_parser(Tokens * t, Pipeline * expected, int is_fail_case) {;
    Pipeline p;
    ParseStatus parse_status = parse_tokens(t, &p);

    int is_fail_case_fail = 0;
    if(is_fail_case && parse_status != PAR_OK) {
        //char * status_str = (parse_status == PAR_ERROR) ? "PAR_ERROR" : "PAR_REDIRECT_ERROR";
        //printf("Expected failure successfully: %s\n", status_str);
    } else if(is_fail_case && parse_status == PAR_OK) {
        printf("Expected failure but got success: %d\n", parse_status);
        is_fail_case_fail = 1;
    }

    int failed = is_pipelines_equal(&p, expected);
    if (failed || is_fail_case_fail) {
        printf("Token input:\n");
        test_tokens(t);
        printf("Pipeline result:\n");
        test_parse(&p);
        //printf("Pipeline expected:\n");
        //test_parse(expected);
        return PAR_ERROR;
    }
    return PAR_OK;
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
    }
    Tokens multi_stage_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = tk_pipe,
            [2] = tk_oneword,
            [3] = tk_pipe,
            [4] = tk_oneword
        },
        .num_tokens = 5
    };
    Tokens fullpipeline_token = {
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
    Tokens background_token = {
        .cstring_storage = {0},
        .token_array = {
            [0] = tk_oneword,
            [1] = {
                .text = "&",
                .type = TOKEN_BACKGROUND
            }
        },
        .num_tokens = 2
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
    Pipeline expected_full = {
        .stages = {
            [0] = {
                .argv = {0},
                .argc = MAX_ARGS
            },
        },
        .num_stages = 1,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    for(int i = 0; i < MAX_ARGS; i++){
        expected_full.stages[0].argv[i] = "Hello";
    }
    expected_full.stages[0].argv[MAX_ARGS] = NULL;

    Pipeline expected_background = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 1,
        .background = 1,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
    Pipeline expected_multi_stage = {
        .stages = {
            [0] = {
                .argv = {"Hello", NULL},
                .argc = 1
            },
            [1] = {
                .argv = {"Hello", NULL},
                .argc = 1
            },
            [2] = {
                .argv = {"Hello", NULL},
                .argc = 1
            }
        },
        .num_stages = 3,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };
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
        .num_stages = 1,
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
        .num_stages = 1,
        .background = 0,
        .infile = NULL,
        .outfile = NULL,
        .append = 0
    };

    int errors = 0;
    if(test_parser(&empty_token, &expected_empty, 0) == PAR_ERROR){
        printf("Empty test failed\n");
        errors++;
    }
    if(test_parser(&hello_token, &expected_word, 0) == PAR_ERROR){
        printf("Single word test failed\n");
        errors++;
    }
    if(test_parser(&pipe_token, &expected_pipe, 0) == PAR_ERROR){
        printf("Pipe test failed\n");
        errors++;
    }
    if(test_parser(&redirect_token, &expected_redirect, 0) == PAR_ERROR){
        printf("Redirect test failed\n");
        errors++;
    }
    if(test_parser(&full_token, &expected_full, 0) == PAR_ERROR){
        printf("Full test failed\n");
        errors++;
    }
    if(test_parser(&multi_stage_token, &expected_multi_stage, 0) == PAR_ERROR){
        printf("Multi-stage test failed\n");
        errors++;
    }
    // IDK why this test is failing.
    if(test_parser(&fullpipeline_token, &expected_fullpipeline, 0) == PAR_ERROR){
        printf("Full pipeline test failed\n"); 
        errors++;
    }
    if(test_parser(&redirect_in_fail_token, &expected_redirect_in_fail, 1) == PAR_ERROR){
        printf("Redirect fail test failed\n");
        errors++;
    }
    if(test_parser(&redirect_out_fail_token1, &expected_redirect_out_fail, 1) == PAR_ERROR){
        printf("Redirect out fail test 1 failed\n");
        errors++;
    }if(test_parser(&redirect_out_fail_token2, &expected_redirect_out_fail, 1) == PAR_ERROR){
        printf("Redirect out fail test 2 failed\n");
        errors++;
    }
    if(test_parser(&background_token, &expected_background, 0) == PAR_ERROR){
        printf("Background test failed\n");
        errors++;
    }
    printf("Total errors: %d\n", errors);

    return 0;
}