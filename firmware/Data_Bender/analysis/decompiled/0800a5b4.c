/* 0800a5b4 FUN_0800a5b4; analyst naming is provisional. */

bool FUN_0800a5b4(int *param_1)

{
  ulonglong uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  
  if (param_1 == (int *)0x0) {
    return true;
  }
  if (param_1[0x15] == 0) {
    FUN_080075e0();
    param_1[0x16] = 0;
    *(undefined *)(param_1 + 0x14) = 0;
  }
  iVar2 = *param_1;
  if (*(int *)(iVar2 + 8) << 2 < 0) {
    *(uint *)(iVar2 + 8) = DAT_0800a7a0 & *(uint *)(iVar2 + 8);
  }
  if (-1 < *(int *)(iVar2 + 8) << 3) {
    uVar3 = *DAT_0800a7a4;
    uVar1 = (ulonglong)DAT_0800a7a8;
    *(uint *)(iVar2 + 8) = DAT_0800a7ac & *(uint *)(iVar2 + 8) | 0x10000000;
    for (iVar4 = (uint)(uVar1 * (uVar3 >> 6) >> 0x26) + 1; iVar4 != 0; iVar4 = iVar4 + -1) {
    }
  }
  bVar6 = *(int *)(iVar2 + 8) << 3 < 0;
  if (bVar6) {
    uVar3 = *(uint *)(iVar2 + 8);
    iVar4 = param_1[0x15];
  }
  else {
    param_1[0x15] = param_1[0x15] | 0x10;
    param_1[0x16] = param_1[0x16] | 1;
    uVar3 = *(uint *)(iVar2 + 8);
    iVar4 = param_1[0x15];
  }
  if (((uVar3 & 4) == 0) && (-1 < iVar4 << 0x1b)) {
    param_1[0x15] = param_1[0x15] & 0xfffffefdU | 2;
    if (-1 < *(int *)(iVar2 + 8) << 0x1f) {
      if ((iVar2 == DAT_0800a7b0) || (iVar2 == DAT_0800a7b0 + 0x100)) {
        uVar3 = *(uint *)(DAT_0800a7bc + 8) | *(uint *)(DAT_0800a7b0 + 8);
        iVar2 = DAT_0800a7c0;
      }
      else {
        uVar3 = *(uint *)(DAT_0800a7b4 + 8);
        iVar2 = DAT_0800a7b8;
      }
      if (-1 < (int)(uVar3 << 0x1f)) {
        *(uint *)(iVar2 + 8) = *(uint *)(iVar2 + 8) & 0xffc0ffff | param_1[1];
      }
    }
    uVar3 = FUN_08009d18();
    uVar5 = (uint)*(byte *)(param_1 + 7);
    if ((uVar3 < 0x1004) || (param_1[2] != 0x10)) {
      uVar3 = uVar5 << 0x10 | (uint)*(byte *)((int)param_1 + 0x15) << 0xd | param_1[0xc] |
              param_1[2];
    }
    else {
      uVar3 = uVar5 << 0x10 | (uint)*(byte *)((int)param_1 + 0x15) << 0xd | param_1[0xc] | 0x1c;
    }
    if (uVar5 == 1) {
      uVar3 = uVar3 | (param_1[8] + -1) * 0x20000;
    }
    if (param_1[9] != 0) {
      uVar3 = uVar3 | param_1[9] & 0x3e0U | param_1[10];
    }
    iVar2 = *param_1;
    *(uint *)(iVar2 + 0xc) = uVar3 | DAT_0800a7c4 & *(uint *)(iVar2 + 0xc);
    if (((*(uint *)(iVar2 + 8) & 4) == 0) && (-1 < *(int *)(iVar2 + 8) << 0x1c)) {
      *(uint *)(iVar2 + 0xc) =
           DAT_0800a7c8 & *(uint *)(iVar2 + 0xc) | (uint)*(byte *)(param_1 + 5) << 0xe |
           param_1[0xb];
      if (*(char *)(param_1 + 0xe) == '\x01') {
        *(uint *)(iVar2 + 0x10) =
             param_1[0x10] | param_1[0x11] | (param_1[0xf] + -1) * 0x10000 | param_1[0x12] |
             DAT_0800a7cc & *(uint *)(iVar2 + 0x10) | 1;
      }
      else {
        *(uint *)(iVar2 + 0x10) = *(uint *)(iVar2 + 0x10) & 0xfffffffe;
      }
      *(uint *)(iVar2 + 0x10) = *(uint *)(iVar2 + 0x10) & 0xfffffff | param_1[0xd];
      FUN_0800a438(param_1);
      iVar2 = *param_1;
    }
    if (param_1[3] == 1) {
      *(uint *)(iVar2 + 0x30) = param_1[6] - 1U | *(uint *)(iVar2 + 0x30) & 0xfffffff0;
    }
    else {
      *(uint *)(iVar2 + 0x30) = *(uint *)(iVar2 + 0x30) & 0xfffffff0;
    }
    param_1[0x15] = param_1[0x15] & 0xfffffffcU | 1;
    return !bVar6;
  }
  param_1[0x15] = param_1[0x15] | 0x10;
  return true;
}


