/* 0800572c DataBenderHardware_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DataBenderHardware_Init(int param_1)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  byte bVar5;
  uint uVar6;
  undefined *puVar7;
  int iVar8;
  undefined *puVar9;
  undefined *puVar10;
  int iVar11;
  char cVar12;
  uint *puVar13;
  undefined4 *puVar14;
  int iVar15;
  uint *puVar16;
  byte *pbVar17;
  int iVar19;
  undefined4 local_4e4;
  uint local_4e0 [4];
  uint local_4d0;
  undefined4 local_4cc;
  undefined4 local_4c8;
  undefined4 local_4c4;
  undefined4 local_4c0;
  undefined local_4bc;
  uint local_4b8 [6];
  uint local_4a0;
  uint uStack_49c;
  uint uStack_498;
  uint uStack_494;
  uint local_490;
  uint uStack_48c;
  uint local_488 [4];
  uint local_478;
  uint uStack_474;
  uint uStack_470;
  undefined4 local_46c;
  undefined auStack_41c [424];
  undefined auStack_274 [592];
  byte *pbVar18;
  
  local_488[0] = *DAT_080058a0;
  local_488[1] = DAT_080058a0[1];
  local_488[2] = DAT_080058a0[2];
  local_488[3] = DAT_080058a0[3];
  local_478 = DAT_080058a0[4];
  uStack_474 = DAT_080058a0[5];
  uStack_470 = DAT_080058a0[6];
  local_4e0[0] = DAT_080058a0[7];
  local_4e0[1] = DAT_080058a0[8];
  local_4e0[2] = DAT_080058a0[9];
  local_4e0[3] = DAT_080058a0[10];
  local_4d0 = DAT_080058a0[0xb];
  local_4b8[0] = DAT_080058a0[0xc];
  local_4b8[1] = DAT_080058a0[0xd];
  local_4b8[2] = DAT_080058a0[0xe];
  local_4b8[3] = DAT_080058a0[0xf];
  local_4b8[4] = DAT_080058a0[0x10];
  local_4b8[5] = DAT_080058a0[0x11];
  local_4a0 = DAT_080058a0[0x12];
  uStack_49c = DAT_080058a0[0x13];
  uStack_498 = DAT_080058a0[0x14];
  uStack_494 = DAT_080058a0[0x15];
  local_490 = DAT_080058a0[0x16];
  uStack_48c = DAT_080058a0[0x17];
  DaisySeed_Configure(param_1);
  DaisySeed_Init(param_1,0);
  *(undefined4 *)(param_1 + 0x64c) = 0x60;
  DaisySeed_SetAudioBlockSize(param_1);
  iVar3 = param_1 + 0x170;
  puVar13 = local_4e0;
  do {
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    iVar11 = iVar3 + 0x18;
    local_46c = CONCAT22(local_46c._2_2_,uVar1);
    GateIn_Init(iVar3,local_46c,1);
    iVar3 = iVar11;
    puVar13 = puVar13 + 1;
  } while (iVar11 != param_1 + 0x1d0);
  puVar13 = &uStack_48c;
  uVar1 = DaisySeed_GetPin(0xd);
  GPIO_Init(param_1 + 0x620,uVar1,0,0,0);
  uVar1 = DaisySeed_GetPin(0xd);
  GateIn_Init(param_1 + 0x634,uVar1,1);
  iVar3 = param_1 + 0x74;
  do {
    puVar13 = puVar13 + 1;
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    local_46c = CONCAT22(local_46c._2_2_,uVar1);
    iVar11 = iVar3 + 0x24;
    Switch_Init(DAT_080058a4 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),iVar3,local_46c);
    iVar3 = iVar11;
  } while (iVar11 != param_1 + 0x170);
  local_4c8 = 0xff0bff0b;
  local_4bc = 0x10;
  uVar1 = DaisySeed_GetPin(0xb);
  local_4c8 = CONCAT22(local_4c8._2_2_,uVar1);
  uVar1 = DaisySeed_GetPin(0xc);
  local_4c8 = CONCAT22(uVar1,(undefined2)local_4c8);
  local_4c0 = 0;
  local_4cc = 0;
  local_4e4 = 0;
  local_4c4 = 2;
  I2CHandle_Init(&local_4e4,&local_4cc);
  iVar3 = DAT_080058a8;
  uVar6 = 0;
  iVar11 = param_1 + 0x350;
  *(int *)(param_1 + 0x354) = DAT_080058a8;
  *(undefined4 *)(param_1 + 0x350) = local_4e4;
  *(int *)(param_1 + 0x358) = iVar3 + 0x84;
  *(undefined4 *)(param_1 + 0x35c) = DAT_080058ac;
  *(undefined *)(param_1 + 0x374) = 0xff;
  while( true ) {
    uVar2 = uVar6 & 0xf;
    iVar8 = uVar6 << 2;
    iVar19 = ((int)uVar6 >> 4) * 0x41;
    uVar6 = uVar6 + 1;
    uVar1 = (undefined2)iVar8;
    *(undefined *)(iVar3 + iVar19) = 6;
    iVar3 = iVar19 + uVar2 * 4;
    iVar15 = *(int *)(param_1 + 0x358);
    iVar8 = *(int *)(param_1 + 0x354) + iVar3;
    *(undefined2 *)(iVar8 + 1) = uVar1;
    *(undefined2 *)(iVar8 + 3) = uVar1;
    *(undefined *)(iVar15 + iVar19) = 6;
    iVar3 = *(int *)(param_1 + 0x358) + iVar3;
    *(undefined2 *)(iVar3 + 1) = uVar1;
    *(undefined2 *)(iVar3 + 3) = uVar1;
    if (uVar6 == 0x20) break;
    iVar3 = *(int *)(param_1 + 0x354);
  }
  if (*(char *)(param_1 + 0x35e) != '\v') {
    GPIO_Init(param_1 + 0x360,*(undefined2 *)(param_1 + 0x35e),1,0,0);
    GPIO_Write(param_1 + 0x360,0);
  }
  pbVar18 = (byte *)(param_1 + 0x35c);
  do {
    pbVar17 = pbVar18 + 1;
    bVar5 = *pbVar18 | 0x40;
    local_46c = local_46c & 0xffff0000;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c._0_2_ = 0;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c._0_2_ = 0x2000;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c = CONCAT22(local_46c._2_2_,0x3600);
    local_46c = CONCAT31(local_46c._1_3_,1);
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,5);
    uVar4 = DAT_08005bcc;
    pbVar18 = pbVar17;
  } while ((byte *)(param_1 + 0x35e) != pbVar17);
  *(undefined2 *)(param_1 + 0x57c) = 0x100;
  *(int *)(param_1 + 0x578) = iVar11;
  *(undefined *)(param_1 + 0x57e) = 2;
  *(undefined4 *)(param_1 + 0x58c) = 0;
  *(undefined4 *)(param_1 + 0x588) = 0;
  *(undefined4 *)(param_1 + 0x584) = 0;
  *(undefined4 *)(param_1 + 0x580) = uVar4;
  *(undefined *)(param_1 + 0x596) = 5;
  *(int *)(param_1 + 0x590) = iVar11;
  *(undefined2 *)(param_1 + 0x594) = 0x403;
  *(undefined4 *)(param_1 + 0x5a4) = 0;
  *(undefined4 *)(param_1 + 0x5a0) = 0;
  *(undefined4 *)(param_1 + 0x59c) = 0;
  *(undefined4 *)(param_1 + 0x598) = uVar4;
  *(undefined2 *)(param_1 + 0x5ac) = 0x706;
  *(int *)(param_1 + 0x5a8) = iVar11;
  *(undefined *)(param_1 + 0x5ae) = 8;
  *(undefined4 *)(param_1 + 0x5bc) = 0;
  *(undefined4 *)(param_1 + 0x5b8) = 0;
  *(undefined4 *)(param_1 + 0x5b4) = 0;
  *(undefined4 *)(param_1 + 0x5b0) = uVar4;
  *(undefined2 *)(param_1 + 0x5c4) = 0xa09;
  *(int *)(param_1 + 0x5c0) = iVar11;
  *(undefined *)(param_1 + 0x5c6) = 0xb;
  *(undefined4 *)(param_1 + 0x5d4) = 0;
  *(undefined4 *)(param_1 + 0x5d0) = 0;
  *(undefined4 *)(param_1 + 0x5cc) = 0;
  *(undefined4 *)(param_1 + 0x5c8) = uVar4;
  *(int *)(param_1 + 0x5d8) = iVar11;
  *(undefined2 *)(param_1 + 0x5dc) = 0xd0c;
  *(undefined *)(param_1 + 0x5de) = 0xe;
  *(undefined4 *)(param_1 + 0x5ec) = 0;
  *(undefined4 *)(param_1 + 0x5e8) = 0;
  *(undefined4 *)(param_1 + 0x5e4) = 0;
  *(undefined4 *)(param_1 + 0x5e0) = uVar4;
  *(int *)(param_1 + 0x5f0) = iVar11;
  *(undefined2 *)(param_1 + 0x5f4) = 0x1110;
  *(undefined *)(param_1 + 0x5f6) = 0x12;
  *(undefined4 *)(param_1 + 0x604) = 0;
  *(undefined4 *)(param_1 + 0x600) = 0;
  *(undefined4 *)(param_1 + 0x5fc) = 0;
  *(undefined4 *)(param_1 + 0x5f8) = uVar4;
  *(int *)(param_1 + 0x608) = iVar11;
  *(undefined2 *)(param_1 + 0x60c) = 0x1413;
  *(undefined *)(param_1 + 0x60e) = 0x15;
  puVar7 = auStack_41c;
  *(undefined4 *)(param_1 + 0x610) = uVar4;
  *(undefined4 *)(param_1 + 0x61c) = 0;
  *(undefined4 *)(param_1 + 0x618) = 0;
  *(undefined4 *)(param_1 + 0x614) = 0;
  do {
    puVar7[-0x4f] = 0xff;
    puVar7[-0x50] = 0xb;
    *(undefined4 *)(puVar7 + -0x4c) = 0;
    *(undefined4 *)(puVar7 + -0x48) = 0;
    *(undefined4 *)(puVar7 + -0x44) = 0;
    puVar9 = puVar7 + -0x3c;
    do {
      *(undefined4 *)(puVar9 + 4) = 0;
      *puVar9 = 0xb;
      *(undefined4 *)(puVar9 + 8) = 0;
      *(undefined4 *)(puVar9 + 0xc) = 0;
      puVar10 = puVar9 + 0x14;
      puVar9[1] = 0xff;
      puVar9 = puVar10;
    } while (puVar7 != puVar10);
    puVar7 = puVar7 + 0x54;
  } while (&stack0x00000028 != puVar7);
  puVar14 = &local_46c;
  puVar13 = local_4b8;
  do {
    puVar16 = puVar13 + 1;
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    FUN_080071f4(puVar14,uVar1,2);
    puVar14 = puVar14 + 0x15;
    puVar13 = puVar16;
  } while (puVar16 != &local_4a0);
  puVar7 = auStack_274;
  do {
    puVar13 = puVar16 + 1;
    uVar1 = DaisySeed_GetPin(*puVar16 & 0xff);
    FUN_080071f4(puVar7,uVar1,2);
    puVar7 = puVar7 + 0x54;
    puVar16 = puVar13;
  } while (local_488 != puVar13);
  iVar19 = param_1 + 0x18;
  uVar6 = 0;
  FUN_08007214(iVar19,&local_46c,0xc,4);
  iVar3 = param_1 + 0x1d0;
  do {
    uVar4 = FUN_080075cc(iVar19,uVar6 & 0xff);
    uVar6 = uVar6 + 1;
    AnalogControl_Init(DAT_08005bc0 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),DAT_08005bc4,
                       iVar3,uVar4,0);
    iVar3 = iVar3 + 0x20;
  } while (uVar6 != 6);
  cVar12 = '\x06';
  iVar3 = param_1 + 0x290;
  do {
    uVar4 = FUN_080075cc(iVar19,cVar12);
    iVar8 = iVar3 + 0x20;
    cVar12 = cVar12 + '\x01';
    AnalogControl_InitBipolarCv
              (DAT_08005bc0 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),iVar3,uVar4);
    iVar3 = iVar8;
  } while (iVar11 != iVar8);
  *(undefined4 *)(param_1 + 0x294) = DAT_08005bc8;
  return;
}


