/* 08000bac FUN_08000bac; analyst naming is provisional. */

void FUN_08000bac(int *param_1)

{
  ushort uVar1;
  float fVar2;
  int iVar3;
  ushort uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  ushort uVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  
  fVar11 = DAT_08000d20;
  fVar10 = (float)param_1[2];
  fVar9 = fVar10 * (float)param_1[3] * DAT_08000d20;
  iVar7 = *param_1;
  if ((int)((uint)(fVar9 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar9 == DAT_08000d20 || fVar9 < DAT_08000d20 != (NAN(fVar9) || NAN(DAT_08000d20))) {
    uVar5 = (uint)(0.0 < fVar9) * (int)fVar9 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  iVar3 = *(int *)(iVar7 + 4);
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  fVar9 = fVar10 * (float)param_1[4] * DAT_08000d20;
  iVar6 = ((int)(uint)*(byte *)(param_1 + 1) >> 4) * 0x41 + (*(byte *)(param_1 + 1) & 0xf) * 4 +
          iVar3;
  uVar4 = *(ushort *)(iVar6 + 1) & 0xfff;
  uVar8 = uVar1 + uVar4;
  if (0xffe < uVar1) {
    uVar4 = uVar4 | 0x1000;
  }
  *(ushort *)(iVar6 + 1) = uVar4;
  *(ushort *)(iVar6 + 3) = uVar8 & 0xfff;
  fVar2 = DAT_08000d20;
  if ((int)((uint)(fVar9 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar9 == fVar11 || fVar9 < fVar11 != (NAN(fVar9) || NAN(fVar11))) {
    uVar5 = (uint)(0.0 < fVar9) * (int)fVar9 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  fVar11 = fVar10 * (float)param_1[5] * DAT_08000d20;
  iVar6 = ((int)(uint)*(byte *)((int)param_1 + 5) >> 4) * 0x41 +
          (*(byte *)((int)param_1 + 5) & 0xf) * 4 + iVar3;
  uVar4 = *(ushort *)(iVar6 + 1) & 0xfff;
  uVar8 = uVar1 + uVar4;
  if (0xffe < uVar1) {
    uVar4 = uVar4 | 0x1000;
  }
  *(ushort *)(iVar6 + 1) = uVar4;
  *(ushort *)(iVar6 + 3) = uVar8 & 0xfff;
  if ((int)((uint)(fVar11 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar11 == fVar2 || fVar11 < fVar2 != (NAN(fVar11) || NAN(fVar2))) {
    uVar5 = (uint)(0.0 < fVar11) * (int)fVar11 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  iVar3 = ((int)(uint)*(byte *)((int)param_1 + 6) >> 4) * 0x41 +
          (*(byte *)((int)param_1 + 6) & 0xf) * 4 + iVar3;
  uVar8 = *(ushort *)(iVar3 + 1) & 0xfff;
  uVar4 = uVar1 + uVar8;
  if (0xffe < uVar1) {
    uVar8 = uVar8 | 0x1000;
  }
  *(ushort *)(iVar3 + 1) = uVar8;
  *(ushort *)(iVar3 + 3) = uVar4 & 0xfff;
  return;
}


