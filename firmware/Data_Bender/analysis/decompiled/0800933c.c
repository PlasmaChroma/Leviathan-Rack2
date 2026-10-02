/* 0800933c SaiHandle_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_Init(int **param_1,int *param_2)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  int local_28 [5];
  
  piVar1 = (int *)(*param_2 * 0x25c + DAT_08009350);
  *param_1 = piVar1;
  iVar4 = *param_2;
  if (1 < iVar4) {
    return 1;
  }
  piVar1[0x92] = 0;
  piVar1[0x93] = 0;
  piVar1[0x94] = 0;
  local_28[0] = DAT_08008ea8;
  local_28[1] = DAT_08008ea8 + 0x400;
  iVar8 = local_28[iVar4];
  local_28[2] = DAT_08008ea8 + 0x20;
  local_28[3] = DAT_08008ea8 + 0x420;
  iVar7 = local_28[iVar4 + 2];
  iVar4 = param_2[1];
  iVar3 = param_2[2];
  iVar5 = param_2[3];
  *piVar1 = *param_2;
  piVar1[1] = iVar4;
  piVar1[2] = iVar3;
  piVar1[3] = iVar5;
  iVar4 = param_2[5];
  iVar3 = param_2[6];
  iVar5 = param_2[7];
  piVar1[4] = param_2[4];
  piVar1[5] = iVar4;
  piVar1[6] = iVar3;
  piVar1[7] = iVar5;
  iVar4 = param_2[9];
  iVar3 = param_2[4];
  piVar1[8] = param_2[8];
  piVar1[9] = iVar4;
  piVar1[10] = iVar8;
  piVar1[0x30] = iVar7;
  iVar4 = DAT_08008eac;
  switch(iVar3) {
  case 0:
    piVar1[0x12] = 8000;
    piVar1[0x38] = 8000;
    break;
  case 1:
    piVar1[0x12] = 16000;
    piVar1[0x38] = 16000;
    break;
  case 2:
    piVar1[0x12] = 32000;
    piVar1[0x38] = 32000;
    break;
  case 3:
    piVar1[0x12] = 48000;
    piVar1[0x38] = 48000;
    break;
  case 4:
    piVar1[0x12] = DAT_08008eac;
    piVar1[0x38] = iVar4;
  }
  iVar4 = param_2[8];
  if (param_2[6] == 0) {
    piVar1[0xc] = 0;
    iVar3 = param_2[7];
    if (iVar4 != 0) {
      iVar4 = 1;
    }
    piVar1[0xb] = iVar4;
    iVar5 = param_2[9];
  }
  else {
    iVar5 = param_2[9];
    piVar1[0xc] = 1;
    if (iVar4 == 0) {
      iVar4 = 2;
    }
    else {
      iVar4 = 3;
    }
    piVar1[0xb] = iVar4;
    iVar3 = param_2[7];
  }
  if (iVar3 == 0) {
    iVar4 = param_2[5];
    piVar1[0x32] = 0;
    if (iVar5 != 0) {
      iVar5 = 1;
    }
    piVar1[0x31] = iVar5;
  }
  else {
    iVar4 = param_2[5];
    piVar1[0x32] = 1;
    if (iVar5 == 0) {
      iVar3 = 2;
    }
    else {
      iVar3 = 3;
    }
    piVar1[0x31] = iVar3;
  }
  if (iVar4 == 1) {
    uVar6 = 2;
    uVar2 = 1;
  }
  else if (iVar4 == 2) {
    uVar2 = 0;
    uVar6 = 3;
  }
  else {
    uVar2 = 0;
    uVar6 = uVar2;
  }
  piVar1[0x11] = 0;
  piVar1[0xd] = 0;
  piVar1[0x17] = 0;
  piVar1[0x37] = 0;
  piVar1[0x33] = 0;
  piVar1[0x3d] = 0;
  piVar1[0xf] = 0;
  piVar1[0x10] = 0;
  piVar1[0x15] = 0;
  piVar1[0x16] = 0;
  piVar1[0x35] = 0;
  piVar1[0x36] = 0;
  piVar1[0x3b] = 0;
  piVar1[0x3c] = 0;
  iVar4 = HAL_SAI_InitProtocol(piVar1 + 10,uVar2,uVar6,2);
  if (iVar4 == 0) {
    iVar4 = HAL_SAI_InitProtocol(piVar1 + 0x30,uVar2,uVar6,2);
    if (iVar4 != 0) {
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


