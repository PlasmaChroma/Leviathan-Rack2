/* 08003878 main; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void main(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined uVar4;
  short sVar5;
  undefined4 *puVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  undefined *puVar11;
  char *pcVar12;
  undefined4 *puVar13;
  float *pfVar14;
  undefined4 *puVar15;
  byte bVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  uint uVar19;
  char *pcVar20;
  uint uVar21;
  char cVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  undefined4 uVar26;
  int iVar27;
  undefined4 *puVar28;
  int iVar29;
  int iVar30;
  bool bVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 uStack_30;
  undefined4 local_2c;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  
  uVar17 = DAT_08003b4c;
  uVar18 = DAT_08003b48;
  puVar6 = DAT_08003b44;
  uVar26 = DAT_08003b40;
  puVar28 = DAT_08003b44 + 0x1a5;
  DataBenderHardware_Init(DAT_08003b40);
  uVar9 = DAT_08003b58;
  uVar8 = DAT_08003b54;
  uVar7 = DAT_08003b50;
  *puVar6 = uVar18;
  puVar6[0x20] = uVar7;
  puVar6[0x21] = uVar8;
  uVar8 = DAT_08003b5c;
  puVar6[0x22] = uVar7;
  puVar6[0x23] = uVar9;
  DB_Buffer_Init(uVar18,puVar6 + 0x26,uVar7,uVar8);
  DB_Corrupt_Init(*puVar6,puVar6 + 0xb5);
  puVar6[0xb5] = 1;
  *(undefined *)(puVar6 + 0xb8) = 1;
  DaisySP_Tone_Init(*puVar6,puVar6 + 0x1a2);
  DaisySP_Tone_Init(*puVar6,puVar6 + 0x1a9);
  *puVar28 = uVar17;
  DaisySP_Tone_CalculateCoefficients(puVar6 + 0x1a2);
  puVar6[0x1ac] = uVar17;
  DaisySP_Tone_CalculateCoefficients(puVar6 + 0x1a9);
  InitSingleFloatHalf_unknown(puVar6 + 0x1b0);
  InitSingleFloatHalf_unknown(puVar6 + 0x1b1);
  *(undefined *)(puVar6 + 0x1d) = 0;
  puVar6[0x1c] = 0;
  puVar6[0x1b4] = 0x3f000000;
  iVar25 = DAT_08003b60;
  puVar6[0x1e] = 0;
  *(undefined4 *)(iVar25 + 4) = 0;
  *(undefined *)(puVar6 + 0x1f) = 0;
  *(undefined *)(puVar6 + 0x1b5) = 0;
  uVar17 = System_GetNow();
  *(undefined4 *)(DAT_08003b60 + 0x20) = uVar17;
  uVar17 = System_GetTick();
  iVar25 = DAT_08003b60;
  *(undefined4 *)(DAT_08003b60 + 0xc) = 0;
  *(undefined4 *)(iVar25 + 0x2c) = 0;
  *(undefined4 *)(iVar25 + 0x18) = uVar17;
  *(undefined4 *)(iVar25 + 0x10) = uVar18;
  iVar10 = DAT_08003b64;
  *(undefined4 *)(iVar25 + 8) = 0x3f800000;
  uVar18 = DAT_08003b68;
  *(undefined4 *)(iVar10 + 500) = 0;
  *(undefined4 *)(iVar25 + 0x30) = uVar18;
  *(undefined4 *)(iVar25 + 0x1c) = uVar18;
  *(undefined4 *)(iVar10 + 0x1f0) = 0;
  iVar29 = iVar10 + 0x48;
  *(int *)(iVar10 + 0x14) = iVar25;
  *(undefined4 *)(iVar25 + 0x54) = uVar18;
  *(undefined4 *)(iVar25 + 0x58) = uVar18;
  *(undefined4 *)(iVar25 + 0x4c) = uVar18;
  *(undefined4 *)(iVar25 + 0x50) = uVar18;
  *(undefined4 *)(iVar25 + 0x14) = DAT_08003b6c;
  *(undefined4 *)(iVar10 + 0xc) = uVar26;
  *(undefined4 **)(iVar10 + 0x10) = puVar6;
  *(undefined *)(iVar10 + 0x1f8) = 2;
  uVar18 = System_GetNow();
  *(undefined4 *)(iVar10 + 0x1fc) = uVar18;
  iVar25 = iVar10;
  do {
    uVar7 = DAT_08003b78;
    uVar17 = DAT_08003b74;
    uVar18 = DAT_08003b70;
    iVar24 = iVar25 + 0xc;
    *(undefined4 *)(iVar25 + 0xa4) = DAT_08003b70;
    *(undefined4 *)(iVar25 + 0xa0) = uVar17;
    *(undefined4 *)(iVar25 + 0x9c) = uVar7;
    iVar25 = iVar24;
  } while (iVar24 != iVar29);
  FUN_08009b70(DAT_08003b7c,2);
  FUN_08009b70(DAT_08003b80,1);
  uVar17 = DAT_08003b88;
  *(undefined4 *)(iVar10 + 0x30) = DAT_08003b84;
  uVar7 = DAT_08003b90;
  *(undefined4 *)(iVar10 + 0x34) = DAT_08003b8c;
  *(undefined4 *)(iVar10 + 0x38) = DAT_08003b94;
  FUN_08009b90(0x3f000000,uVar18,uVar17,uVar7);
  FUN_08009b90(0x3f800000,DAT_08003b9c,DAT_08003b98,DAT_08003ba0);
  FUN_08009b70(DAT_08003ba4,3);
  FUN_08009b70(DAT_08003ba8,7);
  uVar17 = System_GetNow();
  iVar25 = *(int *)(iVar10 + 0x10);
  *(undefined4 *)(iVar10 + 0x214) = uVar17;
  iVar24 = *(int *)(iVar10 + 0xc);
  *(undefined4 *)(iVar25 + 0x684) = 1;
  *(undefined4 *)(iVar25 + 0x2d4) = 1;
  uVar17 = DAT_08003bac;
  *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x70) = uVar17;
  uVar17 = DAT_08003bb0;
  *(undefined4 *)(iVar25 + 0x3c) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x40) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x44) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x1a4) = uVar17;
  *(undefined4 *)(iVar25 + 0x120) = uVar18;
  *(undefined4 *)(iVar25 + 0x158) = uVar18;
  *(undefined4 *)(iVar25 + 0x100) = 0;
  *(undefined4 *)(iVar25 + 0x6d0) = uVar18;
  *(undefined4 *)(iVar25 + 0xc) = 0;
  *(undefined2 *)(iVar25 + 0x10) = 0;
  *(undefined4 *)(iVar25 + 4) = 0;
  *(undefined4 *)(iVar25 + 8) = 0;
  *(undefined2 *)(iVar10 + 0x228) = 0;
  *(undefined *)(iVar10 + 0x22a) = 0;
  iVar25 = iVar24 + 0x578;
  do {
    iVar30 = iVar25 + 0x18;
    *(undefined4 *)(iVar25 + 8) = DAT_08003bb4;
    FUN_08000bac(iVar25);
    iVar25 = iVar30;
  } while (iVar24 + 0x620 != iVar30);
  iVar25 = *(int *)(iVar10 + 0xc);
  uVar19 = GPIO_Read(iVar25 + 0xe8);
  if (*(char *)(iVar25 + 0xfd) != '\0') {
    uVar19 = (uVar19 ^ 1) & 0xff;
  }
  if (uVar19 == 0) {
    iVar25 = *(int *)(iVar10 + 0xc);
    uVar19 = GPIO_Read(iVar25 + 0xc4);
    if (*(char *)(iVar25 + 0xd9) != '\0') {
      uVar19 = (uVar19 ^ 1) & 0xff;
    }
    if (uVar19 != 0) {
      *(undefined *)(iVar10 + 0x1f8) = 4;
    }
  }
  else {
    *(undefined *)(iVar10 + 0x1f8) = 3;
  }
  fVar32 = (float)DaisySeed_AudioSampleRate(DAT_08003b40);
  iVar25 = DaisySeed_AudioBlockSize(DAT_08003b40);
  pcVar12 = DAT_08003bcc;
  uVar19 = System_GetTickFreq();
  uVar18 = DAT_08003bc0;
  puVar11 = DAT_08003bbc;
  fVar33 = DAT_08003bb8;
  local_24 = CONCAT22(local_24._2_2_,0x403);
  *DAT_08003bbc = 1;
  *(undefined4 *)(puVar11 + 0xc) = uVar18;
  *(undefined4 *)(puVar11 + 0x10) = uVar18;
  *(undefined4 *)(puVar11 + 0x14) = uVar18;
  local_2c = 0;
  local_34 = 0;
  uStack_30 = 0;
  fVar33 = fVar33 / (fVar32 / (float)(longlong)iVar25);
  *(float *)(puVar11 + 4) = 1.0 / ((float)(ulonglong)uVar19 * ((float)(longlong)iVar25 / fVar32));
  *(float *)(puVar11 + 0x18) = fVar33 / (fVar33 + 1.0);
  local_38 = 0xff0b;
  GPIO_Init(&local_38,local_24,0,1,0);
  bVar16 = GPIO_Read(&local_38);
  puVar13 = DAT_08003bd0;
  iVar25 = 0;
  *DAT_08003bc4 = bVar16 ^ 1;
  *puVar13 = uVar26;
  puVar13[1] = 0xc;
  puVar28 = DAT_08003bc8;
  puVar13[2] = 0x38;
  puVar13[3] = pcVar12;
  puVar13[4] = puVar28;
  pcVar20 = pcVar12;
  while( true ) {
    pcVar20[iVar25] = (&DAT_90009000)[iVar25];
    iVar25 = iVar25 + 1;
    if (iVar25 == 0xc) break;
    pcVar20 = (char *)puVar13[3];
  }
  iVar25 = 0;
  do {
    *(undefined *)(puVar13[4] + iVar25) = *(undefined *)(iVar25 + DAT_08003e9c);
    iVar25 = iVar25 + 1;
  } while (iVar25 != 0x38);
  if (*(char *)puVar28 != '\x04') {
    puVar28[3] = 0x3f800000;
    puVar28[4] = 0x3f800000;
    puVar28[5] = 0x3f800000;
    puVar28[0xc] = 0x3f800000;
    puVar28[1] = 0;
    *(undefined *)(puVar28 + 2) = 0;
    puVar28[9] = 0;
    puVar28[0xd] = 0;
    puVar28[7] = 0;
    puVar28[8] = 0;
    *puVar28 = 4;
    puVar28[10] = 1;
    puVar28[6] = DAT_08003ea0;
    puVar28[0xb] = DAT_08003ea4;
  }
  if (*pcVar12 != '\x05') {
    *pcVar12 = '\x05';
    local_18 = 0;
    local_24 = 0xff0b;
    local_20 = 0;
    uStack_1c = 0;
    GPIO_Init(&local_24,0x403,0,1,0);
    iVar25 = GPIO_Read(&local_24);
    if (iVar25 == 0) {
      *(undefined4 *)(pcVar12 + 4) = DAT_0800458c;
      *(undefined4 *)(pcVar12 + 8) = DAT_08004590;
    }
    else {
      *(undefined4 *)(pcVar12 + 4) = DAT_08004578;
      *(undefined4 *)(pcVar12 + 8) = DAT_0800457c;
    }
  }
  uVar26 = puVar28[10];
  fVar32 = (float)puVar28[0xb];
  puVar6[0x1a1] = uVar26;
  puVar6[0xb5] = uVar26;
  fVar33 = DAT_08003ea8;
  puVar6[3] = puVar28[1];
  puVar3 = DAT_08003eac;
  puVar6[1] = puVar28[7];
  iVar25 = DAT_08003ed4;
  puVar6[0x40] = puVar28[8];
  if (-1 < (int)((uint)(fVar32 < fVar33) << 0x1f)) {
    fVar33 = fVar32;
  }
  uVar18 = puVar28[6];
  *puVar3 = puVar28[9];
  uVar26 = DAT_08003eb0;
  *(undefined *)(iVar10 + 0x229) = *(undefined *)((int)puVar28 + 2);
  uVar26 = FPMaxNum(uVar18,uVar26);
  fVar32 = (float)FPMinNum(uVar26,0x3f800000);
  *(undefined *)(iVar10 + 0x22a) = *(undefined *)((int)puVar28 + 1);
  uVar4 = *(undefined *)((int)puVar28 + 3);
  puVar6[0x1c] = fVar32;
  puVar6[0x69] = fVar32 * fVar32;
  *(undefined *)(iVar10 + 0x228) = uVar4;
  iVar24 = iVar25;
  do {
    *(float *)(iVar24 + 8) = fVar33;
    iVar30 = iVar24 + 0x18;
    FUN_08000bac(iVar24);
    puVar3 = DAT_08003ebc;
    iVar24 = iVar30;
  } while (iVar25 + 0xa8 != iVar30);
  fVar33 = (float)puVar28[0xc] * DAT_08003eb4;
  *DAT_08003eb8 =
       -(float)((uint)(fVar33 != 1.0) * 0x3f800000 + (uint)(fVar33 == 1.0) * (int)fVar33) * 0.5 +
       0.5;
  puVar6[0xf] = puVar28[3];
  puVar6[0x10] = puVar28[4];
  puVar6[0x11] = puVar28[5];
  *puVar3 = *(undefined4 *)(pcVar12 + 8);
  puVar3 = puVar3 + -1;
  *puVar3 = *(undefined4 *)(pcVar12 + 4);
  iVar25 = puVar28[0xd];
  puVar6[2] = iVar25;
  if ((3 < (int)puVar6[0x1a1]) && (iVar25 == 1)) {
    puVar6[0x1a1] = 1;
  }
  FUN_080075a0(DAT_08003ec0);
  DaisySeed_StartAudio_interleaved(DAT_08003ec8,DAT_08003ec4);
switchD_08003d64_caseD_6:
  while( true ) {
    while( true ) {
      while ((*(int *)(iVar10 + 500) - *(int *)(iVar10 + 0x1f0) & 0x1fU) == 0) {
        iVar25 = System_GetNow();
        if (((1 < *(byte *)(iVar10 + 0x1f8) - 3) && (1 < *(byte *)(iVar10 + 0x1f8))) &&
           (*(uint *)(iVar10 + 4) < (uint)(iVar25 - *(int *)(iVar10 + 0x1fc)))) {
          *(int *)(iVar10 + 0x1fc) = iVar25;
          *(undefined *)(iVar10 + 0x1f8) = 0;
        }
        if (2 < (uint)(iVar25 - *(int *)(iVar10 + 0x20c))) {
          iVar24 = *(int *)(iVar10 + 0xc);
          do {
          } while (-1 < (int)((uint)*(byte *)(iVar24 + 0x374) << 0x18));
          iVar27 = 0;
          iVar23 = *(int *)(iVar24 + 0x354);
          iVar30 = *(int *)(iVar24 + 0x358);
          *(int *)(iVar24 + 0x354) = iVar30;
          *(int *)(iVar24 + 0x358) = iVar23;
          do {
            iVar1 = iVar27 * 4;
            iVar2 = iVar27 * 4;
            iVar27 = iVar27 + 1;
            *(undefined2 *)(iVar30 + iVar2 + 3) = *(undefined2 *)(iVar23 + iVar1 + 3);
          } while (iVar27 != 0x10);
          iVar27 = 0;
          do {
            iVar1 = iVar27 * 4;
            iVar2 = iVar27 * 4;
            iVar27 = iVar27 + 1;
            *(undefined2 *)(iVar30 + iVar2 + 0x44) = *(undefined2 *)(iVar23 + iVar1 + 0x44);
          } while (iVar27 != 0x10);
          *(undefined *)(iVar24 + 0x374) = 0xff;
          *(char *)(iVar24 + 0x374) = *(char *)(iVar24 + 0x374) + '\x01';
          if (*(char *)(iVar24 + 0x374) < '\x02') {
            iVar30 = iVar24 + 0x350;
            iVar24 = I2CHandle_TransmitDma
                               (iVar30,*(byte *)(*(char *)(iVar24 + 0x374) + iVar24 + 0x35c) | 0x40,
                                iVar23 + *(char *)(iVar24 + 0x374) * 0x41,0x41,DAT_08004580,iVar30);
            if (iVar24 != 0) {
              uVar26 = FUN_0800804c(iVar30);
              I2CHandle_Init(iVar30,uVar26);
            }
          }
          else {
            *(undefined *)(iVar24 + 0x374) = 0xff;
          }
          *(int *)(iVar10 + 0x20c) = iVar25;
        }
        if (*(char *)(iVar10 + 0x211) != '\0') {
          iVar25 = puVar6[1];
          cVar22 = *(char *)(puVar6 + 3);
          if (iVar25 == 0) {
            if (cVar22 == '\0') {
              cVar22 = *(char *)((int)puVar6 + 0x12);
            }
            *(char *)(puVar28 + 1) = cVar22;
            *(undefined *)((int)puVar28 + 5) = *(undefined *)((int)puVar6 + 0xd);
            *(undefined *)((int)puVar28 + 6) = *(undefined *)((int)puVar6 + 0xe);
            cVar22 = *(char *)((int)puVar6 + 0xf);
            if (cVar22 == '\0') {
              cVar22 = *(char *)((int)puVar6 + 0x13);
            }
          }
          else {
            *(char *)(puVar28 + 1) = cVar22;
            cVar22 = *(char *)((int)puVar6 + 0xd);
            if (iVar25 == 1) {
              if (cVar22 == '\0') {
                cVar22 = *(char *)((int)puVar6 + 0x12);
              }
              *(char *)((int)puVar28 + 5) = cVar22;
              cVar22 = *(char *)((int)puVar6 + 0xe);
              if (cVar22 == '\0') {
                cVar22 = *(char *)((int)puVar6 + 0x13);
              }
            }
            else {
              *(char *)((int)puVar28 + 5) = cVar22;
              cVar22 = *(char *)((int)puVar6 + 0xe);
            }
            *(char *)((int)puVar28 + 6) = cVar22;
            cVar22 = *(char *)((int)puVar6 + 0xf);
          }
          puVar28[7] = iVar25;
          pfVar14 = DAT_08004228;
          *(char *)((int)puVar28 + 7) = cVar22;
          fVar33 = *pfVar14;
          puVar28[6] = puVar6[0x1c];
          puVar28[10] = puVar6[0xb5];
          uVar26 = puVar6[0x40];
          puVar28[0xc] = -fVar33 * 2.0 + 1.0;
          puVar28[8] = uVar26;
          puVar28[9] = *DAT_0800422c;
          *(undefined *)((int)puVar28 + 2) = *(undefined *)(iVar10 + 0x229);
          *(undefined *)((int)puVar28 + 1) = *(undefined *)(iVar10 + 0x22a);
          *(undefined *)((int)puVar28 + 3) = *(undefined *)(iVar10 + 0x228);
          puVar28[0xb] = *DAT_08004230;
          puVar28[3] = puVar6[0xf];
          puVar28[4] = puVar6[0x10];
          puVar28[5] = puVar6[0x11];
          puVar28[0xd] = puVar6[2];
        }
        if (*(char *)(iVar10 + 0x212) != '\0') {
          *(undefined4 *)(pcVar12 + 4) = *(undefined4 *)(iVar10 + 0x238);
          *(undefined4 *)(pcVar12 + 8) = *(undefined4 *)(iVar10 + 0x23c);
          *pcVar12 = '\x05';
          if (*(char *)(iVar10 + 0x213) != '\0') {
            local_24 = 0xff0b;
            local_18 = 0;
            local_20 = 0;
            uStack_1c = 0;
            GPIO_Init(&local_24,0x403,0,1,0);
            iVar25 = GPIO_Read(&local_24);
            if (iVar25 == 0) {
              *(undefined4 *)(pcVar12 + 4) = DAT_0800458c;
              *(undefined4 *)(pcVar12 + 8) = DAT_08004590;
            }
            else {
              *(undefined4 *)(pcVar12 + 4) = DAT_08004578;
              *(undefined4 *)(pcVar12 + 8) = DAT_0800457c;
            }
          }
          FUN_08008a98(*puVar13,&DAT_90009000,&DAT_90009000 + puVar13[1]);
          FUN_08008a90(*puVar13,&DAT_90009000,puVar13[1],puVar13[3]);
          puVar15 = DAT_08004234;
          *(undefined2 *)(iVar10 + 0x212) = 0;
          *puVar15 = *(undefined4 *)(pcVar12 + 8);
          *puVar3 = *(undefined4 *)(pcVar12 + 4);
        }
        iVar25 = System_GetNow();
        if ((2000 < (uint)(iVar25 - puVar13[5])) && (uVar19 = puVar13[2], uVar19 != 0)) {
          uVar21 = 1;
          bVar31 = false;
          pcVar20 = (char *)(puVar13[4] + -1);
          do {
            pcVar20 = pcVar20 + 1;
            if (*(char *)(DAT_08004238 + uVar21 + -1) == *pcVar20) {
              if (uVar19 <= uVar21) goto code_r0x080040ba;
            }
            else {
              bVar31 = true;
              if (uVar19 <= uVar21) goto LAB_080040c2;
            }
            uVar21 = uVar21 + 1;
          } while( true );
        }
      }
      iVar25 = *(int *)(iVar10 + 0x1f0) + 0x1e;
      iVar24 = iVar10 + iVar25 * 8;
      cVar22 = *(char *)(iVar10 + iVar25 * 8);
      fVar32 = *(float *)(iVar24 + 4);
      sVar5 = *(short *)(iVar24 + 2);
      *(uint *)(iVar10 + 0x1f0) = *(int *)(iVar10 + 0x1f0) + 1U & 0x1f;
      fVar33 = DAT_08004598;
      if (cVar22 == '\x04') break;
      if (cVar22 == '\0') {
        iVar25 = *(int *)(iVar10 + 0xc);
        cVar22 = *(char *)(iVar25 + 0x90);
        switch(sVar5) {
        case 0:
          if (cVar22 == -1) {
            uVar26 = FPMaxNum(*(undefined4 *)(iVar25 + 0x1dc),DAT_08004584);
            fVar33 = (float)FPMinNum(uVar26,0x3f800000);
            iVar25 = *(int *)(iVar10 + 0x10);
            *(float *)(iVar25 + 0x70) = fVar33;
            *(float *)(iVar25 + 0x1a4) = fVar33 * fVar33;
          }
          else {
            *(undefined4 *)(*(int *)(iVar10 + 0x14) + 0x38) = *(undefined4 *)(iVar25 + 0x1dc);
          }
          break;
        case 1:
          if (cVar22 == -1) {
            iVar24 = iVar25 + 0x578;
            fVar33 = DAT_08004594;
            if (-1 < (int)((uint)(fVar32 < DAT_08004594) << 0x1f)) {
              fVar33 = fVar32;
            }
            do {
              *(float *)(iVar24 + 8) = fVar33;
              iVar30 = iVar24 + 0x18;
              FUN_08000bac(iVar24);
              iVar24 = iVar30;
            } while (iVar30 != iVar25 + 0x620);
          }
          else {
            *(float *)(*(int *)(iVar10 + 0x10) + 0x4c) = fVar32;
          }
          break;
        case 2:
          if (cVar22 == -1) {
            fVar32 = fVar32 * DAT_08004588;
            fVar33 = DAT_08004584;
            if (fVar32 == 1.0 || fVar32 < 1.0 != NAN(fVar32)) {
              fVar33 = -fVar32 * 0.5 + 0.5;
            }
            *(float *)(*(int *)(iVar10 + 0x10) + 0x6d0) = fVar33;
          }
          else {
            *(float *)(*(int *)(iVar10 + 0x10) + 0x50) = fVar32;
          }
          break;
        case 3:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x3c) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x30) = fVar32;
          }
          break;
        case 4:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x40) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x34) = fVar32;
          }
          break;
        case 5:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x44) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x38) = fVar32;
          }
        }
      }
    }
    if (fVar32 != 0.0) break;
    if (sVar5 == 0) {
      if (*(char *)(iVar10 + 0x1f8) == '\x04') {
        bVar16 = *(byte *)(iVar10 + 0x234);
        if (bVar16 != 0) {
          fVar32 = *(float *)(iVar10 + 0x230);
          fVar33 = *(float *)(iVar10 + 0x22c);
          fVar34 = fVar32 - fVar33;
          bVar16 = *(byte *)(iVar10 + 0x235) & 1;
          if (fVar32 == fVar33 || fVar32 < fVar33 != (NAN(fVar32) || NAN(fVar33))) {
            bVar16 = 0;
          }
          if (fVar34 == 0.25 || fVar34 < 0.25 != NAN(fVar34)) {
            bVar16 = 0;
          }
        }
        fVar33 = *(float *)(iVar10 + 0x23c);
        if (fVar33 == DAT_0800423c || fVar33 < DAT_0800423c != (NAN(fVar33) || NAN(DAT_0800423c))) {
          bVar16 = 0;
        }
        else if (-1 < (int)((uint)(fVar33 < DAT_08004240) << 0x1f)) {
          bVar16 = 0;
        }
        fVar33 = *(float *)(iVar10 + 0x238);
        if (fVar33 == DAT_08004244 || fVar33 < DAT_08004244 != (NAN(fVar33) || NAN(DAT_08004244))) {
          bVar16 = 0;
        }
        else if (-1 < (int)((uint)(fVar33 < DAT_08004248) << 0x1f)) {
          bVar16 = 0;
        }
        *(byte *)(iVar10 + 0x212) = bVar16;
      }
      *(undefined *)(iVar10 + 0x1f8) = 1;
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x1fc) = uVar26;
      iVar25 = iVar10;
      do {
        iVar24 = iVar25 + 0xc;
        *(undefined4 *)(iVar25 + 0xa0) = DAT_08003ecc;
        *(undefined4 *)(iVar25 + 0x9c) = DAT_08003ed0;
        iVar25 = iVar24;
      } while (iVar24 != iVar29);
    }
    else if (((sVar5 == 3) && (*(char *)(*(int *)(iVar10 + 0xc) + 0x90) != -1)) &&
            (*(char *)(iVar10 + 0x228) != '\0')) {
      *(undefined *)(*(int *)(iVar10 + 0x10) + 0x11) = 1;
    }
  }
  iVar25 = *(int *)(iVar10 + 0xc);
  uVar19 = (uint)*(byte *)(iVar25 + 0x90);
  switch(sVar5) {
  case 0:
    *(undefined *)(iVar10 + 0x1f8) = 0;
    uVar26 = System_GetNow();
    *(undefined4 *)(iVar10 + 0x1fc) = uVar26;
    *(undefined *)(iVar10 + 0x211) = 1;
    iVar25 = iVar10;
    do {
      iVar24 = iVar25 + 0xc;
      *(undefined4 *)(iVar25 + 0xa0) = DAT_08004220;
      *(undefined4 *)(iVar25 + 0x9c) = DAT_08004224;
      iVar25 = iVar24;
    } while (iVar24 != iVar29);
    goto switchD_08003d64_caseD_6;
  case 1:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x22a) = *(byte *)(iVar10 + 0x22a) ^ 1;
    }
    else {
      **(uint **)(iVar10 + 0x14) = (uint)(**(uint **)(iVar10 + 0x14) == 0);
    }
    goto switchD_08003d64_caseD_6;
  case 2:
    iVar25 = *(int *)(iVar10 + 0x10);
    if (uVar19 != 0xff) {
      uVar19 = *(byte *)(iVar25 + 4) + 1 & 1;
      *(uint *)(iVar25 + 4) = uVar19;
      if (uVar19 == 0) {
        *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
      }
      goto switchD_08003d64_caseD_6;
    }
    uVar19 = *(byte *)(iVar25 + 8) + 1 & 1;
    *(uint *)(iVar25 + 8) = uVar19;
    if ((*(int *)(iVar25 + 0x684) < 4) || (uVar19 == 0)) goto switchD_08003d64_caseD_6;
LAB_08003e44:
    iVar24 = 1;
    break;
  case 3:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x228) = *(byte *)(iVar10 + 0x228) ^ 1;
    }
    else if (*(byte *)(iVar10 + 0x228) == 0) {
      iVar25 = *(int *)(iVar10 + 0x10);
      bVar16 = 0;
      if (*(char *)(iVar25 + 0x11) == '\0') {
        bVar16 = *(byte *)(iVar25 + 0x15) ^ 1;
      }
      *(byte *)(iVar25 + 0x11) = bVar16;
    }
    else {
      *(undefined *)(*(int *)(iVar10 + 0x10) + 0x11) = 0;
    }
    goto switchD_08003d64_caseD_6;
  case 4:
    if (*(char *)(iVar10 + 0x1f8) == '\x04') {
      if (*(char *)(iVar10 + 0x234) == '\0') {
        *(undefined4 *)(iVar10 + 0x22c) = *(undefined4 *)(iVar25 + 0x2fc);
        *(undefined *)(iVar10 + 0x234) = 1;
      }
      else if (*(char *)(iVar10 + 0x235) == '\0') {
        fVar34 = *(float *)(iVar25 + 0x2fc);
        *(float *)(iVar10 + 0x230) = fVar34;
        fVar32 = DAT_08004594;
        *(undefined *)(iVar10 + 0x235) = 1;
        fVar33 = fVar33 / (fVar34 - *(float *)(iVar10 + 0x22c));
        *(float *)(iVar10 + 0x23c) = fVar33;
        *(float *)(iVar10 + 0x238) = fVar32 + -*(float *)(iVar10 + 0x22c) * fVar33;
      }
    }
    iVar25 = *(int *)(iVar10 + 0x10);
    if (uVar19 == 0xff) {
      if (*(int *)(iVar25 + 0x100) == 0) {
        *(undefined4 *)(iVar25 + 0x100) = 1;
      }
      else {
        *(undefined4 *)(iVar25 + 0x100) = 0;
      }
    }
    else if (*(int *)(iVar25 + 4) == 0) {
      bVar16 = 0;
      if (*(char *)(iVar25 + 0xc) == '\0') {
        bVar16 = *(byte *)(iVar25 + 0x12) ^ 1;
      }
      *(byte *)(iVar25 + 0xc) = bVar16;
    }
    else if (*(char *)(iVar25 + 0xd) == '\0') {
      *(byte *)(iVar25 + 0xd) = *(byte *)(iVar25 + 0x12) ^ 1;
    }
    else {
      *(undefined *)(iVar25 + 0xd) = 0;
    }
    goto switchD_08003d64_caseD_6;
  case 5:
    if (*(char *)(iVar10 + 0x1f8) == '\x04') {
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x218) = uVar26;
      *(undefined4 *)(iVar10 + 0x23c) = 0x3f800000;
      *(undefined *)(iVar10 + 0x213) = 1;
      *(undefined2 *)(iVar10 + 0x234) = 0x101;
      *(undefined4 *)(iVar10 + 0x238) = 0;
    }
    else if (uVar19 == 0xff) {
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x214) = uVar26;
      iVar24 = *(int *)(iVar10 + 0xc);
      iVar25 = *(int *)(iVar10 + 0x10);
      *(undefined4 *)(iVar25 + 0x684) = 1;
      *(undefined4 *)(iVar25 + 0x2d4) = 1;
      uVar26 = DAT_080045a0;
      *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x70) = uVar26;
      uVar26 = DAT_080045a4;
      *(undefined4 *)(iVar25 + 0x3c) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x40) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x44) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x1a4) = uVar26;
      *(undefined4 *)(iVar25 + 0x100) = 0;
      *(undefined4 *)(iVar25 + 0x120) = 0;
      *(undefined4 *)(iVar25 + 0x158) = 0;
      *(undefined4 *)(iVar25 + 0x6d0) = 0;
      *(undefined4 *)(iVar25 + 0xc) = 0;
      *(undefined2 *)(iVar25 + 0x10) = 0;
      *(undefined4 *)(iVar25 + 4) = 0;
      *(undefined4 *)(iVar25 + 8) = 0;
      *(undefined2 *)(iVar10 + 0x228) = 0;
      *(undefined *)(iVar10 + 0x22a) = 0;
      iVar25 = iVar24 + 0x578;
      do {
        iVar30 = iVar25 + 0x18;
        *(undefined4 *)(iVar25 + 8) = DAT_0800459c;
        FUN_08000bac(iVar25);
        iVar25 = iVar30;
      } while (iVar24 + 0x620 != iVar30);
    }
    else {
      iVar25 = *(int *)(iVar10 + 0x10);
      if (*(int *)(iVar25 + 4) == 0) {
        bVar16 = 0;
        if (*(char *)(iVar25 + 0xf) == '\0') {
          bVar16 = *(byte *)(iVar25 + 0x13) ^ 1;
        }
        *(byte *)(iVar25 + 0xf) = bVar16;
      }
      else if (*(char *)(iVar25 + 0xe) == '\0') {
        *(byte *)(iVar25 + 0xe) = *(byte *)(iVar25 + 0x13) ^ 1;
      }
      else {
        *(undefined *)(iVar25 + 0xe) = 0;
      }
    }
    goto switchD_08003d64_caseD_6;
  case 6:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x229) = *(byte *)(iVar10 + 0x229) ^ 1;
      goto switchD_08003d64_caseD_6;
    }
    iVar25 = *(int *)(iVar10 + 0x10);
    bVar31 = *(int *)(iVar25 + 8) == 1;
    if (bVar31) {
      uVar19 = 4;
    }
    iVar24 = *(int *)(iVar25 + 0x684) + 1;
    if (!bVar31) {
      uVar19 = 6;
    }
    iVar24 = iVar24 - uVar19 * (iVar24 / (int)uVar19);
    if (iVar24 == 0) goto LAB_08003e44;
    break;
  default:
    goto switchD_08003d64_caseD_6;
  }
  *(int *)(iVar25 + 0x684) = iVar24;
  goto switchD_08003d64_caseD_6;
code_r0x080040ba:
  if (bVar31) {
LAB_080040c2:
    FUN_08008a98(*puVar13,DAT_08004238,uVar19 + DAT_08004238);
    FUN_08008a90(*puVar13,DAT_08004238,puVar13[2],puVar13[4]);
    uVar26 = System_GetNow();
    puVar13[5] = uVar26;
    *(undefined *)(iVar10 + 0x211) = 0;
  }
  goto switchD_08003d64_caseD_6;
}


