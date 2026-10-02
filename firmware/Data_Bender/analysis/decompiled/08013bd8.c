/* 08013bd8 FUN_08013bd8; analyst naming is provisional. */

undefined4
FUN_08013bd8(int param_1,undefined4 param_2,undefined4 param_3,undefined2 param_4,code *param_5,
            code *param_6,undefined4 param_7)

{
  bool bVar1;
  undefined *puVar2;
  code **ppcVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  
  do {
    iVar5 = FUN_08015f64(param_1 + 0x28);
  } while (iVar5 != 1);
  iVar5 = FUN_080139ac(param_1);
  ppcVar3 = DAT_08013c9c;
  puVar2 = DAT_08013c98;
  if (iVar5 == 0) {
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08013c98 = *(undefined *)(param_1 + 8);
    puVar4 = DAT_08013ca0;
    *ppcVar3 = param_6;
    *puVar4 = param_7;
    if (param_5 != (code *)0x0) {
      (*param_5)(param_7);
    }
    iVar6 = FUN_08015a48(param_1 + 0x28,param_3,param_2,param_4);
    uVar7 = 0;
    if (iVar6 != 0) {
      *ppcVar3 = (code *)0x0;
      *puVar4 = 0;
      *puVar2 = 0xff;
      if (param_6 == (code *)0x0) {
        uVar7 = 1;
      }
      else {
        uVar7 = 1;
        (*param_6)(param_7);
      }
    }
    if (iVar5 == 0) {
      enableIRQinterrupts();
    }
    return uVar7;
  }
  if (param_6 == (code *)0x0) {
    return 1;
  }
  (*param_6)(param_7,1);
  return 1;
}


