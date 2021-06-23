#ifndef PSEUDOC_OBJECT_H
#define PSEUDOC_OBJECT_H

#include "primitive.h"

#define PSEUDOC_OBJECT 4

typedef struct p_object p_object;

p_int     objget_int(p_object* o, int nhash);
p_byte    objget_byte(p_object* o, int nhash);
p_float   objget_float(p_object* o, int nhash);
p_array   objget_array(p_object* o, int nhash);
p_object  objget_object(p_object* o, int nhash);

#endif
