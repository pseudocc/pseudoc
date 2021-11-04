#ifndef PSEUDOC_H
#define PSEUDOC_H

#include <ptypes.h>

extern int32_t perrno;

typedef struct module_s module_t;

void pseudoc_init(int32_t porder);
module_t* pseudoc_load(char* module);
int32_t pseudoc_run(module_t* main);

#endif
