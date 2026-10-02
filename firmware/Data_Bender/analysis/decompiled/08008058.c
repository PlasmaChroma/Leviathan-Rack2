/* 08008058 I2CHandle_TransmitDma; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4
I2CHandle_TransmitDma
          (int **param_1,undefined2 param_2,undefined4 param_3,undefined2 param_4,undefined4 param_5
          ,undefined4 param_6)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = DAT_080080bc;
  iVar4 = **param_1;
  if (iVar4 == 3) {
    return 1;
  }
  if ((*DAT_080080c0 & 0x80) != 0) {
    uVar2 = FUN_08007b1c();
    return uVar2;
  }
  iVar3 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar3 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *(undefined2 *)(DAT_080080bc + iVar4 * 0x18) = param_2;
  iVar5 = iVar5 + iVar4 * 0x18;
  *(undefined4 *)(iVar5 + 4) = param_3;
  *(undefined2 *)(iVar5 + 8) = param_4;
  *(undefined4 *)(iVar5 + 0x14) = 0;
  *(undefined4 *)(iVar5 + 0xc) = param_5;
  *(undefined4 *)(iVar5 + 0x10) = param_6;
  if (iVar3 == 0) {
    enableIRQinterrupts();
  }
  return 0;
}


