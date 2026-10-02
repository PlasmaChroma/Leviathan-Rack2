/* 08017a90 DaisySP_Tone_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Tone_Init(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_08017aa8;
  *(undefined4 *)(param_2 + 4) = 0;
  *(undefined4 *)(param_2 + 0xc) = uVar1;
  *(undefined4 *)(param_2 + 0x10) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x14) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x18) = param_1;
  return;
}


