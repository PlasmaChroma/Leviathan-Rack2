/* 080098fc SystemInit_STM32H7; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void SystemInit_STM32H7(void)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  
  uVar2 = DAT_08009988;
  puVar1 = DAT_08009984;
  *(uint *)(DAT_08009980 + 0x88) = *(uint *)(DAT_08009980 + 0x88) | 0xf00000;
  puVar3 = DAT_0800998c;
  *puVar1 = *puVar1 | 1;
  puVar1[4] = 0;
  uVar4 = DAT_08009990;
  *puVar1 = uVar2 & *puVar1;
  puVar1[6] = 0;
  puVar1[7] = 0;
  puVar1[8] = 0;
  puVar1[10] = 0;
  puVar1[0xb] = 0;
  puVar1[0xc] = 0;
  puVar1[0xd] = 0;
  puVar1[0xe] = 0;
  puVar1[0xf] = 0;
  puVar1[0x10] = 0;
  puVar1[0x11] = 0;
  *puVar1 = *puVar1 & 0xfffbffff;
  puVar1[0x18] = 0;
  puVar1[0x37] = puVar1[0x37] | 0xe0000000;
  if ((uVar4 & *puVar3) < 0x20000000) {
    *(undefined4 *)(DAT_08009994 + 0x108) = 1;
  }
  *(undefined4 *)(DAT_08009980 + 8) = 0x8000000;
  return;
}


