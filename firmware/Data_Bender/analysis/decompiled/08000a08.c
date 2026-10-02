/* 08000a08 Reset_Handler; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Reset_Handler(void)

{
  undefined4 in_r3;
  
  if (DAT_08000a38 != DAT_08000a3c) {
    libc_memcpy(DAT_08000a38,DAT_08000a40,DAT_08000a3c - DAT_08000a38,in_r3,in_r3);
  }
  if (DAT_08000a44 != DAT_08000a48) {
    memset(DAT_08000a44,0,DAT_08000a48 - DAT_08000a44);
  }
  SystemInit_STM32H7();
  libc_init_array();
  main();
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


