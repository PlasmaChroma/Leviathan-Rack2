/* 08008034 I2CHandle_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 I2CHandle_Init(int **param_1,int *param_2)

{
  byte bVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int aiStack_20 [4];
  
  piVar3 = (int *)(DAT_08008048 + *param_2 * 0xe0);
  *param_1 = piVar3;
  piVar6 = DAT_08007ab0;
  iVar5 = *param_2;
  if (3 < iVar5) {
    return 1;
  }
  if (((uint)param_2[3] < 2) && ((param_2[3] != 1 || (*(byte *)(param_2 + 4) - 0x10 < 0x68)))) {
    iVar7 = *param_2;
    iVar2 = param_2[1];
    iVar4 = param_2[3];
    piVar3[2] = param_2[2];
    *piVar3 = iVar7;
    piVar3[1] = iVar2;
    piVar3[3] = iVar4;
    iVar2 = piVar3[2];
    *(undefined *)(piVar3 + 4) = *(undefined *)(param_2 + 4);
    aiStack_20[3] = piVar6[3];
    aiStack_20[2] = piVar6[2];
    aiStack_20[1] = piVar6[1];
    aiStack_20[0] = *piVar6;
    piVar3[0x23] = aiStack_20[iVar5];
    if (iVar2 == 1) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ab8;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007abc;
      }
      piVar3[0x24] = iVar5;
    }
    else if (iVar2 == 2) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ac8;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007acc;
      }
      piVar3[0x24] = iVar5;
    }
    else if (iVar2 == 0) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ac0;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007ac4;
      }
      piVar3[0x24] = iVar5;
    }
    bVar1 = *(byte *)(param_2 + 4);
    piVar6 = piVar3 + 0x23;
    piVar3[0x28] = 0;
    piVar3[0x25] = (uint)bVar1 << 1;
    piVar3[0x2b] = 0;
    piVar3[0x26] = 1;
    piVar3[0x27] = 0;
    piVar3[0x29] = 0;
    piVar3[0x2a] = 0;
    iVar5 = FUN_0800cf80(piVar6);
    if ((iVar5 == 0) && (iVar5 = FUN_0800e630(piVar6,0), iVar5 == 0)) {
      iVar5 = FUN_0800e684(piVar6,0);
      if (iVar5 == 0) {
        return 0;
      }
      return 1;
    }
  }
  return 1;
}


