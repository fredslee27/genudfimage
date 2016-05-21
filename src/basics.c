#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "basics.h"




struct charspec_s *
charspec_malloc ()
{
  return (struct charspec_s*)malloc(sizeof(struct charspec_s));
}

struct charspec_s *
charspec_destroy (struct charspec_s * obj)
{
  return obj;
}

struct charspec_s *
charspec_init (struct charspec_s * obj, unsigned int charset_type, const char * charset_info)
{
  obj->cst = charset_type;
  strncpy(obj->csi, charset_info, 63);
}

void
charspec_free (struct charspec_s *obj)
{
  free(charspec_destroy(obj));
}

struct charspec_s *
charspec_decode (void * raw, int rawlen)
{
  struct charspec_s * retval;

  retval = charspec_malloc();
  retval->cst = uint8_decode(raw);
  bytestr_decode(retval->csi, sizeof(retval->csi), raw+1, rawlen-1);
  return retval;
}

int
charspec_encode (struct charspec_s * obj, void * raw, int rawlen)
{
  int ofs = 0;
  ofs += uint8_encode(raw+ofs, obj->cst);
  ofs += bytestr_encode(raw+ofs, rawlen-ofs, obj->csi, sizeof(obj->csi));
  return ofs;
}

int
charspec_cmp (const struct charspec_s * a, const struct charspec_s * b)
{
  if (a->cst < b->cst) return -1;
  if (a->cst > b->cst) return 1;
  return memcmp(a->csi, b->csi, SZ_CHARSPEC_CSI);
}

int
charspec_repr (const struct charspec_s * obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct charspec_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .cst=%d,\n", obj->cst);
  n += snprintf(buf+n, buflen-n, "  .csi=\"%s\",\n", obj->csi);
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
charspec_dump (const struct charspec_s * obj)
{
  char buf[512];
  charspec_repr(obj, buf, sizeof(buf));
  puts(buf);
}




struct timestamp_s *
timestamp_malloc ()
{
  return (struct timestamp_s*)malloc(sizeof(struct timestamp_s));
}

struct timestamp_s *
timestamp_destroy (struct timestamp_s *obj)
{
  return obj;
}

struct timestamp_s *
timestamp_init (struct timestamp_s *obj,
		unsigned int tz_type, int tz_ofs,
		int year, unsigned int month, unsigned int day,
		unsigned int hour, unsigned int min, unsigned int sec,
		unsigned int usec)
{
  if (!obj) return obj;

  obj->typ = tz_type;
  obj->tz = tz_ofs;
  obj->year = year;
  obj->month = month;
  obj->day = day;
  obj->hour = hour;
  obj->min = min;
  obj->sec = sec;
  obj->usec = usec;

  return obj;
}

struct timestamp_s *
timestamp_from_tm (struct timestamp_s * obj,
		   const struct tm * tm)
{
  if (!obj) return obj;

  obj->typ = TZTYPE_LOCAL;
  obj->tz = TZ_UNSPEC;
  obj->year = tm->tm_year;
  obj->month = tm->tm_mon;
  obj->day = tm->tm_mday;
  obj->hour = tm->tm_hour;
  obj->min = tm->tm_min;
  obj->sec = tm->tm_sec;
  obj->usec = 0;

  return obj;
}

struct timestamp_s *
timestamp_from_time (struct timestamp_s *obj,
		     time_t time)
{
  struct tm * tm;
  //tm = gmtime(&time);
  tm = localtime(&time);
  return timestamp_from_tm(obj, tm);
}

struct timestamp_s *
timestamp_from_timeval (struct timestamp_s *obj,
			const struct timeval *tv,
			const struct timezone *tz)
{
  obj->typ = TZTYPE_UTC;
  obj->tz = tz->tz_minuteswest;
  //struct tm * tm = gmtime(&(tv->tv_sec));
  struct tm * tm = localtime(&(tv->tv_sec));
  timestamp_from_tm(obj, tm);
  obj->usec = tv->tv_usec;

  return obj;
}

void
timestamp_free (struct timestamp_s *obj)
{
  free(timestamp_destroy(obj));
}

struct timestamp_s *
timestamp_decode (void * raw, int rawlen)
{
  struct timestamp_s * retval;

  retval = timestamp_malloc();
  unsigned int typtz = uint16_decode(raw+0);
  unsigned int typ = (typtz >> 12);
  int tzofs = (typtz & 0x0fff);
  int year = uint16_decode(raw+2);
  unsigned int csec = uint8_decode(raw+9);
  unsigned int husec = uint8_decode(raw+10);
  unsigned int usec = uint8_decode(raw+11);

  retval->typ = typ;
  if (tzofs > 0x07ffL)
    {
      tzofs = (0x7ffL - tzofs);
    }
  retval->tz = tzofs;
  if (year > 0x7fffL)
    {
      year = (0x7fffL - year);
    }
  retval->year = year;
  retval->month = uint8_decode(raw+4);
  retval->day = uint8_decode(raw+5);
  retval->hour = uint8_decode(raw+6);
  retval->min = uint8_decode(raw+7);
  retval->sec = uint8_decode(raw+8);
  retval->usec = (csec * 10000) + (husec * 100) + (usec * 1);
}

int
timestamp_encode (struct timestamp_s *obj, void * raw, int rawlen)
{
  unsigned int typtz;
  unsigned int utzofs;
  unsigned int uyear;
  unsigned int csec, husec, usec;

  if (obj->tz < 0)
    {
      utzofs = 0x07ff - obj->tz;
    }
  else
    {
      utzofs = obj->tz;
    }
  typtz = (obj->typ << 12) | (utzofs & 0x07ff);
  if (obj->year < 0)
    {
      uyear = 0x7fff - obj->year;
    }
  else
    {
      uyear = obj->year;
    }
  csec = (obj->usec / 10000) % 100;
  husec = (obj->usec / 100) % 100;
  usec = (obj->usec / 1) % 100;
  int ofs = 0;
  ofs += uint16_encode(raw+ofs, typtz);
  ofs += uint16_encode(raw+ofs, uyear);
  ofs += uint8_encode(raw+ofs, obj->month);
  ofs += uint8_encode(raw+ofs, obj->day);
  ofs += uint8_encode(raw+ofs, obj->hour);
  ofs += uint8_encode(raw+ofs, obj->min);
  ofs += uint8_encode(raw+ofs, obj->sec);
  ofs += uint8_encode(raw+ofs, csec);
  ofs += uint8_encode(raw+ofs, husec);
  ofs += uint8_encode(raw+ofs, usec);

  return ofs;
}

int
timestamp_cmp (const struct timestamp_s *a, const struct timestamp_s *b)
{
  // TODO: time comparison.
  return memcmp(a, b, sizeof(struct timestamp_s));
}

int
timestamp_repr (const struct timestamp_s *obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct timestamp_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .typ = %u\n", obj->typ);
  n += snprintf(buf+n, buflen-n, "  .tz = %d\n", obj->tz);
  n += snprintf(buf+n, buflen-n, "  .year = %d\n", obj->year);
  n += snprintf(buf+n, buflen-n, "  .month = %u\n", obj->month);
  n += snprintf(buf+n, buflen-n, "  .day = %u\n", obj->day);
  n += snprintf(buf+n, buflen-n, "  .hour = %u\n", obj->hour);
  n += snprintf(buf+n, buflen-n, "  .min = %u\n", obj->min);
  n += snprintf(buf+n, buflen-n, "  .sec = %u\n", obj->sec);
  n += snprintf(buf+n, buflen-n, "  .usec = %u\n", obj->usec);
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
timestamp_dump (const struct timestamp_s *obj)
{
  char buf[512];
  timestamp_repr(obj, buf, sizeof(buf));
  puts(buf);
}




struct regid_s *
regid_malloc ()
{
  return (struct regid_s*)malloc(sizeof(struct regid_s));
}

struct regid_s *
regid_destroy (struct regid_s *obj)
{
  return obj;
}

struct regid_s *
regid_init (struct regid_s *obj, const char * id)
{
  if (!obj) return obj;

  memset(obj, 0, sizeof(*obj));
  obj->flags.dirty = 0;
  obj->flags.protected = 0;
  strncpy(obj->id, id, sizeof(obj->id));

  return obj;
}

void
regid_free (struct regid_s *obj)
{
  free(regid_destroy(obj));
}

struct regid_s *
regid_decode (void * raw, int rawlen)
{
  struct regid_s * obj = regid_malloc();

  unsigned int uflag = uint8_decode(raw+0);
  obj->flags.dirty = ((uflag & REGID_FLAGS_DIRTY) == REGID_FLAGS_DIRTY);
  obj->flags.protected = ((uflag & REGID_FLAGS_PROTECTED) == REGID_FLAGS_PROTECTED);
  bytestr_decode(obj->id, SZ_REGID_ID, raw+1, rawlen-1);
  bytestr_decode(obj->suffix, SZ_REGID_SUFFIX, raw+24, rawlen-24);
  return obj;
}

int
regid_encode (struct regid_s *obj, void * raw, int rawlen)
{
  unsigned int uflag = 0;
  if (obj->flags.dirty) uflag |= REGID_FLAGS_DIRTY;
  if (obj->flags.protected) uflag |= REGID_FLAGS_PROTECTED;
  bytestr_encode(raw+1, rawlen-1, obj->id, sizeof(obj->id));
  bytestr_encode(raw+24, rawlen-24, obj->suffix, sizeof(obj->suffix));
}

int
regid_cmp (const struct regid_s *a, const struct regid_s *b)
{
  // TODO: comparison between regid
  return memcmp(a, b, sizeof(*a));
}

int
regid_repr (const struct regid_s *obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct regid_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .flags = { .dirty=%u, .protected=%u },\n",
		obj->flags.dirty, obj->flags.protected);
  n += snprintf(buf+n, buflen-n, "  .id = \"%s\",\n", obj->id);
  n += snprintf(buf+n, buflen-n, "  .suffix = { %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x },\n",
		obj->suffix[0], obj->suffix[1], obj->suffix[2], obj->suffix[3],
		obj->suffix[4], obj->suffix[5], obj->suffix[6], obj->suffix[7]);
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
regid_dump (const struct regid_s *obj)
{
  char buf[512];
  regid_repr(obj, buf, sizeof(buf));
  puts(buf);
}





struct dstring_s *
dstring_malloc ()
{
  return (struct dstring_s*)malloc(sizeof(struct dstring_s));
}

struct dstring_s *
dstring_destroy (struct dstring_s *obj)
{
  return obj;
}

struct dstring_s *
dstring_init (struct dstring_s *obj, int fldlen, const char *s)
{
  if (!obj) return obj;

  obj->size = fldlen;
  strncpy(obj->d, s, obj->size);
  obj->d[obj->size-1] = 0;

  return obj;
}

void
dstring_free (struct dstring_s *obj)
{
  free(dstring_destroy(obj));
}

struct dstring_s *
dstring_decode (const uint8_t * raw, int rawlen)
{
  struct dstring_s * obj = malloc(sizeof(struct dstring_s));

  memset(obj, 0, sizeof(*obj));
  obj->size = rawlen;
  obj->len = raw[rawlen-1];
  strncpy(obj->d, raw, obj->len);
  // rest alreay filled with '\0'.

  return obj;
}

int
dstring_encode (const struct dstring_s *obj, uint8_t * raw, int rawlen)
{
  memcpy(raw, obj->d, obj->len);
  memset(raw + obj->len, 0, rawlen - obj->len);
  raw[rawlen-1] = obj->len;
  return rawlen;
}

int
dstring_cmp (const struct dstring_s *a, const struct dstring_s *b)
{
  return strncmp(a->d, b->d, a->size < b->size ? a->size : b->size);
}

#if 0
int
dstring_str (const struct dstring_s *obj, char buf[], int buflen)
{
  int copylen = buflen < obj->size ? buflen : obj->size;
  strncpy(buf, obj->d, copylen);
  buf[copylen-1] = 0;
  return copylen;
}
#endif //0

int
dstring_repr (const struct dstring_s *obj, char buf[], int buflen)
{
  int n = 0;
  n += snprintf(buf+n, buflen-n, "struct dstring_s _%p = {\n", obj);
  n += snprintf(buf+n, buflen-n, "  .size = %u,\n", obj->size);
  n += snprintf(buf+n, buflen-n, "  .len = %u,\n", obj->len);
  n += snprintf(buf+n, buflen-n, "  .d = \"%s\",\n", obj->d);
  n += snprintf(buf+n, buflen-n, "};");
  return n;
}

void
dstring_dump (const struct dstring_s *obj)
{
  char buf[512];
  dstring_repr(obj, buf, sizeof(buf));
  puts(buf);
}



