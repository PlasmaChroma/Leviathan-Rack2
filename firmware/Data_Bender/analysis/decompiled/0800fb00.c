/* 0800fb00 FUN_0800fb00; analyst naming is provisional. */

undefined4 FUN_0800fb00(int *param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  puVar2 = DAT_08010070;
  puVar1 = DAT_0800fda8;
  if (param_1 == (int *)0x0) {
    return 1;
  }
  iVar6 = *param_1;
  if (iVar6 << 0x1f < 0) {
    if (((DAT_0800fda8[4] & 0x38) == 0x10) ||
       (((DAT_0800fda8[4] & 0x38) == 0x18 && ((DAT_0800fda8[10] & 3) == 2)))) {
      if (((int)(*DAT_0800fda8 << 0xe) < 0) && (param_1[1] == 0)) {
        return 1;
      }
    }
    else {
      iVar6 = param_1[1];
      if (iVar6 == 0x10000) {
        *DAT_0800fda8 = *DAT_0800fda8 | 0x10000;
LAB_0800fb50:
        iVar6 = FUN_08009ce8();
        puVar1 = DAT_0800fda8;
        while (-1 < (int)(*puVar1 << 0xe)) {
          iVar4 = FUN_08009ce8();
          if (5000 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
      }
      else {
        if (iVar6 != 0) {
          if (iVar6 == 0x50000) {
            *DAT_0800fda8 = *DAT_0800fda8 | 0x40000;
            *puVar1 = *puVar1 | 0x10000;
          }
          else {
            *DAT_0800fda8 = *DAT_0800fda8 & 0xfffeffff;
            *puVar1 = *puVar1 & 0xfffbffff;
          }
          goto LAB_0800fb50;
        }
        *DAT_08010070 = *DAT_08010070 & 0xfffeffff;
        *puVar2 = *puVar2 & 0xfffbffff;
        iVar6 = FUN_08009ce8();
        while ((int)(*puVar2 << 0xe) < 0) {
          iVar4 = FUN_08009ce8();
          if (5000 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
      }
      iVar6 = *param_1;
    }
  }
  puVar1 = DAT_0800fda8;
  if (iVar6 << 0x1e < 0) {
    if (((DAT_0800fda8[4] & 0x38) == 0) ||
       (((DAT_0800fda8[4] & 0x38) == 0x18 && ((DAT_0800fda8[10] & 3) == 0)))) {
      if (((int)(*DAT_0800fda8 << 0x1d) < 0) && (param_1[3] == 0)) {
        return 1;
      }
      *DAT_0800fda8 = *DAT_0800fda8 & 0xffffffe6 | param_1[3];
      iVar6 = FUN_08009ce8();
      while (-1 < (int)(*puVar1 << 0x1d)) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    else {
      if (param_1[3] == 0) {
        *DAT_0800fda8 = *DAT_0800fda8 & 0xfffffffe;
        iVar6 = FUN_08009ce8();
        while ((int)(*puVar1 << 0x1d) < 0) {
          iVar4 = FUN_08009ce8();
          if (2 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
        iVar6 = *param_1;
        goto LAB_0800fb78;
      }
      *DAT_0800fda8 = *DAT_0800fda8 & 0xffffffe6 | param_1[3];
      iVar6 = FUN_08009ce8();
      while (-1 < (int)(*puVar1 << 0x1d)) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    uVar3 = FUN_08009d18();
    if (uVar3 < 0x1004) {
      iVar6 = param_1[4];
      uVar3 = puVar1[1] & 0xfffc0fff;
      if (iVar6 == 0x40) {
        uVar3 = uVar3 | 0x20000;
      }
      if (iVar6 != 0x40) {
        uVar3 = uVar3 | iVar6 << 0xc;
      }
      puVar1[1] = uVar3;
      iVar6 = *param_1;
    }
    else {
      puVar1[1] = puVar1[1] & 0x80ffffff | param_1[4] << 0x18;
      iVar6 = *param_1;
    }
  }
LAB_0800fb78:
  puVar1 = DAT_0800fda8;
  if (iVar6 << 0x1b < 0) {
    if (((DAT_0800fda8[4] & 0x38) == 8) ||
       (((DAT_0800fda8[4] & 0x38) == 0x18 && ((DAT_0800fda8[10] & 3) == 1)))) {
      if (((int)(*DAT_0800fda8 << 0x17) < 0) && (param_1[7] != 0x80)) {
        return 1;
      }
      uVar3 = FUN_08009d18();
      if (uVar3 < 0x1004) {
        if (param_1[8] == 0x20) {
          *(uint *)(DAT_080100f0 + 4) = *(uint *)(DAT_080100f0 + 4) & 0x83ffffff | 0x40000000;
          iVar6 = *param_1;
        }
        else {
          DAT_0800fda8[1] = DAT_0800fda8[1] & 0x83ffffff | param_1[8] << 0x1a;
          iVar6 = *param_1;
        }
      }
      else {
        DAT_08010070[3] = DAT_08010070[3] & 0xc0ffffff | param_1[8] << 0x18;
        iVar6 = *param_1;
      }
    }
    else if (param_1[7] == 0) {
      *DAT_0800fda8 = *DAT_0800fda8 & 0xffffff7f;
      iVar6 = FUN_08009ce8();
      while ((int)(*puVar1 << 0x17) < 0) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
      iVar6 = *param_1;
    }
    else {
      *DAT_0800fda8 = *DAT_0800fda8 | 0x80;
      iVar6 = FUN_08009ce8();
      while (-1 < (int)(*puVar1 << 0x17)) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
      uVar3 = FUN_08009d18();
      if (uVar3 < 0x1004) {
        iVar6 = param_1[8];
        uVar3 = puVar1[1] & 0x83ffffff;
        if (iVar6 == 0x20) {
          uVar3 = uVar3 | 0x40000000;
        }
        if (iVar6 != 0x20) {
          uVar3 = uVar3 | iVar6 << 0x1a;
        }
        puVar1[1] = uVar3;
        iVar6 = *param_1;
      }
      else {
        puVar1[3] = puVar1[3] & 0xc0ffffff | param_1[8] << 0x18;
        iVar6 = *param_1;
      }
    }
  }
  puVar1 = DAT_0800fda8;
  if (iVar6 << 0x1c < 0) {
    if (param_1[5] == 0) {
      DAT_0800fda8[0x1d] = DAT_0800fda8[0x1d] & 0xfffffffe;
      iVar6 = FUN_08009ce8();
      while ((int)(puVar1[0x1d] << 0x1e) < 0) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    else {
      DAT_0800fda8[0x1d] = DAT_0800fda8[0x1d] | 1;
      iVar6 = FUN_08009ce8();
      while (-1 < (int)(puVar1[0x1d] << 0x1e)) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    iVar6 = *param_1;
  }
  puVar1 = DAT_0800fda8;
  if (iVar6 << 0x1a < 0) {
    if (param_1[6] == 0) {
      *DAT_0800fda8 = *DAT_0800fda8 & 0xffffefff;
      iVar6 = FUN_08009ce8();
      while ((int)(*puVar1 << 0x12) < 0) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    else {
      *DAT_0800fda8 = *DAT_0800fda8 | 0x1000;
      iVar6 = FUN_08009ce8();
      while (-1 < (int)(*puVar1 << 0x12)) {
        iVar4 = FUN_08009ce8();
        if (2 < (uint)(iVar4 - iVar6)) {
          return 3;
        }
      }
    }
    iVar6 = *param_1;
  }
  puVar1 = DAT_0800fdac;
  if (iVar6 << 0x1d < 0) {
    *DAT_0800fdac = *DAT_0800fdac | 0x100;
    iVar6 = FUN_08009ce8();
    while (iVar4 = DAT_080100f0, puVar2 = DAT_08010070, -1 < (int)(*puVar1 << 0x17)) {
      iVar4 = FUN_08009ce8();
      if (100 < (uint)(iVar4 - iVar6)) {
        return 3;
      }
    }
    iVar6 = param_1[2];
    if (iVar6 == 1) {
      *(uint *)(DAT_080100f0 + 0x70) = *(uint *)(DAT_080100f0 + 0x70) | 1;
    }
    else {
      if (iVar6 == 0) {
        *(uint *)(DAT_080100f0 + 0x70) = *(uint *)(DAT_080100f0 + 0x70) & 0xfffffffe;
        *(uint *)(iVar4 + 0x70) = *(uint *)(iVar4 + 0x70) & 0xfffffffb;
        iVar6 = FUN_08009ce8();
        while (*(int *)(iVar4 + 0x70) << 0x1e < 0) {
          iVar5 = FUN_08009ce8();
          if (5000 < (uint)(iVar5 - iVar6)) {
            return 3;
          }
        }
        goto LAB_0800fc4e;
      }
      if (iVar6 == 5) {
        DAT_08010070[0x1c] = DAT_08010070[0x1c] | 4;
        puVar2[0x1c] = puVar2[0x1c] | 1;
      }
      else {
        DAT_08010070[0x1c] = DAT_08010070[0x1c] & 0xfffffffe;
        puVar2[0x1c] = puVar2[0x1c] & 0xfffffffb;
      }
    }
    iVar6 = FUN_08009ce8();
    puVar1 = DAT_08010070;
    while (-1 < (int)(puVar1[0x1c] << 0x1e)) {
      iVar4 = FUN_08009ce8();
      if (5000 < (uint)(iVar4 - iVar6)) {
        return 3;
      }
    }
  }
LAB_0800fc4e:
  puVar1 = DAT_0800fda8;
  iVar6 = param_1[9];
  if (iVar6 != 0) {
    if ((DAT_0800fda8[4] & 0x38) == 0x18) {
      uVar3 = DAT_0800fda8[0xc];
      if (iVar6 == 1) {
        return 1;
      }
      if ((DAT_0800fda8[10] & 3) != param_1[10]) {
        return 1;
      }
      if ((DAT_0800fda8[10] << 0x16) >> 0x1a != param_1[0xb]) {
        return 1;
      }
      if ((uVar3 & 0x1ff) != param_1[0xc] - 1U) {
        return 1;
      }
      if ((uVar3 << 0x10) >> 0x19 != param_1[0xd] - 1U) {
        return 1;
      }
      if ((uVar3 << 9) >> 0x19 != param_1[0xe] - 1U) {
        return 1;
      }
      if ((uVar3 << 1) >> 0x19 != param_1[0xf] - 1U) {
        return 1;
      }
      if (param_1[0x12] != (DAT_0800fda8[0xd] << 0x10) >> 0x13) {
        DAT_08010070[0xb] = DAT_08010070[0xb] & 0xfffffffe;
        iVar6 = FUN_08009ce8();
        do {
          iVar4 = FUN_08009ce8();
          puVar1 = DAT_08010070;
        } while (iVar4 == iVar6);
        DAT_08010070[0xd] = DAT_08010078 & DAT_08010070[0xd] | param_1[0x12] << 3;
        puVar1[0xb] = puVar1[0xb] | 1;
        return 0;
      }
    }
    else {
      *DAT_0800fda8 = *DAT_0800fda8 & 0xfeffffff;
      if (iVar6 == 2) {
        iVar6 = FUN_08009ce8();
        while (uVar3 = DAT_08010078, puVar2 = DAT_08010070, (int)(*puVar1 << 6) < 0) {
          iVar4 = FUN_08009ce8();
          if (2 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
        puVar1[10] = DAT_08010074 & puVar1[10] | param_1[10] | param_1[0xb] << 4;
        puVar1[0xc] = (param_1[0xd] + -1) * 0x200 & 0xffffU |
                      (param_1[0xe] + -1) * 0x10000 & 0x7f0000U | param_1[0xc] - 1U & 0x1ff |
                      (param_1[0xf] + -1) * 0x1000000 & 0x7f000000U;
        puVar1[0xb] = puVar1[0xb] & 0xfffffffe;
        puVar1[0xd] = uVar3 & puVar1[0xd] | param_1[0x12] << 3;
        puVar1[0xb] = puVar1[0xb] & 0xfffffff3 | param_1[0x10];
        puVar1[0xb] = puVar1[0xb] & 0xfffffffd | param_1[0x11];
        puVar1[0xb] = puVar1[0xb] | 0x10000;
        puVar1[0xb] = puVar1[0xb] | 0x20000;
        puVar1[0xb] = puVar1[0xb] | 0x40000;
        puVar1[0xb] = puVar1[0xb] | 1;
        *puVar1 = *puVar1 | 0x1000000;
        iVar6 = FUN_08009ce8();
        while (-1 < (int)(*puVar2 << 6)) {
          iVar4 = FUN_08009ce8();
          if (2 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
      }
      else {
        iVar6 = FUN_08009ce8();
        while ((int)(*puVar1 << 6) < 0) {
          iVar4 = FUN_08009ce8();
          if (2 < (uint)(iVar4 - iVar6)) {
            return 3;
          }
        }
      }
    }
  }
  return 0;
}


