/* 080123dc FUN_080123dc; analyst naming is provisional. */

undefined4 FUN_080123dc(int param_1,int *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  if (param_3 == 0) {
    *(uint *)(param_1 + 8) =
         *(uint *)(param_1 + 8) & 0xf0000000 | (param_2[1] + -1) * 0x10 | *param_2 - 1U |
         (param_2[2] + -1) * 0x100 | (param_2[3] + -1) * 0x1000 | (param_2[4] + -1) * 0x10000 |
         (param_2[5] + -1) * 0x100000 | (param_2[6] + -1) * 0x1000000;
    return 0;
  }
  iVar1 = *param_2;
  iVar2 = param_2[1];
  *(uint *)(param_1 + 8) =
       DAT_08012470 & *(uint *)(param_1 + 8) | (param_2[3] + -1) * 0x1000 |
       (param_2[5] + -1) * 0x100000;
  *(uint *)(param_1 + 0xc) =
       iVar1 - 1U | *(uint *)(param_1 + 0xc) & 0xf0000000 | (iVar2 + -1) * 0x10 |
       (param_2[2] + -1) * 0x100 | (param_2[4] + -1) * 0x10000 | (param_2[6] + -1) * 0x1000000;
  return 0;
}


