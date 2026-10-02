/* 08004a6c DB_Buffer_Init; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Buffer_Init(int param_1,int *param_2,int param_3,uint param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  float fVar7;
  undefined4 uVar8;
  
  iVar6 = DAT_08004be8;
  param_2[0x15] = DAT_08004be8;
  param_2[0x14] = iVar6;
  iVar6 = DAT_08004bec;
  uVar4 = param_4 >> 1;
  param_2[0x13] = param_1;
  *(undefined *)((int)param_2 + 0x11a) = 0;
  param_2[0x43] = iVar6;
  DaisySP_DcBlock_Init();
  param_2[2] = uVar4;
  iVar6 = (uVar4 + 0x3fffffff) * 4;
  *param_2 = param_3;
  param_2[1] = param_4;
  param_2[3] = param_3;
  param_2[4] = uVar4;
  if (iVar6 != 0) {
    memset(param_3,0,iVar6);
  }
  param_2[5] = 1;
  iVar3 = iVar6 + 4 + param_3;
  param_2[0xc] = uVar4;
  param_2[6] = 0;
  param_2[0xb] = iVar3;
  param_2[8] = 0;
  iVar5 = 1 - uVar4 * (1 / uVar4);
  param_2[7] = iVar5;
  if (iVar6 != 0) {
    iVar3 = memset(iVar3,0,iVar6,iVar3,param_5);
  }
  iVar6 = DAT_08004bf0;
  iVar2 = param_2[0x1f];
  fVar7 = (float)param_2[0x26];
  param_2[0x70] = uVar4;
  param_2[0x72] = uVar4;
  param_2[0x77] = uVar4;
  param_2[0x21] = 0x41000000;
  param_2[0x2f] = 0x41000000;
  iVar1 = DAT_08004bf4;
  param_2[0x1e] = iVar2;
  param_2[0x1d] = iVar2;
  param_2[0x1c] = iVar2;
  param_2[0x1b] = iVar2;
  param_2[0xf] = iVar5;
  param_2[0x6f] = param_3;
  param_2[0x10] = iVar6;
  param_2[0x20] = iVar6;
  param_2[0x25] = (int)fVar7;
  param_2[0x24] = (int)fVar7;
  param_2[0x23] = (int)fVar7;
  uVar8 = FPMaxNum(fVar7 + fVar7,iVar6);
  param_2[0x74] = iVar3;
  param_2[0x75] = uVar4;
  iVar3 = param_2[0x2d];
  iVar5 = FPMinNum(uVar8,0x3f800000);
  param_2[0x2c] = iVar3;
  param_2[0x2b] = iVar3;
  param_2[0x2a] = iVar3;
  param_2[0x29] = iVar3;
  param_2[0x2e] = (int)&DAT_c1000000;
  param_2[0x71] = 0;
  param_2[0x73] = 0;
  param_2[0x76] = 0;
  param_2[0x78] = 0;
  param_2[0xd] = 1;
  param_2[0xe] = 0;
  param_2[0x22] = (int)fVar7;
  iVar3 = param_2[0x34];
  param_2[0x36] = iVar1;
  param_2[0x33] = iVar3;
  param_2[0x32] = iVar3;
  param_2[0x31] = iVar3;
  param_2[0x30] = iVar3;
  iVar3 = param_2[0x3b];
  param_2[0x27] = iVar6;
  param_2[0x28] = 0x3f800000;
  param_2[0x35] = iVar6;
  param_2[0x3c] = iVar6;
  param_2[0x3a] = iVar3;
  param_2[0x39] = iVar3;
  param_2[0x38] = iVar3;
  param_2[0x3d] = 0x3f800000;
  param_2[0x37] = 0x3f800000;
  *(undefined2 *)((int)param_2 + 0x22a) = 0;
  param_2[0x1a] = 1;
  *(undefined *)((int)param_2 + 0x111) = 0;
  param_2[0x3f] = 0x3f800000;
  param_2[0x3e] = 0x3f800000;
  param_2[0x79] = iVar5;
  param_2[0x7a] = iVar5;
  param_2[0x45] = 0;
  param_2[0x16] = param_2[0x14];
  return;
}


