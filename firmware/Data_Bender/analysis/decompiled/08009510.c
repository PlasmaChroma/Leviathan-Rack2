/* 08009510 System_GetTick; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 System_GetTick(void)

{
  return *(undefined4 *)(*(int *)(*DAT_08009518 + 0x10) + 0x24);
}


