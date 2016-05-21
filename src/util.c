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

