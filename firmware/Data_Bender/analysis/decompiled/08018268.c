/* 08018268 libm_sinf; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_sinf(float param_1)

{
  longlong lVar1;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  float fVar9;
  
  uVar4 = (uint)((int)param_1 << 1) >> 0x15;
  dVar8 = (double)param_1;
  if (uVar4 < 0x3f4) {
    dVar6 = dVar8 * dVar8;
    if (0x397 < uVar4) {
      return (float)(dVar8 + dVar8 * dVar6 * *(double *)(DAT_08018468 + 0x58) +
                    (*(double *)(DAT_08018468 + 0x60) + dVar6 * *(double *)(DAT_08018468 + 0x68)) *
                    dVar6 * dVar8 * dVar6);
    }
  }
  else if (uVar4 < 0x42f) {
    uVar4 = (int)(longlong)(dVar8 * *(double *)(DAT_08018468 + 0x20)) + 0x800000 >> 0x18;
    iVar3 = DAT_08018468 + 0x70;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08018468;
    }
    dVar8 = dVar8 + -(double)(longlong)(int)uVar4 * *(double *)(DAT_08018468 + 0x28);
    dVar6 = dVar8 * dVar8;
    if ((int)(uVar4 << 0x1f) < 0) {
      return (float)(*(double *)(iVar3 + 0x30) + dVar6 * *(double *)(iVar3 + 0x38) +
                     dVar6 * dVar6 * *(double *)(iVar3 + 0x40) +
                    (*(double *)(iVar3 + 0x48) + dVar6 * *(double *)(iVar3 + 0x50)) *
                    dVar6 * dVar6 * dVar6);
    }
    dVar8 = dVar8 * *(double *)(DAT_08018468 + (uVar4 & 3) * 8);
    dVar7 = dVar8 * dVar6;
    param_1 = (float)(dVar8 + dVar7 * *(double *)(iVar3 + 0x58) +
                     (*(double *)(iVar3 + 0x60) + dVar6 * *(double *)(iVar3 + 0x68)) * dVar6 * dVar7
                     );
  }
  else {
    if (0x7f7 < uVar4) {
      fVar9 = (param_1 - param_1) / (param_1 - param_1);
      if (!NAN(param_1)) {
        puVar2 = (undefined4 *)FUN_080188d8();
        *puVar2 = 0x21;
        return fVar9;
      }
      return fVar9;
    }
    uVar4 = (uint)((int)param_1 << 2) >> 0x1c;
    iVar3 = DAT_0801846c + uVar4 * 4;
    uVar5 = ((uint)param_1 & 0x7fffff | 0x800000) << ((uint)((int)param_1 << 6) >> 0x1d);
    lVar1 = (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x10) +
            ((ulonglong)(uVar5 * *(int *)(DAT_0801846c + uVar4 * 4)) << 0x20 |
            (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x20) >> 0x20);
    iVar3 = (int)((ulonglong)lVar1 >> 0x20);
    uVar4 = iVar3 + 0x20000000;
    uVar5 = uVar4 >> 0x1e;
    dVar8 = (double)FUN_0800069c((int)lVar1,iVar3 - (uVar4 & 0xc0000000));
    uVar4 = uVar5 - ((int)param_1 >> 0x1f);
    iVar3 = DAT_08018470;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08018470 + -0x70;
    }
    dVar8 = dVar8 * DAT_08018460;
    dVar6 = dVar8 * dVar8;
    if ((int)(uVar5 << 0x1f) < 0) {
      param_1 = (float)(*(double *)(iVar3 + 0x30) + dVar6 * *(double *)(iVar3 + 0x38) +
                        dVar6 * dVar6 * *(double *)(iVar3 + 0x40) +
                       (*(double *)(iVar3 + 0x48) + dVar6 * *(double *)(iVar3 + 0x50)) *
                       dVar6 * dVar6 * dVar6);
    }
    else {
      dVar8 = dVar8 * *(double *)(DAT_08018470 + -0x70 + (uVar4 & 3) * 8);
      dVar7 = dVar8 * dVar6;
      param_1 = (float)(dVar8 + dVar7 * *(double *)(iVar3 + 0x58) +
                       (*(double *)(iVar3 + 0x60) + dVar6 * *(double *)(iVar3 + 0x68)) *
                       dVar6 * dVar7);
    }
  }
  return param_1;
}


