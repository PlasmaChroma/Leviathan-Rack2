/* 08008d3c SaiHandle_Impl_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_Impl_Init(int *param_1,int *param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int aiStack_28 [5];
  
  iVar3 = *param_2;
  if (1 < iVar3) {
    return 1;
  }
  param_1[0x92] = 0;
  param_1[0x93] = 0;
  param_1[0x94] = 0;
  aiStack_28[0] = DAT_08008ea8;
  aiStack_28[1] = DAT_08008ea8 + 0x400;
  iVar7 = aiStack_28[iVar3];
  aiStack_28[2] = DAT_08008ea8 + 0x20;
  aiStack_28[3] = DAT_08008ea8 + 0x420;
  iVar6 = aiStack_28[iVar3 + 2];
  iVar3 = param_2[1];
  iVar2 = param_2[2];
  iVar4 = param_2[3];
  *param_1 = *param_2;
  param_1[1] = iVar3;
  param_1[2] = iVar2;
  param_1[3] = iVar4;
  iVar3 = param_2[5];
  iVar2 = param_2[6];
  iVar4 = param_2[7];
  param_1[4] = param_2[4];
  param_1[5] = iVar3;
  param_1[6] = iVar2;
  param_1[7] = iVar4;
  iVar3 = param_2[9];
  iVar2 = param_2[4];
  param_1[8] = param_2[8];
  param_1[9] = iVar3;
  param_1[10] = iVar7;
  param_1[0x30] = iVar6;
  iVar3 = DAT_08008eac;
  switch(iVar2) {
  case 0:
    param_1[0x12] = 8000;
    param_1[0x38] = 8000;
    break;
  case 1:
    param_1[0x12] = 16000;
    param_1[0x38] = 16000;
    break;
  case 2:
    param_1[0x12] = 32000;
    param_1[0x38] = 32000;
    break;
  case 3:
    param_1[0x12] = 48000;
    param_1[0x38] = 48000;
    break;
  case 4:
    param_1[0x12] = DAT_08008eac;
    param_1[0x38] = iVar3;
  }
  iVar3 = param_2[8];
  if (param_2[6] == 0) {
    param_1[0xc] = 0;
    iVar2 = param_2[7];
    if (iVar3 != 0) {
      iVar3 = 1;
    }
    param_1[0xb] = iVar3;
    iVar4 = param_2[9];
  }
  else {
    iVar4 = param_2[9];
    param_1[0xc] = 1;
    if (iVar3 == 0) {
      iVar3 = 2;
    }
    else {
      iVar3 = 3;
    }
    param_1[0xb] = iVar3;
    iVar2 = param_2[7];
  }
  if (iVar2 == 0) {
    iVar3 = param_2[5];
    param_1[0x32] = 0;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
    param_1[0x31] = iVar4;
  }
  else {
    iVar3 = param_2[5];
    param_1[0x32] = 1;
    if (iVar4 == 0) {
      iVar2 = 2;
    }
    else {
      iVar2 = 3;
    }
    param_1[0x31] = iVar2;
  }
  if (iVar3 == 1) {
    uVar5 = 2;
    uVar1 = 1;
  }
  else if (iVar3 == 2) {
    uVar1 = 0;
    uVar5 = 3;
  }
  else {
    uVar1 = 0;
    uVar5 = uVar1;
  }
  param_1[0x11] = 0;
  param_1[0xd] = 0;
  param_1[0x17] = 0;
  param_1[0x37] = 0;
  param_1[0x33] = 0;
  param_1[0x3d] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = 0;
  param_1[0x15] = 0;
  param_1[0x16] = 0;
  param_1[0x35] = 0;
  param_1[0x36] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  iVar3 = HAL_SAI_InitProtocol(param_1 + 10,uVar1,uVar5,2);
  if (iVar3 == 0) {
    iVar3 = HAL_SAI_InitProtocol(param_1 + 0x30,uVar1,uVar5,2);
    if (iVar3 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    return 0;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


