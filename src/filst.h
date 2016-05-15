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

    int ncomponents;
    struct path_component_s * components;
};


