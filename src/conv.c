#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "conv.h"

/* Converter functions, byte-endian sensitive. */


int uint8_encode (void * space, uint8_t val)
{
  *((uint8_t*)space) = val;
  return sizeof(val);
}

int uint16_encode (void * space, uint16_t val)
{
//  *((uint16_t*)space) = val;
  ((uint8_t*)space)[0] = (val >> 0) & 0xff;
  ((uint8_t*)space)[1] = (val >> 8) & 0xff;
  return sizeof(val);
}

int uint32_encode (void * space, uint32_t val)
{
//  *((uint32_t*)space) = val;
  ((uint8_t*)space)[0] = (val >> 0) & 0xff;
  ((uint8_t*)space)[1] = (val >> 8) & 0xff;
  ((uint8_t*)space)[2] = (val >> 16) & 0xff;
  ((uint8_t*)space)[3] = (val >> 24) & 0xff;
  return sizeof(val);
}

int uint64_encode (void * space, uint64_t val)
{
//  *((uint64_t*)space) = val;
  ((uint8_t*)space)[0] = (val >> 0) & 0xff;
  ((uint8_t*)space)[1] = (val >> 8) & 0xff;
  ((uint8_t*)space)[2] = (val >> 16) & 0xff;
  ((uint8_t*)space)[3] = (val >> 24) & 0xff;
  ((uint8_t*)space)[4] = (val >> 32) & 0xff;
  ((uint8_t*)space)[5] = (val >> 40) & 0xff;
  ((uint8_t*)space)[6] = (val >> 48) & 0xff;
  ((uint8_t*)space)[7] = (val >> 56) & 0xff;
  return sizeof(val);
}




uint8_t uint8_decode (void * space)
{
  return *((uint8_t*)(space));
}

uint16_t uint16_decode (void * space)
{
//  return *((uint16_t*)(space));
  uint8_t * x = (uint8_t*)space;
  return (uint16_t)(x[0])<<0 | (uint16_t)(x[1])<<8;
}

uint32_t uint32_decode (void * space)
{
//  return *((uint32_t*)(space));
  uint8_t * x = (uint8_t*)space;
  return (uint32_t)(x[0])<<0  | (uint32_t)(x[1])<<8
       | (uint32_t)(x[2])<<16 | (uint32_t)(x[3])<<24;
}

uint64_t uint64_decode (void * space)
{
//  return *((uint64_t*)(space));
  uint8_t * x = (uint8_t*)space;
  return (uint64_t)(x[0])<<0  | (uint64_t)(x[1])<<8
       | (uint64_t)(x[2])<<16 | (uint64_t)(x[3])<<24
       | (uint64_t)(x[4])<<36 | (uint64_t)(x[5])<<40
       | (uint64_t)(x[6])<<48 | (uint64_t)(x[7])<<56;
}


int bytestr_encode (const uint8_t * src, int srclen, void * space, int spacelen)
{
  int n = (srclen < spacelen) ? srclen : spacelen;  // min(srclen, spacelen)
  memcpy(space, src, n);
  return n;
}

uint8_t * bytestr_decode (uint8_t * dst, int dstlen, const void * space, int spacelen)
{
  int n = (dstlen < spacelen) ? dstlen : spacelen;  // min(dstlen, spacelen)
  memcpy(dst, space, n);
  return dst;
}



/* En/De-coding a UDF struct:
Field description provided as layoutfield_t,
Values for those fields are in array of layoutfield_t, indices matching (in the spirit of a register file).
Some field byte positions depend on the contents of other fields, so the
value-holding structure needs to be as non-complex as feasible.
 */


/* ECMA-167/3 4/14.16.1 - Path Component */
struct layoutfield_s udf_path_component[] = {
  { 0x00, 0, "ct", LAYOUT_UINT8 },
  { 0x00, 1, "L_CI", LAYOUT_UINT8 },
  { 0x00, 2, "cfvn", LAYOUT_UINT16 },
  { 0x00, 4, "ci", LAYOUT_PTR },
  { (1<<1), 4, 0, LAYOUT_END },
};

struct layoutfield_s udf_lb_addr[] = {
  { 0x00, 0, "lbn", LAYOUT_UINT32 },
  { 0x00, 4, "prn", LAYOUT_UINT16 },
};



#include <stdio.h>

int
layoutvalues_str (const layoutvalues_t layoutvalues, int count, char buf[], int buflen)
{
  int n = 0;
  int i;
  n += snprintf(buf, buflen, "layoutvalue_t _%p [%d] = {\n", layoutvalues, count);
  int len = layoutvalues[1].word;
  for (i = 0; (i < count) && (n < buflen); i++)
    {
      n += snprintf(buf+n, buflen-n, "  { .word=%lu, .ptr=%p,",
		    layoutvalues[i].word, layoutvalues[i].ptr);
#if 0
      if ((i == 3) && (len > 0))
	{
	  const char * s = (const char *)(layoutvalues[i].ptr);
	  n += snprintf(buf+n, buflen-n, " .s=\"%s\"", s);
	}
      else
	{
	  n += snprintf(buf+n, buflen-n, " .s=%p", layoutvalues[i].ptr);
	}
#endif //0
      if (layoutvalues[i].word > 0xfffffffffUL)  // only on 64b systems.
	{
	  const char * s = (const char *)(layoutvalues[i].ptr);
	  n += snprintf(buf+n, buflen-n, " .s=\"%s\"", s);
	}
      else
	{
	  n += snprintf(buf+n, buflen-n, " .s=%p", layoutvalues[i].ptr);
	}
      n += snprintf(buf+n, buflen-n, " },\n");
    }
  snprintf(buf+n, buflen-n, " };");
  return n;
}

int
layoutvalues_dump (const layoutvalues_t layoutvalues, int count)
{
#if 0
  int i;
  printf("LayoutValues(@%p)\n", layoutvalues);
  for (i = 0; i < count; i++)
    {
      printf(" %d) { %lu, %p }\n", i, layoutvalues[i].word, layoutvalues[i].ptr);
    }
  printf(".\n");
  return 0;
#endif //0
  char buf[1024];
  layoutvalues_str(layoutvalues, count, buf, sizeof(buf));
  puts(buf);
}



static
int
_resolve_bp (const layoutdescr_t descr, const layoutvalues_t contents, int fldidx)
{
  const struct layoutfield_s * fld = descr + fldidx;
  const union layoutvalue_u * valptr = contents + fldidx;
  int bp = fld->rbp;  /* Byte Position. */
  int vbp = 0; /* Variable Byte Position bias. */

  if (fld->vbpmask)
    {
      /* Calculate Variable Byte Position bias. */
      int n;
      for (n = 0; n < 32; n++)
        {
          /* Bit N on => Add Content Of Word N */
          if ( ((1 << n) & fld->vbpmask) == (1 << n) )
            {
              /* TODO: pick .word or .oversize? */
              vbp += contents[n].word;
            }
        }
    }
  bp += vbp;

  return bp;
}


int
udf_decode (const void * raw, int rawlen, const layoutdescr_t descr, layoutvalues_t contents)
{
  uint8_t * udfbytes = (uint8_t*)raw;
  int fldidx = 0;
  int complete = 0;
  const struct layoutfield_s * fld = descr + fldidx;
  union layoutvalue_u * valptr = contents + fldidx;
  int bp = 0;
  //int vbp = 0;

  while (!complete)
    {
      fld = descr + fldidx;
      valptr = contents + fldidx;
#if 0
      bp = fld->rbp;
      vbp = 0;
      /* Variable Byte Position, which other BP values to offset by. */
      if (fld->vbpmask)
        {
          int n;
          for (n = 0; n < 32; n++)
            {
              /* Bit N on => Add Content Of Word N */
              if ( ((1 << n) & fld->vbpmask) == (1 << n) )
                {
                  /* TODO: pick .word or .oversize? */
                  vbp += contents[n].word;
                }
            }
        }
      bp += vbp;
#endif //0
      bp = _resolve_bp(descr, contents, fldidx);
      if (bp >= rawlen)
	{
	  complete = 1;
	  break;
	}

      switch (fld->fldtype)
        {
        case LAYOUT_RESERVED:
          valptr->word = 0;
          break;
        case LAYOUT_UINT8:
          valptr->word = uint8_decode(udfbytes + bp);
          break;
        case LAYOUT_UINT16:
          valptr->word = uint16_decode(udfbytes + bp);
          break;
        case LAYOUT_UINT32:
          valptr->word = uint32_decode(udfbytes + bp);
          break;
        case LAYOUT_UINT64:
          valptr->word = uint64_decode(udfbytes + bp);
          break;
        case LAYOUT_UINT128:
          valptr->oversize = udfbytes + bp;
          break;
        case LAYOUT_PTR:
          valptr->ptr = udfbytes + bp;
          break;
        case LAYOUT_END:
          valptr->word = bp;
          complete = 1;
          break;
        default:
          abort();
          break;
        }

      fldidx++;
    }

}


/*

   Returns number of bytes.  Should be compared against expected number of bytes.
*/
int
udf_encode (void * space, int spacelen, const layoutdescr_t descr, const layoutvalues_t contents)
{
  int retval = 0;
  unsigned int fldidx = 0;
  int complete = 0;
  int bp = 0;
  int nbp = 0; /* Next Byte Position, presumed cutoff for variable-length fields. */
  const struct layoutfield_s * fld = NULL;
  const union layoutvalue_u * valptr = NULL; //contents + fldidx;

  while (!complete)
    {
      fld = descr + fldidx;
      valptr = contents + fldidx;

      if (nbp)
        {
          bp = nbp;
        }
      if (fld->fldtype == LAYOUT_END)
        {
          /* End of descriptors; there is no next.  Avoid OOB. */
          nbp = bp;
        }
      else
        {
          nbp = _resolve_bp(descr, contents, fldidx+1);
        }

      switch (fld->fldtype)
        {
        case LAYOUT_RESERVED:
	  /* fill with #00 to nbp */
	  memset(space + bp, 0, (nbp-bp));
          break;
        case LAYOUT_UINT8:
          uint8_encode(space + bp, contents[fldidx].word);
          break;
        case LAYOUT_UINT16:
          uint16_encode(space + bp, contents[fldidx].word);
          break;
        case LAYOUT_UINT32:
          uint32_encode(space + bp, contents[fldidx].word);
          break;
        case LAYOUT_UINT64:
          uint64_encode(space + bp, contents[fldidx].word);
          break;
        case LAYOUT_UINT128:
          //uint128_encode(space + bp, contents[fldidx].oversize);
          break;
        case LAYOUT_PTR:
            {
              int len = nbp - bp;
              bytestr_encode(contents[fldidx].ptr, len, space + bp, len);
            }
          break;
        case LAYOUT_END:
          if (valptr->word == bp)
            {
              /* Consistent. */
              retval = bp;
            }
          else
            {
              /* Error: inconsistency. */
              retval = bp;
            }
          complete = 1;
          break;
        }

      fldidx++;
    }

  return retval;
}

