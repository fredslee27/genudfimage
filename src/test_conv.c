#include <string.h>

#include "basics.c"
#include "conv.c"



int
test_conv0 ()
{
  uint8_t testdata[4] = {
      0x01, 0x00,  0x00,0x00
  };
  layoutvalue_t contents[5] = { 0, };
  layoutvalue_t expected[5] = { 1, 0, 0, { .ptr=testdata+4 }, 4 };

  int res = udf_decode(testdata, sizeof(testdata), udf_path_component, contents);
  int retval = memcmp(contents, expected, sizeof(expected));

  printf("test0\n");
  layoutvalues_dump(contents, 5);
  printf(" ==> %d\n", retval);

  return retval;
}

int
test_conv1 ()
{
  uint8_t testdata[10] = {
      0x05, 0x06,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o',
  };
  layoutvalue_t contents[5] = { 0, };
  layoutvalue_t expected[5] = { 5, 6, 0, { .ptr=testdata+4 }, 10 };

  int res = udf_decode(testdata, sizeof(testdata), udf_path_component, contents);
  int retval = memcmp(contents, expected, sizeof(expected));

  printf("test1\n");
  layoutvalues_dump(contents, 5);
  printf(" ==> %d\n", retval);
  return retval;
}


int
test_deconv1 ()
{
  char * s = "Potato";
  layoutvalue_t contents[5] = { 5, 6, 0, {.ptr=s}, 10 };
  uint8_t bindata[512] = { 0, };
  uint8_t expected[10] = { 0x05, 0x06, 0x00, 0x00,  'P', 'o', 't', 'a', 't', 'o' };
  int enclen;

  enclen = udf_encode(bindata, sizeof(bindata), udf_path_component, contents);
  int retval = memcmp(bindata, expected, sizeof(expected));

  printf("deconv1:\n");
  int x;
  for (x = 0; x < 32; x++)
    {
      printf("%02x ", bindata[x]);
    }
  puts("");
#if 0
  for (x = 0; x < sizeof(expected); x++)
    {
      printf("%02x ", expected[x]);
    }
  puts("");
#endif //0
  printf(" ==> %d\n", retval);
  return retval;
}





int main ()
{
  test_conv0();
  test_conv1();

  test_deconv1();
  return 0;
}

