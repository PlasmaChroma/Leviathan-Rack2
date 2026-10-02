/* 08005bec DaisySeed_GetPin; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined2 DaisySeed_GetPin(uint param_1)

{
  if (param_1 < 0x21) {
    return CONCAT11(*(undefined *)(DAT_08005c14 + param_1 * 2 + 1),
                    *(undefined *)(DAT_08005c14 + param_1 * 2));
  }
  return 0xc01;
}


