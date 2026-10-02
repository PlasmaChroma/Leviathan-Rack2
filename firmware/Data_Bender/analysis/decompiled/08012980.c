/* 08012980 FUN_08012980; analyst naming is provisional. */

undefined4 FUN_08012980(int param_1,byte *param_2)

{
  uint *puVar1;
  uint local_4;
  
  local_4 = 0;
  if (param_2[1] == 1) {
    puVar1 = (uint *)(param_1 + 0x900 + (uint)*param_2 * 0x20);
    if ((int)*puVar1 < 0) {
      *puVar1 = *puVar1 | 0x8000000;
      *puVar1 = *puVar1 | 0x40000000;
      do {
        local_4 = local_4 + 1;
        if (10000 < local_4) {
          return 1;
        }
      } while ((int)*puVar1 < 0);
    }
  }
  else {
    puVar1 = (uint *)(param_1 + 0xb00 + (uint)*param_2 * 0x20);
    if ((int)*puVar1 < 0) {
      *puVar1 = *puVar1 | 0x8000000;
      *puVar1 = *puVar1 | 0x40000000;
      do {
        local_4 = local_4 + 1;
        if (10000 < local_4) {
          return 1;
        }
      } while ((int)*puVar1 < 0);
    }
  }
  return 0;
}


