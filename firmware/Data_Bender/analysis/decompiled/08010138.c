/* 08010138 FUN_08010138; analyst naming is provisional. */

undefined4 FUN_08010138(int *param_1,uint param_2)

{
  byte bVar1;
  uint *puVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  
  puVar2 = DAT_08010370;
  if (param_1 == (int *)0x0) {
    return 1;
  }
  if (((*DAT_08010370 & 0xf) < param_2) &&
     (*DAT_08010370 = *DAT_08010370 & 0xfffffff0 | param_2, (*puVar2 & 0xf) != param_2)) {
    return 1;
  }
  iVar8 = *param_1;
  if ((iVar8 << 0x1d < 0) && ((DAT_08010374[6] & 0x70U) < (uint)param_1[4])) {
    DAT_08010374[6] = DAT_08010374[6] & 0xffffff8fU | param_1[4];
  }
  if ((iVar8 << 0x1c < 0) && ((DAT_08010374[7] & 0x70U) < (uint)param_1[5])) {
    DAT_08010374[7] = DAT_08010374[7] & 0xffffff8fU | param_1[5];
  }
  if ((iVar8 << 0x1b < 0) && ((DAT_08010374[7] & 0x700U) < (uint)param_1[6])) {
    DAT_08010374[7] = DAT_08010374[7] & 0xfffff8ffU | param_1[6];
  }
  if ((iVar8 << 0x1a < 0) && ((DAT_08010374[8] & 0x70U) < (uint)param_1[7])) {
    DAT_08010374[8] = DAT_08010374[8] & 0xffffff8fU | param_1[7];
  }
  if (iVar8 << 0x1e < 0) {
    uVar7 = param_1[3];
    if ((DAT_08010374[6] & 0xfU) < uVar7) {
      DAT_08010374[6] = DAT_08010374[6] & 0xfffffff0U | uVar7;
    }
    if (iVar8 << 0x1f < 0) goto LAB_08010200;
  }
  else {
    if (-1 < iVar8 << 0x1f) goto LAB_0801026e;
LAB_08010200:
    piVar3 = DAT_08010374;
    DAT_08010374[6] = DAT_08010374[6] & 0xfffff0ffU | param_1[2];
    piVar4 = DAT_08010374;
    uVar7 = param_1[1];
    iVar8 = *piVar3;
    if (uVar7 == 2) {
      iVar8 = iVar8 << 0xe;
    }
    else if (uVar7 == 3) {
      iVar8 = iVar8 << 6;
    }
    else if (uVar7 == 1) {
      iVar8 = iVar8 << 0x17;
    }
    else {
      iVar8 = iVar8 << 0x1d;
    }
    if (-1 < iVar8) {
      return 1;
    }
    DAT_08010374[4] = DAT_08010374[4] & 0xfffffff8U | uVar7;
    iVar8 = FUN_08009ce8();
    while ((piVar4[4] & 0x38U) != param_1[1] * 8) {
      iVar5 = FUN_08009ce8();
      if (5000 < (uint)(iVar5 - iVar8)) {
        return 3;
      }
    }
    iVar8 = *param_1;
    if (-1 < iVar8 << 0x1e) goto LAB_0801026e;
    uVar7 = param_1[3];
  }
  if (uVar7 < (DAT_08010374[6] & 0xfU)) {
    DAT_08010374[6] = uVar7 | DAT_08010374[6] & 0xfffffff0U;
  }
LAB_0801026e:
  puVar2 = DAT_08010370;
  if ((param_2 < (*DAT_08010370 & 0xf)) &&
     (*DAT_08010370 = *DAT_08010370 & 0xfffffff0 | param_2, (*puVar2 & 0xf) != param_2)) {
    return 1;
  }
  if ((iVar8 << 0x1d < 0) && ((uint)param_1[4] < (DAT_08010374[6] & 0x70U))) {
    DAT_08010374[6] = DAT_08010374[6] & 0xffffff8fU | param_1[4];
  }
  if ((iVar8 << 0x1c < 0) && ((uint)param_1[5] < (DAT_08010374[7] & 0x70U))) {
    DAT_08010374[7] = DAT_08010374[7] & 0xffffff8fU | param_1[5];
  }
  if ((iVar8 << 0x1b < 0) && ((uint)param_1[6] < (DAT_08010374[7] & 0x700U))) {
    DAT_08010374[7] = DAT_08010374[7] & 0xfffff8ffU | param_1[6];
  }
  if ((iVar8 << 0x1a < 0) && ((uint)param_1[7] < (DAT_08010374[8] & 0x70U))) {
    DAT_08010374[8] = DAT_08010374[8] & 0xffffff8fU | param_1[7];
  }
  uVar7 = FUN_080100f4();
  puVar2 = DAT_08010380;
  bVar1 = *(byte *)(DAT_08010378 + (DAT_08010374[6] & 0xfU));
  uVar7 = uVar7 >> (*(byte *)(DAT_08010378 + ((uint)(DAT_08010374[6] << 0x14) >> 0x1c)) & 0x1f);
  uVar6 = *DAT_08010384;
  *DAT_0801037c = uVar7;
  *puVar2 = uVar7 >> (bVar1 & 0x1f);
  uVar6 = FUN_08009c24(uVar6);
  return uVar6;
}


