#ifndef _FILST_MACROS_H_
#define _FILST_MACROS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filst.h"
#include "util.h"


#define FILST_TYPE(var) (filst_t*)(var)


#define COMMON_FILST_TAGTYPE(sname, enumval) enum tagid_e sname##_tagtype () { return enumval; }
#define COMMON_FILST_NFIELDS(sname) int sname##_nfields () { \
    const int hardlimit = 128; \
    int n = 0; \
    const struct layoutfield_s * iter = udf_##sname; \
    while ((n < hardlimit) && (iter->fldtype != LAYOUT_END)) { \
	iter++; n++; \
    } \
    return n; }
#define COMMON_FILST_LAYOUTDESCR(sname) const struct layoutfield_s* sname##_layoutdescr () { return udf_##sname; }
#define COMMON_FILST(sname,enumval) \
  COMMON_FILST_TAGTYPE(sname,enumval) \
  COMMON_FILST_NFIELDS(sname) \
  COMMON_FILST_LAYOUTDESCR(sname)

#define FILST_ISA(sname) bool sname##_isa (const filst_t *uself)
#define STD_ISA(sname) return (uself && (uself->generic.tag.tagid == sname##_tagtype()));



/* _init0  - no-argument initializer */
#define FILST_INIT0(sname) static struct sname##_s * sname##_init0 (struct sname##_s * self)


/* _malloc */
#define FILST_MALLOC(sname) struct sname##_s * sname##_malloc (size_t dlen)
#define SELFNEW0(sname) size_t msize = sizeof(struct sname##_s) + dlen;  \
 struct sname##_s * self = (struct sname##_s*)malloc(msize); \
 memset(self, 0, msize); \
 sname##_init0(self)

/* _destroy */
#define FILST_DESTROY(sname) struct sname##_s * sname##_destroy (struct sname##_s *self)

//#define FILST_INIT(sname) filst_t * sname##_init (filst_t *uself)

#define SELF_BAILNULL(sname) if (!self) return self
//#define SELFVAR(sname) struct sname##_s * self = &(uself->sname)
//#define SELFVARCONST(sname) const struct sname##_s * self = &(uself->sname)
#define CHECKUSELF(sname) if (! (uself && (uself->generic.tag.tagid == sname##_tagtype()))) return

/* _free */
#define FILST_FREE(sname) void sname##_free (struct sname##_s *self)
#define SELFFREE(sname) free(sname##_destroy(self))

#define CONVERSION_SANITY if (!self || !space || !spacelen) return

/* _decode */
#define FILST_DECODE(sname) struct sname##_s * sname##_decode (struct sname##_s *self, const uint8_t space[], size_t spacelen)
#define CONTENTSVAR(n) layoutvalue_t contents[n] = { 0, }
#define STD_DECODE(sname) udf_decode(space, spacelen, udf_##sname, contents)
#define DECODE_STRUCT(layoutidx,slen,stype,selffld) stype##_decode(&(self->selffld), contents[layoutidx].ptr, slen)
#define DECODE_WORD(layoutidx,selffld) self->selffld = contents[layoutidx].word
#define DECODE_CHECK(layoutidx,val) if (contents[layoutidx].word == val)
#define RETURN_DECODE(sname) return (struct sname##_s*)self

/* _encode */
#define FILST_ENCODE(sname) size_t sname##_encode (const struct sname##_s *self, uint8_t *space, size_t spacelen)
#define ENCODINGVARS size_t n = 0
#define ENCODE_STRUCT(layoutidx,slen,stype,selffld) uint8_t selffld[slen] = { 0, }; n += stype##_encode(&(self->selffld), selffld, slen); contents[layoutidx].ptr = selffld
#define ENCODE_WORD(layoutidx,selffld) contents[layoutidx].word = self->selffld
#define STD_ENCODE(sname) udf_encode(space, spacelen, udf_##sname, contents)
#define RETURN_ENCODE(sname) return n

/* _cmp */
#define FILST_CMP(sname) int sname##_cmp (const struct sname##_s *self, const struct sname##_s *other)
//#define PLACEHOLDER_CMP(sname) return memcmp(&(a->sname), &(b->sname), sizeof(a->sname))
#define PLACEHOLDER_CMP(sname) return memcmp(self, other, sizeof(*self))

/* _len */
#define FILST_LEN(sname) size_t sname##_len (const struct sname##_s *self)

/* _repr */
#define FILST_REPR(sname) int sname##_repr (const struct sname##_s *self, char buf[], int buflen)
//#define REPRVARS(sname) int n = 0, m = 0;  /* n: needed; m: max */ char tmp[256] = { 0, }; SELFVARCONST(sname)
#define REPRVARS(sname) int n = 0, m = 0;  /* n: needed; m: max */ char tmp[256] = { 0, }
#define REPR_HEADER(sname) n += snprintf(buf+m, buflen-m, "struct " #sname "_s _%p = {\n", self); m = n<buflen ? n : buflen
#define REPR_ATOM(fld,fmt) n += snprintf(buf+m, buflen-m, "  ." #fld " = " #fmt ",\n", self->fld); m = n<buflen ? n : buflen
#define REPR_STRUCT(stype,fld) stype##_repr(&(self->fld), tmp, sizeof(tmp)); reindent_repr(tmp, sizeof(tmp), 2); n += snprintf(buf+m, buflen-m, "  .%s = %s,\n", #fld, tmp); m = n<buflen ? n : buflen
#define REPR_LITERAL(lit) n += snprintf(buf+m, buflen-m, lit); m = n<buflen ? n : buflen
#define REPR_FOOTER(sname) n += snprintf(buf+m, buflen-m, "}"); m = n<buflen ?  n : buflen
#define RETURN_REPR(sname) return n

/* _dump */
#define FILST_DUMP(sname) void sname##_dump (const struct sname##_s *self)
#define STD_DUMP(sname, buflen) char buf[buflen] = { 0, }; size_t n = sname##_repr(self, buf, sizeof(buf)-2); n = n<sizeof(buf)-2 ? n : sizeof(buf)-2; buf[n] = ';'; buf[n+1]=0; puts(buf)




/* struct THISTYPE_s <-> filst_t wrapper functions, for functions table. */
#define FILST_THUNKS(tagidval, sname) \
enum tagid_e filst_##sname##_tagtype () { return tagidval; } \
int filst_##sname##_nfields () { return -1; /* TODO: filst_nfields */ } \
const struct layoutfield_s * filst_##sname##_layoutdescr () { return udf_##sname; } \
bool filst_##sname##_isa (const filst_t * uself) { STD_ISA(sname); } \
filst_t * filst_##sname##_malloc (size_t dlen) { return (filst_t*)sname##_malloc(dlen); } \
filst_t * filst_##sname##_destroy (filst_t * uself) { return uself; } \
void filst_##sname##_free (filst_t * uself) { free(sname##_destroy(&(uself->sname))); } \
filst_t * filst_##sname##_decode (filst_t * uself, const uint8_t space[], size_t spacelen) { \
    CHECKUSELF(sname) NULL; \
    return (filst_t*)sname##_decode(&(uself->sname), space, spacelen); } \
size_t filst_##sname##_encode (const filst_t * uself, uint8_t * space, size_t spacelen) { \
    CHECKUSELF(sname) 0; \
    return sname##_encode(&(uself->sname), space, spacelen); } \
size_t filst_##sname##_len (const filst_t * uself) { \
    CHECKUSELF(sname) 0; \
    return sname##_len(&(uself->sname)); } \
int filst_##sname##_cmp (const filst_t *uself, const filst_t *uother) { \
    CHECKUSELF(sname) 0; \
    if (!filst_##sname##_isa(uother)) return 0; \
    return sname##_cmp(&(uself->sname), &(uother->sname)); } \
int filst_##sname##_repr (const filst_t * uself, char buf[], int buflen) { \
    CHECKUSELF(sname) 0; \
    return sname##_repr(&(uself->sname), buf, buflen); } \
void filst_##sname##_dump (const filst_t * uself) { \
    CHECKUSELF(sname); \
    sname##_dump(&(uself->sname)); }


#define FILST_INTERFACE(tagidval, sname) \
  FILST_THUNKS(tagidval, sname) \
  FILST_FUNCTABLE(te); \
  const struct filstfuncs_s * sname##_functable () { return te_funcs; }






#define FILST_FUNCTABLE(sname) struct filstfuncs_s sname##_funcs = { \
  .tagtype = filst_##sname##_tagtype, \
  .nfields = filst_##sname##_nfields, \
  .layoutdescr = filst_##sname##_layoutdescr, \
  .isa = filst_##sname##_isa, \
  .malloc = filst_##sname##_malloc, \
  .destroy = filst_##sname##_destroy, \
  .free = filst_##sname##_free, \
  .decode = filst_##sname##_decode, \
  .encode = filst_##sname##_encode, \
  .len = filst_##sname##_len, \
  .cmp = filst_##sname##_cmp, \
  .repr = filst_##sname##_repr, \
  .dump = filst_##sname##_dump, \
}



#endif // _FILST_MACROS_H_
