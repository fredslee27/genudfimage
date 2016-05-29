#include "filst_macros.h"


struct layoutfield_s udf_efe[] = {
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
    { 0, 64, "os", LAYOUT_UINT64 },
    { 0, 72, "lbr", LAYOUT_UINT64 },
    { 0, 80, "adt", LAYOUT_PTR },
    { 0, 92, "mdt", LAYOUT_PTR },
    { 0, 104, "cdt", LAYOUT_PTR },
    { 0, 116, "tdt", LAYOUT_PTR },
    { 0, 128, "ckpt", LAYOUT_UINT32 },
    { 0, 132, 0, LAYOUT_RESERVED },
    { 0, 136, "eaicb", LAYOUT_PTR },
    { 0, 152, "sdicb", LAYOUT_PTR },
    { 0, 168, "impid", LAYOUT_PTR },
    { 0, 200, "uniqid", LAYOUT_UINT64 },
  { 0, 208, 0, LAYOUT_END },
//    { 0, 208, "L_EA", LAYOUT_UINT32 },
//    { 0, 212, "L_AD", LAYOUT_UINT32 },
//    { 0, 216, "ea", LAYOUT_PTR },
//    { (1<<22), 176, "ad", LAYOUT_PTR },
//    { (1<<22)|(1<<23), 176, 0, LAYOUT_END },
};


COMMON_FILST(efe, TAGID_EFE)


FILST_ISA(efe)
{
  STD_ISA(efe);
}

FILST_INIT0(efe)
{
  self->tag.tagid = TAGID_EFE;
  self->tag.vers = 3;
  self->tag.serial = 1;

  self->icbtag.ft = ICBFT_REG;
}

FILST_MALLOC(efe)
{
  SELFNEW0(efe);
  return self;
}

FILST_DESTROY(efe)
{
  return self;
}

FILST_FREE(efe)
{
  SELFFREE(efe);
}

FILST_DECODE(efe)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(28);

  STD_DECODE(efe);

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
  DECODE_WORD(10, os);
  DECODE_WORD(11, lbr);
  DECODE_STRUCT(12, 12, timestamp,adt);
  DECODE_STRUCT(13, 12, timestamp,mdt);
  DECODE_STRUCT(14, 12, timestamp,cdt);
  DECODE_STRUCT(15, 12, timestamp,tdt);
  DECODE_WORD(16, ckpt);
  // 17 reserved
  DECODE_STRUCT(18, 16, long_ad,eaicb);
  DECODE_STRUCT(19, 16, long_ad,sdicb);
  DECODE_STRUCT(20, 32, regid,impid);
  DECODE_WORD(21, uniqid);
//  DECODE_WORD(22, L_EA);
//  DECODE_WORD(23, L_AD);
  // TODO: ExtendedAttributes[]
  // TODO: AllocationDescriptors[]

  RETURN_DECODE(efe);
}

FILST_ENCODE(efe)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(28);
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
  ENCODE_WORD(10, os);
  ENCODE_WORD(11, lbr);
  ENCODE_STRUCT(12, 12, timestamp,adt);
  ENCODE_STRUCT(13, 12, timestamp,mdt);
  ENCODE_STRUCT(14, 12, timestamp,cdt);
  ENCODE_STRUCT(15, 12, timestamp,tdt);
  ENCODE_WORD(16, ckpt);
  // 17 reserved
  ENCODE_STRUCT(18, 16, long_ad,eaicb);
  ENCODE_STRUCT(19, 16, long_ad,sdicb);
  ENCODE_STRUCT(20, 32, regid,impid);
  ENCODE_WORD(21, uniqid);
//  ENCODE_WORD(22, L_EA);
//  DECODE_WORD(23, L_AD);
//  contents[24].ptr = self->d;
//  contents[25].ptr = self->d;

  STD_ENCODE(efe);

  RETURN_ENCODE(efe);
}

FILST_CMP(efe)
{
  PLACEHOLDER_CMP(efe);
}

FILST_LEN(efe)
{
  size_t msize = 216;
  // TODO: encoded sizes of ExtendedAttributes[] and AllocationDescripors[]
  return msize;
}

FILST_REPR(efe)
{
  REPRVARS(efe);

  REPR_HEADER(efe);
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
  REPR_ATOM(os, "%llu");
  REPR_ATOM(lbr, "%llu");
  REPR_STRUCT(timestamp, adt);
  REPR_STRUCT(timestamp, mdt);
  REPR_STRUCT(timestamp, cdt);
  REPR_STRUCT(timestamp, tdt);
  REPR_ATOM(ckpt, "%u");
  REPR_STRUCT(long_ad, eaicb);
  REPR_STRUCT(long_ad, sdicb);
  REPR_STRUCT(regid, impid);
  REPR_ATOM(uniqid, "%llu");
  // TODO: EA[]
  // TODO: AD[]
  REPR_LITERAL("  ... //TODO\n");
  REPR_FOOTER(efe);

  RETURN_REPR(efe);
}

FILST_DUMP(efe)
{
  STD_DUMP(efe, 4096);
}





FILST_THUNKS(TAGID_TE, efe)

FILST_FUNCTABLE(efe);

