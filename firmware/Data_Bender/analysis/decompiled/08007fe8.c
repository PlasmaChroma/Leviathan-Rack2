/* 08007fe8 FUN_08007fe8; analyst naming is provisional. */

void FUN_08007fe8(void)

{
  if ((code *)DAT_08007ff0[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08007ff0[0xd])
              (DAT_08007ff0,((undefined4 *)*DAT_08007ff0)[6],*(undefined4 *)*DAT_08007ff0);
    return;
  }
  return;
}


