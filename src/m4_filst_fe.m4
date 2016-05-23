define(classname,fe)dnl
include(`defs_filst.m4')dnl
dnl changequote(`??^', `??$')dnl


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
  { 0, 72, "adt", LAYOUT_PTR },
  { 0, 84, "mdt", LAYOUT_PTR },
  { 0, 96, "cdt", LAYOUT_PTR },
  { 0, 108, "ckpt", LAYOUT_UINT32 },
  { 0, 112, "eaicb", LAYOUT_PTR },
  { 0, 128, "impid", LAYOUT_PTR },
  { 0, 160, "uniqid", LAYOUT_UINT64 },
  { 0, 168, "L_EA", LAYOUT_UINT32 },
  { 0, 172, "L_AD", LAYOUT_UINT32 },
  { 0, 176, "ea", LAYOUT_PTR },
  { (1<<18), 176, "ad", LAYOUT_PTR },
  { (1<<18)|(1<<19), 176, 0, LAYOUT_END },
};


dnl INSTFUNC_malloc(`relevant_size_parameter'=`', `extra_setup_body'=`')
INSTFUNC_malloc(??^adlen??$)


dnl INSTFUNC_destroy(`cleanup_body'=`')
INSTFUNC_destroy()


dnl INSTFUNC_init(`parameters'=`', `init_body'=`')
INSTFUNC_init(,??^
  tag_init(&(obj->tag), TAGID_FE, 3, 0, 0);
  icbtag_init(&(obj->icbtag), 0, 0, 0, 0, ICBFT_REG, NULL, ADTYPE_DIRECT);
??$)


dnl INSTFUNC_free()
INSTFUNC_free()


dnl INSTFUNC_decode(`num_contents_fields', `decoding_body'=`')
INSTFUNC_decode(21, ??^
dnl  DecodingTag(tag,0)
  DecodingStruct(tag,16,tag,0)

  DecodingStruct(icbtag,20,icbtag,1)

  obj->uid = contents[2].word;
  obj->gid = contents[3].word;
  obj->p = contents[4].word;
  obj->flc = contents[5].word;
  obj->rf = contents[6].word;
  obj->rda = contents[7].word;
  obj->rl = contents[8].word;
  obj->il = contents[9].word;
  obj->lbr = contents[10].word;

  DecodingStruct(timestamp,12,adt,11)
  DecodingStruct(timestamp,12,mdt,12)
  DecodingStruct(timestamp,12,cdt,13)

  obj->ckpt = contents[14].word;
  DecodingStruct(long_ad,16,eaicb,15);
  DecodingStruct(regid,32,impid,16);
  obj->uniqid = contents[17].word;
  obj->L_EA = contents[18].word;
  obj->L_AD = contents[19].word;
??$)


dnl INSTFUNC_encode(`num_contents_fields', `encoding_body'=`')
INSTFUNC_encode(21, ??^
  EncodingStruct(tag,16,tag,0)

  EncodingStruct(icbtag,20,icbtag,1)
??$)


dnl INSTFUNC_cmp(`comparison_body'=`return memcmp(a,b,sizeof(*a));')
INSTFUNC_cmp(??^
  retval = memcmp(a,b,sizeof(*a));
??$)


dnl INSTFUNC_len(`length_expression')
INSTFUNC_len(36)


dnl REPRLINE(`fmt', `printf_argument')
dnl REPRATOM(`field_name', `fmt_spec', `obj_field')
dnl REPRSTRUCT(`field_name', `struct_name', 'obj_field')

dnl INSTFUNC_repr()
INSTFUNC_repr()


dnl INSTFUNC_dump(`internal_buffer_size'=`1024')
INSTFUNC_dump()


