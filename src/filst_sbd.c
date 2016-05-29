#include "filst_macros.h"


struct layoutfield_s udf_sbd[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "N_BT", LAYOUT_UINT32 },
    { 0, 20, "N_B", LAYOUT_UINT32 },
    { 0, 24, "bm", LAYOUT_PTR },
    { (1<<2), 24, 0, LAYOUT_END },
};


COMMON_FILST(sbd, TAGID_SBD)


FILST_ISA(sbd)
{
  STD_ISA(sbd);
}

FILST_INIT0(sbd)
{
  self->tag.tagid = TAGID_SBD;
  self->tag.vers = 3;
  self->tag.serial = 1;

}

FILST_MALLOC(sbd)
{
  SELFNEW0(sbd);
  return self;
}

FILST_DESTROY(sbd)
{
  return self;
}

FILST_FREE(sbd)
{
  SELFFREE(sbd);
}

FILST_DECODE(sbd)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(6);

  STD_DECODE(sbd);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_WORD(1, N_BT);
  DECODE_WORD(2, N_B);

  memcpy(self->d, contents[3].ptr, self->N_B);
  self->bm = self->d;

  //DECODE_CHECK(3, 24);

  RETURN_DECODE(sbd);
}

FILST_ENCODE(sbd)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(6);
  ENCODINGVARS;

  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_WORD(1, N_BT);
  ENCODE_WORD(2, N_B);
  contents[3].ptr = self->bm;

  STD_ENCODE(sbd);

  RETURN_ENCODE(sbd);
}

FILST_CMP(sbd)
{
  PLACEHOLDER_CMP(sbd);
}

FILST_LEN(sbd)
{
  size_t msize = 24;
  msize += self->N_B;
  return msize;
}

FILST_REPR(sbd)
{
  REPRVARS(sbd);

  REPR_HEADER(sbd);
  REPR_STRUCT(tag, tag);
  REPR_ATOM(N_BT, "%u");
  REPR_ATOM(N_B, "%u");
  REPR_LITERAL("  [bitmap] //TODO\n");
  REPR_FOOTER(sbd);

  RETURN_REPR(sbd);
}

FILST_DUMP(sbd)
{
  STD_DUMP(sbd, 1024);
}





FILST_THUNKS(TAGID_TE, sbd)

FILST_FUNCTABLE(sbd);

