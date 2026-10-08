#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

#define MAX_ARGS 16
#define MAX_PIPELINE_STAGES 7

typedef struct {
    char * argv[MAX_ARGS + 1];
    unsigned int argc;
} Command;

typedef struct {
    Command stages[MAX_PIPELINE_STAGES];
    int num_stages;
    int background;
    char * infile;
    char * outfile;
    int append;
} Pipeline;

typedef enum {
    PAR_OK,
    PAR_ERROR,
    PAR_REDIRECT_ERROR,
} ParseStatus;

ParseStatus parse_tokens(Tokens * tokens, Pipeline * pipeline);
void test_parse(Pipeline * pipeline);

#endif


