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


  printf("charspec(@%p) {\n", cs0);
  printf("  type=%d, info=\"%s\"\n", cs0->cst, cs0->csi);
  printf("}\n");

  printf("charspec(@%p) {\n", (&expected));
  printf("  type=%d, info=\"%s\"\n", (&expected)->cst, (&expected)->csi);
  printf("}\n");

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

  printf("timestamp(@%p) = {\n", ts0);
  printf(" type=%d, tz=%d\n", ts0->typ, ts0->tz);
  printf(" year=%d, mon=%u, day=%u\n", ts0->year, ts0->month, ts0->day);
  printf(" %02u:%02u:%02u.%06u\n", ts0->hour, ts0->min, ts0->sec, ts0->usec);
  printf("}\n");

  printf("timestamp(@%p) = {\n", (&expected));
  printf(" type=%d, tz=%d\n", (&expected)->typ, (&expected)->tz);
  printf(" year=%d, mon=%u, day=%u\n", (&expected)->year, (&expected)->month, (&expected)->day);
  printf(" %02u:%02u:%02u.%06u\n", (&expected)->hour, (&expected)->min, (&expected)->sec, (&expected)->usec);
  printf("}\n");

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

  printf("regid@%p = {\n", regid0);
  printf("  flags = %c%c\n",
	 regid0->flags.dirty ? 'D' : 'd',
	 regid0->flags.protected ? 'P' : 'p');
  printf("  id = \"%s\"\n", regid0->id);
  printf("  suffix = { %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x }\n", 
	 regid0->suffix[0], regid0->suffix[1], regid0->suffix[2], regid0->suffix[3],
	 regid0->suffix[4], regid0->suffix[5], regid0->suffix[6], regid0->suffix[7]);
  printf("}\n");

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
