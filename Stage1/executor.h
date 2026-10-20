#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

typedef enum {
    EXE_ERROR,
    EXE_OK
} ExecutorStatus;

ExecutorStatus execute_pipeline(Pipeline * pipeline);

#endif