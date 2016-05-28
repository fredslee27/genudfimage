#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "filst.h"
#include "basics.h"
#include "conv.h"
#include "util.h"



/*
Naming convention:

_malloc - allocate space from heap, with any relevant count value.
//_copy - copy constructor (non-NULL self) or duplicator (NULL self).
_copy - duplicator.
_init - constructor, populates from parameters.
_destroy - destructor, handles free() of inner members.
_free - blindly destroys then deallocates.
_decode - deserialize from UDF binary.
_encode - serialize to UDF binary.
_len - number of bytes needed for UDF binary (i.e. minimum space for _encode()).
_cmp - comparator, returns <0, 0, >0 depending on <,==,> sense.
_repr - generate string representation.
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
  if (!obj) return NULL;
  int msize = sizeof(struct path_component_s) + obj->len;

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


#if 0
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
#endif //0


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
path_component_decode (const uint8_t * space, int spacelen)
{
  struct path_component_s * obj = NULL;
  layoutvalue_t contents[5] = { 0, };

  udf_decode(space, spacelen, udf_path_component, contents);

  if (!obj) obj = path_component_malloc(contents[1].word);
  if (!obj || !space || !spacelen) return obj;

  obj->raw = (void*)space;
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
path_component_encode (const struct path_component_s * obj, uint8_t * space, int spacelen)
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
path_component_repr (const struct path_component_s *obj,
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
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}


void
path_component_dump (const struct path_component_s * obj)
{
  char buf[512];
  int n = path_component_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
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
  memset(retval, 0, sizeof(*retval));
  return retval;
}

void
pathname_free (struct pathname_s * obj)
{
  struct path_component_s *iter, *next;

  for (iter = obj->components; iter != NULL; iter = next)
    {
      next = iter->next;
      path_component_free(iter);
    }

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
  struct pathname_s * obj = NULL;
  int ofs = 0;
  int remainder = spacelen;
  int pclen;
  struct path_component_s *front, *prev;

  if (!obj) obj = pathname_malloc(0);
  if (!obj || !space || !spacelen) return obj;

  front = prev = NULL;
  while (remainder > 0)
    {
      front = path_component_decode(space+ofs, remainder);
      front->next = NULL;
      pclen = sizeof(struct path_component_s) + front->len;
      if (!obj->components)
        {
          obj->components = front;
        }
      else
        {
          prev->next = front;
        }
      prev = front;
      ofs += pclen;
      remainder -= pclen;
    }

  return obj;
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







const char * tagid_name (enum tagid_e tagid)
{
  switch (tagid)
    {
			  /*  123456789_123456789_12 */
    case TAGID_PVD:   return "PrimaryVolumeDesc";
    case TAGID_AVDP:  return "AnchorVolumeDescPtr";
    case TAGID_VDP:   return "VolumeDescPointer";
    case TAGID_IUVD:  return "ImplUseVolumeDesc";
    case TAGID_PD:    return "PartitionDesc";
    case TAGID_LVD:   return "LogicalVolumeDesc";
    case TAGID_USD:   return "UnallocatedSpaceDesc";
    case TAGID_TD:    return "TerminatingDesc";
    case TAGID_LVID:  return "LogicalVolumeIntegrityDesc";

    case TAGID_FSD:   return "FileSetDesc";
    case TAGID_FID:   return "FileIdentifierDesc";
    case TAGID_AED:   return "AllocateExtentDesc";
    case TAGID_IE:    return "IndirectEntry";
    case TAGID_TE:    return "TerminalEntry";
    case TAGID_FE:    return "FileEntry";
    case TAGID_EAHD:  return "ExtdAttrHeaderDesc";
    case TAGID_USE:   return "UnallocatedSpaceEntry";
    case TAGID_SBD:   return "SpaceBitmapDesc";
    case TAGID_PIE:   return "PartitionIntegrityEntry";
    case TAGID_EFE:   return "ExtendedFileEntry";
    default: return "(unk)";
    }
}

unsigned int tagid_int (enum tagid_e tagid)
{
  switch (tagid)
    {
    case TAGID_PVD:   return 1;
    case TAGID_AVDP:  return 2;
    case TAGID_VDP:   return 3;
    case TAGID_IUVD:  return 4;
    case TAGID_PD:    return 5;
    case TAGID_LVD:   return 6;
    case TAGID_USD:   return 7;
    case TAGID_TD:    return 8;
    case TAGID_LVID:  return 9;

    case TAGID_FSD: return 256;
    case TAGID_FID: return 257;
    case TAGID_AED: return 258;
    case TAGID_IE:  return 259;
    case TAGID_TE:  return 260;
    case TAGID_FE:  return 261;
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
  memset(retval, 0, sizeof(*retval));
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
  struct lb_addr_s * obj = NULL;
  layoutvalue_t contents[3] = { 0, };

  if (!obj) obj = lb_addr_malloc();
  if (!obj || !space || !spacelen) return obj;

  udf_decode(space, spacelen, udf_lb_addr, contents);

  obj->lbn = contents[0].word;
  obj->prn = contents[1].word;
  return obj;
}

int
lb_addr_encode (const struct lb_addr_s * obj, void * space, int spacelen)
{
  int retval = 0;
  layoutvalue_t contents[3] = { 0, };

  contents[0].word = obj->lbn;
  contents[1].word = obj->prn;
  contents[2].word = 6;

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
lb_addr_repr (const struct lb_addr_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct lb_addr_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .lbn = %u,\n", obj->lbn);
  n += snprintf(buf+n, buflen-n, "  .prn = %u,\n", obj->prn);
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}

void
lb_addr_dump (const struct lb_addr_s * obj)
{
  char buf[512];
  int n = lb_addr_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}






struct layoutfield_s udf_tag[] = {
    { 0, 0, "tagid", LAYOUT_UINT16 },
    { 0, 2, "vers", LAYOUT_UINT16 },
    { 0, 4, "checksum", LAYOUT_UINT16 },
    { 0, 5, "reserved", LAYOUT_RESERVED },
    { 0, 6, "serial", LAYOUT_UINT16 },
    { 0, 8, "crc", LAYOUT_UINT16 },
    { 0, 10, "crclen", LAYOUT_UINT16 },
    { 0, 12, "tagloc", LAYOUT_UINT32 },
    { 0, 16, 0, LAYOUT_END },
};

struct tag_s *
tag_malloc ()
{
  struct tag_s * retval = malloc(sizeof(struct tag_s));
  memset(retval, 0, sizeof(*retval));
  return retval;
}

struct tag_s *
tag_destroy (struct tag_s * obj)
{
  return obj;
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

void
tag_free (struct tag_s * obj)
{
  free(tag_destroy(obj));
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

struct tag_s *
tag_decode (struct tag_s * obj, const uint8_t * space, int spacelen)
{
//  struct tag_s * obj = NULL;

  layoutvalue_t contents[8] = { 0, };

  if (!obj) obj = malloc(sizeof(*obj));
  if (!obj || !space || !spacelen) return obj;
  memset(obj, 0, sizeof(*obj));

  int n = udf_decode(space, spacelen, udf_tag, contents);
  obj->tagid = tagid_enum(contents[0].word);
  obj->vers = contents[1].word;
  obj->checksum = contents[2].word;
  obj->serial = contents[4].word;
  obj->crc = contents[5].word;
  obj->crclen = contents[6].word;
  obj->tagloc = contents[7].word;
  unsigned int declen = contents[8].word;

  return obj;
}

int
tag_encode (const struct tag_s * obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[9] = { 0, };
  if (!obj) return 0;

  contents[0].word = tagid_int(obj->tagid);
  contents[1].word = obj->vers;
  contents[2].word = obj->checksum;
//  contents[3].word = reserved;
  contents[4].word = obj->serial;
  contents[5].word = obj->crc;
  contents[6].word = obj->crclen;
  contents[7].word = obj->tagloc;
  contents[8].word = 16;

  int retval = udf_encode(space, spacelen, udf_tag, contents);

  return retval;
}

int
tag_repr (const struct tag_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct tag_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .tagid=%u /* %s */,\n", tagid_int(obj->tagid), tagid_name(obj->tagid));
  n += snprintf(buf+n, buflen-n, "  .vers=%u,\n", obj->vers);
  n += snprintf(buf+n, buflen-n, "  .checksum=%u,\n", obj->checksum);
  n += snprintf(buf+n, buflen-n, "  .serial=%u,\n", obj->serial);
  n += snprintf(buf+n, buflen-n, "  .crc=%u,\n", obj->crc);
  n += snprintf(buf+n, buflen-n, "  .crclen=%u,\n", obj->crclen);
  n += snprintf(buf+n, buflen-n, "  .tagloc=%u,\n", obj->tagloc);
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}

void
tag_dump (const struct tag_s * obj)
{
  char buf[512];
  int n = tag_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
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
icb_file_type_name (enum icb_file_type_e ft)
{
  switch (ft)
    {
    case ICBFT_OTHER: return "(other)";
    case ICBFT_USE:   return "UnallocatedSpaceEntry";
    case ICBFT_PIE:   return "PartitionIntegrityEntry";
    case ICBFT_IE:    return "IndirectEntry";
    case ICBFT_DIR:   return "Directory";
    case ICBFT_REG:   return "Regular";
    case ICBFT_BLK:   return "BlockSpecial";
    case ICBFT_CHR:   return "CharacterSpecial";
    case ICBFT_EA:    return "ExtendedAttribute";
    case ICBFT_FIFO:  return "NamedFifo";
    case ICBFT_SOCK:  return "UnixSocket";
    case ICBFT_TE:    return "TerminalEntry";
    case ICBFT_SYMLINK: return "SymbolicLink";
    case ICBFT_STREAM: return "Stream";
    case ICBFT_248:   return "Type248";
    case ICBFT_249:   return "Type249";
    case ICBFT_250:   return "Type250";
    case ICBFT_251:   return "Type251";
    case ICBFT_252:   return "Type252";
    case ICBFT_253:   return "Type253";
    case ICBFT_254:   return "Type254";
    case ICBFT_255:   return "Type255";
    default: return "(unk)";
    }
}



struct layoutfield_s udf_icbtag[] = {
    { 0, 0, "prnde", LAYOUT_UINT32 },
    { 0, 4, "st", LAYOUT_UINT16 },
    { 0, 6, "sp", LAYOUT_UINT16 },
    { 0, 8, "mne", LAYOUT_UINT16 },
    { 0, 10, "reserved", LAYOUT_RESERVED },
    { 0, 11, "ft", LAYOUT_UINT8 },
    { 0, 12, "picbl", LAYOUT_PTR },
    { 0, 18, "flags", LAYOUT_UINT16 },
    { 0, 20, 0, LAYOUT_END },
};

struct icbtag_s *
icbtag_malloc ()
{
  struct icbtag_s * obj;
  obj = malloc(sizeof(struct icbtag_s));
  memset(obj, 0, sizeof(obj));
  return obj;
}

struct icbtag_s *
icbtag_destroy (struct icbtag_s *obj)
{
  return obj;
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

void
icbtag_free (struct icbtag_s *obj)
{
  free(icbtag_destroy(obj));
}

struct icbtag_s *
icbtag_decode (struct icbtag_s * obj, const uint8_t * space, int spacelen)
{
//  struct icbtag_s * obj = NULL;
  layoutvalue_t contents[9] = { 0, };

  udf_decode(space, spacelen, udf_icbtag, contents);

  if (!obj) obj = icbtag_malloc();

  obj->prnde = contents[0].word;
  obj->st = contents[1].word;
  obj->sp = contents[2].word;
  obj->mne = contents[3].word;
  obj->ft = contents[5].word;
  struct lb_addr_s * picbl = lb_addr_decode(contents[6].ptr, 6);
  obj->picbl = *picbl;
  lb_addr_free(picbl);

  // TODO: flags
//  obj->flags = contents[7].word;

  return obj;
}

int
icbtag_encode (const struct icbtag_s *obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[9] = { 0, };

  contents[0].word = obj->prnde;
  contents[1].word = obj->st;
  contents[2].word = obj->sp;
  contents[3].word = obj->mne;
  contents[5].word = obj->ft;
  uint8_t picbl[6];
  lb_addr_encode(&(obj->picbl), picbl, sizeof(picbl));
  contents[6].ptr = picbl;
  // TODO: flags
//  contents[7].word = obj->prnde;

  int n = udf_encode(space, spacelen, udf_icbtag, contents);

  return n;
}

int
icbtag_repr (const struct icbtag_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct icbtag_s _%p = {", obj);
  n += snprintf(buf+n, buflen-n, "  .prnde = %u,\n", obj->prnde);
  n += snprintf(buf+n, buflen-n, "  .st = %u,\n", obj->st);
  n += snprintf(buf+n, buflen-n, "  .sp = %u,\n", obj->sp);
  n += snprintf(buf+n, buflen-n, "  .mne = %u,\n", obj->mne);
  n += snprintf(buf+n, buflen-n, "  .ft = %u, /* %s */\n",
		icb_file_type_int(obj->ft), icb_file_type_name(obj->ft));
  char picbl[64];
  lb_addr_repr(&(obj->picbl), picbl, sizeof(picbl));
  reindent_repr(picbl, sizeof(picbl), 2);
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
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}

void
icbtag_dump (const struct icbtag_s * obj)
{
  char buf[512];
  int n = icbtag_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}








struct layoutfield_s udf_long_ad[] = {
      { 0, 0, "len", LAYOUT_UINT32 },
      { 0, 4, "loc", LAYOUT_PTR },
      { 0, 10, "impuse", LAYOUT_PTR },
      { 0, 16, 0, LAYOUT_END },
};

struct long_ad_s *
long_ad_malloc ()
{
  struct long_ad_s * retval = malloc(sizeof(struct long_ad_s));
  memset(retval, 0, sizeof(*retval));
  return retval;
}

struct long_ad_s *
long_ad_destroy (struct long_ad_s *obj)
{
  return obj;
}

struct long_ad_s *
long_ad_init (struct long_ad_s *obj, unsigned int len, const struct lb_addr_s * loc, uint8_t impuse[6])
{
  if (!obj) return obj;

  obj->len = len;
  obj->loc = *loc;
  memcpy(obj->impuse, impuse, 6);

  return obj;
}

void
long_ad_free (struct long_ad_s *obj)
{
  free(long_ad_destroy(obj));
}

struct long_ad_s *
long_ad_decode (struct long_ad_s *obj, const uint8_t space[], int spacelen)
{
  layoutvalue_t contents[4] = { 0, };

  if (!obj) obj = long_ad_malloc();
  if (!obj || !space || !spacelen) return obj;

  udf_decode(space, spacelen, udf_long_ad, contents);

  obj->len = contents[0].word;
  struct lb_addr_s * loc;
  loc = lb_addr_decode(contents[1].ptr, 6);
  obj->loc = *loc;
  lb_addr_free(loc);
  memcpy(obj->impuse, contents[2].ptr, 6);

  return obj;
}

int
long_ad_encode (const struct long_ad_s *obj, uint8_t space[], int spacelen)
{
  layoutvalue_t contents[4] = { 0, };
  uint8_t loc[6];
//  uint8_t impuse[6];

  lb_addr_encode(&(obj->loc), loc, sizeof(loc));

  contents[0].word = obj->len;
  contents[1].ptr = loc;
  contents[2].ptr = (void*)&(obj->impuse);
  contents[3].word = 16;

  int res = udf_encode(space, spacelen, udf_long_ad, contents);
  return res;
}

int
long_ad_cmp (const struct long_ad_s *a, const struct long_ad_s *b)
{
  int res = memcmp(a, b, sizeof(*a));
  return res;
}

int
long_ad_repr (const struct long_ad_s *obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct long_ad_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .len = %u,\n", obj->len);
  char loc[64];
  lb_addr_repr(&(obj->loc), loc, sizeof(loc));
  reindent_repr(loc, sizeof(loc), 2);
  n += snprintf(buf+n, buflen-n, "  .loc = %s,\n", loc);
  n += snprintf(buf+n, buflen-n,
	       	"  .impuse = { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x },\n",
	       	obj->impuse[0], obj->impuse[1], obj->impuse[2],
	       	obj->impuse[3], obj->impuse[4], obj->impuse[5]);
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}

void
long_ad_dump (const struct long_ad_s *obj)
{
  char buf[512];
  int n = long_ad_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}























layoutdescr_t udf_fid = {
      { 0, 0, "tag", LAYOUT_PTR },
      { 0, 16, "fvn", LAYOUT_UINT16 },
      { 0, 18, "fc", LAYOUT_UINT8 },
      { 0, 19, "L_FI", LAYOUT_UINT8 },
      { 0, 20, "ICB", LAYOUT_PTR },
      { 0, 36, "L_IU", LAYOUT_UINT16 },
      { 0, 38, "iu", LAYOUT_PTR },
      { (1 << 5), 38, "fi", LAYOUT_PTR },
      { (1 << 3) | (1 << 5), 38, 0, LAYOUT_END },
};

static unsigned int
_fid_padding (unsigned int x)
{
  return (4 - (x % 4)) % 4;
}

/* Expected size of UDF binary equivalent. */
size_t
fid_len (const struct fid_s *obj)
{
  size_t dlen = obj->L_FI + obj->L_IU + obj->padding;
  size_t msize = 38 + dlen;
  return msize;
}

// malloc, init, destroy, free, decode, encode, str, dump
struct fid_s *
fid_malloc (unsigned int dlen)
{
  dlen += 4;  /* TODO: less-lazy calculation. */
  size_t msize = sizeof(struct fid_s) + dlen;
  struct fid_s * obj = malloc(msize);
  memset(obj, 0, msize);

  obj->fvn = 1;
  tag_init(&(obj->tag), TAGID_FID, 3, 0, 0);
  obj->impuse = obj->d;
  obj->fi = obj->d;
  return obj;
}

struct fid_s *
fid_destroy (struct fid_s *obj)
{
  return obj;
}

struct fid_s *
fid_init (struct fid_s *obj,
	  const struct tag_s *tag,
	  unsigned int file_version_number,
	  unsigned int file_characteristics,
	  const struct long_ad_s * icb,
	  uint8_t * implementation_use,
	  unsigned int length_implementation_use,
	  const char * file_identifier,
	  unsigned int length_file_identifier)
{
  size_t dlen = 0;
  dlen = length_implementation_use + length_file_identifier + 2;
  size_t msize = dlen + sizeof(struct fid_s);

  if (!obj) return obj;

  if (tag) obj->tag = *(tag);
  obj->fvn = file_version_number;
  obj->fc = file_characteristics;
  if (icb) obj->icb = *(icb);
  obj->impuse = obj->d;
  obj->L_IU = length_implementation_use;
  obj->fi = obj->d + length_implementation_use + 1;
  obj->L_FI = length_file_identifier;
  obj->padding = _fid_padding(38 + obj->L_IU + obj->L_FI);

  memcpy(obj->impuse, implementation_use, length_implementation_use);
  memcpy(obj->fi, file_identifier, length_file_identifier);
  obj->fi[length_file_identifier] = 0;

  return obj;
}

void
fid_free (struct fid_s *obj)
{
  free(fid_destroy(obj));
}

struct fid_s *
fid_decode (uint8_t * space, int spacelen)
{
  struct fid_s * obj = NULL;
  layoutvalue_t contents[10] = { 0, };
  int dlen;
  int padding;

  udf_decode(space, spacelen, udf_fid, contents);

  unsigned int L_FI = contents[3].word;
  unsigned int L_IU = contents[5].word;
  dlen = L_FI + L_IU;

  if (!obj) obj = fid_malloc(dlen);
  if (!obj || !space || !spacelen) return obj;

  // align to get 4-byte boundary.
  //padding = 4 * ((L_FI + L_IU + 38 + 3) / 4) - (L_FI + L_IU + 38);
  //padding = (4 - ((L_FI + L_IU + 38) % 4)) % 4;
  padding = _fid_padding(L_FI + L_IU + 38);
  contents[8].word = padding;

//  tag_decode(&(obj->tag), contents[0].ptr, 16);
  obj->fvn = contents[1].word;
  obj->fc = contents[2].word;
  // obj->L_FI = contents[3].word;
//  long_ad_decode(&(obj->icb), contents[4].ptr, 16);
  // obj->L_IU = contents[5].word;
  obj->impuse = obj->d;
  obj->fi = obj->d + L_IU;
  bytestr_decode(obj->impuse, L_IU, contents[6].ptr, L_IU);
  bytestr_decode(obj->fi, L_FI, contents[7].ptr, L_FI);

  return obj;
}

int
fid_encode (const struct fid_s *obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[10] = { 0, };
  uint8_t tag[16];
  uint8_t icb[16];

  tag_encode(&(obj->tag), tag, sizeof(tag));
  long_ad_encode(&(obj->icb), icb, sizeof(icb));

  unsigned int padding =_fid_padding(obj->L_FI + obj->L_IU + 38);
  contents[0].ptr = tag;
  contents[1].word = obj->fvn;
  contents[2].word = obj->fc;
  contents[3].word = obj->L_FI;
  contents[4].ptr = icb;
  contents[5].word = obj->L_IU;
  contents[6].ptr = obj->impuse;
  contents[7].ptr = obj->fi;
//  contents[8].word = _fid_padding(obj->L_FI + obj->L_IU + 38);
  contents[8].word = 38 + obj->L_FI + obj->L_IU + padding;

  int retval = udf_encode(space, spacelen, udf_fid, contents);

  return retval;
}

int
fid_repr (const struct fid_s *obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct fid_s _%p = {\n", obj);

  char tmp[256];
  tag_repr(&(obj->tag), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);

  n += snprintf(buf+n, buflen-n, "  .tag = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .fvn = %u,\n", obj->fvn);
  n += snprintf(buf+n, buflen-n, "  .fc = %u,\n", obj->fc);
  n += snprintf(buf+n, buflen-n, "  .L_FI = %u,\n", obj->L_FI);

  long_ad_repr(&(obj->icb), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);

  n += snprintf(buf+n, buflen-n, "  .icb = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .L_IU = %u,\n", obj->L_IU);
  n += snprintf(buf+n, buflen-n, "  .impuse = ...,\n");
  n += snprintf(buf+n, buflen-n, "  .fi = \"%s\",\n", obj->fi);
  n += snprintf(buf+n, buflen-n, "  .padding = %u,\n", obj->padding);
  n += snprintf(buf+n, buflen-n, "}");
  return n;
}

void
fid_dump (const struct fid_s *obj)
{
  char buf[1024];
  int n = fid_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}







struct layoutfield_s udf_fsd[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "rdt", LAYOUT_PTR },
    { 0, 28, "il", LAYOUT_UINT16 },
    { 0, 30, "mil", LAYOUT_UINT16 },
    { 0, 32, "csl", LAYOUT_UINT32 },
    { 0, 36, "mcsl", LAYOUT_UINT32 },
    { 0, 40, "fsn", LAYOUT_UINT32 },
    { 0, 44, "fsdn", LAYOUT_UINT32 },
    { 0, 112, "lvidcs", LAYOUT_PTR },
    { 0, 240, "lvid", LAYOUT_PTR },
    { 0, 304, "fscs", LAYOUT_PTR },
    { 0, 336, "fsid", LAYOUT_PTR },
    { 0, 368, "cfid", LAYOUT_PTR },
    { 0, 400, "afid", LAYOUT_PTR },
    { 0, 416, "rdicb", LAYOUT_PTR },
    { 0, 448, "domid", LAYOUT_PTR },
    { 0, 464, "ne",LAYOUT_PTR },
    { 0, 480, "ssdicb", LAYOUT_PTR },
    { 0, 480, "reserved", LAYOUT_RESERVED },
    { 0, 512, 0, LAYOUT_END },
};
#define SELF struct fsd_s *obj

#define INSTFORM_fsd_const(args...) (const struct fsd_s *obj, ##args)
#define INSTFORM_fsd(args...) (struct fsd_s *obj, ##args)

//#define INSTFORM(args...) FORM(Thingy, ##args)

#define INSTFUNC(class, method) class##_##method INSTFORM_##class
#define INSTFUNC_CONST(class, method) class##_##method INSTFORM_##class##_const
//#define INSTFUNC(class, method) class##_##method INSTFORM
//#define INSTFUNC_CONST(class, method) class##_##method INSTFORM(class)



struct fsd_s *
fsd_malloc ()
{
  struct fsd_s * retval = malloc(sizeof(struct fsd_s));
  memset(retval, 0, sizeof(*retval));
  return retval;
}

struct fsd_s *
fsd_destroy (struct fsd_s *obj)
{
  return obj;
}

struct fsd_s *
fsd_init_atoms (struct fsd_s *obj,
                unsigned int il,
                unsigned int mil,
                unsigned int csl,
                unsigned int mcsl,
                unsigned int fsn,
                unsigned int fsdn)
{
  obj->il = il;
  obj->mil = mil;
  obj->csl = csl;
  obj->mcsl = mcsl;
  obj->fsn = fsn;
  obj->fsdn = fsdn;
  return obj;
}

struct fsd_s *
fsd_init (struct fsd_s *obj,
          const struct tag_s * tag,
          const struct timestamp_s * rdt,
          unsigned int il,
          unsigned int mil,
          unsigned int csl,
          unsigned int mcsl,
          unsigned int fsn,
          unsigned int fsdn,
          const struct charspec_s * lvidcs,
          const struct dstring_s * lvid,
          const struct charspec_s * fscs,
          const struct dstring_s * fsid,
          const struct dstring_s * cfid,
          const struct dstring_s * afid,
          const struct long_ad_s * rdicb,
          const struct regid_s * domid,
          const struct long_ad_s * ne,
          const struct long_ad_s * ssdicb)
{
  fsd_init_atoms(obj, il, mil, csl, mcsl, fsn, fsdn);
  obj->tag = *tag;
  obj->rdt = *rdt;
  obj->lvidcs = *lvidcs;
  obj->lvid = *lvid;
  obj->fscs = *fscs;
  obj->fsid = *fsid;
  obj->cfid = *cfid;
  obj->afid = *afid;
  obj->rdicb = *rdicb;
  obj->domid = *domid;
  obj->ne = *ne;
  obj->ssdicb = *ssdicb;

  return obj;
}

void
fsd_free (struct fsd_s *obj)
{
  free(fsd_destroy(obj));
}

struct fsd_s *
fsd_decode (const uint8_t * space, int spacelen)
{
  struct fsd_s * obj = NULL;

  layoutvalue_t contents[20] = { 0, };

  if (!obj) obj = fsd_malloc();
  if (!obj || !space || !spacelen) return obj;

  udf_decode(space, spacelen, udf_fsd, contents);

  struct tag_s * tag = tag_decode(NULL, contents[0].ptr, 16);
  obj->tag = *tag;
  tag_free(tag);

  struct timestamp_s * rdt = timestamp_decode(contents[1].ptr, 12);
  obj->rdt = *rdt;
  timestamp_free(rdt);

  obj->il = contents[2].word;
  obj->mil = contents[3].word;
  obj->csl = contents[4].word;
  obj->mcsl = contents[5].word;
  obj->fsn = contents[6].word;
  obj->fsdn = contents[7].word;

  struct charspec_s * lvidcs = charspec_decode(contents[8].ptr, 64);
  obj->lvidcs = *lvidcs;
  charspec_free(lvidcs);

  struct dstring_s * lvid = dstring_decode(contents[9].ptr, 128);
  obj->lvid = *lvid;
  dstring_free(lvid);

  struct charspec_s * fscs = charspec_decode(contents[10].ptr, 64);
  obj->fscs = *fscs;
  charspec_free(fscs);

  struct dstring_s * fsid = dstring_decode(contents[11].ptr, 32);
  obj->fsid = *fsid;
  dstring_free(fsid);

  struct dstring_s * cfid = dstring_decode(contents[12].ptr, 32);
  obj->cfid = *fsid;
  dstring_free(cfid);

  struct dstring_s * afid = dstring_decode(contents[13].ptr, 32);
  obj->afid = *fsid;
  dstring_free(afid);

  struct long_ad_s * rdicb = long_ad_decode(NULL, contents[14].ptr, 16);
  obj->rdicb = *rdicb;
  long_ad_free(rdicb);

  struct regid_s * domid = regid_decode(contents[15].ptr, 32);
  obj->domid = *domid;
  regid_free(domid);

  struct long_ad_s * ne = long_ad_decode(NULL, contents[16].ptr, 16);
  obj->ne = *ne;
  long_ad_free(ne);

  struct long_ad_s * ssdicb = long_ad_decode(NULL, contents[17].ptr, 16);
  obj->ssdicb = *ssdicb;
  long_ad_free(ssdicb);

  /* TODO: make sure reserved is #00. */
  contents[18].word = 512;

  return obj;
}

int
fsd_encode (const struct fsd_s *obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[20] = { 0, };

  uint8_t tag[16];
  uint8_t rdt[12];
  uint8_t lvidcs[64];
  uint8_t lvid[128];
  uint8_t fscs[64];
  uint8_t fsid[32];
  uint8_t cfid[32];
  uint8_t afid[32];
  uint8_t rdicb[16];
  uint8_t domid[32];
  uint8_t ne[16];
  uint8_t ssdicb[16];

  tag_encode(&(obj->tag), tag, sizeof(tag));
  timestamp_encode(&(obj->rdt), rdt, sizeof(rdt));
  charspec_encode(&(obj->lvidcs), lvidcs, sizeof(lvidcs));
  dstring_encode(&(obj->lvid), lvid, sizeof(lvid));
  charspec_encode(&(obj->fscs), fscs, sizeof(fscs));
  dstring_encode(&(obj->fsid), fsid, sizeof(fsid));
  dstring_encode(&(obj->cfid), cfid, sizeof(cfid));
  dstring_encode(&(obj->afid), afid, sizeof(afid));
  long_ad_encode(&(obj->rdicb), rdicb, sizeof(rdicb));
  regid_encode(&(obj->domid), domid, sizeof(domid));
  long_ad_encode(&(obj->ne), ne, sizeof(ne));
  long_ad_encode(&(obj->ssdicb), ssdicb, sizeof(ssdicb));

  printf("/* domain identifier */\n");
  hexdump(domid, sizeof(domid));
  printf("/**/\n");
  printf("/* next_extent */\n");
  hexdump(ne, sizeof(ne));
  printf("/**/\n");

  contents[0].ptr = tag;
  contents[1].ptr = rdt;
  contents[2].word = obj->il;
  contents[3].word = obj->mil;
  contents[4].word = obj->csl;
  contents[5].word = obj->mcsl;
  contents[6].word = obj->fsn;
  contents[7].word = obj->fsdn;
  contents[8].ptr = lvidcs;
  contents[9].ptr = lvid;
  contents[10].ptr = fscs;
  contents[11].ptr = fsid;
  contents[12].ptr = cfid;
  contents[13].ptr = afid;
  contents[14].ptr = rdicb;
  contents[15].ptr = domid;
  contents[16].ptr = ne;
  contents[17].ptr = ssdicb;
  contents[18].word = 512;

  int retval = udf_encode(space, spacelen, udf_fsd, contents);

  return retval;
}

int
fsd_cmp (const struct fsd_s *a, const struct fsd_s *b)
{
  /* TODO: better compare. */
  int retval = memcmp(a, b, sizeof(*a));
  return retval;
}

int
fsd_repr (const struct fsd_s *obj, char buf[], int buflen)
{
  int n = 0;
  char tmp[512];
  n += snprintf(buf+n, buflen-n, "struct fsd_s _%p = {\n", obj);
  tag_repr(&(obj->tag), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .tag = %s,\n", tmp);
  timestamp_repr(&(obj->rdt), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .rdt = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .il = %d,\n", obj->il);
  n += snprintf(buf+n, buflen-n, "  .mil = %d,\n", obj->mil);
  n += snprintf(buf+n, buflen-n, "  .csl = 0x%04x,\n", obj->csl);
  n += snprintf(buf+n, buflen-n, "  .mcsl = 0x%04x,\n", obj->mcsl);
  n += snprintf(buf+n, buflen-n, "  .fsn = %d,\n", obj->fsn);
  n += snprintf(buf+n, buflen-n, "  .fsdn = %d,\n", obj->fsdn);
  charspec_repr(&(obj->lvidcs), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .lvidcs = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .lvid = \"%s\",\n", obj->lvid.d);
  charspec_repr(&(obj->fscs), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .fscs = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .fsid = \"%s\",\n", obj->fsid.d);
  n += snprintf(buf+n, buflen-n, "  .cfid = \"%s\",\n", obj->cfid.d);
  n += snprintf(buf+n, buflen-n, "  .afid = \"%s\",\n", obj->afid.d);
  long_ad_repr(&(obj->rdicb), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .rdicb = %s,\n", tmp);
  regid_repr(&(obj->domid), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .domid = %s,\n", tmp);
  long_ad_repr(&(obj->ne), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .ne = %s,\n", tmp);
  long_ad_repr(&(obj->ssdicb), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .ssdicb = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "}");

  return n;
}

void
INSTFUNC_CONST(fsd, dump) ()
{
  char buf[2048];
  int n = fsd_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}





#if 0
struct layoutfield_s udf_aed[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "pael", LAYOUT_UINT32 },
    { 0, 20, "L_AD", LAYOUT_UINT32 },
    { 0, 24, 0, LAYOUT_END },
};

#if 0
struct aed_s {
    struct tag_s tag;
    unsigned int pael;
    unsigned int dlen;

    unsigned int adtype;  // ADTYPE_*
    unsigned int nad;  /* Number of allocation descriptors, of adtype. */
    union {
        uint8_t * unknown;
        uint8_t * direct;
        struct short_ad_s * short_ad;
        struct long_ad_s * long_ad;
        struct ext_ad_s * ext_ad;
    } ad;  /* Access as Allocation Descriptors Sequence. */

    uint8_t d[];    /* Storage space for Allocation Descriptors Sequence. */
};
#endif //0

struct aed_s *
aed_malloc (int adbytes)
{
  struct aed_s * obj;
  size_t msize = sizeof(struct aed_s) + adbytes;

  fprintf(stderr, "aed_malloc implementation incomplete\n");
  obj = malloc(msize);
  obj->dlen = adbytes;
  obj->adtype = ADTYPE_DIRECT;
  obj->ad.direct = obj->d;

  return obj;
}

struct aed_s *
aed_short_malloc (int nad)
{
  int dlen = sizeof(struct short_ad_s) * nad;
  struct aed_s * obj = aed_malloc(dlen);
  obj->adtype = ADTYPE_SHORT;
  obj->ad.short_ad = (struct short_ad_s *)(obj->d);
  obj->nad = nad;

  return obj;
}

struct aed_s *
aed_long_malloc (int nad)
{
  int dlen = sizeof(struct long_ad_s) * nad;
  struct aed_s * obj = aed_malloc(dlen);
  obj->adtype = ADTYPE_LONG;
  obj->ad.long_ad = (struct long_ad_s*)(obj->d);
  obj->nad = nad;

  return obj;
}

struct aed_s *
aed_ext_malloc (int nad)
{
  int dlen = sizeof(struct ext_ad_s) * nad;
  struct aed_s * obj = aed_malloc(dlen);
  obj->adtype = ADTYPE_EXT;
  obj->ad.ext_ad = (struct ext_ad_s*)(obj->d);
  obj->nad = nad;

  return obj;
}

struct aed_s *
aed_destroy (struct aed_s *obj)
{
  return obj;
}

struct aed_s *
aed_init (struct aed_s *obj,
          unsigned int pael,
          unsigned int adtype,
          const void * ad_data,
          unsigned int adlen)
{
  if (!obj) return obj;

  obj = aed_malloc(adlen);
  obj->pael = pael;
  obj->adtype = adtype;
  switch (adtype)
    {
    case ADTYPE_SHORT:
      obj->nad = adlen / sizeof(struct short_ad_s);
      obj->ad.short_ad = (struct short_ad_s*)(obj->d);
      break;
    case ADTYPE_LONG:
      obj->nad = adlen / sizeof(struct long_ad_s);
      obj->ad.long_ad = (struct long_ad_s*)(obj->d);
      break;
    case ADTYPE_EXT:
      obj->nad = adlen / sizeof(struct ext_ad_s);
      obj->ad.ext_ad = (struct ext_ad_s*)(obj->d);
      break;
    default:
      obj->adtype = ADTYPE_DIRECT;
      obj->nad = adlen;
      obj->ad.direct = (uint8_t*)(obj->d);
      break;
    }
  memcpy(obj->d, ad_data, adlen);

  return obj;
}

void
aed_free (struct aed_s *obj)
{
  free(aed_destroy(obj));
}

struct aed_s *
aed_decode (const uint8_t * space, int spacelen)
{
  struct aed_s * obj = NULL;
  layoutvalue_t contents[4] = { 0, };

  udf_decode(space, spacelen, udf_aed, contents);

  size_t dlen = contents[2].word;

  if (!obj) obj = aed_malloc(dlen);
  if (!obj || !space || !spacelen) return obj;

  struct tag_s * tag = tag_decode(NULL, contents[0].ptr, 16);
  obj->tag = *tag;
  tag_free(tag);
  obj->pael = contents[1].word;
  obj->dlen = dlen;
  memcpy(obj->d, space+24, spacelen-24);

  // TODO: ensure total size of descriptor+AD does not exceed sector size.

  return obj;
}

int
aed_encode (const struct aed_s *obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[4] = { 0, };

  uint8_t tag[16];
  tag_encode(&(obj->tag), tag, sizeof(tag));

  size_t dlen;

  contents[0].ptr = tag;
  contents[1].word = obj->pael;
  /* TODO: calculate packed. */
  dlen = obj->dlen;
  contents[2].word = obj->dlen;

  int retval = udf_encode(space, spacelen, udf_aed, contents);

  // TODO: encode array of allocation descriptors.

  return retval;
}

/* Calculate space needed for encode() */
int
aed_len (const struct aed_s *obj)
{
  size_t msize = 24;
  // TODO: calculate this?
  msize += obj->dlen;

  return msize;
}

int
aed_cmp (const struct aed_s *a, const struct aed_s *b)
{
  return -1;
}

int
aed_repr (const struct aed_s *obj, char buf[], int buflen)
{
  int n;
  char tmp[128];
  n += snprintf(buf+n, buflen-n, "struct aed_s _%p = {\n", obj);
  tag_encode(&(obj->tag), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .tag = %s,\n", tmp);
  n += snprintf(buf+n, buflen-n, "  .pael = %u,\n", obj->pael);
  // TODO: calculated adlen.
  unsigned int dlen = obj->dlen;
  n += snprintf(buf+n, buflen-n, "  .L_AD = %u,\n", obj->dlen);
  n += snprintf(buf+n, buflen-n, "}");

  return n;
}

void
aed_dump (const struct aed_s *obj)
{
  char buf[512];
  aed_repr(obj, buf, sizeof(buf));
  puts(buf);
}
#endif //0
#include "filst_aed.c"


#include "filst_ie.c"


#include "filst_te.c"



