/* 080047a8 DB_Buffer_WriteSample; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Buffer_WriteSample(undefined4 param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  uint uVar8;
  uint uVar9;
  
  fVar6 = DAT_08004938;
  if (*(char *)(param_2 + 0x110) != '\0') {
    *(undefined *)(param_2 + param_3 + 0x22a) = 0;
    if (param_3 == 1) {
      cVar1 = *(char *)(param_2 + 0x11a);
      *(float *)(param_2 + 0x60) =
           (float)(ulonglong)*(uint *)(param_2 + 0x198) + *(float *)(param_2 + 0x148);
    }
    else {
      cVar1 = *(char *)(param_2 + 0x11a);
    }
    if (cVar1 == '\0') {
      iVar3 = param_2 + param_3 * 4;
      iVar2 = param_2 + param_3 * 0x20;
      uVar4 = *(uint *)(iVar2 + 0x10);
      fVar6 = (float)(ulonglong)*(uint *)(iVar3 + 0x194) + *(float *)(iVar3 + 0x144);
      uVar9 = (uint)(0.0 < fVar6) * (int)fVar6;
      *(undefined4 *)(*(int *)(iVar2 + 0xc) + (uVar9 - uVar4 * (uVar9 / uVar4)) * 4) = param_1;
    }
    param_2 = param_2 + param_3 * 4;
    fVar6 = *(float *)(param_2 + 0x144) + 1.0;
    fVar7 = (float)(ulonglong)(*(int *)(param_2 + 0x154) - 1);
    *(float *)(param_2 + 0x144) = fVar6;
    if (fVar6 != fVar7 && fVar6 < fVar7 == (NAN(fVar6) || NAN(fVar7))) {
      *(undefined4 *)(param_2 + 0x144) = 0;
    }
    return;
  }
  iVar2 = param_2 + param_3 * 4;
  uVar4 = *(uint *)(iVar2 + 0x154);
  fVar7 = *(float *)(iVar2 + 0x144);
  fVar5 = (float)(ulonglong)(uVar4 - 1);
  if ((fVar7 != fVar5 && fVar7 < fVar5 == (NAN(fVar7) || NAN(fVar5))) || (uVar4 == 0)) {
    *(float *)(iVar2 + 0x144) = DAT_08004938;
    fVar7 = fVar6;
  }
  cVar1 = *(char *)(param_2 + 0x11a);
  if (cVar1 == '\0') {
    if (param_3 == 1) {
      *(float *)(param_2 + 0x60) =
           (float)(ulonglong)*(uint *)(param_2 + 0x198) + *(float *)(param_2 + 0x148);
    }
    iVar3 = param_2 + param_3 * 0x20;
    uVar9 = *(uint *)(iVar3 + 0x10);
    fVar7 = (float)(ulonglong)*(uint *)(iVar2 + 0x194) + fVar7;
    uVar8 = (uint)(0.0 < fVar7) * (int)fVar7;
    *(undefined4 *)(*(int *)(iVar3 + 0xc) + (uVar8 - uVar9 * (uVar8 / uVar9)) * 4) = param_1;
    fVar7 = *(float *)(iVar2 + 0x144);
  }
  if (-1 < (int)((uint)(fVar5 < fVar7 + 1.0) << 0x1f)) {
    *(float *)(iVar2 + 0x144) = fVar7 + 1.0;
    return;
  }
  *(undefined4 *)(iVar2 + 0x144) = 0;
  if (*(int *)(iVar2 + 0x22c) != 0) {
    *(int *)(iVar2 + 0x22c) = *(int *)(iVar2 + 0x22c) + -1;
    *(undefined *)(param_2 + param_3 + 0x234) = 1;
  }
  *(undefined *)(param_2 + param_3 + 0x22a) = 1;
  if ((cVar1 == '\0') && (uVar4 >> 2 < (uint)(*(int *)(iVar2 + 0x128) - *(int *)(iVar2 + 0x1a4)))) {
    *(int *)(iVar2 + 0x1a4) = *(int *)(iVar2 + 0x128);
    if (*(int *)(iVar2 + 0x194) != 0) {
      uVar4 = 0;
    }
    *(uint *)(iVar2 + 0x194) = uVar4;
  }
  *(undefined *)(param_4 + param_3) = 1;
  return;
}


