/* 08014124 FUN_08014124; analyst naming is provisional. */

void FUN_08014124(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_08014154 << 0x18)) {
    FUN_0800b9d4((char)*DAT_08014154 * 0x1a0 + DAT_08014158 + 0x128);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}


