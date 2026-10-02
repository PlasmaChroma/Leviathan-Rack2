/* 0800ee50 FUN_0800ee50; analyst naming is provisional. */

undefined4 FUN_0800ee50(undefined4 *param_1,undefined param_2)

{
  if (*(char *)(param_1 + 0x12f) != '\x01') {
    *(undefined *)(param_1 + 0xe) = param_2;
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012b3c(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}


