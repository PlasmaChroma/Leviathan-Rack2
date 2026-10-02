/* 08012360 FUN_08012360; analyst naming is provisional. */

undefined4 FUN_08012360(uint *param_1,int *param_2)

{
  uint uVar1;
  
  uVar1 = DAT_080123d8;
  if (*param_2 == 0) {
    *param_1 = *param_1 & 0xffff8000 | param_2[1] | param_2[2] | param_2[3] | param_2[4] |
               param_2[5] | param_2[6] | param_2[7] | param_2[8] | param_2[9];
    return 0;
  }
  *param_1 = *param_1 & 0xffff83ff | param_2[7] | param_2[8] | param_2[9];
  param_1[1] = uVar1 & param_1[1] | param_2[1] | param_2[2] | param_2[3] | param_2[4] | param_2[5] |
               param_2[6];
  return 0;
}


