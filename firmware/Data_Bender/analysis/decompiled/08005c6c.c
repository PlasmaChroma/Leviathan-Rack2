/* 08005c6c DaisySeed_CheckBoardVersion; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 DaisySeed_CheckBoardVersion(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  undefined4 uStack_10;
  
  local_30 = 0xff0b;
  uStack_2c = 0;
  local_1c = 0xff0b;
  uStack_18 = 0;
  local_28 = 0;
  uStack_24 = 0;
  local_14 = 0;
  uStack_10 = 0;
  GPIO_Init(&local_1c,0x303,0,1,0);
  GPIO_Init(&local_30,0x403,0,1,0);
  iVar1 = GPIO_Read(&local_1c);
  if (iVar1 != 0) {
    iVar1 = GPIO_Read(&local_30);
    if (iVar1 == 0) {
      uVar2 = 2;
    }
    else {
      uVar2 = 0;
    }
    return uVar2;
  }
  return 1;
}


