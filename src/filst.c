#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <filst.h>



struct path_component_s *
path_component_init (struct path_component_s * obj,
                     enum path_component_type_e pctype,
                     unsigned int version,
                     int cilen,
                     char ci[])
{
  if (! obj)
    {
      obj = malloc(sizeof(struct path_component_s) + cilen);
    }

  memset(obj, 0, sizeof(*obj));

  obj->typ = pctype;
  obj->len = cilen;
//  obj->ci = ci;
  memcpy(obj->d, ci, obj->len);
  return obj;
}


