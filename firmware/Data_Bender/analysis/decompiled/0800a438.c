/* 0800a438 FUN_0800a438; analyst naming is provisional. */

void FUN_0800a438(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  iVar1 = DAT_0800a598;
  if ((*param_1 == DAT_0800a598) || (iVar1 = DAT_0800a598 + 0x100, *param_1 == iVar1)) {
    uVar2 = *(uint *)(DAT_0800a5a4 + 8);
    if ((uVar2 & 0x30000) == 0) goto LAB_0800a49c;
LAB_0800a456:
    uVar2 = FUN_08010388();
    uVar3 = param_1[1];
    if (uVar3 != 0x20000) {
      if (uVar3 == 0x30000) {
        uVar2 = uVar2 >> 2;
        uVar3 = FUN_08009d18();
        goto joined_r0x0800a552;
      }
      if (uVar3 != 0x10000) goto LAB_0800a474;
    }
    uVar2 = uVar2 / (uVar3 >> 0x10);
    uVar3 = FUN_08009d18();
  }
  else {
    uVar2 = *(uint *)(DAT_0800a59c + 8);
    if ((uVar2 & 0x30000) != 0) goto LAB_0800a456;
LAB_0800a49c:
    uVar2 = FUN_080116d8(0x80000,0,iVar1,uVar2,param_4);
    uVar3 = param_1[1];
    if (uVar3 == 0x240000) {
      uVar2 = uVar2 >> 6;
    }
    else if (uVar3 < 0x240001) {
      if (uVar3 == 0x1c0000) {
        uVar2 = uVar2 >> 4;
      }
      else if (uVar3 < 0x1c0001) {
        if (uVar3 == 0x100000) {
LAB_0800a4fc:
          uVar2 = uVar2 / ((uVar3 >> 0x12) << 1);
        }
        else if (uVar3 < 0x100001) {
          if ((uVar3 == 0x80000) || ((uVar3 & 0xfff7ffff) == 0x40000)) goto LAB_0800a4fc;
        }
        else if ((uVar3 == 0x140000) || (uVar3 == 0x180000)) goto LAB_0800a4fc;
      }
      else if (uVar3 == 0x200000) {
        uVar2 = uVar2 >> 5;
      }
    }
    else if (uVar3 == 0x280000) {
      uVar2 = uVar2 >> 7;
    }
    else if (uVar3 == 0x2c0000) {
      uVar3 = FUN_08009d18();
      if (0x1003 < uVar3) {
        if (DAT_0800a5a8 < uVar2 >> 8) goto LAB_0800a566;
        goto LAB_0800a51e;
      }
      goto LAB_0800a4dc;
    }
LAB_0800a474:
    uVar3 = FUN_08009d18();
  }
joined_r0x0800a552:
  if (0x1003 < uVar3) {
    if (DAT_0800a5a8 < uVar2) {
      if (DAT_0800a5ac < uVar2) {
        iVar1 = *param_1;
        if (uVar2 <= DAT_0800a5b0) {
          *(uint *)(iVar1 + 8) = *(uint *)(iVar1 + 8) & 0xfffffcff | 0x200;
          return;
        }
        *(uint *)(iVar1 + 8) = *(uint *)(iVar1 + 8) | 0x300;
        return;
      }
LAB_0800a566:
      *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xfffffcff | 0x100;
      return;
    }
LAB_0800a51e:
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xfffffcff;
    return;
  }
  if (DAT_0800a5a0 < uVar2) {
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) | 0x100;
    return;
  }
LAB_0800a4dc:
  *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xfffffeff;
  return;
}


