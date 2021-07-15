/**
 * @file p_abort.h
 * @brief system management things for pseudoc
 * 
 * @author Yu Ze <pseudoc@163.com>
 * @date Wed Jul 14 2021 21:49:19 GMT+0800 (China Standard Time)
 **/

#ifndef PSEUDO_SYSTEM_H
#define PSEUDO_SYSTEM_H

#include <stdlib.h>

#define MINOR_PROBLEM 1
#define MAJOR_PROBLEM 2
#define COMMAND_NOT_FOUND 127
#define ABORT_SIGILL 132
#define ABORT_SIGTRAP 133
#define ABORT_SIGABRT 134
#define ABORT_SIGFPE 136
#define OUT_OF_MEMORY 137
#define ABORT_SIGBUS 138

/**
 * normal exit, trigger some hooks
 **/
void pseudo_exit(int code);

/**
 * quick exit, only trigger qexit hook
 **/
void pseudo_qexit(int code);

#endif