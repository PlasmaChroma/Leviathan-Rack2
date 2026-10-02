/* 08006ebc AudioHandle_Init_single_SAI; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 AudioHandle_Init_single_SAI(int *param_1,undefined4 *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  float fVar6;
  
  iVar1 = DAT_08006f20;
  *param_1 = DAT_08006f20;
  uVar3 = param_2[1];
  uVar4 = param_2[2];
  uVar5 = param_2[3];
  *(undefined4 *)(iVar1 + 8) = *param_2;
  *(undefined4 *)(iVar1 + 0xc) = uVar3;
  *(undefined4 *)(iVar1 + 0x10) = uVar4;
  *(undefined4 *)(iVar1 + 0x14) = uVar5;
  fVar6 = *(float *)(iVar1 + 0x10);
  if (fVar6 != 0.0 && fVar6 < 0.0 == NAN(fVar6)) {
    *(float *)(iVar1 + 0x34) = *(float *)(iVar1 + 0x14) * fVar6;
    *(float *)(iVar1 + 0x30) = 1.0 / fVar6;
    if (param_3 != 0) {
      *(int *)(iVar1 + 0x18) = param_3;
      iVar2 = SaiHandle_GetConfig((int *)(iVar1 + 0x18));
      uVar3 = DAT_08006f28;
      uVar4 = *(undefined4 *)(iVar2 + 0x10);
      *(undefined4 *)(iVar1 + 0x20) = DAT_08006f24;
      *(undefined4 *)(iVar1 + 0x28) = uVar3;
      *(undefined4 *)(iVar1 + 0xc) = uVar4;
      return 0;
    }
  }
  return 1;
}


