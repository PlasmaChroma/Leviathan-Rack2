/* 080178f8 DaisySP_Compressor_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Compressor_Init(float param_1,undefined4 *param_2)

{
  float fVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  float fVar5;
  undefined4 uVar6;
  float fVar7;
  undefined *puVar8;
  
  fVar1 = DAT_080179ec;
  if (param_1 == 1.0 || param_1 < 1.0 != NAN(param_1)) {
    iVar3 = 1;
    fVar5 = 1.0;
    fVar7 = 2.0;
    puVar8 = &UNK_c1200000;
  }
  else {
    iVar3 = DAT_080179e8;
    fVar5 = DAT_080179e4;
    fVar7 = DAT_080179e0;
    puVar8 = DAT_080179dc;
    if (param_1 == DAT_080179d8 || param_1 < DAT_080179d8 != (NAN(param_1) || NAN(DAT_080179d8))) {
      fVar7 = (float)(longlong)(int)param_1;
      fVar5 = 1.0 / fVar7;
      iVar3 = (int)param_1;
      fVar7 = 2.0 / fVar7;
      puVar8 = (undefined *)-(fVar5 / DAT_080179ec);
    }
  }
  param_2[0xc] = iVar3;
  *param_2 = 0x40000000;
  param_2[0xe] = fVar5;
  param_2[0xd] = fVar7;
  param_2[2] = fVar1;
  uVar4 = libm_expf(puVar8);
  param_2[10] = uVar4;
  fVar5 = (float)libm_expf(-(fVar7 / fVar1));
  param_2[3] = fVar1;
  param_2[8] = fVar5;
  param_2[9] = (1.0 - fVar5) * -0.5;
  uVar6 = libm_expf(puVar8);
  uVar2 = DAT_080179f4;
  uVar4 = DAT_080179f0;
  param_2[7] = fVar1;
  param_2[6] = fVar1;
  param_2[0xb] = uVar6;
  param_2[1] = uVar4;
  *(undefined *)(param_2 + 0xf) = 1;
  param_2[4] = uVar2;
  return;
}


