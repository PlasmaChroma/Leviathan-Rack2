/* 08007ff4 FUN_08007ff4; analyst naming is provisional. */

void FUN_08007ff4(void)

{
  if ((code *)DAT_08007ffc[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08007ffc[0xd])
              (DAT_08007ffc,((undefined4 *)*DAT_08007ffc)[6],*(undefined4 *)*DAT_08007ffc);
    return;
  }
  return;
}


