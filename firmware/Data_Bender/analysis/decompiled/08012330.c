/* 08012330 FUN_08012330; analyst naming is provisional. */

char FUN_08012330(undefined4 *param_1)

{
  char cVar1;
  
  cVar1 = *(char *)(param_1 + 0xb);
  if (cVar1 != '\x02') {
    if (*(char *)(param_1 + 0xb) == '\x01') {
      *(undefined *)(param_1 + 0xb) = 2;
      FUN_080124a4(*param_1);
      *(undefined *)(param_1 + 0xb) = 1;
      return '\0';
    }
    cVar1 = '\x01';
  }
  return cVar1;
}


