/* 08009520 System_GetBootloaderVersion; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int System_GetBootloaderVersion(void)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  if (*(int *)(DAT_08009548 + 8) + 0xf8000000U < 0x20000) {
    iVar2 = 1;
  }
  else {
    iVar2 = 0;
    while( true ) {
      iVar1 = iVar2;
      if (*(int *)(DAT_0800954c + 8) == iVar2) break;
      bVar3 = iVar2 == 3;
      iVar2 = iVar2 + 1;
      if (bVar3) {
        return iVar1;
      }
    }
  }
  return iVar2;
}


