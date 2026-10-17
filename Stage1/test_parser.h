#include <stdio.h>
#include <string.h>
#include "tokens.h"
#include "const.h"
#include "parser.h"

#define tk_oneword {.text = "Hello", .type = TOKEN_WORD}
#define tk_pipe {.text = "|", .type = TOKEN_PIPE}
#define tk_redirect_in {.text = "<", .type = TOKEN_REDIRECT_IN}
#define tk_redirect_out {.text = ">", .type = TOKEN_REDIRECT_OUT}
#define tk_background {.text = "&", .type = TOKEN_BACKGROUND}

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
        [1] = tk_background
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

static void initialize_test_parser_data() {
    for (int i = 0; i < MAX_INPUT_SIZE; i++) {
        full_token.token_array[i].text = "Hello";
        full_token.token_array[i].type = TOKEN_WORD;
        fullpipeline_token.token_array[i].text = (i % 2 == 0) ? "Hello" : "|";
        fullpipeline_token.token_array[i].type = (i % 2 == 0) ? TOKEN_WORD : TOKEN_PIPE;
    }
    for (int i = 0; i < MAX_ARGS; i++) {
        expected_full.stages[0].argv[i] = "Hello";
    }
    expected_full.stages[0].argv[MAX_ARGS] = NULL;
    for (int i = 0; i < MAX_PIPELINE_STAGES; i++) {
        expected_fullpipeline.stages[i].argv[0] = "Hello";
        expected_fullpipeline.stages[i].argc++;
        expected_fullpipeline.stages[i].argv[1] = NULL;
    }
}

