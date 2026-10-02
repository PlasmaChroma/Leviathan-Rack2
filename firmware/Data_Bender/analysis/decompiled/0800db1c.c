/* 0800db1c FUN_0800db1c; analyst naming is provisional. */

void FUN_0800db1c(uint **param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  
  puVar3 = DAT_0800dc7c;
  *(undefined *)((int)param_1 + 0x42) = 0;
  param_1[0xb] = puVar3;
  *(undefined2 *)((int)param_1 + 0x2a) = 0;
  param_1[0x11] = (uint *)(param_2 | (uint)param_1[0x11]);
  puVar3 = DAT_0800dc80;
  if (*(byte *)((int)param_1 + 0x41) - 0x28 < 3) {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar2 = 0x46;
    }
    else {
      uVar2 = 0xf6;
    }
    puVar4 = *param_1;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar2 = ~uVar2;
    }
    else {
      uVar2 = 0xffffff09;
    }
    *puVar4 = *puVar4 & uVar2;
    *(undefined *)((int)param_1 + 0x41) = 0x28;
    param_1[0xd] = puVar3;
  }
  else {
    puVar4 = *param_1;
    *puVar4 = *puVar4 & 0xffffff01;
    if ((int)(puVar4[6] << 0x1e) < 0) {
      puVar4[10] = 0;
    }
    if (-1 < (int)(puVar4[6] << 0x1f)) {
      puVar4[6] = puVar4[6] | 1;
    }
    if ((*(char *)((int)param_1 + 0x41) != '`') &&
       (*(undefined *)((int)param_1 + 0x41) = 0x20, (int)(puVar4[6] << 0x1a) < 0)) {
      if ((int)(puVar4[6] << 0x1b) < 0) {
        puVar4[7] = 0x10;
        param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
      }
      puVar4[7] = 0x20;
    }
    param_1[0xd] = (uint *)0x0;
  }
  puVar3 = param_1[0xc];
  if ((param_1[0xe] == (uint *)0x0) || ((puVar3 != (uint *)0x11 && (puVar3 != (uint *)0x21)))) {
    if ((param_1[0xf] != (uint *)0x0) && ((puVar3 == (uint *)0x12 || (puVar3 == (uint *)0x22)))) {
      if ((int)(*puVar4 << 0x10) < 0) {
        *puVar4 = *puVar4 & 0xffff7fff;
      }
      iVar1 = FUN_0800c074();
      if (iVar1 != 1) {
        param_1[0xf][0x14] = DAT_0800dc84;
        *(undefined *)(param_1 + 0x10) = 0;
        iVar1 = FUN_0800b840();
        if (iVar1 == 0) {
          return;
        }
                    /* WARNING: Could not recover jumptable at 0x0800dc18. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (*(code *)param_1[0xf][0x14])();
        return;
      }
    }
  }
  else {
    if ((int)(*puVar4 << 0x11) < 0) {
      *puVar4 = *puVar4 & 0xffffbfff;
    }
    iVar1 = FUN_0800c074();
    if (iVar1 != 1) {
      param_1[0xe][0x14] = DAT_0800dc84;
      *(undefined *)(param_1 + 0x10) = 0;
      iVar1 = FUN_0800b840();
      if (iVar1 == 0) {
        return;
      }
                    /* WARNING: Could not recover jumptable at 0x0800dbe4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)param_1[0xe][0x14])();
      return;
    }
  }
  if (*(char *)((int)param_1 + 0x41) != '`') {
    param_1[0xc] = (uint *)0x0;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_0800802c(param_1);
    return;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  param_1[0xc] = (uint *)0x0;
  FUN_0800dad8(param_1);
  return;
}


