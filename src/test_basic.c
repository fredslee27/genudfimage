#include "basics.c"


int test_charspec0 ()
{
  uint8_t bindata[64] = {
      0x00,
      'A', 'S', 'C', 'I', 'I', 0x00,
  };
  struct charspec_s expected = {
      .cst = 0,
      .csi = "ASCII",
  };
  struct charspec_s * cs0;
  cs0 = charspec_decode(bindata, sizeof(bindata));


  charspec_dump(cs0);

  charspec_dump(&expected);

  int retval = charspec_cmp(&expected, cs0);
  printf(" ==> %d\n", retval);

  return retval;
}



int test_timestamp0 ()
{
  uint8_t bindata[12] = {
      0xfe, 0x1f,
      0xdc, 0x07,
      0x0c, 0x15,
      0x0c, 0x00, 0x00,
      0x00, 0x00, 0x00,
  };
  struct timestamp_s expected = {
      1, -2047,
      2012, 12, 21,
      12, 0, 0, 0
  };
  struct timestamp_s * ts0;
  ts0 = timestamp_decode(bindata, sizeof(bindata));

  timestamp_dump(ts0);

  timestamp_dump(&expected);

  int retval = timestamp_cmp(&expected, ts0);
  printf(" ==> %d\n", retval);

  return retval;
}



int test_regid0 ()
{
  uint8_t bindata[32] = {
      0x00,
      'T', 'e', 's', 't', 'E', 'n', 't', 'i', 't', 'y',
  };
  struct regid_s expected = {
      .flags = 0,
      .id = "TestEntity",
      .suffix = "\0\0\0\0\0\0\0\0"
  };

  struct regid_s * regid0;
  regid0 = regid_decode(bindata, sizeof(bindata));

  regid_dump(regid0);

  regid_dump(&expected);

  int retval = regid_cmp(&expected, regid0);
  printf(" ==> %d\n", retval);

  return retval;
}



int main ()
{
  test_charspec0();
  test_timestamp0();
  test_regid0();
}
