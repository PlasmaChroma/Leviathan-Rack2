/* 0800493c DB_Buffer_ReadLinear; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DB_Buffer_ReadLinear(int param_1,int param_2,char *param_3)

{
  ulonglong uVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  uint uVar11;
  float fVar12;
  undefined4 uVar13;
  float fVar14;
  
  iVar5 = param_1 + param_2 * 4;
  fVar10 = *(float *)(iVar5 + 0x13c);
  iVar3 = param_2 * 0x20 + 0xc;
  fVar12 = (float)(ulonglong)*(uint *)(iVar5 + 0x19c) + fVar10;
  iVar6 = *(int *)(param_1 + iVar3);
  uVar7 = *(uint *)(param_1 + iVar3 + 4);
  uVar11 = (uint)(0.0 < fVar12) * (int)fVar12;
  if (param_2 == 1) {
    *(float *)(param_1 + 100) = fVar12;
  }
  uVar1 = (ulonglong)uVar11;
  if (uVar7 < uVar11) {
    uVar11 = uVar11 - uVar7 * (uVar11 / uVar7);
  }
  uVar13 = FPMaxNum(*(float *)(param_1 + 0xa4) * *(float *)(iVar5 + 0xa8),
                    *(undefined4 *)(param_1 + 0xb8));
  fVar14 = (float)FPMinNum(uVar13,*(undefined4 *)(param_1 + 0xbc));
  fVar8 = *(float *)(iVar6 + uVar11 * 4);
  fVar9 = (float)FPMinNum(*(undefined4 *)(param_1 + 0xdc),*(undefined4 *)(iVar5 + 0xe0));
  fVar14 = *(float *)(iVar5 + 0xf8) + (fVar14 - *(float *)(iVar5 + 0xf8)) * fVar9;
  fVar10 = fVar10 + fVar14;
  uVar4 = *(int *)(iVar5 + 0x164) - 1;
  fVar9 = *(float *)(iVar6 + ((uVar11 + 1) - uVar7 * ((uVar11 + 1) / uVar7)) * 4);
  *(float *)(iVar5 + 0xf8) = fVar14;
  *(float *)(iVar5 + 0x13c) = fVar10;
  fVar8 = fVar8 + (fVar12 - (float)uVar1) * (fVar9 - fVar8);
  if ((int)((uint)(fVar10 < (float)(ulonglong)*(uint *)(iVar5 + 0x15c)) << 0x1f) < 0) {
    *(float *)(iVar5 + 0x13c) = (float)(ulonglong)uVar4;
    *param_3 = '\x01';
  }
  else {
    fVar12 = (float)(ulonglong)uVar4;
    if (fVar10 != fVar12 && fVar10 < fVar12 == (NAN(fVar10) || NAN(fVar12))) {
      *(float *)(iVar5 + 0x13c) = (float)(ulonglong)*(uint *)(iVar5 + 0x15c);
      *param_3 = '\x01';
      cVar2 = *(char *)(param_1 + 0x11a);
      goto joined_r0x08004a3c;
    }
    if (*param_3 == '\0') {
      return fVar8;
    }
  }
  cVar2 = *(char *)(param_1 + 0x11a);
joined_r0x08004a3c:
  if (cVar2 == '\0') {
    if (fVar14 == 0.0 || fVar14 < 0.0 != NAN(fVar14)) {
      uVar13 = 0;
      if (*(int *)(iVar5 + 0x194) == 0) {
        uVar13 = *(undefined4 *)(iVar5 + 0x154);
      }
      *(undefined4 *)(iVar5 + 0x19c) = uVar13;
    }
    else {
      *(undefined4 *)(iVar5 + 0x19c) = *(undefined4 *)(iVar5 + 0x194);
    }
  }
  return fVar8;
}


