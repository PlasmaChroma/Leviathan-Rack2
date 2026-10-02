/* 080179f8 DaisySP_ATone_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_ATone_Init(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_08017a0c;
  *(undefined4 *)(param_2 + 4) = 0;
  *(undefined4 *)(param_2 + 0xc) = uVar1;
  *(undefined4 *)(param_2 + 0x10) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x14) = param_1;
  return;
}


