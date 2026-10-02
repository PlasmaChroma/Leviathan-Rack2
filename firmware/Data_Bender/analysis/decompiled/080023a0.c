/* 080023a0 DB_Controls_Poll; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Controls_Poll(int param_1)

{
  char cVar1;
  ulonglong uVar2;
  longlong lVar3;
  int iVar4;
  undefined uVar5;
  byte bVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined uVar9;
  int iVar10;
  int *piVar11;
  uint uVar12;
  float *pfVar13;
  undefined4 uVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  float *pfVar18;
  int iVar19;
  float *pfVar20;
  undefined4 *puVar21;
  bool bVar22;
  undefined4 uVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  float fVar28;
  float local_9c;
  float local_98;
  float local_94;
  int local_90 [4];
  float local_80;
  float local_7c;
  float local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined auStack_68 [12];
  undefined auStack_5c [12];
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined auStack_44 [12];
  undefined auStack_38 [12];
  float local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  uVar7 = System_GetNow();
  iVar19 = *(int *)(param_1 + 0xc);
  iVar15 = iVar19 + 0x1d0;
  do {
    iVar16 = iVar15 + 0x20;
    AnalogControl_Process(iVar15);
    iVar15 = iVar16;
  } while (iVar19 + 0x290 != iVar16);
  iVar15 = iVar19 + 0x290;
  do {
    iVar16 = iVar15 + 0x20;
    AnalogControl_Process(iVar15);
    iVar15 = iVar16;
  } while (iVar19 + 0x350 != iVar16);
  iVar19 = 0x74;
  iVar15 = 0;
  do {
    while( true ) {
      iVar16 = *(int *)(param_1 + 0xc);
      Switch_Debounce(iVar16 + iVar19);
      iVar16 = iVar16 + iVar15 * 0x24;
      if (*(char *)(iVar16 + 0x78) != '\0') break;
LAB_0800240a:
      iVar15 = iVar15 + 1;
      iVar19 = iVar19 + 0x24;
      if (iVar15 == 7) goto LAB_08002480;
    }
    cVar1 = *(char *)(iVar16 + 0x90);
    if (cVar1 == '\x7f') {
      if (iVar15 == 0) {
        *(uint *)(param_1 + 0x200) = uVar7;
      }
      iVar16 = *(int *)(param_1 + 500);
      iVar10 = param_1 + 0xf0 + iVar16 * 8;
      *(undefined *)(param_1 + 0xf0 + iVar16 * 8) = 4;
      *(short *)(iVar10 + 2) = (short)iVar15;
      *(undefined4 *)(iVar10 + 4) = 0;
      *(uint *)(param_1 + 500) = iVar16 + 1U & 0x1f;
      uVar8 = System_GetNow();
      *(undefined4 *)(param_1 + 0xec) = uVar8;
      goto LAB_0800240a;
    }
    if (cVar1 != -0x80) goto LAB_0800240a;
    if (iVar15 == 0) {
      *(uint *)(param_1 + 0x200) = uVar7;
    }
    iVar10 = *(int *)(param_1 + 500);
    iVar19 = iVar19 + 0x24;
    *(undefined *)(param_1 + 0xf0 + iVar10 * 8) = 4;
    iVar16 = param_1 + 0xf0 + iVar10 * 8;
    *(short *)(iVar16 + 2) = (short)iVar15;
    iVar15 = iVar15 + 1;
    *(undefined4 *)(iVar16 + 4) = 0x3f800000;
    *(uint *)(param_1 + 500) = iVar10 + 1U & 0x1f;
    uVar8 = System_GetNow();
    *(undefined4 *)(param_1 + 0xec) = uVar8;
  } while (iVar15 != 7);
LAB_08002480:
  iVar19 = *(int *)(param_1 + 0xc);
  pfVar20 = (float *)(param_1 + 0x84);
  iVar16 = 0;
  iVar15 = param_1;
  do {
    iVar4 = DAT_08002664;
    iVar10 = DAT_08002660;
    fVar24 = *(float *)(iVar19 + iVar16 * 0x20 + 0x1dc);
    if (iVar16 != 3) {
      fVar24 = fVar24 * DAT_0800265c;
    }
    fVar27 = *(float *)(iVar15 + 0xa0) * *(float *)(iVar15 + 0xa4) +
             fVar24 * *(float *)(iVar15 + 0x9c);
    fVar25 = fVar24 - fVar27;
    *(float *)(iVar15 + 0xa4) = fVar27;
    *pfVar20 = fVar25;
    pfVar20 = pfVar20 + 1;
    fVar25 = ABS(fVar25);
    bVar22 = *(char *)(iVar19 + iVar16 * 0x24 + 0x90) == -1;
    fVar27 = (float)((uint)bVar22 * iVar10 + (uint)!bVar22 * iVar4);
    if ((fVar25 != fVar27 && fVar25 < fVar27 == (NAN(fVar25) || NAN(fVar27))) &&
       (200 < uVar7 - *(int *)(param_1 + 0x200))) {
      iVar10 = *(int *)(param_1 + 500);
      *(undefined *)(param_1 + 0xf0 + iVar10 * 8) = 0;
      iVar19 = param_1 + 0xf0 + iVar10 * 8;
      *(short *)(iVar19 + 2) = (short)iVar16;
      *(float *)(iVar19 + 4) = fVar24;
      *(uint *)(param_1 + 500) = iVar10 + 1U & 0x1f;
      uVar8 = System_GetNow();
      *(undefined4 *)(param_1 + 0xec) = uVar8;
      iVar19 = *(int *)(param_1 + 0xc);
    }
    iVar16 = iVar16 + 1;
    iVar15 = iVar15 + 0xc;
  } while (iVar16 != 6);
  iVar15 = *(int *)(param_1 + 0x10);
  *(undefined4 *)(iVar15 + 100) = *(undefined4 *)(iVar19 + 700);
  *(undefined4 *)(iVar15 + 0x68) = *(undefined4 *)(iVar19 + 0x2dc);
  *(undefined4 *)(iVar15 + 0x54) = *(undefined4 *)(iVar19 + 0x2fc);
  *(undefined4 *)(iVar15 + 0x58) = *(undefined4 *)(iVar19 + 0x31c);
  *(undefined4 *)(iVar15 + 0x5c) = *(undefined4 *)(iVar19 + 0x33c);
  *(undefined4 *)(*(int *)(param_1 + 0x14) + 0x3c) = *(undefined4 *)(iVar19 + 0x29c);
  iVar15 = GateIn_Trig();
  if (iVar15 == 0) {
    iVar19 = *(int *)(param_1 + 0x10);
  }
  else if (*(char *)(param_1 + 0x229) == '\0') {
    iVar19 = *(int *)(param_1 + 0x10);
    iVar15 = *(int *)(iVar19 + 0x684) + 1;
    if (*(int *)(iVar19 + 8) == 1) {
      iVar16 = 4;
    }
    else {
      iVar16 = 6;
    }
    iVar15 = iVar15 - iVar16 * (iVar15 / iVar16);
    if (iVar15 == 0) {
      iVar15 = 1;
    }
    *(int *)(iVar19 + 0x684) = iVar15;
  }
  else {
    piVar11 = *(int **)(param_1 + 0x14);
    if (*piVar11 == 1) {
      iVar19 = *(int *)(param_1 + 0x10);
      fVar24 = (float)FPMinNum(0x3f800000,piVar11[0x12]);
      piVar11[0xb] = (uint)(0.0 < 1.0 / fVar24) * (int)(1.0 / fVar24);
    }
    else {
      iVar19 = *(int *)(param_1 + 0x10);
      piVar11[3] = 0;
    }
  }
  iVar15 = *(int *)(param_1 + 0xc);
  if (*(char *)(param_1 + 0x22a) == '\0') {
    *(undefined2 *)(iVar19 + 0x12) = 0;
    *(undefined2 *)(iVar19 + 0x14) = 0;
    iVar15 = GateIn_Trig(iVar15 + 0x188);
    if (iVar15 == 0) {
LAB_0800261c:
      iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x1a0);
    }
    else {
      iVar15 = *(int *)(param_1 + 0x10);
      if (*(int *)(iVar15 + 4) == 0) {
        if (*(char *)(iVar15 + 0xc) == '\0') {
          *(byte *)(iVar15 + 0xc) = *(byte *)(iVar15 + 0x12) ^ 1;
        }
        else {
          *(undefined *)(iVar15 + 0xc) = 0;
        }
        goto LAB_0800261c;
      }
      if (*(char *)(iVar15 + 0xd) == '\0') {
        *(byte *)(iVar15 + 0xd) = *(byte *)(iVar15 + 0x12) ^ 1;
      }
      else {
        *(undefined *)(iVar15 + 0xd) = 0;
      }
      iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x1a0);
    }
    if (iVar15 != 0) {
      iVar15 = *(int *)(param_1 + 0x10);
      if (*(int *)(iVar15 + 4) == 0) {
        bVar6 = 0;
        if (*(char *)(iVar15 + 0xf) == '\0') {
          bVar6 = *(byte *)(iVar15 + 0x13) ^ 1;
        }
        *(byte *)(iVar15 + 0xf) = bVar6;
      }
      else if (*(char *)(iVar15 + 0xe) == '\0') {
        *(byte *)(iVar15 + 0xe) = *(byte *)(iVar15 + 0x13) ^ 1;
      }
      else {
        *(undefined *)(iVar15 + 0xe) = 0;
      }
    }
    iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x170);
    iVar19 = *(int *)(param_1 + 0x10);
    if (iVar15 != 0) {
      if (*(char *)(iVar19 + 0x11) == '\0') {
        *(byte *)(iVar19 + 0x11) = *(byte *)(iVar19 + 0x15) ^ 1;
      }
      else {
        *(undefined *)(iVar19 + 0x11) = 0;
      }
    }
  }
  else {
    if (*(char *)(iVar15 + 0x19e) == '\0') {
      uVar5 = GPIO_Read(iVar15 + 0x188);
      iVar10 = *(int *)(param_1 + 0xc);
      iVar16 = *(int *)(param_1 + 0x10);
      *(undefined *)(iVar19 + 0x12) = uVar5;
      iVar15 = iVar10 + 0x170;
      if (*(char *)(iVar10 + 0x186) == '\0') goto LAB_080028fc;
LAB_08002562:
      bVar6 = GPIO_Read(iVar15);
      iVar15 = *(int *)(param_1 + 0xc);
      iVar10 = *(int *)(param_1 + 0x10);
      cVar1 = *(char *)(iVar15 + 0x1b6);
      *(byte *)(iVar16 + 0x15) = bVar6 ^ 1;
      iVar15 = iVar15 + 0x1a0;
      if (cVar1 == '\0') goto LAB_08002914;
LAB_08002580:
      bVar6 = GPIO_Read(iVar15);
      bVar6 = bVar6 ^ 1;
    }
    else {
      bVar6 = GPIO_Read(iVar15 + 0x188);
      iVar15 = *(int *)(param_1 + 0xc);
      iVar16 = *(int *)(param_1 + 0x10);
      cVar1 = *(char *)(iVar15 + 0x186);
      *(byte *)(iVar19 + 0x12) = bVar6 ^ 1;
      iVar15 = iVar15 + 0x170;
      if (cVar1 != '\0') goto LAB_08002562;
LAB_080028fc:
      uVar5 = GPIO_Read(iVar15);
      iVar19 = *(int *)(param_1 + 0xc);
      iVar10 = *(int *)(param_1 + 0x10);
      *(undefined *)(iVar16 + 0x15) = uVar5;
      iVar15 = iVar19 + 0x1a0;
      if (*(char *)(iVar19 + 0x1b6) != '\0') goto LAB_08002580;
LAB_08002914:
      bVar6 = GPIO_Read(iVar15);
    }
    iVar19 = *(int *)(param_1 + 0x10);
    *(byte *)(iVar10 + 0x13) = bVar6;
  }
  iVar15 = *(int *)(iVar19 + 4);
  if (iVar15 != 0) {
    iVar15 = (uint)(0.0 < *(float *)(iVar19 + 0x284)) * (int)*(float *)(iVar19 + 0x284);
  }
  iVar19 = *(int *)(param_1 + 0x21c);
  uVar5 = *(undefined *)(param_1 + 0x1f8);
  *(int *)(param_1 + 0x21c) = iVar15;
  if (iVar19 != iVar15) {
    *(uint *)(param_1 + 0x220) = uVar7;
  }
  fVar24 = DAT_08002ec0;
  switch(uVar5) {
  case 0:
    if (*(char *)(param_1 + 0x210) == '\0') {
      piVar11 = *(int **)(param_1 + 0x14);
      if (*piVar11 == 0) {
        iVar15 = *(int *)(param_1 + 0xc) + 0x590;
        if (-1 < (int)((uint)((float)piVar11[3] / DAT_08002ec4 < 0.5) << 0x1f)) goto LAB_08002eb4;
        puVar21 = (undefined4 *)(param_1 + 0x18);
      }
      else {
        iVar15 = *(int *)(param_1 + 0xc);
        iVar19 = System_GetNow();
        puVar21 = (undefined4 *)(param_1 + 0x54);
        iVar15 = iVar15 + 0x590;
        uVar17 = iVar19 - piVar11[8];
        uVar12 = piVar11[9];
        if (1.0 < (float)piVar11[0x12]) {
          uVar17 = uVar17 - uVar12 * (uVar17 / uVar12);
        }
        if (uVar12 >> 1 <= uVar17) {
LAB_08002eb4:
          puVar21 = (undefined4 *)(param_1 + 0x60);
        }
      }
      uVar8 = puVar21[1];
      *(undefined4 *)(iVar15 + 0xc) = *puVar21;
      uVar23 = puVar21[2];
      *(undefined4 *)(iVar15 + 0x10) = uVar8;
      *(undefined4 *)(iVar15 + 0x14) = uVar23;
      FUN_08000bac(iVar15);
    }
    else {
      local_80 = 0.0;
      local_7c = 0.0;
      local_78 = 0.0;
      FUN_08009b90(DAT_08002ecc,DAT_08002ecc,&local_80);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x5a0) = local_7c;
      *(float *)(iVar15 + 0x59c) = local_80;
      *(float *)(iVar15 + 0x5a4) = local_78;
      FUN_08000bac();
    }
    if (**(int **)(param_1 + 0x14) == 1) {
      fVar24 = (float)(*(int **)(param_1 + 0x14))[0x12];
      if (*(float *)(param_1 + 0xe8) == fVar24) {
        *(float *)(param_1 + 0xe8) = fVar24;
        if (0x4f < uVar7 - *(int *)(param_1 + 0xe4)) goto LAB_08002bec;
      }
      else {
        *(uint *)(param_1 + 0xe4) = uVar7;
        *(float *)(param_1 + 0xe8) = fVar24;
      }
      iVar15 = *(int *)(param_1 + 0xc);
      uVar8 = *(undefined4 *)(param_1 + 0x30);
      *(undefined4 *)(iVar15 + 0x5a0) = *(undefined4 *)(param_1 + 0x34);
      uVar23 = *(undefined4 *)(param_1 + 0x38);
      *(undefined4 *)(iVar15 + 0x59c) = uVar8;
      *(undefined4 *)(iVar15 + 0x5a4) = uVar23;
      FUN_08000bac();
    }
LAB_08002bec:
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(param_1 + 0x60);
    *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 100);
    uVar23 = *(undefined4 *)(param_1 + 0x68);
    *(undefined4 *)(iVar15 + 0x584) = uVar8;
    *(undefined4 *)(iVar15 + 0x58c) = uVar23;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0x10);
    if (*(int *)(iVar15 + 4) == 0) {
      iVar19 = *(int *)(param_1 + 0xc) + 0x5c0;
      if ((*(char *)(iVar15 + 0xc) == '\0') && (*(char *)(iVar15 + 0x12) == '\0')) {
        puVar21 = (undefined4 *)(param_1 + 0x60);
      }
      else {
        puVar21 = (undefined4 *)(param_1 + 0x18);
      }
    }
    else {
      if ((*(char *)(iVar15 + 0xd) == '\0') && (*(char *)(iVar15 + 0x12) == '\0')) {
        if (*(char *)(iVar15 + 0x75) == '\0') {
          iVar15 = *(int *)(param_1 + 0xc);
          *(undefined4 *)(iVar15 + 0x5cc) = 0;
          *(undefined4 *)(iVar15 + 0x5d0) = 0;
          *(undefined4 *)(iVar15 + 0x5d4) = 0x3f800000;
          FUN_08000bac();
        }
        else {
          iVar15 = *(int *)(param_1 + 0xc);
          *(undefined4 *)(iVar15 + 0x5cc) = 0;
          uVar8 = DAT_08002ebc;
          *(undefined4 *)(iVar15 + 0x5d4) = 0x3f800000;
          *(undefined4 *)(iVar15 + 0x5d0) = uVar8;
          FUN_08000bac();
        }
        goto LAB_08002ef2;
      }
      iVar19 = *(int *)(param_1 + 0xc) + 0x5c0;
      if (*(char *)(iVar15 + 0x75) == '\0') {
        puVar21 = (undefined4 *)(param_1 + 0x24);
      }
      else {
        puVar21 = (undefined4 *)(param_1 + 0x30);
      }
    }
    *(undefined4 *)(iVar19 + 0xc) = *puVar21;
    uVar8 = puVar21[2];
    *(undefined4 *)(iVar19 + 0x10) = puVar21[1];
    *(undefined4 *)(iVar19 + 0x14) = uVar8;
    FUN_08000bac();
LAB_08002ef2:
    iVar15 = *(int *)(param_1 + 0xc);
    iVar19 = *(int *)(param_1 + 0x10);
    if (*(int *)(iVar19 + 4) == 0) {
      cVar1 = *(char *)(iVar19 + 0xf);
    }
    else {
      cVar1 = *(char *)(iVar19 + 0xe);
    }
    if ((cVar1 == '\0') && (*(char *)(iVar19 + 0x13) == '\0')) {
      puVar21 = (undefined4 *)(param_1 + 0x60);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    *(undefined4 *)(iVar15 + 0x5e4) = *puVar21;
    uVar8 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x5e8) = puVar21[1];
    *(undefined4 *)(iVar15 + 0x5ec) = uVar8;
    FUN_08000bac();
    if (uVar7 - *(int *)(param_1 + 0x220) < 0x50) {
      iVar15 = *(int *)(param_1 + 0xc);
      uVar8 = *(undefined4 *)(param_1 + 0x34);
      uVar23 = *(undefined4 *)(param_1 + 0x38);
      *(undefined4 *)(iVar15 + 0x5e4) = *(undefined4 *)(param_1 + 0x30);
      *(undefined4 *)(iVar15 + 0x5e8) = uVar8;
      *(undefined4 *)(iVar15 + 0x5ec) = uVar23;
      FUN_08000bac();
    }
    iVar15 = *(int *)(param_1 + 0xc);
    if ((*(char *)(*(int *)(param_1 + 0x10) + 0x11) == '\0') &&
       (*(char *)(*(int *)(param_1 + 0x10) + 0x15) == '\0')) {
      puVar21 = (undefined4 *)(param_1 + 0x60);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    uVar8 = puVar21[1];
    uVar23 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x614) = *puVar21;
    *(undefined4 *)(iVar15 + 0x618) = uVar8;
    *(undefined4 *)(iVar15 + 0x61c) = uVar23;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0xc);
    if (*(int *)(*(int *)(param_1 + 0x10) + 4) == 0) {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x24);
    }
    uVar8 = puVar21[1];
    uVar23 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x5b4) = *puVar21;
    *(undefined4 *)(iVar15 + 0x5bc) = uVar23;
    *(undefined4 *)(iVar15 + 0x5b8) = uVar8;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0xc);
    switch(*(undefined4 *)(*(int *)(param_1 + 0x10) + 0x2d4)) {
    case 1:
      uVar8 = *(undefined4 *)(param_1 + 0x18);
      uVar23 = *(undefined4 *)(param_1 + 0x1c);
      uVar14 = *(undefined4 *)(param_1 + 0x20);
      break;
    case 2:
      uVar8 = *(undefined4 *)(param_1 + 0x24);
      uVar23 = *(undefined4 *)(param_1 + 0x28);
      uVar14 = *(undefined4 *)(param_1 + 0x2c);
      break;
    case 3:
      uVar8 = *(undefined4 *)(param_1 + 0x30);
      uVar23 = *(undefined4 *)(param_1 + 0x34);
      uVar14 = *(undefined4 *)(param_1 + 0x38);
      break;
    case 4:
      uVar8 = *(undefined4 *)(param_1 + 0x3c);
      uVar23 = *(undefined4 *)(param_1 + 0x40);
      uVar14 = *(undefined4 *)(param_1 + 0x44);
      break;
    case 5:
      uVar8 = *(undefined4 *)(param_1 + 0x48);
      uVar23 = *(undefined4 *)(param_1 + 0x4c);
      uVar14 = *(undefined4 *)(param_1 + 0x50);
      break;
    default:
      uVar8 = *(undefined4 *)(param_1 + 0x60);
      uVar23 = *(undefined4 *)(param_1 + 100);
      uVar14 = *(undefined4 *)(param_1 + 0x68);
    }
    *(undefined4 *)(iVar15 + 0x5fc) = uVar8;
    *(undefined4 *)(iVar15 + 0x600) = uVar23;
    *(undefined4 *)(iVar15 + 0x604) = uVar14;
    FUN_08000bac();
    return;
  case 1:
    uVar17 = System_GetNow();
    fVar24 = *(float *)(*(int *)(param_1 + 0x10) + 0x70);
    local_80 = 0.0;
    local_7c = 0.0;
    local_78 = 0.0;
    fVar25 = 1.0 - ABS(((float)(longlong)(int)(uVar17 & 0x1ff) / DAT_08002668) * 2.0 + -1.0);
    if (fVar24 == 0.0) {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x584) = *(undefined4 *)(param_1 + 0x60);
      *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 100);
      *(undefined4 *)(iVar15 + 0x58c) = *(undefined4 *)(param_1 + 0x68);
      FUN_08000bac();
    }
    else if (((int)((uint)(fVar24 < DAT_0800266c) << 0x1f) < 0) &&
            (fVar24 != DAT_08002670 && fVar24 < DAT_08002670 == (NAN(fVar24) || NAN(DAT_08002670))))
    {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x584) = *(undefined4 *)(param_1 + 0x18);
      *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 0x1c);
      *(undefined4 *)(iVar15 + 0x58c) = *(undefined4 *)(param_1 + 0x20);
      FUN_08000bac();
    }
    else {
      FUN_08009b90(fVar24,fVar24,&local_80);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x584) = local_80;
      *(float *)(iVar15 + 0x588) = local_7c;
      *(float *)(iVar15 + 0x58c) = local_78;
      FUN_08000bac();
    }
    uVar9 = uVar5;
    if (*(char *)(param_1 + 0x228) == '\0') {
      uVar9 = 2;
    }
    FUN_08009b70(&local_80,uVar9);
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x614) = local_80 * fVar25;
    *(float *)(iVar15 + 0x618) = fVar25 * local_7c;
    *(float *)(iVar15 + 0x61c) = fVar25 * local_78;
    FUN_08000bac();
    if (*(int *)(*(int *)(param_1 + 0x10) + 0x100) == 0) {
      uVar5 = 2;
    }
    FUN_08009b70(&local_80,uVar5);
    iVar15 = *(int *)(param_1 + 0xc);
    fVar24 = (*(float *)(*(int *)(param_1 + 0x10) + 0x3c) + DAT_08002674) * fVar25;
    *(float *)(iVar15 + 0x5d0) = fVar24 * local_7c;
    *(float *)(iVar15 + 0x5d4) = fVar24 * local_78;
    *(float *)(iVar15 + 0x5cc) = local_80 * fVar24;
    FUN_08000bac();
    if (*(int *)(*(int *)(param_1 + 0x10) + 8) == 0) {
      fVar27 = *(float *)(param_1 + 0x1c);
      fVar28 = *(float *)(param_1 + 0x20);
      fVar26 = *(float *)(param_1 + 0x18);
    }
    else {
      fVar27 = *(float *)(param_1 + 0x28);
      fVar28 = *(float *)(param_1 + 0x2c);
      fVar26 = *(float *)(param_1 + 0x24);
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5b8) = fVar25 * fVar27;
    *(float *)(iVar15 + 0x5b4) = fVar26 * fVar25;
    *(float *)(iVar15 + 0x5bc) = fVar25 * fVar28;
    FUN_08000bac();
    if (*(char *)(param_1 + 0x229) == '\0') {
      fVar24 = *(float *)(param_1 + 0x20);
      fVar27 = *(float *)(param_1 + 0x18);
      iVar15 = *(int *)(param_1 + 0xc);
      fVar26 = (*(float *)(*(int *)(param_1 + 0x10) + 0x44) + DAT_08002674) * fVar25;
      *(float *)(iVar15 + 0x600) = fVar26 * *(float *)(param_1 + 0x1c);
      *(float *)(iVar15 + 0x604) = fVar26 * fVar24;
      *(float *)(iVar15 + 0x5fc) = fVar27 * fVar26;
      FUN_08000bac();
    }
    else {
      fVar26 = *(float *)(param_1 + 0x2c);
      fVar27 = *(float *)(param_1 + 0x24);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x600) = fVar24 * *(float *)(param_1 + 0x28);
      *(float *)(iVar15 + 0x5fc) = fVar27 * fVar24;
      *(float *)(iVar15 + 0x604) = fVar24 * fVar26;
      FUN_08000bac();
    }
    uVar7 = uVar7 - *(int *)(param_1 + 0x214);
    if (uVar7 < *(uint *)(param_1 + 8)) {
      iVar15 = *(int *)(param_1 + 0xc);
      uVar2 = (ulonglong)DAT_08002b54;
      *(undefined4 *)(iVar15 + 0x5e4) = 0;
      *(undefined4 *)(iVar15 + 0x5e8) = 0;
      *(float *)(iVar15 + 0x5ec) =
           (float)(ulonglong)(0x58 < uVar7 + (uint)(uVar2 * uVar7 >> 0x26) * -0xa7);
      FUN_08000bac(iVar15 + 0x5d8,(int)(uVar2 * uVar7));
    }
    else {
      iVar15 = *(int *)(param_1 + 0xc);
      fVar24 = (*(float *)(*(int *)(param_1 + 0x10) + 0x40) + DAT_08002ec8) * fVar25;
      *(float *)(iVar15 + 0x5e4) = fVar24;
      *(float *)(iVar15 + 0x5e8) = fVar24;
      *(float *)(iVar15 + 0x5ec) = fVar24;
      FUN_08000bac();
    }
    if (*(char *)(param_1 + 0x22a) == '\0') {
      fVar24 = *(float *)(param_1 + 0x1c);
      fVar26 = *(float *)(param_1 + 0x20);
      fVar27 = *(float *)(param_1 + 0x18);
    }
    else {
      fVar24 = *(float *)(param_1 + 0x28);
      fVar26 = *(float *)(param_1 + 0x2c);
      fVar27 = *(float *)(param_1 + 0x24);
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5a0) = fVar25 * fVar24;
    *(float *)(iVar15 + 0x59c) = fVar27 * fVar25;
    *(float *)(iVar15 + 0x5a4) = fVar25 * fVar26;
    FUN_08000bac();
    break;
  case 2:
    local_90[0] = *DAT_08002b58;
    local_90[1] = DAT_08002b58[1];
    local_90[2] = DAT_08002b58[2];
    local_90[3] = DAT_08002b58[3];
    FUN_08009b90(0x3f000000,DAT_08002b60,DAT_08002b5c,param_1 + 0x3c);
    FUN_08009b90(0x3f800000,DAT_08002b68,DAT_08002b64,param_1 + 0x48);
    fVar24 = DAT_08002b60;
    pfVar20 = &local_80;
    do {
      pfVar18 = pfVar20 + 3;
      *pfVar20 = DAT_08002b60;
      pfVar20[1] = DAT_08002b60;
      pfVar20[2] = DAT_08002b60;
      pfVar20 = pfVar18;
    } while ((float *)&stack0xffffffe0 != pfVar18);
    FUN_08009b70(&local_80,7);
    FUN_08009b70(&local_74,2);
    FUN_08009b70(auStack_68,1);
    FUN_08009b70(auStack_5c,0);
    local_4c = DAT_08002b6c;
    local_48 = DAT_08002b70;
    local_50 = DAT_08002b74;
    FUN_08009b90(0x3f000000,fVar24,DAT_08002b5c,auStack_44);
    FUN_08009b70(auStack_38,3);
    FUN_08009b90(0x3f800000,DAT_08002b68,DAT_08002b64,&local_2c);
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x584) = local_74;
    *(undefined4 *)(iVar15 + 0x588) = local_70;
    *(undefined4 *)(iVar15 + 0x58c) = local_6c;
    FUN_08000bac(iVar15 + 0x578);
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x59c) = local_50;
    *(undefined4 *)(iVar15 + 0x5a0) = local_4c;
    *(undefined4 *)(iVar15 + 0x5a4) = local_48;
    FUN_08000bac(iVar15 + 0x590);
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5b4) = local_2c;
    *(undefined4 *)(iVar15 + 0x5b8) = local_28;
    *(undefined4 *)(iVar15 + 0x5bc) = local_24;
    FUN_08000bac(iVar15 + 0x5a8);
    uVar7 = System_GetNow();
    uVar7 = uVar7 & 0x3ff;
    uVar17 = uVar7 + 0x3fc;
    piVar11 = local_90;
    do {
      iVar15 = (int)((ulonglong)DAT_08002b78 * (ulonglong)uVar7 >> 0x20);
      local_9c = DAT_08002b60;
      local_98 = DAT_08002b60;
      local_94 = DAT_08002b60;
      iVar15 = uVar7 + (iVar15 + (uVar7 - iVar15 >> 1) >> 9) * -0x3ff;
      uVar7 = uVar7 + 0xff;
      FUN_08009b90(DAT_08002b60,
                   1.0 - (1.0 - ABS(((float)(longlong)iVar15 / DAT_08002b7c) * 2.0 + -1.0)),
                   &local_9c);
      iVar15 = *(int *)(param_1 + 0xc) + *piVar11 * 0x18;
      *(float *)(iVar15 + 0x584) = local_9c;
      *(float *)(iVar15 + 0x588) = local_98;
      *(float *)(iVar15 + 0x58c) = local_94;
      FUN_08000bac(iVar15 + 0x578);
      piVar11 = piVar11 + 1;
    } while (uVar17 != uVar7);
    return;
  case 3:
    pfVar18 = &local_80;
    pfVar20 = pfVar18;
    do {
      pfVar13 = pfVar20 + 3;
      *pfVar20 = DAT_08002ec0;
      pfVar20[1] = DAT_08002ec0;
      pfVar20[2] = DAT_08002ec0;
      pfVar20 = pfVar13;
    } while (&local_2c != pfVar13);
    iVar15 = System_GetNow();
    iVar19 = *(int *)(param_1 + 0x224);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x1dc),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x29c),fVar24,pfVar18);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x1fc),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 700),
                 (float)(ulonglong)((uint)(iVar15 - iVar19) < 100),&local_74);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x21c),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x2dc),fVar24,auStack_68);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x23c);
    uVar23 = *(undefined4 *)(iVar15 + 0x2fc);
    if (*(char *)(iVar15 + 0x19e) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,auStack_5c);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x25c);
    uVar23 = *(undefined4 *)(iVar15 + 0x31c);
    if (*(char *)(iVar15 + 0x1b6) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,&local_50);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x27c);
    uVar23 = *(undefined4 *)(iVar15 + 0x33c);
    if (*(char *)(iVar15 + 0x1ce) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,auStack_44);
    iVar15 = *(int *)(param_1 + 0xc) + 0x170;
    if (*(char *)(*(int *)(param_1 + 0xc) + 0x186) == '\0') {
      uVar7 = GPIO_Read(iVar15);
    }
    else {
      uVar7 = GPIO_Read(iVar15);
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(DAT_08002ec0,DAT_08002ec0,(float)(ulonglong)uVar7,auStack_38);
    iVar15 = *(int *)(param_1 + 0xc);
    if (*(char *)(iVar15 + 0x90) == -1) {
      FUN_08009b70(pfVar18,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xb4) == -1) {
      FUN_08009b70(&local_74,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xd8) == -1) {
      FUN_08009b70(auStack_68,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x120) == -1) {
      FUN_08009b70(auStack_5c,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x144) == -1) {
      FUN_08009b70(&local_50,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x168) == -1) {
      FUN_08009b70(auStack_44,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xfc) == -1) {
      FUN_08009b70(auStack_38,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    iVar16 = 0x578;
    iVar19 = 0;
    while( true ) {
      fVar27 = *pfVar18;
      fVar25 = pfVar18[1];
      iVar10 = iVar15 + iVar19 * 0x18;
      fVar24 = pfVar18[2];
      iVar15 = iVar15 + iVar16;
      pfVar18 = pfVar18 + 3;
      iVar16 = iVar16 + 0x18;
      *(float *)(iVar10 + 0x584) = fVar27;
      *(float *)(iVar10 + 0x588) = fVar25;
      *(float *)(iVar10 + 0x58c) = fVar24;
      FUN_08000bac(iVar15);
      if (iVar19 + 1 == 7) break;
      iVar15 = *(int *)(param_1 + 0xc);
      iVar19 = iVar19 + 1;
    }
    return;
  case 4:
    if (*(char *)(param_1 + 0x235) == '\0') {
      iVar15 = *(int *)(param_1 + 0xc);
      if (*(char *)(param_1 + 0x234) == '\0') {
        *(undefined4 *)(iVar15 + 0x5d0) = 0;
        *(undefined4 *)(iVar15 + 0x5cc) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d4) = 0;
        FUN_08000bac();
      }
      else {
        *(undefined4 *)(iVar15 + 0x5cc) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d0) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d4) = 0;
        FUN_08000bac();
      }
    }
    else {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x5cc) = 0;
      *(undefined4 *)(iVar15 + 0x5d0) = 0x3f800000;
      *(undefined4 *)(iVar15 + 0x5d4) = 0;
      FUN_08000bac();
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x5e4) = 0x3f800000;
    *(undefined4 *)(iVar15 + 0x5e8) = 0x3f800000;
    *(undefined4 *)(iVar15 + 0x5ec) = 0x3f800000;
    FUN_08000bac();
    if ((uVar7 - *(int *)(param_1 + 0x218) < *(uint *)(param_1 + 8)) &&
       (*(uint *)(param_1 + 8) < uVar7)) {
      uVar7 = uVar7 - *(int *)(param_1 + 0x214);
      iVar15 = *(int *)(param_1 + 0xc);
      lVar3 = (ulonglong)DAT_08002b54 * (ulonglong)uVar7;
      *(undefined4 *)(iVar15 + 0x5e4) = 0;
      *(undefined4 *)(iVar15 + 0x5e8) = 0;
      *(float *)(iVar15 + 0x5ec) =
           (float)(ulonglong)(0x58 < uVar7 + (uint)((ulonglong)lVar3 >> 0x26) * -0xa7);
      FUN_08000bac(iVar15 + 0x5d8,0xa7,(int)lVar3);
    }
  }
  return;
}


