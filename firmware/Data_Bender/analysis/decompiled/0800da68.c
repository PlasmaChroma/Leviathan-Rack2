/* 0800da68 FUN_0800da68; analyst naming is provisional. */

void FUN_0800da68(uint **param_1,int param_2)

{
  uint *puVar1;
  
  puVar1 = DAT_0800dacc;
  param_1[0xd] = (uint *)0x0;
  param_1[0xb] = puVar1;
  param_1[0xc] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  if (param_2 << 0x1d < 0) {
    *(char *)param_1[9] = (char)(*param_1)[9];
    param_1[9] = (uint *)((int)param_1[9] + 1);
    if (*(short *)(param_1 + 10) != 0) {
      *(short *)(param_1 + 10) = *(short *)(param_1 + 10) + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
    }
  }
  puVar1 = *param_1;
  *puVar1 = *puVar1 & 0xffffff01;
  puVar1[7] = 0x10;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800da64();
  return;
}


