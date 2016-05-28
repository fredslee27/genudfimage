#include "filst.h"
#include "util.h"

#include "filst_macros.h"


struct layoutfield_s udf_te[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "icbtag", LAYOUT_PTR },
    { 0, 36, 0, LAYOUT_END },
};


COMMON_FILST_TAGTYPE(te, TAGID_TE)
COMMON_FILST_NFIELDS(te)
COMMON_FILST_LAYOUTDESCR(te)


FILST_ISA(te)
{
  STD_ISA(te);
}

FILST_INIT0(te)
{
  self->tag.tagid = TAGID_TE;
  self->tag.vers = 3;
  self->tag.serial = 1;

  self->icbtag.ft = ICBFT_TE;
}

FILST_MALLOC(te)
{
  SELFNEW0(te);
  return self;
}

FILST_DESTROY(te)
{
  return self;
}

#if 0
FILST_INIT(te)
{
  SELF_BAILNULL;

  tag_init(&(self->tag), TAGID_TE, 3, 0, 0);
  icbtag_init(&(self->icbtag), 0, 0, 0, 0, ICBFT_TE, NULL, ADTYPE_DIRECT);

  return FILST_TYPE(self);
}
#endif //0

FILST_FREE(te)
{
  SELFFREE(te);
}

FILST_DECODE(te)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(3);

  STD_DECODE(te);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_STRUCT(1, 20, icbtag,icbtag);

  RETURN_DECODE(te);
}

FILST_ENCODE(te)
{
  CONVERSION_SANITY 0;
//  SELFVARCONST(te);
  CONTENTSVAR(3);
  ENCODINGVARS;

  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_STRUCT(1, 20, icbtag,icbtag);

  STD_ENCODE(te);

  RETURN_ENCODE(te);
}

FILST_CMP(te)
{
  PLACEHOLDER_CMP(te);
}

FILST_LEN(te)
{
  return 36;
}

FILST_REPR(te)
{
  REPRVARS(te);

  REPR_HEADER(te);
  REPR_STRUCT(tag, tag);
  REPR_STRUCT(icbtag, icbtag);
  REPR_FOOTER(te);

  RETURN_REPR;
}

FILST_DUMP(te)
{
  STD_DUMP(te, 1024);
}





FILST_THUNKS(TAGID_TE, te)

FILST_FUNCTABLE(te);

