/* 080061d0 SdramHandle_InitSequence; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SdramHandle_InitSequence(void)

{
  undefined4 uVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  uVar1 = DAT_0800625c;
  local_28 = 1;
  local_24 = 0x10;
  local_20 = 1;
  local_1c = 0;
  FUN_080122f0(DAT_0800625c,&local_28,0x1000);
  FUN_08009cf4(100);
  local_28 = 2;
  local_1c = 0;
  local_24 = 0x10;
  local_20 = 1;
  FUN_080122f0(uVar1,&local_28,0x1000);
  local_28 = 3;
  local_24 = 0x10;
  local_1c = 0;
  local_20 = 4;
  FUN_080122f0(uVar1,&local_28,0x1000);
  local_28 = 4;
  local_1c = 0x232;
  local_24 = 0x10;
  local_20 = 1;
  FUN_080122f0(uVar1,&local_28,0x1000);
  FUN_08012330(uVar1,0x806);
  return 0;
}


