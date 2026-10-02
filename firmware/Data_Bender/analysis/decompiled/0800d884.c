/* 0800d884 FUN_0800d884; analyst naming is provisional. */

void FUN_0800d884(uint **param_1)

{
  uint uVar1;
  uint uVar2;
  
  *(undefined *)((int)param_1 + 0x42) = 0;
  if (*(char *)((int)param_1 + 0x41) != '!') {
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x12;
    uVar2 = **param_1;
    param_1[0xd] = (uint *)0x0;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar1 = 0xffffffbb;
    }
    else {
      uVar1 = 0xffffff0b;
    }
    **param_1 = uVar1 & uVar2;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_08008014();
    return;
  }
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  param_1[0xc] = (uint *)0x11;
  param_1[0xd] = (uint *)0x0;
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
    uVar2 = 0xffffffbd;
  }
  else {
    uVar2 = 0xffffff0d;
  }
  **param_1 = **param_1 & uVar2;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800800c();
  return;
}


