#include "filst_macros.h"


struct layoutfield_s udf_fe[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "icbtag", LAYOUT_PTR },
    { 0, 36, "uid", LAYOUT_UINT32 },
    { 0, 40, "gid", LAYOUT_UINT32 },
    { 0, 44, "p", LAYOUT_UINT32 },
    { 0, 48, "flc", LAYOUT_UINT16 },
    { 0, 50, "rf", LAYOUT_UINT8 },
    { 0, 51, "rda", LAYOUT_UINT8 },
    { 0, 52, "rl", LAYOUT_UINT32 },
    { 0, 56, "il", LAYOUT_UINT64 },
    { 0, 64, "lbr", LAYOUT_UINT64 },
    { 0, 74, "adt", LAYOUT_PTR },
    { 0, 84, "mdt", LAYOUT_PTR },
    { 0, 96, "cdt", LAYOUT_PTR },
    { 0, 108, "ckpt", LAYOUT_UINT32 },
    { 0, 112, "eaicb", LAYOUT_PTR },
    { 0, 128, "impid", LAYOUT_PTR },
    { 0, 160, "uniqid", LAYOUT_UINT64 },
    { 0, 168, "L_EA", LAYOUT_UINT32 },
    { 0, 172, "L_AD", LAYOUT_UINT32 },
    { 0, 176, "ea", LAYOUT_PTR },
    { (1<<19), 176, "ad", LAYOUT_PTR },
    { (1<<19)|(1<<20), 176, 0, LAYOUT_END },
};


COMMON_FILST(fe, TAGID_FE)


FILST_ISA(fe)
{
  STD_ISA(fe);
}

FILST_INIT0(fe)
{
  self->tag.tagid = TAGID_FE;
  self->tag.vers = 3;
  self->tag.serial = 1;

  self->icbtag.ft = ICBFT_REG;
}

FILST_MALLOC(fe)
{
  SELFNEW0(fe);
  return self;
}

FILST_DESTROY(fe)
{
  return self;
}

FILST_FREE(fe)
{
  SELFFREE(fe);
}

FILST_DECODE(fe)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(22);

  STD_DECODE(fe);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_STRUCT(1, 20, icbtag,icbtag);
  DECODE_WORD(2, uid);
  DECODE_WORD(3, gid);
  DECODE_WORD(4, p);
  DECODE_WORD(5, flc);
  DECODE_WORD(6, rf);
  DECODE_WORD(7, rda);
  DECODE_WORD(8, rl);
  DECODE_WORD(9, il);
  DECODE_WORD(10, lbr);
  DECODE_STRUCT(11, 12, timestamp,adt);
  DECODE_STRUCT(12, 12, timestamp,mdt);
  DECODE_STRUCT(13, 12, timestamp,cdt);
  DECODE_WORD(14, ckpt);
  DECODE_STRUCT(15, 16, long_ad,eaicb);
  DECODE_STRUCT(16, 32, regid,impid);
  DECODE_WORD(17, uniqid);
//  DECODE_WORD(18, L_EA);
//  DECODE_WORD(19, L_AD);
  // TODO: ExtendedAttributes[]
  // TODO: AllocationDescriptors[]

  RETURN_DECODE(fe);
}

FILST_ENCODE(fe)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(22);
  ENCODINGVARS;

  // TODO: ExtendedAttributes[]
  // TODO: AllocationDescritors[]
  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_STRUCT(1, 20, icbtag,icbtag);
  ENCODE_WORD(2, uid);
  ENCODE_WORD(3, gid);
  ENCODE_WORD(4, p);
  ENCODE_WORD(5, flc);
  ENCODE_WORD(6, rf);
  ENCODE_WORD(7, rda);
  ENCODE_WORD(8, rl);
  ENCODE_WORD(9, il);
  ENCODE_WORD(10, lbr);
  ENCODE_STRUCT(11, 12, timestamp,adt);
  ENCODE_STRUCT(12, 12, timestamp,mdt);
  ENCODE_STRUCT(13, 12, timestamp,cdt);
  ENCODE_WORD(14, ckpt);
  ENCODE_STRUCT(15, 16, long_ad,eaicb);
  ENCODE_STRUCT(16, 32, regid,impid);
  ENCODE_WORD(17, uniqid);
//  ENCODE_WORD(18, L_EA);
//  ENCODE_WORD(19, L_AD);

  STD_ENCODE(fe);

  RETURN_ENCODE(fe);
}

FILST_CMP(fe)
{
  PLACEHOLDER_CMP(fe);
}

FILST_LEN(fe)
{
  size_t msize = 176;
  // TODO: encoded sizes of ExtendedAttributes[] and AllocationDescripors[]
  return msize;
}

FILST_REPR(fe)
{
  REPRVARS(fe);

  REPR_HEADER(fe);
  REPR_STRUCT(tag, tag);
  REPR_STRUCT(icbtag, icbtag);
  REPR_ATOM(uid, "%u");
  REPR_ATOM(gid, "%u");
  REPR_ATOM(p, "%08x");
  REPR_ATOM(flc, "%u");
  REPR_ATOM(rf, "%u");
  REPR_ATOM(rda, "%u");
  REPR_ATOM(rl, "%u");
  REPR_ATOM(il, "%llu");
  REPR_ATOM(lbr, "%llu");
  REPR_STRUCT(timestamp, adt);
  REPR_STRUCT(timestamp, mdt);
  REPR_STRUCT(timestamp, cdt);
  REPR_ATOM(ckpt, "%u");
  REPR_STRUCT(long_ad, eaicb);
  REPR_STRUCT(regid, impid);
  REPR_ATOM(uniqid, "%llu");
  // TODO: EA[]
  // TODO: AD[]
  REPR_LITERAL("  ... //TODO\n");
  REPR_FOOTER(fe);

  RETURN_REPR(fe);
}

FILST_DUMP(fe)
{
  STD_DUMP(fe, 4096);
}





FILST_THUNKS(TAGID_TE, fe)

FILST_FUNCTABLE(fe);

