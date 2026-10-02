/* 08014fd4 FUN_08014fd4; analyst naming is provisional. */

void FUN_08014fd4(void)

{
  int iVar1;
  
  iVar1 = DAT_08015004;
  FUN_080167e8(DAT_08015004 + 0xa84);
  if ((*(char *)(iVar1 + 0xa64) != '\0') && (*(int *)(*(int *)(iVar1 + 0xa84) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xa50);
    *(undefined4 *)(*(int *)(iVar1 + 0xa84) + 0x20) = 0x10;
    return;
  }
  return;
}


