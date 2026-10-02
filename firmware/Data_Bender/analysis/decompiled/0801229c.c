/* 0801229c FUN_0801229c; analyst naming is provisional. */

undefined4 FUN_0801229c(undefined4 *param_1,undefined4 param_2)

{
  if (param_1 != (undefined4 *)0x0) {
    if (*(char *)(param_1 + 0xb) == '\0') {
      *(undefined *)((int)param_1 + 0x2d) = 0;
      FUN_0800627c();
    }
    *(undefined *)(param_1 + 0xb) = 2;
    FUN_08012360(*param_1,param_1 + 1);
    FUN_080123dc(*param_1,param_2,param_1[1]);
    *DAT_080122ec = *DAT_080122ec | 0x80000000;
    *(undefined *)(param_1 + 0xb) = 1;
    return 0;
  }
  return 1;
}


