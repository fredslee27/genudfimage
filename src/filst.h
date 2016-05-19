#include <stdint.h>


/* ECMA-167/3 + UDF/3 : File Structure */


#define MAX_COMPONENT_IDENTIFIER_LEN 255



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
    EXTENTTYPE_0,
    EXTENTTYPE_1,
    EXTENTTYPE_2,
    EXTENTTYPE_3,
};

struct lb_addr_s {
    unsigned int et;    /* Extent Type */
    unsigned int lbn;   /* Logical Block Number */
    unsigned int prn;   /* Partition Reference Number */
};

struct lb_addr_s * lb_addr_malloc ();

struct lb_addr_s * lb_addr_init (struct lb_addr_s * obj,
                                 unsigned int extenttype,
                                 unsigned int lbn,
                                 unsigned int prn);
struct lb_addr_s * lb_addr_destroy (struct lb_addr_s * obj);
int lb_addr_encode (struct lb_addr_s * obj, void * space, int spacelen);
struct lb_addr_s * lb_addr_decode (void * space, int spacelen);





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



