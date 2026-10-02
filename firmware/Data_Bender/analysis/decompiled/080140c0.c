/* 080140c0 FUN_080140c0; analyst naming is provisional. */

void FUN_080140c0(void)

{
  uint **ppuVar1;
  char cVar2;
  uint **ppuVar3;
  uint *puVar4;
  int iVar5;
  uint **ppuVar6;
  uint *puVar7;
  uint *puVar8;
  uint uVar9;
  uint uVar10;
  
  ppuVar6 = DAT_080140c8;
  puVar8 = *DAT_080140c8;
  uVar10 = puVar8[2];
  uVar9 = puVar8[4] & puVar8[5];
  cVar2 = *(char *)((int)DAT_080140c8 + 0x81);
  if (((int)(puVar8[5] << 0x14) < 0) && ((int)(puVar8[4] << 0x1c) < 0)) {
    puVar8[6] = puVar8[6] | 0x800;
    FUN_08015d4c();
    return;
  }
  if ((uVar9 & 100) == 4) {
    (*(code *)DAT_080140c8[0x1d])(DAT_080140c8);
    (*(code *)ppuVar6[0x1c])(ppuVar6);
    if ((uVar9 & 0x45) == 1) goto LAB_08015e0c;
  }
  else {
    if ((uVar9 & 0x45) != 1) {
      if ((uVar9 & 0x26) != 2) {
        if ((int)(uVar9 << 0x1c) < 0) {
          puVar8[6] = puVar8[6] | 8;
          puVar8[6] = puVar8[6] | 0x10;
          puVar8[6] = puVar8[6] | 0x800;
          puVar8[4] = puVar8[4] & 0xfffffff7;
          if (((puVar8[2] & 0xc000) == 0) && (*(short *)((int)ppuVar6 + 0x6a) != 0)) {
            puVar4 = ppuVar6[0x19];
            do {
              if (ppuVar6[3] < (uint *)0x10) {
                if (ppuVar6[3] < &SupervisorCall) {
                  *(undefined *)puVar4 = *(undefined *)(*ppuVar6 + 0xc);
                  puVar7 = (uint *)((int)ppuVar6[0x19] + 1);
                  ppuVar6[0x19] = puVar7;
                }
                else {
                  puVar7 = (uint *)((int)puVar4 + 2);
                  *(undefined2 *)puVar4 = *(undefined2 *)(puVar8 + 0xc);
                  ppuVar6[0x19] = puVar7;
                }
              }
              else {
                puVar7 = puVar4 + 1;
                *puVar4 = (*ppuVar6)[0xc];
                ppuVar6[0x19] = puVar7;
              }
              *(short *)((int)ppuVar6 + 0x6a) = *(short *)((int)ppuVar6 + 0x6a) + -1;
              puVar4 = puVar7;
            } while (*(short *)((int)ppuVar6 + 0x6a) != 0);
          }
          ppuVar3 = (uint **)FUN_080154c8(ppuVar6);
          *(undefined *)((int)ppuVar6 + 0x81) = 1;
          ppuVar1 = ppuVar6 + 0x21;
          ppuVar6 = ppuVar3;
          if (*ppuVar1 == (uint *)0x0) {
            if (cVar2 == '\x05') {
              FUN_0801416c();
              return;
            }
            if (cVar2 != '\x04') {
              if (cVar2 != '\x03') {
                return;
              }
              FUN_0801415c();
              return;
            }
            FUN_08014164();
            return;
          }
        }
        else {
          if ((uVar9 & 0x360) == 0) {
            return;
          }
          if ((int)(uVar9 << 0x19) < 0) {
            DAT_080140c8[0x21] = (uint *)((uint)DAT_080140c8[0x21] | 4);
            puVar8[6] = puVar8[6] | 0x40;
          }
          if ((int)(uVar9 << 0x16) < 0) {
            ppuVar6[0x21] = (uint *)((uint)ppuVar6[0x21] | 1);
            puVar8[6] = puVar8[6] | 0x200;
          }
          if ((int)(uVar9 << 0x17) < 0) {
            ppuVar6[0x21] = (uint *)((uint)ppuVar6[0x21] | 8);
            puVar8[6] = puVar8[6] | 0x100;
          }
          if ((int)(uVar9 << 0x1a) < 0) {
            ppuVar6[0x21] = (uint *)((uint)ppuVar6[0x21] | 0x80);
            puVar8[6] = puVar8[6] | 0x20;
          }
          uVar9 = DAT_08015f5c;
          if (ppuVar6[0x21] == (uint *)0x0) {
            return;
          }
          *puVar8 = *puVar8 & 0xfffffffe;
          puVar8[4] = uVar9 & puVar8[4];
          if ((uVar10 & 0xc000) == 0xc000) {
            puVar4 = ppuVar6[0x1f];
            puVar8[2] = puVar8[2] & 0xffff3fff;
            if (puVar4 != (uint *)0x0) {
              puVar4[0x14] = DAT_08015f60;
              iVar5 = FUN_0800b840();
              if (iVar5 != 0) {
                ppuVar6[0x21] = (uint *)((uint)ppuVar6[0x21] | 0x40);
              }
            }
            if (ppuVar6[0x1e] == (uint *)0x0) {
              return;
            }
            ppuVar6[0x1e][0x14] = DAT_08015f60;
            iVar5 = FUN_0800b840();
            if (iVar5 == 0) {
              return;
            }
            ppuVar6[0x21] = (uint *)((uint)ppuVar6[0x21] | 0x40);
            return;
          }
          *(undefined *)((int)ppuVar6 + 0x81) = 1;
        }
        FUN_08014174(ppuVar6);
        return;
      }
      goto LAB_08015f44;
    }
LAB_08015e0c:
    (*(code *)ppuVar6[0x1c])(ppuVar6);
  }
  if ((uVar9 & 0x26) != 2) {
    return;
  }
LAB_08015f44:
                    /* WARNING: Could not recover jumptable at 0x08015f4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)ppuVar6[0x1d])(ppuVar6);
  return;
}


