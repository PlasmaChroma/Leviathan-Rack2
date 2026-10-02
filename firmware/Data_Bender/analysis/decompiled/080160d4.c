/* 080160d4 FUN_080160d4; analyst naming is provisional. */

int FUN_080160d4(int *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  
  if (*(char *)(param_1 + 0xf) == '\x01') {
    return 2;
  }
  iVar5 = *param_1;
  iVar1 = 1;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  *(undefined *)(param_1 + 0xf) = 1;
  *(uint *)(iVar5 + 8) = DAT_0801625c & *(uint *)(iVar5 + 8);
  uVar2 = DAT_08016260;
  uVar3 = *param_2;
  if (uVar3 == 0x70) {
    iVar1 = 0;
    *(uint *)(iVar5 + 8) =
         param_2[2] | param_2[1] | param_2[3] << 8 | *(uint *)(iVar5 + 8) & 0xffff00ff;
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 8) | 0x77;
    goto LAB_0801613e;
  }
  if (uVar3 < 0x71) {
    if (uVar3 == 0x50) {
      uVar4 = *(uint *)(iVar5 + 0x20);
      uVar3 = param_2[1];
      uVar6 = param_2[3];
      *(uint *)(iVar5 + 0x20) = *(uint *)(iVar5 + 0x20) & 0xfffffffe;
      *(uint *)(iVar5 + 0x18) = *(uint *)(iVar5 + 0x18) & 0xffffff0f | uVar6 << 4;
      *(uint *)(iVar5 + 0x20) = uVar3 | uVar4 & 0xfffffff5;
      iVar1 = 0;
      *(uint *)(iVar5 + 8) = uVar2 & *(uint *)(iVar5 + 8) | 0x57;
      goto LAB_0801613e;
    }
    if (0x50 < uVar3) {
      if (uVar3 == 0x60) {
        uVar3 = param_2[1];
        uVar2 = param_2[3];
        *(uint *)(iVar5 + 0x20) = *(uint *)(iVar5 + 0x20) & 0xffffffef;
        iVar1 = 0;
        *(uint *)(iVar5 + 0x18) = *(uint *)(iVar5 + 0x18) & 0xffff0fff | uVar2 << 0xc;
        uVar2 = DAT_08016260;
        *(uint *)(iVar5 + 0x20) = *(uint *)(iVar5 + 0x20) & 0xffffff5f | uVar3 << 4;
        *(uint *)(iVar5 + 8) = uVar2 & *(uint *)(iVar5 + 8) | 0x67;
      }
      goto LAB_0801613e;
    }
    if (uVar3 == 0x40) {
      uVar4 = *(uint *)(iVar5 + 0x20);
      uVar3 = param_2[1];
      uVar6 = param_2[3];
      *(uint *)(iVar5 + 0x20) = *(uint *)(iVar5 + 0x20) & 0xfffffffe;
      *(uint *)(iVar5 + 0x18) = *(uint *)(iVar5 + 0x18) & 0xffffff0f | uVar6 << 4;
      *(uint *)(iVar5 + 0x20) = uVar3 | uVar4 & 0xfffffff5;
      iVar1 = 0;
      *(uint *)(iVar5 + 8) = uVar2 & *(uint *)(iVar5 + 8) | 0x47;
      goto LAB_0801613e;
    }
    if (0x40 < uVar3) goto LAB_0801613e;
    if (uVar3 != 0x20) {
      if (uVar3 < 0x21) {
        if ((uVar3 & 0xffffffef) != 0) goto LAB_0801613e;
      }
      else if (uVar3 != 0x30) {
        iVar1 = 1;
        goto LAB_0801613e;
      }
    }
  }
  else {
    if (uVar3 == 0x2000) {
      iVar1 = 0;
      *(uint *)(iVar5 + 8) =
           param_2[2] | param_2[1] | param_2[3] << 8 | *(uint *)(iVar5 + 8) & 0xffff00ff;
      *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 8) | 0x4000;
      goto LAB_0801613e;
    }
    if (uVar3 < 0x2001) {
      iVar1 = uVar3 - 0x1000;
      if (iVar1 != 0) {
        iVar1 = 1;
      }
      goto LAB_0801613e;
    }
    if (uVar3 != DAT_08016264) {
      if (DAT_08016264 < uVar3) {
        if ((uVar3 != DAT_08016268) && (uVar3 != DAT_08016268 + 0x10)) goto LAB_0801613e;
      }
      else if ((uVar3 & 0xffffffef) != 0x100000) goto LAB_0801613e;
    }
  }
  iVar1 = 0;
  *(uint *)(iVar5 + 8) = uVar3 | DAT_08016260 & *(uint *)(iVar5 + 8) | 7;
LAB_0801613e:
  *(undefined *)((int)param_1 + 0x3d) = 1;
  *(undefined *)(param_1 + 0xf) = 0;
  return iVar1;
}


