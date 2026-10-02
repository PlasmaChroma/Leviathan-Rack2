/* 0800eef8 FUN_0800eef8; analyst naming is provisional. */

undefined4 FUN_0800eef8(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  param_2 = param_2 & 0xf;
  param_1[param_2 * 9 + 0xa3] = param_4;
  param_1[param_2 * 9 + 0xa2] = param_3;
  *(char *)(param_1 + param_2 * 9 + 0x9f) = (char)param_2;
  param_1[param_2 * 9 + 0xa4] = 0;
  *(undefined *)((int)param_1 + param_2 * 0x24 + 0x27d) = 0;
  uVar2 = param_1[3];
  uVar1 = *param_1;
  if (uVar2 == 1) {
    param_1[param_2 * 9 + 0xa6] = param_3;
  }
  FUN_08012700(uVar1,param_1 + param_2 * 9 + 0x9f,uVar2 & 0xff,uVar2,param_4);
  return 0;
}


