/* 080150a4 FUN_080150a4; analyst naming is provisional. */

void FUN_080150a4(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_080150d4 << 0x18)) {
    FUN_0800b9d4((char)*DAT_080150d4 * 0x1b8 + DAT_080150d8 + 0x140);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}


