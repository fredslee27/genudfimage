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

/* Duplicate. */
struct path_component_s *
path_component_dup (const struct path_component_s * obj)
{
  struct path_component_s * retval;
  int msize;

  retval = path_component_malloc(obj->len);
  memcpy(retval, obj, msize);
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
struct path_component_s *
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
  struct path_component_s *iter, *prev, *pc;
  int i;
  va_list vp;

  prev = iter = NULL;
  va_start(vp, ncomponents);
  for (i = 0; i < ncomponents; i++)
    {
      pc = va_arg(vp, struct path_component_s*);
      iter = path_component_dup(pc);
      iter->next = NULL;
      if (!prev)
        {
          obj->components = iter;
        }
      else
        {
          prev->next = iter;
        }
//      obj->components[i] = va_arg(vp, struct path_component_s*);
    }
  return obj;
}

struct pathname_s *
pathname_destroy (struct pathname_s * obj)
{
  struct path_component_s *iter, *next;
  iter = obj->components;
  while (iter)
    {
      next = iter->next;
      free(path_component_destroy(iter));
      iter = next;
    }
  return obj;
}

/* Create instance from UDF binary. */
struct pathname_s *
pathname_decode (void * space, int spacelen)
{
  struct pathname_s * retval = NULL;
  int ofs = 0;
  int remainder = spacelen;
  int pclen;
  struct path_component_s *front, *prev;

  front = prev = NULL;
  retval = pathname_malloc(0);
  while (remainder > 0)
    {
      front = path_component_decode(space+ofs, remainder);
      front->next = NULL;
      pclen = sizeof(struct path_component_s) + front->len;
      if (!retval->components)
        {
          retval->components = front;
        }
      else
        {
          prev->next = front;
        }
      prev = front;
      ofs += pclen;
      remainder -= pclen;
    }

  return 0;
}

/* Write into UDF binary format. */
int
pathname_encode (struct pathname_s * obj, void * space, int spacelen)
{
  int used;
  struct path_component_s *iter;


  return 0;
}

/*
   Create new instance of pathname that is in resolved (normalized) form:
   * Starts with a Component Type 2 (root)
   * Other components are Type 5 and not alias (i.e. resolve alias).
*/
struct pathname_s *
pathname_normalize (struct pathname_s * obj)
{
  printf("NOT IMPLEMENTED: pathname_normalize\n");
  return obj;
}








unsigned int tagid_int (enum tagid_e val)
{
  switch (val)
    {
    case TAGID_PVD: return 1;
    case TAGID_AVDP: return 2;
    case TAGID_VDP: return 3;
    case TAGID_IUVD: return 4;
    case TAGID_PD: return 5;
    case TAGID_LVD: return 6;
    case TAGID_USD: return 7;
    case TAGID_TD: return 8;
    case TAGID_LVID: return 9;

    case TAGID_FSD: return 256;
    case TAGID_FID: return 257;
    case TAGID_AED: return 258;
    case TAGID_IE: return 259;
    case TAGID_TE: return 260;
    case TAGID_FE: return 261;
    case TAGID_EAHD: return 262;
    case TAGID_USE: return 263;
    case TAGID_SBD: return 264;
    case TAGID_PIE: return 265;
    case TAGID_EFE: return 266;
    default: return 0;
    }
}

enum tagid_e tagid_enum (unsigned int val)
{
  switch (val)
    {
    case 1: return TAGID_PVD;
    case 2: return TAGID_AVDP;
    case 3: return TAGID_VDP;
    case 4: return TAGID_IUVD;
    case 5: return TAGID_PD;
    case 6: return TAGID_LVD;
    case 7: return TAGID_USD;
    case 8: return TAGID_TD;
    case 9: return TAGID_LVID;

    case 256: return TAGID_FSD;
    case 257: return TAGID_FID;
    case 258: return TAGID_AED;
    case 259: return TAGID_IE;
    case 260: return TAGID_TE;
    case 261: return TAGID_FE;
    case 262: return TAGID_EAHD;
    case 263: return TAGID_USE;
    case 264: return TAGID_SBD;
    case 265: return TAGID_PIE;
    case 266: return TAGID_EFE;
    default: return TAGID_NONE;
    }
}








struct lb_addr_s *
lb_addr_malloc ()
{
  struct lb_addr_s * retval;
  retval = malloc(sizeof(struct lb_addr_s));
  return retval;
}

struct lb_addr_s *
lb_addr_init (struct lb_addr_s * obj,
              unsigned int extenttype,
              unsigned int lbn,
              unsigned int prn)
{
  if (!obj)
    return obj;
  obj->et = extenttype;
  obj->lbn = lbn;
  obj->prn = prn;
  return obj;
}

struct lb_addr_s *
lb_addr_destroy (struct lb_addr_s * obj)
{
  return obj;
}

int
lb_addr_encode (struct lb_addr_s * obj, void * space, int spacelen)
{
}

struct lb_addr_s *
lb_addr_decode (void * space, int spacelen)
{
}

