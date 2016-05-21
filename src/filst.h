#ifndef _FILST_H_
#define _FILST_H_

#include <stdint.h>

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
int lb_addr_encode (struct lb_addr_s * obj, void * space, int spacelen);
int lb_addr_repr (const struct lb_addr_s *, char[], int);
void lb_addr_dump (const struct lb_addr_s *);





#define ICBTAG_ADTYPE_SHORT 0
#define ICBTAG_ADTYPE_LONG 1
#define ICBTAG_ADTYPE_EXT 2
#define ICBTAG_ADTYPE_DIRECT 3

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

struct icbtag_s * icbtag_init (struct icbtag_s *,
			       unsigned int previous_number_of_direct_entries,
			       unsigned int strategy_type,
			       unsigned int strategy_parameter,
			       unsigned int maximum_number_of_entries,
			       enum icb_file_type_e file_type,
			       const struct lb_addr_s * parent_icb_location,
			       unsigned int allocation_descriptor_type);
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

struct tag_s * tag_init (struct tag_s *,
			 enum tagid_e tagid,
			 unsigned int vers,
			 unsigned int serial,
			 unsigned int tagloc);
unsigned int tag_check_sum (struct tag_s *);
int tag_repr (const struct tag_s *, char[], int);
void tag_dump (const struct tag_s *);






struct long_ad_s {
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
struct long_ad_s * long_ad_decode (uint8_t * raw, int rawlen);
int long_ad_encode (const struct long_ad_s *, uint8_t raw[], int rawlen);
int long_ad_cmp (const struct long_ad_s *, const struct long_ad_s *);
int long_ad_repr (const struct long_ad_s *, char[], int);
void long_ad_dump (const struct long_ad_s *);




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
struct fid_s * fid_decode (struct fid_s *, uint8_t * raw, int rawlen);
int fid_encode (const struct fid_s *, uint8_t * raw, int rawlen);
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






#endif // _FILST_H_
