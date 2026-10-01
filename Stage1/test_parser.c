#include "parser.h"
#include <stdio.h>
#include "tokens.h"

// ParseStatus parse_tokens(Tokens *tokens, Pipeline *pipeline);
// typedef struct {
//     char * text;
//     TokenType type;
// } Token;

// typedef struct {
//     char cstring_storage[MAX_INPUT_SIZE + 1];
//     Token token_array[MAX_INPUT_SIZE + 1];
//     int num_tokens;
// } Tokens;

int main() {
    // Token tk = {
    //     .text = "Hello",
    //     .type = TOKEN_WORD
    // }
    // Tokens t = {
    //     .cstring_storage = {0},
    //     .token_array = {
    //         [0] = tk
    //     },
    //     .num_tokens = 10
    // }
    Tokens t;
    create_tokens(&t, "1 2 | > <<<<< 3");
    Pipeline p;
    printf("%d\n",parse_tokens(&t,&p));
    create_tokens(&t, "hi");
    printf("%d\n",parse_tokens(&t,&p));
    return 0;
}