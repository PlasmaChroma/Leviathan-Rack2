/* 08007dfc FUN_08007dfc; analyst naming is provisional. */

void FUN_08007dfc(int *param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  int iVar8;
  
  uVar7 = DAT_08007fa8;
  uVar6 = DAT_08007fa4;
  uVar5 = DAT_08007fa0;
  iVar4 = DAT_08007f9c;
  iVar3 = DAT_08007f98;
  iVar2 = DAT_08007f90;
  iVar1 = DAT_08007f8c;
  iVar8 = *param_1;
  if (iVar8 == DAT_08007f8c) {
    *(uint *)(DAT_08007f9c + 0xe0) = *(uint *)(DAT_08007f9c + 0xe0) | 2;
    FUN_08007d8c(uVar6,param_2,iVar1,*(uint *)(iVar4 + 0xe0) & 2);
    *(uint *)(iVar4 + 0xe8) = *(uint *)(iVar4 + 0xe8) | 0x200000;
    *(uint *)(iVar4 + 0xd8) = *(uint *)(iVar4 + 0xd8) | 1;
    FUN_0800a968(0x1f,0,0,*(uint *)(iVar4 + 0xd8) & 1);
    FUN_0800a9e4(0x1f);
    return;
  }
  if (iVar8 != DAT_08007f90) {
    if (iVar8 != DAT_08007f94) {
      if (iVar8 == DAT_08007f98) {
        *(uint *)(DAT_08007f9c + 0xe0) = *(uint *)(DAT_08007f9c + 0xe0) | 2;
        FUN_08007d8c(uVar5,param_2,iVar3,*(uint *)(iVar4 + 0xe0) & 2);
        *(uint *)(iVar4 + 0xf4) = *(uint *)(iVar4 + 0xf4) | 0x80;
      }
      return;
    }
    FUN_08007d8c(DAT_08007fac);
    iVar1 = DAT_08007f9c;
    *(uint *)(DAT_08007f9c + 0xe8) = *(uint *)(DAT_08007f9c + 0xe8) | 0x800000;
    *(uint *)(iVar1 + 0xd8) = *(uint *)(iVar1 + 0xd8) | 1;
    FUN_0800a968(0x48,0,0,*(uint *)(iVar1 + 0xd8) & 1);
    FUN_0800a9e4(0x48);
    return;
  }
  *(uint *)(DAT_08007f9c + 0xe0) = *(uint *)(DAT_08007f9c + 0xe0) | 0x80;
  *(uint *)(iVar4 + 0xe0) = *(uint *)(iVar4 + 0xe0) | 2;
  FUN_08007d8c(uVar7,param_2,iVar2,*(uint *)(iVar4 + 0xe0) & 2);
  *(uint *)(iVar4 + 0xe8) = *(uint *)(iVar4 + 0xe8) | 0x400000;
  *(uint *)(iVar4 + 0xd8) = *(uint *)(iVar4 + 0xd8) | 1;
  FUN_0800a968(0x21,0,0,*(uint *)(iVar4 + 0xd8) & 1);
  FUN_0800a9e4(0x21);
  return;
}


