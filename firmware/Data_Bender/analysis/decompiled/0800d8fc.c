/* 0800d8fc FUN_0800d8fc; analyst naming is provisional. */

void FUN_0800d8fc(uint **param_1)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar2 = *param_1;
  uVar3 = *puVar2;
  *(undefined *)((int)param_1 + 0x42) = 0;
  if ((int)(uVar3 << 0x11) < 0) {
    *puVar2 = *puVar2 & 0xffffbfff;
  }
  else if ((int)(uVar3 << 0x10) < 0) {
    *puVar2 = *puVar2 & 0xffff7fff;
    cVar1 = *(char *)((int)param_1 + 0x41);
    goto joined_r0x0800d938;
  }
  cVar1 = *(char *)((int)param_1 + 0x41);
joined_r0x0800d938:
  if (cVar1 == ')') {
    *(undefined *)((int)param_1 + 0x41) = 0x28;
    param_1[0xc] = (uint *)0x21;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbd;
    }
    else {
      uVar3 = 0xffffff0d;
    }
    *puVar2 = *puVar2 & uVar3;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_0800801c();
    return;
  }
  if (*(char *)((int)param_1 + 0x41) != '*') {
    return;
  }
  *(undefined *)((int)param_1 + 0x41) = 0x28;
  param_1[0xc] = (uint *)0x22;
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
    uVar3 = 0xffffffbb;
  }
  else {
    uVar3 = 0xffffff0b;
  }
  *puVar2 = uVar3 & *puVar2;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_08008024();
  return;
}


