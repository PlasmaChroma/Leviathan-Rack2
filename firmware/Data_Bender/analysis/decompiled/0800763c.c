/* 0800763c FUN_0800763c; analyst naming is provisional. */

int * FUN_0800763c(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  byte *pbVar1;
  byte bVar2;
  undefined uVar3;
  byte bVar4;
  int *piVar5;
  undefined4 *puVar6;
  int iVar7;
  int *piVar8;
  uint uVar9;
  undefined4 uVar10;
  ushort uVar11;
  uint uVar12;
  undefined4 unaff_r4;
  uint uVar13;
  undefined4 unaff_r5;
  ushort *puVar14;
  undefined4 unaff_r6;
  undefined4 unaff_r7;
  byte *pbVar15;
  undefined4 unaff_r8;
  int iVar16;
  uint uVar17;
  undefined4 in_lr;
  
  iVar7 = DAT_0800770c;
  if ((*param_1 != DAT_08007708) || (*(char *)(DAT_0800770c + 0x668) == '\0')) {
    return param_1;
  }
  if (*(char *)(DAT_0800770c + 0x550) != '\0') {
    uVar13 = 0;
    iVar16 = DAT_0800770c + 0x3c;
    puVar14 = (ushort *)(DAT_0800770c + 0x562);
    pbVar15 = (byte *)(DAT_0800770c + 0x550);
    do {
      pbVar1 = pbVar15 + 1;
      if (*pbVar1 != 0) {
        uVar9 = 0;
        *(undefined2 *)(*(int *)(iVar7 + 0x588) + uVar13 * 0x10 + (uint)*(byte *)puVar14 * 2) =
             *(undefined2 *)(*(int *)(iVar7 + 0x584) + uVar13 * 2);
        uVar11 = *puVar14 + 1;
        uVar12 = (uint)uVar11;
        *puVar14 = uVar11;
        uVar17 = (uint)(byte)uVar11;
        bVar2 = *pbVar1;
        if (uVar12 < bVar2) {
          uVar9 = uVar12 & 1;
        }
        else {
          *puVar14 = 0;
          uVar17 = uVar9;
        }
        bVar4 = pbVar15[-0x10];
        GPIO_Write(iVar16 + -0x28,uVar9,(uint)bVar2,uVar12,param_4);
        if (1 < bVar4) {
          GPIO_Write(iVar16 + -0x14,(uVar17 << 0x1e) >> 0x1f);
          if (bVar4 != 2) {
            GPIO_Write(iVar16,(uVar17 << 0x1d) >> 0x1f);
          }
        }
      }
      uVar13 = uVar13 + 1;
      iVar16 = iVar16 + 0x54;
      puVar14 = puVar14 + 1;
      pbVar15 = pbVar1;
    } while ((uVar13 & 0xffff) < (uint)*(byte *)(iVar7 + 0x550));
  }
  FUN_08007190();
  piVar5 = DAT_08007710;
  uVar3 = *(undefined *)(iVar7 + 0x550);
  uVar10 = *(undefined4 *)(iVar7 + 0x584);
  puVar6 = (undefined4 *)*DAT_08007710;
  if ((puVar6 == DAT_0800a3a0) || (puVar6 == DAT_0800a3a4)) {
    uVar13 = *(uint *)(DAT_0800a3ac + 8);
    iVar7 = puVar6[2];
  }
  else {
    uVar13 = *(uint *)(DAT_0800a3a8 + 8);
    iVar7 = puVar6[2];
  }
  if ((-1 < iVar7 << 0x1d) && (*(char *)(DAT_08007710 + 0x14) != '\x01')) {
    uVar13 = uVar13 & 0x1f;
    *(undefined *)(DAT_08007710 + 0x14) = 1;
    if ((uVar13 < 10) && ((~(0x221U >> uVar13) & 1) == 0)) {
      piVar8 = (int *)FUN_0800a1f4(piVar5);
      if (piVar8 != (int *)0x0) {
        *(undefined *)(piVar5 + 0x14) = 0;
        return piVar8;
      }
      puVar6 = (undefined4 *)*piVar5;
      piVar5[0x15] = DAT_0800a3b0 & piVar5[0x15] | 0x100;
      if ((uVar13 == 0) || (puVar6 != DAT_0800a3a4)) {
        piVar5[0x15] = piVar5[0x15] & 0xffefffff;
      }
      if ((piVar5[0x15] & 0x1000U) == 0) {
        piVar5[0x16] = 0;
      }
      else {
        piVar5[0x16] = piVar5[0x16] & 0xfffffff9;
      }
      iVar7 = piVar5[0x13];
      uVar13 = piVar5[0xb];
      *(undefined4 *)(iVar7 + 0x3c) = DAT_0800a3b4;
      *(undefined4 *)(iVar7 + 0x40) = DAT_0800a3b8;
      *(undefined4 *)(iVar7 + 0x4c) = DAT_0800a3bc;
      *puVar6 = 0x1c;
      *(undefined *)(piVar5 + 0x14) = 0;
      puVar6[1] = puVar6[1] | 0x10;
      puVar6[3] = puVar6[3] & 0xfffffffc | uVar13;
      piVar8 = (int *)FUN_0800b3a0(iVar7,puVar6 + 0x10,uVar10,uVar3,unaff_r4,unaff_r5,unaff_r6,
                                   unaff_r7,unaff_r8,in_lr);
      *(uint *)(*piVar5 + 8) = DAT_0800a3c0 & *(uint *)(*piVar5 + 8) | 4;
      return piVar8;
    }
    *(undefined *)(piVar5 + 0x14) = 0;
    return (int *)0x1;
  }
  return (int *)0x2;
}


