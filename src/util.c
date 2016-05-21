#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

/*
   Re-indent a repr() string by replacing '\n' with "\n"+spaces.
*/
int
reindent_repr (char *buf, int buflen, int indentdepth)
{
  int n = 0;
  char *tmp, *p;
  int tmplen = buflen*indentdepth;

  tmp = calloc(1, tmplen);
//  n += snprintf(tmp+n, tmplen-n, "%*s", indentdepth, "");
  for (p = buf; *p && (p < buf+buflen); p++)
    {
      tmp[n] = *p;
      n++;

      if (*p == '\n')
	{
	  n += snprintf(tmp+n, tmplen-n, "%*s", indentdepth, "");
	}
    }
  tmp[n] = 0;

  memcpy(buf, tmp, buflen);
  buf[buflen-1] = 0;

  free(tmp);
  return n;
}


int
hexdump (uint8_t *bin, int binlen)
{
  int n, x, y;
  uint8_t * row;

  y = 0;
  row = bin + 0;
  while (y*16 < binlen)
    {
      row = bin + (y*16);
      n = (y*16);
      printf("%08X | ", (y*16));
      for (x = 0; (x < 16) && (y*16+x < binlen); x++)
	{
	  printf("%02X ", bin[y*16+x]);
	}
      printf("| ");
      for (x = 0; (x < 16) && (y*16+x < binlen); x++)
	{
	  int ch = bin[y*16+x];
	  if ((ch < 32) || (ch >= 127))
	    ch = '.';
	  printf("%c", ch);
	}
      printf("\n");
      y += 1;
    }
  return 0;
}

