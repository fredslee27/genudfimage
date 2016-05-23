define(classname,te)dnl
include(`defs_filst.m4')dnl
dnl changequote(`??^', `??$')dnl


struct layoutfield_s udf_te[] = {
  { 0, 0, "tag", LAYOUT_PTR },
  { 0, 16, "icbtag", LAYOUT_PTR },
  { 0, 36, 0, LAYOUT_END },
};


dnl INSTFUNC_malloc(`relevant_size_parameter'=`', `extra_setup_body'=`')
INSTFUNC_malloc()


dnl INSTFUNC_destroy(`cleanup_body'=`')
INSTFUNC_destroy()


dnl INSTFUNC_init(`parameters'=`', `init_body'=`')
INSTFUNC_init(,??^
  tag_init(&(obj->tag), TAGID_TE, 3, 0, 0);
  icbtag_init(&(obj->icbtag), 0, 0, 0, 0, ICBFT_TE, NULL, ADTYPE_DIRECT);
??$)


dnl INSTFUNC_free()
INSTFUNC_free()


dnl INSTFUNC_decode(`num_contents_fields', `decoding_body'=`')
INSTFUNC_decode(3, ??^
dnl  DecodingTag(tag,0)
  DecodingStruct(tag,16,tag,0)

  DecodingStruct(icbtag,20,icbtag,1)
??$)


dnl INSTFUNC_encode(`num_contents_fields', `encoding_body'=`')
INSTFUNC_encode(3, ??^
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


