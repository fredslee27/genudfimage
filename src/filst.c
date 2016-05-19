#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <filst.h>


#include "conv.c"



/*
Naming convention:

_malloc - allocate space from heap, with any relevant count value.
_free - blindly deallocate.
_init - constructor, populate from parameters.
_destroy - destructor, handling free inner members.
_encode - serialize to UDF binary
_decode - deserialize from UDF binary
*/



/* malloc to accomodate a given Component Identifier length. */
struct path_component_s *
path_component_malloc (int cilen)
{
  struct path_component_s * retval;

  int msize = sizeof(struct path_component_s) + cilen + 1;
  /* Plus 1 for terminating NUL; eases use with str*(). */
  retval = (struct path_component_s *)malloc(msize);
  memset(retval, 0, msize);
  retval->len = cilen;
  return retval;
}


/* corresponding free. */
void
path_component_free (const struct path_component_s * obj)
{
  free((void*)obj);
}


/* Populate instance from parameters. */
struct path_component_s *
path_component_init (struct path_component_s * obj,
                     enum path_component_type_e pctype,
                     unsigned int version,
                     int cilen,
                     char ci[])
{
  if (! obj)
    obj = path_component_malloc(cilen);
  if (! obj)
    {
      abort();
    }

  obj->typ = pctype;
  obj->vers = version;
  /* Bounds-check is far too primitive; either assumes the object passed in is
   * already large enough, or was allocated in here for exactly the right size.
   */
  obj->len = cilen;
//  obj->ci = ci;
  memcpy(obj->d, ci, obj->len);
  return obj;
}


struct path_component_s *
path_component_destroy (struct path_component_s * obj)
{
  return obj;
}


/* Create read-only instance (on heap) from parameters. */
const struct path_component_s *
path_component_make (enum path_component_type_e pctype,
                     unsigned int version,
                     int cilen,
                     char ci[])
{
  if (cilen < 0)
    cilen = strlen(ci);
  struct path_component_s * obj = path_component_malloc(cilen);

  obj->raw = NULL;
  obj->typ = pctype;
  obj->vers = version;
  memcpy(obj->d, ci, cilen);
  obj->d[cilen] = '\0';

  return obj;
}


void
path_component_dump (const struct path_component_s * obj)
{
  printf("path_component(@%p) = {\n", obj);
  printf("  ct = %d\n", obj->typ);
  printf("  cfvn = %d\n", obj->vers);
  printf("  L_CI = %d\n", obj->len);
  if (obj->len)
    printf("  ci = \"%s\"\n", obj->d);
  else
    puts("  ci = \"\"");
  puts("}");
}


/*
   Create instance (decode) from UDF binary format.
*/
const struct path_component_s *
path_component_decode (void * space, int spacelen)
{
  struct path_component_s * obj;
  layoutvalue_t contents[5] = { 0, };

  udf_decode(space, udf_path_component, contents);

  obj = path_component_malloc(contents[1].word);
  obj->raw = space;
  obj->typ = contents[0].word;
  obj->len = contents[1].word;
  obj->vers = contents[2].word;
  memcpy(obj->d, contents[3].ptr, obj->len);
  return obj;
}


/*
   Write (encode) into UDF binary format.
*/
int
path_component_encode (struct path_component_s * obj, void * space, int spacelen)
{
  layoutvalue_t contents[5] = { 0, };
  int retval;

  contents[0].word = obj->typ;
  contents[1].word = obj->len;
  contents[2].word = obj->vers;
  contents[3].ptr = 0;
  contents[4].word = 4 + obj->len;
  udf_encode(space, spacelen, udf_path_component, contents);
  memcpy(((uint8_t*)space)+4, obj->d, obj->len);
  retval = 4 + obj->len;

  return retval;
}




struct pathname_s *
pathname_malloc (int ncomponents)
{
  struct pathname_s * retval;
  int msize = 0;

  msize = (ncomponents * sizeof(struct path_component_s));
  msize += sizeof(struct pathname_s);

  retval = (struct pathname_s*)malloc(msize);
  return retval;
}

void
pathname_free (struct pathname_s * obj)
{
  free(obj);
}

/*
   Initialize Pathname from arguments of struct path_component_s*
*/
struct pathname_s *
pathname_init (struct pathname_s * obj, int ncomponents, ...)
{
  struct path_component_s * iter;
  int i;
  va_list vp;

  vp = va_start(ncomponents);
  for (i = 0; i < ncomponents; i++)
    {
      obj->components[i] = va_arg(vp, struct path_component_s*);
    }
  return obj;
}

struct pathname_s *
pathname_destroy (struct pathname_s * obj)
{
  return obj;
}

/* Create instance from UDF binary. */
struct pathname_s *
pathname_encode (void * space, int spacelen)
{
  return 0;
}

/* Write into UDF binary format. */
int
pathname_decode (struct pathname_s * obj, void * space, int spacelen)
{
  return 0;
}




