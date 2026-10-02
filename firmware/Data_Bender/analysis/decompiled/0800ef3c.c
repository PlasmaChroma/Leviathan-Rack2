/* 0800ef3c FUN_0800ef3c; analyst naming is provisional. */

undefined4 FUN_0800ef3c(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  param_2 = param_2 & 0xf;
  param_1[param_2 * 9 + 0x13] = param_4;
  param_1[param_2 * 9 + 0x12] = param_3;
  param_1[param_2 * 9 + 0x14] = 0;
  *(char *)(param_1 + param_2 * 9 + 0xf) = (char)param_2;
  *(undefined *)((int)param_1 + param_2 * 0x24 + 0x3d) = 1;
  uVar2 = param_1[3];
  uVar1 = *param_1;
  if (uVar2 == 1) {
    param_1[param_2 * 9 + 0x16] = param_3;
  }
  FUN_08012700(uVar1,param_1 + param_2 * 9 + 0xf,uVar2 & 0xff,uVar2,param_4);
  return 0;
}


