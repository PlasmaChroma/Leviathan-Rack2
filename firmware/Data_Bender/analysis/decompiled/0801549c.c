/* 0801549c FUN_0801549c; analyst naming is provisional. */

undefined4 FUN_0801549c(int *param_1)

{
  if (param_1 == (int *)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x3d) != '\x02') {
    param_1[0x1a] = 0x80;
    return 1;
  }
  *(undefined *)((int)param_1 + 0x3d) = 4;
  *(uint *)(*param_1 + 0xc) = *(uint *)(*param_1 + 0xc) & 0xfffffffe;
  return 0;
}


