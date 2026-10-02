/* 08009358 SaiHandle_StartDma; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4
SaiHandle_StartDma(int *param_1,undefined4 param_2,undefined4 param_3,uint param_4,
                  undefined4 param_5)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *param_1;
  *(uint *)(iVar1 + 0x250) = param_4;
  *(undefined4 *)(iVar1 + 0x248) = param_2;
  *(undefined4 *)(iVar1 + 0x24c) = param_3;
  *(undefined4 *)(iVar1 + 0x254) = param_5;
  if (*(int *)(iVar1 + 0x18) == 1) {
    if (*(int *)(iVar1 + 0x20) == 1) {
      FUN_08012088(iVar1 + 0x28);
      iVar2 = *(int *)(iVar1 + 0x24);
    }
    else {
      FUN_08011f64(iVar1 + 0x28,param_3);
      iVar2 = *(int *)(iVar1 + 0x24);
    }
    if (iVar2 != 1) {
      FUN_08011f64(iVar1 + 0xc0,param_3);
      return 0;
    }
    FUN_08012088(iVar1 + 0xc0,param_2,param_4 & 0xffff);
    return 0;
  }
  if (*(int *)(iVar1 + 0x24) == 1) {
    FUN_08012088(iVar1 + 0xc0);
    iVar2 = *(int *)(iVar1 + 0x20);
  }
  else {
    FUN_08011f64(iVar1 + 0xc0,param_3);
    iVar2 = *(int *)(iVar1 + 0x20);
  }
  if (iVar2 != 1) {
    FUN_08011f64(iVar1 + 0x28,param_3);
    return 0;
  }
  FUN_08012088(iVar1 + 0x28,param_2,param_4 & 0xffff);
  return 0;
}


