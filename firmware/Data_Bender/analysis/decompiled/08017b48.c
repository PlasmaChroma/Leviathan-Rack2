/* 08017b48 libm_cosf; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_cosf(float param_1)

{
  longlong lVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 in_r3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  float fVar13;
  double dVar14;
  
  uVar4 = (uint)((int)param_1 << 1) >> 0x15;
  dVar14 = (double)param_1;
  if (uVar4 < 0x3f4) {
    dVar14 = dVar14 * dVar14;
    if (0x397 < uVar4) {
      return (float)(*(double *)(DAT_08017cf8 + 0x30) + dVar14 * *(double *)(DAT_08017cf8 + 0x38) +
                     dVar14 * dVar14 * *(double *)(DAT_08017cf8 + 0x40) +
                    (*(double *)(DAT_08017cf8 + 0x48) + dVar14 * *(double *)(DAT_08017cf8 + 0x50)) *
                    dVar14 * dVar14 * dVar14);
    }
    return 1.0;
  }
  if (uVar4 < 0x42f) {
    uVar4 = (int)(longlong)(dVar14 * *(double *)(DAT_08017cf8 + 0x20)) + 0x800000 >> 0x18;
    iVar3 = DAT_08017cf8 + 0x70;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08017cf8;
    }
    dVar14 = dVar14 + -(double)(longlong)(int)uVar4 * *(double *)(DAT_08017cf8 + 0x28);
    dVar12 = dVar14 * dVar14;
    if (-1 < (int)(uVar4 << 0x1f)) {
      dVar7 = *(double *)(iVar3 + 0x50);
      dVar11 = *(double *)(iVar3 + 0x48);
      dVar8 = *(double *)(iVar3 + 0x38);
      dVar14 = *(double *)(iVar3 + 0x30);
      dVar9 = *(double *)(iVar3 + 0x40);
LAB_08017c28:
      return (float)(dVar14 + dVar12 * dVar8 + dVar12 * dVar12 * dVar9 +
                    (dVar11 + dVar12 * dVar7) * dVar12 * dVar12 * dVar12);
    }
    dVar7 = *(double *)(DAT_08017cf8 + (uVar4 & 3) * 8);
    dVar9 = *(double *)(iVar3 + 0x68);
    dVar11 = *(double *)(iVar3 + 0x60);
    dVar8 = *(double *)(iVar3 + 0x58);
  }
  else {
    if (0x7f7 < uVar4) {
      fVar13 = (param_1 - param_1) / (param_1 - param_1);
      if (!NAN(param_1)) {
        puVar2 = (undefined4 *)FUN_080188d8();
        *puVar2 = 0x21;
        return fVar13;
      }
      return fVar13;
    }
    uVar4 = (uint)((int)param_1 << 2) >> 0x1c;
    iVar3 = DAT_08017cfc + uVar4 * 4;
    uVar5 = ((uint)param_1 & 0x7fffff | 0x800000) << ((uint)((int)param_1 << 6) >> 0x1d);
    lVar1 = (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x10) +
            ((ulonglong)(uVar5 * *(int *)(DAT_08017cfc + uVar4 * 4)) << 0x20 |
            (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x20) >> 0x20);
    iVar3 = (int)((ulonglong)lVar1 >> 0x20);
    uVar6 = iVar3 + 0x20000000U >> 0x1e;
    dVar14 = (double)FUN_0800069c((int)lVar1,iVar3 - (iVar3 + 0x20000000U & 0xc0000000),iVar3,uVar5,
                                  in_r3);
    uVar4 = uVar6 - ((int)param_1 >> 0x1f);
    iVar3 = DAT_08017d00;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08017d00 + -0x70;
    }
    dVar14 = dVar14 * DAT_08017cf0;
    dVar12 = dVar14 * dVar14;
    if (-1 < (int)(uVar6 << 0x1f)) {
      dVar7 = *(double *)(iVar3 + 0x50);
      dVar11 = *(double *)(iVar3 + 0x48);
      dVar8 = *(double *)(iVar3 + 0x38);
      dVar14 = *(double *)(iVar3 + 0x30);
      dVar9 = *(double *)(iVar3 + 0x40);
      goto LAB_08017c28;
    }
    dVar7 = *(double *)(DAT_08017d00 + -0x70 + (uVar4 & 3) * 8);
    dVar9 = *(double *)(iVar3 + 0x68);
    dVar11 = *(double *)(iVar3 + 0x60);
    dVar8 = *(double *)(iVar3 + 0x58);
  }
  dVar10 = dVar14 * dVar7 * dVar12;
  return (float)(dVar14 * dVar7 + dVar10 * dVar8 + (dVar11 + dVar12 * dVar9) * dVar12 * dVar10);
}


