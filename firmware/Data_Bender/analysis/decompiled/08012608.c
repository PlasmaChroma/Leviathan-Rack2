/* 08012608 FUN_08012608; analyst naming is provisional. */

undefined4 FUN_08012608(int param_1)

{
  uint uVar1;
  uint local_4;
  
  uVar1 = DAT_08012654;
  local_4 = 0;
  do {
    local_4 = local_4 + 1;
    if (DAT_08012654 < local_4) {
      return 3;
    }
  } while (-1 < *(int *)(param_1 + 0x10));
  local_4 = 0;
  *(undefined4 *)(param_1 + 0x10) = 0x10;
  do {
    local_4 = local_4 + 1;
    if (uVar1 < local_4) {
      return 3;
    }
  } while ((*(uint *)(param_1 + 0x10) & 0x10) != 0);
  return 0;
}


