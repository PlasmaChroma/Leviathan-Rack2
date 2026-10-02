/* 08005f38 Wm8731_WriteRegister; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 Wm8731_WriteRegister(int param_1,int param_2,int param_3)

{
  int iVar1;
  byte local_c;
  undefined local_b;
  
  local_b = (undefined)param_3;
  local_c = (byte)((uint)(param_3 << 0x17) >> 0x1f) | (byte)(param_2 << 1);
  iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 0x10),&local_c,2,0xfa);
  if (iVar1 != 0) {
    return 1;
  }
  System_Delay(10);
  return 0;
}


