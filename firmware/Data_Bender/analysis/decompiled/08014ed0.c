/* 08014ed0 FUN_08014ed0; analyst naming is provisional. */

void FUN_08014ed0(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f00;
  FUN_080167e8(DAT_08014f00 + 0x1ec);
  if ((*(char *)(iVar1 + 0x1cc) != '\0') && (*(int *)(*(int *)(iVar1 + 0x1ec) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x1b8);
    *(undefined4 *)(*(int *)(iVar1 + 0x1ec) + 0x20) = 0x10;
    return;
  }
  return;
}


