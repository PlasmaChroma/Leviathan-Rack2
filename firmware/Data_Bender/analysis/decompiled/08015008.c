/* 08015008 FUN_08015008; analyst naming is provisional. */

void FUN_08015008(void)

{
  int iVar1;
  
  iVar1 = DAT_08015038;
  FUN_080167e8(DAT_08015038 + 0xc3c);
  if ((*(char *)(iVar1 + 0xc1c) != '\0') && (*(int *)(*(int *)(iVar1 + 0xc3c) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xc08);
    *(undefined4 *)(*(int *)(iVar1 + 0xc3c) + 0x20) = 0x10;
    return;
  }
  return;
}


