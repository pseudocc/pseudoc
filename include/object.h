#ifndef PSEUDOC_OBJECT_H
#define PSEUDOC_OBJECT_H

#include "primitive.h"

#define PSEUDOC_OBJECT 4

typedef struct p_object p_object;

p_int     objget_int(int nhash);
p_byte    objget_byte(int nhash);
p_float   objget_float(int nhash);
p_array   objget_array(int nhash);
p_object  objget_object(int nhash);

#endif
