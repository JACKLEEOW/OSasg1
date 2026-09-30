#include "parser.h"
#include <stddef.h>
#include <stdio.h>

ParseStatus new_command(Pipeline * pipeline, int num_commands);

ParseStatus parse_tokens(Tokens *tokens, Pipeline *pipeline){
  int num_commands = 1;
  pipeline->num_stages = 0;
  pipeline->background = 0;

  if(new_command(pipeline, num_commands) == PAR_ERROR){ return PAR_ERROR;}

  int i = 0; // Iterate over every token
  // For every token in tokens, loop through
  while(i < tokens->num_tokens){
    Token cur_token = tokens->token_array[i];
    Command *cur_command;
    Token next_token; // For redirects

    switch (cur_token.type) {
      case TOKEN_PIPE:
        // Next command in pipeline
        if(new_command(pipeline, num_commands+1) == PAR_ERROR) { return PAR_ERROR;}
        num_commands++;
        break;
      case TOKEN_REDIRECT_IN:
        // set infile as next token
        cur_command = &(pipeline->stages[num_commands - 1]);
        if(i+1 >= tokens->num_tokens) { return PAR_ERROR;}
        i++;
        next_token = tokens->token_array[i];
        cur_command->infile = next_token.text;
        break;
      case TOKEN_REDIRECT_OUT:
        // set outfile as next token
        cur_command = &(pipeline->stages[num_commands - 1]);
        if(i+1 >= tokens->num_tokens) { return PAR_ERROR;}
        i++;
        next_token = tokens->token_array[i];
        cur_command->outfile = next_token.text;
        break;

      default:
        // add an argument in the command argv array
        cur_command = &(pipeline->stages[num_commands - 1]);
        if(cur_command->argc > MAX_ARGS) { return PAR_ERROR;}
        cur_command->argv[cur_command->argc] = cur_token.text;
        cur_command->argc++;
        break;
    }
    i++;
  }
  return PAR_OK;
}

ParseStatus new_command(Pipeline * pipeline, int num_commands){
  if(num_commands > MAX_PIPELINE_STAGES){
    return PAR_ERROR;
  }
  pipeline->num_stages++;
  Command * command = &(pipeline->stages[num_commands - 1]);
  command->argc = 0;
  command->argv[command->argc] = 0;
  return PAR_OK;
}

Command *index_command(Pipeline * pipeline, int i) {
    if (i < 0 || i >= pipeline->num_stages) return NULL;
    return &(pipeline->stages[i]);
}

void test_parse(Pipeline *pipeline){

  printf("Number of Commands: %d\n", pipeline->num_stages);
  int i = 0;
  Command *c;

  // For number of command structs in the pipeline
  while(i < pipeline->num_stages){
    printf("Command array : ");
    c = index_command(pipeline, i);
    unsigned int j;

    // For number of arguments in the command
    for (j = 0; j < c->argc; j++){
      printf("'%s', ",c->argv[j]);
    }

    // Print other values of the command
    printf("Argc: %d, ", c->argc);
    if(c->infile) { printf("Infile: %s, ", c->infile);}
    if(c->outfile) { printf("Outfile: %s, ", c->outfile);}
    printf("Append: %d, ", c->append);

    i++;
    printf("\n");
  }
}