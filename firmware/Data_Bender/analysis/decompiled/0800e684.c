/* 0800e684 FUN_0800e684; analyst naming is provisional. */

undefined4 FUN_0800e684(uint **param_1,int param_2)

{
  uint *puVar1;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if (*(char *)(param_1 + 0x10) != '\x01') {
    puVar1 = *param_1;
    *(undefined *)((int)param_1 + 0x41) = 0x24;
    *puVar1 = *puVar1 & 0xfffffffe;
    *puVar1 = *puVar1 & 0xfffff0ff | param_2 << 8;
    *puVar1 = *puVar1 | 1;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)(param_1 + 0x10) = 0;
    return 0;
  }
  return 2;
}


