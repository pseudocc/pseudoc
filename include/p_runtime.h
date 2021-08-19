/**
 * @file p_abort.h
 * @brief load declarations and statements
 * and then run all the statements
 * 
 * @author Yu Ze <pseudoc@163.com>
 * @date Sun Aug 15 2021 22:07:41 GMT+0800 (China Standard Time)
 **/

#ifndef PSEUDO_RUNTIME_H
#define PSEUDO_RUNTIME_H

#include <stdio.h>

typedef struct runtime runtime_t;

static void load_library(runtime_t* runtime, FILE* f);

static void safe_exit(runtime_t* runtime);

runtime_t* pseudo_init(char* manifest);

void pseudo_run(runtime_t* runtime, char** argv, int argc);

#endif
