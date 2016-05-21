#include <stdio.h>

#include "util.c"
#include "basics.c"
#include "conv.c"

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

  lb_addr_dump(obj);
}




int test_fid1 ()
{
  uint8_t testdata[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00,
      0x00,
      0x0a,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00,
      'P', 'o', 't', 'a', 't', 'o', '.', 'p', 'n', 'g',
  };

  struct fid_s * fid0 = NULL;
  printf("fid0=%p\n", fid0);
  fid0 = fid_decode(fid0, testdata, sizeof(testdata));

  fid_dump(fid0);
}



int test_fsd1 ()
{
  uint8_t testdata[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00, 0x00,
      0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00,
  };
}

int test_fsd2 ()
{
  uint8_t testdata[512];
  struct fsd_s * fsd0;

  fsd0 = fsd_malloc();

  fsd_dump(fsd0);

  return -1;
}

int test_fsd3 ()
{
  uint8_t bindata[512] = { 0, };
  struct fsd_s * fsd0;

  fsd0 = fsd_malloc();
  fsd0->tag.tagid = TAGID_FSD;
  fsd0->il = 2;
  fsd0->mil = 3;
  fsd_encode(fsd0, bindata, sizeof(bindata));

  fsd_dump(fsd0);
  printf("fsd %p:\n", fsd0);
  hexdump(bindata, sizeof(bindata));

  return -1;
}


int main ()
{
  test_pc1();
  test_pn1();
  test_lb_addr1();
  test_fid1();
  test_fsd3();
  return 0;
}


