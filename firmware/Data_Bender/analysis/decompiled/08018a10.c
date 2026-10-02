/* 08018a10 FUN_08018a10; analyst naming is provisional. */

void FUN_08018a10(undefined4 *param_1,undefined2 param_2,undefined2 param_3)

{
  undefined4 uVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[2] = 0;
  *(undefined2 *)(param_1 + 3) = param_2;
  param_1[0x19] = 0;
  *(undefined2 *)((int)param_1 + 0xe) = param_3;
  param_1[6] = 0;
  memset(param_1 + 0x17,0,8);
  param_1[9] = DAT_08018a48;
  param_1[10] = DAT_08018a4c;
  param_1[0xb] = DAT_08018a50;
  uVar1 = DAT_08018a54;
  param_1[8] = param_1;
  param_1[0xc] = uVar1;
  return;
}


