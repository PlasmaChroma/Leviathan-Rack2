/* 08014ea4 FUN_08014ea4; analyst naming is provisional. */

void FUN_08014ea4(void)

{
  int iVar1;
  
  iVar1 = DAT_08014ecc;
  FUN_080167e8(DAT_08014ecc + 0x34);
  if ((*(char *)(iVar1 + 0x14) != '\0') && (*(int *)(*(int *)(iVar1 + 0x34) + 0x1c) << 0x1b < 0)) {
    FUN_08014660(iVar1);
    *(undefined4 *)(*(int *)(iVar1 + 0x34) + 0x20) = 0x10;
    return;
  }
  return;
}


