/* 08008000 FUN_08008000; analyst naming is provisional. */

void FUN_08008000(void)

{
  if ((code *)DAT_08008008[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08008008[0xd])
              (DAT_08008008,((undefined4 *)*DAT_08008008)[6],*(undefined4 *)*DAT_08008008);
    return;
  }
  return;
}


