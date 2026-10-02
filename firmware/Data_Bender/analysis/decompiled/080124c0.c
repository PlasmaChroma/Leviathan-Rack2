/* 080124c0 FUN_080124c0; analyst naming is provisional. */

undefined4 FUN_080124c0(int param_1,int param_2,int param_3)

{
  uint uVar1;
  
  if (param_3 == 2) {
    if ((uint)(DAT_0801256c + param_2) <= DAT_08012570) {
      uVar1 = 0x3c00;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012574 + param_2) <= DAT_08012578) {
      uVar1 = 0x3800;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_0801257c + param_2) <= DAT_08012580) {
      uVar1 = 0x3400;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012584 + param_2) < DAT_08012588) {
      uVar1 = 0x3000;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_0801258c + param_2) <= DAT_08012590) {
      uVar1 = 0x2c00;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012594 + param_2) < DAT_08012598) {
      uVar1 = 0x2800;
      goto LAB_080124ca;
    }
    if (DAT_080125a0 <= (uint)(DAT_0801259c + param_2)) {
      if ((uint)(DAT_080125a4 + param_2) < DAT_080125a8) {
        uVar1 = 0x2000;
      }
      else if ((uint)(DAT_080125ac + param_2) < DAT_080125b0) {
        uVar1 = 0x1c00;
      }
      else {
        uVar1 = 0x1800;
      }
      goto LAB_080124ca;
    }
  }
  uVar1 = 0x2400;
LAB_080124ca:
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) & 0xffffc3ff;
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | uVar1;
  return 0;
}


