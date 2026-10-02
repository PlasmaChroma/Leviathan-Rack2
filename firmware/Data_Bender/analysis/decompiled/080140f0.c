/* 080140f0 FUN_080140f0; analyst naming is provisional. */

void FUN_080140f0(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_0801411c << 0x18)) {
    FUN_0800b9d4((char)*DAT_0801411c * 0x1a0 + DAT_08014120 + 0xb0);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}


