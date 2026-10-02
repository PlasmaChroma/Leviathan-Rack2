/* 080167e8 FUN_080167e8; analyst naming is provisional. */

uint ** FUN_080167e8(uint **param_1)

{
  bool bVar1;
  uint **ppuVar2;
  uint uVar3;
  uint uVar4;
  uint *UNRECOVERED_JUMPTABLE;
  uint uVar5;
  int iVar6;
  
  UNRECOVERED_JUMPTABLE = *param_1;
  uVar4 = UNRECOVERED_JUMPTABLE[7];
  uVar5 = *UNRECOVERED_JUMPTABLE;
  uVar3 = UNRECOVERED_JUMPTABLE[2];
  if ((uVar4 & 0x80f) == 0) {
    if ((-1 < (int)(uVar4 << 0x1a)) || ((uVar5 & 0x20 | uVar3 & 0x10000000) == 0))
    goto LAB_08016812;
    UNRECOVERED_JUMPTABLE = param_1[0x1d];
    if (UNRECOVERED_JUMPTABLE == (uint *)0x0) {
      return param_1;
    }
    goto LAB_080169fe;
  }
  ppuVar2 = (uint **)(DAT_08016a84 & uVar3);
  if ((uVar5 & DAT_08016a80 | (uint)ppuVar2) == 0) {
LAB_08016812:
    if (((param_1[0x1b] == (uint *)0x1) && ((int)(uVar4 << 0x1b) < 0)) && ((int)(uVar5 << 0x1b) < 0)
       ) {
      UNRECOVERED_JUMPTABLE[8] = 0x10;
      if ((int)(UNRECOVERED_JUMPTABLE[2] << 0x19) < 0) {
        ppuVar2 = (uint **)param_1[0x20];
        uVar3 = (*ppuVar2)[1] & 0xffff;
        if ((uVar3 != 0) && (uVar4 = (uint)*(ushort *)(param_1 + 0x17), uVar3 < uVar4)) {
          *(short *)((int)param_1 + 0x5e) = (short)(*ppuVar2)[1];
          if (ppuVar2[7] != (uint *)0x100) {
            do {
              ExclusiveAccess(UNRECOVERED_JUMPTABLE);
              bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE);
            } while (!bVar1);
            *UNRECOVERED_JUMPTABLE = *UNRECOVERED_JUMPTABLE & 0xfffffeff;
            do {
              ExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
              bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
            } while (!bVar1);
            UNRECOVERED_JUMPTABLE[2] = UNRECOVERED_JUMPTABLE[2] & 0xfffffffe;
            do {
              ExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
              bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
            } while (!bVar1);
            UNRECOVERED_JUMPTABLE[2] = UNRECOVERED_JUMPTABLE[2] & 0xffffffbf;
            param_1[0x23] = (uint *)0x20;
            param_1[0x1b] = (uint *)0x0;
            do {
              ExclusiveAccess(UNRECOVERED_JUMPTABLE);
              bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE);
            } while (!bVar1);
            *UNRECOVERED_JUMPTABLE = *UNRECOVERED_JUMPTABLE & 0xffffffef;
            FUN_0800b550();
            uVar4 = (uint)*(ushort *)(param_1 + 0x17);
          }
          param_1[0x1c] = (uint *)0x2;
          ppuVar2 = (uint **)FUN_080167e4(param_1,uVar4 - *(ushort *)((int)param_1 + 0x5e) & 0xffff)
          ;
        }
      }
      else {
        ppuVar2 = (uint **)(uint)*(ushort *)((int)param_1 + 0x5e);
        if ((*(short *)((int)param_1 + 0x5e) != 0) &&
           (((uint)*(ushort *)(param_1 + 0x17) - (int)ppuVar2 & 0xffff) != 0)) {
          do {
            ExclusiveAccess(UNRECOVERED_JUMPTABLE);
            bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE);
          } while (!bVar1);
          *UNRECOVERED_JUMPTABLE = *UNRECOVERED_JUMPTABLE & 0xfffffedf;
          do {
            ExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
            bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
          } while (!bVar1);
          UNRECOVERED_JUMPTABLE[2] = UNRECOVERED_JUMPTABLE[2] & DAT_08016b0c;
          param_1[0x1d] = (uint *)0x0;
          param_1[0x23] = (uint *)0x20;
          param_1[0x1b] = (uint *)0x0;
          do {
            ExclusiveAccess(UNRECOVERED_JUMPTABLE);
            bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE);
          } while (!bVar1);
          *UNRECOVERED_JUMPTABLE = *UNRECOVERED_JUMPTABLE & 0xffffffef;
          param_1[0x1c] = (uint *)0x2;
          ppuVar2 = (uint **)FUN_080167e4(param_1);
        }
      }
    }
    else {
      if (((int)(uVar4 << 0xb) < 0) && ((int)(uVar3 << 9) < 0)) {
        UNRECOVERED_JUMPTABLE[8] = 0x100000;
        return param_1;
      }
      if (((int)(uVar4 << 0x18) < 0) &&
         (ppuVar2 = (uint **)(uVar5 & 0x80), (uVar3 & 0x800000 | (uint)ppuVar2) != 0)) {
        if (param_1[0x1e] != (uint *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x08016a4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
          ppuVar2 = (uint **)(*(code *)param_1[0x1e])(param_1);
          return ppuVar2;
        }
      }
      else if (((int)(uVar4 << 0x19) < 0) && ((int)(uVar5 << 0x19) < 0)) {
        do {
          ExclusiveAccess(UNRECOVERED_JUMPTABLE);
          bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE);
        } while (!bVar1);
        *UNRECOVERED_JUMPTABLE = *UNRECOVERED_JUMPTABLE & 0xffffffbf;
        param_1[0x1e] = (uint *)0x0;
        param_1[0x22] = (uint *)0x20;
        ppuVar2 = (uint **)FUN_080150dc(param_1);
      }
      else {
        ppuVar2 = (uint **)(uVar4 << 8);
        if (((int)ppuVar2 < 0) && ((int)(uVar5 << 1) < 0)) {
          return param_1;
        }
        if (((int)(uVar4 << 7) < 0) && ((int)uVar5 < 0)) {
          return param_1;
        }
      }
    }
    return ppuVar2;
  }
  if (((int)(uVar4 << 0x1f) < 0) && ((int)(uVar5 << 0x17) < 0)) {
    UNRECOVERED_JUMPTABLE[8] = 1;
    param_1[0x24] = (uint *)((uint)param_1[0x24] | 1);
  }
  if ((int)(uVar4 << 0x1e) < 0) {
    if ((int)(uVar3 << 0x1f) < 0) {
      UNRECOVERED_JUMPTABLE[8] = 2;
      param_1[0x24] = (uint *)((uint)param_1[0x24] | 4);
      iVar6 = uVar4 << 0x1d;
joined_r0x08016a2a:
      if (iVar6 < 0) {
        UNRECOVERED_JUMPTABLE[8] = 4;
        param_1[0x24] = (uint *)((uint)param_1[0x24] | 2);
      }
    }
  }
  else if ((int)(uVar4 << 0x1d) < 0) {
    iVar6 = uVar3 << 0x1f;
    goto joined_r0x08016a2a;
  }
  if (((int)(uVar4 << 0x1c) < 0) &&
     (ppuVar2 = (uint **)((uint)ppuVar2 | uVar5 & 0x20), ppuVar2 != (uint **)0x0)) {
    UNRECOVERED_JUMPTABLE[8] = 8;
    ppuVar2 = (uint **)((uint)param_1[0x24] | 8);
    param_1[0x24] = (uint *)ppuVar2;
  }
  if (((int)(uVar4 << 0x14) < 0) && (ppuVar2 = (uint **)(uVar5 << 5), (int)ppuVar2 < 0)) {
    ppuVar2 = (uint **)0x800;
    UNRECOVERED_JUMPTABLE[8] = 0x800;
    param_1[0x24] = (uint *)((uint)param_1[0x24] | 0x20);
  }
  if (param_1[0x24] == (uint *)0x0) {
    return ppuVar2;
  }
  if ((((int)(uVar4 << 0x1a) < 0) && ((uVar5 & 0x20 | uVar3 & 0x10000000) != 0)) &&
     (param_1[0x1d] != (uint *)0x0)) {
    (*(code *)param_1[0x1d])(param_1);
    UNRECOVERED_JUMPTABLE = *param_1;
  }
  if (((uint)param_1[0x24] & 0x28 | UNRECOVERED_JUMPTABLE[2] & 0x40) == 0) {
    ppuVar2 = (uint **)FUN_080151b4(param_1);
    param_1[0x24] = (uint *)0x0;
    return ppuVar2;
  }
  FUN_0801660c();
  UNRECOVERED_JUMPTABLE = *param_1;
  if ((int)(UNRECOVERED_JUMPTABLE[2] << 0x19) < 0) {
    do {
      ExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
      bVar1 = (bool)hasExclusiveAccess(UNRECOVERED_JUMPTABLE + 2);
    } while (!bVar1);
    UNRECOVERED_JUMPTABLE[2] = UNRECOVERED_JUMPTABLE[2] & 0xffffffbf;
    if (param_1[0x20] != (uint *)0x0) {
      param_1[0x20][0x14] = DAT_08016a88;
      iVar6 = FUN_0800b840();
      if (iVar6 == 0) {
        return (uint **)0x0;
      }
      UNRECOVERED_JUMPTABLE = (uint *)param_1[0x20][0x14];
LAB_080169fe:
                    /* WARNING: Could not recover jumptable at 0x08016a02. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      ppuVar2 = (uint **)(*(code *)UNRECOVERED_JUMPTABLE)();
      return ppuVar2;
    }
  }
  ppuVar2 = (uint **)FUN_080151b4(param_1);
  return ppuVar2;
}


