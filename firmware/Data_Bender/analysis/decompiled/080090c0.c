/* 080090c0 FUN_080090c0; analyst naming is provisional. */

void FUN_080090c0(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int *piVar7;
  int *piVar8;
  
  piVar7 = DAT_08009220;
  piVar1 = DAT_0800921c;
  iVar2 = DAT_08009218;
  iVar4 = *param_1;
  iVar3 = DAT_08009210;
  if ((iVar4 == DAT_08009210) || (iVar3 = DAT_08009210 + 0x20, iVar4 == iVar3)) {
    *(uint *)(DAT_08009218 + 0xe0) = *(uint *)(DAT_08009218 + 0xe0) | 1;
    uVar5 = *(uint *)(iVar2 + 0xe0);
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x10;
    uVar6 = *(uint *)(iVar2 + 0xe0);
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x40;
    *(uint *)(iVar2 + 0xf0) = *(uint *)(iVar2 + 0xf0) | 0x400000;
    FUN_08009004(piVar7,param_2,iVar3,*(uint *)(iVar2 + 0xf0) & 0x400000,uVar5 & 1,uVar6 & 0x10,
                 *(uint *)(iVar2 + 0xe0) & 0x40);
    piVar1 = DAT_08009220;
    iVar3 = DAT_08009210;
    iVar4 = *param_1;
    *(uint *)(iVar2 + 0xd8) = *(uint *)(iVar2 + 0xd8) | 1;
    iVar4 = iVar4 - iVar3;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
  }
  else {
    iVar3 = DAT_08009214;
    if ((iVar4 != DAT_08009214) && (iVar3 = DAT_08009214 + 0x20, iVar4 != iVar3)) {
      return;
    }
    *(uint *)(DAT_08009218 + 0xe0) = *(uint *)(DAT_08009218 + 0xe0) | 1;
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 8;
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x40;
    *(uint *)(iVar2 + 0xf0) = *(uint *)(iVar2 + 0xf0) | 0x800000;
    FUN_08009004(piVar1,param_2,iVar3,*(uint *)(iVar2 + 0xf0) & 0x800000);
    piVar1 = DAT_0800921c;
    iVar3 = DAT_08009214;
    iVar4 = *param_1;
    *(uint *)(iVar2 + 0xd8) = *(uint *)(iVar2 + 0xd8) | 1;
    iVar4 = iVar4 - iVar3;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
  }
  if (iVar4 == 0) {
    piVar8 = piVar1 + 10;
    piVar7 = piVar1 + 0x56;
    if (piVar1[8] == 1) {
      iVar2 = 0;
    }
    else {
      iVar2 = 0x40;
    }
    if (*piVar1 == 0) {
      iVar3 = 0x57;
      piVar1[0x56] = DAT_08008f40;
    }
    else {
      iVar3 = 0x59;
      piVar1[0x56] = DAT_08008f4c;
    }
  }
  else {
    piVar8 = piVar1 + 0x30;
    piVar7 = piVar1 + 0x74;
    if (piVar1[9] == 1) {
      iVar2 = 0;
    }
    else {
      iVar2 = 0x40;
    }
    if (*piVar1 == 0) {
      iVar3 = 0x58;
      piVar1[0x74] = DAT_08008f48;
    }
    else {
      iVar3 = 0x5a;
      piVar1[0x74] = DAT_08008f44;
    }
  }
  piVar7[1] = iVar3;
  piVar7[2] = iVar2;
  piVar7[9] = 0;
  piVar7[5] = 0x1000;
  piVar7[3] = 0;
  piVar7[4] = 0x400;
  piVar7[6] = 0x4000;
  piVar7[7] = 0x100;
  piVar7[8] = 0x20000;
  iVar2 = FUN_0800af40(piVar7);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  piVar8[0x20] = (int)piVar7;
  piVar8[0x21] = (int)piVar7;
  piVar7[0xe] = (int)piVar8;
  return;
}


