#ifndef _UTIL_H_
#define _UTIL_H_
#include <stdint.h>

int reindent_repr (char *buf, int buflen, int indentdepth);
int hexdump (uint8_t * bin, int binlen);


#endif // _UTIL_H_
