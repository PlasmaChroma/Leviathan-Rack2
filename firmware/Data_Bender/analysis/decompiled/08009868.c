/* 08009868 System_GetProgramMemoryRegion; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 System_GetProgramMemoryRegion(void)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(DAT_080098dc + 8);
  if (uVar2 + 0xf8000000 < 0x20000) {
    return 0;
  }
  if (uVar2 < 0x10000) {
    return 1;
  }
  if (uVar2 + 0xe0000000 < 0x20000) {
    return 2;
  }
  if (uVar2 + 0xdc000000 < 0x80000) {
    return 3;
  }
  if (uVar2 + 0xf0000000 < 0x48000) {
    return 4;
  }
  if (uVar2 + 0xc8000000 < 0x10000) {
    return 5;
  }
  if (uVar2 + 0x40000000 < 0x4000000) {
    return 6;
  }
  if (uVar2 + 0x70000000 < 0x800000) {
    uVar1 = 7;
  }
  else {
    uVar1 = 8;
  }
  return uVar1;
}


