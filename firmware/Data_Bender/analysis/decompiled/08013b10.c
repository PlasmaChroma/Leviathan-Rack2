/* 08013b10 FUN_08013b10; analyst naming is provisional. */

undefined4
FUN_08013b10(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  undefined uVar1;
  bool bVar2;
  code **ppcVar3;
  undefined4 *puVar4;
  undefined *puVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  
  do {
    iVar6 = FUN_08015f64(param_1 + 0x28);
  } while (iVar6 != 1);
  iVar6 = FUN_080139ac(param_1);
  puVar5 = DAT_08013bd4;
  puVar4 = DAT_08013bd0;
  ppcVar3 = DAT_08013bcc;
  if (iVar6 == 0) {
    iVar6 = 0;
    bVar2 = (bool)isCurrentModePrivileged();
    if (bVar2) {
      iVar6 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    uVar1 = *(undefined *)(param_1 + 8);
    *DAT_08013bcc = param_5;
    *puVar5 = uVar1;
    *puVar4 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_080158b4(param_1 + 0x28,param_2,param_3);
    uVar8 = 0;
    if (iVar7 != 0) {
      *ppcVar3 = (code *)0x0;
      *puVar4 = 0;
      *puVar5 = 0xff;
      if (param_5 == (code *)0x0) {
        uVar8 = 1;
      }
      else {
        uVar8 = 1;
        (*param_5)(param_6);
      }
    }
    if (iVar6 == 0) {
      enableIRQinterrupts();
    }
    return uVar8;
  }
  if (param_5 == (code *)0x0) {
    return 1;
  }
  (*param_5)(param_6,1);
  return 1;
}


