#include "parser.h"
#include <stddef.h>
#include "io.h"
// #include <stdio.h>

ParseStatus new_command(Pipeline * pipeline);
void append_null(Command *c);
void reset_pipeline(Pipeline * pipeline);

ParseStatus parse_tokens(Tokens *tokens, Pipeline *pipeline){
  reset_pipeline(pipeline);

  if(tokens->num_tokens == 0) { return PAR_OK;}
  
  if(new_command(pipeline) == PAR_ERROR){ return PAR_ERROR;}
  int i = 0; // Iterate over every token
  // For every token in tokens, loop through
  while(i < tokens->num_tokens){
    Token cur_token = tokens->token_array[i];
    Command *cur_command = &(pipeline->stages[pipeline->num_stages]);
    append_null(cur_command);
    Token * next_token; // For redirects

    switch (cur_token.type) {
      case TOKEN_PIPE:
        // Next command in pipeline
        if(cur_command->argc == 0) {
          WRITE_ERR(ERROR_START " Syntax error: No word before '|' \n");
          return PAR_ERROR;
        }
        pipeline->num_stages++;
        if(new_command(pipeline) == PAR_ERROR) { 
          pipeline->num_stages--;
          return PAR_ERROR;
        }
        if(i+1 >= tokens->num_tokens) { 
          WRITE_ERR(ERROR_START " Syntax error: No word after '|' \n");
          pipeline->num_stages++;
          return PAR_ERROR;
        }
        break;
      case TOKEN_REDIRECT_IN:
        // set infile as next token
        if(i+1 >= tokens->num_tokens) { 
          WRITE_ERR(ERROR_START " Syntax error: '<' missing argument\n");
          return PAR_REDIRECT_ERROR;
        }
        i++;
        // next_token = tokens->token_array[i];
        next_token = index_tokens(tokens, i);
        if(next_token->type != TOKEN_WORD){
          pipeline->num_stages++;
          WRITE_ERR(ERROR_START " Syntax error: unexpected token '", next_token->text, "'\n");
          return PAR_REDIRECT_ERROR;
        }
        pipeline->infile = next_token->text;
        break;
      case TOKEN_REDIRECT_OUT:
        // set outfile as next token
        if(i+1 >= tokens->num_tokens) { pipeline->num_stages++; 
          WRITE_ERR(ERROR_START " Syntax error: '>' missing argument\n");
          return PAR_REDIRECT_ERROR;
        }
        i++;
        // next_token = tokens->token_array[i];
        next_token = index_tokens(tokens, i);
        if(next_token->type != TOKEN_WORD){
          pipeline->num_stages++;
          WRITE_ERR(ERROR_START " Syntax error: '>' missing argument\n");
          return PAR_REDIRECT_ERROR;
        }
        pipeline->outfile = next_token->text;
        break;
        case TOKEN_BACKGROUND:
          if(cur_command->argc == 0) {
            //WRITE_ERR(ERROR_START " Syntax error: No word before '&' \n");
            //return PAR_ERROR;
          }
          pipeline->background = 1;
          next_token = index_tokens(tokens, i);
          if(next_token != NULL) {
            //WRITE_ERR(ERROR_START " Syntax error: Invalid '&' placement\n");
            //return PAR_ERROR;
          }
          break;

      default:
        // add an argument in the command argv array
        if(cur_command->argc > MAX_ARGS) { 
          pipeline->num_stages++;
          WRITE_ERR(ERROR_START " Command reached max number of arguments ("TO_STRING(MAX_ARGS)")\n"); 
          return PAR_ERROR;}
        cur_command->argv[cur_command->argc] = cur_token.text;
        cur_command->argc++;
        break;
    }
    i++;
  }
  pipeline->num_stages++;
  return PAR_OK;
}

ParseStatus new_command(Pipeline * pipeline){
  if(pipeline->num_stages > MAX_PIPELINE_STAGES){
    WRITE_ERR(ERROR_START " Syntax error: Max pipeline limit reached ("TO_STRING(MAX_PIPELINE_STAGES)")\n");
    return PAR_ERROR;
  }
  Command * command = &(pipeline->stages[pipeline->num_stages]);
  command->argc = 0;
  command->argv[command->argc] = 0;
  return PAR_OK;
}

void append_null(Command *c){
  if(c->argc == 0){
    c->argv[c->argc] = NULL;
  }
  c->argv[c->argc+1] = NULL;
}

Command *index_command(Pipeline * pipeline, int i) {
    if (i < 0 || i > pipeline->num_stages) return NULL;
    return &(pipeline->stages[i]);
}

void reset_pipeline(Pipeline* pipeline){
  for(int i = 0; i < MAX_PIPELINE_STAGES; i++){
    Command c = pipeline->stages[i];
    c.argv[0] = NULL;
    c.argc = 0;
  }
  pipeline->num_stages = 0;
  pipeline->background = 0;
  pipeline->infile = NULL;
  pipeline->outfile = NULL;
  pipeline->append = 0;
  pipeline->background = 0;
}
