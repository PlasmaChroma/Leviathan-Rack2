/* 0800a968 FUN_0800a968; analyst naming is provisional. */

void FUN_0800a968(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = (uint)(*(int *)(DAT_0800a9d8 + 0xc) << 0x15) >> 0x1d;
  uVar2 = 7 - uVar1;
  if (3 < uVar2) {
    uVar2 = 4;
  }
  if (uVar1 + 4 < 7) {
    param_3 = 0;
    uVar1 = param_3;
  }
  else {
    param_3 = param_3 & ~(-1 << (uVar1 - 3 & 0xff));
    uVar1 = uVar1 - 3;
  }
  param_3 = (param_2 & ~(-1 << (uVar2 & 0xff))) << (uVar1 & 0xff) | param_3;
  if (-1 < (int)param_1) {
    *(char *)(DAT_0800a9dc + param_1 + 0x300) = (char)(param_3 << 4);
    return;
  }
  *(char *)(DAT_0800a9e0 + (param_1 & 0xf) + 0x18) = (char)(param_3 << 4);
  return;
}


