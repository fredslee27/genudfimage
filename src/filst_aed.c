#include "filst_macros.h"


struct layoutfield_s udf_aed[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "pael", LAYOUT_UINT32 },
    { 0, 20, "L_AD", LAYOUT_UINT32 },
    { 0, 24, 0, LAYOUT_END },
};


COMMON_FILST(aed, TAGID_AED)

FILST_ISA(aed)
{
  STD_ISA(aed);
}

FILST_INIT0(aed)
{
  self->tag.tagid = TAGID_AED;
  self->tag.vers = 3;
  self->tag.serial = 1;
}

FILST_MALLOC(aed)
{
  SELFNEW0(aed);

  self->dlen = dlen;
  self->adtype = ADTYPE_DIRECT;
  self->ad.direct = self->d;

  return self;
}


struct aed_s *
aed_short_malloc (int nad)
{
  int dlen = sizeof(struct short_ad_s) * nad;
  struct aed_s * self = aed_malloc(dlen);
  self->adtype = ADTYPE_SHORT;
  self->ad.short_ad = (struct short_ad_s *)(self->d);
  self->nad = nad;

  return self;
}

struct aed_s *
aed_long_malloc (int nad)
{
  int dlen = sizeof(struct long_ad_s) * nad;
  struct aed_s * self = aed_malloc(dlen);
  self->adtype = ADTYPE_LONG;
  self->ad.long_ad = (struct long_ad_s *)(self->d);
  self->nad = nad;

  return self;
}

struct aed_s *
aed_ext_malloc (int nad)
{
  int dlen = sizeof(struct ext_ad_s) * nad;
  struct aed_s * self = aed_malloc(dlen);
  self->adtype = ADTYPE_EXT;
  self->ad.ext_ad = (struct ext_ad_s *)(self->d);
  self->nad = nad;

  return self;
}

FILST_DESTROY(aed)
{
  return self;
}

FILST_FREE(aed)
{
  SELFFREE(aed);
}


FILST_DECODE(aed)
{
  CONVERSION_SANITY NULL;
  CONTENTSVAR(4);

  STD_DECODE(aed);

  size_t dlen = contents[2].word;

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_WORD(1, pael);
  DECODE_WORD(2, dlen);

  memcpy(self->d, space+24, spacelen-24);

  // TODO: ensure total size of descriptor+AD does not exceed sector size.

  return self;
}

FILST_ENCODE(aed)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(4);
  ENCODINGVARS;

  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_WORD(1, pael);
  ENCODE_WORD(2, dlen);

#if 0
  size_t dlen;

  contents[0].ptr = tag;
  contents[1].word = obj->pael;
  /* TODO: calculate packed. */
  dlen = obj->dlen;
  contents[2].word = obj->dlen;
#endif //

  // TODO: encode array of allocation descriptors.

//  int retval = udf_encode(space, spacelen, udf_aed, contents);
  STD_ENCODE(aed);

  RETURN_ENCODE(aed);
}

/* Calculate space needed for encode() */
FILST_LEN(aed)
{
  size_t msize = 24;
  // TODO: calculate this from 'nad'.
  msize += self->dlen;

  return msize;
}

FILST_CMP(aed)
{
  return -1;
}

FILST_REPR(aed)
{
  REPRVARS(aed);

  REPR_HEADER(aed);
  REPR_STRUCT(tag,tag);
  REPR_ATOM(pael,"%u");
  REPR_ATOM(dlen,"%u");
  // TODO: involve nad
  REPR_FOOTER(aed);

  RETURN_REPR(aed);
}

FILST_DUMP(aed)
{
  STD_DUMP(aed, 1024);
}

