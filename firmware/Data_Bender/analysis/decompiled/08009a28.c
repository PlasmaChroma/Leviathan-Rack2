/* 08009a28 FUN_08009a28; analyst naming is provisional. */

undefined4 FUN_08009a28(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x508);
  if (*(char *)(iVar1 + 0x29c) == '\x04') {
    *(undefined *)(iVar1 + 0x29c) = *(undefined *)(iVar1 + 0x29d);
  }
  return 0;
}


