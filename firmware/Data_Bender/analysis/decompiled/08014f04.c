/* 08014f04 FUN_08014f04; analyst naming is provisional. */

void FUN_08014f04(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f34;
  FUN_080167e8(DAT_08014f34 + 0x3a4);
  if ((*(char *)(iVar1 + 900) != '\0') && (*(int *)(*(int *)(iVar1 + 0x3a4) + 0x1c) << 0x1b < 0)) {
    FUN_08014660(iVar1 + 0x370);
    *(undefined4 *)(*(int *)(iVar1 + 0x3a4) + 0x20) = 0x10;
    return;
  }
  return;
}


