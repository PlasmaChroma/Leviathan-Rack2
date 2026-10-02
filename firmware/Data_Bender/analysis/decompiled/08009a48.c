/* 08009a48 FUN_08009a48; analyst naming is provisional. */

undefined4 FUN_08009a48(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  code *pcVar2;
  
  iVar1 = *(int *)(param_1 + 0x508);
  *(undefined *)(iVar1 + 0x29c) = 1;
  if (*(int *)(iVar1 + 0x2b8) == 0) {
    return 0;
  }
  pcVar2 = *(code **)(*(int *)(iVar1 + 0x2b8) + 4);
  iVar1 = (*pcVar2)(iVar1,*(undefined *)(iVar1 + 4),pcVar2,param_4,param_4);
  if (iVar1 == 0) {
    return 0;
  }
  return 3;
}


