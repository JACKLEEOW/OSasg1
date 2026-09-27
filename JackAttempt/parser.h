#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

#define MAX_ARGS 16
#define MAX_PIPELINE_STAGES 7

typedef struct {
    char * argv[MAX_ARGS + 1]; 
    unsigned int argc;
    char * infile;         
    char * outfile;
    int append;
} Command;

typedef struct {
    Command stages[MAX_PIPELINE_STAGES];
    int num_stages;
    bool background;
} Pipeline;

typedef enum {
    PAR_ERROR,
    PAR_OK
} ParseStatus;

ParseStatus parse_tokens(Tokens * tokens, Pipeline * pipeline);
void test_parse(Pipeline * pipeline);
#endif