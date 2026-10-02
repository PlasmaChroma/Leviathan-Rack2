/* 0800df90 FUN_0800df90; analyst naming is provisional. */

void FUN_0800df90(uint **param_1,int param_2)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  
  puVar4 = *param_1;
  puVar4[7] = 0x20;
  if (*(char *)((int)param_1 + 0x41) == '!') {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbd;
    }
    else {
      uVar3 = 0xffffff0d;
    }
    *puVar4 = *puVar4 & uVar3;
    param_1[0xc] = (uint *)0x11;
  }
  else if (*(char *)((int)param_1 + 0x41) == '\"') {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbb;
    }
    else {
      uVar3 = 0xffffff0b;
    }
    *puVar4 = uVar3 & *puVar4;
    param_1[0xc] = (uint *)0x12;
  }
  puVar4[1] = puVar4[1] & DAT_0800e0b8;
  puVar2 = DAT_0800e0bc;
  param_1[0xd] = (uint *)0x0;
  param_1[0xb] = puVar2;
  if (param_2 << 0x1b < 0) {
    puVar4[7] = 0x10;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
  }
  if ((int)(puVar4[6] << 0x1e) < 0) {
    puVar4[10] = 0;
  }
  if (-1 < (int)(puVar4[6] << 0x1f)) {
    puVar4[6] = puVar4[6] | 1;
  }
  if ((*(char *)((int)param_1 + 0x41) != '`') && (param_1[0x11] == (uint *)0x0)) {
    if (*(char *)((int)param_1 + 0x41) == '!') {
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      param_1[0xc] = (uint *)0x0;
      cVar1 = *(char *)((int)param_1 + 0x42);
      *(undefined *)((int)param_1 + 0x42) = 0;
      if (cVar1 == '@') {
        FUN_0800dad0();
      }
      else {
        FUN_0800800c();
      }
    }
    else if (*(char *)((int)param_1 + 0x41) == '\"') {
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      param_1[0xc] = (uint *)0x0;
      cVar1 = *(char *)((int)param_1 + 0x42);
      *(undefined *)((int)param_1 + 0x42) = 0;
      if (cVar1 == '@') {
        FUN_0800dad4();
      }
      else {
        FUN_08008014();
      }
    }
    return;
  }
  FUN_0800db1c(param_1,param_1[0x11]);
  return;
}


