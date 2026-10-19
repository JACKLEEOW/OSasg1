#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

#define MAX_ARGS 16
#define MAX_PIPELINE_STAGES 7

typedef struct {
    const char * argv[MAX_ARGS + 1];
    unsigned int argc;
} Command;

typedef struct {
    Command stages[MAX_PIPELINE_STAGES];
    int num_stages;
    int background;
    const char * infile;
    const char * outfile;
    int append;
} Pipeline;

typedef enum {
    PAR_OK,
    PAR_ERROR,
    PAR_REDIRECT_ERROR,
} ParseStatus;

/**
 * @brief parses the tokens into a pipeline
 * @param tokens a pointer to a tokens struct
 * @param pipeline a pointer to a pipeline struct that will be populated with the parsed tokens
 * @note the pipeline struct must be initialized before calling this function
 * @return PAR_OK if successful, PAR_ERROR if failed, PAR_REDIRECT_ERROR if failed due to redirection error
 */
ParseStatus parse_tokens(Tokens * tokens, Pipeline * pipeline);
Command *index_command(Pipeline * pipeline, int i);

#endif