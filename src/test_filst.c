#include <stdio.h>

#include "filst.c"



/* Path Component. */
int test_pc1 ()
{
  uint8_t testdata[10] = {
      0x05, 0x06,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o',
  };

  const struct path_component_s * obj = path_component_decode(testdata, sizeof(testdata));

  path_component_dump(obj);
  return 0;
}


int test_pn1 ()
{
  uint8_t pc0[4] = {
      0x02, 0x00,  0x00, 0x00,
  };
  uint8_t pc1[11] = {
      0x05, 0x07,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o', 's',
  };
  uint8_t pc2[10] = {
      0x05, 0x06,  0x00,0x00,
      'c', 'a', 'n', 'n', 'o', 'n',
  };

  uint8_t pn0[25] = {
      0x02, 0x00,  0x00, 0x00,
      0x05, 0x07,  0x00,0x00,
      'p', 'o', 't', 'a', 't', 'o', 's',
      0x05, 0x06,  0x00,0x00,
      'c', 'a', 'n', 'n', 'o', 'n',
  };

  const struct pathname_s * obj = pathname_decode(pn0, sizeof(pn0));
}


int test_lb_addr1 ()
{
  uint8_t testdata[6] = {
    0x23, 0x74, 0x01, 0x00,
    0x02, 0x00,
  };
  const struct lb_addr_s * obj = lb_addr_decode(testdata, sizeof(testdata));

  printf("lb_addr(@%p) {\n", obj);
  printf("  lbn=%u, prn=%d\n", obj->lbn, obj->prn);
  printf("}\n");

}


int main ()
{
  test_pc1();
  test_pn1();
  test_lb_addr1();
  return 0;
}


