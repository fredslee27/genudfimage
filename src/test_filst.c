#include "filst.c"



/* Path Component. */
int test_pc1 ()
{
  uint8_t testdata[10] = {
      0x05, 0x06,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o',
  };

  const struct path_component_s * obj = path_component_decode(testdata);

  path_component_dump(obj);
  return 0;
}


int main ()
{
  test_pc1();
  return 0;
}


