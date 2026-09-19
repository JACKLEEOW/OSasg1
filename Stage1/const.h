#ifndef CONST_H
#define CONST_H

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define MAX_INPUT_SIZE 256
#define ERROR_NUMBER -1

#define TO_STRING_HELPER(X) #X
#define TO_STRING(X) TO_STRING_HELPER(X)

#define RED   "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#endif