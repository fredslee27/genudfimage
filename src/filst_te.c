/* m4 macros, filst */
#include <stdio.h>
#include <string.h>

#include "basics.h"
#include "conv.h"
#include "filst.h"



struct layoutfield_s udf_te[] = {
  { 0, 0, "tag", LAYOUT_PTR },
  { 0, 16, "icbtag", LAYOUT_PTR },
  { 0, 36, 0, LAYOUT_END },
};


struct te_s *
te_malloc ()
{
  struct te_s * obj = NULL;
  size_t msize = sizeof(struct te_s) + 0;
  obj = (struct te_s *)malloc(msize);
  memset(obj, 0x00, msize);
  
  return obj;
}


struct te_s *
te_destroy (struct te_s *obj)
{
  return(obj);
}


struct te_s *
te_init (struct te_s * obj)
{
  if (!obj) return obj;
  
  tag_init(&(obj->tag), TAGID_TE, 3, 0, 0);
  icbtag_init(&(obj->icbtag), 0, 0, 0, 0, ICBFT_TE, NULL, ADTYPE_DIRECT);

  return obj;
}


void
te_free (struct te_s * obj)
{
  free(te_destroy(obj));
}


struct te_s *
te_decode (const uint8_t * space, int spacelen)
{
  struct te_s * obj = NULL;
  layoutvalue_t contents[3] = { 0, };

  if (!obj) obj = te_malloc();
  if (!obj || !space || !spacelen) return obj;

  udf_decode(space, spacelen, udf_te, contents);

  { struct tag_s * sub = tag_decode(contents[0].ptr, 16); obj->tag = *sub; tag_free(sub); }

  { struct icbtag_s * sub = icbtag_decode(contents[1].ptr, 20); obj->icbtag = *sub; icbtag_free(sub); }

  return obj;
}


int
te_encode (const struct te_s * obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[3] = { 0, };

  if (!obj) return 0;
  if (!obj || !space || !spacelen) return 0;

  uint8_t tag[16] = { 0, }; tag_encode(&(obj->tag), tag, 16); contents[0].ptr = tag;

  uint8_t icbtag[20] = { 0, }; icbtag_encode(&(obj->icbtag), icbtag, 20); contents[1].ptr = icbtag;

  int retval = udf_encode(space, spacelen, udf_te, contents);
  if (retval != contents[3-1].word) { }
  return retval;
}


int
te_cmp (const struct te_s *a, const struct te_s *b)
{
  int retval = -1;

  retval = memcmp(a,b,sizeof(*a));

  return retval;
}


int
te_len (const struct te_s *obj)
{
  int retval = 0;
  retval = 36;
  return retval;
}



int
te_repr (const struct te_s *obj, char buf[], int buflen)
{
  int n = 0;
//  char tmp[buflen/2] = { 0, };
  char tmp[512] = { 0, };

  n += snprintf(buf+n, buflen-1, "struct te_s _%p = {\n", obj);

  tag_encode(&(obj->tag), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .tag = %s,\n", tmp);

  icbtag_encode(&(obj->icbtag), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .icbtag = %s,\n", tmp);

  n += snprintf(buf+n, buflen-n, "}");

  return n;
}


void
te_dump (const struct te_s *obj)
{
  char buf[1024] = { 0, };
  int n = te_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}


