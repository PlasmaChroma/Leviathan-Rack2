/* 080125b4 FUN_080125b4; analyst naming is provisional. */

undefined4 FUN_080125b4(int param_1,int param_2)

{
  uint uVar1;
  uint local_4;
  
  uVar1 = DAT_08012604;
  local_4 = 0;
  do {
    local_4 = local_4 + 1;
    if (DAT_08012604 < local_4) {
      return 3;
    }
  } while (-1 < *(int *)(param_1 + 0x10));
  local_4 = 0;
  *(uint *)(param_1 + 0x10) = param_2 << 6 | 0x20;
  do {
    local_4 = local_4 + 1;
    if (uVar1 < local_4) {
      return 3;
    }
  } while ((*(uint *)(param_1 + 0x10) & 0x20) != 0);
  return 0;
}


