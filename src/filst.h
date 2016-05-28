#ifndef _FILST_H_
#define _FILST_H_

#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "basics.h"


/* ECMA-167/3 + UDF/3 : File Structure */


#define MAX_COMPONENT_IDENTIFIER_LEN 255
#define SZ_FSD_LVID 128
#define SZ_FSD_FSID 32
#define SZ_FSD_CFID 32
#define SZ_FSD_AFID 32



/* ECMA-167/3 4/14.16 : Pathname */
/* ECMA-167/3 4/14.16.1 : Path Component */
enum path_component_type_e {
  PATH_COMPONENT_TYPE_RESERVED,
  PATH_COMPONENT_TYPE_ROOT,
  PATH_COMPONENT_TYPE_PARENT,
  PATH_COMPONENT_TYPE_SELF,
  PATH_COMPONENT_TYPE_COPYNAME,
};

struct path_component_s {
    /* NULL for newly created instance.
       Points to raw bytes (could be mmap) struct was built from.
       */
    void * raw;

    enum path_component_type_e typ;
    unsigned int len;  /* Length of 'd'. */
    unsigned int vers;  /* Version; typically 0 */
    unsigned char d[MAX_COMPONENT_IDENTIFIER_LEN];

    struct path_component_s * next;
};

struct path_component_s * path_component_malloc (int);
struct path_component_s * path_component_dup (const struct path_component_s *);
struct path_component_s * path_component_destroy (struct path_component_s *);
//struct path_component_s * path_component_init (struct path_component_s *);
const struct path_component_s * path_component_make (
    enum path_component_type_e,
    unsigned int version,
    int cilen,
    char ci[]);
void path_component_free (struct path_component_s *);
struct path_component_s * path_component_decode (const uint8_t * space, int spacelen);
int path_component_encode (const struct path_component_s *, uint8_t * space, int spacelen);
int path_component_cmp (const struct path_component_s *, const struct path_component_s *);
int path_component_repr (const struct path_component_s *, char[], int);
void path_component_dump (const struct path_component_s *);


/* ECMA-167/3 4/8.7 : Pathname */
struct pathname_s {
    /* Raw storage is array of PathComponent. */
    void * raw;
    int rawlen;

    int ncomponents;
    struct path_component_s * components;  /* Linked list of PathComponent. */
};










enum extent_type_e {
    EXTENTTYPE_USED,
    EXTENTTYPE_READY,
    EXTENTTYPE_FREE,
    EXTENTTYPE_NEXT,
};


struct lb_addr_s {
    enum extent_type_e et;  /* Extent Type */
    unsigned int lbn;   /* Logical Block Number */
    unsigned int prn;   /* Partition Reference Number */
};

struct lb_addr_s * lb_addr_malloc ();
struct lb_addr_s * lb_addr_init (struct lb_addr_s * obj,
                                 unsigned int lbn,
                                 unsigned int prn);
struct lb_addr_s * lb_addr_destroy (struct lb_addr_s * obj);
void lb_addr_free (struct lb_addr_s * obj);
struct lb_addr_s * lb_addr_decode (void * space, int spacelen);
int lb_addr_encode (const struct lb_addr_s * obj, void * space, int spacelen);
int lb_addr_repr (const struct lb_addr_s *, char[], int);
void lb_addr_dump (const struct lb_addr_s *);





enum icb_file_type_e {
    ICBFT_OTHER,
    ICBFT_USE,
    ICBFT_PIE,
    ICBFT_IE,
    ICBFT_DIR,
    ICBFT_REG,
    ICBFT_BLK,
    ICBFT_CHR,
    ICBFT_EA,
    ICBFT_FIFO,
    ICBFT_SOCK,
    ICBFT_TE,
    ICBFT_SYMLINK,
    ICBFT_STREAM,
    ICBFT_248,
    ICBFT_249,
    ICBFT_250,
    ICBFT_251,
    ICBFT_252,
    ICBFT_253,
    ICBFT_254,
    ICBFT_255,
};

//const char * icb_file_type_name (enum icb_file_type_e);
unsigned int icb_file_type_int (enum icb_file_type_e);
enum icb_file_type_e icb_file_type_enum (unsigned int);

/* Allocation Descriptor Type: short_ad, long_ad, ext_ad, direct/not_ad */
#define ADTYPE_SHORT 0
#define ADTYPE_LONG 1
#define ADTYPE_EXT 2
#define ADTYPE_DIRECT 3

struct icbtag_s {
    unsigned int prnde; /* Prior Recorded Number of Direct Entries. */
    unsigned int st;    /* Strategy Type */
    unsigned int sp;    /* Strategy Parameter */
    unsigned int mne;   /* Maximum Number of Entries */
    enum icb_file_type_e ft;  /* File Type */
    struct lb_addr_s picbl;      /* Parent ICB Location */
    struct icb_flags {
        unsigned int adtype:3;    /* Allocation Descriptor type */
        unsigned int dirsort:1;   /* DIR : is sorted */
        unsigned int nonreloc:1;  /* Non-relocatable */
        unsigned int archive:1;   /* Archive */
        unsigned int setuid:1;    /* SetUID */
        unsigned int setgid:1;    /* SetGID */
        unsigned int sticky:1;    /* Sticky */
        unsigned int contiguous:1;  /* Contiguous */
        unsigned int system:1;      /* System */
        unsigned int transformed:1; /* Transformed */
        unsigned int multiversion:1;  /* Multi-versions */
        unsigned int stream:1;    /* is stream */
    } flags;
};

struct icbtag_s * icbtag_malloc ();
struct icbtag_s * icbtag_destroy (struct icbtag_s *);
struct icbtag_s * icbtag_init (struct icbtag_s *,
			       unsigned int previous_number_of_direct_entries,
			       unsigned int strategy_type,
			       unsigned int strategy_parameter,
			       unsigned int maximum_number_of_entries,
			       enum icb_file_type_e file_type,
			       const struct lb_addr_s * parent_icb_location,
			       unsigned int allocation_descriptor_type);
void icbtag_free (struct icbtag_s *);
struct icbtag_s * icbtag_decode (struct icbtag_s *, const uint8_t * space, int spacelen);
int icbtag_encode (const struct icbtag_s *, uint8_t * space, int spacelen);
int icbtag_len (const struct icbtag_s *);
int icbtag_cmp (const struct icbtag_s *, const struct icbtag_s *);
int icbtag_repr (const struct icbtag_s *, char[], int);
void icbtag_dump (const struct icbtag_s *);




enum tagid_e {
    TAGID_NONE,
    TAGID_PVD,    /* Primary Volume Descriptor */
    TAGID_AVDP,   /* Anchor Volume Descriptor Pointer */
    TAGID_VDP,    /* Volume Descriptor Pointer */
    TAGID_IUVD,   /* Implementation Use Volume Descriptor */
    TAGID_PD,     /* Partition Descriptor */
    TAGID_LVD,    /* Logical Volume Descriptor */
    TAGID_USD,    /* Unallocated Space Descriptor */
    TAGID_TD,     /* Terminating Descriptor */
    TAGID_LVID,   /* Logical Volume Integrity Descriptor */

    TAGID_FSD,    /* File Set Descriptor */
    TAGID_FID,    /* File Identifier Descriptor */
    TAGID_AED,    /* Allocation Extent Descriptor */
    TAGID_IE,     /* Indirect Entry */
    TAGID_TE,     /* Terminal Entry */
    TAGID_FE,     /* File Entry */
    TAGID_EAHD,   /* Extended Attribute Header Descriptor */
    TAGID_USE,    /* Unallocated Space Entry */
    TAGID_SBD,    /* Space Bitmap Descriptor */
    TAGID_PIE,    /* Partition Integrity Entry */
    TAGID_EFE,    /* Extended File Entry */
};

/* ECMA-167/3 3/7.2 - Descriptor Tag */
struct tag_s {
    enum tagid_e tagid;
    unsigned int vers;
    unsigned int checksum;
    unsigned int serial;
    unsigned int crc;
    unsigned int crclen;
    unsigned int tagloc;
};

/* Given tagid_e, yield integer value used in UDF binary. */
unsigned int tagid_int (enum tagid_e);
/* Given value used in UDF binary, yield enum. */
enum tagid_e tagid_enum (unsigned int);

struct tag_s * tag_malloc ();
struct tag_s * tag_destroy (struct tag_s *);
struct tag_s * tag_init (struct tag_s *,
			 enum tagid_e tagid,
			 unsigned int vers,
			 unsigned int serial,
			 unsigned int tagloc);
void tag_free (struct tag_s *);
unsigned int tag_check_sum (struct tag_s *);
struct tag_s * tag_decode (struct tag_s *, const uint8_t *, int);
int tag_encode (const struct tag_s *, uint8_t *, int);
int tag_repr (const struct tag_s *, char[], int);
void tag_dump (const struct tag_s *);






struct short_ad_s {
    enum extent_type_e typ;
    unsigned int len;
    unsigned int pos;
};


struct long_ad_s {
    enum extent_type_e typ;
    unsigned int len;
    struct lb_addr_s loc;
    uint8_t impuse[6];
};

struct long_ad_s * long_ad_malloc ();
struct long_ad_s * long_ad_destroy (struct long_ad_s *);
struct long_ad_s * long_ad_init (struct long_ad_s *,
				 unsigned int,
				 const struct lb_addr_s * loc,
				 uint8_t impuse[6]);
void long_ad_free (struct long_ad_s *);
struct long_ad_s * long_ad_decode (struct long_ad_s *, const uint8_t space[], int spacelen);
int long_ad_encode (const struct long_ad_s *, uint8_t space[], int spacelen);
int long_ad_cmp (const struct long_ad_s *, const struct long_ad_s *);
int long_ad_repr (const struct long_ad_s *, char[], int);
void long_ad_dump (const struct long_ad_s *);


struct ext_ad_s {
    enum extent_type_e typ; /* Extent Type */
    unsigned int len;   /* Extent Length */
    unsigned int rl;    /* Recorded Length */
    unsigned int il;    /* Information Length */
    unsigned int loc;   /* Extent Location */
    uint8_t impuse[2];  /* Implementation Use */
};




/* ECMA-167/3 4/14.4 : File Identifier Descriptor */
struct fid_s {
    struct fid_s * next;

    struct tag_s tag;	  /* Descriptor Tag */
    unsigned int fvn;	  /* File Version Number */
    unsigned int fc;	  /* File Characteristics */
    unsigned int L_FI;	  /* Length of File Identifier */
    struct long_ad_s icb; /* ICB */
    unsigned int L_IU;	  /* Length of Implementation Use */

    uint8_t * impuse;	  /* Implementation Use space in 'd'. */
    uint8_t * fi;	  /* File Identifer space in 'd'. */

    uint8_t padding;      /* Number of padding bytes (to reach 4B align). */

    uint8_t d[];
};

// malloc, init, destroy, free, decode, encode, str, dump
struct fid_s * fid_malloc (unsigned int);
struct fid_s * fid_destroy (struct fid_s *);
struct fid_s * fid_init (struct fid_s *,
			 const struct tag_s *,
			 unsigned int file_version_number,
			 unsigned int file_characteristics,
			 const struct long_ad_s * icb,
			 uint8_t * implementation_use,
			 unsigned int length_implementation_use,
			 const char * file_identifier,
			 unsigned int length_file_identifier);
void fid_free (struct fid_s *);
struct fid_s * fid_decode (uint8_t * space, int spacelen);
int fid_encode (const struct fid_s *, uint8_t * space, int spacelen);
int fid_cmp (const struct fid_s *, const struct fid_s *);
int fid_repr (const struct fid_s *, char [], int);
void fid_dump (const struct fid_s *);




/* ECMA-167/3 4/14.1 : File Set Descripor */
struct fsd_s {
    struct tag_s tag;	      /* Descriptor Tag */
    struct timestamp_s rdt;   /* Recording Date and Time. */
    unsigned int il;	      /* Interchange Level */
    unsigned int mil;	      /* Maximum Interchange Level */
    unsigned int csl;	      /* Character Set List */
    unsigned int mcsl;	      /* Maximum Character Set List */
    unsigned int fsn;	      /* File Set Number */
    unsigned int fsdn;	      /* File Set Descriptor Number */
    struct charspec_s lvidcs; /* Logical Volume Character Set */
    struct dstring_s lvid;    /* Logical Volume Identifier */
    struct charspec_s fscs;   /* File Set Character Set */
    struct dstring_s fsid;    /* File Set Identifier */
    struct dstring_s cfid;    /* Copyright File Identifier */
    struct dstring_s afid;    /* Abstract File Identifier */
    struct long_ad_s rdicb;   /* Root ICB */
    struct regid_s domid;     /* Domain Identifier */
    struct long_ad_s ne;      /* Next Extent */
    struct long_ad_s ssdicb;  /* System Stream Directory ICB */
};


struct fsd_s * fsd_malloc ();
struct fsd_s * fsd_destroy (struct fsd_s *);
struct fsd_s * fsd_init (struct fsd_s *,
                         const struct tag_s * tag,
                         const struct timestamp_s * rdt,
                         unsigned int il,
                         unsigned int mil,
                         unsigned int csl,
                         unsigned int mcsl,
                         unsigned int fsn,
                         unsigned int fsdn,
                         const struct charspec_s * lvidcs,
                         const struct dstring_s * lvid,
                         const struct charspec_s * fscs,
                         const struct dstring_s * fsid,
                         const struct dstring_s * cfid,
                         const struct dstring_s * afid,
                         const struct long_ad_s * rdicb,
                         const struct regid_s * domid,
                         const struct long_ad_s * ne,
                         const struct long_ad_s * ssdicb);
struct fsd_s * fsd_init_atoms (struct fsd_s *,
                               unsigned int il,
                               unsigned int mil,
                               unsigned int csl,
                               unsigned int mcsl,
                               unsigned int fsn,
                               unsigned int fsdn);
void fsd_free (struct fsd_s *);
struct fsd_s * fsd_decode (const uint8_t * space, int spacelen);
int fsd_encode (const struct fsd_s *, uint8_t * space, int spacelen);
int fsd_cmp (const struct fsd_s *, const struct fsd_s *);
int fsd_repr (const struct fsd_s *, char buf[], int buflen);
void fsd_dump (const struct fsd_s *);




/* ECMA-167/3 14.5 : Allocation Extent Descriptor */
struct aed_s {
    struct tag_s tag;
    unsigned int pael;
    unsigned int dlen;

    unsigned int adtype;  // ADTYPE_*
    unsigned int nad;  /* Number of allocation descriptors, of adtype. */
    union {
        uint8_t * unknown;
        uint8_t * direct;
        struct short_ad_s * short_ad;
        struct long_ad_s * long_ad;
        struct ext_ad_s * ext_ad;
    } ad;  /* Access as Allocation Descriptors Sequence. */

    uint8_t d[];    /* Storage space for Allocation Descriptors Sequence. */
};

struct aed_s * aed_malloc (size_t dlen);
struct aed_s * aed_short_malloc (int nad);
struct aed_s * aed_long_malloc (int nad);
struct aed_s * aed_ext_malloc (int nad);
struct aed_s * aed_destroy (struct aed_s *);
/*
struct aed_s * aed_init (struct aed_s *,
                         unsigned int pael,
                         unsigned int adtype,
                         const void * ad_data,
                         unsigned int adlen);
*/
void aed_free (struct aed_s *);
struct aed_s * aed_decode (struct aed_s *, const uint8_t space[], size_t spacelen);
size_t aed_encode (const struct aed_s *, uint8_t * space, size_t spacelen);
size_t aed_len (const struct aed_s *);
int aed_cmp (const struct aed_s *, const struct aed_s *);
int aed_repr (const struct aed_s *, char buf[], int buflen);
void aed_dump (const struct aed_s *);



/* Indirect Entry */
struct ie_s {
    struct tag_s tag;
    struct icbtag_s icbtag;
    struct long_ad_s iicb;
};

struct ie_s * ie_malloc ();
struct ie_s * ie_destroy (struct ie_s *);
#if 0
struct ie_s * ie_init (struct ie_s *,
                       const struct tag_s *,
                       const struct icbtag_s *,
                       const struct long_ad_s *);
#endif //0
void ie_free (struct ie_s *);
struct ie_s * ie_decode (struct ie_s *, const uint8_t space[], size_t spacelen);
size_t ie_encode (const struct ie_s *, uint8_t * space, size_t spacelen);
size_t ie_len (const struct ie_s *);
int ie_cmp (const struct ie_s *, const struct ie_s *);
int ie_repr (const struct ie_s *, char buf[], int buflen);
void ie_dump (const struct ie_s *);




/* Terminal Entry */
struct te_s {
    struct tag_s tag;
    struct icbtag_s icbtag;
};

struct te_s * te_malloc ();
struct te_s * te_destroy (struct te_s *);
struct te_s * te_init (struct te_s *);
void te_free (struct te_s *);
struct te_s * te_decode (struct te_s *, const uint8_t space[], size_t spacelen);
size_t te_encode (const struct te_s *, uint8_t * space, size_t spacelen);
int te_cmp (const struct te_s *, const struct te_s *);
size_t te_len (const struct te_s *);
int te_repr (const struct te_s *, char buf[], int);
void te_dump (const struct te_s *);



/* File Entry */
struct fe_s {
};

struct fe_s * fe_malloc ();
struct fe_s * fe_destroy (struct fe_s *);
struct fe_s * fe_init (struct fe_s *);
void fe_free (struct fe_s *);
struct fe_s * fe_decode (const uint8_t * space, int spacelen);
int fe_encode (const struct fe_s *, uint8_t * space, int spacelen);
int fe_len (const struct fe_s *);
int fe_cmp (const struct fe_s *, const struct fe_s *);
int fe_repr (const struct fe_s *, char buf[], int buflen);
void fe_dump (const struct fe_s *);



/* Extended Attribute Header Descriptor */
struct eahd_s {
};

struct eahd_s * eahd_malloc ();
struct eahd_s * eahd_destroy (struct eahd_s *);
struct eahd_s * eahd_init (struct eahd_s *, unsigned int ial, unsigned int aal);
void eahd_free (struct eahd_s *);
struct eahd_s * eahd_decode (const uint8_t * space, int spacelen);
int eahd_encode (const struct eahd_s *, uint8_t * space, int spacelen);
int eahd_len (const struct eahd_s *);
int eahd_cmp (const struct eahd_s *, const struct eahd_s *);
int eahd_repr (const struct eahd_s *, char buf[], int buflen);
void eahd_dump (const struct eahd_s *);



/* Unallocated Space Entry */
struct use_s {
};

struct use_s * use_malloc ();
struct use_s * use_destroy (struct use_s *);
struct use_s * use_init (struct use_s *,
                         const struct tag_s * tag,
                         const struct icbtag_s * icbtag,
                         ...);
void use_free (struct use_s *);
struct use_s * use_decode (const uint8_t * space, int spacelen);
int use_encode (const struct use_s *, uint8_t * space, int spacelen);
int use_len (const struct use_s *);
int use_cmp (const struct use_s *, const struct use_s *);
int use_repr (const struct use_s *, char buf[], int buflen);
void use_dump (const struct use_s *);



/* Space Bitmap Descriptor */
struct sbd_s {
};

struct sbd_s * sbd_malloc ();
struct sbd_s * sbd_destroy (struct sbd_s *);
struct sbd_s * sbd_init (struct sbd_s *,
                         unsigned int nbits,
                         unsigned int nbytes,
                         uint8_t bitmap[]);
void sbd_free (struct sbd_s *);
struct sbd_s * sbd_decode (const uint8_t * space, int spacelen);
int sbd_encode (const struct sbd_s *, uint8_t * space, int spacelen);
int sbd_len (const struct sbd_s *);
int sbd_cmp (const struct sbd_s *, const struct sbd_s *);
int sbd_repr (const struct sbd_s *, char buf[], int buflen);
void sbd_dump (const struct sbd_s *);



/* Partition Integrity Entry */
struct pie_s {
};

struct pie_s * pie_malloc ();
struct pie_s * pie_destroy (struct pie_s *);
struct pie_s * pie_init (struct pie_s *);
void pie_free (struct pie_s *);
struct pie_s * pie_decode (const uint8_t * space, int spacelen);
int pie_encode (const struct pie_s *, uint8_t * space, int spacelen);
int pie_len (const struct pie_s *);
int pie_cmp (const struct pie_s *, const struct pie_s *);
int pie_repr (const struct pie_s *, char buf[], int buflen);
void pie_dump (const struct pie_s *);



/* Extended File Entry */
struct efe_s {
};

struct efe_s * efe_malloc ();
struct efe_s * efe_destroy (struct efe_s *);
struct efe_s * efe_init (struct efe_s *);
void efe_free (struct efe_s *);
struct efe_s * efe_decode (const uint8_t * space, int spacelen);
int efe_encode (const struct efe_s *, uint8_t * space, int spacelen);
int efe_len (const struct efe_s *);
int efe_cmp (const struct efe_s *, const struct efe_s *);
int efe_repr (const struct efe_s *, char buf[], int buflen);
void efe_dump (const struct efe_s *);








/* File Structure (Part 3) union. */
union filst_u {
    struct {
	struct tag_s tag;
	struct icbtag_s icbtag; /* Caution: invalid access for non-ICB puns. */
    } generic;

    /* Tag-only (non-ICB) */
    struct fsd_s fsd;
    struct fid_s fid;
    struct aed_s aed;
//    struct aehd_s aehd;

    /* ICB-tag */
    struct ie_s ie;
    struct te_s te;
    struct fe_s fe;
    struct use_s use;
    struct sbd_s sbd;
    struct pie_s pie;
    struct efe_s efe;
};

typedef union filst_u filst_t;




typedef enum tagid_e (*filst_tagtype)();
typedef int (*filst_nfields)();
typedef const struct layoutfield_s* (*filst_layoutdescr)();

typedef bool (*filst_isa)(const filst_t *self);
typedef filst_t* (*filst_malloc)(size_t dlen);
typedef filst_t* (*filst_destroy)(filst_t *self);
typedef void (*filst_free)(filst_t *self);
typedef filst_t* (*filst_decode)(filst_t *self, const uint8_t space[], size_t spacelen);
typedef size_t (*filst_encode)(const filst_t *self, uint8_t *space, size_t spacelen);
typedef size_t (*filst_len)(const filst_t *self);
typedef int (*filst_cmp)(const filst_t *self, const filst_t *other);
typedef int (*filst_repr)(const filst_t *self, char buf[], int buflen);
typedef void (*filst_dump)(const filst_t *self);




/* Function Table for File Structures. */
struct filstfuncs_s {
  enum tagid_e (*tagtype)();
  int (*nfields)();
  const struct layoutfield_s* (*layoutdescr)();

  bool (*isa)(const filst_t *self);
  filst_t* (*malloc)(size_t dlen);
  filst_t* (*destroy)(filst_t *self);
  void (*free)(filst_t *self);
  filst_t* (*decode)(filst_t *self, const uint8_t space[], size_t spacelen);
  size_t (*encode)(const filst_t *self, uint8_t *space, size_t spacelen);
  size_t (*len)(const filst_t *self);
  int (*cmp)(const filst_t *self, const filst_t *other);
  int (*repr)(const filst_t *self, char buf[], int buflen);
  void (*dump)(const filst_t *self);
};

typedef struct filstfuncs_s filstfuncs_t;


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




#endif // _FILST_H_
