/* 080100f4 FUN_080100f4; analyst naming is provisional. */

uint FUN_080100f4(void)

{
  uint uVar1;
  
  uVar1 = DAT_08010128[4] & 0x38;
  if (uVar1 == 0x10) {
    return DAT_08010130;
  }
  if (uVar1 == 0x18) {
    uVar1 = FUN_0800f9d0();
    return uVar1;
  }
  if (uVar1 != 0) {
    return DAT_0801012c;
  }
  if (*DAT_08010128 << 0x1a < 0) {
    return DAT_08010134 >> ((uint)(*DAT_08010128 << 0x1b) >> 0x1e);
  }
  return DAT_08010134;
}


