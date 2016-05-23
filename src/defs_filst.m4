/* m4 macros, filst */
#include <stdio.h>
#include <string.h>

#include "basics.h"
#include "conv.h"
#include "filst.h"

changequote(`??^', `??$')dnl
define(classname, ??^te??$)dnl
dnl
dnl CLASS(`classname')
define(CLASS,??^define(classname,??^$1??$)??$)dnl
dnl
dnl
dnl INSTFUNC_malloc(`extrasize', `extrasetup')
define(??^INSTFUNC_malloc??$,??^struct ??$classname??^_s *
??$classname??^_malloc ()
{
  struct ??$classname??^_s * obj = NULL;
  size_t msize = sizeof(struct ??$classname??^_s) + ifelse($1,,0,$1);
  obj = (struct ??$classname??^_s *)malloc(msize);
  memset(obj, 0x00, msize);
  ifelse($2,??$??^,,{$2})
  return obj;
}??$)dnl
dnl
dnl
dnl INSTFUNC_destroy(`cleanup')
define(??^INSTFUNC_destroy??$,??^struct ??$classname??^_s *
??$classname??^_destroy (struct ??$classname??^_s *obj)
{
ifelse($1,,,??^  {
    $1
  }
??$)dnl
  return(obj);
}??$)dnl
dnl
dnl
dnl INSTFUNC_init(`parameters', `extrabody')
define(??^INSTFUNC_init??$,??^struct ??$classname??^_s *
??$classname??^_init (struct ??$classname??^_s * ifelse(??^$1??$,,??^obj??$,??^obj, $1??$))
{
  if (!obj) return obj;
ifelse(??^$2??$,,,??^  $2??$)
  return obj;
}??$)dnl
dnl
dnl
dnl INSTFUNC_free()
define(??^INSTFUNC_free??$,??^void
??$classname??^_free (struct ??$classname??^_s * obj)
{
  free(??$classname??^_destroy(obj));
}??$)dnl
dnl
dnl
dnl INSTFUNC_decode(`num_fields', `decoding')
define(??^INSTFUNC_decode??$,??^struct ??$classname??^_s *
??$classname??^_decode (const uint8_t * space, int spacelen)
{
  struct ??$classname??^_s * obj = NULL;
  layoutvalue_t contents[$1] = { 0, };

  if (!obj) obj = ??$classname??^_malloc();
  if (!obj || !space || !spacelen) return obj;

  udf_decode(space, spacelen, udf_??$classname??^, contents);
$2
  return obj;
}??$)dnl
dnl
dnl
dnl INSTFUNC_encode(`num_fields', `encoding')
define(??^INSTFUNC_encode??$,??^int
??$classname??^_encode (const struct ??$classname??^_s * obj, uint8_t * space, int spacelen)
{
  layoutvalue_t contents[$1] = { 0, };

  if (!obj) return 0;
  if (!obj || !space || !spacelen) return 0;
$2
  int retval = udf_encode(space, spacelen, udf_??$classname??^, contents);
  if (retval != contents[$1-1].word) { }
  return retval;
}??$)dnl
dnl
dnl
dnl DecodingTag(`obj_field', `fieldnum')
define(??^DecodingTag??$,??^{ struct tag_s * tag = tag_decode(contents[$2].ptr, 16); obj->??$$2??^ =  *tag; tag_free(tag); }??$)dnl
dnl
dnl DecodingStruct(`struct_name', `udfsize', `obj_field', `fieldnum')
define(??^DecodingStruct??$,??^{ struct ??$$1??^_s * sub = ??$$1??^_decode(contents[$4].ptr, $2); obj->$3 = *sub; ??$$1??^_free(sub); }??$)dnl
dnl
dnl EncodingStruct(`struct_name', `udfsize', `obj_field', `fieldnum')
define(??^EncodingStruct??$,??^uint8_t $3[$2] = { 0, }; $1_encode(&(obj->$3), $3, $2); contents[$4].ptr = $3;??$)dnl
dnl
dnl
dnl INSTFUNC_cmp(`comparison_body')
define(??^INSTFUNC_cmp??$,??^int
??$classname??^_cmp (const struct ??$classname??^_s *a, const struct ??$classname??^_s *b)
{
  int retval = -1;
$1
  return retval;
}??$)dnl
dnl
dnl
dnl INSTFUNC_len(`length_expression')
define(??^INSTFUNC_len??$,??^int
??$classname??^_len (const struct ??$classname??^_s *obj)
{
  int retval = 0;
  retval = $1;
  return retval;
}??$)dnl
dnl
dnl
dnl REPRLINE(`fmt', `printf_argument')
define(??^REPRLINE??$,??^n += snprintf(buf+n, buflen-1, $1, $2)??$)dnl
dnl
dnl REPRATOM(`field_name', `fmt_spec', `obj_field')
define(??^REPRATOM??$,??^n += snprintf(buf+n, buflen-1, "  .$1 = $2,\n", obj->$3);??$)dnl
dnl
dnl REPRSTRUCT(`field_name', `struct_name', 'obj_field')
define(??^REPRSTRUCT??$,??^$2_encode(&(obj->$3), tmp, sizeof(tmp));
  reindent_repr(tmp, sizeof(tmp), 2);
  n += snprintf(buf+n, buflen-n, "  .$1 = %s,\n", tmp)??$)dnl
dnl
dnl
dnl INSTFUNC_repr()
define(??^INSTFUNC_repr??$,??^int
??$classname??^_repr (const struct ??$classname??^_s *obj, char buf[], int buflen)
{
  int n = 0;
//  char tmp[buflen/2] = { 0, };
  char tmp[512] = { 0, };

  REPRLINE("struct ??$classname??^_s _%p = {\n", obj);

  REPRSTRUCT(tag,tag,tag);

  REPRSTRUCT(icbtag,icbtag,icbtag);

  n += snprintf(buf+n, buflen-n, "}");

  return n;
}??$)dnl
dnl
dnl
dnl INSTFUNC_dump(`internal_buffer_size')
define(??^INSTFUNC_dump??$,??^void
??$classname??^_dump (const struct ??$classname??^_s *obj)
{
  char buf[ifelse($1,,1024,$1)] = { 0, };
  int n = ??$classname??^_repr(obj, buf, sizeof(buf));
  n += snprintf(buf+n, sizeof(buf)-n, ";");
  puts(buf);
}??$)dnl
