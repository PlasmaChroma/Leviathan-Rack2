/* 08014f6c FUN_08014f6c; analyst naming is provisional. */

void FUN_08014f6c(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f9c;
  FUN_080167e8(DAT_08014f9c + 0x714);
  if ((*(char *)(iVar1 + 0x6f4) != '\0') && (*(int *)(*(int *)(iVar1 + 0x714) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x6e0);
    *(undefined4 *)(*(int *)(iVar1 + 0x714) + 0x20) = 0x10;
    return;
  }
  return;
}


