#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "filst.h"

#include "conv.c"



/*
Naming convention:

_malloc - allocate space from heap, with any relevant count value.
_init - constructor, populate from parameters.
_destroy - destructor, handling free inner members.
_free - blindly destroy then deallocate.
_decode - deserialize from UDF binary
_encode - serialize to UDF binary
_cmp - comparator
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


/* destroy and deallocate. */
void
path_component_free (struct path_component_s * obj)
{
  free(path_component_destroy(obj));
}


/*
   Create instance (decode) from UDF binary format.
*/
struct path_component_s *
path_component_decode (void * space, int spacelen)
{
  struct path_component_s * obj;
  layoutvalue_t contents[5] = { 0, };

  udf_decode(space, spacelen, udf_path_component, contents);

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


/*
   Comparison between PathComponent, sorting sense.
   Return 0: 1 = a is greater than b
            -1 = a is less than b
             0 = a is equal to b
*/
int
path_component_cmp (const struct path_component_s * a,
		    const struct path_component_s * b)
{
  int ia, ib;
  const char *pa, *pb;

  /* if one is empty. */
  if ((a->len == 0) && (b->len > 0))
    return -1;
  if ((a->len > 0) && (b->len == 0))
    return 1;
  ia = ib = 0;
  pa = a->d;
  pb = b->d;
  while ((ia < a->len) && (ib < b->len))
    {
      pa = a->d + ia;
      pb = b->d + ib;
      if (*pa < *pb) return -1;  /* asciibetically lesser. */
      if (*pa > *pb) return 1;  /* asciibetically greater. */
      /* Still equal; proceed to next. */
      ia++; ib++;
    }
  /* One or both ended. */
  if (a->len < b->len) return -1;  /* a is shorter -> lesser. */
  if (a->len > b->len) return 1;  /* a is longer -> greater. */
  /* equal content, equal length.  Compare versions. */
  if (a->vers < b->vers) return -1;  /* a is older -> lesser. */
  if (a->vers > b->vers) return 1;  /* a is newer -> greater. */

  /* still equal, nothing else to compare. */
  return 0;
}


int
path_component_str (const struct path_component_s *obj,
		    char buf[],
		    int buflen)
{
  int n = 0;
  n += snprintf(buf, buflen, "struct path_component_s _%p = {\n\
  .ct=%d,\n\
  .cfvn=%d,\n\
  .L_CI=%d,\n", obj, obj->typ, obj->vers, obj->len);
  if (obj->len)
    {
      n += snprintf(buf+n, buflen-n, "  .ci=\"%s\",\n", obj->d);
    }
  else
    {
      n += snprintf(buf+n, buflen-n, "  .ci=\"\",\n");
    }
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}


void
path_component_dump (const struct path_component_s * obj)
{
  char buf[512];
  path_component_str(obj, buf, sizeof(buf));
  puts(buf);
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
              unsigned int lbn,
              unsigned int prn)
{
  if (!obj)
    return obj;
  obj->lbn = lbn;
  obj->prn = prn;
  return obj;
}

struct lb_addr_s *
lb_addr_destroy (struct lb_addr_s * obj)
{
  return obj;
}

void
lb_addr_free (struct lb_addr_s * obj)
{
  free(lb_addr_destroy(obj));
}

struct lb_addr_s *
lb_addr_decode (void * space, int spacelen)
{
  struct lb_addr_s * retval = NULL;
  layoutvalue_t contents[2] = { 0, };

  udf_decode(space, spacelen, udf_lb_addr, contents);

  retval = lb_addr_malloc();
  retval->lbn = contents[0].word;
  retval->prn = contents[1].word;
  return retval;
}

int
lb_addr_encode (struct lb_addr_s * obj, void * space, int spacelen)
{
  int retval = 0;
  layoutvalue_t contents[2] = {
      obj->lbn,
      obj->prn,
  };  /* Length implied in layout descriptor. */
  retval = udf_encode(space, spacelen, udf_lb_addr, contents);
  return retval;
}

int
lb_addr_cmp (struct lb_addr_s * a, struct lb_addr_s * b)
{
  if (a->prn < b->prn) return -1;
  if (a->prn > b->prn) return 1;
  if (a->lbn < b->lbn) return -1;
  if (a->lbn > b->lbn) return 1;
  return 0;
}

int
lb_addr_str (const struct lb_addr_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf, buflen, "struct lb_addr _%p = {\n\
  .lbn=%u,\n\
  .prn=%u,\n\
};", obj, obj->lbn, obj->prn);
  return n;
}

void
lb_addr_dump (const struct lb_addr_s * obj)
{
  char buf[512];
  lb_addr_str(obj, buf, sizeof(buf));
  puts(buf);
}






struct tag_s *
tag_init (struct tag_s * obj, enum tagid_e tagid, unsigned int version, unsigned int serial, unsigned int tagloc)
{
  if (!obj) return obj;

  obj->tagid = tagid;
  obj->vers = version;
  obj->serial = serial;
  obj->tagloc = tagloc;
}

unsigned int
tag_check_sum (struct tag_s * obj)
{
  uint32_t accum = 0;
  unsigned int tagval = tagid_int(obj->tagid);
  // 16b checksum then collapse to 8b.
  accum += tagval + obj->vers + obj->serial + obj->crc + obj->crclen;
  accum += ((obj->tagloc >> 16) & 0xffff) + (obj->tagloc & 0xffff);
  // sum byte0 and byte1, subtracting overflow of byte2.
  accum = ((accum >> 8) & 0xff) + (accum & 0xff) - (accum >> 16);
  unsigned int retval = accum & 0xff;
  return retval;
}

int
tag_str (const struct tag_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct tag_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .tagid=%u,\n", tagid_int(obj->tagid));
  n += snprintf(buf+n, buflen-n, "  .vers=%u,\n", obj->vers);
  n += snprintf(buf+n, buflen-n, "  .checksum=%u,\n", obj->checksum);
  n += snprintf(buf+n, buflen-n, "  .serial=%u,\n", obj->serial);
  n += snprintf(buf+n, buflen-n, "  .crc=%u,\n", obj->crc);
  n += snprintf(buf+n, buflen-n, "  .crclen=%u,\n", obj->crclen);
  n += snprintf(buf+n, buflen-n, "  .tagloc=%u,\n", obj->tagloc);
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
tag_dump (const struct tag_s * obj)
{
  char buf[512];
  tag_str(obj, buf, sizeof(buf));
  puts(buf);
}



unsigned int
icb_file_type_int (enum icb_file_type_e ft)
{
  switch (ft)
    {
    case ICBFT_OTHER: return 0;
    case ICBFT_USE: return 1;
    case ICBFT_PIE: return 2;
    case ICBFT_IE: return 3;
    case ICBFT_DIR: return 4;
    case ICBFT_REG: return 5;
    case ICBFT_BLK: return 6;
    case ICBFT_CHR: return 7;
    case ICBFT_EA: return 8;
    case ICBFT_FIFO: return 9;
    case ICBFT_SOCK: return 10;
    case ICBFT_TE: return 11;
    case ICBFT_SYMLINK: return 12;
    case ICBFT_STREAM: return 13;
    case ICBFT_248: return 248;
    case ICBFT_249: return 249;
    case ICBFT_250: return 250;
    case ICBFT_251: return 251;
    case ICBFT_252: return 252;
    case ICBFT_253: return 253;
    case ICBFT_254: return 254;
    case ICBFT_255: return 255;
    default: return 0;
    }
}

enum icb_file_type_e
icb_file_type_enum (unsigned int ftval)
{
  switch (ftval)
    {
    case 0: return ICBFT_OTHER;
    case 1: return ICBFT_USE;
    case 2: return ICBFT_PIE;
    case 3: return ICBFT_IE;
    case 4: return ICBFT_DIR;
    case 5: return ICBFT_REG;
    case 6: return ICBFT_BLK;
    case 7: return ICBFT_CHR;
    case 8: return ICBFT_EA;
    case 9: return ICBFT_FIFO;
    case 10: return ICBFT_SOCK;
    case 11: return ICBFT_TE;
    case 12: return ICBFT_SYMLINK;
    case 13: return ICBFT_STREAM;

    case 248: return ICBFT_248;
    case 249: return ICBFT_249;
    case 250: return ICBFT_250;
    case 251: return ICBFT_251;
    case 252: return ICBFT_252;
    case 253: return ICBFT_253;
    case 254: return ICBFT_254;
    case 255: return ICBFT_255;
    }
}

static const char *
icb_file_type_str (enum icb_file_type_e ft)
{
  switch (ft)
    {
    case ICBFT_OTHER: return "(other)";
    case ICBFT_USE: return "UnallocatedSpaceEntry";
    case ICBFT_PIE: return "PartitionIntegrityEntry";
    case ICBFT_IE: return "IndirectEntry";
    case ICBFT_DIR: return "Directory";
    case ICBFT_REG: return "Regular";
    case ICBFT_BLK: return "BlockSpecial";
    case ICBFT_CHR: return "CharacterSpecial";
    case ICBFT_EA: return "ExtendedAttribute";
    case ICBFT_FIFO: return "NamedFifo";
    case ICBFT_SOCK: return "UnixSocket";
    case ICBFT_TE: return "TerminalEntry";
    case ICBFT_SYMLINK: return "SymbolicLink";
    case ICBFT_STREAM: return "Stream";
    case ICBFT_248: return "Type248";
    case ICBFT_249: return "Type249";
    case ICBFT_250: return "Type250";
    case ICBFT_251: return "Type251";
    case ICBFT_252: return "Type252";
    case ICBFT_253: return "Type253";
    case ICBFT_254: return "Type254";
    case ICBFT_255: return "Type255";
    default: return "(unk)";
    }
}

struct icbtag_s *
icbtag_init (struct icbtag_s * obj,
	     unsigned int previous_recorded_number_of_direct_entries,
	     unsigned int strategy_type,
	     unsigned int strategy_parameter,
	     unsigned int maximum_number_of_entries,
	     enum icb_file_type_e file_type,
	     const struct lb_addr_s * parent_icb_location,
	     unsigned int allocation_descriptor_type)
{
  if (!obj) return obj;

  obj->prnde = previous_recorded_number_of_direct_entries;
  obj->st = strategy_type;
  obj->sp = strategy_parameter;
  obj->mne = maximum_number_of_entries;
  obj->ft = file_type;
  obj->picbl = *(parent_icb_location);
  memset(&(obj->flags), 0, sizeof(obj->flags));
  obj->flags.adtype = allocation_descriptor_type;
  return obj;
}

int
icbtag_str (const struct icbtag_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct icbtag_s _%p = {", obj);
  n += snprintf(buf+n, buflen-n, "  .prnde = %u,\n", obj->prnde);
  n += snprintf(buf+n, buflen-n, "  .st = %u,\n", obj->st);
  n += snprintf(buf+n, buflen-n, "  .sp = %u,\n", obj->sp);
  n += snprintf(buf+n, buflen-n, "  .mne = %u,\n", obj->mne);
  n += snprintf(buf+n, buflen-n, "  .ft = %u, /* %s */\n",
		icb_file_type_int(obj->ft), icb_file_type_str(obj->ft));
  char picbl[16];
  lb_addr_str(&(obj->picbl), picbl, sizeof(picbl));
  n += snprintf(buf+n, buflen-n, "  .picbl = %s,\n", picbl);
  n += snprintf(buf+n, buflen-n, "  .flags = (%c*%c%c%c%c%c%c%c%c%c%c%c)",
		"slxd"[obj->flags.adtype],
		obj->flags.dirsort ? 'O': 'o',
		obj->flags.nonreloc ? 'N': 'n',
		obj->flags.archive ? 'A' : 'a',
		obj->flags.setuid ? 'U' : 'u',
		obj->flags.setgid ? 'G' : 'g',
		obj->flags.sticky ? 'T' : 't',
		obj->flags.contiguous ? 'C' : 'c',
		obj->flags.system ? 'Y' : 'y',
		obj->flags.transformed ? 'X' : 'x',
		obj->flags.multiversion ? 'V' : 'v',
		obj->flags.stream ? 'R' : 'r');
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
icbtag_dump (const struct icbtag_s * obj)
{
  char buf[512];
  icbtag_str(obj, buf, sizeof(buf));
  puts(buf);
}





