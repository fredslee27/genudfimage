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

  struct path_component_s * obj = path_component_decode(testdata, sizeof(testdata));

  path_component_dump(obj);

  path_component_free(obj);
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

  struct pathname_s * obj = pathname_decode(pn0, sizeof(pn0));

  pathname_free(obj);
  return -1;
}


int test_lb_addr1 ()
{
  uint8_t testdata[6] = {
    0x23, 0x74, 0x01, 0x00,
    0x02, 0x00,
  };
  struct lb_addr_s * obj = lb_addr_decode(testdata, sizeof(testdata));

  lb_addr_dump(obj);

  lb_addr_free(obj);
  return -1;
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
  fid0 = fid_decode(testdata, sizeof(testdata));
  printf("fid0=%p\n", fid0);

  fid_dump(fid0);

  fid_free(fid0);
  return -1;
}

int test_fid2 ()
{
  uint8_t bindata[2048] = { 0, };

  struct fid_s * fid2 = NULL;
  const char * fi = "Potato.jpeg";
  //fid2 = fid_malloc(12);
  fid2 = fid_malloc(strlen(fi));

  fid_init(fid2, NULL,
           1, 0x00,
           NULL,
           NULL, 0,
           fi, strlen(fi));
  printf("fid2=%p\n", fid2);

  fid_dump(fid2);

  fid_encode(fid2, bindata, sizeof(bindata));
  printf("fid2=%p +%lu==0x%lx\n", fid2, fid_len(fid2), fid_len(fid2));
  hexdump(bindata, fid_len(fid2));

  fid_free(fid2);
  return -1;
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

  fsd_free(fsd0);
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

  fsd_free(fsd0);
  return -1;
}


int test_ie2 ()
{
  uint8_t bindata[2048] = { 0, };
  struct ie_s * ie2;
  ie2 = ie_malloc(0);
  ie_dump(ie2);
  int n = ie_encode(ie2, bindata, sizeof(bindata));
  hexdump(bindata, n);
  return 0;
}

int test_te2 ()
{
  uint8_t bindata[2048] = { 0, };
  struct te_s * te2;
  te2 = te_malloc(0);
  te_dump(te2);
  int n = te_encode(te2, bindata, sizeof(bindata));
  hexdump(bindata, n);
  return 0;
}


int main ()
{
  test_pc1();

  test_pn1();

  test_lb_addr1();

  test_fid1();
  test_fid2();

  test_fsd3();

  test_te2();
  test_ie2();
  return 0;
}


