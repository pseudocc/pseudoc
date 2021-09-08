#ifndef PSEUDOC_MODULE_H
#define PSEUDOC_MODULE_H

#include <pseudoc/types.h>
#include <pseudoc/builtin.h>
#include <pseudoc/call.h>

typedef struct module_s module_t;

typedef struct module_header_s module_header_t;

#define HASH_SIZE 256
static module_t* module_map[HASH_SIZE];

static bmethod_t* get_bmethod(char* name, unsigned op); 
void load_module(char* path);

#endif
