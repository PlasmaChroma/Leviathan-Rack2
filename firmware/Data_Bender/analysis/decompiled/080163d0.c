/* 080163d0 FUN_080163d0; analyst naming is provisional. */

void FUN_080163d0(uint *param_1,uint *param_2)

{
  bool bVar1;
  bool bVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar4 = *param_1;
  bVar1 = param_1 == DAT_080164d0;
  if ((param_1 == (uint *)0x40000000) || (bVar1)) {
    uVar4 = uVar4 & 0xffffff8f | param_2[1];
LAB_08016442:
    bVar2 = param_1 != DAT_080164e0;
    *param_1 = (uVar4 & 0xfffffcff | param_2[3]) & 0xffffff7f | param_2[5];
    param_1[0xb] = param_2[2];
    param_1[10] = *param_2;
    if ((!bVar1) && (bVar2)) goto LAB_08016460;
  }
  else {
    if ((param_1 == DAT_080164d4) || (param_1 == DAT_080164d4 + 0x100)) {
      uVar4 = uVar4 & 0xffffff8f | param_2[1];
      goto LAB_08016442;
    }
    if ((param_1 == DAT_080164d4 + 0x200) || (param_1 == DAT_080164d4 + 0x4000)) {
      uVar4 = uVar4 & 0xffffff8f | param_2[1];
      if ((param_1 == DAT_080164e4) || (param_1 == DAT_080164e0)) goto LAB_08016442;
    }
    if ((param_1 != DAT_080164dc && param_1 != DAT_080164d8) && (param_1 != DAT_080164dc + 0x100)) {
      uVar5 = param_2[2];
      uVar3 = *param_2;
      *param_1 = uVar4 & 0xffffff7f | param_2[5];
      param_1[0xb] = uVar5;
      param_1[10] = uVar3;
      goto LAB_08016478;
    }
    uVar5 = param_2[2];
    uVar3 = *param_2;
    *param_1 = (uVar4 & 0xfffffcff | param_2[3]) & 0xffffff7f | param_2[5];
    param_1[0xb] = uVar5;
    param_1[10] = uVar3;
LAB_08016460:
    if ((param_1 != DAT_080164dc && param_1 != DAT_080164d8) && (param_1 != DAT_080164dc + 0x100))
    goto LAB_08016478;
  }
  param_1[0xc] = param_2[4];
LAB_08016478:
  param_1[5] = 1;
  return;
}


