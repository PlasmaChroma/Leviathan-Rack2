/* 08017b38 FUN_08017b38; analyst naming is provisional. */

undefined8 FUN_08017b38(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (DAT_080188d4 == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = 2;
    param_1 = param_2;
  }
  return CONCAT44(param_1,uVar1);
}


