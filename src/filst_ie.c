#include "filst_macros.h"


struct layoutfield_s udf_ie[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "icbag", LAYOUT_PTR },
    { 0, 36, "iicb", LAYOUT_PTR },
    { 0, 52, 0, LAYOUT_END },
};


COMMON_FILST_TAGTYPE(ie, TAGID_IE)
COMMON_FILST_NFIELDS(ie)
COMMON_FILST_LAYOUTDESCR(ie)


FILST_INIT0(ie)
{
  self->tag.tagid = ie_tagtype();
  self->tag.vers = 3;
  self->tag.serial = 1;

  self->icbtag.ft = ICBFT_IE;
}


FILST_MALLOC(ie)
{
  SELFNEW0(ie);
  return self;
}


FILST_DESTROY(ie)
{
  return self;
}

FILST_FREE(ie)
{
  SELFFREE(ie);
}

FILST_DECODE(ie)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(4);

  STD_DECODE(ie);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_STRUCT(1, 20, icbtag,icbtag);
  DECODE_STRUCT(2, 16, long_ad,iicb);

  RETURN_DECODE(ie);
}

FILST_ENCODE(ie)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(4);
  ENCODINGVARS;

  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_STRUCT(1, 20, icbtag,icbtag);
  ENCODE_STRUCT(2, 16, long_ad,iicb);

  STD_ENCODE(ie);

  RETURN_ENCODE(ie);
}

FILST_LEN(ie)
{
  return 52;
}

FILST_CMP(ie)
{
  PLACEHOLDER_CMP(ie);
}

FILST_REPR(ie)
{
  REPRVARS(ie);

  REPR_HEADER(ie);
  REPR_STRUCT(tag,tag);
  REPR_STRUCT(icbtag,icbtag);
  REPR_STRUCT(long_ad,iicb);
  REPR_FOOTER(ie);

  RETURN_REPR(ie);
}

FILST_DUMP(ie)
{
  STD_DUMP(ie, 1024);
}




FILST_THUNKS(TAGID_IE, ie)

FILST_FUNCTABLE(ie);


