/* 08009b38 FUN_08009b38; analyst naming is provisional. */

undefined4 FUN_08009b38(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x300);
  *(undefined *)(iVar1 + 0x321) = 1;
  *(undefined *)(iVar1 + 0x323) = 0;
  *(undefined *)(iVar1 + 800) = 0;
  FUN_08009b54();
  FUN_08013750(iVar1,*(undefined *)(iVar1 + 4));
  FUN_08013750(iVar1,*(undefined *)(iVar1 + 5));
  return 0;
}


