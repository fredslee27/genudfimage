#include "filst_macros.h"

struct layoutfield_s udf_fsd[] = {
    { 0, 0, "tag", LAYOUT_PTR },
    { 0, 16, "rdt", LAYOUT_PTR },
    { 0, 28, "il", LAYOUT_UINT16 },
    { 0, 30, "mil", LAYOUT_UINT16 },
    { 0, 32, "csl", LAYOUT_UINT32 },
    { 0, 36, "mcsl", LAYOUT_UINT32 },
    { 0, 40, "fsn", LAYOUT_UINT32 },
    { 0, 44, "fsdn", LAYOUT_UINT32 },
    { 0, 112, "lvidcs", LAYOUT_PTR },
    { 0, 240, "lvid", LAYOUT_PTR },
    { 0, 304, "fscs", LAYOUT_PTR },
    { 0, 336, "fsid", LAYOUT_PTR },
    { 0, 368, "cfid", LAYOUT_PTR },
    { 0, 400, "afid", LAYOUT_PTR },
    { 0, 416, "rdicb", LAYOUT_PTR },
    { 0, 448, "domid", LAYOUT_PTR },
    { 0, 464, "ne",LAYOUT_PTR },
    { 0, 480, "ssdicb", LAYOUT_PTR },
    { 0, 480, "reserved", LAYOUT_RESERVED },
    { 0, 512, 0, LAYOUT_END },
};



COMMON_FILST(fsd, TAGID_FSD);


FILST_INIT0(fsd)
{
  self->tag.tagid = fsd_tagtype();
  self->tag.vers = 3;
  self->tag.serial = 1;

  // TODO: interchange levels
}

FILST_MALLOC(fsd)
{
  SELFNEW0(fsd);
  return self;
}

FILST_DESTROY(fsd)
{
  return self;
}

FILST_FREE(fsd)
{
  SELFFREE(fsd);
}

FILST_DECODE(fsd)
{
  CONVERSION_SANITY self;
  CONTENTSVAR(20);

  STD_DECODE(fsd);

  DECODE_STRUCT(0, 16, tag,tag);
  DECODE_STRUCT(1, 12, timestamp,rdt);

  DECODE_WORD(2, il);
  DECODE_WORD(3, mil);
  DECODE_WORD(4, csl);
  DECODE_WORD(5, mcsl);
  DECODE_WORD(6, fsn);
  DECODE_WORD(7, fsdn);

  DECODE_STRUCT(8, 64, charspec,lvidcs);
  DECODE_STRUCT(9, 128, dstring,lvid);
  DECODE_STRUCT(10, 64, charspec,fscs);
  DECODE_STRUCT(11, 32, dstring,fsid);
  DECODE_STRUCT(12, 32, dstring,cfid);
  DECODE_STRUCT(13, 32, dstring,afid);
  DECODE_STRUCT(14, 16, long_ad,rdicb);
  DECODE_STRUCT(15, 32, regid,domid);
  DECODE_STRUCT(16, 16, long_ad,ne);
  DECODE_STRUCT(17, 16, long_ad,ssdicb);

  // TODO: ensure reserved is #00.

  DECODE_CHECK(18, 512);

  return self;
}

FILST_ENCODE(fsd)
{
  CONVERSION_SANITY 0;
  CONTENTSVAR(20);
  ENCODINGVARS;

  ENCODE_STRUCT(0, 16, tag,tag);
  ENCODE_STRUCT(1, 12, timestamp,rdt);

  ENCODE_WORD(2, il);
  ENCODE_WORD(3, mil);
  ENCODE_WORD(4, csl);
  ENCODE_WORD(5, mcsl);
  ENCODE_WORD(6, fsn);
  ENCODE_WORD(7, fsdn);

  ENCODE_STRUCT(8, 64, charspec,lvidcs);
  ENCODE_STRUCT(9, 128, dstring,lvid);
  ENCODE_STRUCT(10, 64, charspec,fscs);
  ENCODE_STRUCT(11, 32, dstring,fsid);
  ENCODE_STRUCT(12, 32, dstring,cfid);
  ENCODE_STRUCT(13, 32, dstring,afid);
  ENCODE_STRUCT(14, 16, long_ad,rdicb);
  ENCODE_STRUCT(15, 32, regid,domid);
  ENCODE_STRUCT(16, 16, long_ad,ne);
  ENCODE_STRUCT(17, 16, long_ad,ssdicb);

  contents[18].word = 512;

  STD_ENCODE(fsd);

  RETURN_ENCODE(fsd);
}

FILST_CMP(fsd)
{
  PLACEHOLDER_CMP(fsd);
}

FILST_REPR(fsd)
{
  REPRVARS(fsd);

  REPR_HEADER(fsd);
  REPR_STRUCT(tag,tag);
  REPR_STRUCT(timestamp,rdt);
  REPR_ATOM(il,"%u");
  REPR_ATOM(mil,"%u");
  REPR_ATOM(csl,"%u");
  REPR_ATOM(mcsl,"%u");
  REPR_ATOM(fsn,"%u");
  REPR_ATOM(fsdn,"%u");
  REPR_STRUCT(charspec,lvidcs);

  REPR_STRUCT(charspec,fscs);
  REPR_STRUCT(dstring,fsid);
  REPR_STRUCT(dstring,cfid);
  REPR_STRUCT(dstring,afid);
  REPR_STRUCT(long_ad,rdicb);
  REPR_STRUCT(regid,domid);
  REPR_STRUCT(long_ad,ne);
  REPR_STRUCT(long_ad,ssdicb);

  REPR_FOOTER(fsd);

  RETURN_REPR(fsd);
}

FILST_DUMP(fsd)
{
  STD_DUMP(fsd,2048);
}


