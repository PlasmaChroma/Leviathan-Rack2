/* 08009b30 FUN_08009b30; analyst naming is provisional. */

undefined4 FUN_08009b30(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x300);
  *(undefined *)(iVar1 + 800) = 1;
  *(undefined *)(iVar1 + 0x321) = 0;
  *(undefined *)(iVar1 + 0x322) = 0;
  return 0;
}


