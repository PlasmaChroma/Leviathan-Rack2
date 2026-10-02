/* 08007fb4 FUN_08007fb4; analyst naming is provisional. */

void FUN_08007fb4(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_08007fe0 << 0x18)) {
    FUN_0800b9d4(DAT_08007fe4 + (char)*DAT_08007fe0 * 0xe0 + 0x14);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}


