
#include "util.c"

int test_reindent0 ()
{
  char sample[512] = "struct A {\n\
    .a = 5,\n\
    .b = 10,\n\
}";
  printf("sample, before =\n%s\n", sample);
  reindent_repr(sample, sizeof(sample), 2);
  printf("sample, after =\n%s\n", sample);

  return 0;
}


int main ()
{
  test_reindent0();
}
