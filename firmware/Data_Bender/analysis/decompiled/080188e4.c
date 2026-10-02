/* 080188e4 libc_init_array; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void libc_init_array(void)

{
  code **ppcVar1;
  int iVar2;
  int iVar3;
  code **ppcVar4;
  int iVar5;
  
  iVar3 = DAT_08018920 - (int)DAT_0801891c;
  ppcVar4 = DAT_0801891c;
  for (iVar5 = 0; iVar2 = DAT_08018928, ppcVar1 = DAT_08018924, iVar5 != iVar3 >> 2;
      iVar5 = iVar5 + 1) {
    (**ppcVar4)();
    ppcVar4 = ppcVar4 + 1;
  }
  FUN_0801a51c();
  ppcVar4 = ppcVar1;
  for (iVar3 = 0; iVar3 != iVar2 - (int)ppcVar1 >> 2; iVar3 = iVar3 + 1) {
    (**ppcVar4)();
    ppcVar4 = ppcVar4 + 1;
  }
  return;
}


