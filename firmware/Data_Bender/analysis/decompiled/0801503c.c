/* 0801503c FUN_0801503c; analyst naming is provisional. */

void FUN_0801503c(void)

{
  int iVar1;
  
  iVar1 = DAT_0801506c;
  FUN_080167e8(DAT_0801506c + 0xdf4);
  if ((*(char *)(iVar1 + 0xdd4) != '\0') && (*(int *)(*(int *)(iVar1 + 0xdf4) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xdc0);
    *(undefined4 *)(*(int *)(iVar1 + 0xdf4) + 0x20) = 0x10;
    return;
  }
  return;
}


