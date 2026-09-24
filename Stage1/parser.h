#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 16

#include "tokens.h"


typedef struct {
    char * argv[MAX_ARGS + 1];
    unsigned int argc;
} Command;

void parse_tokens(Tokens * t, Command * c);
#endif