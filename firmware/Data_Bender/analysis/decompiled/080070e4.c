/* 080070e4 GateIn_Trig; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

uint GateIn_Trig(int param_1)

{
  uint uVar1;
  
  *(undefined *)(param_1 + 0x14) = *(undefined *)(param_1 + 0x15);
  if (*(char *)(param_1 + 0x16) == '\0') {
    uVar1 = GPIO_Read();
  }
  else {
    uVar1 = GPIO_Read();
    uVar1 = (uVar1 ^ 1) & 0xff;
  }
  *(char *)(param_1 + 0x15) = (char)uVar1;
  if (uVar1 != 0) {
    uVar1 = *(byte *)(param_1 + 0x14) ^ 1;
  }
  return uVar1;
}


