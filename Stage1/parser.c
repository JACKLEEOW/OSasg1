#include "parser.h"
#include <stddef.h>
#include <stdio.h>

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
        pipeline->num_stages++;
        if(new_command(pipeline) == PAR_ERROR) { 
          pipeline->num_stages--;
          return PAR_ERROR;
        }
        break;
      case TOKEN_REDIRECT_IN:
        // set infile as next token
        if(i+1 >= tokens->num_tokens) { return PAR_ERROR;}
        i++;
        // next_token = tokens->token_array[i];
        next_token = index_tokens(tokens, i);
        if(next_token->type != TOKEN_WORD){
          return PAR_ERROR;
        }
        pipeline->infile = next_token->text;
        break;
      case TOKEN_REDIRECT_OUT:
        // set outfile as next token
        if(i+1 >= tokens->num_tokens) { return PAR_ERROR;}
        i++;
        // next_token = tokens->token_array[i];
        next_token = index_tokens(tokens, i);
        if(next_token->type != TOKEN_WORD){
          return PAR_ERROR;
        }
        pipeline->outfile = next_token->text;
        break;

      default:
        // add an argument in the command argv array
        if(cur_command->argc > MAX_ARGS) { return PAR_ERROR;}
        cur_command->argv[cur_command->argc] = cur_token.text;
        cur_command->argc++;
        break;
    }
    i++;
  }
  // pipeline->num_stages++;
  return PAR_OK;
}

ParseStatus new_command(Pipeline * pipeline){
  if(pipeline->num_stages > MAX_PIPELINE_STAGES){
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
  //int i = 0;
  for(int i = 0; i < MAX_PIPELINE_STAGES; i++){
    Command com = pipeline->stages[i];
    com.argv[0] = NULL;
    com.argc = 0;
  }
  pipeline->num_stages = 0;
  pipeline->background = 0;
  pipeline->infile = NULL;
  pipeline->outfile = NULL;
}


void test_parse(Pipeline *pipeline){

  printf("Number of Commands: %d\n", pipeline->num_stages + 1);
    if(pipeline->infile) { printf("Infile: %s, \n", pipeline->infile);}
    if(pipeline->outfile) { printf("Outfile: %s, \n", pipeline->outfile);}
  int i = 0;
  Command *c;

  // For number of command structs in the pipeline
  while(i <= pipeline->num_stages){
    printf("Command array : \n");
    c = index_command(pipeline, i);
    unsigned int j;

    // For number of arguments in the command
    for (j = 0; j < c->argc; j++){
      printf("'%s', \n",c->argv[j]);
    }

    // Print other values of the command
    printf("Argc: %d, \n", c->argc);
    //rintf("Append: %d, \n", c->append);

    i++;
    printf("\n");
  }
}