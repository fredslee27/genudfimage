#ifndef _CONV_H_
#define _CONV_H_

/* Converter functions, byte-endian sensitive. */
int uint8_encode (void * space, uint8_t val);
int uint16_encode (void * space, uint16_t val);
int uint32_encode (void * space, uint32_t val);
int uint64_encode (void * space, uint64_t val);

uint8_t uint8_decode (const void * space);
uint16_t uint16_decode (const void * space);
uint32_t uint32_decode (const void * space);
uint64_t uint64_decode (const void * space);

int bytestr_encode (const uint8_t * src, int srclen, void * space, int spacelen);
uint8_t * bytestr_decode (uint8_t * dst, int dstlen, const void * space, int spacelen);


enum layoutfield_type_e {
  LAYOUT_END = 0,   /* End of layout; Byte Position used to double check length of raw bytes. */
  LAYOUT_RESERVED,  /* UDF field is named but no value (zero) extracted. */
  LAYOUT_UINT8,     /* UDF field is Uint8, endian-sensitive extraction */
  LAYOUT_UINT16,    /* UDF field is Uint16, endian-sensitive extraction */
  LAYOUT_UINT32,    /* UDF field is Uint32, endian-sensitive extraction */
  LAYOUT_UINT64,    /* UDF field is Uint64, endian-sensitive extraction */
  LAYOUT_UINT128,   /* Not used in UDF, but implemented to keep 64-bit code on its toes. */
  LAYOUT_PTR,	    /* Location of this UDF field as binary string */
};


union layoutvalue_u {
    /* Types larger than one word stored as pointer to the bit pattern. */
#if 1 // 64-bit arch.
    uint64_t word;
    void * oversize;
#endif // 1
    void * ptr;
};
typedef union layoutvalue_u layoutvalue_t;
typedef layoutvalue_t layoutvalues_t[];

struct layoutfield_s {
  /* Variable Byte Position mask.
Each field has a field index, encodable as a bit offset.
If the bit offset is set to 1, add the contents of the corresponding field to the byte position.
e.g. Path Component,
    field 1 is specified as [ 1, 1, Length of Component Identifier = LC_I, Uint8 ]
    field 3 is specified as [ 4, L_CI, Component Identifier, d-characters ]; where L_CI indicates the contents of field 2.
   fld 3 would be encoded as vbpmask=0, rbp=4, fldtype=Uint16
   End would be encode as vbpmask=(1<<1), rbp=4, fldtype=ptr
*/
  unsigned int vbpmask;  /* Variable Byte Position, as bitmask of field indices. */
  int rbp;  /* Relative Byte Position. */

  const char * shortname;
  enum layoutfield_type_e fldtype;
};

/* Layout description is array of layoutfield_s terminated by type=LAYOUT_END. */
typedef struct layoutfield_s layoutdescr_t[];



/* En/De-coding a UDF struct:
Field description provided as layoutfield_t,
Values for those fields are in array of layoutfield_t, indices matching (in the spirit of a register file).
Some field byte positions depend on the contents of other fields, so the
value-holding structure needs to be as non-complex as feasible.
 */


/* ECMA-167/3 4/14.16.1 - Path Component */
extern struct layoutfield_s udf_path_component[];

/* ECMA-167/3 4/7.1 - Recorded Address, lb_addr */
extern struct layoutfield_s udf_lb_addr[];


int layoutvalues_dump (const layoutvalues_t layoutvalues, int count);


int udf_decode (const void * raw, int rawlen, const layoutdescr_t descr, layoutvalues_t contents);
int udf_encode (void * space, int spacelen, const layoutdescr_t descr, const layoutvalues_t contents);


#endif // _CONV_H_
