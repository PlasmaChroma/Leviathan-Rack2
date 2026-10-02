/* 08014a2c FUN_08014a2c; analyst naming is provisional. */

undefined4
FUN_08014a2c(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  code **ppcVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  
  iVar7 = param_1 + 0x34;
  do {
    iVar5 = FUN_08016bcc(iVar7);
  } while (iVar5 != 0x20);
  *(undefined4 *)(param_1 + 0xd8) = 0x400;
  *(undefined4 *)(param_1 + 0x150) = 0x400;
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *(undefined4 *)(param_1 + 0xe8) = 0x30000;
  *(undefined4 *)(param_1 + 0x160) = 0x30000;
  iVar5 = DAT_08014b24;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  *(int *)(param_1 + 200) = iVar5;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(int *)(param_1 + 0x140) = iVar5 + 1000;
  *(undefined4 *)(param_1 + 0xd0) = 0;
  *(undefined4 *)(param_1 + 0x14c) = 0;
  *(undefined4 *)(param_1 + 0x15c) = 0;
  *(undefined4 *)(param_1 + 0x164) = 0;
  *(undefined4 *)(param_1 + 0x148) = 0x40;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0x154) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  FUN_08014880(param_1);
  iVar5 = FUN_0800af40(param_1 + 200);
  if (iVar5 == 0) {
    *(int *)(param_1 + 0xb4) = param_1 + 200;
    *(int *)(param_1 + 0x100) = iVar7;
    ppcVar4 = DAT_08014b30;
    puVar3 = DAT_08014b2c;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08014b2c = *(undefined *)(param_1 + 0x1c);
    puVar2 = DAT_08014b28;
    *ppcVar4 = param_5;
    *puVar2 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_08017434(iVar7,param_2,param_3);
    if (iVar7 == 0) {
      uVar6 = 0;
    }
    else {
      *puVar3 = 0xff;
      *ppcVar4 = (code *)0x0;
      *puVar2 = 0;
      if (param_5 == (code *)0x0) {
        uVar6 = 1;
      }
      else {
        (*param_5)(param_6,1);
        uVar6 = 1;
      }
    }
    if (iVar5 == 0) {
      enableIRQinterrupts();
    }
    return uVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


