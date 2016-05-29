#include "filst_macros.h"


struct layoutfield_s udf_eahd[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "ial", LAYOUT_UINT32 },
    { 0, 20, "aal", LAYOUT_UINT32 },
    { 0, 24, 0, LAYOUT_END },
};


COMMON_FILST(eahd, TAGID_EAHD)


FILST_ISA(eahd)
{
  STD_ISA(eahd);
}

FILST_INIT0(eahd)
{
  self->tag.tagid = TAGID_EAHD;
  self->tag.vers = 3;
  self->tag.serial = 1;
}

FILST_MALLOC(eahd)
{
  SELFNEW0(eahd);
  return self;
}

FILST_DESTROY(eahd)
{
  return self;
}

FILST_FREE(eahd)
{
  SELFFREE(eahd);
}

FILST_DECODE(eahd)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(4);

  STD_DECODE(eahd);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_WORD(1, ial);
  DECODE_WORD(2, aal);
  DECODE_CHECK(3, 24);
  // TODO: ImplementationAttributes[], ApplicationAttributes[]

  RETURN_DECODE(eahd);
}

FILST_ENCODE(eahd)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(4);
  ENCODINGVARS;

  // TODO: ImplementationAttributes[], ApplicationAttributes[]
  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_WORD(1, ial);
  ENCODE_WORD(2, aal);

  STD_ENCODE(eahd);

  RETURN_ENCODE(eahd);
}

FILST_CMP(eahd)
{
  PLACEHOLDER_CMP(eahd);
}

FILST_LEN(eahd)
{
  size_t msize = 24;
  // TODO: encoded sizes of ImplementationAttributes[], ApplicationAttributes[]
  return msize;
}

FILST_REPR(eahd)
{
  REPRVARS(eahd);

  REPR_HEADER(eahd);
  REPR_STRUCT(tag, tag);
  REPR_ATOM(ial, "%u");
  REPR_ATOM(aal, "%u");
  // TODO: ImplementationAttributes[]
  // TODO: ApplicationAttributes[]
  REPR_LITERAL("  ... //TODO\n");
  REPR_FOOTER(eahd);

  RETURN_REPR(eahd);
}

FILST_DUMP(eahd)
{
  STD_DUMP(eahd, 4096);
}





FILST_THUNKS(TAGID_TE, eahd)

FILST_FUNCTABLE(eahd);

