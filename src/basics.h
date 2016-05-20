#ifndef _BASICS_H_
#define _BASICS_H_

#include <stdint.h>
#include <time.h>

#include <sys/time.h>

#include "conv.h"


/* Part 1 "basic" structures */


#define SZ_CHARSPEC_CSI 63
#define SZ_REGID_ID 23
#define SZ_REGID_SUFFIX 8

#define TZ_UNSPEC (-2047)


/* ECMA-167/3 1/7.2.1 : Character set specification */
struct charspec_s {
    unsigned int cst;  /* Character Set Type, 0..8 */
    uint8_t csi[SZ_CHARSPEC_CSI+1];  /* Charater Set Information */ /* 63 significant, +1 for C string */
};


/* ECMA-167/3 1/7.3 : Timestamp */
enum tz_type_e {
    TZTYPE_UTC,	    /* Coordinated Univeral Time */
    TZTYPE_LOCAL,   /* local time. */
    TZTYPE_STOBOAR, /* Subject To Agreement Between Originator And Recipient */
};

struct timestamp_s {
    unsigned int typ;	  /* Time zone type. */
    int tz;		  /* Time zone, offset from UTC [minutes] */
    int year;		  /* Year, 1..9999 */
    unsigned int month;	  /* Month, 1..12 */
    unsigned int day;	  /* Day, 1..31 */
    unsigned int hour;	  /* Hour, 0..23 */
    unsigned int min;	  /* Minute, 0..59 */
    unsigned int sec;	  /* Second, 0..59 or 0..60 depending on Type */
    unsigned int usec;	  /* Microseconds, 0..999999 */
};


/* ECMA-167/3 1/7.4 : Entity Identifier (regid = Registered Identifier) */
#define REGID_FLAGS_DIRTY	(1 << 0)
#define REGID_FLAGS_PROTECTED	(1 << 1)
struct regid_s {
    struct {
	unsigned int dirty:1;
	unsigned int protected:1;
    } flags;			/* Flags */
    uint8_t id[SZ_REGID_ID+1];  /* Identifier */ /* 23 significant, +1 for C string */
    uint8_t suffix[SZ_REGID_SUFFIX];  /* Suffix */ /* Arbitrary bytes */
};




/*
   Methods pattern:
   * malloc(int?) - allocate instance on heap with relevant size parameter.
   * destroy() - release internal memory allocations; instance must be explicity freed().
   * init(...) - populate instance from parameters.
   * free(...) - release and deallocate instance.
   * decode(...) - populate instance from UDF binary.
   * encode(...) - fill UDF binary from instance.
   * cmp(a,b) - comparator (lt=-1, eq=0, gt=1).

*/

struct charspec_s * charspec_malloc ();
struct charspec_s * charspec_destroy (struct charspec_s *);
struct charspec_s * charspec_init (struct charspec_s *,
				   unsigned int charset_type,
				   const char * charset_info);
void charspec_free (struct charspec_s *);
struct charspec_s * charspec_decode (void * raw, int rawlen);
int charspec_encode (struct charspec_s *, void * raw, int rawlen);
int charspec_cmp (const struct charspec_s *, const struct charspec_s *);
int charspec_str (const struct charspec_s *, char[], int);
void charspec_dump (const struct charspec_s *);


struct timestamp_s * timestamp_malloc ();
struct timestamp_s * timestamp_destroy (struct timestamp_s *);
struct timestamp_s * timestamp_init (struct timestamp_s *,
				     unsigned int tz_type,
				     int tz_ofs,
				     int year,
				     unsigned int month,
				     unsigned int day,
				     unsigned int hour,
				     unsigned int min,
				     unsigned int sec,
				     unsigned int usec);
struct timestamp_s * timestamp_from_time (struct timestamp_s *,
					  time_t time);
struct timestamp_s * timestamp_from_timeval (struct timestamp_s *,
					     const struct timeval *,
					     const struct timezone *);
void timestamp_free (struct timestamp_s *);
struct timestamp_s * timestamp_decode (void * raw, int rawlen);
int timestamp_encode (struct timestamp_s *, void * raw, int rawlen);
int timestamp_cmp (const struct timestamp_s *, const struct timestamp_s *);
int timestamp_str (const struct timestamp_s *, char[], int);
void timestamp_dump (const struct timestamp_s *);


struct regid_s * regid_malloc ();
struct regid_s * regid_destroy (struct regid_s *);
struct regid_s * regid_init (struct regid_s *, const char * id);
void regid_free (struct regid_s *);
struct regid_s * regid_decode (void * raw, int rawlen);
int regid_encode (struct regid_s *, void * raw, int rawlen);
int regid_cmp (const struct regid_s *, const struct regid_s *);
int regid_str (const struct regid_s *, char[], int);
void regid_dump (const struct regid_s *);


#endif // _BASICS_H_
