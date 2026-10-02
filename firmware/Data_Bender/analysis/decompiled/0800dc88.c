/* 0800dc88 FUN_0800dc88; analyst naming is provisional. */

void FUN_0800dc88(uint **param_1,uint param_2)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  
  puVar6 = *param_1;
  uVar4 = *puVar6;
  puVar7 = param_1[0xb];
  bVar1 = *(byte *)((int)param_1 + 0x41);
  puVar6[7] = 0x20;
  if ((bVar1 - 0x28 < 2) || (bVar1 == 0x21)) {
    *puVar6 = *puVar6 & 0xffffff05;
    param_1[0xc] = (uint *)0x21;
  }
  else if ((bVar1 & 0xf7) == 0x22) {
    *puVar6 = *puVar6 & 0xffffff03;
    param_1[0xc] = (uint *)0x22;
  }
  uVar3 = DAT_0800de4c;
  puVar6[1] = puVar6[1] | 0x8000;
  puVar6[1] = puVar6[1] & uVar3;
  if ((int)(puVar6[6] << 0x1e) < 0) {
    puVar6[10] = 0;
  }
  if (-1 < (int)(puVar6[6] << 0x1f)) {
    puVar6[6] = puVar6[6] | 1;
  }
  if ((int)(uVar4 << 0x11) < 0) {
    *puVar6 = *puVar6 & 0xffffbfff;
    puVar5 = param_1[0xe];
  }
  else {
    if (-1 < (int)(uVar4 << 0x10)) goto LAB_0800dcfa;
    *puVar6 = *puVar6 & 0xffff7fff;
    puVar5 = param_1[0xf];
  }
  if (puVar5 != (uint *)0x0) {
    *(short *)((int)param_1 + 0x2a) = (short)*(undefined4 *)(*puVar5 + 4);
  }
LAB_0800dcfa:
  if ((int)(param_2 << 0x1d) < 0) {
    param_2 = param_2 & 0xfffffffb;
    *(char *)param_1[9] = (char)puVar6[9];
    param_1[9] = (uint *)((int)param_1[9] + 1);
    if (*(short *)(param_1 + 10) != 0) {
      *(short *)(param_1 + 10) = *(short *)(param_1 + 10) + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
    }
  }
  if (*(short *)((int)param_1 + 0x2a) != 0) {
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
  }
  if (((int)(param_2 << 0x1b) < 0) && ((int)(uVar4 << 0x1b) < 0)) {
    if (*(short *)((int)param_1 + 0x2a) == 0) {
      if ((puVar7 == (uint *)0x2000000) && (*(char *)((int)param_1 + 0x41) == '(')) {
        FUN_0800da68(param_1,param_2);
      }
      else {
        cVar2 = *(char *)((int)param_1 + 0x41);
        puVar6 = *param_1;
        puVar6[7] = 0x10;
        if ((cVar2 == ')') && (puVar7 != (uint *)&H_Reset)) {
          if ((int)(puVar6[6] << 0x1e) < 0) {
            puVar6[10] = 0;
          }
          if (-1 < (int)(puVar6[6] << 0x1f)) {
            puVar6[6] = puVar6[6] | 1;
          }
          FUN_0800d8fc(param_1);
        }
      }
    }
    else {
      (*param_1)[7] = 0x10;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
      if (((uint)puVar7 & 0xfeffffff) == 0) {
        FUN_0800db1c(param_1,param_1[0x11]);
      }
    }
  }
  *(undefined *)((int)param_1 + 0x42) = 0;
  param_1[0xd] = (uint *)0x0;
  puVar6 = DAT_0800de50;
  if (param_1[0x11] == (uint *)0x0) {
    if (param_1[0xb] != DAT_0800de50) {
      FUN_0800d8fc(param_1);
      param_1[0xb] = puVar6;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      *(undefined *)(param_1 + 0x10) = 0;
      param_1[0xc] = (uint *)0x0;
      FUN_0800da64(param_1);
      return;
    }
    cVar2 = *(char *)((int)param_1 + 0x41);
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x0;
    if (cVar2 == '\"') {
      FUN_08008024(param_1);
      return;
    }
    FUN_0800801c();
  }
  else {
    FUN_0800db1c(param_1,param_1[0x11]);
    if (*(char *)((int)param_1 + 0x41) == '(') {
      FUN_0800da68(param_1,param_2);
      return;
    }
  }
  return;
}


