/* 08012658 FUN_08012658; analyst naming is provisional. */

uint FUN_08012658(int param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_1 + 0x808) & 6;
  if (uVar1 != 0) {
    if ((*(uint *)(param_1 + 0x808) & 2) == 0) {
      uVar1 = 0xf;
    }
    else {
      uVar1 = 2;
    }
  }
  return uVar1;
}


