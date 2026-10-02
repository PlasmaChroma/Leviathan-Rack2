/* 08003264 DB_Clock_CaptureEdgeAndMedian3; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Clock_CaptureEdgeAndMedian3(void)

{
  int *piVar1;
  undefined *puVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  
  piVar1 = DAT_080032e0;
  if (*DAT_080032e0 == 1) {
    iVar6 = System_GetTick();
    uVar11 = DAT_080032f8;
    iVar10 = piVar1[6];
    uVar7 = piVar1[0x15];
    piVar1[6] = iVar6;
    uVar8 = piVar1[0x14];
    piVar1[0x16] = uVar7;
    uVar11 = (uint)((ulonglong)uVar11 * (ulonglong)(uint)(iVar6 - iVar10) >> 0x26);
    piVar1[0x15] = uVar8;
    piVar1[7] = uVar11;
    piVar1[0x14] = uVar11;
    if (uVar8 < uVar11) {
      uVar9 = uVar8;
      if ((uVar8 < uVar7) && (uVar9 = uVar7, uVar11 <= uVar7)) {
        uVar9 = uVar11;
      }
    }
    else {
      uVar9 = uVar11;
      if ((uVar11 < uVar7) && (uVar9 = uVar8, uVar7 <= uVar8)) {
        uVar9 = uVar7;
      }
    }
    piVar1[0x13] = uVar9;
    *(undefined *)(piVar1 + 0x17) = 1;
  }
  *DAT_080032e4 = 0;
  uVar4 = System_GetTick();
  uVar5 = System_GetNow();
  puVar3 = DAT_080032f0;
  puVar2 = DAT_080032ec;
  *DAT_080032e8 = uVar4;
  *puVar3 = uVar5;
  *puVar2 = 1;
  uVar4 = System_GetNow();
  *(undefined4 *)(DAT_080032f4 + 0x224) = uVar4;
  return;
}


