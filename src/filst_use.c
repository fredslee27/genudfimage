#include "filst_macros.h"


struct layoutfield_s udf_use[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "icbtag", LAYOUT_PTR },
    { 0, 36, "L_AD", LAYOUT_UINT32 },
    { 0, 40, "ad", LAYOUT_PTR },
    { (1<<2), 40, 0, LAYOUT_END },
};


COMMON_FILST(use, TAGID_USE)


FILST_ISA(use)
{
  STD_ISA(use);
}

FILST_INIT0(use)
{
  self->tag.tagid = TAGID_USE;
  self->tag.vers = 3;
  self->tag.serial = 1;

  self->icbtag.ft = ICBFT_USE;
}

FILST_MALLOC(use)
{
  SELFNEW0(use);
  return self;
}

FILST_DESTROY(use)
{
  return self;
}

FILST_FREE(use)
{
  SELFFREE(use);
}

FILST_DECODE(use)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(5);

  STD_DECODE(use);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_STRUCT(1, 20, icbtag,icbtag);
  DECODE_WORD(2, L_AD);
  //DECODE_CHECK(3, 24);
  // TODO: AllocationDescriptors[]

  RETURN_DECODE(use);
}

FILST_ENCODE(use)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(5);
  ENCODINGVARS;

  // TODO: AllocationDescriptors[]
  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_STRUCT(1, 20, icbtag,icbtag);
  ENCODE_WORD(2, L_AD);

  STD_ENCODE(use);

  RETURN_ENCODE(use);
}

FILST_CMP(use)
{
  PLACEHOLDER_CMP(use);
}

FILST_LEN(use)
{
  size_t msize = 40;
  // TODO: encoded sizes of AllocationDescriptors[]
  return msize;
}

FILST_REPR(use)
{
  REPRVARS(use);

  REPR_HEADER(use);
  REPR_STRUCT(tag, tag);
  REPR_STRUCT(icbtag, icbtag);
  REPR_ATOM(L_AD, "%u");
  // TODO: AllocationDescriptors[]
  REPR_LITERAL("  ... //TODO\n");
  REPR_FOOTER(use);

  RETURN_REPR(use);
}

FILST_DUMP(use)
{
  STD_DUMP(use, 4096);
}





FILST_THUNKS(TAGID_TE, use)

FILST_FUNCTABLE(use);

