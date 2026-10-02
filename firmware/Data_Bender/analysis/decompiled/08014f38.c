/* 08014f38 FUN_08014f38; analyst naming is provisional. */

void FUN_08014f38(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f68;
  FUN_080167e8(DAT_08014f68 + 0x55c);
  if ((*(char *)(iVar1 + 0x53c) != '\0') && (*(int *)(*(int *)(iVar1 + 0x55c) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x528);
    *(undefined4 *)(*(int *)(iVar1 + 0x55c) + 0x20) = 0x10;
    return;
  }
  return;
}


