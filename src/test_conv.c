#include <conv.c>



int
test_conv0 ()
{
  uint8_t testdata[4] = {
      0x01, 0x00,  0x00,0x00
  };
  layoutvalue_t contents[5] = { 0, };

  int res = udf_decode(testdata, udf_path_component, contents);

  printf("test0\n");
  layoutvalues_dump(contents, 5);
  printf("\n");
  return 0;
}

int
test_conv1 ()
{
  uint8_t testdata[10] = {
      0x05, 0x06,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o',
  };
  layoutvalue_t contents[5] = { 0, };

  int res = udf_decode(testdata, udf_path_component, contents);

  printf("test1\n");
  layoutvalues_dump(contents, 5);
  printf("\n");
  return 0;
}





int main ()
{
  test_conv0();
  test_conv1();
  return 0;
}
