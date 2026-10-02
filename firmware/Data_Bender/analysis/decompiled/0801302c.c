/* 0801302c FUN_0801302c; analyst naming is provisional. */

undefined4 FUN_0801302c(int param_1)

{
  if (*(char *)(param_1 + 0x29c) != '\x04') {
    *(undefined *)(param_1 + 0x29d) = *(undefined *)(param_1 + 0x29c);
  }
  *(undefined *)(param_1 + 0x29c) = 4;
  return 0;
}


