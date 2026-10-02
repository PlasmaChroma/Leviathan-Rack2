/* 08009b70 FUN_08009b70; analyst naming is provisional. */

void FUN_08009b70(undefined4 *param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  puVar1 = (undefined4 *)(DAT_08009b8c + param_2 * 0xc);
  uVar2 = puVar1[1];
  uVar3 = puVar1[2];
  *param_1 = *puVar1;
  param_1[1] = uVar2;
  param_1[2] = uVar3;
  return;
}


