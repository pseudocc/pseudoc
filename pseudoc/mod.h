#ifndef PSEUDOC_MODULE_H
#define PSEUDOC_MODULE_H

#include <ptypes.h>
#include <pseudoc/var.h>
#include <pseudoc/func.h>

typedef struct module_s module_t;
typedef struct depend_s depend_t;

struct modules_s {
  char* name;

  funcdef_t* static_funcs;
  uint32_t n_static_funcs;

  vardef_t* static_vars;
  uint32_t n_static_vars;

  module_t* deps;
  uint32_t n_deps;
};

module_t* module_load(char* path);

#endif
