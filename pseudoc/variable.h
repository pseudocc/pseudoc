#ifndef PSEUDOC_VARIABLE_H
#define PSEUDOC_VARIABLE_H

#include <stdio.h>

typedef struct vardef_s vardef_t;

vardef_t* vardef_read(FILE* f);

#endif
