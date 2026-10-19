#include "parser.h"
#include <stdio.h>
#include "tokens.h"
//#include "test_token.h"
#include <string.h>
#include "const.h"
#include "test_parser.h"


void test_parse(Pipeline *pipeline){
  printf("Pipeline test:\n");
  printf("Number of Commands: %d\n", pipeline->num_stages);
    if(pipeline->infile) { printf("Infile: %s, \n", pipeline->infile);} else { printf("Infile: NULL, \n");}
    if(pipeline->outfile) { printf("Outfile: %s, \n", pipeline->outfile);} else { printf("Outfile: NULL, \n");}
    printf("Append: %d, \n", pipeline->append);
    printf("Background: %d, \n", pipeline->background);
  int i = 0;
  Command *c;

  // For number of command structs in the pipeline
  while(i < pipeline->num_stages){
    printf("Command array: \n");
    c = index_command(pipeline, i);
    unsigned int j;

    // For number of arguments in the command
    for (j = 0; j < c->argc; j++){
      printf("'%s', \n",c->argv[j]);
    }

    // Print other values of the command
    printf("Argc: %d, \n", c->argc);

    i++;
    //printf("\n");
  }
}

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
        //printf("Token input:\n");
        // test_tokens(t);
        printf("Pipeline result:\n");
        test_parse(&p);
        //printf("Pipeline expected:\n");
        //test_parse(expected);
        return PAR_ERROR;
    }
    return PAR_OK;
}

int main() {
    initialize_test_parser_data();
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
    printf("Testing redirect fail case: '< |'\n");
    if(test_parser(&redirect_in_fail_token, &expected_redirect_in_fail, 1) == PAR_ERROR){
        printf("Redirect fail test failed\n");
        errors++;
    }
    printf("Testing redirect out fail case: '>'\n");
    if(test_parser(&redirect_out_fail_token1, &expected_redirect_out_fail, 1) == PAR_ERROR){
        printf("Redirect out fail test 1 failed\n");
        errors++;
    }
    printf("Testing redirect out fail case: '> >'\n");
    if(test_parser(&redirect_out_fail_token2, &expected_redirect_out_fail, 1) == PAR_ERROR){
        printf("Redirect out fail test 2 failed\n");
        errors++;
    }
    if(test_parser(&background_token, &expected_background, 0) == PAR_ERROR){
        printf("Background test failed\n");
        errors++;
    }
    //printf("Testing background fail case: 'Hello & Hello'\n");
    if(test_parser(&background_token_fail, &expected_background, 0) == PAR_ERROR){
        printf("Background fail test failed\n");
        errors++;
    }
    //if(test_parser(&fullpipeline_token, &expected_fullpipeline, 0) == PAR_ERROR){
    //    printf("Full pipeline test failed\n"); 
    //    errors++;
    //}
    if(errors > 0) {
        printf("Total errors: %d\n", errors);
    } else {
        printf("All parser tests passed\n");
    }
    return 0;
}