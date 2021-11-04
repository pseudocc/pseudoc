#ifndef PSEUDOC_FUNCTION_H
#define PSEUDOC_FUNCTION_H

#include <stdio.h>

typedef struct funcdef_s funcdef_t;

funcdef_t* fundef_read(FILE* f);

#endif
