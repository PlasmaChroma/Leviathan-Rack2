/* 0800cd24 FUN_0800cd24; analyst naming is provisional. */

undefined4 FUN_0800cd24(undefined4 *param_1)

{
  if (*(char *)(param_1 + 0xbe) != '\x01') {
    *(undefined *)(param_1 + 0xbe) = 1;
    FUN_08012d88(*param_1);
    *(undefined *)(param_1 + 0xbe) = 0;
    return 0;
  }
  return 2;
}


