/* 08007c04 FUN_08007c04; analyst naming is provisional. */

int FUN_08007c04(int *param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
                undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  
  piVar7 = param_1 + 0x23;
  uVar8 = param_4;
  do {
    iVar5 = FUN_0800e628(piVar7);
  } while (iVar5 != 0x20);
  iVar5 = *param_1;
  param_1[5] = DAT_08007cd8;
  if (iVar5 == 1) {
    param_1[6] = 0x23;
  }
  else if (iVar5 == 2) {
    param_1[6] = 0x49;
  }
  else {
    if (iVar5 != 0) {
      return 1;
    }
    param_1[6] = 0x21;
  }
  param_1[0xe] = 0;
  param_1[9] = 0x400;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  iVar5 = FUN_0800af40(param_1 + 5);
  if (iVar5 == 0) {
    param_1[0x32] = (int)(param_1 + 5);
    param_1[0x13] = (int)piVar7;
    puVar4 = DAT_08007ce4;
    puVar3 = DAT_08007ce0;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08007ce0 = *(undefined *)param_1;
    *puVar4 = param_5;
    puVar2 = DAT_08007cdc;
    iVar6 = param_1[3];
    *DAT_08007cdc = param_6;
    if (iVar6 == 0) {
      iVar6 = FUN_0800d534(piVar7,(param_2 & 0x7fff) << 1,param_3,param_4);
    }
    else {
      iVar6 = FUN_0800d798(piVar7,param_3,param_4,iVar6,uVar8);
    }
    if (iVar6 != 0) {
      iVar6 = 1;
      *puVar3 = 0xff;
      *puVar4 = 0;
      *puVar2 = 0;
    }
    if (iVar5 != 0) {
      return iVar6;
    }
    enableIRQinterrupts();
    return iVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


