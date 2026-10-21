#include <stdio.h>
#include <stddef.h>

#include "io.h"
#include "parser.h"

/* ---------------------------- HELPER FUNCTIONS ---------------------------- */

static void init_pipeline(Pipeline * p) {
    p->num_stages = 0;
    p->background = 0;
    p->infile = NULL;
    p->outfile = NULL;
}

static Command * get_next_available_command(Pipeline * p) {
    if (p->num_stages == MAX_PIPELINE_STAGES) return NULL;
    Command * command = &(p->stages[p->num_stages++]);
    command->argv[0] = NULL;
    command->argc = 0;
    return command;
}

static int add_argv(Command * c, const char * arg) {
    if (c->argc > MAX_ARGS) return -1;
    c->argv[c->argc] = arg;
    c->argc++;
    c->argv[c->argc] = NULL;
    return 0;
}

/* --------------------------- FUNCTION DEFINITONS -------------------------- */

ParseStatus parse_tokens(Tokens * tokens, Pipeline * pipeline) {

    init_pipeline(pipeline);
    int current_token_index = 0;
    int unfulfilled_pipe = 0;
    Command * command;
    
    new_command:
    command = get_next_available_command(pipeline);

    while (current_token_index < num_tokens(tokens)) {
        
        Token * token = index_tokens(tokens, current_token_index);
        TokenType type = token->type;
        const char * text = token->text;

        switch (type) {
        case TOKEN_REDIRECT_IN:
            current_token_index++;
            token = index_tokens(tokens, current_token_index);
            if (token == NULL) {
                WRITE_ERR(ERROR_START " Syntax error: '<' missing argument\n");
                return PAR_ERROR;
            }

            type = token->type;
            text = token->text;

            if (type != TOKEN_WORD) {
                WRITE_ERR(ERROR_START " Syntax error: unexpected token '", text, "'\n");
                return PAR_ERROR;
            }

            pipeline->infile = text;
            current_token_index++;

            break;
        case TOKEN_REDIRECT_OUT:
            current_token_index++;
            token = index_tokens(tokens, current_token_index);
            if (token == NULL) {
                WRITE_ERR(ERROR_START " Syntax error: '>' missing argument\n");
                return PAR_ERROR;
            }

            type = token->type;
            text = token->text;

            if (type != TOKEN_WORD) {
                WRITE_ERR(ERROR_START " Syntax error: unexpected token '", text, "'\n");
                return PAR_ERROR;
            }

            pipeline->outfile = text;
            current_token_index++;

            break;
        case TOKEN_PIPE:

            if (command->argc == 0) {
                WRITE_ERR(ERROR_START " Syntax error: No word before '|' \n");
                return PAR_ERROR;
            }

            if (pipeline->num_stages == MAX_PIPELINE_STAGES) {
                WRITE_ERR(ERROR_START " Syntax error: Max pipeline limit reached ("TO_STRING(MAX_PIPELINE_STAGES)")\n");
                return PAR_ERROR; 
            }
            current_token_index++;
            unfulfilled_pipe = 1;

            goto new_command;
        case TOKEN_BACKGROUND:

            if (command->argc == 0) {
                WRITE_ERR(ERROR_START " Syntax error: No word before '&' \n");
                return PAR_ERROR; 
            }
            if (current_token_index < num_tokens(tokens) - 1) {
                WRITE_ERR(ERROR_START " Syntax error: Invalid '&' placement\n");
                return PAR_ERROR; 
            }

            pipeline->background = 1;
            current_token_index++;

            break;
        case TOKEN_WORD:
            
            if (add_argv(command, text) < 0) {
                WRITE_ERR(ERROR_START " Command reached max number of arguments ("TO_STRING(MAX_ARGS)")\n");
                return PAR_ERROR; 
            }

            current_token_index++;
            unfulfilled_pipe = 0;

            break;
        default:
            WRITE_ERR(ERROR_START " Unsupported token type '", text, "'\n");
            return PAR_ERROR; 
            break;
        }
    }

    if (unfulfilled_pipe) {
        WRITE_ERR(ERROR_START " Syntax error: No word after '|' \n");
        return PAR_ERROR;
    }

    return PAR_OK;
}