/* 08008c6c FUN_08008c6c; analyst naming is provisional. */

void FUN_08008c6c(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  
  iVar2 = DAT_08008cec;
  iVar1 = DAT_08008ce8;
  if (*param_1 != DAT_08008ce0) {
    return;
  }
  uVar7 = 0;
  *(uint *)(DAT_08008ce4 + 0xd4) = *(uint *)(DAT_08008ce4 + 0xd4) & 0xffffbfff;
  do {
    uVar4 = *(uint *)(iVar1 + 0x60);
    uVar5 = uVar7 & 0xff;
    if (uVar4 == 0) {
      if ((uVar7 & 0xfc) != 0) goto code_r0x0800aa04;
      pbVar6 = *(byte **)(iVar1 + (uVar5 + 0x20) * 4);
      uVar4 = (uint)*pbVar6;
      iVar3 = 0;
      if (uVar4 < 0xb) {
        iVar3 = iVar2 + uVar4 * 0x400;
      }
    }
    else {
      if (5 < uVar5) {
code_r0x0800aa04:
        *(undefined4 *)(DAT_0800aa24 + 0x88) = 0x10000000;
        DataSynchronizationBarrier(0xf);
        InstructionSynchronizationBarrier(0xf);
        return;
      }
      pbVar6 = *(byte **)(iVar1 + (uVar5 + 0x1a) * 4);
      if (*pbVar6 < 0xb) {
        iVar3 = iVar2 + (uint)*pbVar6 * 0x400;
      }
      else {
        iVar3 = 0;
      }
    }
    uVar7 = uVar7 + 1;
    FUN_0800c2f4(iVar3,1 << pbVar6[1] & 0xffff,uVar4,pbVar6,param_4);
  } while( true );
}


