/* 08012ca4 FUN_08012ca4; analyst naming is provisional. */

undefined4 FUN_08012ca4(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint local_c;
  
  iVar1 = param_1 + param_2 * 0x20;
  local_c = 0;
  uVar2 = *(uint *)(iVar1 + 0x500);
  if ((-1 < *(int *)(param_1 + 8) << 0x1a) || (*(int *)(iVar1 + 0x500) < 0)) {
    *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x40000000;
    if ((uVar2 >> 0x12 & 1) == 0) {
      if (*(int *)(param_1 + 8) << 0x1a < 0) {
        *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
        return 0;
      }
      uVar2 = *(uint *)(iVar1 + 0x500);
      if ((*(uint *)(param_1 + 0x2c) & 0xff0000) != 0) goto LAB_08012d5e;
      *(uint *)(iVar1 + 0x500) = uVar2 & 0x7fffffff;
      *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
      do {
        local_c = local_c + 1;
        if (1000 < local_c) {
          return 0;
        }
      } while (*(int *)(iVar1 + 0x500) < 0);
    }
    else {
      uVar2 = *(uint *)(iVar1 + 0x500);
      if ((*(uint *)(param_1 + 0x410) & 0xff0000) != 0) {
LAB_08012d5e:
        *(uint *)(iVar1 + 0x500) = uVar2 | 0x80000000;
        return 0;
      }
      *(uint *)(iVar1 + 0x500) = uVar2 & 0x7fffffff;
      *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
      do {
        local_c = local_c + 1;
        if (1000 < local_c) {
          return 0;
        }
      } while (*(int *)(iVar1 + 0x500) < 0);
    }
  }
  return 0;
}


