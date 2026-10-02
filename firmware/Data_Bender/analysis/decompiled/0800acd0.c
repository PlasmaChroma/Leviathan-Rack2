/* 0800acd0 FUN_0800acd0; analyst naming is provisional. */

uint FUN_0800acd0(uint *param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *param_1;
  if ((uVar1 == DAT_0800ada8 + 0x460 ||
       (uVar1 == DAT_0800adb0 + 0x430 ||
       (uVar1 == DAT_0800ada8 + 0x430 ||
       (uVar1 == DAT_0800adb0 + 0x400 ||
       (uVar1 == DAT_0800ada8 + 0x400 ||
       (uVar1 == DAT_0800adb0 + 0x3d0 ||
       (uVar1 == DAT_0800ada8 + 0x3d0 ||
       (uVar1 == DAT_0800adb0 + 0x60 ||
       (uVar1 == DAT_0800ada8 + 0x60 ||
       (uVar1 == DAT_0800adb0 + 0x30 ||
       (uVar1 == DAT_0800ada8 + 0x30 ||
       (uVar1 == DAT_0800adb0 ||
       (uVar1 == DAT_0800ada8 || (uVar1 == DAT_0800adac || uVar1 == DAT_0800ada4)))))))))))))) ||
     (uVar1 == DAT_0800adb4)) {
    uVar2 = (uVar1 & 0xff) - 0x10;
    uVar1 = DAT_0800adc0 & uVar1;
    if (0x5f < uVar2) {
      uVar1 = uVar1 + 4;
    }
    param_1[0x17] =
         (uint)*(byte *)(DAT_0800adbc +
                        ((uint)((int)((ulonglong)DAT_0800adb8 * (ulonglong)uVar2 >> 0x20) << 0x19)
                        >> 0x1d));
    param_1[0x16] = uVar1;
  }
  else {
    uVar1 = uVar1 & 0xffffff00;
    param_1[0x16] = uVar1;
  }
  return uVar1;
}


