/* 08014fa0 FUN_08014fa0; analyst naming is provisional. */

void FUN_08014fa0(void)

{
  int iVar1;
  
  iVar1 = DAT_08014fd0;
  FUN_080167e8(DAT_08014fd0 + 0x8cc);
  if ((*(char *)(iVar1 + 0x8ac) != '\0') && (*(int *)(*(int *)(iVar1 + 0x8cc) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x898);
    *(undefined4 *)(*(int *)(iVar1 + 0x8cc) + 0x20) = 0x10;
    return;
  }
  return;
}


