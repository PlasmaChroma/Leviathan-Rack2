
/* === 080002c4 FUN_080002c4 === */

undefined8 FUN_080002c4(undefined4 param_1,undefined4 param_2)

{
  if (DAT_080002d4 != 0) {
    param_1 = DAT_080002dc;
    param_2 = DAT_080002d8;
  }
  return CONCAT44(param_2,param_1);
}



/* === 080002e0 FUN_080002e0 === */

byte * FUN_080002e0(uint *param_1,uint param_2,uint param_3)

{
  byte bVar1;
  char cVar2;
  char cVar3;
  char cVar4;
  char cVar5;
  byte *pbVar6;
  uint *puVar7;
  byte *pbVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  bool bVar15;
  bool bVar16;
  
  param_2 = param_2 & 0xff;
  if ((int)param_3 < 0x10) {
joined_r0x08000340:
    do {
      if (param_3 == 0) {
        return (byte *)0x0;
      }
      puVar7 = (uint *)((int)param_1 + 1);
      bVar1 = *(byte *)param_1;
      param_3 = param_3 - 1;
      param_1 = puVar7;
    } while (bVar1 != param_2);
  }
  else {
    uVar9 = (uint)param_1 & 7;
    while( true ) {
      if (uVar9 == 0) {
        uVar9 = param_2 | param_2 << 8;
        uVar9 = uVar9 | uVar9 << 0x10;
        uVar10 = param_3 & 0xfffffff8;
        do {
          puVar7 = param_1 + 2;
          uVar10 = uVar10 - 8;
          uVar11 = *param_1 ^ uVar9;
          uVar12 = param_1[1] ^ uVar9;
          cVar2 = -((char)uVar11 == '\0');
          cVar3 = -((char)(uVar11 >> 8) == '\0');
          cVar4 = -((char)(uVar11 >> 0x10) == '\0');
          cVar5 = -((char)(uVar11 >> 0x18) == '\0');
          uVar11 = CONCAT13(cVar5,CONCAT12(cVar4,CONCAT11(cVar3,cVar2)));
          bVar13 = (char)uVar12 != '\0';
          bVar14 = (char)(uVar12 >> 8) != '\0';
          bVar15 = (char)(uVar12 >> 0x10) != '\0';
          bVar16 = (char)(uVar12 >> 0x18) != '\0';
          uVar12 = CONCAT13(bVar16 * cVar5 - !bVar16,
                            CONCAT12(bVar15 * cVar4 - !bVar15,
                                     CONCAT11(bVar14 * cVar3 - !bVar14,bVar13 * cVar2 - !bVar13)));
          if (uVar12 != 0) {
            if (uVar11 == 0) {
              pbVar8 = (byte *)((int)param_1 + 5);
              uVar11 = uVar12;
            }
            else {
              pbVar8 = (byte *)((int)param_1 + 1);
            }
            if ((uVar11 & 1) == 0) {
              bVar13 = (uVar11 & 0x100) == 0;
              pbVar6 = pbVar8 + 1;
              if (bVar13) {
                bVar13 = (uVar11 & 0x18000) == 0;
                pbVar6 = pbVar8 + 2;
              }
              pbVar8 = pbVar6;
              if (bVar13) {
                pbVar8 = pbVar8 + 1;
              }
            }
            return pbVar8 + -1;
          }
          param_1 = puVar7;
        } while (uVar10 != 0);
        param_3 = param_3 & 7;
        goto joined_r0x08000340;
      }
      puVar7 = (uint *)((int)param_1 + 1);
      param_3 = param_3 - 1;
      if (*(byte *)param_1 == param_2) break;
      uVar9 = (uint)puVar7 & 7;
      param_1 = puVar7;
      if (param_3 == 0) {
        return (byte *)0x0;
      }
    }
  }
  return (byte *)((int)puVar7 + -1);
}



/* === 0800069c FUN_0800069c === */

ulonglong FUN_0800069c(uint param_1,uint param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  
  if ((param_1 | param_2) == 0) {
    return CONCAT44(param_2,param_1);
  }
  uVar8 = param_2 & 0x80000000;
  uVar3 = param_2;
  if ((int)uVar8 < 0) {
    bVar11 = param_1 != 0;
    param_1 = -param_1;
    uVar3 = -param_2 - (uint)bVar11;
  }
  iVar7 = 0x432;
  uVar9 = uVar3 >> 0x16;
  if (uVar9 != 0) {
    iVar7 = 3;
    if (uVar3 >> 0x19 != 0) {
      iVar7 = 6;
    }
    if (uVar3 >> 0x1c != 0) {
      iVar7 = iVar7 + 3;
    }
    uVar4 = iVar7 - ((int)uVar3 >> 0x1f);
    uVar9 = param_1 << (0x20 - uVar4 & 0xff);
    param_1 = param_1 >> (uVar4 & 0xff) | uVar3 << (0x20 - uVar4 & 0xff);
    uVar3 = uVar3 >> (uVar4 & 0xff);
    iVar7 = uVar4 + 0x432;
  }
  if (0xfffff < uVar3) {
    if (0x1fffff < uVar3) {
      uVar4 = uVar3 & 1;
      uVar3 = uVar3 >> 1;
      bVar1 = (byte)param_1;
      param_1 = (uint)(uVar4 != 0) << 0x1f | param_1 >> 1;
      uVar9 = (uint)(bVar1 & 1) << 0x1f | uVar9 >> 1;
      iVar7 = iVar7 + 1;
      if (0xffbfffff < (uint)(iVar7 * 0x200000)) {
        return (ulonglong)(uVar8 | 0x7ff00000) << 0x20;
      }
    }
LAB_08000498:
    bVar11 = 0x7fffffff < uVar9;
    if (uVar9 == 0x80000000) {
      bVar11 = (param_1 & 1) != 0;
    }
    return CONCAT44(uVar3 + iVar7 * 0x100000 + (uint)CARRY4(param_1,(uint)bVar11) | uVar8,
                    param_1 + bVar11);
  }
  bVar10 = (uVar9 & 0x80000000) != 0;
  uVar9 = uVar9 << 1;
  uVar4 = param_1 * 2;
  bVar11 = CARRY4(param_1,param_1);
  param_1 = param_1 * 2 + (uint)bVar10;
  uVar3 = uVar3 * 2 + (uint)(bVar11 || CARRY4(uVar4,(uint)bVar10));
  bVar11 = iVar7 != 0;
  iVar7 = iVar7 + -1;
  if (bVar11 && 0xfffff < uVar3) goto LAB_08000498;
  uVar2 = param_1;
  uVar4 = uVar3;
  if (uVar3 == 0) {
    uVar2 = 0;
    uVar4 = param_1;
  }
  iVar5 = LZCOUNT(uVar4);
  if (uVar3 == 0) {
    iVar5 = iVar5 + 0x20;
  }
  uVar6 = iVar5 - 0xb;
  bVar12 = SBORROW4(uVar6,0x20);
  uVar3 = iVar5 - 0x2b;
  bVar11 = (int)uVar3 < 0;
  bVar10 = uVar3 == 0;
  if ((int)uVar6 < 0x20) {
    bVar12 = SCARRY4(uVar3,0xc);
    iVar5 = iVar5 + -0x1f;
    bVar11 = iVar5 < 0;
    bVar10 = iVar5 == 0;
    uVar3 = uVar6;
    if (!bVar10 && bVar11 == bVar12) {
      uVar2 = uVar4 << (uVar6 & 0xff);
      uVar4 = uVar4 >> (0xcU - iVar5 & 0xff);
      goto LAB_08000510;
    }
  }
  if (bVar10 || bVar11 != bVar12) {
    uVar9 = 0x20 - uVar3;
  }
  uVar4 = uVar4 << (uVar3 & 0xff);
  if (bVar10 || bVar11 != bVar12) {
    uVar4 = uVar4 | uVar2 >> (uVar9 & 0xff);
    uVar2 = uVar2 << (uVar3 & 0xff);
  }
LAB_08000510:
  if ((int)uVar6 <= iVar7) {
    return CONCAT44(uVar4 + (iVar7 - uVar6) * 0x100000 | uVar8,uVar2);
  }
  uVar3 = ~(iVar7 - uVar6);
  if ((int)uVar3 < 0x1f) {
    iVar7 = uVar3 - 0x13;
    if (iVar7 != 0 && iVar7 < 0 == SCARRY4(uVar3 - 0x1f,0xc)) {
      return CONCAT44(param_2,uVar2 >> (0x20 - (0xcU - iVar7) & 0xff) |
                              uVar4 << (0xcU - iVar7 & 0xff)) & 0x80000000ffffffff;
    }
    uVar3 = uVar3 + 1;
    return CONCAT44(uVar8 | uVar4 >> (uVar3 & 0xff),
                    uVar2 >> (uVar3 & 0xff) | uVar4 << (0x20 - uVar3 & 0xff));
  }
  return CONCAT44(param_2,uVar4 >> (uVar3 - 0x1f & 0xff)) & 0x80000000ffffffff;
}



/* === 080006f8 FUN_080006f8 === */

undefined8 FUN_080006f8(int param_1,int param_2,int param_3,int param_4)

{
  undefined8 uVar1;
  
  if ((param_4 == 0) && (param_3 == 0)) {
    if (param_2 != 0 || param_1 != 0) {
      param_2 = -1;
      param_1 = -1;
    }
    return CONCAT44(param_2,param_1);
  }
  uVar1 = FUN_08000728();
  return uVar1;
}



/* === 08000728 FUN_08000728 === */

/* WARNING: Removing unreachable block (ram,0x080009c8) */

ulonglong FUN_08000728(uint param_1,uint param_2,uint param_3,uint param_4,uint *param_5)

{
  code *pcVar1;
  ulonglong uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  
  if (param_4 == 0) {
    if (param_2 < param_3) {
      iVar6 = LZCOUNT(param_3);
      uVar11 = param_3;
      if (iVar6 != 0) {
        uVar11 = param_3 << iVar6;
        param_2 = param_1 >> (0x20U - iVar6 & 0xff) | param_2 << iVar6;
        param_1 = param_1 << iVar6;
      }
      uVar10 = uVar11 >> 0x10;
      uVar8 = param_2 / uVar10;
      uVar7 = param_1 >> 0x10 | (param_2 - uVar10 * uVar8) * 0x10000;
      uVar5 = uVar8 * (uVar11 & 0xffff);
      uVar3 = uVar8;
      if (uVar7 <= uVar5 && uVar5 - uVar7 != 0) {
        bVar13 = CARRY4(uVar11,uVar7);
        uVar7 = uVar11 + uVar7;
        uVar3 = uVar8 - 1;
        if ((bVar13 == false) && (uVar7 <= uVar5 && uVar5 - uVar7 != 0)) {
          uVar3 = uVar8 - 2;
          uVar7 = uVar7 + uVar11;
        }
      }
      uVar4 = (uVar7 - uVar5) / uVar10;
      uVar8 = param_1 & 0xffff | ((uVar7 - uVar5) - uVar10 * uVar4) * 0x10000;
      uVar7 = uVar4 * (uVar11 & 0xffff);
      uVar5 = uVar4;
      if (uVar8 <= uVar7 && uVar7 - uVar8 != 0) {
        bVar13 = CARRY4(uVar11,uVar8);
        uVar8 = uVar11 + uVar8;
        uVar5 = uVar4 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar7 && uVar7 - uVar8 != 0)) {
          uVar8 = uVar8 + uVar11;
          uVar5 = uVar4 - 2;
        }
      }
      uVar5 = uVar5 | uVar3 << 0x10;
      uVar8 = uVar8 - uVar7;
      uVar11 = 0;
    }
    else {
      if (param_3 == 0) {
                    /* WARNING: Does not return */
        pcVar1 = (code *)software_udf(0xff,0x8000812);
        (*pcVar1)();
      }
      iVar6 = LZCOUNT(param_3);
      if (iVar6 == 0) {
        param_2 = param_2 - param_3;
        uVar4 = param_3 >> 0x10;
        uVar12 = param_3 & 0xffff;
        uVar11 = 1;
        uVar3 = param_3;
      }
      else {
        uVar3 = param_3 << iVar6;
        uVar5 = param_2 >> (0x20U - iVar6 & 0xff);
        uVar7 = param_2 << iVar6 | param_1 >> (0x20U - iVar6 & 0xff);
        uVar4 = uVar3 >> 0x10;
        uVar12 = uVar3 & 0xffff;
        uVar11 = uVar5 / uVar4;
        uVar8 = uVar7 >> 0x10 | (uVar5 - uVar4 * uVar11) * 0x10000;
        uVar10 = uVar11 * uVar12;
        param_1 = param_1 << iVar6;
        uVar5 = uVar11;
        if (uVar8 <= uVar10 && uVar10 - uVar8 != 0) {
          bVar13 = CARRY4(uVar3,uVar8);
          uVar8 = uVar3 + uVar8;
          uVar5 = uVar11 - 1;
          if ((bVar13 == false) && (uVar8 <= uVar10 && uVar10 - uVar8 != 0)) {
            uVar5 = uVar11 - 2;
            uVar8 = uVar8 + uVar3;
          }
        }
        uVar9 = (uVar8 - uVar10) / uVar4;
        param_2 = uVar7 & 0xffff | ((uVar8 - uVar10) - uVar4 * uVar9) * 0x10000;
        uVar7 = uVar9 * uVar12;
        uVar11 = uVar9;
        if (param_2 <= uVar7 && uVar7 - param_2 != 0) {
          bVar13 = CARRY4(uVar3,param_2);
          param_2 = uVar3 + param_2;
          uVar11 = uVar9 - 1;
          if ((bVar13 == false) && (param_2 <= uVar7 && uVar7 - param_2 != 0)) {
            uVar11 = uVar9 - 2;
            param_2 = param_2 + uVar3;
          }
        }
        param_2 = param_2 - uVar7;
        uVar11 = uVar11 | uVar5 << 0x10;
      }
      uVar10 = param_2 / uVar4;
      uVar8 = param_1 >> 0x10 | (param_2 - uVar4 * uVar10) * 0x10000;
      uVar5 = uVar12 * uVar10;
      uVar7 = uVar10;
      if (uVar8 <= uVar5 && uVar5 - uVar8 != 0) {
        bVar13 = CARRY4(uVar3,uVar8);
        uVar8 = uVar3 + uVar8;
        uVar7 = uVar10 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar5 && uVar5 - uVar8 != 0)) {
          uVar7 = uVar10 - 2;
          uVar8 = uVar8 + uVar3;
        }
      }
      uVar10 = (uVar8 - uVar5) / uVar4;
      uVar8 = param_1 & 0xffff | ((uVar8 - uVar5) - uVar4 * uVar10) * 0x10000;
      uVar12 = uVar12 * uVar10;
      uVar5 = uVar10;
      if (uVar8 <= uVar12 && uVar12 - uVar8 != 0) {
        bVar13 = CARRY4(uVar3,uVar8);
        uVar8 = uVar3 + uVar8;
        uVar5 = uVar10 - 1;
        if ((bVar13 == false) && (uVar8 <= uVar12 && uVar12 - uVar8 != 0)) {
          uVar8 = uVar8 + uVar3;
          uVar5 = uVar10 - 2;
        }
      }
      uVar8 = uVar8 - uVar12;
      uVar5 = uVar5 | uVar7 << 0x10;
    }
    if (param_5 != (uint *)0x0) {
      *param_5 = uVar8 >> LZCOUNT(param_3);
      param_5[1] = 0;
    }
  }
  else if (param_2 < param_4) {
    if (param_5 != (uint *)0x0) {
      *param_5 = param_1;
      param_5[1] = param_2;
      return 0;
    }
    uVar5 = 0;
    uVar11 = 0;
  }
  else {
    iVar6 = LZCOUNT(param_4);
    if (iVar6 != 0) {
      uVar8 = 0x20 - iVar6;
      uVar12 = param_3 >> (uVar8 & 0xff) | param_4 << iVar6;
      uVar7 = param_1 >> (uVar8 & 0xff) | param_2 << iVar6;
      param_2 = param_2 >> (uVar8 & 0xff);
      uVar4 = uVar12 >> 0x10;
      param_1 = param_1 << iVar6;
      uVar10 = param_2 / uVar4;
      uVar3 = uVar7 >> 0x10 | (param_2 - uVar4 * uVar10) * 0x10000;
      uVar11 = uVar10 * (uVar12 & 0xffff);
      uVar5 = uVar10;
      if (uVar3 <= uVar11 && uVar11 - uVar3 != 0) {
        bVar13 = CARRY4(uVar12,uVar3);
        uVar3 = uVar12 + uVar3;
        uVar5 = uVar10 - 1;
        if ((bVar13 == false) && (uVar3 <= uVar11 && uVar11 - uVar3 != 0)) {
          uVar5 = uVar10 - 2;
          uVar3 = uVar3 + uVar12;
        }
      }
      uVar10 = (uVar3 - uVar11) / uVar4;
      uVar3 = uVar7 & 0xffff | ((uVar3 - uVar11) - uVar4 * uVar10) * 0x10000;
      uVar7 = uVar10 * (uVar12 & 0xffff);
      uVar11 = uVar10;
      if (uVar3 <= uVar7 && uVar7 - uVar3 != 0) {
        bVar13 = CARRY4(uVar12,uVar3);
        uVar3 = uVar12 + uVar3;
        uVar11 = uVar10 - 1;
        if ((bVar13 == false) && (uVar3 <= uVar7 && uVar7 - uVar3 != 0)) {
          uVar11 = uVar10 - 2;
          uVar3 = uVar3 + uVar12;
        }
      }
      uVar11 = uVar11 | uVar5 << 0x10;
      uVar2 = (ulonglong)uVar11 * (ulonglong)(param_3 << iVar6);
      if (CONCAT44(uVar3 - uVar7,param_1) < uVar2) {
        uVar2 = uVar2 - CONCAT44(uVar12,param_3 << iVar6);
        uVar11 = uVar11 - 1;
      }
      if (param_5 != (uint *)0x0) {
        uVar5 = ((uVar3 - uVar7) - (int)(uVar2 >> 0x20)) - (uint)(param_1 < (uint)uVar2);
        *param_5 = uVar5 << (uVar8 & 0xff) | param_1 - (uint)uVar2 >> iVar6;
        param_5[1] = uVar5 >> iVar6;
      }
      return (ulonglong)uVar11;
    }
    if ((param_4 < param_2) || (param_3 <= param_1)) {
      bVar13 = param_1 < param_3;
      param_1 = param_1 - param_3;
      param_2 = (param_2 - param_4) - (uint)bVar13;
      uVar5 = 1;
    }
    else {
      uVar5 = 0;
    }
    uVar11 = 0;
    if (param_5 != (uint *)0x0) {
      *param_5 = param_1;
      param_5[1] = param_2;
    }
  }
  return CONCAT44(uVar11,uVar5);
}



/* === 080009fc Default_Handler === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Default_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08000a08 Reset_Handler === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Reset_Handler(void)

{
  undefined4 in_r3;
  
  if (DAT_08000a38 != DAT_08000a3c) {
    libc_memcpy(DAT_08000a38,DAT_08000a40,DAT_08000a3c - DAT_08000a38,in_r3,in_r3);
  }
  if (DAT_08000a44 != DAT_08000a48) {
    memset(DAT_08000a44,0,DAT_08000a48 - DAT_08000a44);
  }
  SystemInit_STM32H7();
  libc_init_array();
  main();
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08000ac8 FUN_08000ac8 === */

undefined4 *
FUN_08000ac8(undefined4 *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  *(undefined *)(param_1 + 10) = 0xb;
  *(undefined *)(param_1 + 0xf) = 0xb;
  *(undefined *)(param_1 + 0x16) = 0xb;
  param_1[5] = 0;
  param_1[8] = 0;
  *(undefined *)((int)param_1 + 0x29) = 0xff;
  param_1[0xd] = 0;
  *(undefined *)((int)param_1 + 0x3d) = 0xff;
  param_1[0x12] = 0;
  *(undefined *)((int)param_1 + 0x59) = 0xff;
  param_1[0x19] = 0;
  param_1[0x1c] = 0;
  *param_1 = 0;
  param_1[1] = 0xff0bff0b;
  param_1[2] = 0xff0bff0b;
  param_1[3] = 0xff0bff0b;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x17] = 0;
  param_1[0x18] = 0;
  puVar2 = param_1 + 0x1d;
  do {
    puVar3 = puVar2 + 9;
    puVar2[3] = 0;
    *(undefined *)(puVar2 + 2) = 0xb;
    *(undefined *)((int)puVar2 + 9) = 0xff;
    puVar2[4] = 0;
    puVar2[5] = 0;
    puVar2 = puVar3;
  } while (puVar3 != param_1 + 0x5c);
  do {
    puVar2 = puVar3 + 6;
    puVar3[1] = 0;
    *(undefined *)puVar3 = 0xb;
    *(undefined *)((int)puVar3 + 1) = 0xff;
    puVar3[2] = 0;
    puVar3[3] = 0;
    uVar1 = DAT_08000ba8;
    puVar3 = puVar2;
  } while (puVar2 != param_1 + 0x74);
  param_1[0xd4] = 0;
  *(undefined *)((int)param_1 + 0x35e) = 0xb;
  *(undefined *)((int)param_1 + 0x35f) = 0xff;
  *(undefined *)(param_1 + 0xd8) = 0xb;
  *(undefined *)((int)param_1 + 0x361) = 0xff;
  param_1[0xdb] = 0;
  param_1[0xd9] = 0;
  param_1[0xda] = 0;
  libc_memcpy((int)param_1 + 0x376,uVar1,0x200,puVar2,param_4);
  *(undefined *)(param_1 + 0x188) = 0xb;
  *(undefined *)((int)param_1 + 0x621) = 0xff;
  param_1[0x189] = 0;
  param_1[0x18a] = 0;
  param_1[0x18b] = 0;
  *(undefined *)(param_1 + 0x18d) = 0xb;
  *(undefined *)((int)param_1 + 0x635) = 0xff;
  param_1[0x18e] = 0;
  param_1[399] = 0;
  param_1[400] = 0;
  return param_1;
}



/* === 08000bac FUN_08000bac === */

void FUN_08000bac(int *param_1)

{
  ushort uVar1;
  float fVar2;
  int iVar3;
  ushort uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  ushort uVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  
  fVar11 = DAT_08000d20;
  fVar10 = (float)param_1[2];
  fVar9 = fVar10 * (float)param_1[3] * DAT_08000d20;
  iVar7 = *param_1;
  if ((int)((uint)(fVar9 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar9 == DAT_08000d20 || fVar9 < DAT_08000d20 != (NAN(fVar9) || NAN(DAT_08000d20))) {
    uVar5 = (uint)(0.0 < fVar9) * (int)fVar9 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  iVar3 = *(int *)(iVar7 + 4);
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  fVar9 = fVar10 * (float)param_1[4] * DAT_08000d20;
  iVar6 = ((int)(uint)*(byte *)(param_1 + 1) >> 4) * 0x41 + (*(byte *)(param_1 + 1) & 0xf) * 4 +
          iVar3;
  uVar4 = *(ushort *)(iVar6 + 1) & 0xfff;
  uVar8 = uVar1 + uVar4;
  if (0xffe < uVar1) {
    uVar4 = uVar4 | 0x1000;
  }
  *(ushort *)(iVar6 + 1) = uVar4;
  *(ushort *)(iVar6 + 3) = uVar8 & 0xfff;
  fVar2 = DAT_08000d20;
  if ((int)((uint)(fVar9 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar9 == fVar11 || fVar9 < fVar11 != (NAN(fVar9) || NAN(fVar11))) {
    uVar5 = (uint)(0.0 < fVar9) * (int)fVar9 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  fVar11 = fVar10 * (float)param_1[5] * DAT_08000d20;
  iVar6 = ((int)(uint)*(byte *)((int)param_1 + 5) >> 4) * 0x41 +
          (*(byte *)((int)param_1 + 5) & 0xf) * 4 + iVar3;
  uVar4 = *(ushort *)(iVar6 + 1) & 0xfff;
  uVar8 = uVar1 + uVar4;
  if (0xffe < uVar1) {
    uVar4 = uVar4 | 0x1000;
  }
  *(ushort *)(iVar6 + 1) = uVar4;
  *(ushort *)(iVar6 + 3) = uVar8 & 0xfff;
  if ((int)((uint)(fVar11 < 0.0) << 0x1f) < 0) {
    uVar5 = 0;
  }
  else if (fVar11 == fVar2 || fVar11 < fVar2 != (NAN(fVar11) || NAN(fVar2))) {
    uVar5 = (uint)(0.0 < fVar11) * (int)fVar11 & 0xff;
  }
  else {
    uVar5 = 0xff;
  }
  uVar1 = *(ushort *)(iVar7 + uVar5 * 2 + 0x26);
  iVar3 = ((int)(uint)*(byte *)((int)param_1 + 6) >> 4) * 0x41 +
          (*(byte *)((int)param_1 + 6) & 0xf) * 4 + iVar3;
  uVar8 = *(ushort *)(iVar3 + 1) & 0xfff;
  uVar4 = uVar1 + uVar8;
  if (0xffe < uVar1) {
    uVar8 = uVar8 | 0x1000;
  }
  *(ushort *)(iVar3 + 1) = uVar8;
  *(ushort *)(iVar3 + 3) = uVar4 & 0xfff;
  return;
}



/* === 08000d28 DB_Vinyl_ProcessStereo === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Vinyl_ProcessStereo(float param_1,float param_2,int param_3,float *param_4,float *param_5)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  float fVar4;
  float fVar7;
  double dVar5;
  double dVar6;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  
  if (*(char *)(param_3 + 0x16c) == '\0') {
    *param_4 = param_1;
    *param_5 = param_2;
    return;
  }
  iVar2 = *(int *)(param_3 + 0x120) + 1;
  iVar1 = *(int *)(param_3 + 0x128) + 1;
  iVar2 = iVar2 - *(int *)(param_3 + 0x124) * (iVar2 / *(int *)(param_3 + 0x124));
  iVar1 = iVar1 - *(int *)(param_3 + 300) * (iVar1 / *(int *)(param_3 + 300));
  *(int *)(param_3 + 0x120) = iVar2;
  *(int *)(param_3 + 0x128) = iVar1;
  if (iVar1 == 0) {
    fVar9 = (float)FixedToFP(*(undefined4 *)(param_3 + 0x118),0x20,0x20,0x1f,0,0);
    *(float *)(param_3 + 0x130) = fVar9 * DAT_080010c8;
  }
  if (iVar2 == 0) {
    iVar1 = *(int *)(param_3 + 0x118) * 0x41a7;
    *(int *)(param_3 + 0x118) = iVar1;
    fVar9 = (float)FixedToFP(iVar1,0x20,0x20,0x1f,0,0);
    fVar9 = fVar9 * *(float *)(param_3 + 0x114) * *(float *)(param_3 + 0x130);
    *(float *)(param_3 + 0x11c) = fVar9;
  }
  else {
    fVar9 = *(float *)(param_3 + 0x11c);
  }
  fVar8 = DAT_080010bc;
  iVar1 = *(int *)(param_3 + 0x138) * 0x41a7;
  iVar2 = *(int *)(param_3 + 8) + 1;
  *(int *)(param_3 + 0x138) = iVar1;
  iVar2 = iVar2 - *(int *)(param_3 + 0xc) * (iVar2 / *(int *)(param_3 + 0xc));
  *(int *)(param_3 + 8) = iVar2;
  fVar14 = (float)(longlong)iVar1 * fVar8 * *(float *)(param_3 + 0x134);
  if (iVar2 == 0) {
    fVar10 = *(float *)(param_3 + 0x14);
    fVar7 = *(float *)(param_3 + 0x1c);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar8 = (float)(longlong)(int)uVar3 * fVar8;
    if (fVar10 == fVar7) {
      fVar7 = *(float *)(param_3 + 0x24);
      fVar10 = *(float *)(param_3 + 0x20);
      if (*(int *)(param_3 + 0x2c) == 0) goto LAB_08001278;
LAB_08000e44:
      dVar5 = (double)*(float *)(param_3 + 0x18);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar8 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar8) - 1.0;
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0x28);
      *(float *)(param_3 + 0x20) = fVar10;
      fVar7 = DAT_08001380;
      if (*(int *)(param_3 + 0x2c) != 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar7 = 2.0 / fVar10;
        }
        *(float *)(param_3 + 0x24) = fVar7;
        goto LAB_08000e44;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar7 = 1.0 / fVar10;
      }
      *(float *)(param_3 + 0x24) = fVar7;
LAB_08001278:
      dVar5 = (double)*(float *)(param_3 + 0x18);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar8 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar8 * fVar7);
      }
    }
    fVar8 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x10) = fVar8;
  }
  else {
    fVar8 = *(float *)(param_3 + 0x10);
  }
  local_28 = fVar8 + fVar9 + fVar14;
  local_30 = param_2;
  local_2c = param_1;
  fVar8 = (float)DaisySP_ATone_Process(param_3 + 0xe4,&local_28);
  iVar1 = *(int *)(param_3 + 0x34) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0x38) * (iVar1 / *(int *)(param_3 + 0x38));
  *(int *)(param_3 + 0x34) = iVar1;
  if (iVar1 == 0) {
    fVar10 = *(float *)(param_3 + 0x40);
    fVar11 = *(float *)(param_3 + 0x48);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar7 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar10 == fVar11) {
      fVar11 = *(float *)(param_3 + 0x50);
      fVar10 = *(float *)(param_3 + 0x4c);
      if (*(int *)(param_3 + 0x58) != 0) goto LAB_08001230;
LAB_080012ea:
      dVar5 = (double)*(float *)(param_3 + 0x44);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar11);
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0x54);
      *(float *)(param_3 + 0x4c) = fVar10;
      fVar11 = DAT_08001380;
      if (*(int *)(param_3 + 0x58) == 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar11 = 1.0 / fVar10;
        }
        *(float *)(param_3 + 0x50) = fVar11;
        goto LAB_080012ea;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar11 = 2.0 / fVar10;
      }
      *(float *)(param_3 + 0x50) = fVar11;
LAB_08001230:
      dVar5 = (double)*(float *)(param_3 + 0x44);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar11 * fVar7) - 1.0;
      }
    }
    fVar7 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x3c) = fVar7;
  }
  else {
    fVar7 = *(float *)(param_3 + 0x3c);
  }
  local_24 = fVar7 + fVar9 + fVar14;
  fVar9 = (float)DaisySP_ATone_Process(param_3 + 0xfc,&local_24);
  iVar1 = *(int *)(param_3 + 0x8c) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0x90) * (iVar1 / *(int *)(param_3 + 0x90));
  *(int *)(param_3 + 0x8c) = iVar1;
  if (iVar1 == 0) {
    fVar10 = *(float *)(param_3 + 0x98);
    fVar14 = *(float *)(param_3 + 0xa0);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar7 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar10 == fVar14) {
      fVar14 = *(float *)(param_3 + 0xa8);
      fVar10 = *(float *)(param_3 + 0xa4);
      if (*(int *)(param_3 + 0xb0) != 0) goto LAB_08001148;
LAB_080011d0:
      dVar5 = (double)*(float *)(param_3 + 0x9c);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar14);
      }
    }
    else {
      fVar10 = fVar10 * *(float *)(param_3 + 0xac);
      *(float *)(param_3 + 0xa4) = fVar10;
      fVar14 = DAT_08001380;
      if (*(int *)(param_3 + 0xb0) == 0) {
        if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
          fVar14 = 1.0 / fVar10;
        }
        *(float *)(param_3 + 0xa8) = fVar14;
        goto LAB_080011d0;
      }
      if (fVar10 != 0.0 && fVar10 < 0.0 == NAN(fVar10)) {
        fVar14 = 2.0 / fVar10;
      }
      *(float *)(param_3 + 0xa8) = fVar14;
LAB_08001148:
      dVar5 = (double)*(float *)(param_3 + 0x9c);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar7 < fVar10) << 0x1f) < 0) {
        dVar6 = (double)(fVar14 * fVar7) - 1.0;
      }
    }
    fVar14 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x94) = fVar14;
  }
  else {
    fVar14 = *(float *)(param_3 + 0x94);
  }
  iVar1 = *(int *)(param_3 + 0x60) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 100) * (iVar1 / *(int *)(param_3 + 100));
  *(int *)(param_3 + 0x60) = iVar1;
  if (iVar1 == 0) {
    fVar11 = *(float *)(param_3 + 0x6c);
    fVar7 = *(float *)(param_3 + 0x74);
    uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
    *DAT_080010c0 = uVar3;
    fVar10 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
    if (fVar11 == fVar7) {
      fVar7 = *(float *)(param_3 + 0x7c);
      fVar11 = *(float *)(param_3 + 0x78);
      if (*(int *)(param_3 + 0x84) != 0) goto LAB_080012a2;
LAB_0800131e:
      dVar5 = (double)*(float *)(param_3 + 0x70);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
        dVar6 = (double)(fVar10 * fVar7);
      }
    }
    else {
      fVar11 = fVar11 * *(float *)(param_3 + 0x80);
      *(float *)(param_3 + 0x78) = fVar11;
      fVar7 = DAT_08001380;
      if (*(int *)(param_3 + 0x84) == 0) {
        if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
          fVar7 = 1.0 / fVar11;
        }
        *(float *)(param_3 + 0x7c) = fVar7;
        goto LAB_0800131e;
      }
      if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
        fVar7 = 2.0 / fVar11;
      }
      *(float *)(param_3 + 0x7c) = fVar7;
LAB_080012a2:
      dVar5 = (double)*(float *)(param_3 + 0x70);
      dVar6 = DAT_08001378;
      if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
        dVar6 = (double)(fVar7 * fVar10) - 1.0;
      }
    }
    fVar7 = (float)(dVar5 * dVar6);
    *(float *)(param_3 + 0x68) = fVar7;
  }
  else {
    fVar7 = *(float *)(param_3 + 0x68);
  }
  iVar1 = *(int *)(param_3 + 0xb8) + 1;
  iVar1 = iVar1 - *(int *)(param_3 + 0xbc) * (iVar1 / *(int *)(param_3 + 0xbc));
  *(int *)(param_3 + 0xb8) = iVar1;
  if (iVar1 != 0) {
    fVar10 = *(float *)(param_3 + 0xc0);
    goto LAB_080010d4;
  }
  fVar11 = *(float *)(param_3 + 0xc4);
  fVar12 = *(float *)(param_3 + 0xcc);
  uVar3 = DAT_080010c4 * *DAT_080010c0 + 0x3039 & 0x7fffffff;
  *DAT_080010c0 = uVar3;
  fVar10 = (float)FixedToFP(uVar3,0x20,0x20,0x1f,0,0);
  if (fVar11 == fVar12) {
    fVar12 = *(float *)(param_3 + 0xd4);
    fVar11 = *(float *)(param_3 + 0xd0);
    if (*(int *)(param_3 + 0xdc) != 0) goto LAB_08001186;
LAB_08001206:
    dVar5 = (double)*(float *)(param_3 + 200);
    dVar6 = DAT_08001378;
    if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
      dVar6 = (double)(fVar10 * fVar12);
    }
  }
  else {
    fVar11 = fVar11 * *(float *)(param_3 + 0xd8);
    *(float *)(param_3 + 0xd0) = fVar11;
    fVar12 = DAT_08001380;
    if (*(int *)(param_3 + 0xdc) == 0) {
      if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
        fVar12 = 1.0 / fVar11;
      }
      *(float *)(param_3 + 0xd4) = fVar12;
      goto LAB_08001206;
    }
    if (fVar11 != 0.0 && fVar11 < 0.0 == NAN(fVar11)) {
      fVar12 = 2.0 / fVar11;
    }
    *(float *)(param_3 + 0xd4) = fVar12;
LAB_08001186:
    dVar5 = (double)*(float *)(param_3 + 200);
    dVar6 = DAT_08001378;
    if ((int)((uint)(fVar10 < fVar11) << 0x1f) < 0) {
      dVar6 = (double)(fVar12 * fVar10) - 1.0;
    }
  }
  fVar10 = (float)(dVar5 * dVar6);
  *(float *)(param_3 + 0xc0) = fVar10;
LAB_080010d4:
  fVar11 = fVar9 * DAT_080010c8;
  fVar12 = fVar8 * DAT_080010c8;
  local_2c = (float)DaisySP_ATone_Process(param_3 + 0x13c,&local_2c);
  fVar4 = (float)DaisySP_ATone_Process(param_3 + 0x154,&local_30);
  fVar13 = *(float *)(param_3 + 4);
  fVar9 = (fVar9 + fVar12 + fVar14 + fVar10) * DAT_080010cc;
  *param_4 = fVar13 * local_2c + (fVar8 + fVar11 + fVar7 + fVar10) * DAT_080010cc;
  *param_5 = fVar4 * fVar13 + fVar9;
  return;
}



/* === 08001384 DB_Corrupt_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Corrupt_Init(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  uint *puVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  undefined4 uVar10;
  float fVar11;
  float fVar12;
  
  *(undefined4 *)(param_2 + 0x14) = param_1;
  DaisySP_Decimator_Init(param_2 + 0x28);
  fVar12 = DAT_0800177c;
  DaisySP_Decimator_Init(param_2 + 0x50);
  DaisySP_Compressor_Init(*(undefined4 *)(param_2 + 0x14),param_2 + 0x78);
  DaisySP_Compressor_Init(*(undefined4 *)(param_2 + 0x14),param_2 + 0xb8);
  *(float *)(param_2 + 0x80) = fVar12;
  *(undefined *)(param_2 + 0xc) = 0;
  *(undefined *)(param_2 + 0x18) = 0;
  *(undefined4 *)(param_2 + 0x10) = 0;
  uVar10 = libm_expf(-(*(float *)(param_2 + 0xb0) / fVar12));
  *(undefined4 *)(param_2 + 0xa0) = uVar10;
  fVar11 = (float)libm_expf(-(*(float *)(param_2 + 0xac) / *(float *)(param_2 + 0x80)));
  *(float *)(param_2 + 0x98) = fVar11;
  *(float *)(param_2 + 0x84) = fVar12;
  *(float *)(param_2 + 0x9c) = (1.0 / *(float *)(param_2 + 0x78) - 1.0) * (1.0 - fVar11);
  uVar10 = libm_expf(-(*(float *)(param_2 + 0xb0) / fVar12));
  *(undefined4 *)(param_2 + 0xa4) = uVar10;
  *(undefined **)(param_2 + 0x7c) = &DAT_c0c00000;
  if (*(char *)(param_2 + 0xb4) != '\0') {
    *(float *)(param_2 + 0x88) = ABS(-6.0 - -6.0 / *(float *)(param_2 + 0x78)) * 0.5;
  }
  fVar12 = DAT_0800177c;
  fVar11 = *(float *)(param_2 + 0xf0) / DAT_0800177c;
  *(undefined4 *)(param_2 + 0x78) = 0x40000000;
  *(float *)(param_2 + 0xc0) = fVar12;
  *(float *)(param_2 + 0x9c) = (1.0 - *(float *)(param_2 + 0x98)) * -0.5;
  uVar10 = libm_expf(-fVar11);
  *(undefined4 *)(param_2 + 0xe0) = uVar10;
  fVar11 = (float)libm_expf(-(*(float *)(param_2 + 0xec) / *(float *)(param_2 + 0xc0)));
  *(float *)(param_2 + 0xd8) = fVar11;
  *(float *)(param_2 + 0xc4) = fVar12;
  *(float *)(param_2 + 0xdc) = (1.0 / *(float *)(param_2 + 0xb8) - 1.0) * (1.0 - fVar11);
  uVar10 = libm_expf(-(*(float *)(param_2 + 0xf0) / fVar12));
  *(undefined4 *)(param_2 + 0xe4) = uVar10;
  *(undefined **)(param_2 + 0xbc) = &DAT_c0c00000;
  if (*(char *)(param_2 + 0xf4) != '\0') {
    *(float *)(param_2 + 200) = ABS(-6.0 - -6.0 / *(float *)(param_2 + 0xb8)) * 0.5;
  }
  iVar9 = param_2 + 0xfc;
  iVar8 = param_2 + 0x148;
  *(undefined4 *)(param_2 + 0xb8) = 0x40000000;
  iVar7 = param_2 + 0x194;
  *(undefined4 *)(param_2 + 0xf8) = param_1;
  uVar10 = DAT_08001780;
  iVar6 = param_2 + 0x1e0;
  *(float *)(param_2 + 0xdc) = (1.0 - *(float *)(param_2 + 0xd8)) * -0.5;
  DaisySP_Svf_Init(param_1,iVar9);
  DaisySP_Svf_Init(param_1,iVar8);
  DaisySP_Svf_Init(param_1,iVar7);
  DaisySP_Svf_Init(param_1,iVar6);
  DaisySP_Svf_SetFreq(DAT_08001784,iVar9);
  DaisySP_Svf_SetFreq(DAT_08001784,iVar8);
  DaisySP_Svf_SetFreq(DAT_08001784,iVar7);
  DaisySP_Svf_SetFreq(DAT_08001784,iVar6);
  uVar1 = DAT_0800178c;
  *(undefined4 *)(param_2 + 0x22c) = DAT_08001788;
  *(undefined4 *)(param_2 + 0x230) = DAT_08001790;
  *(undefined4 *)(param_2 + 0x234) = DAT_08001794;
  *(undefined4 *)(param_2 + 0x238) = DAT_08001798;
  DaisySP_Svf_SetRes(uVar1,iVar9);
  DaisySP_Svf_SetRes(DAT_0800178c,iVar8);
  DaisySP_Svf_SetRes(DAT_0800178c,iVar7);
  DaisySP_Svf_SetRes(DAT_0800178c,iVar6);
  DaisySP_Svf_SetDrive(uVar10,iVar9);
  DaisySP_Svf_SetDrive(uVar10,iVar8);
  iVar8 = param_2 + 0x37c;
  DaisySP_Svf_SetDrive(uVar10,iVar7);
  iVar7 = DAT_0800179c;
  DaisySP_Svf_SetDrive(uVar10,iVar6);
  uVar3 = DAT_080017ac;
  uVar1 = DAT_080017a8;
  puVar2 = DAT_080017a0;
  fVar12 = *(float *)(param_2 + 0x14);
  fVar11 = 1.0 / fVar12;
  uVar4 = iVar7 * *DAT_080017a0 + 0x3039 & 0x7fffffff;
  *(undefined4 *)(param_2 + 600) = DAT_080017a8;
  *(undefined4 *)(param_2 + 0x26c) = 1;
  *(uint *)(param_2 + 0x270) = uVar4;
  *(undefined4 *)(param_2 + 0x284) = uVar1;
  uVar4 = iVar7 * uVar4 + 0x3039 & 0x7fffffff;
  *(undefined4 *)(param_2 + 0x254) = 0x40c00000;
  *(uint *)(param_2 + 0x29c) = uVar4;
  *(undefined4 *)(param_2 + 0x280) = 0x40c00000;
  uVar4 = iVar7 * uVar4 + 0x3039 & 0x7fffffff;
  *(undefined4 *)(param_2 + 0x298) = 1;
  *(undefined4 *)(param_2 + 0x2ac) = 0x40c00000;
  *(uint *)(param_2 + 0x2c8) = uVar4;
  *(undefined4 *)(param_2 + 0x2b0) = uVar1;
  uVar5 = iVar7 * uVar4 + 0x3039 & 0x7fffffff;
  *(undefined4 *)(param_2 + 0x2c4) = 1;
  *(undefined4 *)(param_2 + 0x2d8) = 0x40c00000;
  *(undefined4 *)(param_2 + 0x2dc) = uVar1;
  uVar4 = iVar7 * uVar5 + 0x3039 & 0x7fffffff;
  *(float *)(param_2 + 0x268) = fVar11;
  *(float *)(param_2 + 0x294) = fVar11;
  *(float *)(param_2 + 0x2c0) = fVar11;
  *(undefined4 *)(param_2 + 0x260) = uVar3;
  *(undefined4 *)(param_2 + 0x28c) = uVar3;
  *(undefined4 *)(param_2 + 0x2b8) = uVar3;
  *(undefined4 *)(param_2 + 0x24c) = 3;
  *(undefined4 *)(param_2 + 0x278) = 3;
  *(undefined4 *)(param_2 + 0x25c) = uVar10;
  *(undefined4 *)(param_2 + 0x264) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x288) = uVar10;
  *(undefined4 *)(param_2 + 0x290) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x2b4) = uVar10;
  *(undefined4 *)(param_2 + 700) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x2e0) = uVar10;
  *(undefined4 *)(param_2 + 0x2a4) = 3;
  *puVar2 = uVar4;
  *(undefined4 *)(param_2 + 0x2e4) = uVar3;
  *(undefined4 *)(param_2 + 0x304) = 0x40c00000;
  *(undefined4 *)(param_2 + 0x308) = uVar1;
  *(undefined4 *)(param_2 + 0x310) = uVar3;
  *(int *)(param_2 + 0x36c) = (int)(fVar12 * 0.5);
  *(float *)(param_2 + 0x2ec) = fVar11;
  *(undefined4 *)(param_2 + 0x2f0) = 1;
  *(uint *)(param_2 + 800) = uVar4;
  *(float *)(param_2 + 0x318) = fVar11;
  *(undefined4 *)(param_2 + 0x31c) = 1;
  *(undefined4 *)(param_2 + 0x358) = 1;
  *(undefined4 *)(param_2 + 0x378) = 1;
  *(uint *)(param_2 + 0x2f4) = uVar5;
  *(undefined4 *)(param_2 + 0x2d0) = 3;
  *(undefined4 *)(param_2 + 0x2fc) = 3;
  iVar7 = param_2 + 0x394;
  *(undefined4 *)(param_2 + 0x2e8) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x30c) = uVar10;
  *(undefined4 *)(param_2 + 0x314) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x354) = 0x3f800000;
  *(undefined4 *)(param_2 + 0x370) = uVar10;
  *(undefined4 *)(param_2 + 0x374) = 0x3f800000;
  DaisySP_ATone_Init(param_2 + 0x324);
  DaisySP_ATone_Init(fVar12,param_2 + 0x33c);
  DaisySP_ATone_Init(fVar12,iVar8);
  DaisySP_ATone_Init(fVar12,iVar7);
  uVar10 = DAT_080017a4;
  *(undefined4 *)(param_2 + 0x2a4) = 0xf;
  *(undefined4 *)(param_2 + 0x2d0) = 0xf;
  *(undefined4 *)(param_2 + 0x2fc) = 0xf;
  *(undefined4 *)(param_2 + 0x330) = uVar10;
  *(undefined4 *)(param_2 + 0x24c) = 5;
  *(undefined4 *)(param_2 + 0x278) = 5;
  *(undefined4 *)(param_2 + 0x364) = 10;
  DaisySP_ATone_CalculateCoefficients(param_2 + 0x324);
  *(undefined4 *)(param_2 + 0x348) = uVar10;
  DaisySP_ATone_CalculateCoefficients(param_2 + 0x33c);
  uVar1 = DAT_08001864;
  uVar10 = DAT_08001844;
  *(undefined4 *)(param_2 + 0x2b0) = DAT_08001844;
  *(undefined4 *)(param_2 + 0x2dc) = uVar10;
  *(undefined4 *)(param_2 + 0x308) = uVar10;
  uVar10 = DAT_08001848;
  *(undefined *)(param_2 + 0x3ac) = 0;
  *(undefined4 *)(param_2 + 0x2ac) = uVar10;
  *(undefined4 *)(param_2 + 0x2d8) = uVar10;
  *(undefined4 *)(param_2 + 0x304) = uVar10;
  uVar10 = DAT_0800184c;
  *(undefined4 *)(param_2 + 0x388) = uVar1;
  *(undefined4 *)(param_2 + 600) = uVar10;
  *(undefined4 *)(param_2 + 0x284) = uVar10;
  uVar10 = DAT_08001850;
  *(undefined4 *)(param_2 + 0x254) = DAT_08001850;
  *(undefined4 *)(param_2 + 0x280) = uVar10;
  *(undefined4 *)(param_2 + 0x354) = DAT_08001854;
  *(undefined4 *)(param_2 + 0x374) = DAT_08001858;
  DaisySP_ATone_CalculateCoefficients(iVar8);
  *(undefined4 *)(param_2 + 0x3a0) = uVar1;
  DaisySP_ATone_CalculateCoefficients(iVar7);
  uVar10 = DAT_08001860;
  *(undefined4 *)(param_2 + 0x244) = DAT_0800185c;
  *(undefined4 *)(param_2 + 0x388) = uVar10;
  DaisySP_ATone_CalculateCoefficients(iVar8);
  *(undefined4 *)(param_2 + 0x3a0) = uVar10;
  DaisySP_ATone_CalculateCoefficients(iVar7);
  *(undefined *)(param_2 + 0x3ac) = 0;
  return;
}



/* === 08001868 DB_Corrupt_ProcessInterleaved === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Corrupt_ProcessInterleaved(int *param_1,float *param_2,float *param_3,int param_4)

{
  char cVar1;
  uint uVar2;
  float *pfVar3;
  float *pfVar4;
  int iVar5;
  uint uVar6;
  undefined unaff_r8;
  bool bVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  undefined4 uVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  undefined4 uVar16;
  float fVar17;
  
  fVar8 = DAT_08001c70;
  fVar13 = DAT_08001c68;
  iVar5 = *param_1;
  if (*(char *)(param_1 + 3) == '\0') {
    if (iVar5 == 0) {
      libc_memcpy(param_3,param_2,param_4 << 2);
      return;
    }
  }
  else if (iVar5 == 0) {
    iVar5 = param_1[2];
    fVar17 = (float)param_1[7];
    goto LAB_08001894;
  }
  fVar17 = (float)param_1[4];
LAB_08001894:
  switch(iVar5) {
  case 1:
    fVar17 = fVar17 + fVar17;
    fVar13 = (float)libm_fmodf(fVar17);
    fVar13 = (float)FPRoundInt(fVar13 * 16.0,0x20,4,0);
    fVar9 = *(float *)(DAT_08001e40 + (int)fVar13 * 4);
    fVar8 = -fVar9 * 16.0 + 2.0;
    pfVar3 = (float *)(DAT_08001e44 + (int)fVar13 * 4);
    param_1[0xc] = (int)fVar9;
    param_1[0x16] = (int)fVar9;
    uVar6 = (uint)(0.0 < (float)(longlong)(int)(uint)*(byte *)(param_1 + 10) * fVar9) *
            (int)((float)(longlong)(int)(uint)*(byte *)(param_1 + 10) * fVar9);
    param_1[0xd] = uVar6;
    param_1[0x13] = (int)((float)(ulonglong)uVar6 + fVar8);
    fVar9 = (float)(longlong)(int)(uint)*(byte *)(param_1 + 0x14) * fVar9;
    uVar6 = (uint)(0.0 < fVar9) * (int)fVar9;
    param_1[0x17] = uVar6;
    fVar13 = (float)((uint)(fVar17 != 1.0) * 0x3f800000 + (uint)(fVar17 == 1.0) * 0x3e800000) *
             *pfVar3;
    param_1[0x1d] = (int)((float)(ulonglong)uVar6 + fVar8);
    param_1[0xb] = (int)fVar13;
    param_1[0x15] = (int)fVar13;
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        fVar13 = (float)DaisySP_Decimator_Process(*param_2,param_1 + 10);
        *param_3 = fVar13;
        fVar13 = (float)DaisySP_Decimator_Process(param_2[1],param_1 + 0x14);
        bVar7 = param_4 - 1U >> 1 != uVar6;
        param_3[1] = fVar13;
        uVar6 = uVar6 + 1;
        param_2 = param_2 + 2;
        param_3 = param_3 + 2;
      } while (bVar7);
      return;
    }
    break;
  case 2:
    if (fVar17 == DAT_08001ca0 || fVar17 < DAT_08001ca0 != (NAN(fVar17) || NAN(DAT_08001ca0))) {
      *(undefined *)(param_1 + 6) = 1;
    }
    else {
      uVar2 = newlib_rand_LCG64();
      uVar6 = uVar2 & 0x3ff;
      if ((int)uVar2 < 1) {
        uVar6 = -(uVar2 * -0x400000 >> 0x16);
      }
      if ((int)((uint)((float)(longlong)(int)uVar6 < fVar17 * fVar17 * DAT_08001e48) << 0x1f) < 0) {
        *(byte *)(param_1 + 6) = *(byte *)(param_1 + 6) ^ 1;
      }
    }
    if (param_4 != 0) {
      cVar1 = *(char *)(param_1 + 6);
      iVar5 = 4;
      pfVar3 = param_3 + 1;
      do {
        fVar13 = 0.0;
        if (cVar1 == '\0') {
          pfVar3[-1] = 0.0;
        }
        else {
          pfVar3[-1] = *param_2;
          fVar13 = param_2[1];
        }
        iVar5 = iVar5 + 8;
        param_2 = param_2 + 2;
        *pfVar3 = fVar13;
        pfVar3 = pfVar3 + 2;
      } while ((param_4 - 1U >> 1) * 8 + 0xc != iVar5);
    }
    break;
  case 3:
    fVar8 = fVar17 * fVar17 + fVar17 * fVar17;
    uVar11 = FPMaxNum(fVar8,DAT_08001c68);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    fVar9 = fVar13 * DAT_08001c8c + 1.0;
    uVar11 = FPMaxNum(fVar8 - 1.0,DAT_08001c68);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    fVar13 = fVar13 * 8.0 + 1.0;
    if ((int)((uint)(fVar9 < 2.0) << 0x1f) < 0) {
      fVar9 = 2.0;
    }
    if (param_4 != 0) {
      iVar5 = (uint)(fVar17 < DAT_08001c90) << 0x1f;
      pfVar3 = param_2 + (param_4 - 1U & 0xfffffffe) + 2;
      fVar8 = DAT_08001c68;
      if (-1 < iVar5) {
        fVar8 = 1.0;
      }
      if (iVar5 < 0) {
        unaff_r8 = 1;
      }
      if (-1 < iVar5) {
        unaff_r8 = 0;
      }
      do {
        fVar10 = *param_2;
        if (fVar10 == 0.0 || fVar10 < 0.0 != NAN(fVar10)) {
          fVar10 = (float)libm_expf(fVar10 * fVar9);
          fVar10 = fVar10 - 1.0;
          fVar12 = param_2[1];
          if (fVar12 == 0.0 || fVar12 < 0.0 != NAN(fVar12)) goto LAB_08001c4e;
LAB_08001b72:
          fVar12 = (float)libm_expf(-(fVar12 * fVar9));
          fVar12 = 1.0 - fVar12;
        }
        else {
          fVar10 = (float)libm_expf(-(fVar10 * fVar9));
          fVar10 = 1.0 - fVar10;
          fVar12 = param_2[1];
          if (fVar12 != 0.0 && fVar12 < 0.0 == NAN(fVar12)) goto LAB_08001b72;
LAB_08001c4e:
          fVar12 = (float)libm_expf(fVar12 * fVar9);
          fVar12 = fVar12 - 1.0;
        }
        uVar16 = FPMaxNum(fVar13 * fVar12,DAT_08001c94);
        uVar11 = FPMaxNum(fVar13 * fVar10,DAT_08001c94);
        fVar12 = (float)FPMinNum(uVar16,DAT_08001c98);
        fVar10 = (float)FPMinNum(uVar11,DAT_08001c98);
        if (fVar9 != 1.0 && fVar9 < 1.0 == NAN(fVar9)) {
          fVar14 = (float)libm_sinf(fVar17 * DAT_08001c9c);
          fVar14 = -fVar14 * 0.375 + 0.5;
          fVar10 = fVar10 * fVar14;
          fVar12 = fVar12 * fVar14;
        }
        fVar14 = DAT_08001c70;
        pfVar4 = param_2 + 2;
        *(undefined *)(param_1 + 8) = unaff_r8;
        fVar14 = (float)param_1[9] + (fVar8 - (float)param_1[9]) * fVar14;
        param_1[9] = (int)fVar14;
        *param_3 = fVar14 * fVar10 + *param_2 * (1.0 - fVar14);
        param_3[1] = (float)param_1[9] * fVar12 + param_2[1] * (1.0 - (float)param_1[9]);
        param_2 = pfVar4;
        param_3 = param_3 + 2;
      } while (pfVar3 != pfVar4);
    }
    break;
  case 4:
    param_1[0x8f] = (int)fVar17;
    uVar11 = FPMaxNum(fVar17 + fVar17,fVar13);
    fVar8 = (float)FPMinNum(uVar11,0x3f800000);
    uVar11 = libm_expf((float)param_1[0x8b] + fVar8 * ((float)param_1[0x8c] - (float)param_1[0x8b]))
    ;
    DaisySP_Svf_SetFreq(param_1 + 0x3f);
    DaisySP_Svf_SetFreq(uVar11,param_1 + 0x52);
    uVar11 = FPMaxNum((float)param_1[0x8f] * 2.0 + -1.0,fVar13);
    fVar13 = (float)FPMinNum(uVar11,0x3f800000);
    uVar11 = libm_expf((float)param_1[0x8d] + fVar13 * ((float)param_1[0x8e] - (float)param_1[0x8d])
                      );
    DaisySP_Svf_SetFreq(param_1 + 0x65);
    DaisySP_Svf_SetFreq(uVar11,param_1 + 0x78);
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        DaisySP_Svf_Process(*param_2,param_1 + 0x3f);
        pfVar3 = param_2 + 1;
        param_2 = param_2 + 2;
        DaisySP_Svf_Process(*pfVar3,param_1 + 0x52);
        iVar5 = param_1[0x5e];
        DaisySP_Svf_Process(param_1[0x4b],param_1 + 0x65);
        DaisySP_Svf_Process(iVar5,param_1 + 0x78);
        fVar13 = (float)libm_tanhf(param_1[0x72]);
        fVar8 = (float)libm_tanhf(param_1[0x85]);
        param_3[1] = fVar8;
        bVar7 = param_4 - 1U >> 1 != uVar6;
        *param_3 = fVar13;
        uVar6 = uVar6 + 1;
        param_3 = param_3 + 2;
      } while (bVar7);
    }
    break;
  case 5:
    bVar7 = fVar17 < DAT_08001c70;
    *(bool *)(param_1 + 0xeb) =
         fVar17 != DAT_08001c6c && fVar17 < DAT_08001c6c == (NAN(fVar17) || NAN(DAT_08001c6c));
    fVar13 = DAT_08001c74;
    if ((int)((uint)bVar7 << 0x1f) < 0) {
      fVar17 = fVar8;
    }
    fVar8 = (float)libm_expf(fVar17);
    fVar15 = (float)((double)(fVar8 - 1.0) / DAT_08001c60) * 1.25;
    fVar14 = fVar15 * DAT_08001c78 + 0.5;
    fVar8 = DAT_08001c6c + fVar15 * DAT_08001c7c;
    fVar12 = fVar15 * 15.0;
    fVar10 = fVar15 * DAT_08001c84;
    fVar9 = fVar15 * DAT_08001c80;
    fVar17 = fVar15 * DAT_08001ca0;
    param_1[0xab] = (int)fVar12;
    param_1[0xac] = (int)fVar14;
    param_1[0xb7] = (int)fVar14;
    param_1[0xc2] = (int)fVar14;
    param_1[0xb6] = (int)fVar12;
    param_1[0xc1] = (int)fVar12;
    param_1[0x95] = (int)fVar10;
    param_1[0x96] = (int)fVar8;
    param_1[0xa1] = (int)fVar8;
    param_1[0xa0] = (int)fVar10;
    param_1[0xd5] = (int)fVar17;
    param_1[0xdd] = (int)fVar9;
    param_1[0xe2] = (int)(fVar15 * fVar13);
    DaisySP_ATone_CalculateCoefficients(param_1 + 0xdf);
    param_1[0xe8] = (int)(fVar15 * fVar13);
    DaisySP_ATone_CalculateCoefficients(param_1 + 0xe5);
    param_1[0x91] = (int)(-fVar15 * DAT_08001c88 + 1.0);
    if (param_4 != 0) {
      uVar6 = 0;
      do {
        DB_Vinyl_ProcessStereo(*param_2,param_1 + 0x90,param_3,param_3 + 1);
        bVar7 = uVar6 != param_4 - 1U >> 1;
        param_2 = param_2 + 2;
        param_3 = param_3 + 2;
        uVar6 = uVar6 + 1;
      } while (bVar7);
      return;
    }
  }
  return;
}



/* === 08001e4c DB_Engine_MapControlsAndProcess === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Engine_MapControlsAndProcess(int param_1,float *param_2,undefined4 *param_3,uint param_4)

{
  longlong lVar1;
  int iVar2;
  int iVar3;
  char cVar4;
  float *pfVar5;
  uint uVar6;
  byte bVar7;
  char cVar8;
  uint uVar9;
  undefined4 *puVar10;
  float *pfVar11;
  undefined4 *puVar12;
  bool bVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  uint uVar18;
  undefined4 uVar19;
  undefined8 uVar20;
  undefined *local_70 [3];
  uint local_64;
  float local_5c [4];
  float local_4c;
  float fStack_48;
  float fStack_44;
  
  fVar14 = DAT_08002050;
  fVar17 = *(float *)(param_1 + 0x6c8) + *(float *)(param_1 + 0x54) * *(float *)(param_1 + 0x6cc);
  fVar15 = *(float *)(param_1 + 0x30);
  uVar18 = FPToFixed(*(undefined4 *)(param_1 + 0x28),0x20,0x20,3,0,3);
  iVar3 = *(int *)(param_1 + 4);
  *(float *)(param_1 + 0x54) = fVar17;
  *(float *)(param_1 + 0x104) = (float)(ulonglong)uVar18;
  uVar19 = FPMaxNum(fVar17 + fVar15,fVar14);
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x18) = uVar19;
  if (iVar3 == 0) {
    uVar19 = FPMaxNum(fVar15 + fVar17 * *(float *)(param_1 + 0x3c),fVar14);
    uVar19 = FPMinNum(uVar19,0x3f800000);
    *(undefined4 *)(param_1 + 0x18) = uVar19;
  }
  fVar14 = DAT_08002050;
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x68) + *(float *)(param_1 + 0x50),DAT_08002050);
  fVar16 = (float)FPMinNum(uVar19,0x3f800000);
  bVar13 = fVar16 < DAT_08002034;
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x34) +
                    *(float *)(param_1 + 0x58) * *(float *)(param_1 + 0x40),DAT_08002050);
  fVar17 = (float)FPMinNum(uVar19,0x3f800000);
  uVar19 = FPMaxNum(*(float *)(param_1 + 0x38) +
                    *(float *)(param_1 + 0x5c) * *(float *)(param_1 + 0x44),DAT_08002050);
  *(float *)(param_1 + 0x1c) = fVar17;
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x20) = uVar19;
  if ((int)((uint)bVar13 << 0x1f) < 0) {
    *(float *)(param_1 + 0x2c) = fVar14;
  }
  else {
    *(uint *)(param_1 + 0x2c) =
         (uint)(fVar16 != DAT_0800205c) * 0x3f800000 + (uint)(fVar16 == DAT_0800205c) * (int)fVar16;
  }
  cVar4 = *(char *)(param_1 + 0x7c);
  *(undefined *)(param_1 + 0x2e0) = *(undefined *)(param_1 + 0x10);
  *(undefined4 *)(param_1 + 0x2e4) = uVar19;
  fVar14 = DAT_08002050;
  *(char *)(param_1 + 0x1b4) = cVar4;
  uVar19 = FPMaxNum(*(float *)(param_1 + 100) + *(float *)(param_1 + 0x4c),fVar14);
  *(undefined *)(param_1 + 0x75) = 0;
  uVar19 = FPMinNum(uVar19,0x3f800000);
  *(undefined4 *)(param_1 + 0x28) = uVar19;
  pfVar5 = DAT_08002048;
  local_70[0] = (undefined *)param_3;
  local_64 = param_4;
  if (iVar3 != 0) {
    if (iVar3 != 1) goto LAB_08002096;
    *(float *)(param_1 + 0x19c) = fVar14;
    *(float *)(param_1 + 0x1a0) = fVar14;
    fVar16 = DAT_08002038;
    local_5c[0] = *pfVar5;
    local_5c[1] = pfVar5[1];
    local_5c[2] = pfVar5[2];
    local_5c[3] = pfVar5[3];
    local_4c = pfVar5[4];
    fStack_48 = pfVar5[5];
    fStack_44 = pfVar5[6];
    if ((*(char *)(param_1 + 0xe) == '\0') && (*(char *)(param_1 + 0x13) == '\0')) {
      *(float *)(param_1 + 0x158) = fVar17;
    }
    else {
      *(float *)(param_1 + 0x158) = fVar14;
      if (-1 < (int)((uint)(fVar17 < fVar16) << 0x1f)) goto LAB_08001fae;
    }
    fVar17 = DAT_08002394;
LAB_08001fae:
    fVar16 = DAT_08002050;
    fVar14 = DAT_0800203c;
    pfVar11 = local_5c;
    *(float *)(param_1 + 0x120) = fVar17;
    fVar14 = (float)libm_expf(fVar16 + fVar15 * fVar14);
    fVar14 = fVar14 * 0.125;
    iVar3 = 1;
    pfVar5 = pfVar11;
    do {
      fVar15 = *pfVar5;
      pfVar5 = pfVar5 + 1;
      if (iVar3 == 1) {
        if ((int)((uint)(fVar15 - fVar15 * DAT_08002044 < fVar14) << 0x1f) < 0) {
          bVar13 = fVar15 + fVar15 * DAT_08002044 != fVar14;
          fVar14 = (float)((uint)bVar13 * (int)fVar15 + (uint)!bVar13 * (int)fVar14);
        }
      }
      else {
        if ((int)((uint)(fVar15 - fVar15 * DAT_08002040 < fVar14) << 0x1f) < 0) {
          bVar13 = fVar15 + fVar15 * DAT_08002040 != fVar14;
          fVar14 = (float)((uint)bVar13 * (int)fVar15 + (uint)!bVar13 * (int)fVar14);
        }
        if (iVar3 == 7) goto LAB_080022c2;
      }
      iVar3 = iVar3 + 1;
    } while( true );
  }
  if ((*(char *)(param_1 + 0xc) != '\0') || (*(char *)(param_1 + 0x12) != '\0')) {
    fVar14 = *(float *)(param_1 + 0x18);
  }
  *(float *)(param_1 + 0x19c) = fVar14;
  if (*(char *)(param_1 + 0xf) == '\0') {
    bVar13 = *(char *)(param_1 + 0x13) == '\0';
    fVar17 = (float)((uint)bVar13 * (int)DAT_08002394 + (uint)!bVar13 * (int)fVar17);
  }
  *(float *)(param_1 + 0x1a0) = fVar17;
  *(undefined4 *)(param_1 + 0x120) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  goto LAB_08002096;
LAB_080022c2:
  fVar15 = (float)libm_powf(0x40000000,*(float *)(param_1 + 0x54) * 5.0);
  iVar3 = 1;
  fVar14 = fVar14 * fVar15;
  do {
    fVar15 = *pfVar11;
    pfVar11 = pfVar11 + 1;
    if (iVar3 == 1) {
      if (((int)((uint)(fVar15 - fVar15 * DAT_0800239c < fVar14) << 0x1f) < 0) &&
         (fVar17 = fVar15 + fVar15 * DAT_0800239c,
         fVar17 != fVar14 && fVar17 < fVar14 == (NAN(fVar17) || NAN(fVar14)))) {
LAB_080022fe:
        *(undefined *)(param_1 + 0x75) = 1;
        fVar14 = fVar15;
        goto LAB_08002308;
      }
    }
    else {
      if (((int)((uint)(fVar15 - fVar15 * DAT_08002398 < fVar14) << 0x1f) < 0) &&
         (fVar17 = fVar15 + fVar15 * DAT_08002398,
         fVar17 != fVar14 && fVar17 < fVar14 == (NAN(fVar17) || NAN(fVar14)))) goto LAB_080022fe;
LAB_08002308:
      if (iVar3 == 7) break;
    }
    iVar3 = iVar3 + 1;
  } while( true );
  bVar7 = *(byte *)(param_1 + 0x75);
  if (fVar14 != 8.0 && fVar14 < 8.0 == NAN(fVar14)) {
    bVar7 = bVar7 | 1;
  }
  *(byte *)(param_1 + 0x75) = bVar7;
  if (*(int *)(param_1 + 4) == 0) {
    if (*(char *)(param_1 + 0xc) != '\0') goto LAB_0800208a;
LAB_0800237c:
    if (*(char *)(param_1 + 0x12) != '\0') goto LAB_0800208a;
  }
  else {
    if (*(char *)(param_1 + 0xd) == '\0') goto LAB_0800237c;
LAB_0800208a:
    fVar14 = -fVar14;
  }
  cVar4 = *(char *)(param_1 + 0x1b4);
  *(float *)(param_1 + 0x13c) = fVar14;
LAB_08002096:
  *(undefined4 *)(param_1 + 0x2d4) = *(undefined4 *)(param_1 + 0x684);
  cVar8 = *(char *)(param_1 + 0x11);
  if (cVar8 == '\0') {
    cVar8 = *(char *)(param_1 + 0x15);
  }
  *(char *)(param_1 + 0x1b3) = cVar8;
  if (cVar4 != '\0') {
    *(char *)(param_1 + 0x1b2) = cVar8;
  }
  if (*(char *)(param_1 + 0x74) != '\0') {
    uVar20 = newlib_rand_LCG64();
    iVar3 = (int)uVar20;
    lVar1 = (longlong)DAT_08002388;
    iVar2 = iVar3 + ((int)((ulonglong)(lVar1 * iVar3) >> 0x20) - (iVar3 >> 0x1f)) * -3 + 1;
    *(int *)(param_1 + 0x2dc) = iVar2;
    uVar18 = newlib_rand_LCG64(iVar2,(int)((ulonglong)uVar20 >> 0x20),(int)(lVar1 * iVar3));
    fVar15 = *(float *)(param_1 + 0x2e4) * DAT_0800238c;
    *(undefined *)(param_1 + 0x74) = 0;
    *(undefined4 *)(param_1 + 0x1b8) = *(undefined4 *)(param_1 + 0x90);
    fVar14 = DAT_08002390;
    uVar6 = (uint)(0.0 < fVar15) * (int)fVar15;
    *(undefined *)(param_1 + 0x1b1) = 1;
    *(float *)(param_1 + 0x2f0) = (float)(ulonglong)(uVar18 - (uVar18 / uVar6) * uVar6) / fVar14;
  }
  uVar18 = local_64;
  uVar6 = local_64 * 4 + 7 & 0xfffffff8;
  DB_Buffer_ProcessBlock(param_1 + 0x98,param_2,(int)local_70 + -uVar6);
  DB_Corrupt_ProcessInterleaved
            (param_1 + 0x2d4,(int)local_70 + -uVar6,(int)local_70 + uVar6 * -2,uVar18);
  uVar9 = (uVar18 >> 1) * 4 + 7 & 0xfffffff8;
  local_70[2] = (undefined *)((int)local_70 + (uVar6 * -2 - uVar9));
  local_70[1] = (undefined *)((int)local_70 + uVar9 * -2 + uVar6 * -2);
  if (uVar18 != 0) {
    uVar18 = 0;
    pfVar5 = (float *)((int)local_70 + uVar6 * -2);
    do {
      fVar14 = DAT_08002050;
      fVar15 = *(float *)(param_1 + 0x2c);
      if (((int)((uint)(*(float *)(param_1 + 0x2c) < DAT_0800204c) << 0x1f) < 0) &&
         (*(float *)(param_1 + 0x2c) = DAT_08002050, fVar15 = fVar14,
         *(char *)(param_1 + 0x1b3) != '\0')) {
        fVar15 = 1.0;
        *(undefined4 *)(param_1 + 0x2c) = 0x3f800000;
      }
      fVar14 = DAT_08002058;
      uVar6 = uVar18 >> 1;
      puVar12 = (undefined4 *)(local_70[2] + uVar6 * 4);
      fVar17 = *(float *)(param_1 + 0x6c) + (fVar15 - *(float *)(param_1 + 0x6c)) * DAT_08002054;
      uVar18 = uVar18 + 2;
      puVar10 = (undefined4 *)(local_70[1] + uVar6 * 4);
      *(float *)(param_1 + 0x6c) = fVar17;
      fVar15 = (float)libm_sinf((1.0 - fVar17) * fVar14);
      fVar14 = (float)libm_sinf(fVar17 * fVar14);
      uVar19 = DaisySP_Tone_Process(fVar14 * *pfVar5 + *param_2 * fVar15,param_1 + 0x688);
      fVar17 = pfVar5[1];
      *puVar12 = uVar19;
      uVar19 = DaisySP_Tone_Process(fVar14 * fVar17 + param_2[1] * fVar15,param_1 + 0x6a4);
      *puVar10 = uVar19;
      uVar19 = DaisySP_CrossFade_Process(param_1 + 0x6d0,puVar12,puVar10);
      *puVar12 = uVar19;
      uVar19 = DaisySP_CrossFade_Process(param_1 + 0x6d0,puVar10,puVar12);
      *puVar10 = uVar19;
      param_2 = param_2 + 2;
      pfVar5 = pfVar5 + 2;
    } while (uVar18 < local_64);
    uVar18 = 0;
    do {
      uVar6 = uVar18 >> 1;
      uVar18 = uVar18 + 2;
      uVar19 = *(undefined4 *)(local_70[1] + uVar6 * 4);
      *(undefined4 *)local_70[0] = *(undefined4 *)(local_70[2] + uVar6 * 4);
      *(undefined4 *)((int)local_70[0] + 4) = uVar19;
      local_70[0] = (undefined *)((int)local_70[0] + 8);
    } while (uVar18 < local_64);
  }
  return;
}



/* === 080023a0 DB_Controls_Poll === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Controls_Poll(int param_1)

{
  char cVar1;
  ulonglong uVar2;
  longlong lVar3;
  int iVar4;
  undefined uVar5;
  byte bVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined uVar9;
  int iVar10;
  int *piVar11;
  uint uVar12;
  float *pfVar13;
  undefined4 uVar14;
  int iVar15;
  int iVar16;
  uint uVar17;
  float *pfVar18;
  int iVar19;
  float *pfVar20;
  undefined4 *puVar21;
  bool bVar22;
  undefined4 uVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  float fVar28;
  float local_9c;
  float local_98;
  float local_94;
  int local_90 [4];
  float local_80;
  float local_7c;
  float local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined auStack_68 [12];
  undefined auStack_5c [12];
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined auStack_44 [12];
  undefined auStack_38 [12];
  float local_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  uVar7 = System_GetNow();
  iVar19 = *(int *)(param_1 + 0xc);
  iVar15 = iVar19 + 0x1d0;
  do {
    iVar16 = iVar15 + 0x20;
    AnalogControl_Process(iVar15);
    iVar15 = iVar16;
  } while (iVar19 + 0x290 != iVar16);
  iVar15 = iVar19 + 0x290;
  do {
    iVar16 = iVar15 + 0x20;
    AnalogControl_Process(iVar15);
    iVar15 = iVar16;
  } while (iVar19 + 0x350 != iVar16);
  iVar19 = 0x74;
  iVar15 = 0;
  do {
    while( true ) {
      iVar16 = *(int *)(param_1 + 0xc);
      Switch_Debounce(iVar16 + iVar19);
      iVar16 = iVar16 + iVar15 * 0x24;
      if (*(char *)(iVar16 + 0x78) != '\0') break;
LAB_0800240a:
      iVar15 = iVar15 + 1;
      iVar19 = iVar19 + 0x24;
      if (iVar15 == 7) goto LAB_08002480;
    }
    cVar1 = *(char *)(iVar16 + 0x90);
    if (cVar1 == '\x7f') {
      if (iVar15 == 0) {
        *(uint *)(param_1 + 0x200) = uVar7;
      }
      iVar16 = *(int *)(param_1 + 500);
      iVar10 = param_1 + 0xf0 + iVar16 * 8;
      *(undefined *)(param_1 + 0xf0 + iVar16 * 8) = 4;
      *(short *)(iVar10 + 2) = (short)iVar15;
      *(undefined4 *)(iVar10 + 4) = 0;
      *(uint *)(param_1 + 500) = iVar16 + 1U & 0x1f;
      uVar8 = System_GetNow();
      *(undefined4 *)(param_1 + 0xec) = uVar8;
      goto LAB_0800240a;
    }
    if (cVar1 != -0x80) goto LAB_0800240a;
    if (iVar15 == 0) {
      *(uint *)(param_1 + 0x200) = uVar7;
    }
    iVar10 = *(int *)(param_1 + 500);
    iVar19 = iVar19 + 0x24;
    *(undefined *)(param_1 + 0xf0 + iVar10 * 8) = 4;
    iVar16 = param_1 + 0xf0 + iVar10 * 8;
    *(short *)(iVar16 + 2) = (short)iVar15;
    iVar15 = iVar15 + 1;
    *(undefined4 *)(iVar16 + 4) = 0x3f800000;
    *(uint *)(param_1 + 500) = iVar10 + 1U & 0x1f;
    uVar8 = System_GetNow();
    *(undefined4 *)(param_1 + 0xec) = uVar8;
  } while (iVar15 != 7);
LAB_08002480:
  iVar19 = *(int *)(param_1 + 0xc);
  pfVar20 = (float *)(param_1 + 0x84);
  iVar16 = 0;
  iVar15 = param_1;
  do {
    iVar4 = DAT_08002664;
    iVar10 = DAT_08002660;
    fVar24 = *(float *)(iVar19 + iVar16 * 0x20 + 0x1dc);
    if (iVar16 != 3) {
      fVar24 = fVar24 * DAT_0800265c;
    }
    fVar27 = *(float *)(iVar15 + 0xa0) * *(float *)(iVar15 + 0xa4) +
             fVar24 * *(float *)(iVar15 + 0x9c);
    fVar25 = fVar24 - fVar27;
    *(float *)(iVar15 + 0xa4) = fVar27;
    *pfVar20 = fVar25;
    pfVar20 = pfVar20 + 1;
    fVar25 = ABS(fVar25);
    bVar22 = *(char *)(iVar19 + iVar16 * 0x24 + 0x90) == -1;
    fVar27 = (float)((uint)bVar22 * iVar10 + (uint)!bVar22 * iVar4);
    if ((fVar25 != fVar27 && fVar25 < fVar27 == (NAN(fVar25) || NAN(fVar27))) &&
       (200 < uVar7 - *(int *)(param_1 + 0x200))) {
      iVar10 = *(int *)(param_1 + 500);
      *(undefined *)(param_1 + 0xf0 + iVar10 * 8) = 0;
      iVar19 = param_1 + 0xf0 + iVar10 * 8;
      *(short *)(iVar19 + 2) = (short)iVar16;
      *(float *)(iVar19 + 4) = fVar24;
      *(uint *)(param_1 + 500) = iVar10 + 1U & 0x1f;
      uVar8 = System_GetNow();
      *(undefined4 *)(param_1 + 0xec) = uVar8;
      iVar19 = *(int *)(param_1 + 0xc);
    }
    iVar16 = iVar16 + 1;
    iVar15 = iVar15 + 0xc;
  } while (iVar16 != 6);
  iVar15 = *(int *)(param_1 + 0x10);
  *(undefined4 *)(iVar15 + 100) = *(undefined4 *)(iVar19 + 700);
  *(undefined4 *)(iVar15 + 0x68) = *(undefined4 *)(iVar19 + 0x2dc);
  *(undefined4 *)(iVar15 + 0x54) = *(undefined4 *)(iVar19 + 0x2fc);
  *(undefined4 *)(iVar15 + 0x58) = *(undefined4 *)(iVar19 + 0x31c);
  *(undefined4 *)(iVar15 + 0x5c) = *(undefined4 *)(iVar19 + 0x33c);
  *(undefined4 *)(*(int *)(param_1 + 0x14) + 0x3c) = *(undefined4 *)(iVar19 + 0x29c);
  iVar15 = GateIn_Trig();
  if (iVar15 == 0) {
    iVar19 = *(int *)(param_1 + 0x10);
  }
  else if (*(char *)(param_1 + 0x229) == '\0') {
    iVar19 = *(int *)(param_1 + 0x10);
    iVar15 = *(int *)(iVar19 + 0x684) + 1;
    if (*(int *)(iVar19 + 8) == 1) {
      iVar16 = 4;
    }
    else {
      iVar16 = 6;
    }
    iVar15 = iVar15 - iVar16 * (iVar15 / iVar16);
    if (iVar15 == 0) {
      iVar15 = 1;
    }
    *(int *)(iVar19 + 0x684) = iVar15;
  }
  else {
    piVar11 = *(int **)(param_1 + 0x14);
    if (*piVar11 == 1) {
      iVar19 = *(int *)(param_1 + 0x10);
      fVar24 = (float)FPMinNum(0x3f800000,piVar11[0x12]);
      piVar11[0xb] = (uint)(0.0 < 1.0 / fVar24) * (int)(1.0 / fVar24);
    }
    else {
      iVar19 = *(int *)(param_1 + 0x10);
      piVar11[3] = 0;
    }
  }
  iVar15 = *(int *)(param_1 + 0xc);
  if (*(char *)(param_1 + 0x22a) == '\0') {
    *(undefined2 *)(iVar19 + 0x12) = 0;
    *(undefined2 *)(iVar19 + 0x14) = 0;
    iVar15 = GateIn_Trig(iVar15 + 0x188);
    if (iVar15 == 0) {
LAB_0800261c:
      iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x1a0);
    }
    else {
      iVar15 = *(int *)(param_1 + 0x10);
      if (*(int *)(iVar15 + 4) == 0) {
        if (*(char *)(iVar15 + 0xc) == '\0') {
          *(byte *)(iVar15 + 0xc) = *(byte *)(iVar15 + 0x12) ^ 1;
        }
        else {
          *(undefined *)(iVar15 + 0xc) = 0;
        }
        goto LAB_0800261c;
      }
      if (*(char *)(iVar15 + 0xd) == '\0') {
        *(byte *)(iVar15 + 0xd) = *(byte *)(iVar15 + 0x12) ^ 1;
      }
      else {
        *(undefined *)(iVar15 + 0xd) = 0;
      }
      iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x1a0);
    }
    if (iVar15 != 0) {
      iVar15 = *(int *)(param_1 + 0x10);
      if (*(int *)(iVar15 + 4) == 0) {
        bVar6 = 0;
        if (*(char *)(iVar15 + 0xf) == '\0') {
          bVar6 = *(byte *)(iVar15 + 0x13) ^ 1;
        }
        *(byte *)(iVar15 + 0xf) = bVar6;
      }
      else if (*(char *)(iVar15 + 0xe) == '\0') {
        *(byte *)(iVar15 + 0xe) = *(byte *)(iVar15 + 0x13) ^ 1;
      }
      else {
        *(undefined *)(iVar15 + 0xe) = 0;
      }
    }
    iVar15 = GateIn_Trig(*(int *)(param_1 + 0xc) + 0x170);
    iVar19 = *(int *)(param_1 + 0x10);
    if (iVar15 != 0) {
      if (*(char *)(iVar19 + 0x11) == '\0') {
        *(byte *)(iVar19 + 0x11) = *(byte *)(iVar19 + 0x15) ^ 1;
      }
      else {
        *(undefined *)(iVar19 + 0x11) = 0;
      }
    }
  }
  else {
    if (*(char *)(iVar15 + 0x19e) == '\0') {
      uVar5 = GPIO_Read(iVar15 + 0x188);
      iVar10 = *(int *)(param_1 + 0xc);
      iVar16 = *(int *)(param_1 + 0x10);
      *(undefined *)(iVar19 + 0x12) = uVar5;
      iVar15 = iVar10 + 0x170;
      if (*(char *)(iVar10 + 0x186) == '\0') goto LAB_080028fc;
LAB_08002562:
      bVar6 = GPIO_Read(iVar15);
      iVar15 = *(int *)(param_1 + 0xc);
      iVar10 = *(int *)(param_1 + 0x10);
      cVar1 = *(char *)(iVar15 + 0x1b6);
      *(byte *)(iVar16 + 0x15) = bVar6 ^ 1;
      iVar15 = iVar15 + 0x1a0;
      if (cVar1 == '\0') goto LAB_08002914;
LAB_08002580:
      bVar6 = GPIO_Read(iVar15);
      bVar6 = bVar6 ^ 1;
    }
    else {
      bVar6 = GPIO_Read(iVar15 + 0x188);
      iVar15 = *(int *)(param_1 + 0xc);
      iVar16 = *(int *)(param_1 + 0x10);
      cVar1 = *(char *)(iVar15 + 0x186);
      *(byte *)(iVar19 + 0x12) = bVar6 ^ 1;
      iVar15 = iVar15 + 0x170;
      if (cVar1 != '\0') goto LAB_08002562;
LAB_080028fc:
      uVar5 = GPIO_Read(iVar15);
      iVar19 = *(int *)(param_1 + 0xc);
      iVar10 = *(int *)(param_1 + 0x10);
      *(undefined *)(iVar16 + 0x15) = uVar5;
      iVar15 = iVar19 + 0x1a0;
      if (*(char *)(iVar19 + 0x1b6) != '\0') goto LAB_08002580;
LAB_08002914:
      bVar6 = GPIO_Read(iVar15);
    }
    iVar19 = *(int *)(param_1 + 0x10);
    *(byte *)(iVar10 + 0x13) = bVar6;
  }
  iVar15 = *(int *)(iVar19 + 4);
  if (iVar15 != 0) {
    iVar15 = (uint)(0.0 < *(float *)(iVar19 + 0x284)) * (int)*(float *)(iVar19 + 0x284);
  }
  iVar19 = *(int *)(param_1 + 0x21c);
  uVar5 = *(undefined *)(param_1 + 0x1f8);
  *(int *)(param_1 + 0x21c) = iVar15;
  if (iVar19 != iVar15) {
    *(uint *)(param_1 + 0x220) = uVar7;
  }
  fVar24 = DAT_08002ec0;
  switch(uVar5) {
  case 0:
    if (*(char *)(param_1 + 0x210) == '\0') {
      piVar11 = *(int **)(param_1 + 0x14);
      if (*piVar11 == 0) {
        iVar15 = *(int *)(param_1 + 0xc) + 0x590;
        if (-1 < (int)((uint)((float)piVar11[3] / DAT_08002ec4 < 0.5) << 0x1f)) goto LAB_08002eb4;
        puVar21 = (undefined4 *)(param_1 + 0x18);
      }
      else {
        iVar15 = *(int *)(param_1 + 0xc);
        iVar19 = System_GetNow();
        puVar21 = (undefined4 *)(param_1 + 0x54);
        iVar15 = iVar15 + 0x590;
        uVar17 = iVar19 - piVar11[8];
        uVar12 = piVar11[9];
        if (1.0 < (float)piVar11[0x12]) {
          uVar17 = uVar17 - uVar12 * (uVar17 / uVar12);
        }
        if (uVar12 >> 1 <= uVar17) {
LAB_08002eb4:
          puVar21 = (undefined4 *)(param_1 + 0x60);
        }
      }
      uVar8 = puVar21[1];
      *(undefined4 *)(iVar15 + 0xc) = *puVar21;
      uVar23 = puVar21[2];
      *(undefined4 *)(iVar15 + 0x10) = uVar8;
      *(undefined4 *)(iVar15 + 0x14) = uVar23;
      FUN_08000bac(iVar15);
    }
    else {
      local_80 = 0.0;
      local_7c = 0.0;
      local_78 = 0.0;
      FUN_08009b90(DAT_08002ecc,DAT_08002ecc,&local_80);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x5a0) = local_7c;
      *(float *)(iVar15 + 0x59c) = local_80;
      *(float *)(iVar15 + 0x5a4) = local_78;
      FUN_08000bac();
    }
    if (**(int **)(param_1 + 0x14) == 1) {
      fVar24 = (float)(*(int **)(param_1 + 0x14))[0x12];
      if (*(float *)(param_1 + 0xe8) == fVar24) {
        *(float *)(param_1 + 0xe8) = fVar24;
        if (0x4f < uVar7 - *(int *)(param_1 + 0xe4)) goto LAB_08002bec;
      }
      else {
        *(uint *)(param_1 + 0xe4) = uVar7;
        *(float *)(param_1 + 0xe8) = fVar24;
      }
      iVar15 = *(int *)(param_1 + 0xc);
      uVar8 = *(undefined4 *)(param_1 + 0x30);
      *(undefined4 *)(iVar15 + 0x5a0) = *(undefined4 *)(param_1 + 0x34);
      uVar23 = *(undefined4 *)(param_1 + 0x38);
      *(undefined4 *)(iVar15 + 0x59c) = uVar8;
      *(undefined4 *)(iVar15 + 0x5a4) = uVar23;
      FUN_08000bac();
    }
LAB_08002bec:
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(param_1 + 0x60);
    *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 100);
    uVar23 = *(undefined4 *)(param_1 + 0x68);
    *(undefined4 *)(iVar15 + 0x584) = uVar8;
    *(undefined4 *)(iVar15 + 0x58c) = uVar23;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0x10);
    if (*(int *)(iVar15 + 4) == 0) {
      iVar19 = *(int *)(param_1 + 0xc) + 0x5c0;
      if ((*(char *)(iVar15 + 0xc) == '\0') && (*(char *)(iVar15 + 0x12) == '\0')) {
        puVar21 = (undefined4 *)(param_1 + 0x60);
      }
      else {
        puVar21 = (undefined4 *)(param_1 + 0x18);
      }
    }
    else {
      if ((*(char *)(iVar15 + 0xd) == '\0') && (*(char *)(iVar15 + 0x12) == '\0')) {
        if (*(char *)(iVar15 + 0x75) == '\0') {
          iVar15 = *(int *)(param_1 + 0xc);
          *(undefined4 *)(iVar15 + 0x5cc) = 0;
          *(undefined4 *)(iVar15 + 0x5d0) = 0;
          *(undefined4 *)(iVar15 + 0x5d4) = 0x3f800000;
          FUN_08000bac();
        }
        else {
          iVar15 = *(int *)(param_1 + 0xc);
          *(undefined4 *)(iVar15 + 0x5cc) = 0;
          uVar8 = DAT_08002ebc;
          *(undefined4 *)(iVar15 + 0x5d4) = 0x3f800000;
          *(undefined4 *)(iVar15 + 0x5d0) = uVar8;
          FUN_08000bac();
        }
        goto LAB_08002ef2;
      }
      iVar19 = *(int *)(param_1 + 0xc) + 0x5c0;
      if (*(char *)(iVar15 + 0x75) == '\0') {
        puVar21 = (undefined4 *)(param_1 + 0x24);
      }
      else {
        puVar21 = (undefined4 *)(param_1 + 0x30);
      }
    }
    *(undefined4 *)(iVar19 + 0xc) = *puVar21;
    uVar8 = puVar21[2];
    *(undefined4 *)(iVar19 + 0x10) = puVar21[1];
    *(undefined4 *)(iVar19 + 0x14) = uVar8;
    FUN_08000bac();
LAB_08002ef2:
    iVar15 = *(int *)(param_1 + 0xc);
    iVar19 = *(int *)(param_1 + 0x10);
    if (*(int *)(iVar19 + 4) == 0) {
      cVar1 = *(char *)(iVar19 + 0xf);
    }
    else {
      cVar1 = *(char *)(iVar19 + 0xe);
    }
    if ((cVar1 == '\0') && (*(char *)(iVar19 + 0x13) == '\0')) {
      puVar21 = (undefined4 *)(param_1 + 0x60);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    *(undefined4 *)(iVar15 + 0x5e4) = *puVar21;
    uVar8 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x5e8) = puVar21[1];
    *(undefined4 *)(iVar15 + 0x5ec) = uVar8;
    FUN_08000bac();
    if (uVar7 - *(int *)(param_1 + 0x220) < 0x50) {
      iVar15 = *(int *)(param_1 + 0xc);
      uVar8 = *(undefined4 *)(param_1 + 0x34);
      uVar23 = *(undefined4 *)(param_1 + 0x38);
      *(undefined4 *)(iVar15 + 0x5e4) = *(undefined4 *)(param_1 + 0x30);
      *(undefined4 *)(iVar15 + 0x5e8) = uVar8;
      *(undefined4 *)(iVar15 + 0x5ec) = uVar23;
      FUN_08000bac();
    }
    iVar15 = *(int *)(param_1 + 0xc);
    if ((*(char *)(*(int *)(param_1 + 0x10) + 0x11) == '\0') &&
       (*(char *)(*(int *)(param_1 + 0x10) + 0x15) == '\0')) {
      puVar21 = (undefined4 *)(param_1 + 0x60);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    uVar8 = puVar21[1];
    uVar23 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x614) = *puVar21;
    *(undefined4 *)(iVar15 + 0x618) = uVar8;
    *(undefined4 *)(iVar15 + 0x61c) = uVar23;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0xc);
    if (*(int *)(*(int *)(param_1 + 0x10) + 4) == 0) {
      puVar21 = (undefined4 *)(param_1 + 0x18);
    }
    else {
      puVar21 = (undefined4 *)(param_1 + 0x24);
    }
    uVar8 = puVar21[1];
    uVar23 = puVar21[2];
    *(undefined4 *)(iVar15 + 0x5b4) = *puVar21;
    *(undefined4 *)(iVar15 + 0x5bc) = uVar23;
    *(undefined4 *)(iVar15 + 0x5b8) = uVar8;
    FUN_08000bac();
    iVar15 = *(int *)(param_1 + 0xc);
    switch(*(undefined4 *)(*(int *)(param_1 + 0x10) + 0x2d4)) {
    case 1:
      uVar8 = *(undefined4 *)(param_1 + 0x18);
      uVar23 = *(undefined4 *)(param_1 + 0x1c);
      uVar14 = *(undefined4 *)(param_1 + 0x20);
      break;
    case 2:
      uVar8 = *(undefined4 *)(param_1 + 0x24);
      uVar23 = *(undefined4 *)(param_1 + 0x28);
      uVar14 = *(undefined4 *)(param_1 + 0x2c);
      break;
    case 3:
      uVar8 = *(undefined4 *)(param_1 + 0x30);
      uVar23 = *(undefined4 *)(param_1 + 0x34);
      uVar14 = *(undefined4 *)(param_1 + 0x38);
      break;
    case 4:
      uVar8 = *(undefined4 *)(param_1 + 0x3c);
      uVar23 = *(undefined4 *)(param_1 + 0x40);
      uVar14 = *(undefined4 *)(param_1 + 0x44);
      break;
    case 5:
      uVar8 = *(undefined4 *)(param_1 + 0x48);
      uVar23 = *(undefined4 *)(param_1 + 0x4c);
      uVar14 = *(undefined4 *)(param_1 + 0x50);
      break;
    default:
      uVar8 = *(undefined4 *)(param_1 + 0x60);
      uVar23 = *(undefined4 *)(param_1 + 100);
      uVar14 = *(undefined4 *)(param_1 + 0x68);
    }
    *(undefined4 *)(iVar15 + 0x5fc) = uVar8;
    *(undefined4 *)(iVar15 + 0x600) = uVar23;
    *(undefined4 *)(iVar15 + 0x604) = uVar14;
    FUN_08000bac();
    return;
  case 1:
    uVar17 = System_GetNow();
    fVar24 = *(float *)(*(int *)(param_1 + 0x10) + 0x70);
    local_80 = 0.0;
    local_7c = 0.0;
    local_78 = 0.0;
    fVar25 = 1.0 - ABS(((float)(longlong)(int)(uVar17 & 0x1ff) / DAT_08002668) * 2.0 + -1.0);
    if (fVar24 == 0.0) {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x584) = *(undefined4 *)(param_1 + 0x60);
      *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 100);
      *(undefined4 *)(iVar15 + 0x58c) = *(undefined4 *)(param_1 + 0x68);
      FUN_08000bac();
    }
    else if (((int)((uint)(fVar24 < DAT_0800266c) << 0x1f) < 0) &&
            (fVar24 != DAT_08002670 && fVar24 < DAT_08002670 == (NAN(fVar24) || NAN(DAT_08002670))))
    {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x584) = *(undefined4 *)(param_1 + 0x18);
      *(undefined4 *)(iVar15 + 0x588) = *(undefined4 *)(param_1 + 0x1c);
      *(undefined4 *)(iVar15 + 0x58c) = *(undefined4 *)(param_1 + 0x20);
      FUN_08000bac();
    }
    else {
      FUN_08009b90(fVar24,fVar24,&local_80);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x584) = local_80;
      *(float *)(iVar15 + 0x588) = local_7c;
      *(float *)(iVar15 + 0x58c) = local_78;
      FUN_08000bac();
    }
    uVar9 = uVar5;
    if (*(char *)(param_1 + 0x228) == '\0') {
      uVar9 = 2;
    }
    FUN_08009b70(&local_80,uVar9);
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x614) = local_80 * fVar25;
    *(float *)(iVar15 + 0x618) = fVar25 * local_7c;
    *(float *)(iVar15 + 0x61c) = fVar25 * local_78;
    FUN_08000bac();
    if (*(int *)(*(int *)(param_1 + 0x10) + 0x100) == 0) {
      uVar5 = 2;
    }
    FUN_08009b70(&local_80,uVar5);
    iVar15 = *(int *)(param_1 + 0xc);
    fVar24 = (*(float *)(*(int *)(param_1 + 0x10) + 0x3c) + DAT_08002674) * fVar25;
    *(float *)(iVar15 + 0x5d0) = fVar24 * local_7c;
    *(float *)(iVar15 + 0x5d4) = fVar24 * local_78;
    *(float *)(iVar15 + 0x5cc) = local_80 * fVar24;
    FUN_08000bac();
    if (*(int *)(*(int *)(param_1 + 0x10) + 8) == 0) {
      fVar27 = *(float *)(param_1 + 0x1c);
      fVar28 = *(float *)(param_1 + 0x20);
      fVar26 = *(float *)(param_1 + 0x18);
    }
    else {
      fVar27 = *(float *)(param_1 + 0x28);
      fVar28 = *(float *)(param_1 + 0x2c);
      fVar26 = *(float *)(param_1 + 0x24);
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5b8) = fVar25 * fVar27;
    *(float *)(iVar15 + 0x5b4) = fVar26 * fVar25;
    *(float *)(iVar15 + 0x5bc) = fVar25 * fVar28;
    FUN_08000bac();
    if (*(char *)(param_1 + 0x229) == '\0') {
      fVar24 = *(float *)(param_1 + 0x20);
      fVar27 = *(float *)(param_1 + 0x18);
      iVar15 = *(int *)(param_1 + 0xc);
      fVar26 = (*(float *)(*(int *)(param_1 + 0x10) + 0x44) + DAT_08002674) * fVar25;
      *(float *)(iVar15 + 0x600) = fVar26 * *(float *)(param_1 + 0x1c);
      *(float *)(iVar15 + 0x604) = fVar26 * fVar24;
      *(float *)(iVar15 + 0x5fc) = fVar27 * fVar26;
      FUN_08000bac();
    }
    else {
      fVar26 = *(float *)(param_1 + 0x2c);
      fVar27 = *(float *)(param_1 + 0x24);
      iVar15 = *(int *)(param_1 + 0xc);
      *(float *)(iVar15 + 0x600) = fVar24 * *(float *)(param_1 + 0x28);
      *(float *)(iVar15 + 0x5fc) = fVar27 * fVar24;
      *(float *)(iVar15 + 0x604) = fVar24 * fVar26;
      FUN_08000bac();
    }
    uVar7 = uVar7 - *(int *)(param_1 + 0x214);
    if (uVar7 < *(uint *)(param_1 + 8)) {
      iVar15 = *(int *)(param_1 + 0xc);
      uVar2 = (ulonglong)DAT_08002b54;
      *(undefined4 *)(iVar15 + 0x5e4) = 0;
      *(undefined4 *)(iVar15 + 0x5e8) = 0;
      *(float *)(iVar15 + 0x5ec) =
           (float)(ulonglong)(0x58 < uVar7 + (uint)(uVar2 * uVar7 >> 0x26) * -0xa7);
      FUN_08000bac(iVar15 + 0x5d8,(int)(uVar2 * uVar7));
    }
    else {
      iVar15 = *(int *)(param_1 + 0xc);
      fVar24 = (*(float *)(*(int *)(param_1 + 0x10) + 0x40) + DAT_08002ec8) * fVar25;
      *(float *)(iVar15 + 0x5e4) = fVar24;
      *(float *)(iVar15 + 0x5e8) = fVar24;
      *(float *)(iVar15 + 0x5ec) = fVar24;
      FUN_08000bac();
    }
    if (*(char *)(param_1 + 0x22a) == '\0') {
      fVar24 = *(float *)(param_1 + 0x1c);
      fVar26 = *(float *)(param_1 + 0x20);
      fVar27 = *(float *)(param_1 + 0x18);
    }
    else {
      fVar24 = *(float *)(param_1 + 0x28);
      fVar26 = *(float *)(param_1 + 0x2c);
      fVar27 = *(float *)(param_1 + 0x24);
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5a0) = fVar25 * fVar24;
    *(float *)(iVar15 + 0x59c) = fVar27 * fVar25;
    *(float *)(iVar15 + 0x5a4) = fVar25 * fVar26;
    FUN_08000bac();
    break;
  case 2:
    local_90[0] = *DAT_08002b58;
    local_90[1] = DAT_08002b58[1];
    local_90[2] = DAT_08002b58[2];
    local_90[3] = DAT_08002b58[3];
    FUN_08009b90(0x3f000000,DAT_08002b60,DAT_08002b5c,param_1 + 0x3c);
    FUN_08009b90(0x3f800000,DAT_08002b68,DAT_08002b64,param_1 + 0x48);
    fVar24 = DAT_08002b60;
    pfVar20 = &local_80;
    do {
      pfVar18 = pfVar20 + 3;
      *pfVar20 = DAT_08002b60;
      pfVar20[1] = DAT_08002b60;
      pfVar20[2] = DAT_08002b60;
      pfVar20 = pfVar18;
    } while ((float *)&stack0xffffffe0 != pfVar18);
    FUN_08009b70(&local_80,7);
    FUN_08009b70(&local_74,2);
    FUN_08009b70(auStack_68,1);
    FUN_08009b70(auStack_5c,0);
    local_4c = DAT_08002b6c;
    local_48 = DAT_08002b70;
    local_50 = DAT_08002b74;
    FUN_08009b90(0x3f000000,fVar24,DAT_08002b5c,auStack_44);
    FUN_08009b70(auStack_38,3);
    FUN_08009b90(0x3f800000,DAT_08002b68,DAT_08002b64,&local_2c);
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x584) = local_74;
    *(undefined4 *)(iVar15 + 0x588) = local_70;
    *(undefined4 *)(iVar15 + 0x58c) = local_6c;
    FUN_08000bac(iVar15 + 0x578);
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x59c) = local_50;
    *(undefined4 *)(iVar15 + 0x5a0) = local_4c;
    *(undefined4 *)(iVar15 + 0x5a4) = local_48;
    FUN_08000bac(iVar15 + 0x590);
    iVar15 = *(int *)(param_1 + 0xc);
    *(float *)(iVar15 + 0x5b4) = local_2c;
    *(undefined4 *)(iVar15 + 0x5b8) = local_28;
    *(undefined4 *)(iVar15 + 0x5bc) = local_24;
    FUN_08000bac(iVar15 + 0x5a8);
    uVar7 = System_GetNow();
    uVar7 = uVar7 & 0x3ff;
    uVar17 = uVar7 + 0x3fc;
    piVar11 = local_90;
    do {
      iVar15 = (int)((ulonglong)DAT_08002b78 * (ulonglong)uVar7 >> 0x20);
      local_9c = DAT_08002b60;
      local_98 = DAT_08002b60;
      local_94 = DAT_08002b60;
      iVar15 = uVar7 + (iVar15 + (uVar7 - iVar15 >> 1) >> 9) * -0x3ff;
      uVar7 = uVar7 + 0xff;
      FUN_08009b90(DAT_08002b60,
                   1.0 - (1.0 - ABS(((float)(longlong)iVar15 / DAT_08002b7c) * 2.0 + -1.0)),
                   &local_9c);
      iVar15 = *(int *)(param_1 + 0xc) + *piVar11 * 0x18;
      *(float *)(iVar15 + 0x584) = local_9c;
      *(float *)(iVar15 + 0x588) = local_98;
      *(float *)(iVar15 + 0x58c) = local_94;
      FUN_08000bac(iVar15 + 0x578);
      piVar11 = piVar11 + 1;
    } while (uVar17 != uVar7);
    return;
  case 3:
    pfVar18 = &local_80;
    pfVar20 = pfVar18;
    do {
      pfVar13 = pfVar20 + 3;
      *pfVar20 = DAT_08002ec0;
      pfVar20[1] = DAT_08002ec0;
      pfVar20[2] = DAT_08002ec0;
      pfVar20 = pfVar13;
    } while (&local_2c != pfVar13);
    iVar15 = System_GetNow();
    iVar19 = *(int *)(param_1 + 0x224);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x1dc),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x29c),fVar24,pfVar18);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x1fc),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 700),
                 (float)(ulonglong)((uint)(iVar15 - iVar19) < 100),&local_74);
    FUN_08009b90(*(undefined4 *)(*(int *)(param_1 + 0xc) + 0x21c),
                 *(undefined4 *)(*(int *)(param_1 + 0xc) + 0x2dc),fVar24,auStack_68);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x23c);
    uVar23 = *(undefined4 *)(iVar15 + 0x2fc);
    if (*(char *)(iVar15 + 0x19e) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,auStack_5c);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x25c);
    uVar23 = *(undefined4 *)(iVar15 + 0x31c);
    if (*(char *)(iVar15 + 0x1b6) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,&local_50);
    iVar15 = *(int *)(param_1 + 0xc);
    uVar8 = *(undefined4 *)(iVar15 + 0x27c);
    uVar23 = *(undefined4 *)(iVar15 + 0x33c);
    if (*(char *)(iVar15 + 0x1ce) == '\0') {
      uVar7 = GPIO_Read();
    }
    else {
      uVar7 = GPIO_Read();
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(uVar8,uVar23,(float)(ulonglong)uVar7,auStack_44);
    iVar15 = *(int *)(param_1 + 0xc) + 0x170;
    if (*(char *)(*(int *)(param_1 + 0xc) + 0x186) == '\0') {
      uVar7 = GPIO_Read(iVar15);
    }
    else {
      uVar7 = GPIO_Read(iVar15);
      uVar7 = (uVar7 ^ 1) & 0xff;
    }
    FUN_08009b90(DAT_08002ec0,DAT_08002ec0,(float)(ulonglong)uVar7,auStack_38);
    iVar15 = *(int *)(param_1 + 0xc);
    if (*(char *)(iVar15 + 0x90) == -1) {
      FUN_08009b70(pfVar18,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xb4) == -1) {
      FUN_08009b70(&local_74,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xd8) == -1) {
      FUN_08009b70(auStack_68,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x120) == -1) {
      FUN_08009b70(auStack_5c,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x144) == -1) {
      FUN_08009b70(&local_50,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0x168) == -1) {
      FUN_08009b70(auStack_44,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    if (*(char *)(iVar15 + 0xfc) == -1) {
      FUN_08009b70(auStack_38,3);
      iVar15 = *(int *)(param_1 + 0xc);
    }
    iVar16 = 0x578;
    iVar19 = 0;
    while( true ) {
      fVar27 = *pfVar18;
      fVar25 = pfVar18[1];
      iVar10 = iVar15 + iVar19 * 0x18;
      fVar24 = pfVar18[2];
      iVar15 = iVar15 + iVar16;
      pfVar18 = pfVar18 + 3;
      iVar16 = iVar16 + 0x18;
      *(float *)(iVar10 + 0x584) = fVar27;
      *(float *)(iVar10 + 0x588) = fVar25;
      *(float *)(iVar10 + 0x58c) = fVar24;
      FUN_08000bac(iVar15);
      if (iVar19 + 1 == 7) break;
      iVar15 = *(int *)(param_1 + 0xc);
      iVar19 = iVar19 + 1;
    }
    return;
  case 4:
    if (*(char *)(param_1 + 0x235) == '\0') {
      iVar15 = *(int *)(param_1 + 0xc);
      if (*(char *)(param_1 + 0x234) == '\0') {
        *(undefined4 *)(iVar15 + 0x5d0) = 0;
        *(undefined4 *)(iVar15 + 0x5cc) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d4) = 0;
        FUN_08000bac();
      }
      else {
        *(undefined4 *)(iVar15 + 0x5cc) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d0) = 0x3f800000;
        *(undefined4 *)(iVar15 + 0x5d4) = 0;
        FUN_08000bac();
      }
    }
    else {
      iVar15 = *(int *)(param_1 + 0xc);
      *(undefined4 *)(iVar15 + 0x5cc) = 0;
      *(undefined4 *)(iVar15 + 0x5d0) = 0x3f800000;
      *(undefined4 *)(iVar15 + 0x5d4) = 0;
      FUN_08000bac();
    }
    iVar15 = *(int *)(param_1 + 0xc);
    *(undefined4 *)(iVar15 + 0x5e4) = 0x3f800000;
    *(undefined4 *)(iVar15 + 0x5e8) = 0x3f800000;
    *(undefined4 *)(iVar15 + 0x5ec) = 0x3f800000;
    FUN_08000bac();
    if ((uVar7 - *(int *)(param_1 + 0x218) < *(uint *)(param_1 + 8)) &&
       (*(uint *)(param_1 + 8) < uVar7)) {
      uVar7 = uVar7 - *(int *)(param_1 + 0x214);
      iVar15 = *(int *)(param_1 + 0xc);
      lVar3 = (ulonglong)DAT_08002b54 * (ulonglong)uVar7;
      *(undefined4 *)(iVar15 + 0x5e4) = 0;
      *(undefined4 *)(iVar15 + 0x5e8) = 0;
      *(float *)(iVar15 + 0x5ec) =
           (float)(ulonglong)(0x58 < uVar7 + (uint)((ulonglong)lVar3 >> 0x26) * -0xa7);
      FUN_08000bac(iVar15 + 0x5d8,0xa7,(int)lVar3);
    }
  }
  return;
}



/* === 08003264 DB_Clock_CaptureEdgeAndMedian3 === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Clock_CaptureEdgeAndMedian3(void)

{
  int *piVar1;
  undefined *puVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  
  piVar1 = DAT_080032e0;
  if (*DAT_080032e0 == 1) {
    iVar6 = System_GetTick();
    uVar11 = DAT_080032f8;
    iVar10 = piVar1[6];
    uVar7 = piVar1[0x15];
    piVar1[6] = iVar6;
    uVar8 = piVar1[0x14];
    piVar1[0x16] = uVar7;
    uVar11 = (uint)((ulonglong)uVar11 * (ulonglong)(uint)(iVar6 - iVar10) >> 0x26);
    piVar1[0x15] = uVar8;
    piVar1[7] = uVar11;
    piVar1[0x14] = uVar11;
    if (uVar8 < uVar11) {
      uVar9 = uVar8;
      if ((uVar8 < uVar7) && (uVar9 = uVar7, uVar11 <= uVar7)) {
        uVar9 = uVar11;
      }
    }
    else {
      uVar9 = uVar11;
      if ((uVar11 < uVar7) && (uVar9 = uVar8, uVar7 <= uVar8)) {
        uVar9 = uVar7;
      }
    }
    piVar1[0x13] = uVar9;
    *(undefined *)(piVar1 + 0x17) = 1;
  }
  *DAT_080032e4 = 0;
  uVar4 = System_GetTick();
  uVar5 = System_GetNow();
  puVar3 = DAT_080032f0;
  puVar2 = DAT_080032ec;
  *DAT_080032e8 = uVar4;
  *puVar3 = uVar5;
  *puVar2 = 1;
  uVar4 = System_GetNow();
  *(undefined4 *)(DAT_080032f4 + 0x224) = uVar4;
  return;
}



/* === 080032fc DB_AudioCallback === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_AudioCallback(undefined4 param_1,int param_2,uint param_3)

{
  char cVar1;
  ulonglong uVar2;
  char *pcVar3;
  uint uVar4;
  int *piVar5;
  char *pcVar6;
  int *piVar7;
  int *piVar8;
  undefined4 uVar9;
  int iVar10;
  int iVar11;
  undefined uVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float fVar20;
  int iVar21;
  int iVar22;
  float fVar23;
  
  pcVar6 = DAT_080036d4;
  uVar9 = System_GetTick();
  *(undefined4 *)(pcVar6 + 8) = uVar9;
  iVar10 = System_GetNow();
  iVar11 = GateIn_Trig(DAT_08003684);
  if (iVar11 != 0) {
    DB_Clock_CaptureEdgeAndMedian3();
  }
  pcVar3 = DAT_08003688;
  if (*DAT_08003688 != '\0') {
    System_GetTick();
    *pcVar3 = '\0';
  }
  piVar7 = DAT_080036d8;
  DB_Controls_Poll(DAT_0800368c);
  iVar11 = DAT_08003860;
  iVar22 = DAT_08003690;
  if (*piVar7 == 1) {
    fVar20 = (float)piVar7[0x12];
    iVar22 = 1;
    if (fVar20 != 1.0 && fVar20 < 1.0 == NAN(fVar20)) {
      fVar18 = (float)(ulonglong)((uint)(0.0 < fVar20) * (int)fVar20);
      iVar22 = (uint)(0.0 < fVar18) * (int)fVar18;
    }
    uVar15 = piVar7[0xc];
    uVar2 = (ulonglong)DAT_0800385c;
    *(int *)(DAT_08003860 + 0x1bc) = iVar22;
    iVar22 = DAT_08003874;
    fVar18 = (float)(ulonglong)(uint)(iVar10 - *DAT_08003864);
    fVar20 = (float)(longlong)(int)(uint)(uVar2 * uVar15 >> 0x26) * fVar20 * 4.0;
    if (fVar18 != fVar20 && fVar18 < fVar20 == (NAN(fVar18) || NAN(fVar20))) {
      *DAT_08003868 = 1;
      *(undefined *)(iVar22 + 0x210) = 1;
      pcVar3 = DAT_080036dc;
      goto joined_r0x080037da;
    }
    uVar12 = *DAT_08003868;
  }
  else {
    uVar12 = 0;
    *(undefined4 *)(DAT_08003690 + 0x1bc) = 1;
    *DAT_08003694 = 0;
    iVar11 = iVar22;
  }
  iVar22 = DAT_0800368c;
  *(undefined *)(DAT_0800368c + 0x210) = uVar12;
  pcVar3 = DAT_080036dc;
joined_r0x080037da:
  DAT_080036dc = pcVar3;
  if (param_3 == 0) {
    uVar12 = *DAT_0800386c;
    *(undefined *)(iVar11 + 0x7c) = *(undefined *)(iVar22 + 0x228);
    *(undefined *)(iVar11 + 0x94) = uVar12;
    *(undefined *)(iVar11 + 0x1a8) = uVar12;
    DB_Engine_MapControlsAndProcess(DAT_08003860,param_1,param_2,0);
  }
  else {
    uVar15 = 0;
    do {
      while( true ) {
        fVar23 = DAT_080036b0;
        fVar18 = DAT_080036ac;
        fVar20 = DAT_080036a8;
        fVar19 = (float)(ulonglong)(uint)piVar7[0xc] * DAT_080036a8;
        fVar16 = (float)piVar7[0x10];
        fVar17 = (float)piVar7[0x1a] * DAT_080036a4 + fVar19 * DAT_080036ac;
        piVar7[0x19] = (int)DAT_080036a4;
        piVar7[0x18] = (int)fVar18;
        uVar9 = FPMaxNum((float)piVar7[0xe] + (float)piVar7[0xf],fVar23);
        fVar23 = (float)FPMinNum(uVar9,0x3f800000);
        fVar19 = fVar19 - fVar17;
        piVar7[0x1a] = (int)fVar17;
        fVar18 = ABS(fVar19);
        piVar7[0x1b] = (int)fVar19;
        *pcVar3 = fVar18 != 5.0 && fVar18 < 5.0 == NAN(fVar18);
        if (fVar23 != fVar16) {
          piVar7[0x10] = (int)fVar23;
          fVar16 = DAT_080036b4;
          fVar18 = DAT_080036b0;
          fVar17 = (float)piVar7[0x11] + fVar20;
          if ((fVar23 != fVar17 && fVar23 < fVar17 == (NAN(fVar23) || NAN(fVar17))) ||
             ((int)((uint)(fVar23 < (float)piVar7[0x11] - fVar20) << 0x1f) < 0)) {
            piVar7[0x11] = (int)fVar23;
            fVar20 = (float)libm_expf(fVar18 + fVar23 * fVar16);
            iVar10 = DAT_080036bc;
            uVar9 = FPMaxNum(fVar23 * DAT_080036b8,fVar18);
            fVar18 = (float)FPMinNum(uVar9,0x41000000);
            piVar7[0xd] = (int)(fVar20 * DAT_080036c0);
            piVar7[0x12] = *(int *)(iVar10 + (int)fVar18 * 4);
          }
        }
        fVar20 = DAT_08003698;
        if (*piVar7 == 0) break;
        iVar10 = System_GetTick();
        if (*(char *)(piVar7 + 0x17) == '\0') {
          fVar20 = (float)(ulonglong)(uint)piVar7[0x13] * (1.0 / (float)piVar7[0x12]);
          piVar7[0xc] = (uint)(0.0 < fVar20) * (int)fVar20;
LAB_080033b8:
          System_GetNow();
        }
        else {
          fVar20 = (float)piVar7[0x12];
          *(undefined *)(piVar7 + 0x17) = 0;
          if (fVar20 == 1.0) {
            piVar7[10] = iVar10;
            piVar7[0xc] = piVar7[0x13];
            iVar10 = System_GetNow();
            piVar7[9] = iVar10 - piVar7[8];
            iVar10 = System_GetNow();
            piVar7[8] = iVar10;
            goto LAB_08003564;
          }
          fVar18 = 1.0 / fVar20;
          iVar21 = (uint)(0.0 < (float)(ulonglong)(uint)piVar7[0x13] * fVar18) *
                   (int)((float)(ulonglong)(uint)piVar7[0x13] * fVar18);
          piVar7[0xc] = iVar21;
          if ((int)((uint)(fVar20 < 1.0) << 0x1f) < 0) {
            if ((uint)(0.0 < fVar18) * (int)fVar18 <= piVar7[0xb] + 1U) {
              piVar7[0xb] = 0;
              piVar7[10] = iVar21 + 1 +
                           (uint)((ulonglong)DAT_08003870 * (ulonglong)(uint)(iVar10 - piVar7[10])
                                 >> 0x26);
              iVar10 = System_GetNow();
              goto LAB_0800352c;
            }
            piVar7[0xb] = piVar7[0xb] + 1U;
          }
          iVar10 = System_GetNow();
          if (fVar20 < 1.0 == NAN(fVar20)) goto LAB_0800352c;
        }
        *(float *)(iVar11 + 0xec) = 1.0 / ((float)(ulonglong)(uint)piVar7[0xc] * DAT_080036a0);
LAB_080033d8:
        uVar15 = uVar15 + 2;
        if (param_3 <= uVar15) goto LAB_080035c6;
      }
      fVar18 = (float)piVar7[0xd];
      piVar7[2] = (int)fVar18;
      fVar23 = (fVar18 * fVar20) / (float)piVar7[4];
      fVar18 = (1.0 / fVar18) * DAT_0800369c;
      piVar7[0xc] = (uint)(0.0 < fVar18) * (int)fVar18;
      fVar18 = fVar23 + (float)piVar7[3];
      piVar7[5] = (int)fVar23;
      piVar7[3] = (int)fVar18;
      if (fVar18 < fVar20 != (NAN(fVar18) || NAN(fVar20))) goto LAB_080033b8;
      piVar7[3] = (int)(fVar18 - fVar20);
      iVar10 = System_GetTick();
      uVar4 = DAT_080036c4;
      iVar21 = piVar7[6];
      piVar7[6] = iVar10;
      piVar7[7] = (uint)((ulonglong)uVar4 * (ulonglong)(uint)(iVar10 - iVar21) >> 0x26);
      iVar10 = System_GetNow();
LAB_0800352c:
      fVar20 = (float)piVar7[0x12];
      piVar7[9] = iVar10 - piVar7[8];
      if (fVar20 != 1.0 && fVar20 < 1.0 == NAN(fVar20)) {
        fVar20 = (float)(ulonglong)(uint)(iVar10 - piVar7[8]) * (1.0 / fVar20);
        piVar7[9] = (uint)(0.0 < fVar20) * (int)fVar20;
      }
      piVar7[8] = iVar10;
LAB_08003564:
      iVar10 = System_GetTick();
      piVar8 = DAT_080036e0;
      piVar5 = DAT_080036cc;
      fVar20 = DAT_080036c8;
      iVar14 = *DAT_080036e0;
      *DAT_080036e0 = iVar10;
      piVar8[1] = iVar14;
      piVar8 = DAT_080036e4;
      iVar13 = *piVar5;
      iVar21 = *DAT_080036e4;
      piVar5[1] = iVar13;
      piVar8[1] = iVar21;
      cVar1 = *pcVar3;
      iVar10 = (int)((float)(ulonglong)(uint)(iVar10 - iVar14) / fVar20);
      *piVar5 = iVar10;
      *piVar8 = iVar13 - iVar10;
      if (cVar1 != '\0') goto LAB_080033d8;
      uVar15 = uVar15 + 2;
      *(undefined4 *)(iVar11 + 0x90) = 0;
      *(undefined *)(iVar11 + 0x74) = 1;
    } while (uVar15 < param_3);
LAB_080035c6:
    cVar1 = *pcVar3;
    uVar12 = *(undefined *)(iVar22 + 0x228);
    *(char *)(iVar11 + 0x94) = cVar1;
    *(undefined *)(iVar11 + 0x7c) = uVar12;
    *(char *)(iVar11 + 0x1a8) = cVar1;
    DB_Engine_MapControlsAndProcess(DAT_08003690,param_1,param_2,param_3);
    if (*DAT_080036d0 != '\0') {
      iVar10 = param_2;
      do {
        iVar11 = iVar10 + 8;
        *(float *)(iVar10 + 4) = -*(float *)(iVar10 + 4);
        iVar10 = iVar11;
      } while (param_2 + 8 + (param_3 - 1 >> 1) * 8 != iVar11);
    }
  }
  iVar10 = System_GetTick();
  fVar20 = (float)(ulonglong)(uint)(iVar10 - *(int *)(pcVar6 + 8)) * *(float *)(pcVar6 + 4);
  if (*pcVar6 != '\0') {
    *(float *)(pcVar6 + 0x14) = fVar20;
    *(float *)(pcVar6 + 0xc) = fVar20;
    *(float *)(pcVar6 + 0x10) = fVar20;
    *pcVar6 = '\0';
    return;
  }
  fVar18 = *(float *)(pcVar6 + 0x10);
  if (fVar20 != fVar18 && fVar20 < fVar18 == (NAN(fVar20) || NAN(fVar18))) {
    *(float *)(pcVar6 + 0x10) = fVar20;
  }
  if ((int)((uint)(fVar20 < *(float *)(pcVar6 + 0xc)) << 0x1f) < 0) {
    *(float *)(pcVar6 + 0xc) = fVar20;
  }
  *(float *)(pcVar6 + 0x14) =
       (1.0 - *(float *)(pcVar6 + 0x18)) * *(float *)(pcVar6 + 0x14) +
       fVar20 * *(float *)(pcVar6 + 0x18);
  return;
}



/* === 08003878 main === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void main(void)

{
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined uVar4;
  short sVar5;
  undefined4 *puVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  int iVar10;
  undefined *puVar11;
  char *pcVar12;
  undefined4 *puVar13;
  float *pfVar14;
  undefined4 *puVar15;
  byte bVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  uint uVar19;
  char *pcVar20;
  uint uVar21;
  char cVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  undefined4 uVar26;
  int iVar27;
  undefined4 *puVar28;
  int iVar29;
  int iVar30;
  bool bVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 uStack_30;
  undefined4 local_2c;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  
  uVar17 = DAT_08003b4c;
  uVar18 = DAT_08003b48;
  puVar6 = DAT_08003b44;
  uVar26 = DAT_08003b40;
  puVar28 = DAT_08003b44 + 0x1a5;
  DataBenderHardware_Init(DAT_08003b40);
  uVar9 = DAT_08003b58;
  uVar8 = DAT_08003b54;
  uVar7 = DAT_08003b50;
  *puVar6 = uVar18;
  puVar6[0x20] = uVar7;
  puVar6[0x21] = uVar8;
  uVar8 = DAT_08003b5c;
  puVar6[0x22] = uVar7;
  puVar6[0x23] = uVar9;
  DB_Buffer_Init(uVar18,puVar6 + 0x26,uVar7,uVar8);
  DB_Corrupt_Init(*puVar6,puVar6 + 0xb5);
  puVar6[0xb5] = 1;
  *(undefined *)(puVar6 + 0xb8) = 1;
  DaisySP_Tone_Init(*puVar6,puVar6 + 0x1a2);
  DaisySP_Tone_Init(*puVar6,puVar6 + 0x1a9);
  *puVar28 = uVar17;
  DaisySP_Tone_CalculateCoefficients(puVar6 + 0x1a2);
  puVar6[0x1ac] = uVar17;
  DaisySP_Tone_CalculateCoefficients(puVar6 + 0x1a9);
  InitSingleFloatHalf_unknown(puVar6 + 0x1b0);
  InitSingleFloatHalf_unknown(puVar6 + 0x1b1);
  *(undefined *)(puVar6 + 0x1d) = 0;
  puVar6[0x1c] = 0;
  puVar6[0x1b4] = 0x3f000000;
  iVar25 = DAT_08003b60;
  puVar6[0x1e] = 0;
  *(undefined4 *)(iVar25 + 4) = 0;
  *(undefined *)(puVar6 + 0x1f) = 0;
  *(undefined *)(puVar6 + 0x1b5) = 0;
  uVar17 = System_GetNow();
  *(undefined4 *)(DAT_08003b60 + 0x20) = uVar17;
  uVar17 = System_GetTick();
  iVar25 = DAT_08003b60;
  *(undefined4 *)(DAT_08003b60 + 0xc) = 0;
  *(undefined4 *)(iVar25 + 0x2c) = 0;
  *(undefined4 *)(iVar25 + 0x18) = uVar17;
  *(undefined4 *)(iVar25 + 0x10) = uVar18;
  iVar10 = DAT_08003b64;
  *(undefined4 *)(iVar25 + 8) = 0x3f800000;
  uVar18 = DAT_08003b68;
  *(undefined4 *)(iVar10 + 500) = 0;
  *(undefined4 *)(iVar25 + 0x30) = uVar18;
  *(undefined4 *)(iVar25 + 0x1c) = uVar18;
  *(undefined4 *)(iVar10 + 0x1f0) = 0;
  iVar29 = iVar10 + 0x48;
  *(int *)(iVar10 + 0x14) = iVar25;
  *(undefined4 *)(iVar25 + 0x54) = uVar18;
  *(undefined4 *)(iVar25 + 0x58) = uVar18;
  *(undefined4 *)(iVar25 + 0x4c) = uVar18;
  *(undefined4 *)(iVar25 + 0x50) = uVar18;
  *(undefined4 *)(iVar25 + 0x14) = DAT_08003b6c;
  *(undefined4 *)(iVar10 + 0xc) = uVar26;
  *(undefined4 **)(iVar10 + 0x10) = puVar6;
  *(undefined *)(iVar10 + 0x1f8) = 2;
  uVar18 = System_GetNow();
  *(undefined4 *)(iVar10 + 0x1fc) = uVar18;
  iVar25 = iVar10;
  do {
    uVar7 = DAT_08003b78;
    uVar17 = DAT_08003b74;
    uVar18 = DAT_08003b70;
    iVar24 = iVar25 + 0xc;
    *(undefined4 *)(iVar25 + 0xa4) = DAT_08003b70;
    *(undefined4 *)(iVar25 + 0xa0) = uVar17;
    *(undefined4 *)(iVar25 + 0x9c) = uVar7;
    iVar25 = iVar24;
  } while (iVar24 != iVar29);
  FUN_08009b70(DAT_08003b7c,2);
  FUN_08009b70(DAT_08003b80,1);
  uVar17 = DAT_08003b88;
  *(undefined4 *)(iVar10 + 0x30) = DAT_08003b84;
  uVar7 = DAT_08003b90;
  *(undefined4 *)(iVar10 + 0x34) = DAT_08003b8c;
  *(undefined4 *)(iVar10 + 0x38) = DAT_08003b94;
  FUN_08009b90(0x3f000000,uVar18,uVar17,uVar7);
  FUN_08009b90(0x3f800000,DAT_08003b9c,DAT_08003b98,DAT_08003ba0);
  FUN_08009b70(DAT_08003ba4,3);
  FUN_08009b70(DAT_08003ba8,7);
  uVar17 = System_GetNow();
  iVar25 = *(int *)(iVar10 + 0x10);
  *(undefined4 *)(iVar10 + 0x214) = uVar17;
  iVar24 = *(int *)(iVar10 + 0xc);
  *(undefined4 *)(iVar25 + 0x684) = 1;
  *(undefined4 *)(iVar25 + 0x2d4) = 1;
  uVar17 = DAT_08003bac;
  *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x70) = uVar17;
  uVar17 = DAT_08003bb0;
  *(undefined4 *)(iVar25 + 0x3c) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x40) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x44) = 0x3f800000;
  *(undefined4 *)(iVar25 + 0x1a4) = uVar17;
  *(undefined4 *)(iVar25 + 0x120) = uVar18;
  *(undefined4 *)(iVar25 + 0x158) = uVar18;
  *(undefined4 *)(iVar25 + 0x100) = 0;
  *(undefined4 *)(iVar25 + 0x6d0) = uVar18;
  *(undefined4 *)(iVar25 + 0xc) = 0;
  *(undefined2 *)(iVar25 + 0x10) = 0;
  *(undefined4 *)(iVar25 + 4) = 0;
  *(undefined4 *)(iVar25 + 8) = 0;
  *(undefined2 *)(iVar10 + 0x228) = 0;
  *(undefined *)(iVar10 + 0x22a) = 0;
  iVar25 = iVar24 + 0x578;
  do {
    iVar30 = iVar25 + 0x18;
    *(undefined4 *)(iVar25 + 8) = DAT_08003bb4;
    FUN_08000bac(iVar25);
    iVar25 = iVar30;
  } while (iVar24 + 0x620 != iVar30);
  iVar25 = *(int *)(iVar10 + 0xc);
  uVar19 = GPIO_Read(iVar25 + 0xe8);
  if (*(char *)(iVar25 + 0xfd) != '\0') {
    uVar19 = (uVar19 ^ 1) & 0xff;
  }
  if (uVar19 == 0) {
    iVar25 = *(int *)(iVar10 + 0xc);
    uVar19 = GPIO_Read(iVar25 + 0xc4);
    if (*(char *)(iVar25 + 0xd9) != '\0') {
      uVar19 = (uVar19 ^ 1) & 0xff;
    }
    if (uVar19 != 0) {
      *(undefined *)(iVar10 + 0x1f8) = 4;
    }
  }
  else {
    *(undefined *)(iVar10 + 0x1f8) = 3;
  }
  fVar32 = (float)DaisySeed_AudioSampleRate(DAT_08003b40);
  iVar25 = DaisySeed_AudioBlockSize(DAT_08003b40);
  pcVar12 = DAT_08003bcc;
  uVar19 = System_GetTickFreq();
  uVar18 = DAT_08003bc0;
  puVar11 = DAT_08003bbc;
  fVar33 = DAT_08003bb8;
  local_24 = CONCAT22(local_24._2_2_,0x403);
  *DAT_08003bbc = 1;
  *(undefined4 *)(puVar11 + 0xc) = uVar18;
  *(undefined4 *)(puVar11 + 0x10) = uVar18;
  *(undefined4 *)(puVar11 + 0x14) = uVar18;
  local_2c = 0;
  local_34 = 0;
  uStack_30 = 0;
  fVar33 = fVar33 / (fVar32 / (float)(longlong)iVar25);
  *(float *)(puVar11 + 4) = 1.0 / ((float)(ulonglong)uVar19 * ((float)(longlong)iVar25 / fVar32));
  *(float *)(puVar11 + 0x18) = fVar33 / (fVar33 + 1.0);
  local_38 = 0xff0b;
  GPIO_Init(&local_38,local_24,0,1,0);
  bVar16 = GPIO_Read(&local_38);
  puVar13 = DAT_08003bd0;
  iVar25 = 0;
  *DAT_08003bc4 = bVar16 ^ 1;
  *puVar13 = uVar26;
  puVar13[1] = 0xc;
  puVar28 = DAT_08003bc8;
  puVar13[2] = 0x38;
  puVar13[3] = pcVar12;
  puVar13[4] = puVar28;
  pcVar20 = pcVar12;
  while( true ) {
    pcVar20[iVar25] = (&DAT_90009000)[iVar25];
    iVar25 = iVar25 + 1;
    if (iVar25 == 0xc) break;
    pcVar20 = (char *)puVar13[3];
  }
  iVar25 = 0;
  do {
    *(undefined *)(puVar13[4] + iVar25) = *(undefined *)(iVar25 + DAT_08003e9c);
    iVar25 = iVar25 + 1;
  } while (iVar25 != 0x38);
  if (*(char *)puVar28 != '\x04') {
    puVar28[3] = 0x3f800000;
    puVar28[4] = 0x3f800000;
    puVar28[5] = 0x3f800000;
    puVar28[0xc] = 0x3f800000;
    puVar28[1] = 0;
    *(undefined *)(puVar28 + 2) = 0;
    puVar28[9] = 0;
    puVar28[0xd] = 0;
    puVar28[7] = 0;
    puVar28[8] = 0;
    *puVar28 = 4;
    puVar28[10] = 1;
    puVar28[6] = DAT_08003ea0;
    puVar28[0xb] = DAT_08003ea4;
  }
  if (*pcVar12 != '\x05') {
    *pcVar12 = '\x05';
    local_18 = 0;
    local_24 = 0xff0b;
    local_20 = 0;
    uStack_1c = 0;
    GPIO_Init(&local_24,0x403,0,1,0);
    iVar25 = GPIO_Read(&local_24);
    if (iVar25 == 0) {
      *(undefined4 *)(pcVar12 + 4) = DAT_0800458c;
      *(undefined4 *)(pcVar12 + 8) = DAT_08004590;
    }
    else {
      *(undefined4 *)(pcVar12 + 4) = DAT_08004578;
      *(undefined4 *)(pcVar12 + 8) = DAT_0800457c;
    }
  }
  uVar26 = puVar28[10];
  fVar32 = (float)puVar28[0xb];
  puVar6[0x1a1] = uVar26;
  puVar6[0xb5] = uVar26;
  fVar33 = DAT_08003ea8;
  puVar6[3] = puVar28[1];
  puVar3 = DAT_08003eac;
  puVar6[1] = puVar28[7];
  iVar25 = DAT_08003ed4;
  puVar6[0x40] = puVar28[8];
  if (-1 < (int)((uint)(fVar32 < fVar33) << 0x1f)) {
    fVar33 = fVar32;
  }
  uVar18 = puVar28[6];
  *puVar3 = puVar28[9];
  uVar26 = DAT_08003eb0;
  *(undefined *)(iVar10 + 0x229) = *(undefined *)((int)puVar28 + 2);
  uVar26 = FPMaxNum(uVar18,uVar26);
  fVar32 = (float)FPMinNum(uVar26,0x3f800000);
  *(undefined *)(iVar10 + 0x22a) = *(undefined *)((int)puVar28 + 1);
  uVar4 = *(undefined *)((int)puVar28 + 3);
  puVar6[0x1c] = fVar32;
  puVar6[0x69] = fVar32 * fVar32;
  *(undefined *)(iVar10 + 0x228) = uVar4;
  iVar24 = iVar25;
  do {
    *(float *)(iVar24 + 8) = fVar33;
    iVar30 = iVar24 + 0x18;
    FUN_08000bac(iVar24);
    puVar3 = DAT_08003ebc;
    iVar24 = iVar30;
  } while (iVar25 + 0xa8 != iVar30);
  fVar33 = (float)puVar28[0xc] * DAT_08003eb4;
  *DAT_08003eb8 =
       -(float)((uint)(fVar33 != 1.0) * 0x3f800000 + (uint)(fVar33 == 1.0) * (int)fVar33) * 0.5 +
       0.5;
  puVar6[0xf] = puVar28[3];
  puVar6[0x10] = puVar28[4];
  puVar6[0x11] = puVar28[5];
  *puVar3 = *(undefined4 *)(pcVar12 + 8);
  puVar3 = puVar3 + -1;
  *puVar3 = *(undefined4 *)(pcVar12 + 4);
  iVar25 = puVar28[0xd];
  puVar6[2] = iVar25;
  if ((3 < (int)puVar6[0x1a1]) && (iVar25 == 1)) {
    puVar6[0x1a1] = 1;
  }
  FUN_080075a0(DAT_08003ec0);
  DaisySeed_StartAudio_interleaved(DAT_08003ec8,DAT_08003ec4);
switchD_08003d64_caseD_6:
  while( true ) {
    while( true ) {
      while ((*(int *)(iVar10 + 500) - *(int *)(iVar10 + 0x1f0) & 0x1fU) == 0) {
        iVar25 = System_GetNow();
        if (((1 < *(byte *)(iVar10 + 0x1f8) - 3) && (1 < *(byte *)(iVar10 + 0x1f8))) &&
           (*(uint *)(iVar10 + 4) < (uint)(iVar25 - *(int *)(iVar10 + 0x1fc)))) {
          *(int *)(iVar10 + 0x1fc) = iVar25;
          *(undefined *)(iVar10 + 0x1f8) = 0;
        }
        if (2 < (uint)(iVar25 - *(int *)(iVar10 + 0x20c))) {
          iVar24 = *(int *)(iVar10 + 0xc);
          do {
          } while (-1 < (int)((uint)*(byte *)(iVar24 + 0x374) << 0x18));
          iVar27 = 0;
          iVar23 = *(int *)(iVar24 + 0x354);
          iVar30 = *(int *)(iVar24 + 0x358);
          *(int *)(iVar24 + 0x354) = iVar30;
          *(int *)(iVar24 + 0x358) = iVar23;
          do {
            iVar1 = iVar27 * 4;
            iVar2 = iVar27 * 4;
            iVar27 = iVar27 + 1;
            *(undefined2 *)(iVar30 + iVar2 + 3) = *(undefined2 *)(iVar23 + iVar1 + 3);
          } while (iVar27 != 0x10);
          iVar27 = 0;
          do {
            iVar1 = iVar27 * 4;
            iVar2 = iVar27 * 4;
            iVar27 = iVar27 + 1;
            *(undefined2 *)(iVar30 + iVar2 + 0x44) = *(undefined2 *)(iVar23 + iVar1 + 0x44);
          } while (iVar27 != 0x10);
          *(undefined *)(iVar24 + 0x374) = 0xff;
          *(char *)(iVar24 + 0x374) = *(char *)(iVar24 + 0x374) + '\x01';
          if (*(char *)(iVar24 + 0x374) < '\x02') {
            iVar30 = iVar24 + 0x350;
            iVar24 = I2CHandle_TransmitDma
                               (iVar30,*(byte *)(*(char *)(iVar24 + 0x374) + iVar24 + 0x35c) | 0x40,
                                iVar23 + *(char *)(iVar24 + 0x374) * 0x41,0x41,DAT_08004580,iVar30);
            if (iVar24 != 0) {
              uVar26 = FUN_0800804c(iVar30);
              I2CHandle_Init(iVar30,uVar26);
            }
          }
          else {
            *(undefined *)(iVar24 + 0x374) = 0xff;
          }
          *(int *)(iVar10 + 0x20c) = iVar25;
        }
        if (*(char *)(iVar10 + 0x211) != '\0') {
          iVar25 = puVar6[1];
          cVar22 = *(char *)(puVar6 + 3);
          if (iVar25 == 0) {
            if (cVar22 == '\0') {
              cVar22 = *(char *)((int)puVar6 + 0x12);
            }
            *(char *)(puVar28 + 1) = cVar22;
            *(undefined *)((int)puVar28 + 5) = *(undefined *)((int)puVar6 + 0xd);
            *(undefined *)((int)puVar28 + 6) = *(undefined *)((int)puVar6 + 0xe);
            cVar22 = *(char *)((int)puVar6 + 0xf);
            if (cVar22 == '\0') {
              cVar22 = *(char *)((int)puVar6 + 0x13);
            }
          }
          else {
            *(char *)(puVar28 + 1) = cVar22;
            cVar22 = *(char *)((int)puVar6 + 0xd);
            if (iVar25 == 1) {
              if (cVar22 == '\0') {
                cVar22 = *(char *)((int)puVar6 + 0x12);
              }
              *(char *)((int)puVar28 + 5) = cVar22;
              cVar22 = *(char *)((int)puVar6 + 0xe);
              if (cVar22 == '\0') {
                cVar22 = *(char *)((int)puVar6 + 0x13);
              }
            }
            else {
              *(char *)((int)puVar28 + 5) = cVar22;
              cVar22 = *(char *)((int)puVar6 + 0xe);
            }
            *(char *)((int)puVar28 + 6) = cVar22;
            cVar22 = *(char *)((int)puVar6 + 0xf);
          }
          puVar28[7] = iVar25;
          pfVar14 = DAT_08004228;
          *(char *)((int)puVar28 + 7) = cVar22;
          fVar33 = *pfVar14;
          puVar28[6] = puVar6[0x1c];
          puVar28[10] = puVar6[0xb5];
          uVar26 = puVar6[0x40];
          puVar28[0xc] = -fVar33 * 2.0 + 1.0;
          puVar28[8] = uVar26;
          puVar28[9] = *DAT_0800422c;
          *(undefined *)((int)puVar28 + 2) = *(undefined *)(iVar10 + 0x229);
          *(undefined *)((int)puVar28 + 1) = *(undefined *)(iVar10 + 0x22a);
          *(undefined *)((int)puVar28 + 3) = *(undefined *)(iVar10 + 0x228);
          puVar28[0xb] = *DAT_08004230;
          puVar28[3] = puVar6[0xf];
          puVar28[4] = puVar6[0x10];
          puVar28[5] = puVar6[0x11];
          puVar28[0xd] = puVar6[2];
        }
        if (*(char *)(iVar10 + 0x212) != '\0') {
          *(undefined4 *)(pcVar12 + 4) = *(undefined4 *)(iVar10 + 0x238);
          *(undefined4 *)(pcVar12 + 8) = *(undefined4 *)(iVar10 + 0x23c);
          *pcVar12 = '\x05';
          if (*(char *)(iVar10 + 0x213) != '\0') {
            local_24 = 0xff0b;
            local_18 = 0;
            local_20 = 0;
            uStack_1c = 0;
            GPIO_Init(&local_24,0x403,0,1,0);
            iVar25 = GPIO_Read(&local_24);
            if (iVar25 == 0) {
              *(undefined4 *)(pcVar12 + 4) = DAT_0800458c;
              *(undefined4 *)(pcVar12 + 8) = DAT_08004590;
            }
            else {
              *(undefined4 *)(pcVar12 + 4) = DAT_08004578;
              *(undefined4 *)(pcVar12 + 8) = DAT_0800457c;
            }
          }
          FUN_08008a98(*puVar13,&DAT_90009000,&DAT_90009000 + puVar13[1]);
          FUN_08008a90(*puVar13,&DAT_90009000,puVar13[1],puVar13[3]);
          puVar15 = DAT_08004234;
          *(undefined2 *)(iVar10 + 0x212) = 0;
          *puVar15 = *(undefined4 *)(pcVar12 + 8);
          *puVar3 = *(undefined4 *)(pcVar12 + 4);
        }
        iVar25 = System_GetNow();
        if ((2000 < (uint)(iVar25 - puVar13[5])) && (uVar19 = puVar13[2], uVar19 != 0)) {
          uVar21 = 1;
          bVar31 = false;
          pcVar20 = (char *)(puVar13[4] + -1);
          do {
            pcVar20 = pcVar20 + 1;
            if (*(char *)(DAT_08004238 + uVar21 + -1) == *pcVar20) {
              if (uVar19 <= uVar21) goto code_r0x080040ba;
            }
            else {
              bVar31 = true;
              if (uVar19 <= uVar21) goto LAB_080040c2;
            }
            uVar21 = uVar21 + 1;
          } while( true );
        }
      }
      iVar25 = *(int *)(iVar10 + 0x1f0) + 0x1e;
      iVar24 = iVar10 + iVar25 * 8;
      cVar22 = *(char *)(iVar10 + iVar25 * 8);
      fVar32 = *(float *)(iVar24 + 4);
      sVar5 = *(short *)(iVar24 + 2);
      *(uint *)(iVar10 + 0x1f0) = *(int *)(iVar10 + 0x1f0) + 1U & 0x1f;
      fVar33 = DAT_08004598;
      if (cVar22 == '\x04') break;
      if (cVar22 == '\0') {
        iVar25 = *(int *)(iVar10 + 0xc);
        cVar22 = *(char *)(iVar25 + 0x90);
        switch(sVar5) {
        case 0:
          if (cVar22 == -1) {
            uVar26 = FPMaxNum(*(undefined4 *)(iVar25 + 0x1dc),DAT_08004584);
            fVar33 = (float)FPMinNum(uVar26,0x3f800000);
            iVar25 = *(int *)(iVar10 + 0x10);
            *(float *)(iVar25 + 0x70) = fVar33;
            *(float *)(iVar25 + 0x1a4) = fVar33 * fVar33;
          }
          else {
            *(undefined4 *)(*(int *)(iVar10 + 0x14) + 0x38) = *(undefined4 *)(iVar25 + 0x1dc);
          }
          break;
        case 1:
          if (cVar22 == -1) {
            iVar24 = iVar25 + 0x578;
            fVar33 = DAT_08004594;
            if (-1 < (int)((uint)(fVar32 < DAT_08004594) << 0x1f)) {
              fVar33 = fVar32;
            }
            do {
              *(float *)(iVar24 + 8) = fVar33;
              iVar30 = iVar24 + 0x18;
              FUN_08000bac(iVar24);
              iVar24 = iVar30;
            } while (iVar30 != iVar25 + 0x620);
          }
          else {
            *(float *)(*(int *)(iVar10 + 0x10) + 0x4c) = fVar32;
          }
          break;
        case 2:
          if (cVar22 == -1) {
            fVar32 = fVar32 * DAT_08004588;
            fVar33 = DAT_08004584;
            if (fVar32 == 1.0 || fVar32 < 1.0 != NAN(fVar32)) {
              fVar33 = -fVar32 * 0.5 + 0.5;
            }
            *(float *)(*(int *)(iVar10 + 0x10) + 0x6d0) = fVar33;
          }
          else {
            *(float *)(*(int *)(iVar10 + 0x10) + 0x50) = fVar32;
          }
          break;
        case 3:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x3c) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x30) = fVar32;
          }
          break;
        case 4:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x40) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x34) = fVar32;
          }
          break;
        case 5:
          iVar25 = *(int *)(iVar10 + 0x10);
          if (cVar22 == -1) {
            *(float *)(iVar25 + 0x44) = fVar32;
          }
          if (cVar22 != -1) {
            *(float *)(iVar25 + 0x38) = fVar32;
          }
        }
      }
    }
    if (fVar32 != 0.0) break;
    if (sVar5 == 0) {
      if (*(char *)(iVar10 + 0x1f8) == '\x04') {
        bVar16 = *(byte *)(iVar10 + 0x234);
        if (bVar16 != 0) {
          fVar32 = *(float *)(iVar10 + 0x230);
          fVar33 = *(float *)(iVar10 + 0x22c);
          fVar34 = fVar32 - fVar33;
          bVar16 = *(byte *)(iVar10 + 0x235) & 1;
          if (fVar32 == fVar33 || fVar32 < fVar33 != (NAN(fVar32) || NAN(fVar33))) {
            bVar16 = 0;
          }
          if (fVar34 == 0.25 || fVar34 < 0.25 != NAN(fVar34)) {
            bVar16 = 0;
          }
        }
        fVar33 = *(float *)(iVar10 + 0x23c);
        if (fVar33 == DAT_0800423c || fVar33 < DAT_0800423c != (NAN(fVar33) || NAN(DAT_0800423c))) {
          bVar16 = 0;
        }
        else if (-1 < (int)((uint)(fVar33 < DAT_08004240) << 0x1f)) {
          bVar16 = 0;
        }
        fVar33 = *(float *)(iVar10 + 0x238);
        if (fVar33 == DAT_08004244 || fVar33 < DAT_08004244 != (NAN(fVar33) || NAN(DAT_08004244))) {
          bVar16 = 0;
        }
        else if (-1 < (int)((uint)(fVar33 < DAT_08004248) << 0x1f)) {
          bVar16 = 0;
        }
        *(byte *)(iVar10 + 0x212) = bVar16;
      }
      *(undefined *)(iVar10 + 0x1f8) = 1;
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x1fc) = uVar26;
      iVar25 = iVar10;
      do {
        iVar24 = iVar25 + 0xc;
        *(undefined4 *)(iVar25 + 0xa0) = DAT_08003ecc;
        *(undefined4 *)(iVar25 + 0x9c) = DAT_08003ed0;
        iVar25 = iVar24;
      } while (iVar24 != iVar29);
    }
    else if (((sVar5 == 3) && (*(char *)(*(int *)(iVar10 + 0xc) + 0x90) != -1)) &&
            (*(char *)(iVar10 + 0x228) != '\0')) {
      *(undefined *)(*(int *)(iVar10 + 0x10) + 0x11) = 1;
    }
  }
  iVar25 = *(int *)(iVar10 + 0xc);
  uVar19 = (uint)*(byte *)(iVar25 + 0x90);
  switch(sVar5) {
  case 0:
    *(undefined *)(iVar10 + 0x1f8) = 0;
    uVar26 = System_GetNow();
    *(undefined4 *)(iVar10 + 0x1fc) = uVar26;
    *(undefined *)(iVar10 + 0x211) = 1;
    iVar25 = iVar10;
    do {
      iVar24 = iVar25 + 0xc;
      *(undefined4 *)(iVar25 + 0xa0) = DAT_08004220;
      *(undefined4 *)(iVar25 + 0x9c) = DAT_08004224;
      iVar25 = iVar24;
    } while (iVar24 != iVar29);
    goto switchD_08003d64_caseD_6;
  case 1:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x22a) = *(byte *)(iVar10 + 0x22a) ^ 1;
    }
    else {
      **(uint **)(iVar10 + 0x14) = (uint)(**(uint **)(iVar10 + 0x14) == 0);
    }
    goto switchD_08003d64_caseD_6;
  case 2:
    iVar25 = *(int *)(iVar10 + 0x10);
    if (uVar19 != 0xff) {
      uVar19 = *(byte *)(iVar25 + 4) + 1 & 1;
      *(uint *)(iVar25 + 4) = uVar19;
      if (uVar19 == 0) {
        *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
      }
      goto switchD_08003d64_caseD_6;
    }
    uVar19 = *(byte *)(iVar25 + 8) + 1 & 1;
    *(uint *)(iVar25 + 8) = uVar19;
    if ((*(int *)(iVar25 + 0x684) < 4) || (uVar19 == 0)) goto switchD_08003d64_caseD_6;
LAB_08003e44:
    iVar24 = 1;
    break;
  case 3:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x228) = *(byte *)(iVar10 + 0x228) ^ 1;
    }
    else if (*(byte *)(iVar10 + 0x228) == 0) {
      iVar25 = *(int *)(iVar10 + 0x10);
      bVar16 = 0;
      if (*(char *)(iVar25 + 0x11) == '\0') {
        bVar16 = *(byte *)(iVar25 + 0x15) ^ 1;
      }
      *(byte *)(iVar25 + 0x11) = bVar16;
    }
    else {
      *(undefined *)(*(int *)(iVar10 + 0x10) + 0x11) = 0;
    }
    goto switchD_08003d64_caseD_6;
  case 4:
    if (*(char *)(iVar10 + 0x1f8) == '\x04') {
      if (*(char *)(iVar10 + 0x234) == '\0') {
        *(undefined4 *)(iVar10 + 0x22c) = *(undefined4 *)(iVar25 + 0x2fc);
        *(undefined *)(iVar10 + 0x234) = 1;
      }
      else if (*(char *)(iVar10 + 0x235) == '\0') {
        fVar34 = *(float *)(iVar25 + 0x2fc);
        *(float *)(iVar10 + 0x230) = fVar34;
        fVar32 = DAT_08004594;
        *(undefined *)(iVar10 + 0x235) = 1;
        fVar33 = fVar33 / (fVar34 - *(float *)(iVar10 + 0x22c));
        *(float *)(iVar10 + 0x23c) = fVar33;
        *(float *)(iVar10 + 0x238) = fVar32 + -*(float *)(iVar10 + 0x22c) * fVar33;
      }
    }
    iVar25 = *(int *)(iVar10 + 0x10);
    if (uVar19 == 0xff) {
      if (*(int *)(iVar25 + 0x100) == 0) {
        *(undefined4 *)(iVar25 + 0x100) = 1;
      }
      else {
        *(undefined4 *)(iVar25 + 0x100) = 0;
      }
    }
    else if (*(int *)(iVar25 + 4) == 0) {
      bVar16 = 0;
      if (*(char *)(iVar25 + 0xc) == '\0') {
        bVar16 = *(byte *)(iVar25 + 0x12) ^ 1;
      }
      *(byte *)(iVar25 + 0xc) = bVar16;
    }
    else if (*(char *)(iVar25 + 0xd) == '\0') {
      *(byte *)(iVar25 + 0xd) = *(byte *)(iVar25 + 0x12) ^ 1;
    }
    else {
      *(undefined *)(iVar25 + 0xd) = 0;
    }
    goto switchD_08003d64_caseD_6;
  case 5:
    if (*(char *)(iVar10 + 0x1f8) == '\x04') {
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x218) = uVar26;
      *(undefined4 *)(iVar10 + 0x23c) = 0x3f800000;
      *(undefined *)(iVar10 + 0x213) = 1;
      *(undefined2 *)(iVar10 + 0x234) = 0x101;
      *(undefined4 *)(iVar10 + 0x238) = 0;
    }
    else if (uVar19 == 0xff) {
      uVar26 = System_GetNow();
      *(undefined4 *)(iVar10 + 0x214) = uVar26;
      iVar24 = *(int *)(iVar10 + 0xc);
      iVar25 = *(int *)(iVar10 + 0x10);
      *(undefined4 *)(iVar25 + 0x684) = 1;
      *(undefined4 *)(iVar25 + 0x2d4) = 1;
      uVar26 = DAT_080045a0;
      *(undefined4 *)(iVar25 + 0x13c) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x70) = uVar26;
      uVar26 = DAT_080045a4;
      *(undefined4 *)(iVar25 + 0x3c) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x40) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x44) = 0x3f800000;
      *(undefined4 *)(iVar25 + 0x1a4) = uVar26;
      *(undefined4 *)(iVar25 + 0x100) = 0;
      *(undefined4 *)(iVar25 + 0x120) = 0;
      *(undefined4 *)(iVar25 + 0x158) = 0;
      *(undefined4 *)(iVar25 + 0x6d0) = 0;
      *(undefined4 *)(iVar25 + 0xc) = 0;
      *(undefined2 *)(iVar25 + 0x10) = 0;
      *(undefined4 *)(iVar25 + 4) = 0;
      *(undefined4 *)(iVar25 + 8) = 0;
      *(undefined2 *)(iVar10 + 0x228) = 0;
      *(undefined *)(iVar10 + 0x22a) = 0;
      iVar25 = iVar24 + 0x578;
      do {
        iVar30 = iVar25 + 0x18;
        *(undefined4 *)(iVar25 + 8) = DAT_0800459c;
        FUN_08000bac(iVar25);
        iVar25 = iVar30;
      } while (iVar24 + 0x620 != iVar30);
    }
    else {
      iVar25 = *(int *)(iVar10 + 0x10);
      if (*(int *)(iVar25 + 4) == 0) {
        bVar16 = 0;
        if (*(char *)(iVar25 + 0xf) == '\0') {
          bVar16 = *(byte *)(iVar25 + 0x13) ^ 1;
        }
        *(byte *)(iVar25 + 0xf) = bVar16;
      }
      else if (*(char *)(iVar25 + 0xe) == '\0') {
        *(byte *)(iVar25 + 0xe) = *(byte *)(iVar25 + 0x13) ^ 1;
      }
      else {
        *(undefined *)(iVar25 + 0xe) = 0;
      }
    }
    goto switchD_08003d64_caseD_6;
  case 6:
    if (uVar19 == 0xff) {
      *(byte *)(iVar10 + 0x229) = *(byte *)(iVar10 + 0x229) ^ 1;
      goto switchD_08003d64_caseD_6;
    }
    iVar25 = *(int *)(iVar10 + 0x10);
    bVar31 = *(int *)(iVar25 + 8) == 1;
    if (bVar31) {
      uVar19 = 4;
    }
    iVar24 = *(int *)(iVar25 + 0x684) + 1;
    if (!bVar31) {
      uVar19 = 6;
    }
    iVar24 = iVar24 - uVar19 * (iVar24 / (int)uVar19);
    if (iVar24 == 0) goto LAB_08003e44;
    break;
  default:
    goto switchD_08003d64_caseD_6;
  }
  *(int *)(iVar25 + 0x684) = iVar24;
  goto switchD_08003d64_caseD_6;
code_r0x080040ba:
  if (bVar31) {
LAB_080040c2:
    FUN_08008a98(*puVar13,DAT_08004238,uVar19 + DAT_08004238);
    FUN_08008a90(*puVar13,DAT_08004238,puVar13[2],puVar13[4]);
    uVar26 = System_GetNow();
    puVar13[5] = uVar26;
    *(undefined *)(iVar10 + 0x211) = 0;
  }
  goto switchD_08003d64_caseD_6;
}



/* === 08004678 DB_MacroBend_ChooseRateAndSlew === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_MacroBend_ChooseRateAndSlew(int param_1,int param_2)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  int iVar4;
  int iVar5;
  float fVar6;
  undefined4 uVar8;
  double dVar7;
  float fVar9;
  uint uVar10;
  uint uVar11;
  
  iVar4 = newlib_rand_LCG64();
  uVar8 = 0x3fc00000;
  fVar6 = *(float *)(param_1 + 0x104);
  fVar9 = ((float)(longlong)
                  (iVar4 + (((int)((ulonglong)((longlong)DAT_08004790 * (longlong)iVar4) >> 0x20) +
                             iVar4 >> 7) - (iVar4 >> 0x1f)) * -0xff) / DAT_08004794) * fVar6 * 1.5 *
          9.0;
  uVar10 = (uint)(0.0 < fVar9) * (int)fVar9;
  if (uVar10 < 10) {
    uVar8 = *(undefined4 *)(DAT_08004798 + uVar10 * 4);
  }
  iVar4 = param_1 + param_2 * 4;
  bVar1 = fVar6 < DAT_0800479c;
  bVar2 = fVar6 != DAT_0800479c;
  bVar3 = NAN(DAT_0800479c);
  *(undefined4 *)(iVar4 + 0xa8) = uVar8;
  if (bVar2 && bVar1 == (NAN(fVar6) || bVar3)) {
    iVar5 = newlib_rand_LCG64();
    fVar6 = (float)(longlong)
                   (iVar5 + (((int)((ulonglong)((longlong)DAT_08004790 * (longlong)iVar5) >> 0x20) +
                              iVar5 >> 7) - (iVar5 >> 0x1f)) * -0xff) / DAT_08004794;
    if (fVar6 == 0.25 || fVar6 < 0.25 != NAN(fVar6)) {
      return;
    }
    uVar10 = newlib_rand_LCG64();
    dVar7 = DAT_08004788 + ((double)*(float *)(param_1 + 0x104) - DAT_08004778) * 3.0 * DAT_08004780
    ;
    uVar11 = (uint)(0.0 < dVar7) * (int)(longlong)dVar7;
    uVar10 = uVar10 - (uVar10 / uVar11) * uVar11;
    if (0x7f < uVar10) {
      uVar8 = DAT_080047a0;
      if (0xff < uVar10) {
        uVar8 = DAT_080047a4;
      }
      *(undefined4 *)(iVar4 + 0xe0) = uVar8;
      return;
    }
  }
  *(undefined4 *)(iVar4 + 0xe0) = 0x3f800000;
  return;
}



/* === 080047a8 DB_Buffer_WriteSample === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Buffer_WriteSample(undefined4 param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  uint uVar8;
  uint uVar9;
  
  fVar6 = DAT_08004938;
  if (*(char *)(param_2 + 0x110) != '\0') {
    *(undefined *)(param_2 + param_3 + 0x22a) = 0;
    if (param_3 == 1) {
      cVar1 = *(char *)(param_2 + 0x11a);
      *(float *)(param_2 + 0x60) =
           (float)(ulonglong)*(uint *)(param_2 + 0x198) + *(float *)(param_2 + 0x148);
    }
    else {
      cVar1 = *(char *)(param_2 + 0x11a);
    }
    if (cVar1 == '\0') {
      iVar3 = param_2 + param_3 * 4;
      iVar2 = param_2 + param_3 * 0x20;
      uVar4 = *(uint *)(iVar2 + 0x10);
      fVar6 = (float)(ulonglong)*(uint *)(iVar3 + 0x194) + *(float *)(iVar3 + 0x144);
      uVar9 = (uint)(0.0 < fVar6) * (int)fVar6;
      *(undefined4 *)(*(int *)(iVar2 + 0xc) + (uVar9 - uVar4 * (uVar9 / uVar4)) * 4) = param_1;
    }
    param_2 = param_2 + param_3 * 4;
    fVar6 = *(float *)(param_2 + 0x144) + 1.0;
    fVar7 = (float)(ulonglong)(*(int *)(param_2 + 0x154) - 1);
    *(float *)(param_2 + 0x144) = fVar6;
    if (fVar6 != fVar7 && fVar6 < fVar7 == (NAN(fVar6) || NAN(fVar7))) {
      *(undefined4 *)(param_2 + 0x144) = 0;
    }
    return;
  }
  iVar2 = param_2 + param_3 * 4;
  uVar4 = *(uint *)(iVar2 + 0x154);
  fVar7 = *(float *)(iVar2 + 0x144);
  fVar5 = (float)(ulonglong)(uVar4 - 1);
  if ((fVar7 != fVar5 && fVar7 < fVar5 == (NAN(fVar7) || NAN(fVar5))) || (uVar4 == 0)) {
    *(float *)(iVar2 + 0x144) = DAT_08004938;
    fVar7 = fVar6;
  }
  cVar1 = *(char *)(param_2 + 0x11a);
  if (cVar1 == '\0') {
    if (param_3 == 1) {
      *(float *)(param_2 + 0x60) =
           (float)(ulonglong)*(uint *)(param_2 + 0x198) + *(float *)(param_2 + 0x148);
    }
    iVar3 = param_2 + param_3 * 0x20;
    uVar9 = *(uint *)(iVar3 + 0x10);
    fVar7 = (float)(ulonglong)*(uint *)(iVar2 + 0x194) + fVar7;
    uVar8 = (uint)(0.0 < fVar7) * (int)fVar7;
    *(undefined4 *)(*(int *)(iVar3 + 0xc) + (uVar8 - uVar9 * (uVar8 / uVar9)) * 4) = param_1;
    fVar7 = *(float *)(iVar2 + 0x144);
  }
  if (-1 < (int)((uint)(fVar5 < fVar7 + 1.0) << 0x1f)) {
    *(float *)(iVar2 + 0x144) = fVar7 + 1.0;
    return;
  }
  *(undefined4 *)(iVar2 + 0x144) = 0;
  if (*(int *)(iVar2 + 0x22c) != 0) {
    *(int *)(iVar2 + 0x22c) = *(int *)(iVar2 + 0x22c) + -1;
    *(undefined *)(param_2 + param_3 + 0x234) = 1;
  }
  *(undefined *)(param_2 + param_3 + 0x22a) = 1;
  if ((cVar1 == '\0') && (uVar4 >> 2 < (uint)(*(int *)(iVar2 + 0x128) - *(int *)(iVar2 + 0x1a4)))) {
    *(int *)(iVar2 + 0x1a4) = *(int *)(iVar2 + 0x128);
    if (*(int *)(iVar2 + 0x194) != 0) {
      uVar4 = 0;
    }
    *(uint *)(iVar2 + 0x194) = uVar4;
  }
  *(undefined *)(param_4 + param_3) = 1;
  return;
}



/* === 0800493c DB_Buffer_ReadLinear === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DB_Buffer_ReadLinear(int param_1,int param_2,char *param_3)

{
  ulonglong uVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  uint uVar11;
  float fVar12;
  undefined4 uVar13;
  float fVar14;
  
  iVar5 = param_1 + param_2 * 4;
  fVar10 = *(float *)(iVar5 + 0x13c);
  iVar3 = param_2 * 0x20 + 0xc;
  fVar12 = (float)(ulonglong)*(uint *)(iVar5 + 0x19c) + fVar10;
  iVar6 = *(int *)(param_1 + iVar3);
  uVar7 = *(uint *)(param_1 + iVar3 + 4);
  uVar11 = (uint)(0.0 < fVar12) * (int)fVar12;
  if (param_2 == 1) {
    *(float *)(param_1 + 100) = fVar12;
  }
  uVar1 = (ulonglong)uVar11;
  if (uVar7 < uVar11) {
    uVar11 = uVar11 - uVar7 * (uVar11 / uVar7);
  }
  uVar13 = FPMaxNum(*(float *)(param_1 + 0xa4) * *(float *)(iVar5 + 0xa8),
                    *(undefined4 *)(param_1 + 0xb8));
  fVar14 = (float)FPMinNum(uVar13,*(undefined4 *)(param_1 + 0xbc));
  fVar8 = *(float *)(iVar6 + uVar11 * 4);
  fVar9 = (float)FPMinNum(*(undefined4 *)(param_1 + 0xdc),*(undefined4 *)(iVar5 + 0xe0));
  fVar14 = *(float *)(iVar5 + 0xf8) + (fVar14 - *(float *)(iVar5 + 0xf8)) * fVar9;
  fVar10 = fVar10 + fVar14;
  uVar4 = *(int *)(iVar5 + 0x164) - 1;
  fVar9 = *(float *)(iVar6 + ((uVar11 + 1) - uVar7 * ((uVar11 + 1) / uVar7)) * 4);
  *(float *)(iVar5 + 0xf8) = fVar14;
  *(float *)(iVar5 + 0x13c) = fVar10;
  fVar8 = fVar8 + (fVar12 - (float)uVar1) * (fVar9 - fVar8);
  if ((int)((uint)(fVar10 < (float)(ulonglong)*(uint *)(iVar5 + 0x15c)) << 0x1f) < 0) {
    *(float *)(iVar5 + 0x13c) = (float)(ulonglong)uVar4;
    *param_3 = '\x01';
  }
  else {
    fVar12 = (float)(ulonglong)uVar4;
    if (fVar10 != fVar12 && fVar10 < fVar12 == (NAN(fVar10) || NAN(fVar12))) {
      *(float *)(iVar5 + 0x13c) = (float)(ulonglong)*(uint *)(iVar5 + 0x15c);
      *param_3 = '\x01';
      cVar2 = *(char *)(param_1 + 0x11a);
      goto joined_r0x08004a3c;
    }
    if (*param_3 == '\0') {
      return fVar8;
    }
  }
  cVar2 = *(char *)(param_1 + 0x11a);
joined_r0x08004a3c:
  if (cVar2 == '\0') {
    if (fVar14 == 0.0 || fVar14 < 0.0 != NAN(fVar14)) {
      uVar13 = 0;
      if (*(int *)(iVar5 + 0x194) == 0) {
        uVar13 = *(undefined4 *)(iVar5 + 0x154);
      }
      *(undefined4 *)(iVar5 + 0x19c) = uVar13;
    }
    else {
      *(undefined4 *)(iVar5 + 0x19c) = *(undefined4 *)(iVar5 + 0x194);
    }
  }
  return fVar8;
}



/* === 08004a6c DB_Buffer_Init === */

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



/* === 08004bf8 DB_MacroBreak_ChooseRepeatSilencePosition === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_MacroBreak_ChooseRepeatSilencePosition(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  if ((*(int *)(param_1 + 0x68) == 1) && (param_2 == 1)) {
    *(undefined4 *)(param_1 + 0x74) = *(undefined4 *)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x90) = *(undefined4 *)(param_1 + 0x8c);
    *(undefined4 *)(param_1 + 200) = *(undefined4 *)(param_1 + 0xc4);
    return;
  }
  fVar6 = *(float *)(param_1 + 0x108);
  fVar5 = DAT_08004d44;
  if (fVar6 != DAT_08004d40 && fVar6 < DAT_08004d40 == (NAN(fVar6) || NAN(DAT_08004d40))) {
    iVar3 = newlib_rand_LCG64();
    fVar6 = *(float *)(param_1 + 0x108);
    fVar5 = ((float)(longlong)
                    (iVar3 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar3) >> 0x20)
                               + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) / DAT_08004d50) *
            fVar6 * 8.0;
    fVar5 = (float)(ulonglong)((uint)(0.0 < fVar5) * (int)fVar5);
  }
  iVar3 = param_1 + param_2 * 4;
  *(float *)(iVar3 + 0x70) = fVar5;
  if (fVar6 == 0.5 || fVar6 < 0.5 != NAN(fVar6)) {
    uVar2 = 0;
  }
  else {
    iVar1 = newlib_rand_LCG64();
    fVar6 = *(float *)(param_1 + 0x108);
    uVar2 = FPMaxNum(fVar6 * 2.0 + -1.0,DAT_08004d44);
    fVar5 = (float)FPMinNum(uVar2,0x3f800000);
    fVar5 = (float)FPRoundInt(fVar5 * 4.0,0x20,4,0);
    fVar5 = ((float)(longlong)
                    (iVar1 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar1) >> 0x20)
                               + iVar1 >> 7) - (iVar1 >> 0x1f)) * -0xff) / DAT_08004d50) * fVar5;
    uVar2 = *(undefined4 *)(DAT_08004d54 + (uint)(0.0 < fVar5) * (int)fVar5 * 4);
  }
  fVar5 = DAT_08004d48;
  *(undefined4 *)(iVar3 + 0x8c) = uVar2;
  fVar4 = DAT_08004d44;
  if (fVar6 != fVar5 && fVar6 < fVar5 == (NAN(fVar6) || NAN(fVar5))) {
    iVar1 = newlib_rand_LCG64();
    fVar4 = (float)(longlong)
                   (iVar1 + (((int)((ulonglong)((longlong)DAT_08004d4c * (longlong)iVar1) >> 0x20) +
                              iVar1 >> 7) - (iVar1 >> 0x1f)) * -0xff) / DAT_08004d50;
  }
  *(float *)(iVar3 + 0xc4) = fVar4;
  return;
}



/* === 08004d58 DB_Buffer_ProcessBlock === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DB_Buffer_ProcessBlock(int param_1,undefined4 *param_2,undefined4 *param_3,uint param_4)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  float fVar4;
  uint uVar5;
  uint uVar6;
  undefined4 uVar7;
  float *pfVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  undefined4 **ppuVar12;
  undefined4 **ppuVar13;
  undefined *puVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar19;
  float fVar20;
  double dVar18;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  undefined4 *puVar25;
  undefined4 *local_68;
  int local_64;
  uint local_60;
  undefined4 **local_5c;
  uint local_58;
  int local_54;
  uint local_50;
  undefined4 **local_4c;
  uint local_48;
  undefined4 **local_44;
  char local_39 [5];
  
  ppuVar13 = &local_68 + param_4 * -2;
  local_5c = ppuVar13 + param_4 * -2;
  local_68 = param_3;
  local_50 = param_4;
  if (param_4 == 0) {
    local_60 = 0;
  }
  else {
    uVar5 = 0;
    local_60 = param_4 & 0x3fffffff;
    do {
      uVar6 = uVar5 >> 1;
      uVar5 = uVar5 + 2;
      puVar9 = (undefined4 *)param_2[1];
      ppuVar13[uVar6] = (undefined4 *)*param_2;
      ppuVar13[(param_4 & 0x3fffffff) + uVar6] = puVar9;
      param_2 = param_2 + 2;
    } while (uVar5 < param_4);
  }
  if (*(char *)(param_1 + 0x110) == '\0') {
    *(undefined4 *)(param_1 + 0x114) = 0;
    *(undefined4 *)(param_1 + 0x58) = *(undefined4 *)(param_1 + 0x50);
    if (*(char *)(param_1 + 0x119) != '\0') {
      *(undefined *)(param_1 + 0x119) = 0;
      *(undefined2 *)(param_1 + 0x234) = 0x101;
    }
  }
  else {
    fVar15 = *(float *)(param_1 + 0x50);
    fVar19 = *(float *)(param_1 + 0x58);
    *(undefined2 *)(param_1 + 0x118) = 0;
    uVar7 = 1;
    *(undefined2 *)(param_1 + 0x236) = 0x101;
    *(undefined *)(param_1 + 0x111) = 1;
    if (fVar15 == fVar19 || fVar15 < fVar19 != (NAN(fVar15) || NAN(fVar19))) {
      uVar7 = 0xffffffff;
    }
    *(undefined4 *)(param_1 + 0x114) = uVar7;
  }
  puVar14 = (undefined *)(param_1 + 0x22a);
  local_58 = local_50 >> 1;
  pfVar8 = (float *)(param_1 + 0x70);
  iVar11 = 0;
  local_54 = local_60 << 2;
  local_64 = local_58 * -4;
  local_44 = local_5c + local_58;
  iVar10 = param_1;
  local_4c = ppuVar13;
  do {
    if (local_58 != 0) {
      ppuVar12 = (undefined4 **)((int)local_44 + local_64);
      ppuVar13 = local_4c;
      do {
        fVar15 = *(float *)(param_1 + 0x50);
        if (iVar11 == 0) {
          fVar15 = fVar15 + (*(float *)(param_1 + 0x54) - fVar15) * DAT_08005378;
          fVar19 = 1.0 / fVar15;
          *(float *)(param_1 + 0x50) = fVar15;
          *(float *)(param_1 + 0x5c) = fVar19;
        }
        else {
          fVar19 = *(float *)(param_1 + 0x5c);
        }
        fVar24 = *(float *)(param_1 + 0x4c);
        fVar15 = fVar24 / fVar15;
        fVar16 = pfVar8[0x33];
        fVar4 = (float)(*(uint *)(param_1 + 8) >> 1);
        fVar20 = (float)(ulonglong)((int)pfVar8[0x3d] - 1);
        fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
        if ((int)fVar4 - 1U < (uint)fVar15) {
          fVar15 = fVar4;
        }
        if ((fVar16 == fVar20 || fVar16 < fVar20 != (NAN(fVar16) || NAN(fVar20))) &&
           (-1 < (int)((uint)(fVar16 < (float)(ulonglong)(uint)pfVar8[0x3b]) << 0x1f))) {
          fVar4 = pfVar8[0x22];
LAB_08004e98:
          if (puVar14[0xc] == '\0') goto LAB_0800535a;
LAB_08004ea2:
          fVar16 = pfVar8[0x60];
          if (((fVar16 < 0.0 == NAN(fVar16)) && (pfVar8[0x62] <= 0.0)) ||
             ((fVar16 <= 0.0 && (pfVar8[0x62] < 0.0 == NAN(pfVar8[0x62]))))) {
            pfVar8[0x39] = fVar15;
          }
          *puVar14 = 0;
        }
        else {
          fVar4 = pfVar8[0x22];
          if (fVar4 == 0.0 || fVar4 < 0.0 != NAN(fVar4)) {
            pfVar8[0x33] = (float)(ulonglong)((int)pfVar8[0x3f] - 1);
            goto LAB_08004e98;
          }
          cVar2 = puVar14[0xc];
          pfVar8[0x33] = (float)(ulonglong)(uint)pfVar8[0x3b];
          if (cVar2 != '\0') goto LAB_08004ea2;
LAB_0800535a:
          pfVar8[0x39] = fVar15;
        }
        fVar24 = fVar24 * fVar19;
        uVar7 = FPMaxNum(pfVar8[7] + *(float *)(param_1 + 0x88),*(undefined4 *)(param_1 + 0x9c));
        fVar19 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
        fVar15 = fVar24 - (float)(ulonglong)(uint)pfVar8[0x37];
        fVar16 = (float)(ulonglong)(uint)pfVar8[0x45];
        fVar19 = (1.0 - fVar19) * (float)(ulonglong)(uint)pfVar8[0x37];
        pfVar8[0x47] = (float)((uint)(0.0 < fVar19) * (int)fVar19);
        if (fVar16 != fVar15 && fVar16 < fVar15 == (NAN(fVar16) || NAN(fVar15))) {
          pfVar8[0x45] = 0.0;
        }
        local_39[0] = '\0';
        if (fVar4 == 0.0 || fVar4 < 0.0 != NAN(fVar4)) {
          puVar9 = (undefined4 *)DB_Buffer_ReadLinear(param_1,iVar11,local_39);
          puVar25 = *ppuVar13;
          DB_Buffer_WriteSample(puVar25,param_1,iVar11,param_1 + 0x238);
        }
        else {
          puVar25 = *ppuVar13;
          DB_Buffer_WriteSample(puVar25,param_1,iVar11,param_1 + 0x238);
          puVar9 = (undefined4 *)DB_Buffer_ReadLinear(param_1,iVar11,local_39);
        }
        fVar15 = pfVar8[0x39];
        uVar6 = *(uint *)(iVar10 + 0x1cc);
        uVar5 = *(uint *)(iVar10 + 0x1c0);
        if ((uint)((int)fVar15 << 1) <= *(uint *)(iVar10 + 0x1c0)) {
          uVar5 = (int)fVar15 << 1;
        }
        *(uint *)(iVar10 + 0x1c4) = uVar5;
        if ((uVar6 <= *(int *)(iVar10 + 0x1c8) - 1U) && (uVar5 < uVar6)) {
          uVar5 = uVar6;
        }
        *(undefined4 **)(*(int *)(iVar10 + 0x1bc) + uVar5 * 4) = puVar25;
        *(uint *)(iVar10 + 0x1cc) = uVar5 + 1;
        fVar4 = pfVar8[0x47];
        *(float *)(iVar10 + 0x200) = fVar4;
        fVar20 = *(float *)(param_1 + 0x10c);
        fVar19 = (float)(ulonglong)(uint)fVar4 * fVar20 * 0.5;
        dVar18 = (double)FPMaxNum((double)(ulonglong)((uint)(0.0 < fVar19) * (int)fVar19),
                                  0x4038000000000000);
        fVar21 = (float)((uint)(0.0 < dVar18) * (int)(longlong)dVar18);
        *(float *)(iVar10 + 0x204) = fVar21;
        fVar17 = pfVar8[0x33];
        fVar16 = (float)(ulonglong)(uint)pfVar8[0x3b];
        fVar22 = (float)((uint)(0.0 < fVar17 - fVar16) * (int)(fVar17 - fVar16));
        *(float *)(iVar10 + 0x208) = fVar22;
        fVar19 = DAT_08005360;
        fVar23 = (float)(ulonglong)((int)pfVar8[0x3f] - 1);
        puVar25 = DAT_08005390;
        if ((fVar17 == fVar23 || fVar17 < fVar23 != (NAN(fVar17) || NAN(fVar23))) &&
           (puVar25 = puVar9, (int)((uint)(fVar17 < fVar16) << 0x1f) < 0)) {
          puVar25 = DAT_08005390;
        }
        *ppuVar12 = puVar25;
        if (fVar20 != fVar19 && fVar20 < fVar19 == (NAN(fVar20) || NAN(fVar19))) {
          puVar9 = puVar25;
          if (((uint)fVar22 < (uint)fVar21) ||
             ((uint)((int)fVar4 - (int)fVar21) <= (uint)fVar22 &&
              (int)fVar22 - ((int)fVar4 - (int)fVar21) != 0)) {
            if ((uint)fVar22 < (uint)fVar4 >> 1) {
              fVar19 = (float)(longlong)(int)fVar22 / (float)(ulonglong)(uint)fVar21;
              *(float *)(iVar10 + 0x20c) = fVar19;
              puVar9 = (undefined4 *)((float)puVar25 * fVar19);
            }
            else {
              fVar19 = (float)(ulonglong)(uint)((int)fVar4 - (int)fVar22) /
                       (float)(ulonglong)(uint)fVar21;
              if (fVar19 == 1.0 || fVar19 < 1.0 != NAN(fVar19)) {
                puVar9 = (undefined4 *)((float)puVar25 * fVar19);
                *(float *)(iVar10 + 0x20c) = fVar19;
              }
              else {
                *(undefined4 *)(iVar10 + 0x20c) = 0x3f800000;
              }
            }
          }
          else if ((uint)fVar22 < (uint)fVar4) {
            *(undefined4 *)(iVar10 + 0x20c) = 0x3f800000;
          }
          else {
            puVar9 = (undefined4 *)((float)puVar25 * DAT_08005728);
            *(float *)(iVar10 + 0x20c) = DAT_08005728;
          }
          fVar19 = pfVar8[0x35];
          if ((int)((uint)(fVar19 < DAT_0800537c) << 0x1f) < 0) {
            puVar9 = (undefined4 *)((float)puVar9 * (fVar19 / DAT_0800537c));
          }
          else {
            fVar4 = (float)(ulonglong)(uint)fVar15 - DAT_0800538c;
            if (fVar19 != fVar4 && fVar19 < fVar4 == (NAN(fVar19) || NAN(fVar4))) {
              uVar7 = FPMaxNum(((float)(ulonglong)(uint)fVar15 - (fVar19 + DAT_0800537c)) /
                               DAT_0800537c,DAT_08005390);
              fVar19 = (float)FPMinNum(uVar7,0x3f800000);
              puVar9 = (undefined4 *)((float)puVar9 * fVar19);
            }
          }
          *ppuVar12 = puVar9;
        }
        fVar19 = pfVar8[0x60];
        pfVar8[0x60] = (float)puVar25;
        pfVar8[0x62] = fVar19;
        if (local_39[0] != '\0') {
          pfVar8[0x4f] = (float)((int)pfVar8[0x4f] + 1);
        }
        if (puVar14[10] == '\0') {
joined_r0x0800532e:
          if (iVar11 == 0) {
LAB_080051fe:
            uVar5 = *(uint *)(param_1 + 0x1b4);
            puVar9 = DAT_08005390;
            if ((uVar5 < 2) ||
               (puVar9 = (undefined4 *)((float)(ulonglong)(uVar5 - 1) * *(float *)(param_1 + 0xc0)),
               uVar5 < 5)) {
              bVar1 = (float)puVar9 - *(float *)(param_1 + 0x1ec) != 0.0;
              fVar15 = (float)((uint)bVar1 * DAT_08005384 + (uint)!bVar1 * DAT_08005388);
            }
            else {
              bVar1 = (float)puVar9 - *(float *)(param_1 + 0x1ec) != 0.0;
              fVar15 = (float)((uint)bVar1 * DAT_08005370 + (uint)!bVar1 * DAT_08005374);
            }
            *(float *)(param_1 + 0x1ec) = (float)(longlong)(int)(fVar15 + (float)puVar9 + 0.5);
          }
          if (local_39[0] != '\0') {
            fVar15 = pfVar8[0x51];
            uVar5 = (uint)(0.0 < *(float *)(param_1 + 0x1ec)) * (int)*(float *)(param_1 + 0x1ec);
            if (1 < (uint)fVar15) {
              if ((iVar11 == 0) || (*(int *)(param_1 + 0x68) != 1)) {
                local_48 = uVar5;
                iVar3 = newlib_rand_LCG64();
                fVar19 = DAT_08005368;
                fVar15 = (float)(longlong)
                                (iVar3 + (((int)((ulonglong)
                                                 ((longlong)DAT_08005364 * (longlong)iVar3) >> 0x20)
                                           + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) / DAT_08005368;
                if ((fVar15 == 0.75 || fVar15 < 0.75 != NAN(fVar15)) ||
                   (fVar15 = *(float *)(param_1 + 0x108),
                   fVar15 == DAT_0800536c ||
                   fVar15 < DAT_0800536c != (NAN(fVar15) || NAN(DAT_0800536c)))) {
                  fVar19 = pfVar8[0x15];
                  fVar15 = pfVar8[0x51];
                  uVar5 = local_48;
                }
                else {
                  iVar3 = newlib_rand_LCG64();
                  fVar15 = pfVar8[0x51];
                  fVar19 = (float)(longlong)
                                  (iVar3 + (((int)((ulonglong)
                                                   ((longlong)DAT_08005724 * (longlong)iVar3) >>
                                                  0x20) + iVar3 >> 7) - (iVar3 >> 0x1f)) * -0xff) /
                           fVar19;
                  pfVar8[0x15] = fVar19;
                  uVar5 = local_48;
                }
              }
              else {
                *(undefined4 *)(param_1 + 200) = *(undefined4 *)(param_1 + 0xc4);
                fVar19 = pfVar8[0x15];
              }
              uVar5 = (int)((float)(ulonglong)(uint)fVar15 * fVar19) + uVar5;
              if ((int)fVar15 - 1U <= uVar5) {
                uVar5 = (int)fVar15 - 1U;
              }
            }
            fVar19 = (float)((int)pfVar8[0x37] * uVar5);
            fVar15 = (float)((int)pfVar8[0x39] - (int)pfVar8[0x37]);
            if ((uint)fVar19 <= (uint)fVar15) {
              fVar15 = fVar19;
            }
            pfVar8[0x45] = fVar15;
          }
          if (iVar11 != 0) goto LAB_080054f6;
          if (puVar14[0xe] != '\0') {
            DB_MacroBreak_ChooseRepeatSilencePosition(param_1,0);
            goto LAB_08005546;
          }
        }
        else {
          if (*(int *)(param_1 + 0x120) != 0) {
            if ((((float)puVar25 < 0.0 == NAN((float)puVar25)) && (fVar19 <= 0.0)) ||
               (((float)puVar25 <= 0.0 && (fVar19 < 0.0 == NAN(fVar19))))) goto LAB_08005262;
            goto joined_r0x0800532e;
          }
          if (((((float)puVar25 < 0.0 == NAN((float)puVar25)) && (fVar19 <= 0.0)) ||
              (((float)puVar25 <= 0.0 && (fVar19 < 0.0 == NAN(fVar19))))) || (local_39[0] != '\0'))
          {
LAB_08005262:
            cVar2 = puVar14[0xc];
            *(undefined4 *)(param_1 + 0x120) = 0;
            if (cVar2 != '\0') {
              uVar5 = *(uint *)(param_1 + 8);
              fVar4 = *(float *)(param_1 + 0x4c) / *(float *)(param_1 + 0x50);
              pfVar8[0x43] = fVar15;
              fVar19 = (float)(uVar5 >> 1);
              fVar15 = (float)((uint)(0.0 < fVar4) * (int)fVar4);
              if ((int)fVar19 - 1U < (uint)fVar15) {
                fVar15 = fVar19;
              }
              pfVar8[0x39] = fVar15;
              puVar14[0xc] = 0;
            }
            fVar15 = pfVar8[0x22];
            pfVar8[0x35] = 0.0;
            pfVar8[0x33] = (float)((uint)(fVar15 != 0.0) * (int)fVar16 +
                                  (uint)(fVar15 == 0.0) * (int)fVar23);
            *(undefined4 *)(iVar10 + 0x208) = 0;
            pfVar8[0x4f] = 0.0;
            puVar14[10] = 0;
            *puVar14 = 0;
            puVar14[0xe] = 1;
            pfVar8[0x6f] = *(float *)(param_1 + 0x124);
            *(char *)(param_1 + 0x11a) = *(char *)(param_1 + 0x11b);
            if ((*(char *)(param_1 + 0x11b) == '\0') && (*(char *)(param_1 + 0x110) == '\0')) {
              fVar19 = pfVar8[0x39];
              if ((uint)fVar19 >> 2 < (uint)((int)pfVar8[0x2e] - (int)pfVar8[0x4d])) {
                pfVar8[0x4d] = pfVar8[0x2e];
                fVar4 = fVar19;
                if (pfVar8[0x49] != 0.0) {
                  fVar4 = 0.0;
                }
                pfVar8[0x49] = fVar4;
              }
              if (fVar15 == 0.0 || fVar15 < 0.0 != NAN(fVar15)) {
                if (pfVar8[0x49] != 0.0) {
                  fVar19 = 0.0;
                }
                pfVar8[0x4b] = fVar19;
              }
              else {
                pfVar8[0x4b] = pfVar8[0x49];
              }
            }
            goto joined_r0x0800532e;
          }
          if (iVar11 == 0) goto LAB_080051fe;
LAB_080054f6:
          if (*(int *)(param_1 + 0x120) != 0) {
            *(int *)(param_1 + 0x120) = *(int *)(param_1 + 0x120) + -1;
          }
          if (puVar14[0xe] != '\0') {
            DB_MacroBreak_ChooseRepeatSilencePosition(param_1,1);
            if (*(int *)(param_1 + 0x68) == 1) {
              *(undefined4 *)(param_1 + 0xac) = *(undefined4 *)(param_1 + 0xa8);
              *(undefined4 *)(param_1 + 0xe4) = *(undefined4 *)(param_1 + 0xe0);
            }
            else {
LAB_08005546:
              fVar15 = *(float *)(param_1 + 0x104);
              if (fVar15 == DAT_08005380 ||
                  fVar15 < DAT_08005380 != (NAN(fVar15) || NAN(DAT_08005380))) {
                pfVar8[0xe] = 1.0;
                pfVar8[0x1c] = 1.0;
              }
              else {
                DB_MacroBend_ChooseRateAndSlew(param_1,iVar11);
              }
            }
            puVar14[0xe] = 0;
          }
        }
        if ((int)((uint)(pfVar8[0x33] < (float)(ulonglong)(uint)pfVar8[0x3f]) << 0x1f) < 0) {
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x88) + pfVar8[7],*(undefined4 *)(param_1 + 0x9c));
          fVar15 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
          pfVar8[0x5d] = fVar15;
          if (local_39[0] != '\0') goto LAB_080053be;
LAB_08005194:
          fVar15 = pfVar8[0x60];
          if (((fVar15 < 0.0 == NAN(fVar15)) && (pfVar8[0x62] <= 0.0)) ||
             ((fVar15 <= 0.0 && (pfVar8[0x62] < 0.0 == NAN(pfVar8[0x62]))))) goto LAB_080053be;
        }
        else {
          if (local_39[0] == '\0') goto LAB_08005194;
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x88) + pfVar8[7],*(undefined4 *)(param_1 + 0x9c));
          fVar15 = (float)FPMinNum(uVar7,*(undefined4 *)(param_1 + 0xa0));
          pfVar8[0x5d] = fVar15;
LAB_080053be:
          uVar7 = FPMaxNum(*(float *)(param_1 + 0x6c) + *pfVar8,(int)*(undefined8 *)(param_1 + 0x80)
                          );
          uVar7 = FPMinNum(uVar7,(int)((ulonglong)*(undefined8 *)(param_1 + 0x80) >> 0x20));
          fVar15 = (float)libm_powf(0x40000000,uVar7);
          fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
          fVar19 = pfVar8[0x45];
          pfVar8[0x51] = fVar15;
          pfVar8[0x3b] = fVar19;
          fVar15 = fVar24 / (float)(ulonglong)(uint)fVar15 + 1.0;
          fVar15 = (float)((uint)(0.0 < fVar15) * (int)fVar15);
          if ((uint)pfVar8[0x39] <= (uint)fVar15) {
            fVar15 = pfVar8[0x39];
          }
          pfVar8[0x37] = fVar15;
          pfVar8[0x3d] = (float)((int)fVar15 + (int)fVar19);
          pfVar8[0x3f] = (float)((int)pfVar8[0x47] + (int)fVar19);
        }
        ppuVar12 = ppuVar12 + 1;
        ppuVar13 = ppuVar13 + 1;
        pfVar8[0x2e] = (float)((int)pfVar8[0x2e] + 1);
      } while (local_44 != ppuVar12);
    }
    pfVar8 = pfVar8 + 1;
    puVar14 = puVar14 + 1;
    iVar10 = iVar10 + 0x14;
    local_44 = (undefined4 **)((int)local_44 + local_54);
    local_4c = (undefined4 **)((int)local_4c + local_54);
    if (iVar11 != 0) {
      if (local_50 != 0) {
        uVar5 = 0;
        do {
          uVar6 = uVar5 >> 1;
          uVar5 = uVar5 + 2;
          puVar9 = local_5c[uVar6 + local_60];
          *local_68 = local_5c[uVar6];
          local_68[1] = puVar9;
          local_68 = local_68 + 2;
        } while (uVar5 < local_50);
      }
      return;
    }
    iVar11 = 1;
  } while( true );
}



/* === 0800572c DataBenderHardware_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DataBenderHardware_Init(int param_1)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  byte bVar5;
  uint uVar6;
  undefined *puVar7;
  int iVar8;
  undefined *puVar9;
  undefined *puVar10;
  int iVar11;
  char cVar12;
  uint *puVar13;
  undefined4 *puVar14;
  int iVar15;
  uint *puVar16;
  byte *pbVar17;
  int iVar19;
  undefined4 local_4e4;
  uint local_4e0 [4];
  uint local_4d0;
  undefined4 local_4cc;
  undefined4 local_4c8;
  undefined4 local_4c4;
  undefined4 local_4c0;
  undefined local_4bc;
  uint local_4b8 [6];
  uint local_4a0;
  uint uStack_49c;
  uint uStack_498;
  uint uStack_494;
  uint local_490;
  uint uStack_48c;
  uint local_488 [4];
  uint local_478;
  uint uStack_474;
  uint uStack_470;
  undefined4 local_46c;
  undefined auStack_41c [424];
  undefined auStack_274 [592];
  byte *pbVar18;
  
  local_488[0] = *DAT_080058a0;
  local_488[1] = DAT_080058a0[1];
  local_488[2] = DAT_080058a0[2];
  local_488[3] = DAT_080058a0[3];
  local_478 = DAT_080058a0[4];
  uStack_474 = DAT_080058a0[5];
  uStack_470 = DAT_080058a0[6];
  local_4e0[0] = DAT_080058a0[7];
  local_4e0[1] = DAT_080058a0[8];
  local_4e0[2] = DAT_080058a0[9];
  local_4e0[3] = DAT_080058a0[10];
  local_4d0 = DAT_080058a0[0xb];
  local_4b8[0] = DAT_080058a0[0xc];
  local_4b8[1] = DAT_080058a0[0xd];
  local_4b8[2] = DAT_080058a0[0xe];
  local_4b8[3] = DAT_080058a0[0xf];
  local_4b8[4] = DAT_080058a0[0x10];
  local_4b8[5] = DAT_080058a0[0x11];
  local_4a0 = DAT_080058a0[0x12];
  uStack_49c = DAT_080058a0[0x13];
  uStack_498 = DAT_080058a0[0x14];
  uStack_494 = DAT_080058a0[0x15];
  local_490 = DAT_080058a0[0x16];
  uStack_48c = DAT_080058a0[0x17];
  DaisySeed_Configure(param_1);
  DaisySeed_Init(param_1,0);
  *(undefined4 *)(param_1 + 0x64c) = 0x60;
  DaisySeed_SetAudioBlockSize(param_1);
  iVar3 = param_1 + 0x170;
  puVar13 = local_4e0;
  do {
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    iVar11 = iVar3 + 0x18;
    local_46c = CONCAT22(local_46c._2_2_,uVar1);
    GateIn_Init(iVar3,local_46c,1);
    iVar3 = iVar11;
    puVar13 = puVar13 + 1;
  } while (iVar11 != param_1 + 0x1d0);
  puVar13 = &uStack_48c;
  uVar1 = DaisySeed_GetPin(0xd);
  GPIO_Init(param_1 + 0x620,uVar1,0,0,0);
  uVar1 = DaisySeed_GetPin(0xd);
  GateIn_Init(param_1 + 0x634,uVar1,1);
  iVar3 = param_1 + 0x74;
  do {
    puVar13 = puVar13 + 1;
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    local_46c = CONCAT22(local_46c._2_2_,uVar1);
    iVar11 = iVar3 + 0x24;
    Switch_Init(DAT_080058a4 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),iVar3,local_46c);
    iVar3 = iVar11;
  } while (iVar11 != param_1 + 0x170);
  local_4c8 = 0xff0bff0b;
  local_4bc = 0x10;
  uVar1 = DaisySeed_GetPin(0xb);
  local_4c8 = CONCAT22(local_4c8._2_2_,uVar1);
  uVar1 = DaisySeed_GetPin(0xc);
  local_4c8 = CONCAT22(uVar1,(undefined2)local_4c8);
  local_4c0 = 0;
  local_4cc = 0;
  local_4e4 = 0;
  local_4c4 = 2;
  I2CHandle_Init(&local_4e4,&local_4cc);
  iVar3 = DAT_080058a8;
  uVar6 = 0;
  iVar11 = param_1 + 0x350;
  *(int *)(param_1 + 0x354) = DAT_080058a8;
  *(undefined4 *)(param_1 + 0x350) = local_4e4;
  *(int *)(param_1 + 0x358) = iVar3 + 0x84;
  *(undefined4 *)(param_1 + 0x35c) = DAT_080058ac;
  *(undefined *)(param_1 + 0x374) = 0xff;
  while( true ) {
    uVar2 = uVar6 & 0xf;
    iVar8 = uVar6 << 2;
    iVar19 = ((int)uVar6 >> 4) * 0x41;
    uVar6 = uVar6 + 1;
    uVar1 = (undefined2)iVar8;
    *(undefined *)(iVar3 + iVar19) = 6;
    iVar3 = iVar19 + uVar2 * 4;
    iVar15 = *(int *)(param_1 + 0x358);
    iVar8 = *(int *)(param_1 + 0x354) + iVar3;
    *(undefined2 *)(iVar8 + 1) = uVar1;
    *(undefined2 *)(iVar8 + 3) = uVar1;
    *(undefined *)(iVar15 + iVar19) = 6;
    iVar3 = *(int *)(param_1 + 0x358) + iVar3;
    *(undefined2 *)(iVar3 + 1) = uVar1;
    *(undefined2 *)(iVar3 + 3) = uVar1;
    if (uVar6 == 0x20) break;
    iVar3 = *(int *)(param_1 + 0x354);
  }
  if (*(char *)(param_1 + 0x35e) != '\v') {
    GPIO_Init(param_1 + 0x360,*(undefined2 *)(param_1 + 0x35e),1,0,0);
    GPIO_Write(param_1 + 0x360,0);
  }
  pbVar18 = (byte *)(param_1 + 0x35c);
  do {
    pbVar17 = pbVar18 + 1;
    bVar5 = *pbVar18 | 0x40;
    local_46c = local_46c & 0xffff0000;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c._0_2_ = 0;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c._0_2_ = 0x2000;
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,1);
    System_Delay(0x14);
    local_46c = CONCAT22(local_46c._2_2_,0x3600);
    local_46c = CONCAT31(local_46c._1_3_,1);
    I2CHandle_TransmitBlocking(iVar11,bVar5,&local_46c,2,5);
    uVar4 = DAT_08005bcc;
    pbVar18 = pbVar17;
  } while ((byte *)(param_1 + 0x35e) != pbVar17);
  *(undefined2 *)(param_1 + 0x57c) = 0x100;
  *(int *)(param_1 + 0x578) = iVar11;
  *(undefined *)(param_1 + 0x57e) = 2;
  *(undefined4 *)(param_1 + 0x58c) = 0;
  *(undefined4 *)(param_1 + 0x588) = 0;
  *(undefined4 *)(param_1 + 0x584) = 0;
  *(undefined4 *)(param_1 + 0x580) = uVar4;
  *(undefined *)(param_1 + 0x596) = 5;
  *(int *)(param_1 + 0x590) = iVar11;
  *(undefined2 *)(param_1 + 0x594) = 0x403;
  *(undefined4 *)(param_1 + 0x5a4) = 0;
  *(undefined4 *)(param_1 + 0x5a0) = 0;
  *(undefined4 *)(param_1 + 0x59c) = 0;
  *(undefined4 *)(param_1 + 0x598) = uVar4;
  *(undefined2 *)(param_1 + 0x5ac) = 0x706;
  *(int *)(param_1 + 0x5a8) = iVar11;
  *(undefined *)(param_1 + 0x5ae) = 8;
  *(undefined4 *)(param_1 + 0x5bc) = 0;
  *(undefined4 *)(param_1 + 0x5b8) = 0;
  *(undefined4 *)(param_1 + 0x5b4) = 0;
  *(undefined4 *)(param_1 + 0x5b0) = uVar4;
  *(undefined2 *)(param_1 + 0x5c4) = 0xa09;
  *(int *)(param_1 + 0x5c0) = iVar11;
  *(undefined *)(param_1 + 0x5c6) = 0xb;
  *(undefined4 *)(param_1 + 0x5d4) = 0;
  *(undefined4 *)(param_1 + 0x5d0) = 0;
  *(undefined4 *)(param_1 + 0x5cc) = 0;
  *(undefined4 *)(param_1 + 0x5c8) = uVar4;
  *(int *)(param_1 + 0x5d8) = iVar11;
  *(undefined2 *)(param_1 + 0x5dc) = 0xd0c;
  *(undefined *)(param_1 + 0x5de) = 0xe;
  *(undefined4 *)(param_1 + 0x5ec) = 0;
  *(undefined4 *)(param_1 + 0x5e8) = 0;
  *(undefined4 *)(param_1 + 0x5e4) = 0;
  *(undefined4 *)(param_1 + 0x5e0) = uVar4;
  *(int *)(param_1 + 0x5f0) = iVar11;
  *(undefined2 *)(param_1 + 0x5f4) = 0x1110;
  *(undefined *)(param_1 + 0x5f6) = 0x12;
  *(undefined4 *)(param_1 + 0x604) = 0;
  *(undefined4 *)(param_1 + 0x600) = 0;
  *(undefined4 *)(param_1 + 0x5fc) = 0;
  *(undefined4 *)(param_1 + 0x5f8) = uVar4;
  *(int *)(param_1 + 0x608) = iVar11;
  *(undefined2 *)(param_1 + 0x60c) = 0x1413;
  *(undefined *)(param_1 + 0x60e) = 0x15;
  puVar7 = auStack_41c;
  *(undefined4 *)(param_1 + 0x610) = uVar4;
  *(undefined4 *)(param_1 + 0x61c) = 0;
  *(undefined4 *)(param_1 + 0x618) = 0;
  *(undefined4 *)(param_1 + 0x614) = 0;
  do {
    puVar7[-0x4f] = 0xff;
    puVar7[-0x50] = 0xb;
    *(undefined4 *)(puVar7 + -0x4c) = 0;
    *(undefined4 *)(puVar7 + -0x48) = 0;
    *(undefined4 *)(puVar7 + -0x44) = 0;
    puVar9 = puVar7 + -0x3c;
    do {
      *(undefined4 *)(puVar9 + 4) = 0;
      *puVar9 = 0xb;
      *(undefined4 *)(puVar9 + 8) = 0;
      *(undefined4 *)(puVar9 + 0xc) = 0;
      puVar10 = puVar9 + 0x14;
      puVar9[1] = 0xff;
      puVar9 = puVar10;
    } while (puVar7 != puVar10);
    puVar7 = puVar7 + 0x54;
  } while (&stack0x00000028 != puVar7);
  puVar14 = &local_46c;
  puVar13 = local_4b8;
  do {
    puVar16 = puVar13 + 1;
    uVar1 = DaisySeed_GetPin(*puVar13 & 0xff);
    FUN_080071f4(puVar14,uVar1,2);
    puVar14 = puVar14 + 0x15;
    puVar13 = puVar16;
  } while (puVar16 != &local_4a0);
  puVar7 = auStack_274;
  do {
    puVar13 = puVar16 + 1;
    uVar1 = DaisySeed_GetPin(*puVar16 & 0xff);
    FUN_080071f4(puVar7,uVar1,2);
    puVar7 = puVar7 + 0x54;
    puVar16 = puVar13;
  } while (local_488 != puVar13);
  iVar19 = param_1 + 0x18;
  uVar6 = 0;
  FUN_08007214(iVar19,&local_46c,0xc,4);
  iVar3 = param_1 + 0x1d0;
  do {
    uVar4 = FUN_080075cc(iVar19,uVar6 & 0xff);
    uVar6 = uVar6 + 1;
    AnalogControl_Init(DAT_08005bc0 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),DAT_08005bc4,
                       iVar3,uVar4,0);
    iVar3 = iVar3 + 0x20;
  } while (uVar6 != 6);
  cVar12 = '\x06';
  iVar3 = param_1 + 0x290;
  do {
    uVar4 = FUN_080075cc(iVar19,cVar12);
    iVar8 = iVar3 + 0x20;
    cVar12 = cVar12 + '\x01';
    AnalogControl_InitBipolarCv
              (DAT_08005bc0 / (float)(ulonglong)*(uint *)(param_1 + 0x64c),iVar3,uVar4);
    iVar3 = iVar8;
  } while (iVar11 != iVar8);
  *(undefined4 *)(param_1 + 0x294) = DAT_08005bc8;
  return;
}



/* === 08005be8 DaisySeed_Configure === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_Configure(void)

{
  return;
}



/* === 08005bec DaisySeed_GetPin === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined2 DaisySeed_GetPin(uint param_1)

{
  if (param_1 < 0x21) {
    return CONCAT11(*(undefined *)(DAT_08005c14 + param_1 * 2 + 1),
                    *(undefined *)(DAT_08005c14 + param_1 * 2));
  }
  return 0xc01;
}



/* === 08005c18 DaisySeed_StartAudio_interleaved === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_StartAudio_interleaved(int param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)(param_1 + 0x14);
  SaiHandle_StartDma(puVar1 + 6,puVar1[8],puVar1[10],puVar1[2] << 2,DAT_08006f80);
  *puVar1 = 0;
  puVar1[1] = param_2;
  return;
}



/* === 08005c20 DaisySeed_AudioSampleRate === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_AudioSampleRate(int param_1)

{
  AudioHandle_GetSampleRate(param_1 + 0x14);
  return;
}



/* === 08005c28 DaisySeed_SetAudioBlockSize === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_SetAudioBlockSize(int param_1)

{
  uint *puVar1;
  int iVar2;
  float fVar3;
  
  iVar2 = param_1 + 0x14;
  AudioHandle_SetBlockSize(iVar2);
  fVar3 = (float)AudioHandle_GetSampleRate(iVar2);
  puVar1 = (uint *)AudioHandle_GetConfig(iVar2);
  *(float *)(param_1 + 0x6c) = fVar3 / (float)(ulonglong)*puVar1;
  return;
}



/* === 08005c60 DaisySeed_AudioBlockSize === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 DaisySeed_AudioBlockSize(int param_1)

{
  undefined4 *puVar1;
  
  puVar1 = (undefined4 *)AudioHandle_GetConfig(param_1 + 0x14);
  return *puVar1;
}



/* === 08005c6c DaisySeed_CheckBoardVersion === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 DaisySeed_CheckBoardVersion(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  undefined4 uStack_10;
  
  local_30 = 0xff0b;
  uStack_2c = 0;
  local_1c = 0xff0b;
  uStack_18 = 0;
  local_28 = 0;
  uStack_24 = 0;
  local_14 = 0;
  uStack_10 = 0;
  GPIO_Init(&local_1c,0x303,0,1,0);
  GPIO_Init(&local_30,0x403,0,1,0);
  iVar1 = GPIO_Read(&local_1c);
  if (iVar1 != 0) {
    iVar1 = GPIO_Read(&local_30);
    if (iVar1 == 0) {
      uVar2 = 2;
    }
    else {
      uVar2 = 0;
    }
    return uVar2;
  }
  return 1;
}



/* === 08005cd8 DaisySeed_ConfigureAudio === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_ConfigureAudio(int param_1)

{
  int iVar1;
  undefined4 local_78;
  undefined2 local_74;
  undefined local_72;
  int local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  int iStack_60;
  undefined4 local_5c;
  undefined local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 uStack_48;
  undefined4 local_40;
  int iStack_3c;
  int local_38;
  undefined2 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  int local_20;
  undefined4 local_1c;
  
  local_30 = 3;
  uStack_2c = 1;
  local_34 = 0xff0b;
  local_40 = 0;
  iStack_3c = DAT_08005df4;
  local_38 = DAT_08005df4 + -0x4f8fd00;
  local_28 = 0;
  uStack_24 = 1;
  iVar1 = DaisySeed_CheckBoardVersion();
  if (iVar1 == 1) {
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_34 = 0x304;
    local_5c = 0;
    local_58 = 0x10;
    local_78 = 0;
    local_64 = DAT_08005dfc;
    local_1c = 0;
    local_68 = iVar1;
    iStack_60 = iVar1;
    local_20 = iVar1;
    I2CHandle_Init(&local_78,&local_68);
    local_74 = 1;
    local_72 = 0;
    local_6c = 8;
    local_54 = 0;
    local_70 = iVar1;
    Wm8731_Init(&local_54,&local_74,local_78);
  }
  else if (iVar1 == 2) {
    local_54 = 0xff0b;
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_68 = CONCAT22(local_68._2_2_,0xb01);
    local_34 = 0x304;
    local_50 = 0;
    local_20 = 0;
    local_1c = 1;
    local_4c = 0;
    uStack_48 = 0;
    GPIO_Init(&local_54,local_68,1,0,0);
    GPIO_Write(&local_54,0);
  }
  else {
    local_54 = CONCAT22(local_54._2_2_,0xb01);
    local_20 = 0;
    local_1c = 1;
    local_38 = CONCAT22(0x604,(undefined2)local_38);
    local_34 = 0x304;
    Ak4556_Init(param_1 + 0x58,local_54);
  }
  SaiHandle_Init(param_1 + 0x70,&local_40);
  local_54 = *DAT_08005df8;
  local_50 = DAT_08005df8[1];
  local_4c = DAT_08005df8[2];
  uStack_48 = DAT_08005df8[3];
  AudioHandle_Init_single_SAI(param_1 + 0x14,&local_54,*(undefined4 *)(param_1 + 0x70));
  return;
}



/* === 08005e00 DaisySeed_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySeed_Init(int param_1,int param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  float fVar6;
  uint local_28;
  undefined2 local_24;
  undefined local_22;
  
  uVar2 = DAT_08005ef0;
  uVar1 = DAT_08005eec;
  local_28 = (uint)(param_2 != 0);
  local_22 = 0;
  local_24 = 0x101;
  *(undefined4 *)(param_1 + 4) = DAT_08005ee8;
  *(undefined4 *)(param_1 + 8) = uVar1;
  *(undefined4 *)(param_1 + 0xc) = uVar2;
  *(undefined2 *)(param_1 + 0x28) = 0x702;
  *(undefined2 *)(param_1 + 0x3c) = 0xe06;
  *(undefined2 *)(param_1 + 0x10) = 1;
  *(undefined4 *)(param_1 + 0x2c) = 1;
  *(undefined4 *)(param_1 + 0x40) = 1;
  iVar3 = System_GetProgramMemoryRegion();
  iVar4 = System_GetBootloaderVersion();
  if ((iVar4 == 0) && (iVar3 != 0)) {
    local_22 = 1;
    System_Init(param_1 + 0x50,&local_28);
    if (iVar3 != 7) {
      FUN_08008a80(param_1,param_1 + 4);
    }
  }
  else {
    System_Init(param_1 + 0x50,&local_28);
    if (iVar3 == 7) {
      if (iVar4 == 0) goto LAB_08005e62;
    }
    else {
      FUN_08008a80(param_1,param_1 + 4);
      if ((iVar4 == 0) && (iVar3 != 0)) goto LAB_08005e62;
    }
    FUN_08007928(param_1 + 0x28);
    FUN_08007928(param_1 + 0x3c);
    SdramHandle_Init(param_1 + 0x12);
  }
LAB_08005e62:
  DaisySeed_ConfigureAudio(param_1);
  fVar6 = (float)AudioHandle_GetSampleRate(param_1 + 0x14);
  puVar5 = (uint *)AudioHandle_GetConfig(param_1 + 0x14);
  *(float *)(param_1 + 0x6c) = fVar6 / (float)(ulonglong)*puVar5;
  return;
}



/* === 08005ef4 Ak4556_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Ak4556_Init(undefined4 param_1,undefined2 param_2)

{
  GPIO_Init(param_1,param_2,1,0,0);
  GPIO_Write(param_1,1);
  System_Delay(1);
  GPIO_Write(param_1,0);
  System_Delay(1);
  GPIO_Write(param_1,1);
  return;
}



/* === 08005f38 Wm8731_WriteRegister === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 Wm8731_WriteRegister(int param_1,int param_2,int param_3)

{
  int iVar1;
  byte local_c;
  undefined local_b;
  
  local_b = (undefined)param_3;
  local_c = (byte)((uint)(param_3 << 0x17) >> 0x1f) | (byte)(param_2 << 1);
  iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 0x10),&local_c,2,0xfa);
  if (iVar1 != 0) {
    return 1;
  }
  System_Delay(10);
  return 0;
}



/* === 08005f74 Wm8731_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 Wm8731_Init(undefined4 *param_1,undefined4 *param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined uVar4;
  undefined local_1c;
  byte local_1b;
  
  *param_1 = param_3;
  uVar2 = param_2[1];
  uVar3 = param_2[2];
  param_1[1] = *param_2;
  param_1[2] = uVar2;
  param_1[3] = uVar3;
  if (*(char *)((int)param_1 + 6) == '\0') {
    uVar2 = 0x1a;
    uVar4 = 0x1a;
  }
  else {
    uVar2 = 0x1b;
    uVar4 = 0x1b;
  }
  *(undefined *)(param_1 + 4) = uVar4;
  local_1c = 0x1e;
  local_1b = 0;
  iVar1 = I2CHandle_TransmitBlocking(param_1,uVar2,&local_1c,2,0xfa);
  if (iVar1 == 0) {
    System_Delay(10);
    local_1c = 0;
    local_1b = 0x17;
    iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
    if (iVar1 == 0) {
      System_Delay(10);
      local_1b = 0x17;
      local_1c = 2;
      iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
      if (iVar1 == 0) {
        System_Delay(10);
        local_1c = 4;
        local_1b = 0;
        iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
        if (iVar1 == 0) {
          System_Delay(10);
          local_1c = 6;
          local_1b = 0;
          iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
          if (iVar1 == 0) {
            System_Delay(10);
            local_1c = 8;
            local_1b = 0x12;
            iVar1 = I2CHandle_TransmitBlocking(param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa)
            ;
            if (iVar1 == 0) {
              System_Delay(10);
              local_1b = 0;
              local_1c = 10;
              iVar1 = I2CHandle_TransmitBlocking
                                (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
              if (iVar1 == 0) {
                System_Delay(10);
                local_1c = 0xc;
                if (*(char *)(param_1 + 1) == '\0') {
                  local_1b = 0x42;
                }
                else {
                  local_1b = 0x62;
                }
                iVar1 = I2CHandle_TransmitBlocking
                                  (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                if (iVar1 == 0) {
                  System_Delay(10);
                  local_1b = (byte)param_1[2] | (byte)param_1[3];
                  if (*(char *)(param_1 + 1) == '\0') {
                    local_1b = local_1b | 0x40;
                  }
                  if (*(char *)((int)param_1 + 5) != '\0') {
                    local_1b = local_1b | 0x20;
                  }
                  local_1c = 0xe;
                  iVar1 = I2CHandle_TransmitBlocking
                                    (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                  if (iVar1 == 0) {
                    System_Delay(10);
                    local_1c = 0x10;
                    local_1b = 0;
                    iVar1 = I2CHandle_TransmitBlocking
                                      (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                    if (iVar1 == 0) {
                      System_Delay(10);
                      local_1c = 0x12;
                      local_1b = 0;
                      iVar1 = I2CHandle_TransmitBlocking
                                        (param_1,*(undefined *)(param_1 + 4),&local_1c,2,0xfa);
                      if (iVar1 == 0) {
                        System_Delay(10);
                        iVar1 = Wm8731_WriteRegister(param_1,9,1);
                        if (iVar1 == 0) {
                          return 0;
                        }
                        return 1;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 1;
}



/* === 08006168 SdramHandle_ConfigureController === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int SdramHandle_ConfigureController(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  undefined4 uStack_10;
  undefined4 local_c;
  
  uVar1 = DAT_080061cc;
  iVar2 = DAT_080061c8;
  *(undefined4 *)(DAT_080061c8 + 0x20) = 0;
  *(undefined4 *)(iVar2 + 0x2c) = 0;
  *(undefined4 *)(iVar2 + 0xc) = 1;
  *(undefined4 *)(iVar2 + 0x10) = 8;
  *(undefined4 *)(iVar2 + 4) = uVar1;
  *(undefined4 *)(iVar2 + 8) = 0;
  *(undefined4 *)(iVar2 + 0x14) = 0x20;
  *(undefined4 *)(iVar2 + 0x18) = 0x40;
  local_24 = 2;
  uStack_20 = 7;
  *(undefined4 *)(iVar2 + 0x1c) = 0x180;
  local_1c = 4;
  uStack_18 = 8;
  *(undefined4 *)(iVar2 + 0x24) = 0x800;
  local_c = 10;
  *(undefined4 *)(iVar2 + 0x28) = 0x1000;
  local_14 = 3;
  uStack_10 = 0x10;
  iVar2 = FUN_0801229c(iVar2 + 4,&local_24);
  if (iVar2 != 0) {
    iVar2 = 1;
  }
  return iVar2;
}



/* === 080061d0 SdramHandle_InitSequence === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SdramHandle_InitSequence(void)

{
  undefined4 uVar1;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  uVar1 = DAT_0800625c;
  local_28 = 1;
  local_24 = 0x10;
  local_20 = 1;
  local_1c = 0;
  FUN_080122f0(DAT_0800625c,&local_28,0x1000);
  FUN_08009cf4(100);
  local_28 = 2;
  local_1c = 0;
  local_24 = 0x10;
  local_20 = 1;
  FUN_080122f0(uVar1,&local_28,0x1000);
  local_28 = 3;
  local_24 = 0x10;
  local_1c = 0;
  local_20 = 4;
  FUN_080122f0(uVar1,&local_28,0x1000);
  local_28 = 4;
  local_1c = 0x232;
  local_24 = 0x10;
  local_20 = 1;
  FUN_080122f0(uVar1,&local_28,0x1000);
  FUN_08012330(uVar1,0x806);
  return 0;
}



/* === 08006260 SdramHandle_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int SdramHandle_Init(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = SdramHandle_ConfigureController();
  if (iVar1 != 0) {
    return 1;
  }
  iVar1 = SdramHandle_InitSequence(param_1);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}



/* === 0800627c FUN_0800627c === */

void FUN_0800627c(void)

{
  int iVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  undefined4 local_2c;
  undefined4 local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  
  iVar1 = DAT_080063f8;
  iVar4 = *DAT_080063f4;
  local_24 = 0;
  if (iVar4 == 0) {
    *DAT_080063f4 = 1;
    uVar2 = DAT_080063fc;
    *(uint *)(iVar1 + 0xd4) = *(uint *)(iVar1 + 0xd4) | 0x1000;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x10;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x40;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 8;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x100;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x80;
    *(uint *)(iVar1 + 0xe0) = *(uint *)(iVar1 + 0xe0) | 0x20;
    uVar3 = *(uint *)(iVar1 + 0xe0) | 4;
    *(uint *)(iVar1 + 0xe0) = uVar3;
    local_2c = 0xff83;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    FUN_0800c080(uVar2,&local_2c,uVar3,*(uint *)(iVar1 + 0xe0) & 4);
    local_2c = 0x8137;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006400,&local_2c);
    local_2c = 0xc703;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006404,&local_2c);
    local_2c = 0x6ff;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006408,&local_2c);
    local_2c = 0xff0c;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_0800640c,&local_2c);
    local_2c = 0xf83f;
    local_28 = 2;
    local_20 = 3;
    local_1c = 0xc;
    local_24 = iVar4;
    FUN_0800c080(DAT_08006410,&local_2c);
    local_28 = 2;
    local_1c = 0xc;
    local_2c = 0x20;
    local_20 = 3;
    local_24 = iVar4;
    FUN_0800c080(DAT_0800640c,&local_2c);
  }
  return;
}



/* === 08006414 AudioHandle_Impl_InternalCallback === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AudioHandle_Impl_InternalCallback(int param_1,int param_2,uint param_3)

{
  short sVar1;
  float fVar2;
  undefined4 uVar3;
  code **ppcVar4;
  undefined4 uVar5;
  float fVar6;
  int *piVar7;
  float **ppfVar8;
  int iVar9;
  float **ppfVar10;
  int iVar11;
  float **ppfVar12;
  int iVar13;
  int iVar14;
  uint uVar15;
  uint uVar16;
  int iVar17;
  uint uVar18;
  float *pfVar19;
  uint uVar20;
  code *pcVar21;
  float **ppfVar22;
  float **ppfVar23;
  float **ppfVar24;
  int iVar25;
  undefined4 uVar26;
  code *pcVar27;
  float fVar28;
  float fVar29;
  undefined auStack_58 [4];
  int iStack_54;
  float *apfStack_50 [2];
  float *pfStack_48;
  int local_44;
  float **local_40;
  float **local_3c;
  code *local_38;
  float **local_34;
  
  ppcVar4 = DAT_08006818;
  iVar9 = SaiHandle_GetConfig(DAT_08006818 + 6);
  if (ppcVar4[6] == (code *)0x0) {
    if (ppcVar4[7] == (code *)0x0) {
      return;
    }
    pcVar21 = ppcVar4[1];
    uVar20 = 2;
    local_34 = *(float ***)(iVar9 + 0x14);
  }
  else {
    pcVar21 = ppcVar4[1];
    local_34 = *(float ***)(iVar9 + 0x14);
    if (ppcVar4[7] == (code *)0x0) {
      uVar20 = 2;
    }
    else {
      uVar20 = 4;
    }
  }
  if (pcVar21 != (code *)0x0) {
    uVar20 = param_3 * 4 + 7 & 0xfffffff8;
    iVar9 = -uVar20;
    if (local_34 == (float **)0x1) {
      if (param_3 != 0) {
        pcVar27 = ppcVar4[0xc];
        uVar15 = 0;
        pfVar19 = (float *)((int)&pfStack_48 + iVar9);
        do {
          iVar17 = uVar15 * 4;
          uVar16 = *(uint *)(param_1 + 4 + uVar15 * 4);
          uVar15 = uVar15 + 2;
          *pfVar19 = (float)(longlong)(int)((*(uint *)(param_1 + iVar17) ^ 0x800000) - 0x800000) *
                     DAT_08006824 * (float)pcVar27;
          pfVar19[1] = (float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * DAT_08006824 *
                       (float)pcVar27;
          pfVar19 = pfVar19 + 2;
        } while (uVar15 < param_3);
        local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
        (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
        fVar6 = DAT_0800682c;
        fVar2 = DAT_08006828;
        uVar5 = DAT_08006820;
        uVar3 = DAT_08006814;
        pcVar21 = ppcVar4[0xd];
        uVar20 = 0;
        do {
          fVar29 = (float)pcVar21 * (float)*local_34;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          fVar29 = ((float *)local_34)[1];
          *(undefined4 *)(param_2 + uVar20 * 4) = uVar26;
          fVar29 = (float)pcVar21 * fVar29;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(param_2 + 4 + uVar20 * 4) = uVar26;
          uVar20 = uVar20 + 2;
          local_34 = (float **)((float *)local_34 + 2);
        } while (uVar20 < param_3);
        return;
      }
    }
    else if (local_34 == (float **)0x2) {
      if (param_3 != 0) {
        pcVar27 = ppcVar4[0xc];
        uVar15 = 0;
        pfVar19 = (float *)((int)&pfStack_48 + iVar9);
        do {
          iVar17 = uVar15 * 4;
          iVar13 = *(int *)(param_1 + 4 + uVar15 * 4);
          uVar15 = uVar15 + 2;
          *pfVar19 = (float)(longlong)*(int *)(param_1 + iVar17) * DAT_0800680c * (float)pcVar27;
          pfVar19[1] = (float)(longlong)iVar13 * DAT_0800680c * (float)pcVar27;
          pfVar19 = pfVar19 + 2;
        } while (uVar15 < param_3);
        local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
        (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
        fVar6 = DAT_0800682c;
        fVar2 = DAT_08006828;
        uVar5 = DAT_0800681c;
        uVar3 = DAT_08006810;
        pcVar21 = ppcVar4[0xd];
        uVar20 = 0;
        do {
          fVar29 = (float)pcVar21 * (float)*local_34;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          fVar29 = ((float *)local_34)[1];
          *(undefined4 *)(param_2 + uVar20 * 4) = uVar26;
          fVar29 = (float)pcVar21 * fVar29;
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(param_2 + 4 + uVar20 * 4) = uVar26;
          uVar20 = uVar20 + 2;
          local_34 = (float **)((float *)local_34 + 2);
        } while (uVar20 < param_3);
        return;
      }
    }
    else if ((local_34 == (float **)0x0) && (param_3 != 0)) {
      pcVar27 = ppcVar4[0xc];
      uVar15 = 0;
      pfVar19 = (float *)((int)&pfStack_48 + iVar9);
      do {
        iVar17 = uVar15 * 4;
        sVar1 = *(short *)(param_1 + 4 + uVar15 * 4);
        uVar15 = uVar15 + 2;
        *pfVar19 = (float)(longlong)(int)*(short *)(param_1 + iVar17) * DAT_08006800 *
                   (float)pcVar27;
        pfVar19[1] = (float)(longlong)(int)sVar1 * DAT_08006800 * (float)pcVar27;
        pfVar19 = pfVar19 + 2;
      } while (uVar15 < param_3);
      local_34 = (float **)((int)&pfStack_48 + uVar20 * -2);
      (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
      fVar29 = DAT_0800682c;
      fVar6 = DAT_08006828;
      fVar2 = DAT_08006808;
      iVar9 = DAT_08006804;
      pcVar21 = ppcVar4[0xd];
      uVar20 = 0;
      do {
        fVar28 = (float)pcVar21 * (float)*local_34;
        iVar17 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar17 = 0x7ffe;
          }
          else {
            iVar17 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        fVar28 = ((float *)local_34)[1];
        *(int *)(param_2 + uVar20 * 4) = iVar17;
        fVar28 = (float)pcVar21 * fVar28;
        iVar17 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar17 = 0x7ffe;
          }
          else {
            iVar17 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(param_2 + 4 + uVar20 * 4) = iVar17;
        uVar20 = uVar20 + 2;
        local_34 = (float **)((float *)local_34 + 2);
      } while (uVar20 < param_3);
      return;
    }
    (*pcVar21)((int)&pfStack_48 + iVar9,(int)&pfStack_48 + uVar20 * -2,param_3);
    return;
  }
  local_38 = *ppcVar4;
  if (local_38 == (code *)0x0) {
    return;
  }
  iVar9 = FUN_08009384(DAT_080067fc);
  ppfVar8 = local_34;
  fVar29 = DAT_08006bdc;
  fVar6 = DAT_08006bcc;
  fVar2 = DAT_08006824;
  if (uVar20 == 2) {
    uVar15 = param_3 * 4 + 7 & 0xfffffff8;
    iVar17 = -uVar15;
    ppfVar24 = (float **)((int)&pfStack_48 + iVar17);
    ppfVar10 = (float **)((int)apfStack_50 + uVar15 * -2);
    ppfVar22 = (float **)((int)&pfStack_48 + (param_3 >> 1) * 4 + iVar17);
    ppfVar23 = (float **)(auStack_58 + uVar15 * -2);
    *(int *)((int)apfStack_50 + uVar15 * -2) = (int)&pfStack_48 + iVar17;
    *(float ***)((int)apfStack_50 + uVar15 * -2 + 4) = ppfVar22;
    *(uint *)(auStack_58 + uVar15 * -2) = (int)&pfStack_48 + uVar15 * -2;
    *(uint *)((int)&iStack_54 + uVar15 * -2) = (int)&pfStack_48 + (param_3 >> 1) * 4 + uVar15 * -2;
  }
  else {
    ppfVar24 = &pfStack_48 + param_3 * -2;
    uVar15 = (param_3 << 1) / uVar20;
    local_3c = ppfVar24 + param_3 * -2;
    ppfVar22 = ppfVar24 + uVar15;
    local_34 = local_3c + uVar15;
    ppfVar10 = local_3c + -uVar20;
    ppfVar10[2] = (float *)(ppfVar22 + uVar15);
    ppfVar10[3] = (float *)(ppfVar22 + uVar15 + uVar15);
    ppfVar12 = local_34 + uVar15;
    ppfVar23 = ppfVar10 + -uVar20;
    local_40 = ppfVar12 + uVar15;
    *ppfVar23 = (float *)local_3c;
    ppfVar23[1] = (float *)local_34;
    ppfVar23[2] = (float *)ppfVar12;
    ppfVar12 = local_40;
    *ppfVar10 = (float *)ppfVar24;
    ppfVar10[1] = (float *)ppfVar22;
    ppfVar23[3] = (float *)ppfVar12;
  }
  if (ppfVar8 == (float **)0x1) {
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      uVar15 = 0;
      local_40 = ppfVar23;
      local_3c = (float **)(iVar9 << 2);
      local_34 = (float **)param_2;
      do {
        uVar18 = uVar15 >> 1;
        pcVar27 = ppcVar4[0xc];
        uVar16 = *(uint *)(param_1 + 4 + uVar15 * 4);
        ppfVar24[uVar18] =
             (float *)((float)(longlong)
                              (int)((*(uint *)(param_1 + uVar15 * 4) ^ 0x800000) - 0x800000) * fVar2
                      * (float)pcVar27);
        ppfVar22[uVar18] =
             (float *)((float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * fVar2 *
                      (float)pcVar27);
        iVar17 = (int)local_34;
        piVar7 = (int *)local_40;
        if (uVar20 != 2) {
          uVar16 = *(uint *)(pcVar21 + uVar15 * 4 + iVar9 * 4 + 4);
          ppfVar10[2][uVar18] =
               (float)(longlong)
                      (int)((*(uint *)(pcVar21 + uVar15 * 4 + iVar9 * 4) ^ 0x800000) - 0x800000) *
               fVar2 * (float)pcVar27;
          ppfVar10[3][uVar18] =
               (float)(longlong)(int)((uVar16 ^ 0x800000) - 0x800000) * fVar2 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_40,param_3 >> 1);
      uVar5 = DAT_08006bd8;
      uVar3 = DAT_08006bc8;
      fVar6 = DAT_0800682c;
      fVar2 = DAT_08006828;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar9 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar25 = (uVar15 >> 1) * 4;
        fVar29 = (float)pcVar27 * *(float *)(iVar9 + (uVar15 >> 1) * 4);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
        }
        *(undefined4 *)(iVar17 + uVar15 * 4) = uVar26;
        fVar29 = (float)pcVar27 * *(float *)(iVar13 + iVar25);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
        }
        *(undefined4 *)(iVar17 + 4 + uVar15 * 4) = uVar26;
        if (uVar20 != 2) {
          fVar29 = (float)pcVar27 * *(float *)(piVar7[2] + iVar25);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c) = uVar26;
          fVar29 = (float)pcVar27 * *(float *)(iVar25 + piVar7[3]);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x17,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c + 4) = uVar26;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  else if (ppfVar8 == (float **)0x2) {
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      uVar15 = 0;
      local_40 = ppfVar23;
      local_3c = (float **)(iVar9 << 2);
      local_34 = (float **)param_2;
      do {
        pcVar27 = ppcVar4[0xc];
        iVar17 = *(int *)(param_1 + 4 + uVar15 * 4);
        uVar16 = uVar15 >> 1;
        ppfVar24[uVar16] =
             (float *)((float)(longlong)*(int *)(param_1 + uVar15 * 4) * fVar29 * (float)pcVar27);
        ppfVar22[uVar16] = (float *)((float)(longlong)iVar17 * fVar29 * (float)pcVar27);
        iVar17 = (int)local_34;
        piVar7 = (int *)local_40;
        if (uVar20 != 2) {
          iVar13 = *(int *)(pcVar21 + uVar15 * 4 + iVar9 * 4 + 4);
          ppfVar10[2][uVar16] =
               (float)(longlong)*(int *)(pcVar21 + uVar15 * 4 + iVar9 * 4) * fVar29 * (float)pcVar27
          ;
          ppfVar10[3][uVar16] = (float)(longlong)iVar13 * fVar29 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_40,param_3 >> 1);
      uVar5 = DAT_08006eb8;
      uVar3 = DAT_08006eb4;
      fVar6 = DAT_08006be4;
      fVar2 = DAT_08006be0;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar9 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar25 = (uVar15 >> 1) * 4;
        fVar29 = (float)pcVar27 * *(float *)(iVar9 + (uVar15 >> 1) * 4);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
        }
        *(undefined4 *)(iVar17 + uVar15 * 4) = uVar26;
        fVar29 = (float)pcVar27 * *(float *)(iVar13 + iVar25);
        uVar26 = uVar5;
        if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
          uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
        }
        *(undefined4 *)(iVar17 + 4 + uVar15 * 4) = uVar26;
        if (uVar20 != 2) {
          fVar29 = (float)pcVar27 * *(float *)(piVar7[2] + iVar25);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c) = uVar26;
          fVar29 = (float)pcVar27 * *(float *)(iVar25 + piVar7[3]);
          uVar26 = uVar5;
          if ((fVar2 < fVar29) && (uVar26 = uVar3, fVar29 < fVar6 != (NAN(fVar29) || NAN(fVar6)))) {
            uVar26 = FPToFixed(fVar29,0x20,0x20,0x1f,0,3);
          }
          *(undefined4 *)(pcVar21 + uVar15 * 4 + (int)local_3c + 4) = uVar26;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  else {
    if (ppfVar8 != (float **)0x0) {
      (*local_38)(ppfVar10,ppfVar23,param_3 >> 1);
      return;
    }
    if (param_3 != 0) {
      pcVar21 = ppcVar4[9];
      local_40 = (float **)(iVar9 << 2);
      local_44 = param_2;
      local_34 = (float **)(pcVar21 + iVar9 * 4 + 4);
      local_3c = ppfVar23;
      uVar15 = 0;
      do {
        pcVar27 = ppcVar4[0xc];
        sVar1 = *(short *)(param_1 + 4 + uVar15 * 4);
        uVar16 = uVar15 >> 1;
        ppfVar24[uVar16] =
             (float *)((float)(longlong)(int)*(short *)(param_1 + uVar15 * 4) * fVar6 *
                      (float)pcVar27);
        ppfVar22[uVar16] = (float *)((float)(longlong)(int)sVar1 * fVar6 * (float)pcVar27);
        piVar7 = (int *)local_3c;
        iVar17 = local_44;
        if (uVar20 != 2) {
          sVar1 = *(short *)((int)local_34 + uVar15 * 4);
          pfVar19 = ppfVar10[3];
          ppfVar10[2][uVar16] =
               (float)(longlong)(int)*(short *)(pcVar21 + uVar15 * 4 + iVar9 * 4) * fVar6 *
               (float)pcVar27;
          pfVar19[uVar16] = (float)(longlong)(int)sVar1 * fVar6 * (float)ppcVar4[0xc];
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      (*local_38)(ppfVar10,local_3c,param_3 >> 1);
      fVar29 = DAT_08006be4;
      fVar6 = DAT_08006be0;
      iVar9 = DAT_08006bd4;
      fVar2 = DAT_08006bd0;
      pcVar21 = ppcVar4[0xb];
      pcVar27 = ppcVar4[0xd];
      uVar15 = 0;
      iVar25 = *piVar7;
      iVar13 = piVar7[1];
      do {
        iVar14 = (uVar15 >> 1) * 4;
        fVar28 = (float)pcVar27 * *(float *)(iVar25 + (uVar15 >> 1) * 4);
        iVar11 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar11 = 0x7ffe;
          }
          else {
            iVar11 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(iVar17 + uVar15 * 4) = iVar11;
        fVar28 = (float)pcVar27 * *(float *)(iVar13 + iVar14);
        iVar11 = iVar9;
        if (fVar6 < fVar28) {
          if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
            iVar11 = 0x7ffe;
          }
          else {
            iVar11 = (int)(short)(int)(fVar28 * fVar2);
          }
        }
        *(int *)(iVar17 + 4 + uVar15 * 4) = iVar11;
        if (uVar20 != 2) {
          fVar28 = (float)pcVar27 * *(float *)(piVar7[2] + iVar14);
          iVar11 = iVar9;
          if (fVar6 < fVar28) {
            if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
              iVar11 = 0x7ffe;
            }
            else {
              iVar11 = (int)(short)(int)(fVar28 * fVar2);
            }
          }
          *(int *)(pcVar21 + uVar15 * 4 + (int)local_40) = iVar11;
          fVar28 = (float)pcVar27 * *(float *)(iVar14 + piVar7[3]);
          iVar11 = iVar9;
          if (fVar6 < fVar28) {
            if (fVar28 < fVar29 == (NAN(fVar28) || NAN(fVar29))) {
              iVar11 = 0x7ffe;
            }
            else {
              iVar11 = (int)(short)(int)(fVar28 * fVar2);
            }
          }
          *(int *)(pcVar21 + uVar15 * 4 + (int)local_40 + 4) = iVar11;
        }
        uVar15 = uVar15 + 2;
      } while (uVar15 < param_3);
      return;
    }
  }
  (*local_38)(ppfVar10,ppfVar23,param_3);
  return;
}



/* === 08006ebc AudioHandle_Init_single_SAI === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 AudioHandle_Init_single_SAI(int *param_1,undefined4 *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  float fVar6;
  
  iVar1 = DAT_08006f20;
  *param_1 = DAT_08006f20;
  uVar3 = param_2[1];
  uVar4 = param_2[2];
  uVar5 = param_2[3];
  *(undefined4 *)(iVar1 + 8) = *param_2;
  *(undefined4 *)(iVar1 + 0xc) = uVar3;
  *(undefined4 *)(iVar1 + 0x10) = uVar4;
  *(undefined4 *)(iVar1 + 0x14) = uVar5;
  fVar6 = *(float *)(iVar1 + 0x10);
  if (fVar6 != 0.0 && fVar6 < 0.0 == NAN(fVar6)) {
    *(float *)(iVar1 + 0x34) = *(float *)(iVar1 + 0x14) * fVar6;
    *(float *)(iVar1 + 0x30) = 1.0 / fVar6;
    if (param_3 != 0) {
      *(int *)(iVar1 + 0x18) = param_3;
      iVar2 = SaiHandle_GetConfig((int *)(iVar1 + 0x18));
      uVar3 = DAT_08006f28;
      uVar4 = *(undefined4 *)(iVar2 + 0x10);
      *(undefined4 *)(iVar1 + 0x20) = DAT_08006f24;
      *(undefined4 *)(iVar1 + 0x28) = uVar3;
      *(undefined4 *)(iVar1 + 0xc) = uVar4;
      return 0;
    }
  }
  return 1;
}



/* === 08006f2c AudioHandle_GetConfig === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int AudioHandle_GetConfig(int *param_1)

{
  return *param_1 + 8;
}



/* === 08006f34 AudioHandle_SetBlockSize === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

bool AudioHandle_SetBlockSize(int *param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = param_2;
  if (0xff < param_2) {
    uVar1 = 0x100;
  }
  *(uint *)(*param_1 + 8) = uVar1;
  return 0x100 < param_2;
}



/* === 08006f50 AudioHandle_GetSampleRate === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 AudioHandle_GetSampleRate(int *param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(*(int *)(*param_1 + 0x18) + 0x10);
  if (uVar1 < 5) {
    return *(undefined4 *)(DAT_0800937c + uVar1 * 4);
  }
  return DAT_08009380;
}



/* === 08006f58 AudioHandle_Start_interleaved === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AudioHandle_Start_interleaved(undefined4 *param_1,undefined4 param_2)

{
  param_1 = (undefined4 *)*param_1;
  SaiHandle_StartDma(param_1 + 6,param_1[8],param_1[10],param_1[2] << 2,DAT_08006f80);
  *param_1 = 0;
  param_1[1] = param_2;
  return;
}



/* === 08006fa8 AnalogControl_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_Init(float param_1,float param_2,undefined4 *param_3,undefined4 param_4,
                       undefined param_5,undefined param_6)

{
  float fVar1;
  float fVar2;
  
  fVar1 = DAT_08007008;
  *param_3 = param_4;
  param_3[3] = fVar1;
  param_3[2] = param_1;
  fVar2 = 1.0 / (param_1 * param_2 * 0.5);
  if (fVar2 == 1.0 || fVar2 < 1.0 != NAN(fVar2)) {
    if ((int)((uint)(fVar2 < fVar1) << 0x1f) < 0) {
      fVar2 = fVar1;
    }
  }
  else {
    fVar2 = 1.0;
  }
  *(undefined *)(param_3 + 6) = param_5;
  param_3[1] = fVar2;
  param_3[5] = 0;
  param_3[4] = 0x3f800000;
  *(undefined *)((int)param_3 + 0x19) = param_6;
  *(undefined *)((int)param_3 + 0x1a) = 0;
  param_3[7] = param_2;
  return;
}



/* === 0800700c AnalogControl_InitBipolarCv === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_InitBipolarCv(float param_1,undefined4 *param_2,undefined4 param_3)

{
  float fVar1;
  float fVar2;
  
  fVar1 = DAT_08007074;
  fVar2 = param_1 * DAT_08007070;
  *param_2 = param_3;
  param_2[3] = fVar1;
  param_2[2] = param_1;
  fVar2 = 1.0 / (fVar2 * 0.5);
  if (fVar2 == 1.0 || fVar2 < 1.0 != NAN(fVar2)) {
    if ((int)((uint)(fVar2 < fVar1) << 0x1f) < 0) {
      fVar2 = fVar1;
    }
  }
  else {
    fVar2 = 1.0;
  }
  param_2[1] = fVar2;
  param_2[4] = 0x40000000;
  param_2[5] = 0x3f000000;
  *(undefined2 *)(param_2 + 6) = 0x100;
  *(undefined *)((int)param_2 + 0x1a) = 1;
  return;
}



/* === 08007078 AnalogControl_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void AnalogControl_Process(ushort **param_1)

{
  float fVar1;
  
  fVar1 = (float)FixedToFP((uint)**param_1,0x20,0x20,0x10,1,0);
  if (*(char *)(param_1 + 6) != '\0') {
    fVar1 = 1.0 - fVar1;
  }
  fVar1 = (fVar1 - (float)param_1[5]) * (float)param_1[4];
  if (*(char *)((int)param_1 + 0x19) != '\0') {
    fVar1 = -fVar1;
  }
  param_1[3] = (ushort *)((float)param_1[3] + (float)param_1[1] * (fVar1 - (float)param_1[3]));
  return;
}



/* === 080070c0 GateIn_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GateIn_Init(int param_1,undefined2 param_2,undefined param_3)

{
  GPIO_Init(param_1,param_2,0,0,0);
  *(undefined2 *)(param_1 + 0x14) = 0;
  *(undefined *)(param_1 + 0x16) = param_3;
  return;
}



/* === 080070e4 GateIn_Trig === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

uint GateIn_Trig(int param_1)

{
  uint uVar1;
  
  *(undefined *)(param_1 + 0x14) = *(undefined *)(param_1 + 0x15);
  if (*(char *)(param_1 + 0x16) == '\0') {
    uVar1 = GPIO_Read();
  }
  else {
    uVar1 = GPIO_Read();
    uVar1 = (uVar1 ^ 1) & 0xff;
  }
  *(char *)(param_1 + 0x15) = (char)uVar1;
  if (uVar1 != 0) {
    uVar1 = *(byte *)(param_1 + 0x14) ^ 1;
  }
  return uVar1;
}



/* === 0800710c Switch_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Switch_Init(undefined4 *param_1,undefined2 param_2)

{
  undefined4 uVar1;
  
  uVar1 = System_GetNow();
  *param_1 = uVar1;
  *(undefined2 *)(param_1 + 1) = 0x100;
  *(undefined2 *)(param_1 + 7) = 0x100;
  GPIO_Init(param_1 + 2,param_2,0,1,0);
  return;
}



/* === 0800713c Switch_Debounce === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Switch_Debounce(int *param_1)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = System_GetNow();
  *(undefined *)(param_1 + 1) = 0;
  if (*param_1 != iVar2) {
    *param_1 = iVar2;
    *(undefined *)(param_1 + 1) = 1;
    bVar1 = GPIO_Read(param_1 + 2);
    if (*(char *)((int)param_1 + 0x1d) != '\0') {
      bVar1 = bVar1 ^ 1;
    }
    bVar1 = *(char *)(param_1 + 7) << 1 | bVar1;
    *(byte *)(param_1 + 7) = bVar1;
    if (bVar1 == 0x7f) {
      uVar3 = System_GetNow();
      param_1[8] = (int)(float)(ulonglong)uVar3;
      return;
    }
  }
  return;
}



/* === 08007190 FUN_08007190 === */

void FUN_08007190(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  
  uVar3 = DAT_080071f0;
  iVar2 = DAT_080071ec;
  cVar1 = *(char *)(DAT_080071ec + 0x668);
  *(undefined4 *)(DAT_080071ec + 0x600) = 0x400;
  *(undefined4 *)(iVar2 + 0x5f0) = uVar3;
  *(undefined4 *)(iVar2 + 0x604) = 0x800;
  if (cVar1 == '\0') {
    uVar3 = 0x100;
  }
  else {
    uVar3 = 0;
  }
  *(undefined4 *)(iVar2 + 0x5f4) = 9;
  *(undefined4 *)(iVar2 + 0x5f8) = 0;
  *(undefined4 *)(iVar2 + 0x5fc) = 0;
  *(undefined4 *)(iVar2 + 0x610) = 0;
  *(undefined4 *)(iVar2 + 0x608) = 0x2000;
  *(undefined4 *)(iVar2 + 0x60c) = uVar3;
  *(undefined4 *)(iVar2 + 0x614) = 0;
  iVar2 = FUN_0800af40(iVar2 + 0x5f0);
  if (iVar2 == 0) {
    return;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 080071f4 FUN_080071f4 === */

void FUN_080071f4(undefined2 *param_1,undefined2 param_2,undefined param_3)

{
  *param_1 = param_2;
  *(undefined *)(param_1 + 0x28) = 0;
  *(undefined *)((int)param_1 + 0x51) = param_3;
  *(undefined4 *)(param_1 + 2) = 3;
  *(undefined4 *)(param_1 + 4) = 0;
  return;
}



/* === 08007214 FUN_08007214 === */

void FUN_08007214(char *param_1,int param_2,uint param_3,char param_4)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  undefined uVar5;
  int iVar6;
  int iVar7;
  undefined4 *puVar8;
  uint uVar9;
  undefined2 *puVar10;
  char *pcVar11;
  int iVar12;
  undefined4 local_74;
  int local_70;
  undefined4 uStack_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 uStack_54;
  undefined4 local_50;
  undefined4 uStack_4c;
  undefined4 local_48;
  undefined4 uStack_44;
  undefined4 uStack_40;
  undefined4 uStack_3c;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 uStack_30;
  undefined4 uStack_2c;
  
  iVar3 = DAT_08007524;
  puVar10 = DAT_08007520;
  iVar7 = 0;
  *(uint *)(param_1 + 4) = param_3;
  *param_1 = param_4;
  *(undefined2 **)(iVar3 + 0x588) = puVar10 + 0x20;
  *(undefined2 **)(iVar3 + 0x584) = puVar10;
  local_70 = 0;
  uStack_6c = 0;
  local_68 = 0;
  local_64 = 0;
  local_60 = 0;
  local_5c = 0;
  local_58 = 0;
  uStack_54 = 0;
  local_50 = 0;
  uStack_4c = 0;
  memset(puVar10,0,0x20);
  *(undefined4 *)(iVar3 + 0x551) = 0;
  *(undefined4 *)(iVar3 + 0x555) = 0;
  *(undefined4 *)(iVar3 + 0x559) = 0;
  *(undefined4 *)(iVar3 + 0x55d) = 0;
  memset(iVar3 + 0x562,0,0x20);
  *(char *)(iVar3 + 0x550) = (char)param_3;
  iVar6 = *(int *)(param_1 + 4);
  *(undefined *)(iVar3 + 0x668) = 0;
  if (iVar6 == 0) {
    *(undefined4 *)(iVar3 + 0x594) = 0;
    *(undefined *)(iVar3 + 0x5a0) = 0;
    *(undefined4 *)(iVar3 + 0x5b0) = 0;
    *(undefined4 *)(iVar3 + 0x5b4) = 0;
    uVar4 = DAT_08007528;
    *(uint *)(iVar3 + 0x5a4) = param_3 & 0xff;
    *(undefined4 *)(iVar3 + 0x58c) = uVar4;
    *(undefined4 *)(iVar3 + 0x590) = 0x40000;
    *(undefined4 *)(iVar3 + 0x598) = 1;
    *(undefined4 *)(iVar3 + 0x59c) = 8;
  }
  else {
    pcVar11 = (char *)(iVar3 + 0x550);
    uVar9 = 0;
    do {
      uVar9 = uVar9 + 1;
      libc_memcpy(iVar3 + iVar7,param_2 + iVar7,0x52);
      *puVar10 = 0;
      cVar2 = *(char *)(param_2 + 0x50 + iVar7);
      iVar7 = iVar7 + 0x54;
      pcVar11 = pcVar11 + 1;
      *pcVar11 = cVar2;
      if (cVar2 != '\0') {
        *(undefined *)(iVar3 + 0x668) = 1;
      }
      puVar10 = puVar10 + 1;
    } while (uVar9 < *(uint *)(param_1 + 4));
    *(undefined4 *)(iVar3 + 0x58c) = DAT_08007528;
    *(uint *)(iVar3 + 0x5a4) = (uint)*(byte *)(iVar3 + 0x550);
    *(undefined4 *)(iVar3 + 0x590) = 0x40000;
    *(undefined4 *)(iVar3 + 0x59c) = 8;
    *(undefined4 *)(iVar3 + 0x594) = 0;
    *(undefined *)(iVar3 + 0x5a0) = 0;
    *(undefined4 *)(iVar3 + 0x5b0) = 0;
    *(undefined4 *)(iVar3 + 0x5b4) = 0;
    *(undefined4 *)(iVar3 + 0x598) = 1;
    if (*(char *)(iVar3 + 0x668) != '\0') {
      *(undefined4 *)(iVar3 + 0x5b8) = 1;
      *(undefined *)(iVar3 + 0x5a1) = 0;
      *(undefined *)(iVar3 + 0x5a8) = 0;
      goto LAB_08007304;
    }
  }
  *(undefined *)(iVar3 + 0x5a1) = 1;
  *(undefined *)(iVar3 + 0x5a8) = 0;
  *(undefined4 *)(iVar3 + 0x5b8) = 3;
LAB_08007304:
  cVar2 = *param_1;
  *(undefined4 *)(iVar3 + 0x5bc) = 0;
  *(undefined4 *)(iVar3 + 0x5c0) = 0;
  if (cVar2 == '\0') {
    *(undefined *)(iVar3 + 0x5c4) = 0;
  }
  else {
    *(undefined4 *)(iVar3 + 0x5d0) = 0;
    *(undefined *)(iVar3 + 0x5c4) = 1;
    *(undefined4 *)(iVar3 + 0x5d4) = 1;
    switch(cVar2) {
    case '\x01':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x40;
      *(undefined4 *)(iVar3 + 0x5c8) = 3;
      break;
    case '\x02':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x60;
      *(undefined4 *)(iVar3 + 0x5c8) = 7;
      break;
    case '\x03':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x80;
      *(undefined4 *)(iVar3 + 0x5c8) = 0xf;
      break;
    case '\x04':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xa0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x1f;
      break;
    case '\x05':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xc0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x3f;
      break;
    case '\x06':
      *(undefined4 *)(iVar3 + 0x5cc) = 0xe0;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x7f;
      break;
    case '\a':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x100;
      *(undefined4 *)(iVar3 + 0x5c8) = 0xff;
      break;
    case '\b':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x120;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x1ff;
      break;
    case '\t':
      *(undefined4 *)(iVar3 + 0x5cc) = 0x140;
      *(undefined4 *)(iVar3 + 0x5c8) = 0x3ff;
    }
  }
  local_70 = FUN_0800a5b4(DAT_0800752c);
  if (local_70 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  iVar6 = FUN_0800a874(DAT_0800752c,&local_70);
  uVar4 = DAT_0800752c;
  if (iVar6 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_58 = 0x7ff;
  uStack_54 = 4;
  if (*(char *)(iVar3 + 0x550) == '\0') {
    return;
  }
  local_74 = 6;
  uVar9 = 0;
  local_50 = 0;
  do {
    switch(*(undefined *)(uVar9 * 0x54 + iVar3 + 0x51)) {
    case 0:
      local_5c = 0;
      break;
    case 1:
      local_5c = 1;
      break;
    case 2:
      local_5c = 2;
      break;
    case 3:
      local_5c = 3;
      break;
    case 4:
      local_5c = 4;
      break;
    case 5:
      local_5c = 5;
      break;
    case 6:
      local_5c = 6;
      break;
    case 7:
      local_5c = 7;
    }
    iVar6 = (int)(short)uVar9;
    FUN_0800776c(iVar3 + iVar6 * 0x54);
    bVar1 = *(byte *)(uVar9 * 0x54 + iVar3 + 0x50);
    if (bVar1 < 5) {
      if (2 < bVar1) {
        uVar5 = 2;
        goto LAB_08007566;
      }
      if (bVar1 == 2) {
        uVar5 = 1;
        goto LAB_08007566;
      }
      *(undefined *)(iVar3 + uVar9 + 0x540) = 0;
    }
    else {
      uVar5 = 3;
LAB_08007566:
      iVar12 = 0;
      iVar7 = iVar3 + iVar6 * 0x54 + 0x14;
      *(undefined *)(iVar3 + uVar9 + 0x540) = uVar5;
      do {
        iVar12 = iVar12 + 1;
        FUN_0800776c(iVar7);
        iVar7 = iVar7 + 0x14;
      } while (iVar12 < (int)(uint)*(byte *)(iVar3 + uVar9 + 0x540));
    }
    puVar8 = &local_48;
    iVar7 = 0;
    local_48 = *DAT_08007530;
    uStack_44 = DAT_08007530[1];
    uStack_40 = DAT_08007530[2];
    uStack_3c = DAT_08007530[3];
    local_38 = DAT_08007530[4];
    uStack_34 = DAT_08007530[5];
    uStack_30 = DAT_08007530[6];
    uStack_2c = DAT_08007530[7];
    do {
      if ((*(char *)puVar8 == *(char *)(iVar3 + iVar6 * 0x54)) &&
         (*(char *)((int)puVar8 + 1) == *(char *)(uVar9 * 0x54 + iVar3 + 1))) {
        local_64 = *(undefined4 *)(DAT_08007534 + iVar7 * 4);
        goto LAB_0800745e;
      }
      iVar7 = iVar7 + 1;
      puVar8 = (undefined4 *)((int)puVar8 + 2);
    } while (iVar7 != 0x10);
    local_64 = 0;
LAB_0800745e:
    local_60 = local_74;
    iVar6 = FUN_08009dbc(uVar4,&local_64);
    if (iVar6 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    uVar9 = uVar9 + 1 & 0xff;
    if (*(byte *)(iVar3 + 0x550) <= uVar9) {
      return;
    }
    local_74 = *(undefined4 *)(DAT_08007538 + uVar9 * 4);
  } while( true );
}



/* === 080075a0 FUN_080075a0 === */

int FUN_080075a0(void)

{
  undefined uVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 in_r3;
  uint uVar6;
  undefined4 unaff_r4;
  undefined4 unaff_r5;
  int *piVar7;
  
  iVar4 = DAT_080075c8;
  piVar7 = (int *)(DAT_080075c8 + 0x58c);
  FUN_0800a7d0(piVar7,0x10001,0x7ff,in_r3,in_r3);
  uVar1 = *(undefined *)(iVar4 + 0x550);
  uVar5 = *(undefined4 *)(iVar4 + 0x584);
  puVar2 = (undefined4 *)*piVar7;
  if ((puVar2 == DAT_0800a3a0) || (puVar2 == DAT_0800a3a4)) {
    uVar6 = *(uint *)(DAT_0800a3ac + 8);
    iVar3 = puVar2[2];
  }
  else {
    uVar6 = *(uint *)(DAT_0800a3a8 + 8);
    iVar3 = puVar2[2];
  }
  if ((-1 < iVar3 << 0x1d) && (*(char *)(iVar4 + 0x5dc) != '\x01')) {
    uVar6 = uVar6 & 0x1f;
    *(undefined *)(iVar4 + 0x5dc) = 1;
    if ((uVar6 < 10) && ((~(0x221U >> uVar6) & 1) == 0)) {
      iVar3 = FUN_0800a1f4(piVar7);
      if (iVar3 != 0) {
        *(undefined *)(iVar4 + 0x5dc) = 0;
        return iVar3;
      }
      puVar2 = (undefined4 *)*piVar7;
      *(uint *)(iVar4 + 0x5e0) = DAT_0800a3b0 & *(uint *)(iVar4 + 0x5e0) | 0x100;
      if ((uVar6 == 0) || (puVar2 != DAT_0800a3a4)) {
        *(uint *)(iVar4 + 0x5e0) = *(uint *)(iVar4 + 0x5e0) & 0xffefffff;
      }
      if ((*(uint *)(iVar4 + 0x5e0) & 0x1000) == 0) {
        *(undefined4 *)(iVar4 + 0x5e4) = 0;
      }
      else {
        *(uint *)(iVar4 + 0x5e4) = *(uint *)(iVar4 + 0x5e4) & 0xfffffff9;
      }
      iVar3 = *(int *)(iVar4 + 0x5d8);
      uVar6 = *(uint *)(iVar4 + 0x5b8);
      *(undefined4 *)(iVar3 + 0x3c) = DAT_0800a3b4;
      *(undefined4 *)(iVar3 + 0x40) = DAT_0800a3b8;
      *(undefined4 *)(iVar3 + 0x4c) = DAT_0800a3bc;
      *puVar2 = 0x1c;
      *(undefined *)(iVar4 + 0x5dc) = 0;
      puVar2[1] = puVar2[1] | 0x10;
      puVar2[3] = puVar2[3] & 0xfffffffc | uVar6;
      iVar4 = FUN_0800b3a0(iVar3,puVar2 + 0x10,uVar5,uVar1,unaff_r4,unaff_r5);
      *(uint *)(*piVar7 + 8) = DAT_0800a3c0 & *(uint *)(*piVar7 + 8) | 4;
      return iVar4;
    }
    *(undefined *)(iVar4 + 0x5dc) = 0;
    return 1;
  }
  return 2;
}



/* === 080075cc FUN_080075cc === */

int FUN_080075cc(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  iVar1 = *(int *)(DAT_080075dc + 0x584);
  if (param_2 < 0x10) {
    iVar1 = iVar1 + param_2 * 2;
  }
  return iVar1;
}



/* === 080075e0 FUN_080075e0 === */

void FUN_080075e0(int *param_1)

{
  int iVar1;
  int iVar2;
  
  if (*param_1 != DAT_08007624) {
    return;
  }
  *(uint *)(DAT_08007628 + 0xd8) = *(uint *)(DAT_08007628 + 0xd8) | 0x20;
  FUN_08007190();
  iVar1 = DAT_0800762c;
  iVar2 = DAT_0800762c + 0x58c;
  *(int *)(DAT_0800762c + 0x5d8) = DAT_0800762c + 0x5f0;
  *(int *)(iVar1 + 0x628) = iVar2;
  return;
}



/* === 08007630 FUN_08007630 === */

void FUN_08007630(void)

{
  FUN_0800b9d4(DAT_08007638);
  return;
}



/* === 0800763c FUN_0800763c === */

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



/* === 08007714 FUN_08007714 === */

void FUN_08007714(int *param_1)

{
  if (*param_1 != DAT_08007724) {
    return;
  }
  software_bkpt(0xff);
  return;
}



/* === 0800776c FUN_0800776c === */

void FUN_0800776c(byte *param_1)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  
  uVar2 = (uint)*param_1;
  if ((uVar2 != 0xb) && (bVar1 = param_1[1], bVar1 < 0x10)) {
    local_18 = *(int *)(param_1 + 4);
    if (local_18 == 2) {
      local_18 = 0x11;
    }
    else if ((local_18 != 3) && (local_18 != 1)) {
      local_18 = 0;
    }
    local_14 = *(int *)(param_1 + 8);
    if ((local_14 != 1) && (local_14 != 2)) {
      local_14 = 0;
    }
    local_10 = *(int *)(param_1 + 0xc);
    if (((local_10 != 2) && (local_10 != 3)) && (local_10 != 1)) {
      local_10 = 0;
    }
    if (uVar2 < 0xb) {
      iVar3 = DAT_08007920 + uVar2 * 0x400;
    }
    else {
      iVar3 = 0;
    }
    *(int *)(param_1 + 0x10) = iVar3;
    switch(uVar2) {
    case 0:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 1;
      break;
    case 1:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 2;
      break;
    case 2:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 4;
      break;
    case 3:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 8;
      break;
    case 4:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x10;
      break;
    case 5:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x20;
      break;
    case 6:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x40;
      break;
    case 7:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x80;
      break;
    case 8:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x100;
      break;
    case 9:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x200;
      break;
    case 10:
      *(uint *)(DAT_08007924 + 0xe0) = *(uint *)(DAT_08007924 + 0xe0) | 0x400;
    }
    local_1c = 1 << (uint)bVar1;
    FUN_0800c080(iVar3,&local_1c);
    return;
  }
  return;
}



/* === 08007928 FUN_08007928 === */

void FUN_08007928(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  uVar1 = param_2[1];
  uVar2 = param_2[2];
  uVar3 = param_2[3];
  *param_1 = *param_2;
  param_1[1] = uVar1;
  param_1[2] = uVar2;
  param_1[3] = uVar3;
  FUN_0800776c(param_1);
  return;
}



/* === 08007938 GPIO_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GPIO_Init(undefined2 *param_1,undefined2 param_2,undefined4 param_3,undefined4 param_4,
              undefined4 param_5)

{
  *(undefined4 *)(param_1 + 4) = param_4;
  *(undefined4 *)(param_1 + 2) = param_3;
  *param_1 = param_2;
  *(undefined4 *)(param_1 + 6) = param_5;
  FUN_0800776c();
  return;
}



/* === 08007950 GPIO_Read === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int GPIO_Read(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  iVar1 = FUN_0800c484(*(undefined4 *)(param_1 + 0x10),1 << (uint)*(byte *)(param_1 + 1) & 0xffff,
                       param_3,(uint)*(byte *)(param_1 + 1),param_4);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}



/* === 08007968 GPIO_Write === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void GPIO_Write(int param_1,undefined4 param_2)

{
  FUN_0800c490(*(undefined4 *)(param_1 + 0x10),1 << *(sbyte *)(param_1 + 1) & 0xffff,param_2);
  return;
}



/* === 0800797c FUN_0800797c === */

void FUN_0800797c(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_080079c0;
  puVar3 = DAT_080079c0 + 0x12;
  *DAT_080079c4 = 0xff;
  do {
    puVar2 = puVar1 + 6;
    *puVar1 = 0x10;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 08007b1c FUN_08007b1c === */

int FUN_08007b1c(int *param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
                undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  
  piVar7 = param_1 + 0x23;
  uVar8 = param_4;
  do {
    iVar5 = FUN_0800e628(piVar7);
  } while (iVar5 != 0x20);
  iVar5 = *param_1;
  param_1[5] = DAT_08007bf4;
  if (iVar5 == 1) {
    param_1[6] = 0x24;
  }
  else if (iVar5 == 2) {
    param_1[6] = 0x4a;
  }
  else {
    if (iVar5 != 0) {
      return 1;
    }
    param_1[6] = 0x22;
  }
  param_1[7] = 0x40;
  param_1[8] = 0;
  param_1[0xe] = 0;
  param_1[9] = 0x400;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  iVar5 = FUN_0800af40(param_1 + 5);
  if (iVar5 == 0) {
    param_1[0x31] = (int)(param_1 + 5);
    param_1[0x13] = (int)piVar7;
    puVar4 = DAT_08007c00;
    puVar3 = DAT_08007bfc;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08007bfc = *(undefined *)param_1;
    *puVar4 = param_5;
    puVar2 = DAT_08007bf8;
    iVar6 = param_1[3];
    *DAT_08007bf8 = param_6;
    if (iVar6 == 0) {
      iVar6 = FUN_0800d3b4(piVar7,(param_2 & 0x7fff) << 1,param_3,param_4);
    }
    else {
      iVar6 = FUN_0800d678(piVar7,param_3,param_4,iVar6,uVar8);
    }
    if (iVar6 != 0) {
      iVar6 = 1;
      *puVar3 = 0xff;
      *puVar4 = 0;
      *puVar2 = 0;
    }
    if (iVar5 != 0) {
      return iVar6;
    }
    enableIRQinterrupts();
    return iVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08007c04 FUN_08007c04 === */

int FUN_08007c04(int *param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
                undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined4 uVar8;
  
  piVar7 = param_1 + 0x23;
  uVar8 = param_4;
  do {
    iVar5 = FUN_0800e628(piVar7);
  } while (iVar5 != 0x20);
  iVar5 = *param_1;
  param_1[5] = DAT_08007cd8;
  if (iVar5 == 1) {
    param_1[6] = 0x23;
  }
  else if (iVar5 == 2) {
    param_1[6] = 0x49;
  }
  else {
    if (iVar5 != 0) {
      return 1;
    }
    param_1[6] = 0x21;
  }
  param_1[0xe] = 0;
  param_1[9] = 0x400;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  iVar5 = FUN_0800af40(param_1 + 5);
  if (iVar5 == 0) {
    param_1[0x32] = (int)(param_1 + 5);
    param_1[0x13] = (int)piVar7;
    puVar4 = DAT_08007ce4;
    puVar3 = DAT_08007ce0;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08007ce0 = *(undefined *)param_1;
    *puVar4 = param_5;
    puVar2 = DAT_08007cdc;
    iVar6 = param_1[3];
    *DAT_08007cdc = param_6;
    if (iVar6 == 0) {
      iVar6 = FUN_0800d534(piVar7,(param_2 & 0x7fff) << 1,param_3,param_4);
    }
    else {
      iVar6 = FUN_0800d798(piVar7,param_3,param_4,iVar6,uVar8);
    }
    if (iVar6 != 0) {
      iVar6 = 1;
      *puVar3 = 0xff;
      *puVar4 = 0;
      *puVar2 = 0;
    }
    if (iVar5 != 0) {
      return iVar6;
    }
    enableIRQinterrupts();
    return iVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08007d8c FUN_08007d8c === */

void FUN_08007d8c(int *param_1)

{
  byte bVar1;
  int iVar2;
  int local_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = 0;
  local_18 = 0x12;
  uStack_14 = 0;
  iVar2 = *param_1;
  if (iVar2 < 3) {
    if (-1 < iVar2) {
      local_c = 4;
    }
  }
  else if (iVar2 == 3) {
    local_c = 6;
    bVar1 = *(byte *)(param_1 + 1);
    goto joined_r0x08007dee;
  }
  bVar1 = *(byte *)(param_1 + 1);
joined_r0x08007dee:
  if (bVar1 < 0xb) {
    iVar2 = DAT_08007df8 + (uint)bVar1 * 0x400;
  }
  else {
    iVar2 = 0;
  }
  local_1c = 1 << (uint)*(byte *)((int)param_1 + 5);
  FUN_0800c080(iVar2,&local_1c);
  if (*(byte *)((int)param_1 + 6) < 0xb) {
    iVar2 = DAT_08007df8 + (uint)*(byte *)((int)param_1 + 6) * 0x400;
  }
  else {
    iVar2 = 0;
  }
  local_1c = 1 << (uint)*(byte *)((int)param_1 + 7);
  FUN_0800c080(iVar2,&local_1c);
  return;
}



/* === 08007dfc FUN_08007dfc === */

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



/* === 08007fb0 thunk_FUN_0800797c === */

void thunk_FUN_0800797c(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_080079c0;
  puVar3 = DAT_080079c0 + 0x12;
  *DAT_080079c4 = 0xff;
  do {
    puVar2 = puVar1 + 6;
    *puVar1 = 0x10;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 08007fb4 FUN_08007fb4 === */

void FUN_08007fb4(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_08007fe0 << 0x18)) {
    FUN_0800b9d4(DAT_08007fe4 + (char)*DAT_08007fe0 * 0xe0 + 0x14);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08007fe8 FUN_08007fe8 === */

void FUN_08007fe8(void)

{
  if ((code *)DAT_08007ff0[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08007ff0[0xd])
              (DAT_08007ff0,((undefined4 *)*DAT_08007ff0)[6],*(undefined4 *)*DAT_08007ff0);
    return;
  }
  return;
}



/* === 08007ff4 FUN_08007ff4 === */

void FUN_08007ff4(void)

{
  if ((code *)DAT_08007ffc[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08007ffc[0xd])
              (DAT_08007ffc,((undefined4 *)*DAT_08007ffc)[6],*(undefined4 *)*DAT_08007ffc);
    return;
  }
  return;
}



/* === 08008000 FUN_08008000 === */

void FUN_08008000(void)

{
  if ((code *)DAT_08008008[0xd] != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800d87e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (*(code *)DAT_08008008[0xd])
              (DAT_08008008,((undefined4 *)*DAT_08008008)[6],*(undefined4 *)*DAT_08008008);
    return;
  }
  return;
}



/* === 0800800c FUN_0800800c === */

/* WARNING: Removing unreachable block (ram,0x08007d64) */

void FUN_0800800c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,0);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08008014 FUN_08008014 === */

/* WARNING: Removing unreachable block (ram,0x08007d64) */

void FUN_08008014(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,0);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 0800801c FUN_0800801c === */

/* WARNING: Removing unreachable block (ram,0x08007d64) */

void FUN_0800801c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,0);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08008024 FUN_08008024 === */

/* WARNING: Removing unreachable block (ram,0x08007d64) */

void FUN_08008024(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,0);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 0800802c FUN_0800802c === */

void FUN_0800802c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  undefined2 *puVar4;
  int iVar5;
  code *pcVar6;
  undefined2 *puVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  FUN_0800cf80();
  pbVar3 = DAT_08007d7c;
  ppcVar2 = DAT_08007d78;
  *DAT_08007d7c = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08007d80,1);
  }
  puVar4 = DAT_08007d88;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    puVar7 = DAT_08007d88;
    iVar9 = DAT_08007d84;
    do {
      iVar5 = *(int *)(puVar7 + 2);
      if (iVar5 != 0) {
        if (*(int *)(puVar7 + 10) == 0) {
          iVar5 = FUN_08007b1c(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        else {
          iVar5 = FUN_08007c04(iVar9,*puVar7,iVar5,puVar7[4],*(undefined4 *)(puVar7 + 6),
                               *(undefined4 *)(puVar7 + 8));
        }
        if (iVar5 == 0) {
          *(undefined4 *)(puVar4 + iVar8 * 0xc + 2) = 0;
          break;
        }
      }
      iVar8 = iVar8 + 1;
      puVar7 = puVar7 + 0xc;
      iVar9 = iVar9 + 0xe0;
    } while (iVar8 != 3);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08008034 I2CHandle_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 I2CHandle_Init(int **param_1,int *param_2)

{
  byte bVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int iVar7;
  int aiStack_20 [4];
  
  piVar3 = (int *)(DAT_08008048 + *param_2 * 0xe0);
  *param_1 = piVar3;
  piVar6 = DAT_08007ab0;
  iVar5 = *param_2;
  if (3 < iVar5) {
    return 1;
  }
  if (((uint)param_2[3] < 2) && ((param_2[3] != 1 || (*(byte *)(param_2 + 4) - 0x10 < 0x68)))) {
    iVar7 = *param_2;
    iVar2 = param_2[1];
    iVar4 = param_2[3];
    piVar3[2] = param_2[2];
    *piVar3 = iVar7;
    piVar3[1] = iVar2;
    piVar3[3] = iVar4;
    iVar2 = piVar3[2];
    *(undefined *)(piVar3 + 4) = *(undefined *)(param_2 + 4);
    aiStack_20[3] = piVar6[3];
    aiStack_20[2] = piVar6[2];
    aiStack_20[1] = piVar6[1];
    aiStack_20[0] = *piVar6;
    piVar3[0x23] = aiStack_20[iVar5];
    if (iVar2 == 1) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ab8;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007abc;
      }
      piVar3[0x24] = iVar5;
    }
    else if (iVar2 == 2) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ac8;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007acc;
      }
      piVar3[0x24] = iVar5;
    }
    else if (iVar2 == 0) {
      iVar2 = thunk_FUN_08010408();
      iVar5 = DAT_08007ac0;
      if (iVar2 != DAT_08007ab4) {
        iVar5 = DAT_08007ac4;
      }
      piVar3[0x24] = iVar5;
    }
    bVar1 = *(byte *)(param_2 + 4);
    piVar6 = piVar3 + 0x23;
    piVar3[0x28] = 0;
    piVar3[0x25] = (uint)bVar1 << 1;
    piVar3[0x2b] = 0;
    piVar3[0x26] = 1;
    piVar3[0x27] = 0;
    piVar3[0x29] = 0;
    piVar3[0x2a] = 0;
    iVar5 = FUN_0800cf80(piVar6);
    if ((iVar5 == 0) && (iVar5 = FUN_0800e630(piVar6,0), iVar5 == 0)) {
      iVar5 = FUN_0800e684(piVar6,0);
      if (iVar5 == 0) {
        return 0;
      }
      return 1;
    }
  }
  return 1;
}



/* === 0800804c FUN_0800804c === */

undefined4 FUN_0800804c(undefined4 *param_1)

{
  return *param_1;
}



/* === 08008050 I2CHandle_TransmitBlocking === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int I2CHandle_TransmitBlocking
              (int *param_1,uint param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = *param_1;
  iVar3 = iVar2 + 0x8c;
  do {
    iVar1 = FUN_0800e628(iVar3);
  } while (iVar1 != 0x20);
  if (*(int *)(iVar2 + 0xc) == 0) {
    iVar2 = FUN_0800d02c(iVar3,(param_2 & 0x7fff) << 1,param_3,param_4,param_5);
  }
  else {
    iVar2 = FUN_0800d20c(iVar3,param_3,param_4,param_5);
  }
  if (iVar2 != 0) {
    iVar2 = 1;
  }
  return iVar2;
}



/* === 08008058 I2CHandle_TransmitDma === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4
I2CHandle_TransmitDma
          (int **param_1,undefined2 param_2,undefined4 param_3,undefined2 param_4,undefined4 param_5
          ,undefined4 param_6)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = DAT_080080bc;
  iVar4 = **param_1;
  if (iVar4 == 3) {
    return 1;
  }
  if ((*DAT_080080c0 & 0x80) != 0) {
    uVar2 = FUN_08007b1c();
    return uVar2;
  }
  iVar3 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar3 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *(undefined2 *)(DAT_080080bc + iVar4 * 0x18) = param_2;
  iVar5 = iVar5 + iVar4 * 0x18;
  *(undefined4 *)(iVar5 + 4) = param_3;
  *(undefined2 *)(iVar5 + 8) = param_4;
  *(undefined4 *)(iVar5 + 0x14) = 0;
  *(undefined4 *)(iVar5 + 0xc) = param_5;
  *(undefined4 *)(iVar5 + 0x10) = param_6;
  if (iVar3 == 0) {
    enableIRQinterrupts();
  }
  return 0;
}



/* === 080080f0 FUN_080080f0 === */

undefined4 FUN_080080f0(int param_1)

{
  int iVar1;
  undefined4 local_60;
  undefined4 uStack_5c;
  undefined4 local_58;
  undefined4 uStack_54;
  int local_50;
  undefined4 local_4c;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_34 = 0;
  local_48[0] = 6;
  local_14 = 0;
  local_30 = 0x100;
  uStack_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = System_GetProgramMemoryRegion();
  if (iVar1 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  local_50 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (local_50 == 0) {
    local_60 = 2;
    uStack_5c = 2;
    local_58 = 0x10;
    uStack_54 = 1;
    local_4c = 0x400000;
    local_48[0] = 5;
    local_24 = 0x1000000;
    iVar1 = FUN_0800f5d8(param_1 + 0x10,local_48,&local_60,5000);
    if (iVar1 == 0) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 0800817c FUN_0800817c === */

int FUN_0800817c(int param_1)

{
  int iVar1;
  undefined4 local_50;
  undefined4 uStack_4c;
  undefined4 local_48 [2];
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 uStack_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 0xeb;
  local_2c = 0xc00;
  local_28 = 0xc000;
  local_34 = 6;
  local_24 = 0x3000000;
  local_30 = 0x100;
  local_3c = 0x2000;
  uStack_38 = 0;
  local_40 = 0xa0;
  local_14 = 0x10000000;
  local_1c = 0;
  uStack_18 = 0;
  local_50 = 0;
  uStack_4c = 0;
  iVar1 = FUN_0800f6b4(param_1 + 0x10,local_48,&local_50);
  if (iVar1 != 0) {
    iVar1 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return iVar1;
}



/* === 080081d8 FUN_080081d8 === */

int FUN_080081d8(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 5;
  local_28 = 0;
  local_24 = 0x1000000;
  local_34 = 0;
  local_58 = 0x10;
  local_14 = 0;
  local_60 = 0;
  local_50 = 0;
  local_4c = 0x400000;
  local_5c = 1;
  local_54 = 1;
  local_30 = 0x100;
  uStack_2c = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = FUN_0800f5d8(param_1 + 0x10,local_48,&local_60,param_2);
  if (iVar1 != 0) {
    iVar1 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return iVar1;
}



/* === 0800822c FUN_0800822c === */

undefined4 FUN_0800822c(int param_1)

{
  int iVar1;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  local_48[0] = 0x66;
  local_34 = 0;
  local_14 = 0;
  local_30 = 0x100;
  uStack_2c = 0;
  local_28 = 0;
  uStack_24 = 0;
  local_1c = 0;
  uStack_18 = 0;
  iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (iVar1 == 0) {
    local_48[0] = 0x99;
    iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
    if ((iVar1 == 0) && (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 08008290 FUN_08008290 === */

undefined4 FUN_08008290(int param_1,int param_2)

{
  int iVar1;
  ushort local_4a;
  undefined4 local_48 [5];
  undefined4 local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  
  local_4a = 0;
  local_34 = 0;
  local_28 = 0;
  uStack_24 = 0x1000000;
  local_30 = 0x100;
  uStack_2c = 0;
  local_20 = 1;
  uStack_1c = 0;
  local_18 = 0;
  uStack_14 = 0;
  if (param_2 == 0) {
    local_48[0] = 0x61;
    iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
    if ((iVar1 != 0) || (iVar1 = FUN_0800f4e0(param_1 + 0x10,&local_4a,5000), iVar1 != 0))
    goto LAB_080082d8;
    local_4a = local_4a & 0xff87 | 0x40;
    iVar1 = FUN_080080f0(param_1);
    if (iVar1 != 0) goto LAB_080082d8;
  }
  else {
    local_4a = 0xf0;
  }
  local_48[0] = 0xc0;
  iVar1 = FUN_0800f32c(param_1 + 0x10,local_48,5000);
  if (((iVar1 == 0) && (iVar1 = FUN_0800f3f0(param_1 + 0x10,&local_4a,5000), iVar1 == 0)) &&
     (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
    return 0;
  }
LAB_080082d8:
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 08008350 FUN_08008350 === */

undefined4 FUN_08008350(int param_1)

{
  int iVar1;
  int iVar2;
  undefined local_69;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  undefined4 local_54;
  undefined4 local_50 [5];
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 uStack_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  
  local_69 = 0;
  local_38 = 0x100;
  local_50[0] = 1;
  local_2c = 0x1000000;
  local_3c = 0;
  local_34 = 0;
  uStack_30 = 0;
  local_28 = 1;
  uStack_24 = 0;
  local_20 = 0;
  uStack_1c = 0;
  iVar1 = FUN_080080f0();
  if (iVar1 == 0) {
    iVar2 = param_1 + 0x10;
    iVar1 = FUN_0800f32c(iVar2,local_50,5000);
    if (iVar1 == 0) {
      local_69 = 0x40;
      local_58 = FUN_0800f3f0(iVar2,&local_69,5000);
      if (local_58 == 0) {
        local_5c = 1;
        local_60 = 0x8000;
        local_68 = 0x40;
        local_2c = 0x1000000;
        local_54 = 0x400000;
        local_50[0] = 5;
        local_64 = 0x43;
        iVar1 = FUN_0800f5d8(iVar2,local_50,&local_68,5000);
        if ((iVar1 == 0) && (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
          return 0;
        }
      }
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 08008400 FUN_08008400 === */

undefined4 FUN_08008400(int param_1)

{
  int iVar1;
  int iVar2;
  byte local_69;
  uint local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  int local_58;
  undefined4 local_54;
  undefined4 local_50 [5];
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 uStack_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  undefined4 uStack_1c;
  
  local_50[0] = 1;
  local_3c = 0;
  local_38 = 0x100;
  uStack_34 = 0;
  local_30 = 0;
  local_2c = 0x1000000;
  local_28 = 1;
  uStack_24 = 0;
  local_20 = 0;
  uStack_1c = 0;
  iVar1 = FUN_080080f0();
  if (iVar1 == 0) {
    iVar2 = param_1 + 0x10;
    iVar1 = FUN_0800f32c(iVar2,local_50,5000);
    if (iVar1 == 0) {
      local_69 = 0x40;
      local_58 = FUN_0800f3f0(iVar2,&local_69,5000);
      if (local_58 == 0) {
        local_5c = 1;
        local_60 = 0x8000;
        local_2c = 0x1000000;
        local_54 = 0x400000;
        local_50[0] = 5;
        local_68 = (uint)local_69;
        local_64 = 0xff;
        iVar1 = FUN_0800f5d8(iVar2,local_50,&local_68,5000);
        if ((iVar1 == 0) && (iVar1 = FUN_080081d8(param_1,5000), iVar1 == 0)) {
          return 0;
        }
      }
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 080084a8 FUN_080084a8 === */

undefined4 FUN_080084a8(int param_1,undefined4 param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 local_2c;
  undefined4 uStack_28;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  FUN_0800c490(DAT_080085b8,0xc0,1);
  iVar5 = DAT_080085bc;
  uVar8 = DAT_080085b8;
  local_1c = 0;
  bVar2 = (byte)param_2;
  bVar3 = (byte)((uint)param_2 >> 8);
  bVar4 = (byte)((uint)param_2 >> 0x10);
  bVar1 = (byte)((uint)param_2 >> 0x18);
  uVar7 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                          bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                       bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
          (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                          bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                       bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
          (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                          bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                       bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
          (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                          bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                       bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
  local_24 = 0;
  uStack_20 = 0;
  uVar6 = *(uint *)(DAT_080085bc + 0xe0) | 0x20;
  *(uint *)(DAT_080085bc + 0xe0) = uVar6;
  local_2c = 0xc0;
  uStack_28 = 1;
  FUN_0800c080(uVar8,&local_2c,uVar6,*(uint *)(iVar5 + 0xe0) & 0x20);
  FUN_08009cf4(0x14);
  FUN_0800c490(DAT_080085b8,0xc0,0);
  FUN_08009cf4(0x14);
  FUN_0800c490(DAT_080085b8,0xc0,1);
  FUN_08009cf4(0x14);
  uVar8 = DAT_080085c0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x10) = uVar8;
  *(undefined4 *)(param_1 + 0x14) = 1;
  *(undefined4 *)(param_1 + 0x18) = 1;
  if (uVar7 == 0) {
    iVar5 = 0x1f;
  }
  else {
    iVar5 = LZCOUNT(uVar7) + -1;
  }
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x30) = 0;
  *(int *)(param_1 + 0x20) = iVar5;
  *(undefined4 *)(param_1 + 0x24) = 0x100;
  iVar5 = FUN_0800f250(param_1 + 0x10);
  if ((((iVar5 == 0) && (iVar5 = FUN_0800822c(param_1), iVar5 == 0)) &&
      (iVar5 = FUN_08008400(param_1), iVar5 == 0)) &&
     (iVar5 = FUN_0800f308(param_1 + 0x10), iVar5 == 0)) {
    iVar5 = 0;
    if (*(byte *)(param_1 + 4) < 0xb) {
      iVar5 = DAT_080085c4 + (uint)*(byte *)(param_1 + 4) * 0x400;
    }
    FUN_0800c2f4(iVar5,1 << (uint)*(byte *)(param_1 + 5));
    if (*(byte *)(param_1 + 6) < 0xb) {
      iVar5 = DAT_080085c4 + (uint)*(byte *)(param_1 + 6) * 0x400;
    }
    else {
      iVar5 = 0;
    }
    FUN_0800c2f4(iVar5,1 << *(sbyte *)(param_1 + 7));
    *(undefined4 *)(param_1 + 0x60) = 1;
    *(undefined *)(param_1 + 100) = 1;
    uVar8 = 0;
  }
  else {
    uVar8 = 1;
    *(undefined *)(param_1 + 0x5c) = 1;
  }
  return uVar8;
}



/* === 080085c8 FUN_080085c8 === */

undefined4 FUN_080085c8(undefined4 *param_1,undefined4 *param_2)

{
  char cVar1;
  char cVar2;
  uint uVar3;
  byte bVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  iVar5 = System_GetProgramMemoryRegion();
  if (iVar5 == 7) {
    *(undefined *)(param_1 + 0x17) = 3;
    return 1;
  }
  uVar7 = param_2[2];
  uVar6 = param_2[1];
  *param_1 = *param_2;
  param_1[1] = uVar6;
  param_1[2] = uVar7;
  *(undefined2 *)(param_1 + 3) = *(undefined2 *)(param_2 + 3);
  cVar2 = *(char *)(param_1 + 3);
  cVar1 = *(char *)((int)param_1 + 0xd);
  iVar5 = FUN_0800f308(param_1 + 4);
  if (iVar5 == 0) {
    if (cVar2 == '\x01') {
      uVar6 = 0x800000;
    }
    else {
      uVar6 = 0x100000;
    }
    param_1[0x18] = (uint)*(byte *)(param_1 + 0x19);
    if (*(byte *)(param_1 + 0x19) == 0) {
      FUN_080084a8(param_1,uVar6);
    }
    bVar4 = (byte)((uint)uVar6 >> 0x10);
    uVar3 = (uint)(byte)((bVar4 >> 4 & 1) << 3 | bVar4 >> 7);
    param_1[5] = 1;
    param_1[6] = 1;
    param_1[4] = DAT_0800869c;
    param_1[7] = 0;
    if (uVar3 == 0) {
      iVar5 = 0x1f;
    }
    else {
      iVar5 = LZCOUNT(uVar3 << 8) + -1;
    }
    param_1[8] = iVar5;
    param_1[9] = 0x100;
    param_1[0xb] = 0;
    param_1[0xc] = 0;
    iVar5 = FUN_0800f250(param_1 + 4);
    if ((((iVar5 == 0) && (iVar5 = FUN_0800822c(param_1), iVar5 == 0)) &&
        (iVar5 = FUN_08008290(param_1,cVar2), iVar5 == 0)) &&
       ((iVar5 = FUN_08008350(param_1), iVar5 == 0 &&
        ((cVar1 != '\0' || (iVar5 = FUN_0800817c(param_1), iVar5 == 0)))))) {
      param_1[0x18] = 2;
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x17) = 1;
  return 1;
}



/* === 080087a0 FUN_080087a0 === */

undefined4 FUN_080087a0(int param_1,undefined4 param_2,uint param_3,undefined4 param_4,char param_5)

{
  int iVar1;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_4c;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  uint local_30;
  undefined4 local_2c;
  undefined4 uStack_28;
  undefined4 local_24;
  
  iVar1 = System_GetProgramMemoryRegion();
  if (iVar1 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  if (*(char *)(param_1 + 0xd) != '\x01') {
    *(undefined *)(param_1 + 0xd) = 1;
    iVar1 = FUN_080085c8(param_1,param_1);
    if (iVar1 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
      return 1;
    }
  }
  local_34 = 0x1000000;
  if (0xff < param_3) {
    param_3 = 0x100;
  }
  local_40 = 0x100;
  local_3c = 0x400;
  local_38 = 0;
  local_44 = 0;
  local_24 = 0;
  local_58 = 2;
  local_4c = 0x2000;
  local_2c = 0;
  uStack_28 = 0;
  local_54 = param_2;
  local_30 = param_3;
  iVar1 = FUN_080080f0();
  if (iVar1 == 0) {
    iVar1 = FUN_0800f32c(param_1 + 0x10,&local_58,5000);
    if (iVar1 == 0) {
      iVar1 = FUN_0800f3f0(param_1 + 0x10,param_4,5000);
      if (iVar1 != 0) goto LAB_0800875e;
      iVar1 = FUN_080081d8(param_1,5000);
      if (iVar1 == 0) {
        if (param_5 == '\0') {
          return 0;
        }
        if (*(char *)(param_1 + 0xd) == '\0') {
          return 0;
        }
        *(undefined *)(param_1 + 0xd) = 0;
        iVar1 = FUN_080085c8(param_1,param_1);
        if (iVar1 == 0) {
          return 0;
        }
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
        return 1;
      }
    }
    if (*(char *)(param_1 + 0xd) != '\0') {
      *(undefined *)(param_1 + 0xd) = 0;
      iVar1 = FUN_080085c8(param_1,param_1);
      if (iVar1 != 0) {
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
      }
    }
  }
  else {
LAB_0800875e:
    if (*(char *)(param_1 + 0xd) != '\0') {
      *(undefined *)(param_1 + 0xd) = 0;
      iVar1 = FUN_080085c8(param_1,param_1);
      if (iVar1 != 0) {
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)(param_1 + 0x5c) = 2;
        FUN_080085c8(param_1,param_1);
      }
    }
  }
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 0800895c FUN_0800895c === */

undefined4 FUN_0800895c(int param_1,undefined4 param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_44;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  local_38 = 0x100;
  local_50 = 0xd7;
  local_30 = 0;
  local_2c = 0;
  local_34 = 0x400;
  local_3c = 0;
  local_44 = 0x2000;
  local_28 = 1;
  local_1c = 0;
  local_24 = 0;
  uStack_20 = 0;
  local_4c = param_2;
  iVar2 = System_GetProgramMemoryRegion();
  if (iVar2 == 7) {
    *(undefined *)(param_1 + 0x5c) = 3;
    return 1;
  }
  if (*(char *)(param_1 + 0xd) != '\x01') {
    *(undefined *)(param_1 + 0xd) = 1;
    iVar2 = FUN_080085c8(param_1,param_1);
    if (iVar2 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
      return 1;
    }
  }
  iVar2 = FUN_080080f0(param_1);
  if (iVar2 == 0) {
    iVar2 = FUN_0800f32c(param_1 + 0x10,&local_50,5000);
    if (iVar2 == 0) {
      iVar2 = FUN_080081d8(param_1,5000);
      if (iVar2 == 0) {
        uVar3 = 0;
        if (*(char *)(param_1 + 0xd) != '\0') {
          *(undefined *)(param_1 + 0xd) = 0;
          iVar2 = FUN_080085c8(param_1,param_1);
          if (iVar2 != 0) {
            *(undefined *)(param_1 + 0xd) = 0;
            *(undefined *)(param_1 + 0x5c) = 2;
            uVar3 = 1;
            FUN_080085c8(param_1,param_1);
          }
        }
        return uVar3;
      }
      if (*(char *)(param_1 + 0xd) != '\0') {
        *(undefined *)(param_1 + 0xd) = 0;
        iVar2 = FUN_080085c8(param_1,param_1);
        if (iVar2 != 0) {
          *(undefined *)(param_1 + 0xd) = 0;
          *(undefined *)(param_1 + 0x5c) = 2;
          FUN_080085c8(param_1,param_1);
        }
      }
      goto LAB_080089f6;
    }
    cVar1 = *(char *)(param_1 + 0xd);
  }
  else {
    cVar1 = *(char *)(param_1 + 0xd);
  }
  if (cVar1 != '\0') {
    *(undefined *)(param_1 + 0xd) = 0;
    iVar2 = FUN_080085c8(param_1,param_1);
    if (iVar2 != 0) {
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)(param_1 + 0x5c) = 2;
      FUN_080085c8(param_1,param_1);
    }
  }
LAB_080089f6:
  *(undefined *)(param_1 + 0x5c) = 1;
  return 1;
}



/* === 08008a80 FUN_08008a80 === */

void FUN_08008a80(undefined4 *param_1)

{
  *param_1 = DAT_08008a8c;
  FUN_080085c8();
  return;
}



/* === 08008a90 FUN_08008a90 === */

undefined4 FUN_08008a90(int *param_1,uint param_2,uint param_3,int param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  
  iVar2 = *param_1;
  uVar6 = param_2 & 0xff;
  param_2 = param_2 & 0xfffffff;
  uVar7 = param_3 & 0xff;
  if (uVar6 == 0) {
    if (param_3 < 0x100) {
      FUN_080087a0(iVar2,param_2,param_3,param_4,0);
    }
    else {
      uVar6 = param_2;
      do {
        uVar3 = uVar6 + 0x100;
        FUN_080087a0(iVar2,uVar6,0x100,(param_4 - param_2) + uVar6,0);
        uVar6 = uVar3;
      } while (uVar3 != param_2 + (param_3 & 0xffffff00));
      if (uVar7 != 0) {
        FUN_080087a0(iVar2,param_2 + 0x100 + ((param_3 >> 8) - 1) * 0x100,uVar7,
                     (param_3 & 0xffffff00) + param_4,0);
      }
    }
  }
  else {
    uVar3 = 0x100 - uVar6;
    if (param_3 < 0x100) {
      if (uVar3 < uVar7) {
        FUN_080087a0(iVar2,param_2,uVar3,param_4,0);
        FUN_080087a0(iVar2,param_2 + uVar3,uVar6 + (uVar7 - 0x100),param_4 + uVar3,0);
      }
      else {
        FUN_080087a0(iVar2,param_2,param_3,param_4,0);
      }
    }
    else {
      uVar6 = uVar6 + (param_3 - 0x100);
      FUN_080087a0(iVar2,param_2,uVar3,param_4,0);
      param_4 = param_4 + uVar3;
      iVar1 = param_2 + uVar3;
      if (uVar6 >> 8 != 0) {
        iVar4 = iVar1;
        do {
          iVar5 = iVar4 + 0x100;
          FUN_080087a0(iVar2,iVar4,0x100,(param_4 - iVar1) + iVar4,0);
          iVar4 = iVar5;
        } while (iVar5 != iVar1 + (uVar6 & 0xffffff00));
        param_4 = param_4 + (uVar6 & 0xffffff00);
        iVar1 = iVar1 + 0x100 + ((uVar6 >> 8) - 1) * 0x100;
      }
      if ((uVar6 & 0xff) != 0) {
        FUN_080087a0(iVar2,iVar1,uVar6 & 0xff,param_4,0);
      }
    }
  }
  if (*(char *)(iVar2 + 0xd) != '\0') {
    *(undefined *)(iVar2 + 0xd) = 0;
    iVar1 = FUN_080085c8(iVar2,iVar2);
    if (iVar1 != 0) {
      *(undefined *)(iVar2 + 0xd) = 0;
      *(undefined *)(iVar2 + 0x5c) = 2;
      FUN_080085c8(iVar2,iVar2);
      return 1;
    }
  }
  return 0;
}



/* === 08008a98 FUN_08008a98 === */

undefined4 FUN_08008a98(int *param_1,uint param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  
  iVar3 = *param_1;
  param_2 = DAT_08008af0 & param_2;
  do {
    if (param_3 <= param_2) {
      return 0;
    }
    uVar2 = param_2 & 0xfffffff;
    param_2 = param_2 + 0x1000;
    iVar1 = FUN_0800895c(iVar3,uVar2);
  } while (iVar1 == 0);
  if (*(char *)(iVar3 + 0xd) != '\0') {
    *(undefined *)(iVar3 + 0xd) = 0;
    iVar1 = FUN_080085c8(iVar3,iVar3);
    if (iVar1 != 0) {
      *(undefined *)(iVar3 + 0xd) = 0;
      *(undefined *)(iVar3 + 0x5c) = 2;
      FUN_080085c8(iVar3,iVar3);
    }
  }
  *(undefined *)(iVar3 + 0x5c) = 1;
  return 1;
}



/* === 08008af4 FUN_08008af4 === */

void FUN_08008af4(int *param_1)

{
  byte bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  byte *pbVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  uint uVar13;
  uint uVar14;
  uint uVar15;
  undefined4 local_34;
  undefined2 local_30;
  int local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_1c;
  
  iVar5 = DAT_08008c68;
  uVar4 = DAT_08008c64;
  puVar3 = DAT_08008c60;
  iVar2 = DAT_08008c5c;
  iVar12 = DAT_08008c58;
  uVar15 = 0;
  local_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_20 = 0;
  local_1c = 0;
  if (*param_1 != DAT_08008c54) {
    return;
  }
  *(uint *)(DAT_08008c58 + 0xd4) = *(uint *)(DAT_08008c58 + 0xd4) | 0x4000;
  uVar7 = *(uint *)(iVar12 + 0xd4) & 0x4000;
  *(uint *)(iVar12 + 0x7c) = *(uint *)(iVar12 + 0x7c) | 0x4000;
  *(uint *)(iVar12 + 0x7c) = *(uint *)(iVar12 + 0x7c) & 0xffffbfff;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x40;
  uVar8 = *(uint *)(iVar12 + 0xe0) & 0x40;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x20;
  uVar9 = *(uint *)(iVar12 + 0xe0) & 0x20;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 0x10;
  uVar10 = *(uint *)(iVar12 + 0xe0) & 0x10;
  *(uint *)(iVar12 + 0xe0) = *(uint *)(iVar12 + 0xe0) | 2;
  uVar13 = *(uint *)(iVar12 + 0xe0) & 2;
  do {
    uVar14 = uVar15 & 0xff;
    if (*(int *)(iVar5 + 0x60) == 0) {
      if ((uVar15 & 0xfc) != 0) goto LAB_08008c3c;
      pbVar6 = *(byte **)(iVar5 + (uVar14 + 0x20) * 4);
      uVar11 = (uint)*pbVar6;
      iVar12 = 0;
      if (uVar11 < 0xb) {
        iVar12 = iVar2 + uVar11 * 0x400;
      }
      local_34 = uVar4;
      local_2c = 1 << pbVar6[1];
      bVar1 = *(byte *)((int)&local_34 + uVar14);
    }
    else {
      if (5 < uVar14) {
LAB_08008c3c:
        FUN_0800a968(0x5c,0,0,uVar14,uVar7,uVar8,uVar9,uVar10,uVar13);
        FUN_0800a9e4(0x5c);
        return;
      }
      pbVar6 = *(byte **)(iVar5 + (uVar14 + 0x1a) * 4);
      uVar11 = (uint)*pbVar6;
      if (uVar11 < 0xb) {
        iVar12 = iVar2 + uVar11 * 0x400;
      }
      else {
        iVar12 = 0;
      }
      local_2c = 1 << pbVar6[1];
      local_28 = 2;
      local_24 = 0;
      local_20 = 3;
      local_34 = *puVar3;
      local_30 = (short)puVar3[1];
      bVar1 = *(byte *)((int)&local_34 + uVar14);
    }
    local_20 = 3;
    local_24 = 0;
    local_28 = 2;
    local_1c = (uint)bVar1;
    uVar15 = uVar15 + 1;
    FUN_0800c080(iVar12,&local_2c);
  } while( true );
}



/* === 08008c6c FUN_08008c6c === */

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



/* === 08008cf0 FUN_08008cf0 === */

void FUN_08008cf0(void)

{
  uint **ppuVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  
  ppuVar1 = DAT_08008cf8;
  puVar6 = *DAT_08008cf8;
  uVar5 = puVar6[2];
  uVar4 = *puVar6;
  if ((-1 < (int)(uVar5 << 0x1d)) || (-1 < (int)(uVar4 << 0xd))) {
    if (((int)(uVar5 << 0x1e) < 0) && ((int)(uVar4 << 0xe) < 0)) {
      puVar6[3] = 2;
      *puVar6 = *puVar6 & 0xfff8ffff;
      if (*(char *)((int)ppuVar1 + 0x41) == '\x12') {
        if ((int)(*puVar6 << 0x1d) < 0) {
          uVar4 = *ppuVar1[0xf];
          *puVar6 = *puVar6 & 0xfffffffb;
          *(uint *)(uVar4 + 0xc) = *(uint *)(uVar4 + 0xc) & 0xfffffffe;
        }
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7b0(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) == '\"') {
        if ((int)(*puVar6 << 0x1d) < 0) {
          uVar4 = *ppuVar1[0xf];
          *puVar6 = *puVar6 & 0xfffffffb;
          *(uint *)(uVar4 + 0xc) = *(uint *)(uVar4 + 0xc) & 0xfffffffe;
        }
        else {
          uVar4 = puVar6[2];
          while (((uVar4 & 0x3f00) != 0 && (ppuVar1[0xe] != (uint *)0x0))) {
            *(undefined *)ppuVar1[0xc] = *(undefined *)(puVar6 + 8);
            ppuVar1[0xe] = (uint *)((int)ppuVar1[0xe] + -1);
            uVar4 = (*ppuVar1)[2];
            ppuVar1[0xc] = (uint *)((int)ppuVar1[0xc] + 1);
          }
        }
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7ac(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) == '\x02') {
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7a8(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) != '\b') {
        return;
      }
      puVar6[5] = puVar6[5] & 0xf3ffffff;
      *(undefined *)((int)ppuVar1 + 0x41) = 1;
      if (ppuVar1[0x11] == (uint *)0x0) {
        FUN_0800f7a4();
        return;
      }
    }
    else {
      if (((int)(uVar5 << 0x1c) < 0) && ((int)(uVar4 << 0xc) < 0)) {
        puVar6[3] = 8;
        if ((int)(*puVar6 << 9) < 0) {
          *puVar6 = *puVar6 & 0xfff6ffff;
          *(undefined *)((int)ppuVar1 + 0x41) = 1;
        }
        FUN_0800f7b8(ppuVar1);
        return;
      }
      if ((-1 < (int)(uVar5 << 0x1f)) || (-1 < (int)(uVar4 << 0xf))) {
        if (-1 < (int)(uVar5 << 0x1b)) {
          return;
        }
        if (-1 < (int)(uVar4 << 0xb)) {
          return;
        }
        puVar6[3] = 0x10;
        FUN_0800f7bc(ppuVar1);
        return;
      }
      puVar6[3] = 1;
      *puVar6 = *puVar6 & 0xfff0ffff;
      ppuVar1[0x11] = (uint *)((uint)ppuVar1[0x11] | 2);
      if (-1 < (int)(*puVar6 << 0x1d)) {
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f768(ppuVar1);
        return;
      }
      puVar3 = ppuVar1[0xf];
      *puVar6 = *puVar6 & 0xfffffffb;
      puVar3[0x16] = DAT_0800f9cc;
      iVar2 = FUN_0801549c();
      if (iVar2 == 0) {
        return;
      }
      ppuVar1[0x11] = (uint *)((uint)ppuVar1[0x11] | 4);
      *(undefined *)((int)ppuVar1 + 0x41) = 1;
    }
    FUN_0800f768(ppuVar1);
    return;
  }
  puVar3 = puVar6;
  if (*(char *)((int)DAT_08008cf8 + 0x41) == '\x12') {
    uVar4 = puVar6[2];
    while ((int)(uVar4 << 0x1d) < 0) {
      if (ppuVar1[0xb] == (uint *)0x0) goto LAB_0800f8f4;
      *(undefined *)(puVar6 + 8) = *(undefined *)ppuVar1[9];
      puVar3 = *ppuVar1;
      ppuVar1[0xb] = (uint *)((int)ppuVar1[0xb] + -1);
      uVar4 = puVar3[2];
      ppuVar1[9] = (uint *)((int)ppuVar1[9] + 1);
    }
  }
  else if (*(char *)((int)DAT_08008cf8 + 0x41) == '\"') {
    uVar4 = puVar6[2];
    while ((int)(uVar4 << 0x1d) < 0) {
      if (ppuVar1[0xe] == (uint *)0x0) goto LAB_0800f8f4;
      *(undefined *)ppuVar1[0xc] = *(undefined *)(puVar6 + 8);
      puVar3 = *ppuVar1;
      ppuVar1[0xe] = (uint *)((int)ppuVar1[0xe] + -1);
      uVar4 = puVar3[2];
      ppuVar1[0xc] = (uint *)((int)ppuVar1[0xc] + 1);
    }
  }
LAB_0800f7e4:
  FUN_0800f7b4(ppuVar1);
  return;
LAB_0800f8f4:
  *puVar3 = *puVar3 & 0xfffbffff;
  goto LAB_0800f7e4;
}



/* === 08008d3c SaiHandle_Impl_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_Impl_Init(int *param_1,int *param_2)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  int aiStack_28 [5];
  
  iVar3 = *param_2;
  if (1 < iVar3) {
    return 1;
  }
  param_1[0x92] = 0;
  param_1[0x93] = 0;
  param_1[0x94] = 0;
  aiStack_28[0] = DAT_08008ea8;
  aiStack_28[1] = DAT_08008ea8 + 0x400;
  iVar7 = aiStack_28[iVar3];
  aiStack_28[2] = DAT_08008ea8 + 0x20;
  aiStack_28[3] = DAT_08008ea8 + 0x420;
  iVar6 = aiStack_28[iVar3 + 2];
  iVar3 = param_2[1];
  iVar2 = param_2[2];
  iVar4 = param_2[3];
  *param_1 = *param_2;
  param_1[1] = iVar3;
  param_1[2] = iVar2;
  param_1[3] = iVar4;
  iVar3 = param_2[5];
  iVar2 = param_2[6];
  iVar4 = param_2[7];
  param_1[4] = param_2[4];
  param_1[5] = iVar3;
  param_1[6] = iVar2;
  param_1[7] = iVar4;
  iVar3 = param_2[9];
  iVar2 = param_2[4];
  param_1[8] = param_2[8];
  param_1[9] = iVar3;
  param_1[10] = iVar7;
  param_1[0x30] = iVar6;
  iVar3 = DAT_08008eac;
  switch(iVar2) {
  case 0:
    param_1[0x12] = 8000;
    param_1[0x38] = 8000;
    break;
  case 1:
    param_1[0x12] = 16000;
    param_1[0x38] = 16000;
    break;
  case 2:
    param_1[0x12] = 32000;
    param_1[0x38] = 32000;
    break;
  case 3:
    param_1[0x12] = 48000;
    param_1[0x38] = 48000;
    break;
  case 4:
    param_1[0x12] = DAT_08008eac;
    param_1[0x38] = iVar3;
  }
  iVar3 = param_2[8];
  if (param_2[6] == 0) {
    param_1[0xc] = 0;
    iVar2 = param_2[7];
    if (iVar3 != 0) {
      iVar3 = 1;
    }
    param_1[0xb] = iVar3;
    iVar4 = param_2[9];
  }
  else {
    iVar4 = param_2[9];
    param_1[0xc] = 1;
    if (iVar3 == 0) {
      iVar3 = 2;
    }
    else {
      iVar3 = 3;
    }
    param_1[0xb] = iVar3;
    iVar2 = param_2[7];
  }
  if (iVar2 == 0) {
    iVar3 = param_2[5];
    param_1[0x32] = 0;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
    param_1[0x31] = iVar4;
  }
  else {
    iVar3 = param_2[5];
    param_1[0x32] = 1;
    if (iVar4 == 0) {
      iVar2 = 2;
    }
    else {
      iVar2 = 3;
    }
    param_1[0x31] = iVar2;
  }
  if (iVar3 == 1) {
    uVar5 = 2;
    uVar1 = 1;
  }
  else if (iVar3 == 2) {
    uVar1 = 0;
    uVar5 = 3;
  }
  else {
    uVar1 = 0;
    uVar5 = uVar1;
  }
  param_1[0x11] = 0;
  param_1[0xd] = 0;
  param_1[0x17] = 0;
  param_1[0x37] = 0;
  param_1[0x33] = 0;
  param_1[0x3d] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = 0;
  param_1[0x15] = 0;
  param_1[0x16] = 0;
  param_1[0x35] = 0;
  param_1[0x36] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  iVar3 = HAL_SAI_InitProtocol(param_1 + 10,uVar1,uVar5,2);
  if (iVar3 == 0) {
    iVar3 = HAL_SAI_InitProtocol(param_1 + 0x30,uVar1,uVar5,2);
    if (iVar3 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    return 0;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08008f50 SaiHandle_Impl_StartDma === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4
SaiHandle_Impl_StartDma
          (int param_1,undefined4 param_2,undefined4 param_3,uint param_4,undefined4 param_5)

{
  int iVar1;
  
  *(uint *)(param_1 + 0x250) = param_4;
  *(undefined4 *)(param_1 + 0x248) = param_2;
  *(undefined4 *)(param_1 + 0x24c) = param_3;
  *(undefined4 *)(param_1 + 0x254) = param_5;
  if (*(int *)(param_1 + 0x18) == 1) {
    if (*(int *)(param_1 + 0x20) == 1) {
      FUN_08012088(param_1 + 0x28);
      iVar1 = *(int *)(param_1 + 0x24);
    }
    else {
      FUN_08011f64(param_1 + 0x28,param_3);
      iVar1 = *(int *)(param_1 + 0x24);
    }
    if (iVar1 != 1) {
      FUN_08011f64(param_1 + 0xc0,param_3);
      return 0;
    }
    FUN_08012088(param_1 + 0xc0,param_2,param_4 & 0xffff);
    return 0;
  }
  if (*(int *)(param_1 + 0x24) == 1) {
    FUN_08012088(param_1 + 0xc0);
    iVar1 = *(int *)(param_1 + 0x20);
  }
  else {
    FUN_08011f64(param_1 + 0xc0,param_3);
    iVar1 = *(int *)(param_1 + 0x20);
  }
  if (iVar1 != 1) {
    FUN_08011f64(param_1 + 0x28,param_3);
    return 0;
  }
  FUN_08012088(param_1 + 0x28,param_2,param_4 & 0xffff);
  return 0;
}



/* === 08009004 FUN_08009004 === */

void FUN_08009004(int *param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  undefined2 *puVar6;
  undefined2 local_40;
  undefined2 local_3e;
  undefined2 local_3c;
  undefined2 local_3a;
  undefined2 local_38;
  byte abStack_36 [2];
  int local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  undefined4 local_24;
  
  iVar2 = DAT_080090bc;
  puVar6 = &local_40;
  local_40 = *(undefined2 *)((int)param_1 + 6);
  local_3e = *(undefined2 *)(param_1 + 1);
  local_3c = *(undefined2 *)(param_1 + 2);
  local_3a = *(undefined2 *)((int)param_1 + 10);
  local_38 = *(undefined2 *)(param_1 + 3);
  if (param_1[6] == 0) {
    bVar1 = true;
  }
  else {
    bVar1 = param_1[7] == 0;
  }
  local_28 = 1;
  local_30 = 2;
  uStack_2c = 1;
  do {
    uVar3 = (uint)*(byte *)puVar6;
    uVar5 = (uint)*(byte *)((int)puVar6 + 1);
    if ((*(byte *)(param_1 + 1) == uVar3) && (*(byte *)((int)param_1 + 5) == uVar5)) {
      if (bVar1) {
        iVar4 = *param_1;
        if (iVar4 == 0) goto LAB_080090a6;
LAB_08009066:
        if (iVar4 == 1) {
          if (uVar3 == 0) {
            if (uVar5 == 2) {
              local_24 = 8;
            }
            else {
              local_24 = 10;
            }
          }
          else {
            local_24 = 10;
          }
        }
        goto LAB_08009070;
      }
    }
    else {
      iVar4 = *param_1;
      if (iVar4 != 0) goto LAB_08009066;
LAB_080090a6:
      local_24 = 6;
LAB_08009070:
      local_34 = 1 << uVar5;
      if (uVar3 < 0xb) {
        iVar4 = iVar2 + uVar3 * 0x400;
      }
      else {
        iVar4 = 0;
      }
      FUN_0800c080(iVar4,&local_34);
    }
    puVar6 = (undefined2 *)((int)puVar6 + 2);
    if ((undefined2 *)abStack_36 == puVar6) {
      return;
    }
  } while( true );
}



/* === 080090c0 FUN_080090c0 === */

void FUN_080090c0(int *param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int *piVar7;
  int *piVar8;
  
  piVar7 = DAT_08009220;
  piVar1 = DAT_0800921c;
  iVar2 = DAT_08009218;
  iVar4 = *param_1;
  iVar3 = DAT_08009210;
  if ((iVar4 == DAT_08009210) || (iVar3 = DAT_08009210 + 0x20, iVar4 == iVar3)) {
    *(uint *)(DAT_08009218 + 0xe0) = *(uint *)(DAT_08009218 + 0xe0) | 1;
    uVar5 = *(uint *)(iVar2 + 0xe0);
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x10;
    uVar6 = *(uint *)(iVar2 + 0xe0);
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x40;
    *(uint *)(iVar2 + 0xf0) = *(uint *)(iVar2 + 0xf0) | 0x400000;
    FUN_08009004(piVar7,param_2,iVar3,*(uint *)(iVar2 + 0xf0) & 0x400000,uVar5 & 1,uVar6 & 0x10,
                 *(uint *)(iVar2 + 0xe0) & 0x40);
    piVar1 = DAT_08009220;
    iVar3 = DAT_08009210;
    iVar4 = *param_1;
    *(uint *)(iVar2 + 0xd8) = *(uint *)(iVar2 + 0xd8) | 1;
    iVar4 = iVar4 - iVar3;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
  }
  else {
    iVar3 = DAT_08009214;
    if ((iVar4 != DAT_08009214) && (iVar3 = DAT_08009214 + 0x20, iVar4 != iVar3)) {
      return;
    }
    *(uint *)(DAT_08009218 + 0xe0) = *(uint *)(DAT_08009218 + 0xe0) | 1;
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 8;
    *(uint *)(iVar2 + 0xe0) = *(uint *)(iVar2 + 0xe0) | 0x40;
    *(uint *)(iVar2 + 0xf0) = *(uint *)(iVar2 + 0xf0) | 0x800000;
    FUN_08009004(piVar1,param_2,iVar3,*(uint *)(iVar2 + 0xf0) & 0x800000);
    piVar1 = DAT_0800921c;
    iVar3 = DAT_08009214;
    iVar4 = *param_1;
    *(uint *)(iVar2 + 0xd8) = *(uint *)(iVar2 + 0xd8) | 1;
    iVar4 = iVar4 - iVar3;
    if (iVar4 != 0) {
      iVar4 = 1;
    }
  }
  if (iVar4 == 0) {
    piVar8 = piVar1 + 10;
    piVar7 = piVar1 + 0x56;
    if (piVar1[8] == 1) {
      iVar2 = 0;
    }
    else {
      iVar2 = 0x40;
    }
    if (*piVar1 == 0) {
      iVar3 = 0x57;
      piVar1[0x56] = DAT_08008f40;
    }
    else {
      iVar3 = 0x59;
      piVar1[0x56] = DAT_08008f4c;
    }
  }
  else {
    piVar8 = piVar1 + 0x30;
    piVar7 = piVar1 + 0x74;
    if (piVar1[9] == 1) {
      iVar2 = 0;
    }
    else {
      iVar2 = 0x40;
    }
    if (*piVar1 == 0) {
      iVar3 = 0x58;
      piVar1[0x74] = DAT_08008f48;
    }
    else {
      iVar3 = 0x5a;
      piVar1[0x74] = DAT_08008f44;
    }
  }
  piVar7[1] = iVar3;
  piVar7[2] = iVar2;
  piVar7[9] = 0;
  piVar7[5] = 0x1000;
  piVar7[3] = 0;
  piVar7[4] = 0x400;
  piVar7[6] = 0x4000;
  piVar7[7] = 0x100;
  piVar7[8] = 0x20000;
  iVar2 = FUN_0800af40(piVar7);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  piVar8[0x20] = (int)piVar7;
  piVar8[0x21] = (int)piVar7;
  piVar7[0xe] = (int)piVar8;
  return;
}



/* === 08009224 FUN_08009224 === */

void FUN_08009224(void)

{
  FUN_0800b9d4(DAT_0800922c);
  return;
}



/* === 08009230 FUN_08009230 === */

void FUN_08009230(void)

{
  FUN_0800b9d4(DAT_08009238);
  return;
}



/* === 0800923c FUN_0800923c === */

void FUN_0800923c(void)

{
  FUN_0800b9d4(DAT_08009244);
  return;
}



/* === 08009248 FUN_08009248 === */

void FUN_08009248(void)

{
  FUN_0800b9d4(DAT_08009250);
  return;
}



/* === 08009254 FUN_08009254 === */

void FUN_08009254(int *param_1)

{
  code **ppcVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = DAT_080092c8;
  iVar3 = *param_1;
  if ((iVar3 == DAT_080092c0) || (iVar3 == DAT_080092c0 + 0x20)) {
    ppcVar1 = (code **)(DAT_080092c8 + 0x254);
    *(undefined4 *)(DAT_080092c8 + 600) = 0;
    if (*ppcVar1 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x080092bc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**ppcVar1)(*(undefined4 *)(iVar2 + 0x248),*(undefined4 *)(iVar2 + 0x24c),
                  *(uint *)(iVar2 + 0x250) >> 1);
      return;
    }
  }
  else if ((iVar3 == DAT_080092c4) || (iVar3 == DAT_080092c4 + 0x20)) {
    ppcVar1 = (code **)(DAT_080092c8 + 0x4b0);
    *(undefined4 *)(DAT_080092c8 + 0x4b4) = 0;
    if (*ppcVar1 != (code *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0800929a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**ppcVar1)(*(undefined4 *)(iVar2 + 0x4a4),*(undefined4 *)(iVar2 + 0x4a8),
                  *(uint *)(iVar2 + 0x4ac) >> 1);
      return;
    }
  }
  return;
}



/* === 080092cc FUN_080092cc === */

void FUN_080092cc(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  code *UNRECOVERED_JUMPTABLE;
  
  iVar1 = DAT_08009338;
  iVar4 = *param_1;
  if ((iVar4 == DAT_08009330) || (iVar4 == DAT_08009330 + 0x20)) {
    UNRECOVERED_JUMPTABLE = *(code **)(DAT_08009338 + 0x254);
    uVar3 = *(uint *)(DAT_08009338 + 0x250) >> 1;
    *(uint *)(DAT_08009338 + 600) = uVar3;
    if (UNRECOVERED_JUMPTABLE != (code *)0x0) {
      iVar4 = *(int *)(iVar1 + 0x248);
      iVar2 = *(int *)(iVar1 + 0x24c);
LAB_08009320:
                    /* WARNING: Could not recover jumptable at 0x0800932e. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*UNRECOVERED_JUMPTABLE)(iVar4 + uVar3 * 4,iVar2 + uVar3 * 4);
      return;
    }
  }
  else if ((iVar4 == DAT_08009334) || (iVar4 == DAT_08009334 + 0x20)) {
    UNRECOVERED_JUMPTABLE = *(code **)(DAT_08009338 + 0x4b0);
    uVar3 = *(uint *)(DAT_08009338 + 0x4ac) >> 1;
    *(uint *)(DAT_08009338 + 0x4b4) = uVar3;
    if (UNRECOVERED_JUMPTABLE != (code *)0x0) {
      iVar2 = *(int *)(iVar1 + 0x4a8);
      iVar4 = *(int *)(iVar1 + 0x4a4);
      goto LAB_08009320;
    }
  }
  return;
}



/* === 0800933c SaiHandle_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_Init(int **param_1,int *param_2)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  int local_28 [5];
  
  piVar1 = (int *)(*param_2 * 0x25c + DAT_08009350);
  *param_1 = piVar1;
  iVar4 = *param_2;
  if (1 < iVar4) {
    return 1;
  }
  piVar1[0x92] = 0;
  piVar1[0x93] = 0;
  piVar1[0x94] = 0;
  local_28[0] = DAT_08008ea8;
  local_28[1] = DAT_08008ea8 + 0x400;
  iVar8 = local_28[iVar4];
  local_28[2] = DAT_08008ea8 + 0x20;
  local_28[3] = DAT_08008ea8 + 0x420;
  iVar7 = local_28[iVar4 + 2];
  iVar4 = param_2[1];
  iVar3 = param_2[2];
  iVar5 = param_2[3];
  *piVar1 = *param_2;
  piVar1[1] = iVar4;
  piVar1[2] = iVar3;
  piVar1[3] = iVar5;
  iVar4 = param_2[5];
  iVar3 = param_2[6];
  iVar5 = param_2[7];
  piVar1[4] = param_2[4];
  piVar1[5] = iVar4;
  piVar1[6] = iVar3;
  piVar1[7] = iVar5;
  iVar4 = param_2[9];
  iVar3 = param_2[4];
  piVar1[8] = param_2[8];
  piVar1[9] = iVar4;
  piVar1[10] = iVar8;
  piVar1[0x30] = iVar7;
  iVar4 = DAT_08008eac;
  switch(iVar3) {
  case 0:
    piVar1[0x12] = 8000;
    piVar1[0x38] = 8000;
    break;
  case 1:
    piVar1[0x12] = 16000;
    piVar1[0x38] = 16000;
    break;
  case 2:
    piVar1[0x12] = 32000;
    piVar1[0x38] = 32000;
    break;
  case 3:
    piVar1[0x12] = 48000;
    piVar1[0x38] = 48000;
    break;
  case 4:
    piVar1[0x12] = DAT_08008eac;
    piVar1[0x38] = iVar4;
  }
  iVar4 = param_2[8];
  if (param_2[6] == 0) {
    piVar1[0xc] = 0;
    iVar3 = param_2[7];
    if (iVar4 != 0) {
      iVar4 = 1;
    }
    piVar1[0xb] = iVar4;
    iVar5 = param_2[9];
  }
  else {
    iVar5 = param_2[9];
    piVar1[0xc] = 1;
    if (iVar4 == 0) {
      iVar4 = 2;
    }
    else {
      iVar4 = 3;
    }
    piVar1[0xb] = iVar4;
    iVar3 = param_2[7];
  }
  if (iVar3 == 0) {
    iVar4 = param_2[5];
    piVar1[0x32] = 0;
    if (iVar5 != 0) {
      iVar5 = 1;
    }
    piVar1[0x31] = iVar5;
  }
  else {
    iVar4 = param_2[5];
    piVar1[0x32] = 1;
    if (iVar5 == 0) {
      iVar3 = 2;
    }
    else {
      iVar3 = 3;
    }
    piVar1[0x31] = iVar3;
  }
  if (iVar4 == 1) {
    uVar6 = 2;
    uVar2 = 1;
  }
  else if (iVar4 == 2) {
    uVar2 = 0;
    uVar6 = 3;
  }
  else {
    uVar2 = 0;
    uVar6 = uVar2;
  }
  piVar1[0x11] = 0;
  piVar1[0xd] = 0;
  piVar1[0x17] = 0;
  piVar1[0x37] = 0;
  piVar1[0x33] = 0;
  piVar1[0x3d] = 0;
  piVar1[0xf] = 0;
  piVar1[0x10] = 0;
  piVar1[0x15] = 0;
  piVar1[0x16] = 0;
  piVar1[0x35] = 0;
  piVar1[0x36] = 0;
  piVar1[0x3b] = 0;
  piVar1[0x3c] = 0;
  iVar4 = HAL_SAI_InitProtocol(piVar1 + 10,uVar2,uVar6,2);
  if (iVar4 == 0) {
    iVar4 = HAL_SAI_InitProtocol(piVar1 + 0x30,uVar2,uVar6,2);
    if (iVar4 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    return 0;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08009354 SaiHandle_GetConfig === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_GetConfig(undefined4 *param_1)

{
  return *param_1;
}



/* === 08009358 SaiHandle_StartDma === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4
SaiHandle_StartDma(int *param_1,undefined4 param_2,undefined4 param_3,uint param_4,
                  undefined4 param_5)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *param_1;
  *(uint *)(iVar1 + 0x250) = param_4;
  *(undefined4 *)(iVar1 + 0x248) = param_2;
  *(undefined4 *)(iVar1 + 0x24c) = param_3;
  *(undefined4 *)(iVar1 + 0x254) = param_5;
  if (*(int *)(iVar1 + 0x18) == 1) {
    if (*(int *)(iVar1 + 0x20) == 1) {
      FUN_08012088(iVar1 + 0x28);
      iVar2 = *(int *)(iVar1 + 0x24);
    }
    else {
      FUN_08011f64(iVar1 + 0x28,param_3);
      iVar2 = *(int *)(iVar1 + 0x24);
    }
    if (iVar2 != 1) {
      FUN_08011f64(iVar1 + 0xc0,param_3);
      return 0;
    }
    FUN_08012088(iVar1 + 0xc0,param_2,param_4 & 0xffff);
    return 0;
  }
  if (*(int *)(iVar1 + 0x24) == 1) {
    FUN_08012088(iVar1 + 0xc0);
    iVar2 = *(int *)(iVar1 + 0x20);
  }
  else {
    FUN_08011f64(iVar1 + 0xc0,param_3);
    iVar2 = *(int *)(iVar1 + 0x20);
  }
  if (iVar2 != 1) {
    FUN_08011f64(iVar1 + 0x28,param_3);
    return 0;
  }
  FUN_08012088(iVar1 + 0x28,param_2,param_4 & 0xffff);
  return 0;
}



/* === 08009360 SaiHandle_GetSampleRate === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 SaiHandle_GetSampleRate(int *param_1)

{
  if (*(uint *)(*param_1 + 0x10) < 5) {
    return *(undefined4 *)(DAT_0800937c + *(uint *)(*param_1 + 0x10) * 4);
  }
  return DAT_08009380;
}



/* === 08009384 FUN_08009384 === */

undefined4 FUN_08009384(int *param_1)

{
  return *(undefined4 *)(*param_1 + 600);
}



/* === 080093b0 FUN_080093b0 === */

void FUN_080093b0(void)

{
  FUN_08009cd0();
  FUN_0800aaf4();
  return;
}



/* === 080093c0 FUN_080093c0 === */

void FUN_080093c0(void)

{
  char *pcVar1;
  undefined uVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint *puVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int *piVar17;
  uint uVar18;
  
  if (*DAT_080093dc != 0) {
    FUN_0800c498();
  }
  piVar3 = DAT_080093e0;
  if (*DAT_080093e0 == 0) {
    return;
  }
  iVar15 = *DAT_080093e0;
  iVar4 = FUN_08012bcc(iVar15);
  if ((iVar4 == 0) && (iVar4 = FUN_08012b60(*piVar3), iVar4 != 0)) {
    piVar3[0x13f] = (uint)(*(int *)(iVar15 + 0x808) << 10) >> 0x12;
    uVar5 = FUN_08012b60(*piVar3);
    if ((uVar5 & 2) != 0) {
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 2;
    }
    uVar5 = FUN_08012b60();
    iVar4 = *piVar3;
    if ((uVar5 & 0x10) != 0) {
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) & 0xffffffef;
      uVar12 = *(uint *)(iVar15 + 0x20);
      uVar5 = (uVar12 << 0xb) >> 0x1c;
      uVar16 = uVar12 & 0xf;
      if (uVar5 == 2) {
        if ((uVar12 & 0x7ff0) != 0) {
          uVar5 = (uVar12 << 0x11) >> 0x15;
          FUN_08012a34(iVar15,piVar3[uVar16 * 9 + 0xa2]);
          iVar4 = *piVar3;
          piVar3[uVar16 * 9 + 0xa2] = piVar3[uVar16 * 9 + 0xa2] + uVar5;
          piVar3[uVar16 * 9 + 0xa4] = piVar3[uVar16 * 9 + 0xa4] + uVar5;
        }
      }
      else if (uVar5 == 6) {
        FUN_08012a34(iVar15,piVar3 + 0x131,8);
        iVar4 = *piVar3;
        piVar3[uVar16 * 9 + 0xa4] = ((uVar12 << 0x11) >> 0x15) + piVar3[uVar16 * 9 + 0xa4];
      }
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) | 0x10;
    }
    iVar4 = FUN_08012b60();
    if ((iVar4 << 0xc < 0) && (uVar5 = FUN_08012b78(*piVar3), uVar5 != 0)) {
      iVar4 = iVar15 + 0xb00;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((int)(uVar5 << 0x1f) < 0) {
          uVar11 = uVar12 & 0xff;
          uVar16 = FUN_08012b98(*piVar3,uVar11);
          if ((uVar16 & 1) != 0) {
            iVar13 = *piVar3;
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar4 + 8) = 1;
            uVar18 = *(uint *)(iVar13 + 0x40);
            iVar9 = iVar13 + 0xb00 + uVar12 * 0x20;
            uVar14 = *(uint *)(iVar9 + 8);
            if (iVar7 == 1) {
              if ((int)(uVar14 << 0x1c) < 0) {
                if ((DAT_0800ee48 < uVar18) && ((int)(uVar14 << 0x10) < 0)) {
LAB_0800ed9a:
                  *(undefined4 *)(iVar9 + 8) = 0x8000;
                }
              }
              else if ((int)(uVar14 << 0x1a) < 0) {
                *(undefined4 *)(iVar9 + 8) = 0x20;
              }
              else if ((uVar14 & 0x28) == 0) {
                if ((uVar18 <= DAT_0800ee48) || (-1 < (int)(uVar14 << 0x10))) {
                  iVar7 = piVar10[0xa7] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                  piVar10[0xa4] = iVar7;
                  if (uVar12 == 0) {
                    if (piVar3[0xa3] == 0) {
                      FUN_08012c00(iVar13,1,piVar3 + 0x131);
                    }
                    else {
                      piVar3[0xa2] = iVar7 + piVar3[0xa2];
                    }
                  }
                  goto LAB_0800ec4c;
                }
                goto LAB_0800ed9a;
              }
            }
            else {
              if (uVar18 == DAT_0800ee4c) {
                if ((int)(uVar14 << 0x10) < 0) goto LAB_0800ed9a;
                if ((int)(uVar14 << 0x1a) < 0) {
                  *(undefined4 *)(iVar9 + 8) = 0x20;
                }
              }
              else if ((uVar12 == 0) && (piVar3[0xa3] == 0)) {
                FUN_08012c00(iVar13,0,piVar3 + 0x131);
              }
LAB_0800ec4c:
              FUN_080099a4(piVar3,uVar11);
            }
          }
          uVar14 = DAT_0800ee48;
          if ((uVar16 & 8) != 0) {
            iVar13 = *piVar3;
            *(undefined4 *)(iVar4 + 8) = 8;
            iVar7 = iVar13 + 0xb00 + uVar12 * 0x20;
            if (uVar14 < *(uint *)(iVar13 + 0x40)) {
              if (*(int *)(iVar7 + 8) << 0x10 < 0) {
                *(undefined4 *)(iVar7 + 8) = 0x8000;
              }
              FUN_08009998(piVar3);
              if (piVar3[3] == 1) {
                FUN_08012c00(*piVar3,1,piVar3 + 0x131);
              }
            }
            else {
              FUN_08009998(piVar3);
            }
          }
          if ((uVar16 & 0x10) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x10;
          }
          if ((uVar16 & 2) != 0) {
            if (*(int *)(iVar15 + 0x14) << 0x18 < 0) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x400;
            }
            if (*(char *)((int)piVar10 + 0x27f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x27f) = 0;
              FUN_08009a30(piVar3,uVar11);
            }
            *(undefined4 *)(iVar4 + 8) = 2;
          }
          if ((uVar16 & 0x20) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x20;
          }
          if ((uVar16 & 0x2000) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x2000;
          }
        }
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        iVar4 = iVar4 + 0x20;
        piVar10 = piVar10 + 9;
      } while (uVar5 != 0);
    }
    iVar4 = FUN_08012b60(*piVar3);
    if ((iVar4 << 0xd < 0) && (uVar5 = FUN_08012b88(*piVar3), uVar5 != 0)) {
      iVar13 = iVar15 + 0x900;
      iVar4 = *piVar3;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((uVar5 & 1) != 0) {
          uVar16 = uVar12 & 0xff;
          iVar4 = FUN_08012bac(iVar4,uVar16);
          if (iVar4 << 0x1f < 0) {
            *(uint *)(iVar15 + 0x834) = *(uint *)(iVar15 + 0x834) & ~(1 << (uVar12 & 0xf));
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar13 + 8) = 1;
            if (((iVar7 == 1) && (piVar10[0x12] = piVar10[0x12] + piVar10[0x11], uVar12 == 0)) &&
               (piVar3[0x13] == 0)) {
              FUN_08012c00(*piVar3,1,piVar3 + 0x131);
            }
            FUN_080099b8(piVar3,uVar16);
          }
          if (iVar4 << 0x1c < 0) {
            *(undefined4 *)(iVar13 + 8) = 8;
          }
          if (iVar4 << 0x1b < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x10;
          }
          if (iVar4 << 0x19 < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x40;
          }
          if (iVar4 << 0x1e < 0) {
            FUN_080125b4(iVar15,uVar12);
            if (*(char *)((int)piVar10 + 0x3f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x3f) = 0;
              FUN_08009a38(piVar3,uVar16);
            }
            *(undefined4 *)(iVar13 + 8) = 2;
          }
          if (iVar4 << 0x18 < 0) {
            uVar14 = piVar10[0x13];
            uVar11 = piVar10[0x14];
            iVar7 = *piVar3;
            iVar4 = iVar7;
            if (uVar11 <= uVar14) {
              iVar9 = iVar7 + 0x900 + uVar12 * 0x20;
              uVar18 = uVar14 - uVar11;
              if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                uVar18 = piVar10[0x11];
              }
              if ((*(uint *)(iVar9 + 0x18) & 0xffff) < uVar18 + 3 >> 2) {
LAB_0800edb0:
                if (uVar11 < uVar14) goto LAB_0800ea6a;
              }
              else {
                while (uVar11 < uVar14) {
                  uVar18 = uVar14 - uVar11;
                  if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                    uVar18 = piVar10[0x11];
                  }
                  FUN_08012a08(iVar7,piVar10[0x12],uVar16,uVar18 & 0xffff,*(undefined *)(piVar3 + 3)
                              );
                  uVar14 = *(uint *)(iVar9 + 0x18);
                  piVar10[0x12] = piVar10[0x12] + uVar18;
                  uVar11 = piVar10[0x14] + uVar18;
                  piVar10[0x14] = uVar11;
                  if ((uVar14 & 0xffff) < uVar18 + 3 >> 2) {
                    iVar4 = *piVar3;
                    uVar14 = piVar10[0x13];
                    goto LAB_0800edb0;
                  }
                  uVar14 = piVar10[0x13];
                }
                iVar4 = *piVar3;
              }
              *(uint *)(iVar7 + 0x834) = *(uint *)(iVar7 + 0x834) & ~(1 << (uVar12 & 0xf));
            }
          }
          else {
            iVar4 = *piVar3;
          }
        }
LAB_0800ea6a:
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        piVar10 = piVar10 + 9;
        iVar13 = iVar13 + 0x20;
      } while (uVar5 != 0);
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 < 0) {
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      if (*(char *)(piVar3 + 0x13d) == '\x01') {
        *(undefined *)(piVar3 + 0x13d) = 0;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_08009a28(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x80000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x14 < 0) {
      if (*(int *)(iVar15 + 0x808) << 0x1f < 0) {
        FUN_080099f8(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x800;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 4 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x8000000;
      if (*(char *)(piVar3 + 0x13d) == '\0') {
        *(undefined *)(piVar3 + 0x13d) = 1;
        piVar3[0x13e] = (uint)(*(int *)(iVar4 + 0x54) << 0x1a) >> 0x1c;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_080099f8(piVar3);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0x13 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      FUN_080125b4(iVar4,0x10);
      iVar4 = piVar3[1];
      if (iVar4 != 0) {
        iVar13 = 0;
        puVar8 = (uint *)(iVar15 + 0x900);
        do {
          puVar8[2] = 0xfb7f;
          iVar13 = iVar13 + 1;
          *puVar8 = *puVar8 & 0xffdfffff;
          puVar8[0x82] = 0xfb7f;
          puVar8[0x80] = puVar8[0x80] & 0xffdfffff;
          puVar8[0x80] = puVar8[0x80] | 0x8000000;
          puVar8 = puVar8 + 8;
        } while (iVar13 != iVar4);
      }
      iVar4 = piVar3[0xc];
      *(uint *)(iVar15 + 0x81c) = *(uint *)(iVar15 + 0x81c) | 0x10001;
      if (iVar4 == 0) {
        *(uint *)(iVar15 + 0x814) = *(uint *)(iVar15 + 0x814) | 0x202b;
        *(uint *)(iVar15 + 0x810) = *(uint *)(iVar15 + 0x810) | 0xb;
      }
      else {
        *(uint *)(iVar15 + 0x884) = *(uint *)(iVar15 + 0x884) | 0xb;
        *(uint *)(iVar15 + 0x844) = *(uint *)(iVar15 + 0x844) | 0xb;
      }
      uVar2 = *(undefined *)(piVar3 + 3);
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x800) = *(uint *)(iVar15 + 0x800) & 0xfffff80f;
      FUN_08012c00(iVar4,uVar2,piVar3 + 0x131);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x1000;
    }
    else {
      iVar4 = *piVar3;
    }
    uVar5 = FUN_08012b60(iVar4);
    if ((uVar5 & 0x2000) != 0) {
      FUN_08012bd4(*piVar3);
      iVar4 = FUN_08012658(*piVar3);
      iVar13 = *piVar3;
      piVar3[4] = iVar4;
      uVar6 = FUN_08010388();
      FUN_080124c0(iVar13,uVar6,*(undefined *)(piVar3 + 4));
      FUN_080099d4(piVar3);
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 0x2000;
    }
    iVar4 = FUN_08012b60();
    if (iVar4 << 0x1c < 0) {
      FUN_080099cc(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 8;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x18 < 0) {
      uVar5 = piVar3[1];
      *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) & 0xffffff7f;
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          pcVar1 = (char *)((int)piVar10 + 0x2a3);
          piVar10 = piVar10 + 9;
          if (*pcVar1 == '\x01') {
            FUN_08012980(*piVar3,piVar3 + uVar12 * 9 + 0x9f);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
        } while (uVar12 < uVar5);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0xb < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        piVar17 = (int *)(iVar15 + 0x920);
        do {
          if ((*(char *)(piVar10 + 0x19) == '\x01') && (*piVar17 < 0)) {
            *(undefined *)((int)piVar10 + 99) = 1;
            FUN_08012980(*piVar3,piVar3 + (short)((ushort)uVar12 & 0xf) * 9 + 0xf);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
          piVar10 = piVar10 + 9;
          piVar17 = piVar17 + 8;
        } while (uVar12 < uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x100000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 10 < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        puVar8 = (uint *)(iVar15 + 0xb20);
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          uVar12 = uVar12 + 1;
          uVar16 = *puVar8;
          puVar8 = puVar8 + 8;
          if (((*(char *)(piVar10 + 0xa9) == '\x01') && ((int)uVar16 < 0)) &&
             ((uVar16 & 0x10000) == (piVar3[0x13f] & 1U))) {
            *(undefined *)((int)piVar10 + 0x2a3) = 1;
            *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) | 0x80;
            if (-1 < *(int *)(iVar15 + 0x14) << 0x18) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x200;
              break;
            }
          }
          piVar10 = piVar10 + 9;
        } while (uVar12 != uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x200000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 1 < 0) {
      FUN_08009a40(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x40000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x1d < 0) {
      iVar4 = *piVar3;
      uVar5 = *(uint *)(iVar4 + 4);
      if ((int)(uVar5 << 0x1d) < 0) {
        FUN_08009a48(piVar3);
        iVar4 = *piVar3;
      }
      *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) | uVar5;
    }
  }
  return;
}



/* === 080093e4 FUN_080093e4 === */

void FUN_080093e4(void)

{
  char *pcVar1;
  undefined uVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint *puVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int *piVar17;
  uint uVar18;
  
  if (*DAT_08009400 != 0) {
    FUN_0800c498();
  }
  piVar3 = DAT_08009404;
  if (*DAT_08009404 == 0) {
    return;
  }
  iVar15 = *DAT_08009404;
  iVar4 = FUN_08012bcc(iVar15);
  if ((iVar4 == 0) && (iVar4 = FUN_08012b60(*piVar3), iVar4 != 0)) {
    piVar3[0x13f] = (uint)(*(int *)(iVar15 + 0x808) << 10) >> 0x12;
    uVar5 = FUN_08012b60(*piVar3);
    if ((uVar5 & 2) != 0) {
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 2;
    }
    uVar5 = FUN_08012b60();
    iVar4 = *piVar3;
    if ((uVar5 & 0x10) != 0) {
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) & 0xffffffef;
      uVar12 = *(uint *)(iVar15 + 0x20);
      uVar5 = (uVar12 << 0xb) >> 0x1c;
      uVar16 = uVar12 & 0xf;
      if (uVar5 == 2) {
        if ((uVar12 & 0x7ff0) != 0) {
          uVar5 = (uVar12 << 0x11) >> 0x15;
          FUN_08012a34(iVar15,piVar3[uVar16 * 9 + 0xa2]);
          iVar4 = *piVar3;
          piVar3[uVar16 * 9 + 0xa2] = piVar3[uVar16 * 9 + 0xa2] + uVar5;
          piVar3[uVar16 * 9 + 0xa4] = piVar3[uVar16 * 9 + 0xa4] + uVar5;
        }
      }
      else if (uVar5 == 6) {
        FUN_08012a34(iVar15,piVar3 + 0x131,8);
        iVar4 = *piVar3;
        piVar3[uVar16 * 9 + 0xa4] = ((uVar12 << 0x11) >> 0x15) + piVar3[uVar16 * 9 + 0xa4];
      }
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) | 0x10;
    }
    iVar4 = FUN_08012b60();
    if ((iVar4 << 0xc < 0) && (uVar5 = FUN_08012b78(*piVar3), uVar5 != 0)) {
      iVar4 = iVar15 + 0xb00;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((int)(uVar5 << 0x1f) < 0) {
          uVar11 = uVar12 & 0xff;
          uVar16 = FUN_08012b98(*piVar3,uVar11);
          if ((uVar16 & 1) != 0) {
            iVar13 = *piVar3;
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar4 + 8) = 1;
            uVar18 = *(uint *)(iVar13 + 0x40);
            iVar9 = iVar13 + 0xb00 + uVar12 * 0x20;
            uVar14 = *(uint *)(iVar9 + 8);
            if (iVar7 == 1) {
              if ((int)(uVar14 << 0x1c) < 0) {
                if ((DAT_0800ee48 < uVar18) && ((int)(uVar14 << 0x10) < 0)) {
LAB_0800ed9a:
                  *(undefined4 *)(iVar9 + 8) = 0x8000;
                }
              }
              else if ((int)(uVar14 << 0x1a) < 0) {
                *(undefined4 *)(iVar9 + 8) = 0x20;
              }
              else if ((uVar14 & 0x28) == 0) {
                if ((uVar18 <= DAT_0800ee48) || (-1 < (int)(uVar14 << 0x10))) {
                  iVar7 = piVar10[0xa7] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                  piVar10[0xa4] = iVar7;
                  if (uVar12 == 0) {
                    if (piVar3[0xa3] == 0) {
                      FUN_08012c00(iVar13,1,piVar3 + 0x131);
                    }
                    else {
                      piVar3[0xa2] = iVar7 + piVar3[0xa2];
                    }
                  }
                  goto LAB_0800ec4c;
                }
                goto LAB_0800ed9a;
              }
            }
            else {
              if (uVar18 == DAT_0800ee4c) {
                if ((int)(uVar14 << 0x10) < 0) goto LAB_0800ed9a;
                if ((int)(uVar14 << 0x1a) < 0) {
                  *(undefined4 *)(iVar9 + 8) = 0x20;
                }
              }
              else if ((uVar12 == 0) && (piVar3[0xa3] == 0)) {
                FUN_08012c00(iVar13,0,piVar3 + 0x131);
              }
LAB_0800ec4c:
              FUN_080099a4(piVar3,uVar11);
            }
          }
          uVar14 = DAT_0800ee48;
          if ((uVar16 & 8) != 0) {
            iVar13 = *piVar3;
            *(undefined4 *)(iVar4 + 8) = 8;
            iVar7 = iVar13 + 0xb00 + uVar12 * 0x20;
            if (uVar14 < *(uint *)(iVar13 + 0x40)) {
              if (*(int *)(iVar7 + 8) << 0x10 < 0) {
                *(undefined4 *)(iVar7 + 8) = 0x8000;
              }
              FUN_08009998(piVar3);
              if (piVar3[3] == 1) {
                FUN_08012c00(*piVar3,1,piVar3 + 0x131);
              }
            }
            else {
              FUN_08009998(piVar3);
            }
          }
          if ((uVar16 & 0x10) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x10;
          }
          if ((uVar16 & 2) != 0) {
            if (*(int *)(iVar15 + 0x14) << 0x18 < 0) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x400;
            }
            if (*(char *)((int)piVar10 + 0x27f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x27f) = 0;
              FUN_08009a30(piVar3,uVar11);
            }
            *(undefined4 *)(iVar4 + 8) = 2;
          }
          if ((uVar16 & 0x20) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x20;
          }
          if ((uVar16 & 0x2000) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x2000;
          }
        }
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        iVar4 = iVar4 + 0x20;
        piVar10 = piVar10 + 9;
      } while (uVar5 != 0);
    }
    iVar4 = FUN_08012b60(*piVar3);
    if ((iVar4 << 0xd < 0) && (uVar5 = FUN_08012b88(*piVar3), uVar5 != 0)) {
      iVar13 = iVar15 + 0x900;
      iVar4 = *piVar3;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((uVar5 & 1) != 0) {
          uVar16 = uVar12 & 0xff;
          iVar4 = FUN_08012bac(iVar4,uVar16);
          if (iVar4 << 0x1f < 0) {
            *(uint *)(iVar15 + 0x834) = *(uint *)(iVar15 + 0x834) & ~(1 << (uVar12 & 0xf));
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar13 + 8) = 1;
            if (((iVar7 == 1) && (piVar10[0x12] = piVar10[0x12] + piVar10[0x11], uVar12 == 0)) &&
               (piVar3[0x13] == 0)) {
              FUN_08012c00(*piVar3,1,piVar3 + 0x131);
            }
            FUN_080099b8(piVar3,uVar16);
          }
          if (iVar4 << 0x1c < 0) {
            *(undefined4 *)(iVar13 + 8) = 8;
          }
          if (iVar4 << 0x1b < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x10;
          }
          if (iVar4 << 0x19 < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x40;
          }
          if (iVar4 << 0x1e < 0) {
            FUN_080125b4(iVar15,uVar12);
            if (*(char *)((int)piVar10 + 0x3f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x3f) = 0;
              FUN_08009a38(piVar3,uVar16);
            }
            *(undefined4 *)(iVar13 + 8) = 2;
          }
          if (iVar4 << 0x18 < 0) {
            uVar14 = piVar10[0x13];
            uVar11 = piVar10[0x14];
            iVar7 = *piVar3;
            iVar4 = iVar7;
            if (uVar11 <= uVar14) {
              iVar9 = iVar7 + 0x900 + uVar12 * 0x20;
              uVar18 = uVar14 - uVar11;
              if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                uVar18 = piVar10[0x11];
              }
              if ((*(uint *)(iVar9 + 0x18) & 0xffff) < uVar18 + 3 >> 2) {
LAB_0800edb0:
                if (uVar11 < uVar14) goto LAB_0800ea6a;
              }
              else {
                while (uVar11 < uVar14) {
                  uVar18 = uVar14 - uVar11;
                  if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                    uVar18 = piVar10[0x11];
                  }
                  FUN_08012a08(iVar7,piVar10[0x12],uVar16,uVar18 & 0xffff,*(undefined *)(piVar3 + 3)
                              );
                  uVar14 = *(uint *)(iVar9 + 0x18);
                  piVar10[0x12] = piVar10[0x12] + uVar18;
                  uVar11 = piVar10[0x14] + uVar18;
                  piVar10[0x14] = uVar11;
                  if ((uVar14 & 0xffff) < uVar18 + 3 >> 2) {
                    iVar4 = *piVar3;
                    uVar14 = piVar10[0x13];
                    goto LAB_0800edb0;
                  }
                  uVar14 = piVar10[0x13];
                }
                iVar4 = *piVar3;
              }
              *(uint *)(iVar7 + 0x834) = *(uint *)(iVar7 + 0x834) & ~(1 << (uVar12 & 0xf));
            }
          }
          else {
            iVar4 = *piVar3;
          }
        }
LAB_0800ea6a:
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        piVar10 = piVar10 + 9;
        iVar13 = iVar13 + 0x20;
      } while (uVar5 != 0);
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 < 0) {
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      if (*(char *)(piVar3 + 0x13d) == '\x01') {
        *(undefined *)(piVar3 + 0x13d) = 0;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_08009a28(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x80000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x14 < 0) {
      if (*(int *)(iVar15 + 0x808) << 0x1f < 0) {
        FUN_080099f8(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x800;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 4 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x8000000;
      if (*(char *)(piVar3 + 0x13d) == '\0') {
        *(undefined *)(piVar3 + 0x13d) = 1;
        piVar3[0x13e] = (uint)(*(int *)(iVar4 + 0x54) << 0x1a) >> 0x1c;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_080099f8(piVar3);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0x13 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      FUN_080125b4(iVar4,0x10);
      iVar4 = piVar3[1];
      if (iVar4 != 0) {
        iVar13 = 0;
        puVar8 = (uint *)(iVar15 + 0x900);
        do {
          puVar8[2] = 0xfb7f;
          iVar13 = iVar13 + 1;
          *puVar8 = *puVar8 & 0xffdfffff;
          puVar8[0x82] = 0xfb7f;
          puVar8[0x80] = puVar8[0x80] & 0xffdfffff;
          puVar8[0x80] = puVar8[0x80] | 0x8000000;
          puVar8 = puVar8 + 8;
        } while (iVar13 != iVar4);
      }
      iVar4 = piVar3[0xc];
      *(uint *)(iVar15 + 0x81c) = *(uint *)(iVar15 + 0x81c) | 0x10001;
      if (iVar4 == 0) {
        *(uint *)(iVar15 + 0x814) = *(uint *)(iVar15 + 0x814) | 0x202b;
        *(uint *)(iVar15 + 0x810) = *(uint *)(iVar15 + 0x810) | 0xb;
      }
      else {
        *(uint *)(iVar15 + 0x884) = *(uint *)(iVar15 + 0x884) | 0xb;
        *(uint *)(iVar15 + 0x844) = *(uint *)(iVar15 + 0x844) | 0xb;
      }
      uVar2 = *(undefined *)(piVar3 + 3);
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x800) = *(uint *)(iVar15 + 0x800) & 0xfffff80f;
      FUN_08012c00(iVar4,uVar2,piVar3 + 0x131);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x1000;
    }
    else {
      iVar4 = *piVar3;
    }
    uVar5 = FUN_08012b60(iVar4);
    if ((uVar5 & 0x2000) != 0) {
      FUN_08012bd4(*piVar3);
      iVar4 = FUN_08012658(*piVar3);
      iVar13 = *piVar3;
      piVar3[4] = iVar4;
      uVar6 = FUN_08010388();
      FUN_080124c0(iVar13,uVar6,*(undefined *)(piVar3 + 4));
      FUN_080099d4(piVar3);
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 0x2000;
    }
    iVar4 = FUN_08012b60();
    if (iVar4 << 0x1c < 0) {
      FUN_080099cc(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 8;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x18 < 0) {
      uVar5 = piVar3[1];
      *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) & 0xffffff7f;
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          pcVar1 = (char *)((int)piVar10 + 0x2a3);
          piVar10 = piVar10 + 9;
          if (*pcVar1 == '\x01') {
            FUN_08012980(*piVar3,piVar3 + uVar12 * 9 + 0x9f);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
        } while (uVar12 < uVar5);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0xb < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        piVar17 = (int *)(iVar15 + 0x920);
        do {
          if ((*(char *)(piVar10 + 0x19) == '\x01') && (*piVar17 < 0)) {
            *(undefined *)((int)piVar10 + 99) = 1;
            FUN_08012980(*piVar3,piVar3 + (short)((ushort)uVar12 & 0xf) * 9 + 0xf);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
          piVar10 = piVar10 + 9;
          piVar17 = piVar17 + 8;
        } while (uVar12 < uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x100000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 10 < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        puVar8 = (uint *)(iVar15 + 0xb20);
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          uVar12 = uVar12 + 1;
          uVar16 = *puVar8;
          puVar8 = puVar8 + 8;
          if (((*(char *)(piVar10 + 0xa9) == '\x01') && ((int)uVar16 < 0)) &&
             ((uVar16 & 0x10000) == (piVar3[0x13f] & 1U))) {
            *(undefined *)((int)piVar10 + 0x2a3) = 1;
            *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) | 0x80;
            if (-1 < *(int *)(iVar15 + 0x14) << 0x18) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x200;
              break;
            }
          }
          piVar10 = piVar10 + 9;
        } while (uVar12 != uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x200000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 1 < 0) {
      FUN_08009a40(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x40000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x1d < 0) {
      iVar4 = *piVar3;
      uVar5 = *(uint *)(iVar4 + 4);
      if ((int)(uVar5 << 0x1d) < 0) {
        FUN_08009a48(piVar3);
        iVar4 = *piVar3;
      }
      *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) | uVar5;
    }
  }
  return;
}



/* === 08009408 FUN_08009408 === */

void FUN_08009408(void)

{
  char *pcVar1;
  undefined uVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  uint *puVar8;
  int iVar9;
  int *piVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  int iVar15;
  uint uVar16;
  int *piVar17;
  uint uVar18;
  
  if (*DAT_08009424 != 0) {
    FUN_0800c498();
  }
  piVar3 = DAT_08009428;
  if (*DAT_08009428 == 0) {
    return;
  }
  iVar15 = *DAT_08009428;
  iVar4 = FUN_08012bcc(iVar15);
  if ((iVar4 == 0) && (iVar4 = FUN_08012b60(*piVar3), iVar4 != 0)) {
    piVar3[0x13f] = (uint)(*(int *)(iVar15 + 0x808) << 10) >> 0x12;
    uVar5 = FUN_08012b60(*piVar3);
    if ((uVar5 & 2) != 0) {
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 2;
    }
    uVar5 = FUN_08012b60();
    iVar4 = *piVar3;
    if ((uVar5 & 0x10) != 0) {
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) & 0xffffffef;
      uVar12 = *(uint *)(iVar15 + 0x20);
      uVar5 = (uVar12 << 0xb) >> 0x1c;
      uVar16 = uVar12 & 0xf;
      if (uVar5 == 2) {
        if ((uVar12 & 0x7ff0) != 0) {
          uVar5 = (uVar12 << 0x11) >> 0x15;
          FUN_08012a34(iVar15,piVar3[uVar16 * 9 + 0xa2]);
          iVar4 = *piVar3;
          piVar3[uVar16 * 9 + 0xa2] = piVar3[uVar16 * 9 + 0xa2] + uVar5;
          piVar3[uVar16 * 9 + 0xa4] = piVar3[uVar16 * 9 + 0xa4] + uVar5;
        }
      }
      else if (uVar5 == 6) {
        FUN_08012a34(iVar15,piVar3 + 0x131,8);
        iVar4 = *piVar3;
        piVar3[uVar16 * 9 + 0xa4] = ((uVar12 << 0x11) >> 0x15) + piVar3[uVar16 * 9 + 0xa4];
      }
      *(uint *)(iVar4 + 0x18) = *(uint *)(iVar4 + 0x18) | 0x10;
    }
    iVar4 = FUN_08012b60();
    if ((iVar4 << 0xc < 0) && (uVar5 = FUN_08012b78(*piVar3), uVar5 != 0)) {
      iVar4 = iVar15 + 0xb00;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((int)(uVar5 << 0x1f) < 0) {
          uVar11 = uVar12 & 0xff;
          uVar16 = FUN_08012b98(*piVar3,uVar11);
          if ((uVar16 & 1) != 0) {
            iVar13 = *piVar3;
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar4 + 8) = 1;
            uVar18 = *(uint *)(iVar13 + 0x40);
            iVar9 = iVar13 + 0xb00 + uVar12 * 0x20;
            uVar14 = *(uint *)(iVar9 + 8);
            if (iVar7 == 1) {
              if ((int)(uVar14 << 0x1c) < 0) {
                if ((DAT_0800ee48 < uVar18) && ((int)(uVar14 << 0x10) < 0)) {
LAB_0800ed9a:
                  *(undefined4 *)(iVar9 + 8) = 0x8000;
                }
              }
              else if ((int)(uVar14 << 0x1a) < 0) {
                *(undefined4 *)(iVar9 + 8) = 0x20;
              }
              else if ((uVar14 & 0x28) == 0) {
                if ((uVar18 <= DAT_0800ee48) || (-1 < (int)(uVar14 << 0x10))) {
                  iVar7 = piVar10[0xa7] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                  piVar10[0xa4] = iVar7;
                  if (uVar12 == 0) {
                    if (piVar3[0xa3] == 0) {
                      FUN_08012c00(iVar13,1,piVar3 + 0x131);
                    }
                    else {
                      piVar3[0xa2] = iVar7 + piVar3[0xa2];
                    }
                  }
                  goto LAB_0800ec4c;
                }
                goto LAB_0800ed9a;
              }
            }
            else {
              if (uVar18 == DAT_0800ee4c) {
                if ((int)(uVar14 << 0x10) < 0) goto LAB_0800ed9a;
                if ((int)(uVar14 << 0x1a) < 0) {
                  *(undefined4 *)(iVar9 + 8) = 0x20;
                }
              }
              else if ((uVar12 == 0) && (piVar3[0xa3] == 0)) {
                FUN_08012c00(iVar13,0,piVar3 + 0x131);
              }
LAB_0800ec4c:
              FUN_080099a4(piVar3,uVar11);
            }
          }
          uVar14 = DAT_0800ee48;
          if ((uVar16 & 8) != 0) {
            iVar13 = *piVar3;
            *(undefined4 *)(iVar4 + 8) = 8;
            iVar7 = iVar13 + 0xb00 + uVar12 * 0x20;
            if (uVar14 < *(uint *)(iVar13 + 0x40)) {
              if (*(int *)(iVar7 + 8) << 0x10 < 0) {
                *(undefined4 *)(iVar7 + 8) = 0x8000;
              }
              FUN_08009998(piVar3);
              if (piVar3[3] == 1) {
                FUN_08012c00(*piVar3,1,piVar3 + 0x131);
              }
            }
            else {
              FUN_08009998(piVar3);
            }
          }
          if ((uVar16 & 0x10) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x10;
          }
          if ((uVar16 & 2) != 0) {
            if (*(int *)(iVar15 + 0x14) << 0x18 < 0) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x400;
            }
            if (*(char *)((int)piVar10 + 0x27f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x27f) = 0;
              FUN_08009a30(piVar3,uVar11);
            }
            *(undefined4 *)(iVar4 + 8) = 2;
          }
          if ((uVar16 & 0x20) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x20;
          }
          if ((uVar16 & 0x2000) != 0) {
            *(undefined4 *)(iVar4 + 8) = 0x2000;
          }
        }
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        iVar4 = iVar4 + 0x20;
        piVar10 = piVar10 + 9;
      } while (uVar5 != 0);
    }
    iVar4 = FUN_08012b60(*piVar3);
    if ((iVar4 << 0xd < 0) && (uVar5 = FUN_08012b88(*piVar3), uVar5 != 0)) {
      iVar13 = iVar15 + 0x900;
      iVar4 = *piVar3;
      uVar12 = 0;
      piVar10 = piVar3;
      do {
        if ((uVar5 & 1) != 0) {
          uVar16 = uVar12 & 0xff;
          iVar4 = FUN_08012bac(iVar4,uVar16);
          if (iVar4 << 0x1f < 0) {
            *(uint *)(iVar15 + 0x834) = *(uint *)(iVar15 + 0x834) & ~(1 << (uVar12 & 0xf));
            iVar7 = piVar3[3];
            *(undefined4 *)(iVar13 + 8) = 1;
            if (((iVar7 == 1) && (piVar10[0x12] = piVar10[0x12] + piVar10[0x11], uVar12 == 0)) &&
               (piVar3[0x13] == 0)) {
              FUN_08012c00(*piVar3,1,piVar3 + 0x131);
            }
            FUN_080099b8(piVar3,uVar16);
          }
          if (iVar4 << 0x1c < 0) {
            *(undefined4 *)(iVar13 + 8) = 8;
          }
          if (iVar4 << 0x1b < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x10;
          }
          if (iVar4 << 0x19 < 0) {
            *(undefined4 *)(iVar13 + 8) = 0x40;
          }
          if (iVar4 << 0x1e < 0) {
            FUN_080125b4(iVar15,uVar12);
            if (*(char *)((int)piVar10 + 0x3f) == '\x01') {
              *(undefined *)((int)piVar10 + 0x3f) = 0;
              FUN_08009a38(piVar3,uVar16);
            }
            *(undefined4 *)(iVar13 + 8) = 2;
          }
          if (iVar4 << 0x18 < 0) {
            uVar14 = piVar10[0x13];
            uVar11 = piVar10[0x14];
            iVar7 = *piVar3;
            iVar4 = iVar7;
            if (uVar11 <= uVar14) {
              iVar9 = iVar7 + 0x900 + uVar12 * 0x20;
              uVar18 = uVar14 - uVar11;
              if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                uVar18 = piVar10[0x11];
              }
              if ((*(uint *)(iVar9 + 0x18) & 0xffff) < uVar18 + 3 >> 2) {
LAB_0800edb0:
                if (uVar11 < uVar14) goto LAB_0800ea6a;
              }
              else {
                while (uVar11 < uVar14) {
                  uVar18 = uVar14 - uVar11;
                  if ((uint)piVar10[0x11] <= uVar14 - uVar11) {
                    uVar18 = piVar10[0x11];
                  }
                  FUN_08012a08(iVar7,piVar10[0x12],uVar16,uVar18 & 0xffff,*(undefined *)(piVar3 + 3)
                              );
                  uVar14 = *(uint *)(iVar9 + 0x18);
                  piVar10[0x12] = piVar10[0x12] + uVar18;
                  uVar11 = piVar10[0x14] + uVar18;
                  piVar10[0x14] = uVar11;
                  if ((uVar14 & 0xffff) < uVar18 + 3 >> 2) {
                    iVar4 = *piVar3;
                    uVar14 = piVar10[0x13];
                    goto LAB_0800edb0;
                  }
                  uVar14 = piVar10[0x13];
                }
                iVar4 = *piVar3;
              }
              *(uint *)(iVar7 + 0x834) = *(uint *)(iVar7 + 0x834) & ~(1 << (uVar12 & 0xf));
            }
          }
          else {
            iVar4 = *piVar3;
          }
        }
LAB_0800ea6a:
        uVar5 = uVar5 >> 1;
        uVar12 = uVar12 + 1;
        piVar10 = piVar10 + 9;
        iVar13 = iVar13 + 0x20;
      } while (uVar5 != 0);
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 < 0) {
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      if (*(char *)(piVar3 + 0x13d) == '\x01') {
        *(undefined *)(piVar3 + 0x13d) = 0;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_08009a28(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x80000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x14 < 0) {
      if (*(int *)(iVar15 + 0x808) << 0x1f < 0) {
        FUN_080099f8(piVar3);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x800;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 4 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x8000000;
      if (*(char *)(piVar3 + 0x13d) == '\0') {
        *(undefined *)(piVar3 + 0x13d) = 1;
        piVar3[0x13e] = (uint)(*(int *)(iVar4 + 0x54) << 0x1a) >> 0x1c;
        FUN_0800f080(piVar3);
      }
      else {
        FUN_080099f8(piVar3);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0x13 < 0) {
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) & 0xfffffffe;
      FUN_080125b4(iVar4,0x10);
      iVar4 = piVar3[1];
      if (iVar4 != 0) {
        iVar13 = 0;
        puVar8 = (uint *)(iVar15 + 0x900);
        do {
          puVar8[2] = 0xfb7f;
          iVar13 = iVar13 + 1;
          *puVar8 = *puVar8 & 0xffdfffff;
          puVar8[0x82] = 0xfb7f;
          puVar8[0x80] = puVar8[0x80] & 0xffdfffff;
          puVar8[0x80] = puVar8[0x80] | 0x8000000;
          puVar8 = puVar8 + 8;
        } while (iVar13 != iVar4);
      }
      iVar4 = piVar3[0xc];
      *(uint *)(iVar15 + 0x81c) = *(uint *)(iVar15 + 0x81c) | 0x10001;
      if (iVar4 == 0) {
        *(uint *)(iVar15 + 0x814) = *(uint *)(iVar15 + 0x814) | 0x202b;
        *(uint *)(iVar15 + 0x810) = *(uint *)(iVar15 + 0x810) | 0xb;
      }
      else {
        *(uint *)(iVar15 + 0x884) = *(uint *)(iVar15 + 0x884) | 0xb;
        *(uint *)(iVar15 + 0x844) = *(uint *)(iVar15 + 0x844) | 0xb;
      }
      uVar2 = *(undefined *)(piVar3 + 3);
      iVar4 = *piVar3;
      *(uint *)(iVar15 + 0x800) = *(uint *)(iVar15 + 0x800) & 0xfffff80f;
      FUN_08012c00(iVar4,uVar2,piVar3 + 0x131);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x1000;
    }
    else {
      iVar4 = *piVar3;
    }
    uVar5 = FUN_08012b60(iVar4);
    if ((uVar5 & 0x2000) != 0) {
      FUN_08012bd4(*piVar3);
      iVar4 = FUN_08012658(*piVar3);
      iVar13 = *piVar3;
      piVar3[4] = iVar4;
      uVar6 = FUN_08010388();
      FUN_080124c0(iVar13,uVar6,*(undefined *)(piVar3 + 4));
      FUN_080099d4(piVar3);
      *(uint *)(*piVar3 + 0x14) = *(uint *)(*piVar3 + 0x14) & 0x2000;
    }
    iVar4 = FUN_08012b60();
    if (iVar4 << 0x1c < 0) {
      FUN_080099cc(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 8;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x18 < 0) {
      uVar5 = piVar3[1];
      *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) & 0xffffff7f;
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          pcVar1 = (char *)((int)piVar10 + 0x2a3);
          piVar10 = piVar10 + 9;
          if (*pcVar1 == '\x01') {
            FUN_08012980(*piVar3,piVar3 + uVar12 * 9 + 0x9f);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
        } while (uVar12 < uVar5);
      }
    }
    iVar4 = FUN_08012b60(*piVar3);
    if (iVar4 << 0xb < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        uVar12 = 1;
        piVar10 = piVar3;
        piVar17 = (int *)(iVar15 + 0x920);
        do {
          if ((*(char *)(piVar10 + 0x19) == '\x01') && (*piVar17 < 0)) {
            *(undefined *)((int)piVar10 + 99) = 1;
            FUN_08012980(*piVar3,piVar3 + (short)((ushort)uVar12 & 0xf) * 9 + 0xf);
            uVar5 = piVar3[1];
          }
          uVar12 = uVar12 + 1;
          piVar10 = piVar10 + 9;
          piVar17 = piVar17 + 8;
        } while (uVar12 < uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x100000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 10 < 0) {
      uVar5 = piVar3[1];
      if (1 < uVar5) {
        puVar8 = (uint *)(iVar15 + 0xb20);
        uVar12 = 1;
        piVar10 = piVar3;
        do {
          uVar12 = uVar12 + 1;
          uVar16 = *puVar8;
          puVar8 = puVar8 + 8;
          if (((*(char *)(piVar10 + 0xa9) == '\x01') && ((int)uVar16 < 0)) &&
             ((uVar16 & 0x10000) == (piVar3[0x13f] & 1U))) {
            *(undefined *)((int)piVar10 + 0x2a3) = 1;
            *(uint *)(iVar15 + 0x18) = *(uint *)(iVar15 + 0x18) | 0x80;
            if (-1 < *(int *)(iVar15 + 0x14) << 0x18) {
              *(uint *)(iVar15 + 0x804) = *(uint *)(iVar15 + 0x804) | 0x200;
              break;
            }
          }
          piVar10 = piVar10 + 9;
        } while (uVar12 != uVar5);
      }
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x200000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 1 < 0) {
      FUN_08009a40(piVar3);
      iVar4 = *piVar3;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0x40000000;
    }
    else {
      iVar4 = *piVar3;
    }
    iVar4 = FUN_08012b60(iVar4);
    if (iVar4 << 0x1d < 0) {
      iVar4 = *piVar3;
      uVar5 = *(uint *)(iVar4 + 4);
      if ((int)(uVar5 << 0x1d) < 0) {
        FUN_08009a48(piVar3);
        iVar4 = *piVar3;
      }
      *(uint *)(iVar4 + 4) = *(uint *)(iVar4 + 4) | uVar5;
    }
  }
  return;
}



/* === 0800942c FUN_0800942c === */

void FUN_0800942c(void)

{
  if (*(int *)(DAT_08009508 + 0x2c) << 1 < 0) {
    if (*(int *)(DAT_08009508 + 0x28) << 0x18 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x1a < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x1b < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x1c < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x1e < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x1f < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x10 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x12 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x13 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x14 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x15 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x16 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0x17 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 6 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 7 < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0xc < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0xd < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0xe < 0) {
      software_bkpt(0);
    }
    if (*(int *)(DAT_08009508 + 0x28) << 0xf < 0) {
      software_bkpt(0);
    }
  }
  else if (-1 < *(int *)(DAT_08009508 + 0x2c) << 0x1e) goto LAB_080094f6;
  software_bkpt(0);
LAB_080094f6:
  software_bkpt(0);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 0800950c System_GetNow === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 System_GetNow(void)

{
  return *DAT_08009cf0;
}



/* === 08009510 System_GetTick === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 System_GetTick(void)

{
  return *(undefined4 *)(*(int *)(*DAT_08009518 + 0x10) + 0x24);
}



/* === 0800951c System_Delay === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_Delay(uint param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_08009ce8();
  if (param_1 != 0xffffffff) {
    param_1 = param_1 + *DAT_08009d14;
  }
  do {
    iVar2 = FUN_08009ce8();
  } while ((uint)(iVar2 - iVar1) < param_1);
  return;
}



/* === 08009520 System_GetBootloaderVersion === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int System_GetBootloaderVersion(void)

{
  int iVar1;
  int iVar2;
  bool bVar3;
  
  if (*(int *)(DAT_08009548 + 8) + 0xf8000000U < 0x20000) {
    iVar2 = 1;
  }
  else {
    iVar2 = 0;
    while( true ) {
      iVar1 = iVar2;
      if (*(int *)(DAT_0800954c + 8) == iVar2) break;
      bVar3 = iVar2 == 3;
      iVar2 = iVar2 + 1;
      if (bVar3) {
        return iVar1;
      }
    }
  }
  return iVar2;
}



/* === 08009550 System_ConfigureClocks === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_ConfigureClocks(int *param_1)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_14c;
  undefined4 uStack_148;
  int local_144;
  undefined4 local_140;
  undefined4 local_13c;
  undefined4 uStack_138;
  undefined4 local_134;
  undefined4 local_130;
  undefined4 local_12c;
  undefined4 local_128;
  undefined4 local_114;
  undefined4 local_108;
  undefined4 uStack_104;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 uStack_e4;
  undefined4 local_e0;
  undefined4 uStack_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  int local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  undefined4 uStack_b4;
  undefined4 local_b0;
  undefined4 local_ac;
  undefined4 local_a8;
  undefined4 local_a4;
  int local_a0;
  int local_9c;
  undefined4 uStack_98;
  int local_94;
  undefined4 uStack_90;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  int local_68;
  int iStack_64;
  int local_5c;
  undefined4 local_58;
  undefined4 local_48;
  undefined4 local_3c;
  
  memset(&local_12c,0,0x4c);
  memset(&local_14c,0,0x20);
  memset(&local_e0,0,0xc0);
  FUN_0800f084(2);
  iVar1 = DAT_080096dc;
  iVar2 = DAT_080096d8;
  if (*param_1 == 1) {
    uVar3 = 4;
    local_fc = 0xf0;
    *(uint *)(DAT_080096dc + 0x18) = *(uint *)(DAT_080096dc + 0x18) | 0xc000;
    *(uint *)(iVar2 + 0x2c) = *(uint *)(iVar2 + 0x2c) | 1;
  }
  else {
    uVar3 = 2;
    local_fc = 200;
    *(uint *)(DAT_080096d8 + 0x2c) = *(uint *)(DAT_080096d8 + 0x2c) & 0xfffffffe;
    *(uint *)(iVar1 + 0x18) = *(uint *)(iVar1 + 0x18) | 0xc000;
  }
  do {
  } while (-1 < *(int *)(DAT_080096dc + 0x18) << 0x12);
  *(uint *)(DAT_080096e0 + 0x28) = *(uint *)(DAT_080096e0 + 0x28) & 0xfffffffc | 2;
  local_12c = 0x21;
  local_f8 = 2;
  local_f0 = 2;
  local_128 = 0x10000;
  local_114 = 1;
  local_100 = 4;
  local_f4 = 5;
  local_ec = 8;
  local_108 = 2;
  uStack_104 = 2;
  local_e8 = 0;
  uStack_e4 = 0;
  local_144 = FUN_0800fb00(&local_12c);
  if (local_144 != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_130 = 0x40;
  local_140 = 8;
  local_134 = 0x400;
  local_13c = 0x40;
  uStack_138 = 0x40;
  local_14c = 0x3f;
  uStack_148 = 3;
  local_c0 = FUN_08010138(&local_14c,uVar3);
  if (local_c0 != 0) {
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  local_cc = 2;
  local_88 = 2;
  local_bc = 0x1000;
  local_80 = 0x1000;
  local_58 = 0x300000;
  local_d0 = 8;
  local_ac = 4;
  local_a4 = 0x400;
  local_d8 = 1;
  local_c8 = 1;
  local_3c = 0x10000;
  local_b0 = 0x10;
  uStack_98 = 2;
  uStack_90 = 0x10000;
  local_48 = 0x100;
  local_c4 = 0x80;
  local_84 = 0x80;
  local_b8 = 6;
  uStack_b4 = 0x127;
  local_d4 = 0xc;
  local_a8 = 0x20;
  local_e0 = DAT_080096d0;
  uStack_dc = DAT_080096d4;
  local_a0 = local_c0;
  local_9c = local_c0;
  local_94 = local_c0;
  local_68 = local_c0;
  iStack_64 = local_c0;
  local_5c = local_c0;
  iVar2 = FUN_08010638(&local_e0);
  if (iVar2 == 0) {
    FUN_0800f0cc();
    return;
  }
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 080096e4 System_ConfigureMpu === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_ConfigureMpu(void)

{
  undefined4 uVar1;
  undefined4 uVar2;
  undefined2 local_20 [2];
  undefined4 local_1c;
  undefined4 local_18;
  uint local_14;
  
  FUN_0800aa54();
  local_20[0] = 1;
  local_14 = 0x100;
  local_1c = 0x30000000;
  local_18 = DAT_08009760;
  FUN_0800aa90(local_20);
  uVar1 = local_18;
  local_18 = CONCAT31(local_18._1_3_,0x19);
  uVar2 = local_18;
  local_1c = 0xc0000000;
  local_14._0_2_ = (ushort)local_14 & 0xff;
  local_14 = CONCAT22(0x101,(ushort)local_14);
  local_20[0] = CONCAT11(1,(undefined)local_20[0]);
  local_18._3_1_ = SUB41(uVar1,3);
  local_18._0_2_ = (ushort)uVar2;
  local_18._0_3_ = (uint3)(ushort)local_18;
  FUN_0800aa90(local_20);
  uVar1 = local_18;
  local_14._0_2_ = CONCAT11(1,(undefined)local_14);
  local_20[0] = CONCAT11(2,(undefined)local_20[0]);
  local_18 = CONCAT31(local_18._1_3_,0xb);
  uVar2 = local_18;
  local_1c = 0x38800000;
  local_14 = (uint)(ushort)local_14;
  local_18._3_1_ = SUB41(uVar1,3);
  local_18._0_2_ = (ushort)uVar2;
  local_18._0_3_ = CONCAT12(1,(ushort)local_18);
  FUN_0800aa90(local_20);
  FUN_0800aa70(4);
  return;
}



/* === 08009764 System_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void System_Init(undefined4 *param_1,undefined4 *param_2)

{
  uint uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 local_18;
  undefined local_14;
  
  uVar3 = param_2[1];
  *param_1 = *param_2;
  param_1[1] = uVar3;
  FUN_08009c70();
  if (*(char *)((int)param_2 + 6) == '\0') {
    System_ConfigureClocks(param_1);
    System_ConfigureMpu(param_1);
  }
  FUN_08015370();
  thunk_FUN_0800797c();
  thunk_FUN_080138f4();
  thunk_FUN_08014834();
  iVar2 = DAT_08009850;
  if (*(char *)(param_2 + 1) != '\0') {
    if ((*(uint *)(DAT_08009850 + 0x14) & 0x10000) == 0) {
      *(undefined4 *)(DAT_08009850 + 0x84) = 0;
      DataSynchronizationBarrier(0xf);
      uVar5 = ((uint)(*(int *)(iVar2 + 0x80) << 4) >> 0x11) << 5;
      do {
        uVar4 = (uint)(*(int *)(iVar2 + 0x80) << 0x13) >> 0x16;
        do {
          uVar1 = uVar4 << 0x1e;
          uVar4 = uVar4 - 1;
          *(uint *)(iVar2 + 0x260) = uVar5 & 0x3fe0 | uVar1;
        } while (uVar4 != 0xffffffff);
        uVar5 = uVar5 - 0x20;
      } while (uVar5 != 0xffffffe0);
      DataSynchronizationBarrier(0xf);
      *(uint *)(iVar2 + 0x14) = *(uint *)(iVar2 + 0x14) | 0x10000;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
    }
  }
  iVar2 = DAT_08009850;
  if (*(char *)((int)param_2 + 5) != '\0') {
    if ((*(uint *)(DAT_08009850 + 0x14) & 0x20000) == 0) {
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
      *(undefined4 *)(DAT_08009850 + 0x250) = 0;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
      *(uint *)(iVar2 + 0x14) = *(uint *)(iVar2 + 0x14) | 0x20000;
      DataSynchronizationBarrier(0xf);
      InstructionSynchronizationBarrier(0xf);
    }
  }
  local_18 = 0xffffffff;
  local_14 = 0;
  local_20 = 0;
  uStack_1c = 0;
  FUN_080145fc(DAT_08009854,&local_20);
  FUN_08014614(DAT_08009854);
  FUN_08013768();
  return;
}



/* === 08009858 thunk_FUN_08010408 === */

uint thunk_FUN_08010408(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_08010428 + ((uint)(*(int *)(DAT_08010424 + 0x1c) << 0x19) >> 0x1d))
                  & 0x1f);
}



/* === 0800985c System_GetTickFreq === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

int System_GetTickFreq(void)

{
  int iVar1;
  
  iVar1 = FUN_08010408();
  return iVar1 << 1;
}



/* === 08009868 System_GetProgramMemoryRegion === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 System_GetProgramMemoryRegion(void)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(DAT_080098dc + 8);
  if (uVar2 + 0xf8000000 < 0x20000) {
    return 0;
  }
  if (uVar2 < 0x10000) {
    return 1;
  }
  if (uVar2 + 0xe0000000 < 0x20000) {
    return 2;
  }
  if (uVar2 + 0xdc000000 < 0x80000) {
    return 3;
  }
  if (uVar2 + 0xf0000000 < 0x48000) {
    return 4;
  }
  if (uVar2 + 0xc8000000 < 0x10000) {
    return 5;
  }
  if (uVar2 + 0x40000000 < 0x4000000) {
    return 6;
  }
  if (uVar2 + 0x70000000 < 0x800000) {
    uVar1 = 7;
  }
  else {
    uVar1 = 8;
  }
  return uVar1;
}



/* === 080098fc SystemInit_STM32H7 === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void SystemInit_STM32H7(void)

{
  uint *puVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  
  uVar2 = DAT_08009988;
  puVar1 = DAT_08009984;
  *(uint *)(DAT_08009980 + 0x88) = *(uint *)(DAT_08009980 + 0x88) | 0xf00000;
  puVar3 = DAT_0800998c;
  *puVar1 = *puVar1 | 1;
  puVar1[4] = 0;
  uVar4 = DAT_08009990;
  *puVar1 = uVar2 & *puVar1;
  puVar1[6] = 0;
  puVar1[7] = 0;
  puVar1[8] = 0;
  puVar1[10] = 0;
  puVar1[0xb] = 0;
  puVar1[0xc] = 0;
  puVar1[0xd] = 0;
  puVar1[0xe] = 0;
  puVar1[0xf] = 0;
  puVar1[0x10] = 0;
  puVar1[0x11] = 0;
  *puVar1 = *puVar1 & 0xfffbffff;
  puVar1[0x18] = 0;
  puVar1[0x37] = puVar1[0x37] | 0xe0000000;
  if ((uVar4 & *puVar3) < 0x20000000) {
    *(undefined4 *)(DAT_08009994 + 0x108) = 1;
  }
  *(undefined4 *)(DAT_08009980 + 8) = 0x8000000;
  return;
}



/* === 08009998 FUN_08009998 === */

uint FUN_08009998(int param_1,undefined4 param_2,undefined4 param_3,uint param_4)

{
  char cVar1;
  ushort uVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  undefined4 *puVar6;
  uint *puVar7;
  byte bVar8;
  uint uVar9;
  code *UNRECOVERED_JUMPTABLE;
  int iVar10;
  byte *pbVar11;
  ushort local_1a [3];
  uint local_14;
  uint uStack_10;
  
  iVar4 = *(int *)(param_1 + 0x508);
  pbVar11 = (byte *)(iVar4 + 0x2aa);
  uStack_10 = param_4;
  FUN_08013650(pbVar11,param_1 + 0x4c4);
  bVar8 = *(byte *)(iVar4 + 0x2aa);
  puVar7 = (uint *)0x1;
  *(uint *)(iVar4 + 0x298) = (uint)*(ushort *)(iVar4 + 0x2b0);
  *(undefined4 *)(iVar4 + 0x294) = 1;
  pbVar3 = DAT_08013458;
  if ((bVar8 & 0x1f) == 1) {
    if (((*pbVar11 & 0x60) == 0x40) || (-1 < (int)((uint)*pbVar11 << 0x19))) {
      if ((2 < *(byte *)(iVar4 + 0x29c) - 1) || (1 < *(byte *)(iVar4 + 0x2ae))) {
        FUN_08009a74(iVar4,0x80);
        FUN_08009a74(iVar4);
        return 0;
      }
      iVar10 = FUN_08013104(iVar4);
      if ((iVar10 != 0) ||
         (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b8) + 8),
         UNRECOVERED_JUMPTABLE == (code *)0x0)) {
        return 3;
      }
      *(undefined4 *)(iVar4 + 0x2d4) = 0;
      uVar5 = (*UNRECOVERED_JUMPTABLE)(iVar4,pbVar11);
      if ((*(short *)(iVar4 + 0x2b0) == 0) && (uVar5 == 0)) {
        FUN_080136ac(iVar4);
      }
    }
    else {
      uVar5 = 0;
      FUN_08009a74(iVar4,0x80);
      FUN_08009a74(iVar4,0);
    }
    return uVar5;
  }
  if ((bVar8 & 0x1f) != 2) {
    if ((bVar8 & 0x1f) != 0) {
      uVar5 = FUN_08009a74(iVar4,bVar8 & 0x80,1,uStack_10);
      return uVar5;
    }
    uVar5 = *pbVar11 & 0x60;
    if ((uVar5 == 0x20) || (uVar5 == 0x40)) {
                    /* WARNING: Could not recover jumptable at 0x08013152. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      uVar5 = (**(code **)(*(int *)(iVar4 + (*(int *)(iVar4 + 0x2d4) + 0xae) * 4) + 8))
                        (iVar4,pbVar11);
      return uVar5;
    }
    uVar9 = uStack_10;
    if ((*pbVar11 & 0x60) == 0) {
      uVar9 = (uint)*(byte *)(iVar4 + 0x2ab);
      puVar7 = &switchD_0801315c::switchdataD_08013160;
      switch(uVar9) {
      case 0:
        if ((*(byte *)(iVar4 + 0x29c) - 1 < 3) && (*(short *)(iVar4 + 0x2b0) == 2)) {
          *(undefined4 *)(iVar4 + 0xc) = 1;
          if (*(int *)(iVar4 + 0x2a4) != 0) {
            *(undefined4 *)(iVar4 + 0xc) = 3;
          }
          FUN_08013668(iVar4,iVar4 + 0xc,2);
          return uVar9;
        }
        break;
      case 1:
        if (*(byte *)(iVar4 + 0x29c) - 1 < 3) {
          if (*(short *)(iVar4 + 0x2ac) != 1) {
            return uVar5;
          }
          uVar9 = 0;
LAB_08013200:
          *(uint *)(iVar4 + 0x2a4) = uVar9;
          FUN_080136ac(iVar4);
          return uVar5;
        }
        break;
      default:
        goto switchD_0801315c_caseD_2;
      case 3:
        uVar9 = (uint)*(ushort *)(iVar4 + 0x2ac);
        if (uVar9 == 1) goto LAB_08013200;
        if (uVar9 == 2) {
          *(char *)(iVar4 + 0x2a0) = (char)((ushort)*(undefined2 *)(iVar4 + 0x2ae) >> 8);
          FUN_080136ac();
          return uVar5;
        }
        break;
      case 5:
        if (((*(short *)(iVar4 + 0x2ae) == 0) && (*(short *)(iVar4 + 0x2b0) == 0)) &&
           ((uVar2 = *(ushort *)(iVar4 + 0x2ac), uVar2 < 0x80 &&
            (*(char *)(iVar4 + 0x29c) != '\x03')))) {
          *(char *)(iVar4 + 0x29e) = (char)uVar2;
          FUN_08009ad4();
          FUN_080136ac(iVar4);
          if (uVar2 == 0) {
            *(undefined *)(iVar4 + 0x29c) = 1;
            return uVar5;
          }
LAB_0801341e:
          *(undefined *)(iVar4 + 0x29c) = 2;
          return uVar5;
        }
        break;
      case 6:
        local_1a[0] = 0;
        switch(*(ushort *)(iVar4 + 0x2ac) >> 8) {
        case 1:
          iVar10 = (***(code ***)(iVar4 + 0x2b4))(*(undefined *)(iVar4 + 0x10),local_1a);
          break;
        case 2:
          if (*(char *)(iVar4 + 0x10) == '\0') {
            iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x28))(local_1a);
            *(undefined *)(iVar10 + 1) = 2;
          }
          else {
            iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x2c))(local_1a);
            *(undefined *)(iVar10 + 1) = 2;
          }
          break;
        case 3:
          switch((char)*(ushort *)(iVar4 + 0x2ac)) {
          case '\0':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 4);
            break;
          case '\x01':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 8);
            break;
          case '\x02':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0xc);
            break;
          case '\x03':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x10);
            break;
          case '\x04':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x14);
            break;
          case '\x05':
            UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b4) + 0x18);
            break;
          default:
            goto switchD_08013278_caseD_4;
          }
          if (UNRECOVERED_JUMPTABLE == (code *)0x0) goto switchD_08013278_caseD_4;
          iVar10 = (*UNRECOVERED_JUMPTABLE)(*(undefined *)(iVar4 + 0x10),local_1a);
          break;
        default:
          goto switchD_08013278_caseD_4;
        case 6:
          if (*(char *)(iVar4 + 0x10) != '\0') goto switchD_08013278_caseD_4;
          iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x34))(local_1a);
          break;
        case 7:
          if (*(char *)(iVar4 + 0x10) != '\0') goto switchD_08013278_caseD_4;
          iVar10 = (**(code **)(*(int *)(iVar4 + 0x2b8) + 0x30))(local_1a);
          *(undefined *)(iVar10 + 1) = 7;
        }
        uVar2 = *(ushort *)(iVar4 + 0x2b0);
        if (uVar2 == 0) goto LAB_080133a6;
        if (local_1a[0] != 0) {
          if (local_1a[0] <= uVar2) {
            uVar2 = local_1a[0];
          }
          local_1a[0] = uVar2;
          FUN_08013668(iVar4,iVar10);
          return uVar5;
        }
        break;
      case 8:
        if (*(short *)(iVar4 + 0x2b0) == 1) {
          uVar9 = (uint)*(byte *)(iVar4 + 0x29c);
          puVar7 = (uint *)0x1;
          if (*(byte *)(iVar4 + 0x29c) < 3) {
            if (uVar9 != 0) {
              *(undefined4 *)(iVar4 + 8) = 0;
              FUN_08013668(iVar4,(undefined4 *)(iVar4 + 8));
              return uVar5;
            }
          }
          else if (uVar9 == 3) {
            FUN_08013668(iVar4,iVar4 + 4);
            return uVar5;
          }
          goto switchD_0801315c_caseD_2;
        }
        break;
      case 9:
        bVar8 = *(byte *)(iVar4 + 0x2ac);
        *DAT_08013458 = bVar8;
        if (1 < bVar8) {
          FUN_08009a74(iVar4,0x80);
          FUN_08009a74(iVar4,0);
          return 3;
        }
        if (*(char *)(iVar4 + 0x29c) == '\x02') {
          if (bVar8 != 0) {
            *(undefined4 *)(iVar4 + 4) = 1;
            uVar5 = FUN_08012e10();
            if (uVar5 != 0) {
              FUN_08009a74(iVar4,0x80);
              FUN_08009a74(iVar4,0);
              *(undefined *)(iVar4 + 0x29c) = 2;
              return uVar5;
            }
            FUN_080136ac(iVar4);
            *(undefined *)(iVar4 + 0x29c) = 3;
            return 0;
          }
        }
        else {
          if (*(char *)(iVar4 + 0x29c) != '\x03') {
            FUN_08009a74(iVar4,0x80);
            FUN_08009a74(iVar4,0);
            FUN_08012e20(iVar4,*pbVar3);
            return 3;
          }
          if (bVar8 == 0) {
            *(undefined4 *)(iVar4 + 4) = 0;
            *(undefined *)(iVar4 + 0x29c) = 2;
            FUN_08012e20();
            uVar5 = 0;
          }
          else if (*(uint *)(iVar4 + 4) != 1) {
            FUN_08012e20(iVar4,*(uint *)(iVar4 + 4) & 0xff);
            *(uint *)(iVar4 + 4) = (uint)*pbVar3;
            uVar9 = FUN_08012e10(iVar4);
            if (uVar9 != 0) {
              FUN_08009a74(iVar4,0x80);
              FUN_08009a74(iVar4,0);
              FUN_08012e20(iVar4,*(undefined *)(iVar4 + 4));
              uVar5 = uVar9;
              goto LAB_0801341e;
            }
          }
        }
LAB_080133a6:
        FUN_080136ac(iVar4);
        return uVar5;
      }
switchD_08013278_caseD_4:
      FUN_08009a74(iVar4,0x80);
      FUN_08009a74(iVar4,0);
      return uVar5;
    }
switchD_0801315c_caseD_2:
    FUN_08009a74(iVar4,0x80,puVar7,uVar9);
    FUN_08009a74(iVar4);
    return 0;
  }
  uVar2 = *(ushort *)(iVar4 + 0x2ae);
  uVar5 = (uint)(byte)uVar2;
  bVar8 = *pbVar11 & 0x60;
  if ((bVar8 == 0x20) || (bVar8 == 0x40)) {
LAB_08013544:
    iVar10 = FUN_08013108(iVar4,uVar5);
    if (iVar10 != 0) {
      return 0;
    }
    *(undefined4 *)(iVar4 + 0x2d4) = 0;
    UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar4 + 0x2b8) + 8);
    if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
      return 0;
    }
                    /* WARNING: Could not recover jumptable at 0x08013566. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar5 = (*UNRECOVERED_JUMPTABLE)(iVar4,pbVar11);
    return uVar5;
  }
  if ((*pbVar11 & 0x60) != 0) goto LAB_0801350c;
  cVar1 = *(char *)(iVar4 + 0x2ab);
  if (cVar1 == '\x01') {
    if (*(char *)(iVar4 + 0x29c) != '\x02') {
      if (*(char *)(iVar4 + 0x29c) == '\x03') {
        if (*(short *)(iVar4 + 0x2ac) != 0) {
          return 0;
        }
        local_14 = uVar5;
        if ((uVar2 & 0x7f) != 0) {
          FUN_08009a90();
        }
        FUN_080136ac(iVar4);
        uVar5 = local_14;
        goto LAB_08013544;
      }
      goto LAB_0801350c;
    }
  }
  else {
    if (cVar1 != '\x03') {
      if (cVar1 == '\0') {
        if (*(char *)(iVar4 + 0x29c) == '\x02') {
          if ((uVar2 & 0x7f) == 0) {
            if ((int)((uint)uVar2 << 0x18) < 0) {
              puVar6 = (undefined4 *)(iVar4 + 0x14);
            }
            else {
              puVar6 = (undefined4 *)(iVar4 + 0x154);
            }
            *puVar6 = 0;
            FUN_08013668(iVar4,puVar6,2);
            return 0;
          }
          goto LAB_0801350c;
        }
        if (*(char *)(iVar4 + 0x29c) != '\x03') goto LAB_0801350c;
        iVar10 = iVar4 + (uVar5 & 0xf) * 0x14;
        if ((int)((uint)uVar2 << 0x18) < 0) {
          if (*(short *)(iVar10 + 0x24) == 0) goto LAB_0801350c;
          puVar7 = (uint *)(iVar4 + ((uVar5 & 0x7f) + 1) * 0x14);
        }
        else {
          if (*(short *)(iVar10 + 0x164) == 0) goto LAB_0801350c;
          puVar7 = (uint *)((uVar5 & 0x7f) * 0x14 + iVar4 + 0x154);
        }
        uVar5 = uVar5 & 0x7f;
        if ((uVar2 & 0x7f) != 0) {
          iVar10 = FUN_08009aac(iVar4);
          if (iVar10 == 0) {
            *puVar7 = 0;
            goto LAB_080135b0;
          }
          uVar5 = 1;
        }
        *puVar7 = uVar5;
LAB_080135b0:
        FUN_08013668(iVar4,puVar7,2);
        return 0;
      }
      goto LAB_0801350c;
    }
    if (*(char *)(iVar4 + 0x29c) != '\x02') {
      if (*(char *)(iVar4 + 0x29c) == '\x03') {
        if (((*(short *)(iVar4 + 0x2ac) == 0) && ((uVar2 & 0x7f) != 0)) &&
           (*(short *)(iVar4 + 0x2b0) == 0)) {
          FUN_08009a74(iVar4);
        }
        FUN_080136ac(iVar4);
        return 0;
      }
      goto LAB_0801350c;
    }
  }
  if ((uVar2 & 0x7f) != 0) {
    FUN_08009a74();
    FUN_08009a74(iVar4,0x80);
    return 0;
  }
LAB_0801350c:
  FUN_08009a74(iVar4,0x80);
  FUN_08009a74(iVar4,0);
  return 0;
}



/* === 080099a4 FUN_080099a4 === */

undefined4 FUN_080099a4(int param_1,int param_2)

{
  int iVar1;
  undefined4 uVar2;
  code *UNRECOVERED_JUMPTABLE;
  uint uVar3;
  uint uVar4;
  
  iVar1 = *(int *)(param_1 + 0x508);
  uVar2 = *(undefined4 *)(param_1 + param_2 * 0x24 + 0x288);
  if (param_2 == 0) {
    if (*(int *)(iVar1 + 0x294) == 3) {
      uVar4 = *(uint *)(iVar1 + 0x160);
      if (*(uint *)(iVar1 + 0x15c) <= uVar4) {
        if ((*(char *)(iVar1 + 0x29c) == '\x03') &&
           (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar1 + 0x2b8) + 0x10),
           UNRECOVERED_JUMPTABLE != (code *)0x0)) {
          *(undefined4 *)(iVar1 + 0x2d4) = 0;
          (*UNRECOVERED_JUMPTABLE)();
        }
        FUN_080136ac(iVar1);
        return 0;
      }
      uVar3 = *(uint *)(iVar1 + 0x15c) - uVar4;
      *(uint *)(iVar1 + 0x15c) = uVar3;
      if (uVar3 <= uVar4) {
        uVar4 = uVar3;
      }
      FUN_08013698(iVar1,uVar2,uVar4);
    }
  }
  else if ((*(char *)(iVar1 + 0x29c) == '\x03') &&
          (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar1 + 0x2b8) + 0x18),
          UNRECOVERED_JUMPTABLE != (code *)0x0)) {
    *(undefined4 *)(iVar1 + 0x2d4) = 0;
                    /* WARNING: Could not recover jumptable at 0x08012ec4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar2 = (*UNRECOVERED_JUMPTABLE)();
    return uVar2;
  }
  return 0;
}



/* === 080099b8 FUN_080099b8 === */

undefined4 FUN_080099b8(int param_1,int param_2)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  code *UNRECOVERED_JUMPTABLE;
  uint uVar5;
  
  iVar2 = *(int *)(param_1 + 0x508);
  uVar3 = *(undefined4 *)(param_1 + param_2 * 0x24 + 0x48);
  if (param_2 != 0) {
    if (*(char *)(iVar2 + 0x29c) != '\x03') {
      return 0;
    }
    UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar2 + 0x2b8) + 0x14);
    if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
      return 0;
    }
    *(undefined4 *)(iVar2 + 0x2d4) = 0;
                    /* WARNING: Could not recover jumptable at 0x08012f42. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar3 = (*UNRECOVERED_JUMPTABLE)();
    return uVar3;
  }
  if (*(int *)(iVar2 + 0x294) == 2) {
    uVar4 = *(uint *)(iVar2 + 0x1c);
    uVar5 = *(uint *)(iVar2 + 0x20);
    if (uVar5 < uVar4) {
      *(uint *)(iVar2 + 0x1c) = uVar4 - uVar5;
      FUN_08013684(iVar2,uVar3,uVar4 - uVar5);
      FUN_08009b0c(iVar2,0,0,0);
    }
    else {
      if (((uVar4 != uVar5) || (*(uint *)(iVar2 + 0x18) < uVar4)) ||
         (*(uint *)(iVar2 + 0x298) <= *(uint *)(iVar2 + 0x18))) {
        if ((*(char *)(iVar2 + 0x29c) == '\x03') &&
           (UNRECOVERED_JUMPTABLE = *(code **)(*(int *)(iVar2 + 0x2b8) + 0xc),
           UNRECOVERED_JUMPTABLE != (code *)0x0)) {
          *(undefined4 *)(iVar2 + 0x2d4) = 0;
          (*UNRECOVERED_JUMPTABLE)(iVar2);
        }
        FUN_08009a74(iVar2,0x80);
        FUN_080136c4(iVar2);
        cVar1 = *(char *)(iVar2 + 0x2a0);
        goto joined_r0x08012f6c;
      }
      FUN_08013684(iVar2,0,0);
      *(undefined4 *)(iVar2 + 0x298) = 0;
      FUN_08009b0c(iVar2,0,0,0);
    }
  }
  cVar1 = *(char *)(iVar2 + 0x2a0);
joined_r0x08012f6c:
  if (cVar1 != '\0') {
    *(undefined *)(iVar2 + 0x2a0) = 0;
  }
  return 0;
}



/* === 080099cc FUN_080099cc === */

undefined4 FUN_080099cc(int param_1)

{
  int iVar1;
  code *pcVar2;
  
  if (*(char *)(*(int *)(param_1 + 0x508) + 0x29c) != '\x03') {
    return 0;
  }
  iVar1 = *(int *)(*(int *)(param_1 + 0x508) + 0x2b8);
  if ((iVar1 != 0) && (pcVar2 = *(code **)(iVar1 + 0x1c), pcVar2 != (code *)0x0)) {
    (*pcVar2)();
  }
  return 0;
}



/* === 080099d4 FUN_080099d4 === */

undefined4 FUN_080099d4(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  undefined4 unaff_r4;
  undefined4 uVar4;
  
  iVar2 = *(int *)(param_1 + 0x10);
  if (iVar2 != 0) {
    if (iVar2 != 2) {
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    iVar2 = 1;
  }
  FUN_08013024(*(undefined4 *)(param_1 + 0x508),iVar2);
  iVar2 = *(int *)(param_1 + 0x508);
  *(undefined *)(iVar2 + 0x29c) = 1;
  *(undefined4 *)(iVar2 + 4) = 0;
  *(undefined4 *)(iVar2 + 0x294) = 0;
  *(undefined4 *)(iVar2 + 0x2a4) = 0;
  *(undefined *)(iVar2 + 0x2a0) = 0;
  if (((*(int *)(iVar2 + 0x2b8) == 0) ||
      (pcVar3 = *(code **)(*(int *)(iVar2 + 0x2b8) + 4), pcVar3 == (code *)0x0)) ||
     (iVar1 = (*pcVar3)(), iVar1 == 0)) {
    uVar4 = 0;
  }
  else {
    uVar4 = 3;
  }
  FUN_08009a50(iVar2,0,0,0x40,param_4,unaff_r4);
  *(undefined2 *)(iVar2 + 0x164) = 1;
  *(undefined4 *)(iVar2 + 0x160) = 0x40;
  FUN_08009a50(iVar2,0x80,0,0x40);
  *(undefined2 *)(iVar2 + 0x24) = 1;
  *(undefined4 *)(iVar2 + 0x20) = 0x40;
  return uVar4;
}



/* === 080099f8 FUN_080099f8 === */

void FUN_080099f8(int *param_1)

{
  FUN_0801302c(param_1[0x142]);
  *(uint *)(*param_1 + 0xe00) = *(uint *)(*param_1 + 0xe00) | 1;
  if (param_1[8] != 0) {
    *(uint *)(DAT_08009a24 + 0x10) = *(uint *)(DAT_08009a24 + 0x10) | 6;
  }
  return;
}



/* === 08009a28 FUN_08009a28 === */

undefined4 FUN_08009a28(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x508);
  if (*(char *)(iVar1 + 0x29c) == '\x04') {
    *(undefined *)(iVar1 + 0x29c) = *(undefined *)(iVar1 + 0x29d);
  }
  return 0;
}



/* === 08009a30 FUN_08009a30 === */

undefined4 FUN_08009a30(int param_1)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  
  iVar1 = *(int *)(param_1 + 0x508);
  iVar2 = *(int *)(iVar1 + (*(int *)(iVar1 + 0x2d4) + 0xae) * 4);
  if (iVar2 == 0) {
    return 3;
  }
  if (*(char *)(iVar1 + 0x29c) != '\x03') {
    return 0;
  }
  pcVar3 = *(code **)(iVar2 + 0x24);
  if (pcVar3 != (code *)0x0) {
    (*pcVar3)();
    return 0;
  }
  return 0;
}



/* === 08009a38 FUN_08009a38 === */

undefined4 FUN_08009a38(int param_1)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  
  iVar1 = *(int *)(param_1 + 0x508);
  iVar2 = *(int *)(iVar1 + (*(int *)(iVar1 + 0x2d4) + 0xae) * 4);
  if (iVar2 == 0) {
    return 3;
  }
  if (*(char *)(iVar1 + 0x29c) != '\x03') {
    return 0;
  }
  pcVar3 = *(code **)(iVar2 + 0x20);
  if (pcVar3 != (code *)0x0) {
    (*pcVar3)();
    return 0;
  }
  return 0;
}



/* === 08009a40 FUN_08009a40 === */

undefined4 FUN_08009a40(void)

{
  return 0;
}



/* === 08009a48 FUN_08009a48 === */

undefined4 FUN_08009a48(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  code *pcVar2;
  
  iVar1 = *(int *)(param_1 + 0x508);
  *(undefined *)(iVar1 + 0x29c) = 1;
  if (*(int *)(iVar1 + 0x2b8) == 0) {
    return 0;
  }
  pcVar2 = *(code **)(*(int *)(iVar1 + 0x2b8) + 4);
  iVar1 = (*pcVar2)(iVar1,*(undefined *)(iVar1 + 4),pcVar2,param_4,param_4);
  if (iVar1 == 0) {
    return 0;
  }
  return 3;
}



/* === 08009a50 FUN_08009a50 === */

undefined FUN_08009a50(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  
  uVar1 = FUN_0800ee7c(*(undefined4 *)(param_1 + 0x2c8),param_2,param_4,param_3,param_4);
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009a70 + uVar1);
  }
  return 3;
}



/* === 08009a74 FUN_08009a74 === */

undefined FUN_08009a74(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800ef80(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009a8c + uVar1);
  }
  return 3;
}



/* === 08009a90 FUN_08009a90 === */

undefined FUN_08009a90(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800f008(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009aa8 + uVar1);
  }
  return 3;
}



/* === 08009aac FUN_08009aac === */

undefined FUN_08009aac(int param_1,uint param_2)

{
  if (-1 < (int)(param_2 << 0x18)) {
    return *(undefined *)(*(int *)(param_1 + 0x2c8) + param_2 * 0x24 + 0x27e);
  }
  return *(undefined *)(*(int *)(param_1 + 0x2c8) + (param_2 & 0x7f) * 0x24 + 0x3e);
}



/* === 08009ad4 FUN_08009ad4 === */

undefined FUN_08009ad4(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800ee50(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009aec + uVar1);
  }
  return 3;
}



/* === 08009af0 FUN_08009af0 === */

undefined FUN_08009af0(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800ef3c(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009b08 + uVar1);
  }
  return 3;
}



/* === 08009b0c FUN_08009b0c === */

undefined FUN_08009b0c(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800eef8(*(undefined4 *)(param_1 + 0x2c8));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009b24 + uVar1);
  }
  return 3;
}



/* === 08009b28 FUN_08009b28 === */

void FUN_08009b28(int param_1)

{
  char *pcVar1;
  
  pcVar1 = *(char **)(param_1 + 0x300);
  *(int *)(pcVar1 + 0x3c8) = *(int *)(pcVar1 + 0x3c8) + 1;
  if ((*pcVar1 == '\v') && (*(int *)(pcVar1 + 0x380) != 0)) {
                    /* WARNING: Could not recover jumptable at 0x080136f8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*(int *)(pcVar1 + 0x380) + 0x18))();
    return;
  }
  return;
}



/* === 08009b30 FUN_08009b30 === */

undefined4 FUN_08009b30(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x300);
  *(undefined *)(iVar1 + 800) = 1;
  *(undefined *)(iVar1 + 0x321) = 0;
  *(undefined *)(iVar1 + 0x322) = 0;
  return 0;
}



/* === 08009b38 FUN_08009b38 === */

undefined4 FUN_08009b38(int param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(param_1 + 0x300);
  *(undefined *)(iVar1 + 0x321) = 1;
  *(undefined *)(iVar1 + 0x323) = 0;
  *(undefined *)(iVar1 + 800) = 0;
  FUN_08009b54();
  FUN_08013750(iVar1,*(undefined *)(iVar1 + 4));
  FUN_08013750(iVar1,*(undefined *)(iVar1 + 5));
  return 0;
}



/* === 08009b40 FUN_08009b40 === */

void FUN_08009b40(void)

{
  return;
}



/* === 08009b44 FUN_08009b44 === */

void FUN_08009b44(int param_1)

{
  *(undefined *)(*(int *)(param_1 + 0x300) + 0x323) = 1;
  return;
}



/* === 08009b4c FUN_08009b4c === */

void FUN_08009b4c(int param_1)

{
  *(undefined *)(*(int *)(param_1 + 0x300) + 0x323) = 0;
  return;
}



/* === 08009b54 FUN_08009b54 === */

undefined FUN_08009b54(int param_1)

{
  uint uVar1;
  
  uVar1 = FUN_0800cd24(*(undefined4 *)(param_1 + 0x3d4));
  if (uVar1 < 4) {
    return *(undefined *)(DAT_08009b6c + uVar1);
  }
  return 2;
}



/* === 08009b70 FUN_08009b70 === */

void FUN_08009b70(undefined4 *param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  
  puVar1 = (undefined4 *)(DAT_08009b8c + param_2 * 0xc);
  uVar2 = puVar1[1];
  uVar3 = puVar1[2];
  *param_1 = *puVar1;
  param_1[1] = uVar2;
  param_1[2] = uVar3;
  return;
}



/* === 08009b90 FUN_08009b90 === */

void FUN_08009b90(float param_1,float param_2,float param_3,int *param_4)

{
  if ((int)((uint)(param_1 < 0.0) << 0x1f) < 0) {
    *param_4 = DAT_08009c1c;
  }
  else {
    *param_4 = (uint)(param_1 != 1.0) * 0x3f800000 + (uint)(param_1 == 1.0) * (int)param_1;
  }
  if ((int)((uint)(param_2 < 0.0) << 0x1f) < 0) {
    param_4[1] = DAT_08009c1c;
  }
  else {
    param_4[1] = (uint)(param_2 != 1.0) * 0x3f800000 + (uint)(param_2 == 1.0) * (int)param_2;
  }
  if (-1 < (int)((uint)(param_3 < 0.0) << 0x1f)) {
    param_4[2] = (uint)(param_3 != 1.0) * 0x3f800000 + (uint)(param_3 == 1.0) * (int)param_3;
    return;
  }
  param_4[2] = DAT_08009c1c;
  return;
}



/* === 08009c20 FUN_08009c20 === */

void FUN_08009c20(void)

{
  return;
}



/* === 08009c24 FUN_08009c24 === */

undefined4 FUN_08009c24(uint param_1)

{
  int iVar1;
  
  if (*DAT_08009c64 == 0) {
    return 1;
  }
  iVar1 = FUN_0800aa28(*DAT_08009c68 / (1000 / *DAT_08009c64));
  if ((param_1 < 0x10) && (iVar1 == 0)) {
    FUN_0800a968(0xffffffff,param_1,0);
    *DAT_08009c6c = param_1;
    return 0;
  }
  return 1;
}



/* === 08009c70 FUN_08009c70 === */

undefined4 FUN_08009c70(void)

{
  byte bVar1;
  uint *puVar2;
  uint uVar3;
  int iVar4;
  
  puVar2 = DAT_08009cc0;
  FUN_0800a944(3);
  uVar3 = FUN_080100f4();
  bVar1 = *(byte *)(DAT_08009cc8 + (*(uint *)(DAT_08009cc4 + 0x18) & 0xf));
  uVar3 = uVar3 >> (*(byte *)(DAT_08009cc8 + ((uint)(*(int *)(DAT_08009cc4 + 0x18) << 0x14) >> 0x1c)
                             ) & 0x1f);
  *DAT_08009ccc = uVar3;
  *puVar2 = uVar3 >> (bVar1 & 0x1f);
  iVar4 = FUN_08009c24(0xe);
  if (iVar4 != 0) {
    return 1;
  }
  FUN_08009c20();
  return 0;
}



/* === 08009cd0 FUN_08009cd0 === */

void FUN_08009cd0(void)

{
  *DAT_08009ce0 = (uint)*DAT_08009ce4 + *DAT_08009ce0;
  return;
}



/* === 08009ce8 FUN_08009ce8 === */

undefined4 FUN_08009ce8(void)

{
  return *DAT_08009cf0;
}



/* === 08009cf4 FUN_08009cf4 === */

void FUN_08009cf4(uint param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_08009ce8();
  if (param_1 != 0xffffffff) {
    param_1 = param_1 + *DAT_08009d14;
  }
  do {
    iVar2 = FUN_08009ce8();
  } while ((uint)(iVar2 - iVar1) < param_1);
  return;
}



/* === 08009d18 FUN_08009d18 === */

uint FUN_08009d18(void)

{
  return *DAT_08009d20 >> 0x10;
}



/* === 08009d24 FUN_08009d24 === */

void FUN_08009d24(void)

{
  return;
}



/* === 08009dbc FUN_08009dbc === */

uint FUN_08009dbc(int *param_1,uint *param_2)

{
  byte bVar1;
  byte bVar2;
  byte bVar3;
  byte bVar4;
  uint *puVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  uVar6 = 1;
  iVar7 = *param_1;
  *(undefined *)(param_1 + 0x14) = 1;
  if (*(int *)(iVar7 + 8) << 0x1d < 0) {
    param_1[0x15] = param_1[0x15] | 0x20;
    goto LAB_08009de6;
  }
  uVar9 = *param_2;
  if (-1 < (int)uVar9) {
    if ((uVar9 & 0xfffff) == 0) {
      uVar6 = 1 << (uVar9 >> 0x1a);
    }
    else {
      bVar2 = (byte)uVar9;
      bVar3 = (byte)(uVar9 >> 8);
      bVar4 = (byte)(uVar9 >> 0x10);
      bVar1 = (byte)(uVar9 >> 0x18);
      uVar10 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                               bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                            bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
               (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                               bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                            bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
               (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                               bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                            bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
               (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                               bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                            bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
      if (uVar10 != 0) {
        uVar6 = 1 << LZCOUNT(uVar10);
      }
    }
    *(uint *)(iVar7 + 0x1c) = uVar6 | *(uint *)(iVar7 + 0x1c);
  }
  uVar10 = param_2[1] & 0x1f;
  uVar6 = param_2[1] >> 6 & 0xc;
  *(uint *)(uVar6 + iVar7 + 0x30) =
       (uVar9 >> 0x1a & 0x1f) << uVar10 | *(uint *)(uVar6 + iVar7 + 0x30) & ~(0x1f << uVar10);
  puVar5 = DAT_0800a140;
  if (((*(uint *)(iVar7 + 8) & 4) == 0) && ((*(uint *)(iVar7 + 8) & 8) == 0)) {
    uVar6 = (*param_2 << 7) >> 0x1b;
    uVar9 = *param_2 >> 0x17 & 4;
    *(uint *)(uVar9 + iVar7 + 0x14) =
         *(uint *)(uVar9 + iVar7 + 0x14) & ~(7 << uVar6) | param_2[2] << uVar6;
    if ((*puVar5 & 0xf0000000) == 0x10000000) {
      uVar6 = param_2[5] << (((uint)(*(int *)(iVar7 + 0xc) << 0x1b) >> 0x1d) << 1);
    }
    else if (*(int *)(iVar7 + 0xc) << 0x1b < 0) {
      uVar6 = param_2[5] << (*(uint *)(iVar7 + 0xc) >> 1 & 8);
    }
    else {
      uVar6 = param_2[5] << (((*(uint *)(iVar7 + 0xc) << 0x1b) >> 0x1d) << 1);
    }
    uVar9 = param_2[4];
    if (uVar9 == 4) {
      iVar11 = *param_2 * 0x4000000;
      if ((*(uint *)(iVar7 + 0x60) & 0x7c000000) == *param_2 * 0x4000000) {
        *(uint *)(iVar7 + 0x60) = *(uint *)(iVar7 + 0x60) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 100) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 100) = *(uint *)(iVar7 + 100) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 0x68) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 0x68) = *(uint *)(iVar7 + 0x68) & 0x7fffffff;
      }
      if (iVar11 - (*(uint *)(iVar7 + 0x6c) & 0x7c000000) == 0) {
        *(uint *)(iVar7 + 0x6c) = *(uint *)(iVar7 + 0x6c) & 0x7fffffff;
      }
    }
    else {
      iVar11 = iVar7 + 0x60;
      *(uint *)(iVar11 + uVar9 * 4) =
           *param_2 & 0x7c000000 | *(uint *)(iVar11 + uVar9 * 4) & 0x80000000 | uVar6;
      if (*(char *)((int)param_2 + 0x19) == '\x01') {
        uVar6 = 0x80000000;
      }
      else {
        uVar6 = 0;
      }
      *(uint *)(iVar11 + param_2[4] * 4) = *(uint *)(iVar11 + param_2[4] * 4) & 0x7fffffff | uVar6;
      uVar6 = 0;
      if (*(char *)(param_2 + 6) == '\x01') {
        uVar6 = 0x800 << (param_2[4] & 0x1f);
      }
      *(uint *)(iVar7 + 0x10) = uVar6 | *(uint *)(iVar7 + 0x10) & 0xffff87ff;
    }
  }
  if (-1 < *(int *)(iVar7 + 8) << 0x1f) {
    uVar9 = param_2[3];
    uVar6 = *param_2;
    *(uint *)(iVar7 + 0xc0) =
         DAT_0800a144 >> (uVar9 & 0x18) & uVar6 | *(uint *)(iVar7 + 0xc0) & ~(uVar6 & 0xfffff);
    if (uVar9 == DAT_0800a148) {
      if ((uVar6 & 0xfffff) == 0) {
        uVar6 = (uVar6 >> 0x1a) + 1 & 0x1f;
        if (uVar6 < 10) {
          uVar9 = 1 << uVar6;
          uVar6 = uVar6 * 0x300000;
        }
        else {
          uVar9 = 1 << uVar6;
          uVar6 = (uVar6 * 3 + -0x1e) * 0x100000 | 0x2000000;
        }
      }
      else {
        bVar2 = (byte)uVar6;
        bVar3 = (byte)(uVar6 >> 8);
        bVar4 = (byte)(uVar6 >> 0x10);
        bVar1 = (byte)(uVar6 >> 0x18);
        uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1 |
                                bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) << 1 |
                             bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1 |
                                bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) << 1 |
                             bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1 |
                                bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) << 1 |
                             bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1 |
                                bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) << 1 |
                             bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
        if ((uVar6 == 0) || ((LZCOUNT(uVar6) + 1U & 0x1f) < 10)) {
          uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                  | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) <<
                                1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                  (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                  | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) <<
                                1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                  (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                  | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) <<
                                1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                  (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                  | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) <<
                                1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          if (uVar6 == 0) {
            uVar9 = 0;
          }
          else {
            uVar9 = 1 << (LZCOUNT(uVar6) + 1U & 0x1f);
          }
          uVar6 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                  | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1) <<
                                1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                  (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                  | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1) <<
                                1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                  (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                  | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1) <<
                                1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                  (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                  | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1) <<
                                1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          if (uVar6 == 0) {
            uVar6 = 0x300000;
          }
          else {
            uVar6 = (LZCOUNT(uVar6) + 1U & 0x1f) * 0x300000;
          }
        }
        else {
          uVar10 = (uint)(byte)((((((((bVar2 & 1) << 1 | bVar2 >> 1 & 1) << 1 | bVar2 >> 2 & 1) << 1
                                   | bVar2 >> 3 & 1) << 1 | bVar2 >> 4 & 1) << 1 | bVar2 >> 5 & 1)
                                 << 1 | bVar2 >> 6 & 1) << 1 | bVar2 >> 7) << 0x18 |
                   (uint)(byte)((((((((bVar3 & 1) << 1 | bVar3 >> 1 & 1) << 1 | bVar3 >> 2 & 1) << 1
                                   | bVar3 >> 3 & 1) << 1 | bVar3 >> 4 & 1) << 1 | bVar3 >> 5 & 1)
                                 << 1 | bVar3 >> 6 & 1) << 1 | bVar3 >> 7) << 0x10 |
                   (uint)(byte)((((((((bVar4 & 1) << 1 | bVar4 >> 1 & 1) << 1 | bVar4 >> 2 & 1) << 1
                                   | bVar4 >> 3 & 1) << 1 | bVar4 >> 4 & 1) << 1 | bVar4 >> 5 & 1)
                                 << 1 | bVar4 >> 6 & 1) << 1 | bVar4 >> 7) << 8 |
                   (uint)(byte)((((((((bVar1 & 1) << 1 | bVar1 >> 1 & 1) << 1 | bVar1 >> 2 & 1) << 1
                                   | bVar1 >> 3 & 1) << 1 | bVar1 >> 4 & 1) << 1 | bVar1 >> 5 & 1)
                                 << 1 | bVar1 >> 6 & 1) << 1 | bVar1 >> 7);
          uVar9 = 1 << (LZCOUNT(uVar6) + 1U & 0x1f);
          uVar6 = DAT_0800a1f0;
          if (uVar10 != 0) {
            uVar6 = ((short)((short)LZCOUNT(uVar10) + 1U & 0x1f) * 3 + -0x1e) * 0x100000 | 0x2000000
            ;
          }
        }
      }
      uVar10 = ((uVar6 | uVar9) << 7) >> 0x1b;
      uVar6 = (uVar6 | uVar9) >> 0x17 & 4;
      *(uint *)(uVar6 + iVar7 + 0x14) =
           param_2[2] << uVar10 | *(uint *)(uVar6 + iVar7 + 0x14) & ~(7 << uVar10);
      uVar6 = *param_2;
    }
    if ((int)uVar6 < 0) {
      if ((iVar7 == DAT_0800a14c) || (iVar7 == DAT_0800a14c + 0x100)) {
        uVar10 = *(uint *)(DAT_0800a164 + 8);
        uVar9 = *(uint *)(DAT_0800a14c + 0x108) | *(uint *)(DAT_0800a14c + 8);
        iVar11 = DAT_0800a164;
      }
      else {
        uVar10 = *(uint *)(DAT_0800a150 + 8);
        uVar9 = *(uint *)(DAT_0800a154 + 8);
        iVar11 = DAT_0800a150;
      }
      uVar12 = uVar10 & 0x1c00000;
      if ((~uVar9 & 1) == 0) {
        uVar6 = 1;
        param_1[0x15] = param_1[0x15] | 0x20;
        goto LAB_08009de6;
      }
      if (uVar6 == DAT_0800a158) {
        if ((-1 < (int)(uVar10 << 8)) && (iVar7 == DAT_0800a1e4)) {
          iVar8 = (uint)((ulonglong)DAT_0800a1ec * (ulonglong)(*DAT_0800a1e8 >> 6) >> 0x26) + 1;
          *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x800000;
          iVar7 = iVar8 * 2;
          while (iVar8 != 0) {
            iVar7 = iVar7 + -1;
            iVar8 = iVar7;
          }
        }
      }
      else if (uVar6 == DAT_0800a15c) {
        if (((uVar10 & 0x1000000) == 0) && (iVar7 == DAT_0800a1e4)) {
          *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x1000000;
          uVar6 = 0;
          goto LAB_08009de6;
        }
      }
      else if (((uVar6 == DAT_0800a160) && (-1 < (int)(uVar10 << 9))) && (iVar7 == DAT_0800a154)) {
        uVar6 = 0;
        *(uint *)(iVar11 + 8) = *(uint *)(iVar11 + 8) & 0xfe3fffff | uVar12 | 0x400000;
        goto LAB_08009de6;
      }
    }
  }
  uVar6 = 0;
LAB_08009de6:
  *(undefined *)(param_1 + 0x14) = 0;
  return uVar6;
}



/* === 0800a1f4 FUN_0800a1f4 === */

undefined4 FUN_0800a1f4(int **param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  
  piVar4 = *param_1;
  if (piVar4[2] << 0x1f < 0) {
    return 0;
  }
  if ((piVar4[2] & DAT_0800a288) != 0) {
LAB_0800a266:
    param_1[0x15] = (int *)((uint)param_1[0x15] | 0x10);
    param_1[0x16] = (int *)((uint)param_1[0x16] | 1);
    return 1;
  }
  piVar4[2] = DAT_0800a28c & piVar4[2] | 1;
  iVar2 = FUN_08009ce8();
  uVar1 = DAT_0800a28c;
  piVar4 = *param_1;
  if ((((piVar4 != DAT_0800a290) && (piVar4 != DAT_0800a290 + 0x40)) ||
      ((*(uint *)(DAT_0800a298 + 8) & 0x1f) == 0)) || (piVar4 != DAT_0800a29c)) {
    iVar3 = *piVar4;
    while (-1 < iVar3 << 0x1f) {
      if (-1 < piVar4[2] << 0x1f) {
        piVar4[2] = piVar4[2] & uVar1 | 1;
      }
      iVar3 = FUN_08009ce8();
      piVar4 = *param_1;
      if ((2 < (uint)(iVar3 - iVar2)) && (-1 < *piVar4 << 0x1f)) goto LAB_0800a266;
      iVar3 = *piVar4;
    }
  }
  return 0;
}



/* === 0800a3c4 FUN_0800a3c4 === */

undefined4 FUN_0800a3c4(int *param_1)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  
  puVar2 = (undefined4 *)*param_1;
  if ((int)(puVar2[2] << 0x1e) < 0) {
    return 0;
  }
  if ((int)(puVar2[2] << 0x1f) < 0) {
    if ((puVar2[2] & 0xd) != 1) {
LAB_0800a3e6:
      param_1[0x15] = param_1[0x15] | 0x10;
      param_1[0x16] = param_1[0x16] | 1;
      return 1;
    }
    puVar2[2] = DAT_0800a434 & puVar2[2] | 2;
    *puVar2 = 3;
    iVar1 = FUN_08009ce8();
    iVar3 = *(int *)(*param_1 + 8);
    while (iVar3 << 0x1f < 0) {
      iVar3 = FUN_08009ce8();
      if ((2 < (uint)(iVar3 - iVar1)) && (*(int *)(*param_1 + 8) << 0x1f < 0)) goto LAB_0800a3e6;
      iVar3 = *(int *)(*param_1 + 8);
    }
  }
  return 0;
}



/* === 0800a438 FUN_0800a438 === */

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



/* === 0800a5b4 FUN_0800a5b4 === */

bool FUN_0800a5b4(int *param_1)

{
  ulonglong uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  
  if (param_1 == (int *)0x0) {
    return true;
  }
  if (param_1[0x15] == 0) {
    FUN_080075e0();
    param_1[0x16] = 0;
    *(undefined *)(param_1 + 0x14) = 0;
  }
  iVar2 = *param_1;
  if (*(int *)(iVar2 + 8) << 2 < 0) {
    *(uint *)(iVar2 + 8) = DAT_0800a7a0 & *(uint *)(iVar2 + 8);
  }
  if (-1 < *(int *)(iVar2 + 8) << 3) {
    uVar3 = *DAT_0800a7a4;
    uVar1 = (ulonglong)DAT_0800a7a8;
    *(uint *)(iVar2 + 8) = DAT_0800a7ac & *(uint *)(iVar2 + 8) | 0x10000000;
    for (iVar4 = (uint)(uVar1 * (uVar3 >> 6) >> 0x26) + 1; iVar4 != 0; iVar4 = iVar4 + -1) {
    }
  }
  bVar6 = *(int *)(iVar2 + 8) << 3 < 0;
  if (bVar6) {
    uVar3 = *(uint *)(iVar2 + 8);
    iVar4 = param_1[0x15];
  }
  else {
    param_1[0x15] = param_1[0x15] | 0x10;
    param_1[0x16] = param_1[0x16] | 1;
    uVar3 = *(uint *)(iVar2 + 8);
    iVar4 = param_1[0x15];
  }
  if (((uVar3 & 4) == 0) && (-1 < iVar4 << 0x1b)) {
    param_1[0x15] = param_1[0x15] & 0xfffffefdU | 2;
    if (-1 < *(int *)(iVar2 + 8) << 0x1f) {
      if ((iVar2 == DAT_0800a7b0) || (iVar2 == DAT_0800a7b0 + 0x100)) {
        uVar3 = *(uint *)(DAT_0800a7bc + 8) | *(uint *)(DAT_0800a7b0 + 8);
        iVar2 = DAT_0800a7c0;
      }
      else {
        uVar3 = *(uint *)(DAT_0800a7b4 + 8);
        iVar2 = DAT_0800a7b8;
      }
      if (-1 < (int)(uVar3 << 0x1f)) {
        *(uint *)(iVar2 + 8) = *(uint *)(iVar2 + 8) & 0xffc0ffff | param_1[1];
      }
    }
    uVar3 = FUN_08009d18();
    uVar5 = (uint)*(byte *)(param_1 + 7);
    if ((uVar3 < 0x1004) || (param_1[2] != 0x10)) {
      uVar3 = uVar5 << 0x10 | (uint)*(byte *)((int)param_1 + 0x15) << 0xd | param_1[0xc] |
              param_1[2];
    }
    else {
      uVar3 = uVar5 << 0x10 | (uint)*(byte *)((int)param_1 + 0x15) << 0xd | param_1[0xc] | 0x1c;
    }
    if (uVar5 == 1) {
      uVar3 = uVar3 | (param_1[8] + -1) * 0x20000;
    }
    if (param_1[9] != 0) {
      uVar3 = uVar3 | param_1[9] & 0x3e0U | param_1[10];
    }
    iVar2 = *param_1;
    *(uint *)(iVar2 + 0xc) = uVar3 | DAT_0800a7c4 & *(uint *)(iVar2 + 0xc);
    if (((*(uint *)(iVar2 + 8) & 4) == 0) && (-1 < *(int *)(iVar2 + 8) << 0x1c)) {
      *(uint *)(iVar2 + 0xc) =
           DAT_0800a7c8 & *(uint *)(iVar2 + 0xc) | (uint)*(byte *)(param_1 + 5) << 0xe |
           param_1[0xb];
      if (*(char *)(param_1 + 0xe) == '\x01') {
        *(uint *)(iVar2 + 0x10) =
             param_1[0x10] | param_1[0x11] | (param_1[0xf] + -1) * 0x10000 | param_1[0x12] |
             DAT_0800a7cc & *(uint *)(iVar2 + 0x10) | 1;
      }
      else {
        *(uint *)(iVar2 + 0x10) = *(uint *)(iVar2 + 0x10) & 0xfffffffe;
      }
      *(uint *)(iVar2 + 0x10) = *(uint *)(iVar2 + 0x10) & 0xfffffff | param_1[0xd];
      FUN_0800a438(param_1);
      iVar2 = *param_1;
    }
    if (param_1[3] == 1) {
      *(uint *)(iVar2 + 0x30) = param_1[6] - 1U | *(uint *)(iVar2 + 0x30) & 0xfffffff0;
    }
    else {
      *(uint *)(iVar2 + 0x30) = *(uint *)(iVar2 + 0x30) & 0xfffffff0;
    }
    param_1[0x15] = param_1[0x15] & 0xfffffffcU | 1;
    return !bVar6;
  }
  param_1[0x15] = param_1[0x15] | 0x10;
  return true;
}



/* === 0800a7d0 FUN_0800a7d0 === */

int FUN_0800a7d0(int *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  uint local_1c;
  
  local_1c = 0;
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x14) = 1;
  iVar3 = FUN_0800a3c4();
  uVar2 = DAT_0800a870;
  uVar1 = DAT_0800a86c;
  if (iVar3 == 0) {
    param_1[0x15] = DAT_0800a868 & param_1[0x15] | 2;
    iVar4 = *param_1;
    *(uint *)(iVar4 + 8) =
         uVar1 & *(uint *)(iVar4 + 8) | param_3 & 0x40000000 | param_2 & 0x10000 | 0x80000000;
    do {
      if (-1 < *(int *)(iVar4 + 8)) {
        param_1[0x15] = param_1[0x15] & 0xfffffffcU | 1;
        goto LAB_0800a836;
      }
      local_1c = local_1c + 1;
    } while (local_1c < uVar2);
    iVar3 = 1;
    *(undefined *)(param_1 + 0x14) = 0;
    param_1[0x15] = param_1[0x15] & 0xffffffedU | 0x10;
  }
  else {
    param_1[0x15] = param_1[0x15] | 0x10;
LAB_0800a836:
    *(undefined *)(param_1 + 0x14) = 0;
  }
  return iVar3;
}



/* === 0800a874 FUN_0800a874 === */

undefined4 FUN_0800a874(int *param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  uint uVar5;
  bool bVar6;
  
  uVar5 = *param_2;
  if (*(char *)(param_1 + 0x14) == '\x01') {
    return 2;
  }
  iVar4 = *param_1;
  bVar6 = iVar4 != DAT_0800a934;
  *(undefined *)(param_1 + 0x14) = 1;
  iVar2 = DAT_0800a940;
  iVar1 = DAT_0800a938;
  if (bVar6) {
    *(undefined *)(param_1 + 0x14) = 0;
    param_1[0x15] = param_1[0x15] | 0x20;
    return 1;
  }
  if ((*(int *)(DAT_0800a938 + 8) << 0x1d < 0) || ((*(uint *)(iVar4 + 8) & 4) != 0)) {
    uVar3 = 1;
    param_1[0x15] = param_1[0x15] | 0x20;
  }
  else {
    if (uVar5 == 0) {
      *(uint *)(DAT_0800a940 + 8) = *(uint *)(DAT_0800a940 + 8) & 0xffff3fff;
      if (-1 < (int)((*(uint *)(iVar1 + 8) | *(uint *)(iVar4 + 8)) << 0x1f)) {
        *(uint *)(iVar2 + 8) = DAT_0800a93c & *(uint *)(iVar2 + 8);
        uVar3 = 0;
        goto LAB_0800a8be;
      }
    }
    else {
      *(uint *)(DAT_0800a940 + 8) = *(uint *)(DAT_0800a940 + 8) & 0xffff3fff | param_2[1];
      if (-1 < (int)((*(uint *)(iVar1 + 8) | *(uint *)(iVar4 + 8)) << 0x1f)) {
        *(uint *)(iVar2 + 8) = uVar5 | param_2[2] | DAT_0800a93c & *(uint *)(iVar2 + 8);
        uVar3 = 0;
        goto LAB_0800a8be;
      }
    }
    uVar3 = 0;
  }
LAB_0800a8be:
  *(undefined *)(param_1 + 0x14) = 0;
  return uVar3;
}



/* === 0800a944 FUN_0800a944 === */

void FUN_0800a944(uint param_1)

{
  *(uint *)(DAT_0800a960 + 0xc) =
       DAT_0800a964 | (param_1 & 7) << 8 | *(uint *)(DAT_0800a960 + 0xc) & 0xf8ff;
  return;
}



/* === 0800a968 FUN_0800a968 === */

void FUN_0800a968(uint param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = (uint)(*(int *)(DAT_0800a9d8 + 0xc) << 0x15) >> 0x1d;
  uVar2 = 7 - uVar1;
  if (3 < uVar2) {
    uVar2 = 4;
  }
  if (uVar1 + 4 < 7) {
    param_3 = 0;
    uVar1 = param_3;
  }
  else {
    param_3 = param_3 & ~(-1 << (uVar1 - 3 & 0xff));
    uVar1 = uVar1 - 3;
  }
  param_3 = (param_2 & ~(-1 << (uVar2 & 0xff))) << (uVar1 & 0xff) | param_3;
  if (-1 < (int)param_1) {
    *(char *)(DAT_0800a9dc + param_1 + 0x300) = (char)(param_3 << 4);
    return;
  }
  *(char *)(DAT_0800a9e0 + (param_1 & 0xf) + 0x18) = (char)(param_3 << 4);
  return;
}



/* === 0800a9e4 FUN_0800a9e4 === */

void FUN_0800a9e4(uint param_1)

{
  if (-1 < (int)param_1) {
    *(int *)(DAT_0800a9fc + (param_1 >> 5) * 4) = 1 << (param_1 & 0x1f);
  }
  return;
}



/* === 0800aa28 FUN_0800aa28 === */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_0800aa28(int param_1)

{
  if (param_1 - 1U < 0x1000000) {
    _DAT_e000e014 = param_1 - 1U;
    *(undefined *)(DAT_0800aa50 + 0x23) = 0xf0;
    _DAT_e000e018 = 0;
    _DAT_e000e010 = 7;
    return 0;
  }
  return 1;
}



/* === 0800aa54 FUN_0800aa54 === */

void FUN_0800aa54(void)

{
  int iVar1;
  
  iVar1 = DAT_0800aa6c;
  DataMemoryBarrier(0x1f);
  *(uint *)(DAT_0800aa6c + 0x24) = *(uint *)(DAT_0800aa6c + 0x24) & 0xfffeffff;
  *(undefined4 *)(iVar1 + 0x94) = 0;
  return;
}



/* === 0800aa70 FUN_0800aa70 === */

void FUN_0800aa70(uint param_1)

{
  int iVar1;
  
  iVar1 = DAT_0800aa8c;
  *(uint *)(DAT_0800aa8c + 0x94) = param_1 | 1;
  *(uint *)(iVar1 + 0x24) = *(uint *)(iVar1 + 0x24) | 0x10000;
  DataSynchronizationBarrier(0xf);
  InstructionSynchronizationBarrier(0xf);
  return;
}



/* === 0800aa90 FUN_0800aa90 === */

void FUN_0800aa90(byte *param_1)

{
  byte bVar1;
  int iVar2;
  
  iVar2 = DAT_0800aaf0;
  *(uint *)(DAT_0800aaf0 + 0x98) = (uint)param_1[1];
  bVar1 = *param_1;
  if (bVar1 != 0) {
    *(undefined4 *)(iVar2 + 0x9c) = *(undefined4 *)(param_1 + 4);
    *(uint *)(iVar2 + 0xa0) =
         (uint)param_1[0xb] << 0x18 | (uint)param_1[0xc] << 0x1c | (uint)bVar1 |
         (uint)param_1[10] << 0x13 | (uint)param_1[0xd] << 0x12 | (uint)param_1[0xe] << 0x11 |
         (uint)param_1[0xf] << 0x10 | (uint)param_1[9] << 8 | (uint)param_1[8] << 1;
    return;
  }
  *(undefined4 *)(iVar2 + 0x9c) = 0;
  *(undefined4 *)(iVar2 + 0xa0) = 0;
  return;
}



/* === 0800aaf4 FUN_0800aaf4 === */

void FUN_0800aaf4(void)

{
  return;
}



/* === 0800ab00 FUN_0800ab00 === */

void FUN_0800ab00(uint **param_1,uint param_2,uint param_3,uint param_4)

{
  bool bVar1;
  bool bVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  bool bVar13;
  bool bVar14;
  bool bVar15;
  bool bVar16;
  bool bVar17;
  bool bVar18;
  bool bVar19;
  bool bVar20;
  bool bVar21;
  bool bVar22;
  bool bVar23;
  bool bVar24;
  
  puVar3 = *param_1;
  puVar4 = param_1[0x16];
  if ((puVar3 == DAT_0800ac90) || (puVar3 == DAT_0800ac90 + 6)) {
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (param_1[0x1b] != (uint *)0x0) {
      param_1[0x1c][1] = (uint)param_1[0x1d];
    }
LAB_0800ac32:
    puVar5 = param_1[2];
    puVar4[2] = 0x3f << ((uint)param_1[0x17] & 0x1f);
    *puVar3 = *puVar3 & 0xfffbffff;
    puVar3[1] = param_4;
  }
  else {
    bVar6 = puVar3 != DAT_0800ac94;
    bVar1 = puVar3 != DAT_0800ac90 + 0xc;
    bVar7 = puVar3 != DAT_0800ac94 + 6;
    bVar8 = puVar3 != DAT_0800ac94 + 0xc;
    bVar9 = puVar3 != DAT_0800ac94 + 0x12;
    bVar10 = puVar3 != DAT_0800ac94 + 0x18;
    bVar11 = puVar3 != DAT_0800ac94 + 0xee;
    bVar12 = puVar3 != DAT_0800ac9c;
    bVar2 = puVar3 != DAT_0800ac98;
    bVar13 = puVar3 != DAT_0800aca0;
    bVar14 = puVar3 != DAT_0800aca4;
    bVar15 = puVar3 != DAT_0800aca8;
    bVar16 = puVar3 != DAT_0800acac;
    bVar17 = puVar3 != DAT_0800acb0;
    puVar5 = DAT_0800acb0 + 0x6001400;
    bVar18 = puVar3 != DAT_0800acb4;
    bVar19 = puVar3 != DAT_0800acb8;
    bVar20 = puVar3 != DAT_0800acbc;
    bVar21 = puVar3 != DAT_0800acc0;
    bVar22 = puVar3 != DAT_0800acc4;
    bVar23 = puVar3 != DAT_0800acc8;
    bVar24 = puVar3 != DAT_0800accc;
    if ((bVar23 && (bVar21 &&
                   (bVar19 && (puVar3 != puVar5 && (bVar16 && (bVar14 && (bVar12 && bVar2))))))) &&
       (bVar24 && (bVar22 &&
                  (bVar20 &&
                  (bVar18 &&
                  (bVar17 &&
                  (bVar15 &&
                  (bVar13 &&
                  (bVar11 && (bVar10 && (bVar9 && (bVar8 && (bVar7 && (bVar6 && bVar1))))))))))))))
    {
      return;
    }
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (param_1[0x1b] == (uint *)0x0) {
      if (!bVar24 ||
          (!bVar22 ||
          (!bVar20 ||
          (!bVar18 ||
          (!bVar17 ||
          (!bVar15 ||
          (!bVar13 ||
          (!bVar11 || (!bVar10 || (!bVar9 || (!bVar8 || (!bVar7 || (!bVar6 || !bVar1)))))))))))))
      goto LAB_0800ac32;
      if (bVar23 && (bVar21 &&
                    (bVar19 && (puVar3 != puVar5 && (bVar16 && (bVar14 && (bVar12 && bVar2))))))) {
        return;
      }
    }
    else {
      param_1[0x1c][1] = (uint)param_1[0x1d];
      if (!bVar24 ||
          (!bVar22 ||
          (!bVar20 ||
          (!bVar18 ||
          (!bVar17 ||
          (!bVar15 ||
          (!bVar13 ||
          (!bVar11 || (!bVar10 || (!bVar9 || (!bVar8 || (!bVar7 || (!bVar6 || !bVar1)))))))))))))
      goto LAB_0800ac32;
    }
    puVar5 = param_1[2];
    puVar4[1] = 1 << ((uint)param_1[0x17] & 0x1f);
    puVar3[1] = param_4;
  }
  if (puVar5 != (uint *)0x40) {
    puVar3[2] = param_2;
    puVar3[3] = param_3;
    return;
  }
  puVar3[2] = param_3;
  puVar3[3] = param_2;
  return;
}



/* === 0800acd0 FUN_0800acd0 === */

uint FUN_0800acd0(uint *param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = *param_1;
  if ((uVar1 == DAT_0800ada8 + 0x460 ||
       (uVar1 == DAT_0800adb0 + 0x430 ||
       (uVar1 == DAT_0800ada8 + 0x430 ||
       (uVar1 == DAT_0800adb0 + 0x400 ||
       (uVar1 == DAT_0800ada8 + 0x400 ||
       (uVar1 == DAT_0800adb0 + 0x3d0 ||
       (uVar1 == DAT_0800ada8 + 0x3d0 ||
       (uVar1 == DAT_0800adb0 + 0x60 ||
       (uVar1 == DAT_0800ada8 + 0x60 ||
       (uVar1 == DAT_0800adb0 + 0x30 ||
       (uVar1 == DAT_0800ada8 + 0x30 ||
       (uVar1 == DAT_0800adb0 ||
       (uVar1 == DAT_0800ada8 || (uVar1 == DAT_0800adac || uVar1 == DAT_0800ada4)))))))))))))) ||
     (uVar1 == DAT_0800adb4)) {
    uVar2 = (uVar1 & 0xff) - 0x10;
    uVar1 = DAT_0800adc0 & uVar1;
    if (0x5f < uVar2) {
      uVar1 = uVar1 + 4;
    }
    param_1[0x17] =
         (uint)*(byte *)(DAT_0800adbc +
                        ((uint)((int)((ulonglong)DAT_0800adb8 * (ulonglong)uVar2 >> 0x20) << 0x19)
                        >> 0x1d));
    param_1[0x16] = uVar1;
  }
  else {
    uVar1 = uVar1 & 0xffffff00;
    param_1[0x16] = uVar1;
  }
  return uVar1;
}



/* === 0800adc4 FUN_0800adc4 === */

void FUN_0800adc4(uint *param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  iVar1 = DAT_0800ae84;
  uVar2 = *param_1;
  if ((uVar2 == DAT_0800ae70 + 0x50 ||
       (uVar2 == DAT_0800ae78 + 0x28 ||
       (uVar2 == DAT_0800ae70 + 0x28 ||
       (uVar2 == DAT_0800ae78 ||
       (uVar2 == DAT_0800ae70 || (uVar2 == DAT_0800ae74 || uVar2 == DAT_0800ae6c)))))) ||
     (uVar2 == DAT_0800ae7c)) {
    uVar2 = (uint)((ulonglong)DAT_0800ae80 * (ulonglong)((uVar2 & 0xff) - 8) >> 0x20);
    param_1[0x19] = DAT_0800ae88;
    param_1[0x18] = (iVar1 + (uVar2 >> 4)) * 4;
    param_1[0x1a] = 1 << ((uVar2 << 0x17) >> 0x1b);
  }
  else {
    uVar3 = (uint)((ulonglong)DAT_0800ae90 * (ulonglong)((uVar2 & 0xff) - 0x10) >> 0x24);
    if (DAT_0800ae8c + uVar2 < 0xa9) {
      uVar3 = uVar3 + 8;
    }
    iVar1 = DAT_0800ae94 + uVar3;
    param_1[0x19] = DAT_0800ae98;
    param_1[0x1a] = 1 << (uVar3 & 0x1f);
    param_1[0x18] = iVar1 * 4;
  }
  return;
}



/* === 0800ae9c FUN_0800ae9c === */

void FUN_0800ae9c(int *param_1)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(param_1 + 1);
  if (7 < uVar2 - 1) {
    return;
  }
  iVar1 = *param_1;
  if ((iVar1 == DAT_0800af28 + 100 ||
       (iVar1 == DAT_0800af28 + 0x50 ||
       (iVar1 == DAT_0800af28 + 0x3c ||
       (iVar1 == DAT_0800af28 + 0x28 ||
       (iVar1 == DAT_0800af28 + 0x14 || (iVar1 == DAT_0800af28 || iVar1 == DAT_0800af24)))))) ||
     (iVar1 == DAT_0800af2c)) {
    iVar1 = DAT_0800af30 + uVar2;
    param_1[0x1c] = DAT_0800af34;
    param_1[0x1b] = iVar1 * 4;
  }
  else {
    iVar1 = DAT_0800af38 + uVar2;
    param_1[0x1c] = DAT_0800af3c;
    param_1[0x1b] = iVar1 * 4;
  }
  param_1[0x1d] = 1 << (uVar2 - 1 & 0xff);
  return;
}



/* === 0800af40 FUN_0800af40 === */

undefined4 FUN_0800af40(uint **param_1)

{
  ulonglong uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  uint uVar7;
  uint *puVar8;
  uint *puVar9;
  uint uVar10;
  
  iVar2 = FUN_08009ce8();
  uVar7 = DAT_0800b394;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  puVar8 = *param_1;
  if ((((puVar8 == DAT_0800b19c) || (puVar8 == DAT_0800b19c + 6)) ||
      (puVar8 == DAT_0800b1a0 + 0x112 ||
       (puVar8 == DAT_0800b1a0 + 0x10c ||
       (puVar8 == DAT_0800b1a0 + 0x106 ||
       (puVar8 == DAT_0800b1a0 + 0x100 ||
       (puVar8 == DAT_0800b1a0 + 0xfa ||
       (puVar8 == DAT_0800b1a0 + 0xf4 ||
       (puVar8 == DAT_0800b1a0 + 0xee ||
       (puVar8 == DAT_0800b1a0 + 0x18 ||
       (puVar8 == DAT_0800b1a0 + 0x12 ||
       (puVar8 == DAT_0800b1a0 + 0xc ||
       (puVar8 == DAT_0800b1a0 + 6 || (puVar8 == DAT_0800b1a0 || puVar8 == DAT_0800b19c + 0xc)))))))
       )))))) || (puVar8 == DAT_0800b1a4)) {
    *(undefined *)(param_1 + 0xd) = 0;
    *(undefined *)((int)param_1 + 0x35) = 2;
    *puVar8 = *puVar8 & 0xfffffffe;
    while ((int)(*puVar8 << 0x1f) < 0) {
      iVar3 = FUN_08009ce8();
      if (5 < (uint)(iVar3 - iVar2)) {
        param_1[0x15] = (uint *)0x20;
        *(undefined *)((int)param_1 + 0x35) = 3;
        return 1;
      }
      puVar8 = *param_1;
    }
    puVar5 = param_1[6];
    uVar7 = (uint)param_1[2] | (uint)param_1[3] | (uint)param_1[4] | (uint)param_1[5] | (uint)puVar5
            | (uint)param_1[7] | (uint)param_1[8] | DAT_0800b1a8 & *puVar8;
    puVar4 = param_1[9];
    if (puVar4 == (uint *)&UndefinedInstruction) {
      puVar9 = param_1[0xb];
      uVar7 = uVar7 | (uint)param_1[0xc] | (uint)puVar9;
      if (0x1fffffff < (DAT_0800b380 & *DAT_0800b1ac)) goto LAB_0800b1c8;
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | 4;
LAB_0800b1f0:
      puVar4 = param_1[10];
      uVar7 = uVar7 | (uint)puVar4;
      if (puVar9 != (uint *)0x0) {
        if (puVar5 == (uint *)0x0) {
          if (puVar4 == (uint *)0x1) {
switchD_0800b28e_caseD_3:
            if (puVar9 == (uint *)0x1800000) {
switchD_0800b28e_caseD_0:
              param_1[0x15] = (uint *)0x40;
              *(undefined *)((int)param_1 + 0x35) = 1;
              return 1;
            }
          }
          else if (((uint)puVar4 & 0xfffffffd) == 0) goto switchD_0800b28e_caseD_1;
        }
        else if (puVar5 == (uint *)0x2000) {
          switch(puVar4) {
          case (uint *)0x0:
          case (uint *)0x2:
            goto switchD_0800b28e_caseD_0;
          case (uint *)0x1:
switchD_0800b28e_caseD_1:
            if ((int)puVar9 << 7 < 0) goto switchD_0800b28e_caseD_0;
            break;
          case (uint *)0x3:
            goto switchD_0800b28e_caseD_3;
          }
        }
        else {
          if (puVar4 < (uint *)0x3) goto switchD_0800b28e_caseD_0;
          if (puVar4 == (uint *)0x3) goto switchD_0800b28e_caseD_1;
        }
      }
    }
    else if ((DAT_0800b1b0 & *DAT_0800b1ac) < 0x20000000) {
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | (uint)puVar4;
    }
    else {
LAB_0800b1c8:
      uVar10 = (int)param_1[1] - 0x29;
      if (uVar10 < 0x20) {
        if ((int)((DAT_0800b37c >> (uVar10 & 0xff)) << 0x1f) < 0) goto LAB_0800b1d8;
      }
      else if ((int)param_1[1] - 0x4fU < 4) {
LAB_0800b1d8:
        uVar7 = uVar7 | 0x100000;
      }
      *puVar8 = uVar7;
      uVar7 = puVar8[5] & 0xfffffff8 | (uint)puVar4;
      if (puVar4 == (uint *)&UndefinedInstruction) {
        puVar9 = param_1[0xb];
        goto LAB_0800b1f0;
      }
    }
    puVar8[5] = uVar7;
    iVar2 = FUN_0800acd0(param_1);
    *(int *)(iVar2 + 8) = 0x3f << ((uint)param_1[0x17] & 0x1f);
  }
  else {
    if ((puVar8 != DAT_0800b38c + 0x14 &&
         (puVar8 != DAT_0800b388 + 0x14 &&
         (puVar8 != DAT_0800b38c + 10 &&
         (puVar8 != DAT_0800b388 + 10 &&
         (puVar8 != DAT_0800b38c && (puVar8 != DAT_0800b388 && puVar8 != DAT_0800b384)))))) &&
       (puVar8 != DAT_0800b390)) {
      param_1[0x15] = (uint *)0x40;
      *(undefined *)((int)param_1 + 0x35) = 3;
      return 1;
    }
    *(undefined *)((int)param_1 + 0x35) = 2;
    *(undefined *)(param_1 + 0xd) = 0;
    if (param_1[2] == (uint *)0x40) {
      uVar10 = 0x10;
    }
    else if (param_1[2] == (uint *)0x80) {
      uVar10 = 0x4000;
    }
    else {
      uVar10 = 0;
    }
    uVar6 = DAT_0800b398 + (int)puVar8;
    uVar1 = (ulonglong)DAT_0800b39c;
    *puVar8 = ((uint)param_1[4] | (uint)param_1[3] | (uint)param_1[5] | (uint)param_1[6] |
              (uint)param_1[7]) >> 3 | (uint)param_1[8] >> 4 | uVar7 & *puVar8 | uVar10;
    param_1[0x17] = (uint *)((uint)(uVar1 * uVar6 >> 0x24) << 2);
    iVar2 = FUN_0800acd0(param_1);
    *(int *)(iVar2 + 4) = 1 << ((uint)param_1[0x17] & 0x1f);
  }
  puVar8 = *param_1;
  if ((puVar8 == DAT_0800b1bc + 0x1e ||
       (puVar8 == DAT_0800b1c0 + 0x14 ||
       (puVar8 == DAT_0800b1bc + 0x14 ||
       (puVar8 == DAT_0800b1c0 + 10 ||
       (puVar8 == DAT_0800b1bc + 10 ||
       (puVar8 == DAT_0800b1c0 ||
       (puVar8 == DAT_0800b1bc ||
       (puVar8 == DAT_0800b1b4 + 0x124 ||
       (puVar8 == DAT_0800b1b8 + 0x118 ||
       (puVar8 == DAT_0800b1b4 + 0x118 ||
       (puVar8 == DAT_0800b1b8 + 0x10c ||
       (puVar8 == DAT_0800b1b4 + 0x10c ||
       (puVar8 == DAT_0800b1b8 + 0x100 ||
       (puVar8 == DAT_0800b1b4 + 0x100 ||
       (puVar8 == DAT_0800b1b8 + 0xf4 ||
       (puVar8 == DAT_0800b1b4 + 0x24 ||
       (puVar8 == DAT_0800b1b8 + 0x18 ||
       (puVar8 == DAT_0800b1b4 + 0x18 ||
       (puVar8 == DAT_0800b1b8 + 0xc ||
       (puVar8 == DAT_0800b1b4 + 0xc ||
       (puVar8 == DAT_0800b1b8 || (puVar8 == DAT_0800b1b4 || puVar8 == DAT_0800b19c)))))))))))))))))
       ))))) || (puVar8 == DAT_0800b1c4)) {
    FUN_0800adc4(param_1);
    if (param_1[2] == (uint *)0x80) {
      puVar8 = param_1[0x1a];
      puVar4 = param_1[0x19];
      param_1[1] = (uint *)0x0;
      *param_1[0x18] = 0;
      puVar4[1] = (uint)puVar8;
    }
    else {
      puVar8 = param_1[1];
      *param_1[0x18] = (uint)puVar8 & 0xff;
      param_1[0x19][1] = (uint)param_1[0x1a];
      if ((int)puVar8 - 1U < 8) {
        FUN_0800ae9c();
        puVar4 = param_1[0x1c];
        puVar8 = param_1[0x1d];
        *param_1[0x1b] = 0;
        puVar4[1] = (uint)puVar8;
        goto LAB_0800b18e;
      }
    }
    param_1[0x1b] = (uint *)0x0;
    param_1[0x1c] = (uint *)0x0;
    param_1[0x1d] = (uint *)0x0;
  }
LAB_0800b18e:
  param_1[0x15] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x35) = 1;
  return 0;
}



/* === 0800b3a0 FUN_0800b3a0 === */

undefined4 FUN_0800b3a0(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)(param_1 + 0xd) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0xd) = 1;
  if (*(char *)((int)param_1 + 0x35) != '\x01') {
    param_1[0x15] = (uint *)0x800;
    *(undefined *)(param_1 + 0xd) = 0;
    return 1;
  }
  *(undefined *)((int)param_1 + 0x35) = 2;
  param_1[0x15] = (uint *)0x0;
  **param_1 = **param_1 & 0xfffffffe;
  FUN_0800ab00(param_1);
  puVar2 = *param_1;
  if (((puVar2 == DAT_0800b538 || puVar2 == DAT_0800b534) ||
      (puVar2 == DAT_0800b53c + 0x118 ||
       (puVar2 == DAT_0800b53c + 0x112 ||
       (puVar2 == DAT_0800b53c + 0x10c ||
       (puVar2 == DAT_0800b53c + 0x106 ||
       (puVar2 == DAT_0800b53c + 0x100 ||
       (puVar2 == DAT_0800b53c + 0xfa ||
       (puVar2 == DAT_0800b53c + 0xf4 ||
       (puVar2 == DAT_0800b53c + 0x1e ||
       (puVar2 == DAT_0800b53c + 0x18 ||
       (puVar2 == DAT_0800b53c + 0x12 ||
       (puVar2 == DAT_0800b53c + 0xc || (puVar2 == DAT_0800b53c || puVar2 == DAT_0800b538 + 0xc)))))
       )))))))) || (puVar2 == DAT_0800b540)) {
    puVar1 = param_1[0x10];
    *puVar2 = *puVar2 & 0xffffffe1 | 0x16;
    if (puVar1 != (uint *)0x0) {
      *puVar2 = *puVar2 | 8;
    }
  }
  else {
    puVar1 = param_1[0x10];
    *puVar2 = *puVar2 & 0xfffffff1 | 10;
    if (puVar1 != (uint *)0x0) {
      *puVar2 = *puVar2 | 4;
    }
    if ((puVar2 != DAT_0800b548 + 0x1e &&
         (puVar2 != DAT_0800b548 + 0x19 &&
         (puVar2 != DAT_0800b548 + 0x14 &&
         (puVar2 != DAT_0800b548 + 0xf &&
         (puVar2 != DAT_0800b548 + 10 && (puVar2 != DAT_0800b548 && puVar2 != DAT_0800b544)))))) &&
       (puVar2 != DAT_0800b54c)) goto LAB_0800b4b6;
  }
  puVar1 = param_1[0x18];
  if ((int)(*puVar1 << 0xf) < 0) {
    *puVar1 = *puVar1 | 0x100;
  }
  puVar1 = param_1[0x1b];
  if (puVar1 != (uint *)0x0) {
    *puVar1 = *puVar1 | 0x100;
  }
LAB_0800b4b6:
  *puVar2 = *puVar2 | 1;
  return 0;
}



/* === 0800b550 FUN_0800b550 === */

undefined4 FUN_0800b550(uint **param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  bool bVar7;
  
  iVar2 = FUN_08009ce8();
  puVar5 = DAT_0800b7c0;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x35) != '\x02') {
    param_1[0x15] = (uint *)0x80;
    *(undefined *)(param_1 + 0xd) = 0;
    return 1;
  }
  puVar6 = *param_1;
  if (puVar6 == DAT_0800b7b8 || puVar6 == DAT_0800b7b4) {
    bVar1 = true;
LAB_0800b610:
    *puVar6 = *puVar6 & 0xffffffe1;
    puVar6[5] = puVar6[5] & 0xffffff7f;
    if (bVar1) {
      *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    }
    else {
LAB_0800b786:
      *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    }
  }
  else {
    bVar1 = false;
    if (puVar6 == DAT_0800b7b0 + 0x11e ||
        (puVar6 == DAT_0800b7b0 + 0x118 ||
        (puVar6 == DAT_0800b7b0 + 0x112 ||
        (puVar6 == DAT_0800b7b0 + 0x10c ||
        (puVar6 == DAT_0800b7b0 + 0x106 ||
        (puVar6 == DAT_0800b7b0 + 0x100 ||
        (puVar6 == DAT_0800b7b0 + 0xfa ||
        (puVar6 == DAT_0800b7b0 + 0xf4 ||
        (puVar6 == DAT_0800b7b0 + 0x1e ||
        (puVar6 == DAT_0800b7b0 + 0x18 ||
        (puVar6 == DAT_0800b7b0 + 0x12 ||
        (puVar6 == DAT_0800b7b0 + 0xc || (puVar6 == DAT_0800b7b0 || puVar6 == DAT_0800b7ac))))))))))
        ))) goto LAB_0800b610;
    *puVar6 = *puVar6 & 0xfffffff1;
    if ((puVar6 == puVar5 + 0x1e ||
         (puVar6 == puVar5 + 0x19 ||
         (puVar6 == puVar5 + 0x14 ||
         (puVar6 == puVar5 + 0xf ||
         (puVar6 == puVar5 + 10 || (puVar6 == puVar5 || puVar6 == DAT_0800b7c4)))))) ||
       (puVar6 == DAT_0800b7c8)) goto LAB_0800b786;
  }
  *puVar6 = *puVar6 & 0xfffffffe;
  while ((int)(*puVar6 << 0x1f) < 0) {
    iVar3 = FUN_08009ce8();
    if (5 < (uint)(iVar3 - iVar2)) {
      param_1[0x15] = (uint *)0x20;
      *(undefined *)(param_1 + 0xd) = 0;
      *(undefined *)((int)param_1 + 0x35) = 3;
      return 1;
    }
  }
  puVar5 = *param_1;
  if ((puVar5 == DAT_0800b7bc + 0x112 ||
       (puVar5 == DAT_0800b7bc + 0x10c ||
       (puVar5 == DAT_0800b7b0 + 0x112 ||
       (puVar5 == DAT_0800b7bc + 0x100 ||
       (puVar5 == DAT_0800b7b0 + 0x106 ||
       (puVar5 == DAT_0800b7bc + 0xf4 ||
       (puVar5 == DAT_0800b7b0 + 0xfa ||
       (puVar5 == DAT_0800b7bc + 0xe8 ||
       (puVar5 == DAT_0800b7b0 + 0x1e ||
       (puVar5 == DAT_0800b7bc + 0xc ||
       (puVar5 == DAT_0800b7b0 + 0x12 ||
       (puVar5 == DAT_0800b7bc || (puVar5 == DAT_0800b7b0 || puVar5 == DAT_0800b7ac))))))))))))) ||
     (puVar5 == DAT_0800b7b8 || puVar5 == DAT_0800b7bc + -0x18)) {
    param_1[0x16][2] = 0x3f << ((uint)param_1[0x17] & 0x1f);
  }
  else {
    bVar7 = puVar5 != DAT_0800b830;
    bVar1 = puVar5 != DAT_0800b82c;
    param_1[0x16][1] = 1 << ((uint)param_1[0x17] & 0x1f);
    if ((puVar5 != DAT_0800b838 + 0xf &&
         (puVar5 != DAT_0800b838 + 10 &&
         (puVar5 != DAT_0800b834 + 10 &&
         (puVar5 != DAT_0800b838 && (puVar5 != DAT_0800b834 && (bVar7 && bVar1)))))) &&
       (puVar5 != DAT_0800b83c)) goto LAB_0800b712;
  }
  puVar5 = param_1[0x1b];
  param_1[0x19][1] = (uint)param_1[0x1a];
  if (puVar5 != (uint *)0x0) {
    puVar4 = param_1[0x1c];
    puVar6 = param_1[0x1d];
    *puVar5 = *puVar5 & 0xfffffeff;
    puVar4[1] = (uint)puVar6;
  }
LAB_0800b712:
  *(undefined *)((int)param_1 + 0x35) = 1;
  *(undefined *)(param_1 + 0xd) = 0;
  return 0;
}



/* === 0800b840 FUN_0800b840 === */

undefined4 FUN_0800b840(uint **param_1)

{
  bool bVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  bool bVar5;
  
  puVar2 = DAT_0800b9c8;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x35) != '\x02') {
    param_1[0x15] = (uint *)0x80;
    return 1;
  }
  puVar3 = *param_1;
  if ((((puVar3 == DAT_0800b9b0) || (puVar3 == DAT_0800b9b0 + 6)) ||
      (puVar3 == DAT_0800b9b8 + 0x10c ||
       (puVar3 == DAT_0800b9b4 + 0x112 ||
       (puVar3 == DAT_0800b9b8 + 0x100 ||
       (puVar3 == DAT_0800b9b4 + 0x106 ||
       (puVar3 == DAT_0800b9b8 + 0xf4 ||
       (puVar3 == DAT_0800b9b4 + 0xfa ||
       (puVar3 == DAT_0800b9b8 + 0xe8 ||
       (puVar3 == DAT_0800b9b4 + 0x1e ||
       (puVar3 == DAT_0800b9b8 + 0xc ||
       (puVar3 == DAT_0800b9b4 + 0x12 ||
       (puVar3 == DAT_0800b9b8 || (puVar3 == DAT_0800b9b4 || puVar3 == DAT_0800b9b0 + 0x12))))))))))
       ))) || (puVar3 == DAT_0800b9bc)) {
    *(undefined *)((int)param_1 + 0x35) = 4;
    *puVar3 = *puVar3 & 0xfffffffe;
    return 0;
  }
  bVar5 = puVar3 == DAT_0800b9c4;
  bVar1 = puVar3 == DAT_0800b9c0;
  puVar4 = DAT_0800b9c4 + 0xf;
  *puVar3 = *puVar3 & 0xfffffff1;
  *puVar3 = *puVar3 & 0xfffffffe;
  if ((puVar3 == DAT_0800b9cc ||
       (puVar3 == puVar2 + 0xf ||
       (puVar3 == puVar2 + 10 || (puVar3 == puVar4 || (puVar3 == puVar2 || (bVar5 || bVar1)))))) ||
     (puVar3 == DAT_0800b9d0)) {
    puVar3 = param_1[0x16];
    puVar2 = param_1[0x17];
    *param_1[0x18] = *param_1[0x18] & 0xfffffeff;
    puVar3[1] = 1 << ((uint)puVar2 & 0x1f);
    puVar2 = param_1[0x1b];
    param_1[0x19][1] = (uint)param_1[0x1a];
    if (puVar2 != (uint *)0x0) {
      puVar3 = param_1[0x1c];
      puVar4 = param_1[0x1d];
      *puVar2 = *puVar2 & 0xfffffeff;
      puVar3[1] = (uint)puVar4;
    }
  }
  *(undefined *)((int)param_1 + 0x35) = 1;
  *(undefined *)(param_1 + 0xd) = 0;
  if (param_1[0x14] == (uint *)0x0) {
    return 0;
  }
  (*(code *)param_1[0x14])(param_1);
  return 0;
}



/* === 0800b9d4 FUN_0800b9d4 === */

/* WARNING: Type propagation algorithm not settling */

void FUN_0800b9d4(uint **param_1)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  undefined uVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  uint *UNRECOVERED_JUMPTABLE_00;
  uint uVar8;
  bool bVar9;
  uint local_24;
  
  uVar7 = *DAT_0800bc4c;
  local_24 = 0;
  puVar5 = *param_1;
  puVar6 = param_1[0x16];
  uVar8 = *puVar6;
  UNRECOVERED_JUMPTABLE_00 = DAT_0800bc50;
  if (puVar5 == DAT_0800bc54 || puVar5 == DAT_0800bc50) {
    UNRECOVERED_JUMPTABLE_00 = (uint *)0x1;
  }
  uVar2 = *puVar6;
  if (((puVar5 != DAT_0800bc54 && puVar5 != DAT_0800bc50) &&
      (UNRECOVERED_JUMPTABLE_00 = (uint *)0x0,
      puVar5 != DAT_0800bc58 + 0x112 &&
      (puVar5 != DAT_0800bc58 + 0x10c &&
      (puVar5 != DAT_0800bc58 + 0x106 &&
      (puVar5 != DAT_0800bc58 + 0x100 &&
      (puVar5 != DAT_0800bc58 + 0xfa &&
      (puVar5 != DAT_0800bc58 + 0xf4 &&
      (puVar5 != DAT_0800bc58 + 0xee &&
      (puVar5 != DAT_0800bc58 + 0x18 &&
      (puVar5 != DAT_0800bc58 + 0x12 &&
      (puVar5 != DAT_0800bc58 + 0xc &&
      (puVar5 != DAT_0800bc58 + 6 && (puVar5 != DAT_0800bc58 && puVar5 != DAT_0800bc54 + 6))))))))))
      ))) && (puVar5 != DAT_0800bc5c)) {
    if ((puVar5 != DAT_0800c06c + 0x19 &&
         (puVar5 != DAT_0800c06c + 0x14 &&
         (puVar5 != DAT_0800c06c + 0xf &&
         (puVar5 != DAT_0800c06c + 10 &&
         (puVar5 != DAT_0800c06c + 5 && (puVar5 != DAT_0800c06c && puVar5 != DAT_0800c068)))))) &&
       (puVar5 != DAT_0800c070)) {
      return;
    }
    uVar7 = *puVar5;
    uVar8 = (uint)param_1[0x17] & 0x1f;
    if (((4 << uVar8 & uVar2) == 0) || (-1 < (int)(uVar7 << 0x1d))) {
      if (((2 << uVar8 & uVar2) == 0) || (-1 < (int)(uVar7 << 0x1e))) {
        if ((8 << uVar8 & uVar2) == 0) {
          return;
        }
        if (-1 < (int)(uVar7 << 0x1c)) {
          return;
        }
        *puVar5 = *puVar5 & 0xfffffff1;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x13];
        puVar6[1] = 1 << uVar8;
        param_1[0x15] = (uint *)0x1;
        *(undefined *)(param_1 + 0xd) = 0;
        *(undefined *)((int)param_1 + 0x35) = 1;
        if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) {
          return;
        }
                    /* WARNING: Could not recover jumptable at 0x0800c02a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
        return;
      }
      puVar6[1] = 2 << uVar8;
      if ((int)(uVar7 << 0x10) < 0) {
        if (-1 < (int)(uVar7 << 0xf)) {
          UNRECOVERED_JUMPTABLE_00 = param_1[0x11];
          goto joined_r0x0800bf9e;
        }
      }
      else if ((uVar7 & 0x20) == 0) {
        *puVar5 = *puVar5 & 0xfffffff5;
        *(undefined *)((int)param_1 + 0x35) = 1;
        *(undefined *)(param_1 + 0xd) = 0;
      }
      UNRECOVERED_JUMPTABLE_00 = param_1[0xf];
    }
    else {
      puVar6[1] = 4 << uVar8;
      if ((int)(uVar7 << 0x10) < 0) {
        if (-1 < (int)(uVar7 << 0xf)) {
          UNRECOVERED_JUMPTABLE_00 = param_1[0x12];
          goto joined_r0x0800bf9e;
        }
      }
      else if (-1 < (int)(uVar7 << 0x1a)) {
        *puVar5 = *puVar5 & 0xfffffffb;
      }
      UNRECOVERED_JUMPTABLE_00 = param_1[0x10];
    }
    goto joined_r0x0800bf9e;
  }
  puVar3 = param_1[0x17];
  uVar2 = (uint)puVar3 & 0x1f;
  if ((uVar8 & 8 << uVar2) == 0) {
    if ((int)((uVar8 >> uVar2) << 0x1f) < 0) {
LAB_0800baa8:
      if ((int)(puVar5[5] << 0x18) < 0) {
        puVar6[2] = 1 << uVar2;
        param_1[0x15] = (uint *)((uint)param_1[0x15] | 2);
      }
    }
LAB_0800bac2:
    if ((4 << uVar2 & uVar8) == 0) {
LAB_0800bb86:
      if ((0x10 << uVar2 & uVar8) != 0) {
        if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) goto LAB_0800bb96;
        goto LAB_0800bc22;
      }
    }
    else {
      if (((UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) ||
          (puVar5 == DAT_0800bc60 + 0x118 ||
           (puVar5 == DAT_0800bc60 + 0x112 ||
           (puVar5 == DAT_0800bc60 + 0x10c ||
           (puVar5 == DAT_0800bc60 + 0x106 ||
           (puVar5 == DAT_0800bc60 + 0x100 ||
           (puVar5 == DAT_0800bc60 + 0xfa ||
           (puVar5 == DAT_0800bc60 + 0xf4 ||
           (puVar5 == DAT_0800bc60 + 0x1e ||
           (puVar5 == DAT_0800bc60 + 0x18 ||
           (puVar5 == DAT_0800bc60 + 0x12 ||
           (puVar5 == DAT_0800bc60 + 0xc || (puVar5 == DAT_0800bc60 || puVar5 == DAT_0800bc58)))))))
           )))))) || (puVar5 == DAT_0800bc5c)) {
        if ((int)(*puVar5 << 0x1e) < 0) {
          puVar6[2] = 4 << uVar2;
          param_1[0x15] = (uint *)((uint)param_1[0x15] | 4);
        }
        goto LAB_0800bb86;
      }
LAB_0800bfa4:
      if ((uVar8 & 0x10 << uVar2) != 0) goto LAB_0800bfb4;
    }
  }
  else {
    if ((int)(*puVar5 << 0x1d) < 0) {
      *puVar5 = *puVar5 & 0xfffffffb;
      puVar6[2] = 8 << uVar2;
      param_1[0x15] = (uint *)((uint)param_1[0x15] | 1);
    }
    if (-1 < (int)((uVar8 >> uVar2) << 0x1f)) goto LAB_0800bac2;
    if ((puVar5 == DAT_0800bf28 + 0x11e ||
         (puVar5 == DAT_0800bf28 + 0x118 ||
         (puVar5 == DAT_0800bf28 + 0x112 ||
         (puVar5 == DAT_0800bf28 + 0x10c ||
         (puVar5 == DAT_0800bf28 + 0x106 ||
         (puVar5 == DAT_0800bf28 + 0x100 ||
         (puVar5 == DAT_0800bf28 + 0xfa ||
         (puVar5 == DAT_0800bf28 + 0xf4 ||
         (puVar5 == DAT_0800bf28 + 0x1e ||
         (puVar5 == DAT_0800bf28 + 0x18 ||
         (puVar5 == DAT_0800bf28 + 0x12 ||
         (puVar5 == DAT_0800bf28 + 0xc || (puVar5 == DAT_0800bf28 || puVar5 == DAT_0800bf24)))))))))
         )))) || (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0)) goto LAB_0800baa8;
    if ((4 << uVar2 & uVar8) != 0) goto LAB_0800bfa4;
    if ((uVar8 & 0x10 << uVar2) == 0) goto LAB_0800bc6c;
LAB_0800bb96:
    if ((puVar5 == DAT_0800bc60 + 0x118 ||
         (puVar5 == DAT_0800bc60 + 0x112 ||
         (puVar5 == DAT_0800bc60 + 0x10c ||
         (puVar5 == DAT_0800bc60 + 0x106 ||
         (puVar5 == DAT_0800bc60 + 0x100 ||
         (puVar5 == DAT_0800bc60 + 0xfa ||
         (puVar5 == DAT_0800bc60 + 0xf4 ||
         (puVar5 == DAT_0800bc60 + 0x1e ||
         (puVar5 == DAT_0800bc60 + 0x18 ||
         (puVar5 == DAT_0800bc60 + 0x12 ||
         (puVar5 == DAT_0800bc60 + 0xc || (puVar5 == DAT_0800bc60 || puVar5 == DAT_0800bc58)))))))))
         ))) || (puVar5 == DAT_0800bc5c)) {
LAB_0800bc22:
      iVar1 = *puVar5 << 0x1c;
    }
    else {
LAB_0800bfb4:
      iVar1 = *puVar5 << 0x1d;
    }
    if (iVar1 < 0) {
      puVar6[2] = 0x10 << uVar2;
      if ((int)(*puVar5 << 0xd) < 0) {
        if (-1 < (int)(*puVar5 << 0xc)) goto LAB_0800bc42;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x12];
      }
      else {
        if (-1 < (int)(*puVar5 << 0x17)) {
          *puVar5 = *puVar5 & 0xfffffff7;
        }
LAB_0800bc42:
        UNRECOVERED_JUMPTABLE_00 = param_1[0x10];
      }
      if (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) {
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
        puVar3 = param_1[0x17];
      }
    }
  }
LAB_0800bc6c:
  uVar2 = 0x20 << ((uint)puVar3 & 0x1f);
  if ((uVar2 & uVar8) != 0) {
    UNRECOVERED_JUMPTABLE_00 = *param_1;
    if ((UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x11e ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x118 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x112 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x10c ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x106 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x100 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0xfa ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x24 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x1e ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x18 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0x12 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 0xc ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 + 6 ||
         (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf18 || UNRECOVERED_JUMPTABLE_00 == DAT_0800bf14))))))
         )))))))) || (UNRECOVERED_JUMPTABLE_00 == DAT_0800bf1c)) {
      iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1b;
    }
    else {
      iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1e;
    }
    if (iVar1 < 0) {
      puVar6[2] = uVar2;
      if (*(char *)((int)param_1 + 0x35) == '\x04') {
        *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xffffffe9;
        UNRECOVERED_JUMPTABLE_00[5] = UNRECOVERED_JUMPTABLE_00[5] & 0xffffff7f;
        if ((param_1[0x10] != (uint *)0x0) || (param_1[0x12] != (uint *)0x0)) {
          *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xfffffff7;
        }
        UNRECOVERED_JUMPTABLE_00 = param_1[0x14];
        puVar6[2] = 0x3f << ((uint)puVar3 & 0x1f);
        *(undefined *)((int)param_1 + 0x35) = 1;
        *(undefined *)(param_1 + 0xd) = 0;
        goto joined_r0x0800bf9e;
      }
      if ((*UNRECOVERED_JUMPTABLE_00 & 0x40000) == 0) {
        if ((*UNRECOVERED_JUMPTABLE_00 & 0x100) == 0) {
          *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xffffffef;
          *(undefined *)(param_1 + 0xd) = 0;
          *(undefined *)((int)param_1 + 0x35) = 1;
        }
LAB_0800bd40:
        UNRECOVERED_JUMPTABLE_00 = param_1[0xf];
      }
      else {
        if ((int)(*UNRECOVERED_JUMPTABLE_00 << 0xc) < 0) goto LAB_0800bd40;
        UNRECOVERED_JUMPTABLE_00 = param_1[0x11];
      }
      if (UNRECOVERED_JUMPTABLE_00 != (uint *)0x0) {
        (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
      }
    }
  }
  if (param_1[0x15] == (uint *)0x0) {
    return;
  }
  if ((int)param_1[0x15] << 0x1f < 0) {
    UNRECOVERED_JUMPTABLE_00 = *param_1;
    *(undefined *)((int)param_1 + 0x35) = 4;
    *UNRECOVERED_JUMPTABLE_00 = *UNRECOVERED_JUMPTABLE_00 & 0xfffffffe;
    do {
      local_24 = local_24 + 1;
      if ((uint)((ulonglong)DAT_0800bf20 * (ulonglong)uVar7 >> 0x2a) < local_24) break;
    } while ((int)(*UNRECOVERED_JUMPTABLE_00 << 0x1f) < 0);
    iVar1 = *UNRECOVERED_JUMPTABLE_00 << 0x1f;
    bVar9 = iVar1 < 0;
    if (bVar9) {
      iVar1 = 3;
    }
    uVar4 = (undefined)iVar1;
    if (!bVar9) {
      uVar4 = 1;
    }
    *(undefined *)((int)param_1 + 0x35) = uVar4;
    *(undefined *)(param_1 + 0xd) = 0;
  }
  UNRECOVERED_JUMPTABLE_00 = param_1[0x13];
joined_r0x0800bf9e:
  if (UNRECOVERED_JUMPTABLE_00 == (uint *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x0800bdae. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)UNRECOVERED_JUMPTABLE_00)(param_1);
  return;
}



/* === 0800c074 FUN_0800c074 === */

undefined FUN_0800c074(int param_1)

{
  return *(undefined *)(param_1 + 0x35);
}



/* === 0800c07c FUN_0800c07c === */

undefined4 FUN_0800c07c(int param_1)

{
  return *(undefined4 *)(param_1 + 0x54);
}



/* === 0800c080 FUN_0800c080 === */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0800c080(uint *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  
  iVar1 = DAT_0800c2cc;
  uVar4 = *param_2;
  if (uVar4 != 0) {
    uVar10 = 0;
    uVar3 = 0;
    do {
      uVar2 = 1 << (uVar3 & 0xff);
      uVar11 = uVar2 & uVar4;
      if (uVar11 != 0) {
        uVar5 = param_2[1];
        uVar8 = uVar5 & 3;
        if (uVar8 - 1 < 2) {
          uVar7 = 3 << (uVar10 & 0xff);
          param_1[2] = param_2[3] << (uVar10 & 0xff) | param_1[2] & ~uVar7;
          param_1[1] = param_1[1] & ~uVar2 | ((uVar5 << 0x1b) >> 0x1f) << (uVar3 & 0xff);
LAB_0800c210:
          uVar7 = ~uVar7;
          uVar2 = uVar8 << (uVar10 & 0xff);
          param_1[3] = param_2[2] << (uVar10 & 0xff) | param_1[3] & uVar7;
          if (uVar8 == 2) {
            iVar9 = (uVar3 & 7) << 2;
            param_1[(uVar3 >> 3) + 8] =
                 param_2[4] << iVar9 | param_1[(uVar3 >> 3) + 8] & ~(0xf << iVar9);
          }
        }
        else {
          if (uVar8 != 3) {
            uVar7 = 3 << (uVar10 & 0xff);
            goto LAB_0800c210;
          }
          uVar2 = 3 << (uVar10 & 0xff);
          uVar7 = ~uVar2;
        }
        *param_1 = uVar2 | *param_1 & uVar7;
        if ((uVar5 & 0x30000) != 0) {
          iVar9 = (uVar3 & 3) << 2;
          *(uint *)(iVar1 + 0xf4) = *(uint *)(iVar1 + 0xf4) | 2;
          uVar2 = *(uint *)((uVar3 & 0xfffffffc) + 0x58000408) & ~(0xf << iVar9);
          if (param_1 != DAT_0800c2d0) {
            if (param_1 == DAT_0800c2d0 + 0x100) {
              uVar2 = uVar2 | 1 << iVar9;
            }
            else if (param_1 == DAT_0800c2d4) {
              uVar2 = uVar2 | 2 << iVar9;
            }
            else if (param_1 == DAT_0800c2d8) {
              uVar2 = uVar2 | 3 << iVar9;
            }
            else if (param_1 == DAT_0800c2dc) {
              uVar2 = uVar2 | 4 << iVar9;
            }
            else if (param_1 == DAT_0800c2e0) {
              uVar2 = uVar2 | 5 << iVar9;
            }
            else if (param_1 == DAT_0800c2e4) {
              uVar2 = uVar2 | 6 << iVar9;
            }
            else if (param_1 == DAT_0800c2e8) {
              uVar2 = uVar2 | 7 << iVar9;
            }
            else if (param_1 == DAT_0800c2ec) {
              uVar2 = uVar2 | 8 << iVar9;
            }
            else {
              if (param_1 == DAT_0800c2f0) {
                iVar6 = 9;
              }
              else {
                iVar6 = 10;
              }
              uVar2 = uVar2 | iVar6 << iVar9;
            }
          }
          *(uint *)((uVar3 & 0xfffffffc) + 0x58000408) = uVar2;
          uVar2 = ~uVar11;
          if ((int)(uVar5 << 0xb) < 0) {
            _DAT_58000000 = uVar11 | _DAT_58000000;
          }
          else {
            _DAT_58000000 = uVar2 & _DAT_58000000;
          }
          if ((int)(uVar5 << 10) < 0) {
            _DAT_58000004 = uVar11 | _DAT_58000004;
          }
          else {
            _DAT_58000004 = uVar2 & _DAT_58000004;
          }
          if ((int)(uVar5 << 0xe) < 0) {
            _DAT_58000084 = uVar11 | _DAT_58000084;
          }
          else {
            _DAT_58000084 = uVar2 & _DAT_58000084;
          }
          if ((int)(uVar5 << 0xf) < 0) {
            _DAT_58000080 = uVar11 | _DAT_58000080;
          }
          else {
            _DAT_58000080 = uVar2 & _DAT_58000080;
          }
        }
      }
      uVar3 = uVar3 + 1;
      uVar10 = uVar10 + 2;
    } while (uVar4 >> (uVar3 & 0xff) != 0);
  }
  return;
}



/* === 0800c2f4 FUN_0800c2f4 === */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0800c2f4(uint *param_1,uint param_2)

{
  int iVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  
  puVar4 = DAT_0800c478;
  puVar3 = DAT_0800c474;
  puVar2 = DAT_0800c470;
  iVar1 = DAT_0800c46c;
  if (param_2 == 0) {
    return;
  }
  uVar6 = 0;
  do {
    iVar11 = 1;
    uVar5 = 1 << (uVar6 & 0xff);
    uVar10 = uVar5 & param_2;
    if (uVar10 != 0) {
      iVar9 = (uVar6 & 0xfffffffc) + iVar1;
      iVar7 = (uVar6 & 3) << 2;
      if (param_1 == puVar2) {
        uVar8 = 0;
      }
      else if (param_1 == puVar3) {
LAB_0800c412:
        uVar8 = iVar11 << iVar7;
      }
      else if (param_1 == puVar4) {
        uVar8 = 2 << iVar7;
      }
      else if (param_1 == DAT_0800c458) {
        uVar8 = 3 << iVar7;
      }
      else if (param_1 == DAT_0800c45c) {
        uVar8 = 4 << iVar7;
      }
      else if (param_1 == DAT_0800c460) {
        uVar8 = 5 << iVar7;
      }
      else {
        if (param_1 == DAT_0800c464) {
          iVar11 = 6;
          goto LAB_0800c412;
        }
        if (param_1 == DAT_0800c468) {
          uVar8 = 7 << iVar7;
        }
        else if (param_1 == DAT_0800c47c) {
          uVar8 = 8 << iVar7;
        }
        else {
          if (param_1 == DAT_0800c480) {
            iVar11 = 9;
          }
          else {
            iVar11 = 10;
          }
          uVar8 = iVar11 << iVar7;
        }
      }
      if ((*(uint *)(iVar9 + 8) & 0xf << iVar7) == uVar8) {
        _DAT_58000080 = _DAT_58000080 & ~uVar10;
        _DAT_58000084 = _DAT_58000084 & ~uVar10;
        _DAT_58000004 = _DAT_58000004 & ~uVar10;
        _DAT_58000000 = _DAT_58000000 & ~uVar10;
        *(uint *)(iVar9 + 8) = *(uint *)(iVar9 + 8) & ~(0xf << iVar7);
      }
      uVar10 = 3 << ((uVar6 & 0x7f) << 1);
      *param_1 = *param_1 | uVar10;
      param_1[(uVar6 >> 3) + 8] = param_1[(uVar6 >> 3) + 8] & ~(0xf << ((uVar6 & 7) << 2));
      param_1[3] = param_1[3] & ~uVar10;
      param_1[1] = param_1[1] & ~uVar5;
      param_1[2] = param_1[2] & ~uVar10;
    }
    uVar6 = uVar6 + 1;
    if (param_2 >> (uVar6 & 0xff) == 0) {
      return;
    }
  } while( true );
}



/* === 0800c484 FUN_0800c484 === */

bool FUN_0800c484(int param_1,uint param_2)

{
  return (param_2 & *(uint *)(param_1 + 0x10)) != 0;
}



/* === 0800c490 FUN_0800c490 === */

void FUN_0800c490(int param_1,int param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = param_2 << 0x10;
  }
  *(int *)(param_1 + 0x18) = param_2;
  return;
}



/* === 0800c498 FUN_0800c498 === */

void FUN_0800c498(int *param_1)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  undefined4 uVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  uint uVar10;
  int iVar11;
  uint uVar12;
  uint local_2c;
  
  iVar7 = *param_1;
  iVar3 = FUN_08012bcc(iVar7);
  if ((iVar3 == 1) && (iVar3 = FUN_08012b60(*param_1), iVar3 != 0)) {
    uVar4 = FUN_08012b60(*param_1);
    if ((uVar4 & 0x200000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x200000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 0x100000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x100000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 0x4000000) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 0x4000000;
    }
    uVar4 = FUN_08012b60();
    if ((uVar4 & 2) != 0) {
      *(undefined4 *)(*param_1 + 0x14) = 2;
    }
    uVar4 = FUN_08012b60();
    iVar3 = *param_1;
    if (((uVar4 & 0x20000000) != 0) &&
       (*(undefined4 *)(iVar3 + 0x14) = 0x20000000, -1 < *(int *)(iVar7 + 0x440) << 0x1f)) {
      FUN_080125b4(iVar7,0x10);
      FUN_08012608(iVar7);
      if (param_1[6] == 2) {
        FUN_08012c58(*param_1,1);
      }
      FUN_08009b38(param_1);
      iVar3 = *param_1;
    }
    iVar3 = FUN_08012b60(iVar3);
    if (iVar3 << 7 < 0) {
      iVar3 = *param_1;
      uVar4 = *(uint *)(iVar3 + 0x440);
      local_2c = *(uint *)(iVar3 + 0x440) & 0xffffffd1;
      if ((int)(uVar4 << 0x1e) < 0) {
        if ((int)(uVar4 << 0x1f) < 0) {
          FUN_08009b30(param_1);
        }
        local_2c = local_2c | 2;
      }
      if ((int)(uVar4 << 0x1c) < 0) {
        local_2c = local_2c | 8;
        if ((int)(uVar4 << 0x1d) < 0) {
          if (param_1[6] == 2) {
            uVar6 = 2;
            if ((uVar4 & 0x60000) != 0x40000) {
              uVar6 = 1;
            }
            FUN_08012c58(*param_1,uVar6);
          }
          else if (param_1[4] == 1) {
            *(undefined4 *)(iVar3 + 0x404) = 60000;
          }
          FUN_08009b44(param_1);
        }
        else {
          FUN_08009b4c(param_1);
        }
      }
      if ((int)(uVar4 << 0x1a) < 0) {
        local_2c = local_2c | 0x20;
      }
      *(uint *)(iVar3 + 0x440) = local_2c;
    }
    iVar3 = FUN_08012b60(*param_1);
    if (iVar3 << 0x1c < 0) {
      FUN_08009b28(param_1);
      iVar3 = *param_1;
      *(undefined4 *)(iVar3 + 0x14) = 8;
    }
    else {
      iVar3 = *param_1;
    }
    iVar3 = FUN_08012b60(iVar3);
    iVar9 = *param_1;
    if (iVar3 << 0x1b < 0) {
      *(uint *)(iVar9 + 0x18) = *(uint *)(iVar9 + 0x18) & 0xffffffef;
      uVar10 = *(uint *)(iVar9 + 0x20);
      uVar4 = (uVar10 << 0x11) >> 0x15;
      if (((uVar10 << 0xb) >> 0x1c == 2) && (uVar4 != 0)) {
        uVar10 = uVar10 & 0xf;
        if (param_1[uVar10 * 0xb + 0x11] != 0) {
          if ((uint)param_1[uVar10 * 0xb + 0x13] < param_1[uVar10 * 0xb + 0x14] + uVar4) {
            *(undefined *)(param_1 + uVar10 * 0xb + 0x18) = 4;
          }
          else {
            iVar11 = iVar9 + 0x500;
            FUN_08012a34(iVar9);
            iVar3 = *(int *)(iVar11 + uVar10 * 0x20 + 0x10);
            param_1[uVar10 * 0xb + 0x11] = param_1[uVar10 * 0xb + 0x11] + uVar4;
            param_1[uVar10 * 0xb + 0x14] = param_1[uVar10 * 0xb + 0x14] + uVar4;
            if (((uint)(iVar3 << 3) >> 0x16 != 0) &&
               (*(ushort *)(param_1 + uVar10 * 0xb + 0x10) == uVar4)) {
              *(uint *)(uVar10 * 0x20 + iVar11) =
                   *(uint *)(uVar10 * 0x20 + iVar11) & 0xbfffffff | 0x80000000;
              *(byte *)(param_1 + uVar10 * 0xb + 0x15) =
                   *(byte *)(param_1 + uVar10 * 0xb + 0x15) ^ 1;
            }
            iVar9 = *param_1;
          }
        }
      }
      *(uint *)(iVar9 + 0x18) = *(uint *)(iVar9 + 0x18) | 0x10;
    }
    iVar3 = FUN_08012b60(iVar9);
    if (iVar3 << 6 < 0) {
      uVar4 = FUN_08012c9c(*param_1);
      if (param_1[2] != 0) {
        uVar10 = 0;
        piVar8 = (int *)(iVar7 + 0x500);
        do {
          if (-1 < (int)((uVar4 >> (uVar10 & 0xf)) << 0x1f)) goto LAB_0800c5a8;
          uVar12 = uVar10 & 0xff;
          iVar3 = *param_1;
          if (*piVar8 << 0x10 < 0) {
            iVar7 = FUN_08012b68(iVar3,uVar12);
            if (iVar7 << 0x1d < 0) {
              uVar6 = 4;
              iVar7 = iVar3 + 0x500 + uVar12 * 0x20;
LAB_0800c56c:
              *(undefined4 *)(iVar7 + 8) = uVar6;
              *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
              FUN_08012ca4(*param_1,uVar12);
            }
            else {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x17 < 0) {
                *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x100;
                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 8;
                FUN_08012ca4(*param_1,uVar12);
              }
              else {
                iVar7 = FUN_08012b68(*param_1,uVar12);
                if (iVar7 << 0x1c < 0) {
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 8;
                  *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 6;
                  FUN_08012ca4(*param_1,uVar12);
                }
                else {
                  iVar7 = FUN_08012b68(*param_1,uVar12);
                  if (iVar7 << 0x15 < 0) {
                    *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x400;
                    *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 9;
                    FUN_08012ca4(*param_1,uVar12);
                  }
                  else {
                    iVar7 = FUN_08012b68(*param_1,uVar12);
                    if (iVar7 << 0x18 < 0) {
                      uVar6 = 0x80;
                      iVar7 = iVar3 + 0x500 + uVar12 * 0x20;
                      goto LAB_0800c56c;
                    }
                  }
                }
              }
            }
            uVar5 = FUN_08012b68(*param_1,uVar12);
            if ((uVar5 & 0x200) == 0) {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x1f < 0) {
                iVar3 = iVar3 + 0x500;
                iVar7 = param_1[3];
                iVar9 = iVar3 + uVar12 * 0x20;
                *(undefined4 *)(iVar9 + 8) = 0x20;
                if (iVar7 != 0) {
                  param_1[uVar12 * 0xb + 0x14] =
                       param_1[uVar12 * 0xb + 0x12] - (*(uint *)(iVar9 + 0x10) & 0x7ffff);
                }
                param_1[uVar12 * 0xb + 0x17] = 0;
                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 1;
                *(undefined4 *)(iVar9 + 8) = 1;
                bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                if ((bVar2 & 0xfd) == 0) {
                  FUN_08012ca4(*param_1,uVar12);
                  iVar7 = param_1[3];
                  *(undefined4 *)(iVar9 + 8) = 0x10;
                }
                else if ((bVar2 & 0xfd) == 1) {
                  *(uint *)(iVar3 + uVar12 * 0x20) = *(uint *)(iVar3 + uVar12 * 0x20) | 0x20000000;
                  *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 1;
                  FUN_08009b40(param_1,uVar12);
                  iVar7 = param_1[3];
                }
                if ((iVar7 != 1) ||
                   ((int)((param_1[uVar12 * 0xb + 0x14] + -1 +
                          (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10)) /
                          (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10) << 0x1f) < 0)) {
                  *(byte *)(param_1 + uVar12 * 0xb + 0x15) =
                       *(byte *)(param_1 + uVar12 * 0xb + 0x15) ^ 1;
                }
              }
              else {
                iVar7 = FUN_08012b68(*param_1,uVar12);
                if (iVar7 << 0x1a < 0) {
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x20;
                }
                else {
                  uVar5 = FUN_08012b68(*param_1,uVar12);
                  if ((uVar5 & 2) == 0) {
                    uVar5 = FUN_08012b68(*param_1,uVar12);
                    if ((uVar5 & 0x40) == 0) {
                      iVar7 = FUN_08012b68(*param_1,uVar12);
                      if (iVar7 << 0x1b < 0) {
                        bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                        if (bVar2 == 3) {
                          param_1[uVar12 * 0xb + 0x17] = 0;
LAB_0800cb2e:
                          *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 4;
                          goto LAB_0800cae8;
                        }
                        if (((bVar2 & 0xfd) == 0) &&
                           (iVar7 = param_1[3], param_1[uVar12 * 0xb + 0x17] = bVar2 & 0xfd,
                           iVar7 == 0)) goto LAB_0800cb2e;
LAB_0800caf0:
                        *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x10;
                      }
                    }
                    else {
                      *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x40;
                      param_1[uVar12 * 0xb + 0x17] = 0;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 5;
                      FUN_08012ca4(*param_1,uVar12);
                    }
                  }
                  else {
                    *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 2;
                    cVar1 = *(char *)((int)param_1 + uVar12 * 0x2c + 0x61);
                    if (cVar1 == '\x01') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x201;
                    }
                    else if (cVar1 == '\x06') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x205;
                    }
                    else if ((cVar1 == '\a') || (cVar1 == '\t')) {
LAB_0800cb82:
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                      iVar7 = param_1[uVar12 * 0xb + 0x17];
                      param_1[uVar12 * 0xb + 0x17] = iVar7 + 1U;
                      if (iVar7 + 1U < 3) {
                        *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 2;
LAB_0800cbb6:
                        *(uint *)(iVar3 + 0x500 + uVar12 * 0x20) =
                             *(uint *)(iVar3 + 0x500 + uVar12 * 0x20) & 0xbfffffff | 0x80000000;
                      }
                      else {
                        param_1[uVar12 * 0xb + 0x17] = 0;
                        *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 4;
                      }
                    }
                    else if (cVar1 == '\x05') {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                    }
                    else if (cVar1 == '\x03') {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                    }
                    else if (cVar1 == '\x04') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x202;
                      if ((*(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f) & 0xfd) == 0)
                      goto LAB_0800cbb6;
                    }
                    else if (cVar1 == '\b') {
                      *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x204;
                      param_1[uVar12 * 0xb + 0x17] = param_1[uVar12 * 0xb + 0x17] + 1;
                    }
                    else if (cVar1 == '\x02') goto LAB_0800c5a8;
LAB_0800c9b8:
                    FUN_08009b40(param_1,uVar12,*(undefined *)(param_1 + uVar12 * 0xb + 0x18));
                  }
                }
              }
            }
            else {
              FUN_08012ca4();
              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x200;
            }
          }
          else {
            uVar5 = FUN_08012b68(iVar3,uVar12);
            if ((uVar5 & 4) == 0) {
              iVar7 = FUN_08012b68(*param_1,uVar12);
              if (iVar7 << 0x1a < 0) {
                *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x20;
                if (*(char *)((int)param_1 + uVar12 * 0x2c + 0x3d) == '\x01') {
                  *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 0;
                  *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x302;
                  FUN_08012ca4(*param_1,uVar12);
                }
              }
              else {
                uVar5 = FUN_08012b68(*param_1,uVar12);
                if ((uVar5 & 0x200) == 0) {
                  uVar5 = FUN_08012b68(*param_1,uVar12);
                  if ((uVar5 & 1) == 0) {
                    iVar7 = FUN_08012b68(*param_1,uVar12);
                    if (iVar7 << 0x19 < 0) {
                      param_1[uVar12 * 0xb + 0x17] = 0;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 5;
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                      FUN_08012ca4(*param_1,uVar12);
                      *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x40;
                    }
                    else {
                      uVar5 = FUN_08012b68(*param_1,uVar12);
                      if ((uVar5 & 8) == 0) {
                        iVar7 = FUN_08012b68(*param_1,uVar12);
                        if (iVar7 << 0x1b < 0) {
                          param_1[uVar12 * 0xb + 0x17] = 0;
                          *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 4;
                          if ((*(char *)((int)param_1 + uVar12 * 0x2c + 0x3d) == '\0') &&
                             (*(char *)(param_1 + uVar12 * 0xb + 0xf) == '\0')) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                          }
LAB_0800cae8:
                          FUN_08012ca4(*param_1,uVar12);
                          goto LAB_0800caf0;
                        }
                        iVar7 = FUN_08012b68(*param_1,uVar12);
                        if (iVar7 << 0x18 < 0) {
                          if (param_1[3] == 0) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
                            FUN_08012ca4(*param_1,uVar12);
                          }
                          else {
                            iVar7 = param_1[uVar12 * 0xb + 0x17];
                            param_1[uVar12 * 0xb + 0x17] = iVar7 + 1U;
                            if (iVar7 + 1U < 3) {
                              *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 2;
                            }
                            else {
                              param_1[uVar12 * 0xb + 0x17] = 0;
                              *(undefined *)(param_1 + uVar12 * 0xb + 0x18) = 4;
                              FUN_08009b40(param_1,uVar12);
                            }
                          }
                          *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x80;
                        }
                        else {
                          iVar7 = FUN_08012b68(*param_1,uVar12);
                          if (iVar7 << 0x15 < 0) {
                            *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 9;
                            FUN_08012ca4(*param_1,uVar12);
                            *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x400;
                          }
                          else {
                            iVar7 = FUN_08012b68(*param_1,uVar12);
                            if (iVar7 << 0x1e < 0) {
                              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 2;
                              cVar1 = *(char *)((int)param_1 + uVar12 * 0x2c + 0x61);
                              if (cVar1 == '\x01') {
                                bVar2 = *(byte *)((int)param_1 + uVar12 * 0x2c + 0x3f);
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x201;
                                if ((bVar2 - 2 < 2) &&
                                   ((param_1[3] == 0 ||
                                    (((param_1[3] == 1 && (param_1[uVar12 * 0xb + 0x13] != 0)) &&
                                     ((int)((param_1[uVar12 * 0xb + 0x13] + -1 +
                                            (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10)) /
                                            (uint)*(ushort *)(param_1 + uVar12 * 0xb + 0x10) << 0x1f
                                           ) < 0)))))) {
                                  *(byte *)((int)param_1 + uVar12 * 0x2c + 0x55) =
                                       *(byte *)((int)param_1 + uVar12 * 0x2c + 0x55) ^ 1;
                                }
                              }
                              else if (cVar1 == '\x03') {
                                *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 2;
                              }
                              else if ((cVar1 == '\x04') || (cVar1 == '\x05')) {
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x202;
                              }
                              else {
                                if (cVar1 != '\x06') {
                                  if ((cVar1 != '\a') && (cVar1 != '\t')) goto LAB_0800c5a8;
                                  goto LAB_0800cb82;
                                }
                                *(undefined2 *)(param_1 + uVar12 * 0xb + 0x18) = 0x205;
                              }
                              goto LAB_0800c9b8;
                            }
                          }
                        }
                      }
                      else {
                        *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 8;
                        *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 6;
                        FUN_08012ca4(*param_1,uVar12);
                      }
                    }
                  }
                  else {
                    iVar7 = *param_1;
                    param_1[uVar12 * 0xb + 0x17] = 0;
                    uVar5 = FUN_08012b68(iVar7,uVar12);
                    iVar3 = iVar3 + 0x500 + uVar12 * 0x20;
                    if ((uVar5 & 0x40) != 0) {
                      *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x3d) = 1;
                      *(undefined4 *)(iVar3 + 8) = 0x40;
                    }
                    *(undefined4 *)(iVar3 + 8) = 1;
                    *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 1;
                    FUN_08012ca4(*param_1,uVar12);
                  }
                }
                else {
                  iVar7 = *param_1;
                  *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 0x200;
                  FUN_08012ca4(iVar7,uVar12);
                }
              }
            }
            else {
              *(undefined4 *)(iVar3 + uVar12 * 0x20 + 0x508) = 4;
              *(undefined *)((int)param_1 + uVar12 * 0x2c + 0x61) = 7;
              FUN_08012ca4(*param_1,uVar12);
            }
          }
LAB_0800c5a8:
          uVar10 = uVar10 + 1;
          piVar8 = piVar8 + 8;
        } while (uVar10 < (uint)param_1[2]);
      }
      *(undefined4 *)(*param_1 + 0x14) = 0x2000000;
    }
  }
  return;
}



/* === 0800cd24 FUN_0800cd24 === */

undefined4 FUN_0800cd24(undefined4 *param_1)

{
  if (*(char *)(param_1 + 0xbe) != '\x01') {
    *(undefined *)(param_1 + 0xbe) = 1;
    FUN_08012d88(*param_1);
    *(undefined *)(param_1 + 0xbe) = 0;
    return 0;
  }
  return 2;
}



/* === 0800cd4c FUN_0800cd4c === */

undefined4 FUN_0800cd4c(int *param_1,uint param_2,uint param_3,uint param_4,int param_5)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1;
  do {
    do {
      if (((param_2 & ~*(uint *)(iVar2 + 0x18)) == 0) != param_3) {
        return 0;
      }
    } while (param_4 == 0xffffffff);
    iVar1 = FUN_08009ce8();
    iVar2 = *param_1;
  } while ((((uint)(iVar1 - param_5) <= param_4) && (param_4 != 0)) ||
          (((param_2 & ~*(uint *)(iVar2 + 0x18)) == 0) != param_3));
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = param_1[0x11] | 0x20;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}



/* === 0800cdb8 FUN_0800cdb8 === */

undefined4 FUN_0800cdb8(int *param_1,uint param_2,int param_3)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *param_1;
  if ((*(uint *)(iVar5 + 0x18) & 0x10) != 0) {
    *(undefined4 *)(iVar5 + 0x1c) = 0x10;
    while (-1 < *(int *)(iVar5 + 0x18) << 0x1a) {
      if (param_2 == 0xffffffff) goto LAB_0800cdd8;
      iVar4 = FUN_08009ce8();
      iVar5 = *param_1;
      if ((param_2 < (uint)(iVar4 - param_3)) || (param_2 == 0)) {
        if ((*(int *)(iVar5 + 0x18) << 0x10 < 0) &&
           ((-1 < *(int *)(iVar5 + 4) << 0x11 && (*(char *)((int)param_1 + 0x42) != ' ')))) {
          *(uint *)(iVar5 + 4) = *(uint *)(iVar5 + 4) | 0x4000;
          param_3 = FUN_08009ce8();
          iVar5 = *param_1;
        }
        while (-1 < *(int *)(iVar5 + 0x18) << 0x1a) {
          iVar4 = FUN_08009ce8();
          iVar5 = *param_1;
          if (0x19 < (uint)(iVar4 - param_3)) {
            uVar2 = 0x20;
            goto LAB_0800cde4;
          }
        }
      }
    }
    goto LAB_0800cdde;
  }
  iVar4 = *(int *)(iVar5 + 0x18);
  bVar1 = false;
  uVar2 = 0;
  uVar3 = 0;
  if (iVar4 << 0x17 < 0) goto LAB_0800ce4e;
LAB_0800cdf0:
  if (iVar4 << 0x15 < 0) {
    uVar3 = uVar3 | 8;
    *(undefined4 *)(iVar5 + 0x1c) = 0x400;
    goto LAB_0800cdfe;
  }
  if (-1 < iVar4 << 0x16) {
    if (!bVar1) {
      return 0;
    }
    goto LAB_0800ce0c;
  }
  goto LAB_0800ce02;
LAB_0800cdd8:
  do {
  } while (-1 < *(int *)(iVar5 + 0x18) << 0x1a);
LAB_0800cdde:
  uVar2 = 0;
  *(undefined4 *)(iVar5 + 0x1c) = 0x20;
LAB_0800cde4:
  iVar4 = *(int *)(iVar5 + 0x18);
  uVar2 = uVar2 | 4;
  bVar1 = true;
  uVar3 = uVar2;
  if (-1 < iVar4 << 0x17) goto LAB_0800cdf0;
LAB_0800ce4e:
  uVar3 = uVar2 | 1;
  *(undefined4 *)(iVar5 + 0x1c) = 0x100;
  if (iVar4 << 0x15 < 0) {
    uVar3 = uVar2 | 9;
    *(undefined4 *)(iVar5 + 0x1c) = 0x400;
  }
LAB_0800cdfe:
  if (-1 < iVar4 << 0x16) goto LAB_0800ce0c;
LAB_0800ce02:
  uVar3 = uVar3 | 2;
  *(undefined4 *)(iVar5 + 0x1c) = 0x200;
LAB_0800ce0c:
  if (*(int *)(iVar5 + 0x18) << 0x1e < 0) {
    *(undefined4 *)(iVar5 + 0x28) = 0;
  }
  if (-1 < *(int *)(iVar5 + 0x18) << 0x1f) {
    *(uint *)(iVar5 + 0x18) = *(uint *)(iVar5 + 0x18) | 1;
  }
  *(uint *)(iVar5 + 4) = *(uint *)(iVar5 + 4) & DAT_0800cec0;
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = uVar3 | param_1[0x11];
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}



/* === 0800cec4 FUN_0800cec4 === */

undefined4 FUN_0800cec4(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *param_1;
  while( true ) {
    if (*(int *)(iVar2 + 0x18) << 0x1e < 0) {
      return 0;
    }
    iVar2 = FUN_0800cdb8(param_1,param_2,param_3);
    if (iVar2 != 0) break;
    if (param_2 == 0xffffffff) {
      iVar2 = *param_1;
    }
    else {
      iVar1 = FUN_08009ce8();
      iVar2 = *param_1;
      if (((param_2 < (uint)(iVar1 - param_3)) || (param_2 == 0)) &&
         ((*(uint *)(iVar2 + 0x18) & 2) == 0)) {
        *(undefined *)(param_1 + 0x10) = 0;
        param_1[0x11] = param_1[0x11] | 0x20;
        *(undefined *)((int)param_1 + 0x41) = 0x20;
        *(undefined *)((int)param_1 + 0x42) = 0;
        return 1;
      }
    }
  }
  return 1;
}



/* === 0800cf20 FUN_0800cf20 === */

undefined4 FUN_0800cf20(int *param_1,uint param_2,int param_3)

{
  int iVar1;
  
  if (*(int *)(*param_1 + 0x18) << 0x1a < 0) {
    return 0;
  }
  while( true ) {
    iVar1 = FUN_0800cdb8(param_1,param_2,param_3);
    if (iVar1 != 0) {
      return 1;
    }
    iVar1 = FUN_08009ce8();
    if (((param_2 < (uint)(iVar1 - param_3)) || (param_2 == 0)) &&
       ((*(uint *)(*param_1 + 0x18) & 0x20) == 0)) break;
    if (*(int *)(*param_1 + 0x18) << 0x1a < 0) {
      return 0;
    }
  }
  *(undefined *)(param_1 + 0x10) = 0;
  param_1[0x11] = param_1[0x11] | 0x20;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  return 1;
}



/* === 0800cf80 FUN_0800cf80 === */

undefined4 FUN_0800cf80(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  
  if (param_1 != (uint **)0x0) {
    if (*(char *)((int)param_1 + 0x41) == '\0') {
      *(undefined *)(param_1 + 0x10) = 0;
      FUN_08007dfc();
    }
    puVar4 = *param_1;
    puVar2 = param_1[1];
    *(undefined *)((int)param_1 + 0x41) = 0x24;
    puVar1 = param_1[3];
    *puVar4 = *puVar4 & 0xfffffffe;
    puVar4[4] = (uint)puVar2 & 0xf0ffffff;
    puVar4[2] = puVar4[2] & 0xffff7fff;
    if (puVar1 == (uint *)0x1) {
      puVar4[2] = (uint)param_1[2] | 0x8000;
    }
    else {
      puVar4[2] = (uint)param_1[2] | 0x8400;
      if (puVar1 == (uint *)0x2) {
        puVar4[1] = 0x800;
      }
    }
    puVar3 = param_1[4];
    puVar4[1] = DAT_0800d028 | puVar4[1];
    puVar4[3] = puVar4[3] & 0xffff7fff;
    puVar2 = param_1[7];
    puVar1 = param_1[8];
    puVar4[3] = (uint)puVar3 | (uint)param_1[5] | (int)param_1[6] << 8;
    *puVar4 = (uint)puVar2 | (uint)puVar1;
    *puVar4 = *puVar4 | 1;
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x42) = 0;
    return 0;
  }
  return 1;
}



/* === 0800d02c FUN_0800d02c === */

undefined4
FUN_0800d02c(int *param_1,uint param_2,byte *param_3,undefined2 param_4,undefined4 param_5)

{
  ushort uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  
  if ((*(char *)((int)param_1 + 0x41) != ' ') || (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  iVar3 = FUN_08009ce8();
  iVar8 = *param_1;
  while (*(int *)(iVar8 + 0x18) << 0x10 < 0) {
    iVar4 = FUN_08009ce8();
    iVar8 = *param_1;
    if ((0x19 < (uint)(iVar4 - iVar3)) && (*(int *)(iVar8 + 0x18) << 0x10 < 0)) {
      *(undefined *)(param_1 + 0x10) = 0;
      param_1[0x11] = param_1[0x11] | 0x20;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      *(undefined *)((int)param_1 + 0x42) = 0;
      return 1;
    }
  }
  param_1[9] = (int)param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  param_1[0xd] = 0;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = 0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    uVar1 = *(ushort *)((int)param_1 + 0x2a);
    *(ushort *)(param_1 + 10) = uVar1;
    if (uVar1 == 0) {
      *(uint *)(iVar8 + 4) = DAT_0800d1fc | param_2 & 0x3ff | DAT_0800d1f8 & *(uint *)(iVar8 + 4);
      goto LAB_0800d0da;
    }
    sVar2 = uVar1 - 1;
    uVar9 = 0x2000000;
    uVar5 = (uint)(uVar1 & 0xff) << 0x10;
  }
  else {
    uVar5 = 0xff0000;
    sVar2 = 0xfe;
    uVar9 = 0x1000000;
    *(undefined2 *)(param_1 + 10) = 0xff;
  }
  *(uint *)(iVar8 + 0x28) = (uint)*param_3;
  *(short *)(param_1 + 10) = sVar2;
  param_1[9] = (int)(param_3 + 1);
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  *(uint *)(iVar8 + 4) =
       *(uint *)(iVar8 + 4) & DAT_0800d1f8 | uVar5 | param_2 & 0x3ff | uVar9 | 0x2000;
LAB_0800d0da:
  uVar5 = DAT_0800d208;
  uVar9 = DAT_0800d204;
LAB_0800d0e2:
  do {
    sVar2 = *(short *)((int)param_1 + 0x2a);
    while( true ) {
      if (sVar2 == 0) {
        iVar3 = FUN_0800cf20();
        uVar9 = DAT_0800d200;
        if (iVar3 != 0) {
          return 1;
        }
        iVar3 = *param_1;
        *(undefined4 *)(iVar3 + 0x1c) = 0x20;
        *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) & uVar9;
        *(undefined *)((int)param_1 + 0x41) = 0x20;
        *(undefined *)(param_1 + 0x10) = 0;
        *(undefined *)((int)param_1 + 0x42) = 0;
        return 0;
      }
      iVar8 = FUN_0800cec4(param_1,param_5,iVar3);
      if (iVar8 != 0) {
        return 1;
      }
      pbVar6 = (byte *)param_1[9];
      sVar2 = *(short *)(param_1 + 10);
      *(uint *)(*param_1 + 0x28) = (uint)*pbVar6;
      param_1[9] = (int)(pbVar6 + 1);
      *(short *)(param_1 + 10) = sVar2 + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      if (((short)(sVar2 + -1) != 0) || (*(short *)((int)param_1 + 0x2a) == 0)) goto LAB_0800d0e2;
      iVar8 = FUN_0800cd4c(param_1,0x80,0,param_5,iVar3);
      if (iVar8 != 0) {
        return 1;
      }
      if (*(ushort *)((int)param_1 + 0x2a) < 0x100) break;
      *(undefined2 *)(param_1 + 10) = 0xff;
      *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & uVar9 | param_2 & 0x3ff | uVar5;
      sVar2 = *(short *)((int)param_1 + 0x2a);
    }
    uVar7 = *(uint *)(*param_1 + 4);
    *(undefined2 *)(param_1 + 10) = *(undefined2 *)((int)param_1 + 0x2a);
    *(uint *)(*param_1 + 4) =
         param_2 & 0x3ff | (uint)(byte)*(undefined2 *)((int)param_1 + 0x2a) << 0x10 | uVar7 & uVar9
         | 0x2000000;
  } while( true );
}



/* === 0800d20c FUN_0800d20c === */

undefined4 FUN_0800d20c(int *param_1,int param_2,int param_3,undefined4 param_4)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  byte *pbVar4;
  uint uVar5;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if ((param_2 == 0) || (uVar5 = (uint)(param_3 == 0), param_3 == 0)) {
    param_1[0x11] = 0x200;
    return 1;
  }
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  uVar1 = FUN_08009ce8();
  param_1[9] = param_2;
  param_1[0xd] = uVar5;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  *(undefined *)((int)param_1 + 0x42) = 0x20;
  param_1[0x11] = uVar5;
  *(short *)((int)param_1 + 0x2a) = (short)param_3;
  *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xffff7fff;
  iVar2 = FUN_0800cd4c(param_1,8,uVar5,param_4,uVar1);
  if (iVar2 != 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) | 0x8000;
    return 1;
  }
  if (param_1[8] == 0x20000) {
    pbVar4 = (byte *)param_1[9];
    iVar2 = *param_1;
    *(uint *)(iVar2 + 0x28) = (uint)*pbVar4;
    param_1[9] = (int)(pbVar4 + 1);
    *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  }
  else {
    iVar2 = *param_1;
  }
  *(undefined4 *)(iVar2 + 0x1c) = 8;
  if (param_1[3] == 2) {
    iVar2 = FUN_0800cd4c(param_1,8,0,param_4,uVar1);
    if (iVar2 != 0) goto LAB_0800d2e8;
    *(undefined4 *)(*param_1 + 0x1c) = 8;
  }
  iVar2 = FUN_0800cd4c(param_1,0x10000,0,param_4,uVar1);
  if (iVar2 == 0) {
    while (*(short *)((int)param_1 + 0x2a) != 0) {
      iVar2 = FUN_0800cec4(param_1,param_4,uVar1);
      if (iVar2 != 0) goto LAB_0800d2e8;
      pbVar4 = (byte *)param_1[9];
      *(uint *)(*param_1 + 0x28) = (uint)*pbVar4;
      param_1[9] = (int)(pbVar4 + 1);
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
    }
    iVar2 = FUN_0800cd4c(param_1,0x10,0,param_4,uVar1);
    if (iVar2 == 0) {
      iVar2 = *param_1;
      if (*(int *)(iVar2 + 0x18) << 0x1e < 0) {
        *(undefined4 *)(iVar2 + 0x28) = 0;
      }
      if (-1 < *(int *)(iVar2 + 0x18) << 0x1f) {
        *(uint *)(iVar2 + 0x18) = *(uint *)(iVar2 + 0x18) | 1;
      }
      *(undefined4 *)(iVar2 + 0x1c) = 0x10;
      iVar2 = FUN_0800cf20(param_1,param_4,uVar1);
      iVar3 = *param_1;
      if (iVar2 == 0) {
        *(undefined4 *)(iVar3 + 0x1c) = 0x20;
        iVar2 = FUN_0800cd4c(param_1,0x8000,1,param_4,uVar1);
        iVar3 = *param_1;
        if (iVar2 == 0) {
          *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) | 0x8000;
          *(undefined *)((int)param_1 + 0x41) = 0x20;
          *(undefined *)(param_1 + 0x10) = 0;
          *(undefined *)((int)param_1 + 0x42) = 0;
          return 0;
        }
      }
      *(uint *)(iVar3 + 4) = *(uint *)(iVar3 + 4) | 0x8000;
      return 1;
    }
  }
LAB_0800d2e8:
  *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) | 0x8000;
  return 1;
}



/* === 0800d3b4 FUN_0800d3b4 === */

undefined4 FUN_0800d3b4(uint **param_1,uint param_2,uint *param_3,undefined2 param_4)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  
  if (((*(char *)((int)param_1 + 0x41) != ' ') || (puVar7 = *param_1, (puVar7[6] & 0x8000) != 0)) ||
     (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  param_1[9] = param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x21;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = (uint *)0x0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  puVar8 = DAT_0800d518;
  *(undefined *)(param_1 + 0x10) = 1;
  param_1[0xb] = puVar8;
  param_1[0xd] = DAT_0800d51c;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    sVar1 = *(short *)((int)param_1 + 0x2a);
    *(short *)(param_1 + 10) = sVar1;
    uVar5 = 0;
    if (sVar1 != 0) {
      puVar7[10] = (uint)*(byte *)param_3;
      param_1[9] = (uint *)((int)param_3 + 1);
      *(short *)(param_1 + 10) = sVar1 + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      if (sVar1 != 1) {
        uVar5 = 0x2000000;
        goto LAB_0800d456;
      }
      uVar5 = 0x10000;
    }
    uVar3 = DAT_0800d520;
    uVar6 = puVar7[1];
    param_1[0xd] = DAT_0800d524;
    puVar7[1] = DAT_0800d528 | uVar5 | param_2 & 0x3ff | uVar6 & uVar3;
    *(undefined *)(param_1 + 0x10) = 0;
    *puVar7 = *puVar7 | 0xf2;
    return 0;
  }
  uVar5 = 0x1000000;
  *(undefined2 *)(param_1 + 10) = 0xff;
  puVar7[10] = (uint)*(byte *)param_3;
  param_1[9] = (uint *)((int)param_3 + 1);
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
  *(undefined2 *)(param_1 + 10) = 0xfe;
LAB_0800d456:
  puVar8 = param_1[0xe];
  if (puVar8 == (uint *)0x0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
    return 1;
  }
  puVar8[0xf] = DAT_0800d52c;
  uVar3 = DAT_0800d530;
  puVar8[0x10] = 0;
  puVar8[0x13] = uVar3;
  puVar8[0x14] = 0;
  iVar4 = FUN_0800b3a0(puVar8,(byte *)((int)param_3 + 1),puVar7 + 10);
  if (iVar4 != 0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
    return 1;
  }
  puVar7 = *param_1;
  uVar2 = *(ushort *)(param_1 + 10);
  puVar7[1] = param_2 & 0x3ff | uVar5 | puVar7[1] & DAT_0800d520 | (uVar2 + 1 & 0xff) << 0x10 |
              0x2000;
  *(undefined *)(param_1 + 0x10) = 0;
  *(ushort *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) - uVar2;
  *puVar7 = *puVar7 | 0x90;
  *puVar7 = *puVar7 | 0x4000;
  return 0;
}



/* === 0800d534 FUN_0800d534 === */

undefined4 FUN_0800d534(uint **param_1,uint param_2,uint *param_3,undefined2 param_4)

{
  short sVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  
  if (((*(char *)((int)param_1 + 0x41) != ' ') || (puVar4 = *param_1, (puVar4[6] & 0x8000) != 0)) ||
     (*(char *)(param_1 + 0x10) == '\x01')) {
    return 2;
  }
  param_1[9] = param_3;
  *(undefined *)((int)param_1 + 0x41) = 0x22;
  *(undefined *)((int)param_1 + 0x42) = 0x10;
  param_1[0x11] = (uint *)0x0;
  *(undefined2 *)((int)param_1 + 0x2a) = param_4;
  puVar5 = DAT_0800d65c;
  *(undefined *)(param_1 + 0x10) = 1;
  param_1[0xb] = puVar5;
  param_1[0xd] = DAT_0800d660;
  if (*(ushort *)((int)param_1 + 0x2a) < 0x100) {
    *(short *)(param_1 + 10) = *(short *)((int)param_1 + 0x2a);
    uVar6 = DAT_0800d668;
    if (*(short *)((int)param_1 + 0x2a) == 0) {
      param_1[0xd] = DAT_0800d664;
      puVar4[1] = DAT_0800d66c | param_2 & 0x3ff | puVar4[1] & uVar6;
      *(undefined *)(param_1 + 0x10) = 0;
      *puVar4 = *puVar4 | 0xf4;
      return 0;
    }
    uVar6 = 0x2000000;
  }
  else {
    uVar6 = 0x1000000;
    *(undefined2 *)(param_1 + 10) = 0xff;
  }
  puVar5 = param_1[0xf];
  if (puVar5 == (uint *)0x0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
    return 1;
  }
  puVar5[0xf] = DAT_0800d670;
  uVar2 = DAT_0800d674;
  puVar5[0x10] = 0;
  puVar5[0x13] = uVar2;
  puVar5[0x14] = 0;
  iVar3 = FUN_0800b3a0(puVar5,puVar4 + 9);
  if (iVar3 != 0) {
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)((int)param_1 + 0x42) = 0;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
    return 1;
  }
  puVar4 = *param_1;
  sVar1 = *(short *)(param_1 + 10);
  puVar4[1] = param_2 & 0x3ff | uVar6 | puVar4[1] & DAT_0800d668 | (uint)(byte)sVar1 << 0x10 |
              0x2400;
  *(undefined *)(param_1 + 0x10) = 0;
  *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) - sVar1;
  *puVar4 = *puVar4 | 0x90;
  *puVar4 = *puVar4 | 0x8000;
  return 0;
}



/* === 0800d678 FUN_0800d678 === */

undefined4 FUN_0800d678(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  short sVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  
  if (*(char *)((int)param_1 + 0x41) == ' ') {
    if ((param_2 == (uint *)0x0) || (param_3 == 0)) {
      param_1[0x11] = (uint *)0x200;
      return 1;
    }
    if (*(char *)(param_1 + 0x10) != '\x01') {
      param_1[9] = param_2;
      *(undefined *)((int)param_1 + 0x41) = 0x21;
      *(undefined *)((int)param_1 + 0x42) = 0x20;
      param_1[0x11] = (uint *)(uint)(param_3 == 0);
      *(short *)((int)param_1 + 0x2a) = (short)param_3;
      puVar4 = DAT_0800d78c;
      sVar1 = *(short *)((int)param_1 + 0x2a);
      param_1[0xd] = DAT_0800d788;
      param_1[0xb] = puVar4;
      *(short *)(param_1 + 10) = sVar1;
      *(undefined *)(param_1 + 0x10) = 1;
      if (param_1[8] == (uint *)0x20000) {
        (*param_1)[10] = (uint)*(byte *)param_2;
        *(short *)(param_1 + 10) = sVar1 + -1;
        param_1[9] = (uint *)((int)param_2 + 1);
        *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      }
      if (*(short *)((int)param_1 + 0x2a) == 0) {
        puVar4 = *param_1;
        puVar4[1] = puVar4[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar4 = *puVar4 | 0xb8;
        return 0;
      }
      puVar4 = param_1[0xe];
      if (puVar4 == (uint *)0x0) {
        *(undefined *)(param_1 + 0x10) = 0;
        *(undefined *)((int)param_1 + 0x41) = 0x28;
        *(undefined *)((int)param_1 + 0x42) = 0;
        param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
        return 1;
      }
      puVar3 = *param_1;
      puVar4[0xf] = DAT_0800d790;
      puVar4[0x10] = 0;
      puVar4[0x13] = DAT_0800d794;
      puVar4[0x14] = 0;
      iVar2 = FUN_0800b3a0(puVar4,param_1[9],puVar3 + 10,*(undefined2 *)(param_1 + 10),param_4);
      if (iVar2 == 0) {
        puVar4 = *param_1;
        puVar4[1] = puVar4[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar4 = *puVar4 | 0xb8;
        *puVar4 = *puVar4 | 0x4000;
        return 0;
      }
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x28;
      *(undefined *)((int)param_1 + 0x42) = 0;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
      return 1;
    }
  }
  return 2;
}



/* === 0800d798 FUN_0800d798 === */

undefined4 FUN_0800d798(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  undefined2 uVar1;
  bool bVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if (param_2 != (uint *)0x0) {
    bVar2 = param_3 == 0;
    puVar7 = (uint *)(uint)bVar2;
    if (param_3 != 0) {
      if (*(char *)(param_1 + 0x10) == '\x01') {
        return 2;
      }
      puVar6 = param_1[0xf];
      *(undefined *)((int)param_1 + 0x41) = 0x22;
      *(undefined *)((int)param_1 + 0x42) = 0x20;
      param_1[0x11] = puVar7;
      *(short *)((int)param_1 + 0x2a) = (short)param_3;
      uVar1 = *(undefined2 *)((int)param_1 + 0x2a);
      param_1[0xb] = DAT_0800d864;
      puVar5 = DAT_0800d868;
      param_1[9] = param_2;
      *(undefined2 *)(param_1 + 10) = uVar1;
      param_1[0xd] = puVar5;
      *(undefined *)(param_1 + 0x10) = 1;
      if (puVar6 != (uint *)0x0) {
        puVar5 = *param_1;
        puVar6[0xf] = DAT_0800d86c;
        uVar3 = DAT_0800d870;
        puVar6[0x10] = (uint)puVar7;
        puVar6[0x13] = uVar3;
        puVar6[0x14] = (uint)puVar7;
        iVar4 = FUN_0800b3a0(puVar6,puVar5 + 9,param_2,uVar1,param_4);
        if (iVar4 != 0) {
          *(bool *)(param_1 + 0x10) = bVar2;
          *(undefined *)((int)param_1 + 0x41) = 0x28;
          *(bool *)((int)param_1 + 0x42) = bVar2;
          param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x10);
          return 1;
        }
        puVar7 = *param_1;
        puVar7[1] = puVar7[1] & 0xffff7fff;
        *(undefined *)(param_1 + 0x10) = 0;
        *puVar7 = *puVar7 | 0xb8;
        *puVar7 = *puVar7 | 0x8000;
        return 0;
      }
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x28;
      *(undefined *)((int)param_1 + 0x42) = 0;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 0x80);
      return 1;
    }
  }
  param_1[0x11] = (uint *)0x200;
  return 1;
}



/* === 0800d884 FUN_0800d884 === */

void FUN_0800d884(uint **param_1)

{
  uint uVar1;
  uint uVar2;
  
  *(undefined *)((int)param_1 + 0x42) = 0;
  if (*(char *)((int)param_1 + 0x41) != '!') {
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x12;
    uVar2 = **param_1;
    param_1[0xd] = (uint *)0x0;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar1 = 0xffffffbb;
    }
    else {
      uVar1 = 0xffffff0b;
    }
    **param_1 = uVar1 & uVar2;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_08008014();
    return;
  }
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  param_1[0xc] = (uint *)0x11;
  param_1[0xd] = (uint *)0x0;
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
    uVar2 = 0xffffffbd;
  }
  else {
    uVar2 = 0xffffff0d;
  }
  **param_1 = **param_1 & uVar2;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800800c();
  return;
}



/* === 0800d8fc FUN_0800d8fc === */

void FUN_0800d8fc(uint **param_1)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar2 = *param_1;
  uVar3 = *puVar2;
  *(undefined *)((int)param_1 + 0x42) = 0;
  if ((int)(uVar3 << 0x11) < 0) {
    *puVar2 = *puVar2 & 0xffffbfff;
  }
  else if ((int)(uVar3 << 0x10) < 0) {
    *puVar2 = *puVar2 & 0xffff7fff;
    cVar1 = *(char *)((int)param_1 + 0x41);
    goto joined_r0x0800d938;
  }
  cVar1 = *(char *)((int)param_1 + 0x41);
joined_r0x0800d938:
  if (cVar1 == ')') {
    *(undefined *)((int)param_1 + 0x41) = 0x28;
    param_1[0xc] = (uint *)0x21;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbd;
    }
    else {
      uVar3 = 0xffffff0d;
    }
    *puVar2 = *puVar2 & uVar3;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_0800801c();
    return;
  }
  if (*(char *)((int)param_1 + 0x41) != '*') {
    return;
  }
  *(undefined *)((int)param_1 + 0x41) = 0x28;
  param_1[0xc] = (uint *)0x22;
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
    uVar3 = 0xffffffbb;
  }
  else {
    uVar3 = 0xffffff0b;
  }
  *puVar2 = uVar3 & *puVar2;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_08008024();
  return;
}



/* === 0800d9e4 FUN_0800d9e4 === */

void FUN_0800d9e4(void)

{
  return;
}



/* === 0800d9e8 FUN_0800d9e8 === */

void FUN_0800d9e8(uint **param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) != 0x28) {
    (*param_1)[7] = 8;
    *(undefined *)(param_1 + 0x10) = 0;
    return;
  }
  puVar3 = *param_1;
  uVar4 = puVar3[2];
  uVar1 = (puVar3[6] << 0xf) >> 0x1f;
  uVar2 = puVar3[6] >> 0x10 & 0xfe;
  if (param_1[3] == (uint *)0x2) {
    if (((uVar2 ^ uVar4 >> 7) & 6) == 0) {
      param_1[0x12] = (uint *)((int)param_1[0x12] + 1);
      if (param_1[0x12] != (uint *)0x2) {
        return;
      }
      param_1[0x12] = (uint *)0x0;
      puVar3[7] = 8;
      *(undefined *)(param_1 + 0x10) = 0;
      FUN_0800d9e4(param_1,uVar1,uVar4 & 0x3ff,param_1,param_4);
      return;
    }
    uVar2 = puVar3[3] & 0xfe;
  }
  *puVar3 = *puVar3 & 0xffffff47;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800d9e4(param_1,uVar1,uVar2,param_1,param_4);
  return;
}



/* === 0800da64 FUN_0800da64 === */

void FUN_0800da64(void)

{
  return;
}



/* === 0800da68 FUN_0800da68 === */

void FUN_0800da68(uint **param_1,int param_2)

{
  uint *puVar1;
  
  puVar1 = DAT_0800dacc;
  param_1[0xd] = (uint *)0x0;
  param_1[0xb] = puVar1;
  param_1[0xc] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  *(undefined *)((int)param_1 + 0x42) = 0;
  if (param_2 << 0x1d < 0) {
    *(char *)param_1[9] = (char)(*param_1)[9];
    param_1[9] = (uint *)((int)param_1[9] + 1);
    if (*(short *)(param_1 + 10) != 0) {
      *(short *)(param_1 + 10) = *(short *)(param_1 + 10) + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
    }
  }
  puVar1 = *param_1;
  *puVar1 = *puVar1 & 0xffffff01;
  puVar1[7] = 0x10;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800da64();
  return;
}



/* === 0800dad0 FUN_0800dad0 === */

void FUN_0800dad0(void)

{
  return;
}



/* === 0800dad4 FUN_0800dad4 === */

void FUN_0800dad4(void)

{
  return;
}



/* === 0800dad8 FUN_0800dad8 === */

void FUN_0800dad8(void)

{
  return;
}



/* === 0800db1c FUN_0800db1c === */

void FUN_0800db1c(uint **param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  
  puVar3 = DAT_0800dc7c;
  *(undefined *)((int)param_1 + 0x42) = 0;
  param_1[0xb] = puVar3;
  *(undefined2 *)((int)param_1 + 0x2a) = 0;
  param_1[0x11] = (uint *)(param_2 | (uint)param_1[0x11]);
  puVar3 = DAT_0800dc80;
  if (*(byte *)((int)param_1 + 0x41) - 0x28 < 3) {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar2 = 0x46;
    }
    else {
      uVar2 = 0xf6;
    }
    puVar4 = *param_1;
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar2 = ~uVar2;
    }
    else {
      uVar2 = 0xffffff09;
    }
    *puVar4 = *puVar4 & uVar2;
    *(undefined *)((int)param_1 + 0x41) = 0x28;
    param_1[0xd] = puVar3;
  }
  else {
    puVar4 = *param_1;
    *puVar4 = *puVar4 & 0xffffff01;
    if ((int)(puVar4[6] << 0x1e) < 0) {
      puVar4[10] = 0;
    }
    if (-1 < (int)(puVar4[6] << 0x1f)) {
      puVar4[6] = puVar4[6] | 1;
    }
    if ((*(char *)((int)param_1 + 0x41) != '`') &&
       (*(undefined *)((int)param_1 + 0x41) = 0x20, (int)(puVar4[6] << 0x1a) < 0)) {
      if ((int)(puVar4[6] << 0x1b) < 0) {
        puVar4[7] = 0x10;
        param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
      }
      puVar4[7] = 0x20;
    }
    param_1[0xd] = (uint *)0x0;
  }
  puVar3 = param_1[0xc];
  if ((param_1[0xe] == (uint *)0x0) || ((puVar3 != (uint *)0x11 && (puVar3 != (uint *)0x21)))) {
    if ((param_1[0xf] != (uint *)0x0) && ((puVar3 == (uint *)0x12 || (puVar3 == (uint *)0x22)))) {
      if ((int)(*puVar4 << 0x10) < 0) {
        *puVar4 = *puVar4 & 0xffff7fff;
      }
      iVar1 = FUN_0800c074();
      if (iVar1 != 1) {
        param_1[0xf][0x14] = DAT_0800dc84;
        *(undefined *)(param_1 + 0x10) = 0;
        iVar1 = FUN_0800b840();
        if (iVar1 == 0) {
          return;
        }
                    /* WARNING: Could not recover jumptable at 0x0800dc18. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        (*(code *)param_1[0xf][0x14])();
        return;
      }
    }
  }
  else {
    if ((int)(*puVar4 << 0x11) < 0) {
      *puVar4 = *puVar4 & 0xffffbfff;
    }
    iVar1 = FUN_0800c074();
    if (iVar1 != 1) {
      param_1[0xe][0x14] = DAT_0800dc84;
      *(undefined *)(param_1 + 0x10) = 0;
      iVar1 = FUN_0800b840();
      if (iVar1 == 0) {
        return;
      }
                    /* WARNING: Could not recover jumptable at 0x0800dbe4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)param_1[0xe][0x14])();
      return;
    }
  }
  if (*(char *)((int)param_1 + 0x41) != '`') {
    param_1[0xc] = (uint *)0x0;
    *(undefined *)(param_1 + 0x10) = 0;
    FUN_0800802c(param_1);
    return;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  *(undefined *)((int)param_1 + 0x41) = 0x20;
  param_1[0xc] = (uint *)0x0;
  FUN_0800dad8(param_1);
  return;
}



/* === 0800dc88 FUN_0800dc88 === */

void FUN_0800dc88(uint **param_1,uint param_2)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  
  puVar6 = *param_1;
  uVar4 = *puVar6;
  puVar7 = param_1[0xb];
  bVar1 = *(byte *)((int)param_1 + 0x41);
  puVar6[7] = 0x20;
  if ((bVar1 - 0x28 < 2) || (bVar1 == 0x21)) {
    *puVar6 = *puVar6 & 0xffffff05;
    param_1[0xc] = (uint *)0x21;
  }
  else if ((bVar1 & 0xf7) == 0x22) {
    *puVar6 = *puVar6 & 0xffffff03;
    param_1[0xc] = (uint *)0x22;
  }
  uVar3 = DAT_0800de4c;
  puVar6[1] = puVar6[1] | 0x8000;
  puVar6[1] = puVar6[1] & uVar3;
  if ((int)(puVar6[6] << 0x1e) < 0) {
    puVar6[10] = 0;
  }
  if (-1 < (int)(puVar6[6] << 0x1f)) {
    puVar6[6] = puVar6[6] | 1;
  }
  if ((int)(uVar4 << 0x11) < 0) {
    *puVar6 = *puVar6 & 0xffffbfff;
    puVar5 = param_1[0xe];
  }
  else {
    if (-1 < (int)(uVar4 << 0x10)) goto LAB_0800dcfa;
    *puVar6 = *puVar6 & 0xffff7fff;
    puVar5 = param_1[0xf];
  }
  if (puVar5 != (uint *)0x0) {
    *(short *)((int)param_1 + 0x2a) = (short)*(undefined4 *)(*puVar5 + 4);
  }
LAB_0800dcfa:
  if ((int)(param_2 << 0x1d) < 0) {
    param_2 = param_2 & 0xfffffffb;
    *(char *)param_1[9] = (char)puVar6[9];
    param_1[9] = (uint *)((int)param_1[9] + 1);
    if (*(short *)(param_1 + 10) != 0) {
      *(short *)(param_1 + 10) = *(short *)(param_1 + 10) + -1;
      *(short *)((int)param_1 + 0x2a) = *(short *)((int)param_1 + 0x2a) + -1;
    }
  }
  if (*(short *)((int)param_1 + 0x2a) != 0) {
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
  }
  if (((int)(param_2 << 0x1b) < 0) && ((int)(uVar4 << 0x1b) < 0)) {
    if (*(short *)((int)param_1 + 0x2a) == 0) {
      if ((puVar7 == (uint *)0x2000000) && (*(char *)((int)param_1 + 0x41) == '(')) {
        FUN_0800da68(param_1,param_2);
      }
      else {
        cVar2 = *(char *)((int)param_1 + 0x41);
        puVar6 = *param_1;
        puVar6[7] = 0x10;
        if ((cVar2 == ')') && (puVar7 != (uint *)&H_Reset)) {
          if ((int)(puVar6[6] << 0x1e) < 0) {
            puVar6[10] = 0;
          }
          if (-1 < (int)(puVar6[6] << 0x1f)) {
            puVar6[6] = puVar6[6] | 1;
          }
          FUN_0800d8fc(param_1);
        }
      }
    }
    else {
      (*param_1)[7] = 0x10;
      param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
      if (((uint)puVar7 & 0xfeffffff) == 0) {
        FUN_0800db1c(param_1,param_1[0x11]);
      }
    }
  }
  *(undefined *)((int)param_1 + 0x42) = 0;
  param_1[0xd] = (uint *)0x0;
  puVar6 = DAT_0800de50;
  if (param_1[0x11] == (uint *)0x0) {
    if (param_1[0xb] != DAT_0800de50) {
      FUN_0800d8fc(param_1);
      param_1[0xb] = puVar6;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      *(undefined *)(param_1 + 0x10) = 0;
      param_1[0xc] = (uint *)0x0;
      FUN_0800da64(param_1);
      return;
    }
    cVar2 = *(char *)((int)param_1 + 0x41);
    *(undefined *)(param_1 + 0x10) = 0;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    param_1[0xc] = (uint *)0x0;
    if (cVar2 == '\"') {
      FUN_08008024(param_1);
      return;
    }
    FUN_0800801c();
  }
  else {
    FUN_0800db1c(param_1,param_1[0x11]);
    if (*(char *)((int)param_1 + 0x41) == '(') {
      FUN_0800da68(param_1,param_2);
      return;
    }
  }
  return;
}



/* === 0800df90 FUN_0800df90 === */

void FUN_0800df90(uint **param_1,int param_2)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  uint *puVar4;
  
  puVar4 = *param_1;
  puVar4[7] = 0x20;
  if (*(char *)((int)param_1 + 0x41) == '!') {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbd;
    }
    else {
      uVar3 = 0xffffff0d;
    }
    *puVar4 = *puVar4 & uVar3;
    param_1[0xc] = (uint *)0x11;
  }
  else if (*(char *)((int)param_1 + 0x41) == '\"') {
    if ((*(byte *)((int)param_1 + 0x41) & 0x28) == 0x28) {
      uVar3 = 0xffffffbb;
    }
    else {
      uVar3 = 0xffffff0b;
    }
    *puVar4 = uVar3 & *puVar4;
    param_1[0xc] = (uint *)0x12;
  }
  puVar4[1] = puVar4[1] & DAT_0800e0b8;
  puVar2 = DAT_0800e0bc;
  param_1[0xd] = (uint *)0x0;
  param_1[0xb] = puVar2;
  if (param_2 << 0x1b < 0) {
    puVar4[7] = 0x10;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 4);
  }
  if ((int)(puVar4[6] << 0x1e) < 0) {
    puVar4[10] = 0;
  }
  if (-1 < (int)(puVar4[6] << 0x1f)) {
    puVar4[6] = puVar4[6] | 1;
  }
  if ((*(char *)((int)param_1 + 0x41) != '`') && (param_1[0x11] == (uint *)0x0)) {
    if (*(char *)((int)param_1 + 0x41) == '!') {
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      param_1[0xc] = (uint *)0x0;
      cVar1 = *(char *)((int)param_1 + 0x42);
      *(undefined *)((int)param_1 + 0x42) = 0;
      if (cVar1 == '@') {
        FUN_0800dad0();
      }
      else {
        FUN_0800800c();
      }
    }
    else if (*(char *)((int)param_1 + 0x41) == '\"') {
      *(undefined *)(param_1 + 0x10) = 0;
      *(undefined *)((int)param_1 + 0x41) = 0x20;
      param_1[0xc] = (uint *)0x0;
      cVar1 = *(char *)((int)param_1 + 0x42);
      *(undefined *)((int)param_1 + 0x42) = 0;
      if (cVar1 == '@') {
        FUN_0800dad4();
      }
      else {
        FUN_08008014();
      }
    }
    return;
  }
  FUN_0800db1c(param_1,param_1[0x11]);
  return;
}



/* === 0800e4f8 FUN_0800e4f8 === */

void FUN_0800e4f8(int param_1)

{
  ushort uVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint **ppuVar7;
  
  ppuVar7 = *(uint ***)(param_1 + 0x38);
  puVar6 = *ppuVar7;
  *puVar6 = *puVar6 & 0xffffbfff;
  if (*(short *)((int)ppuVar7 + 0x2a) == 0) {
    if ((ppuVar7[0xd] == DAT_0800e588) || (ppuVar7[0xd] == DAT_0800e58c)) {
      uVar4 = 0x60;
    }
    else {
      uVar4 = 0x20;
    }
    *puVar6 = *puVar6 | uVar4;
    return;
  }
  puVar5 = ppuVar7[9];
  uVar1 = *(ushort *)(ppuVar7 + 10);
  ppuVar7[9] = (uint *)((int)puVar5 + (uint)uVar1);
  if (*(ushort *)((int)ppuVar7 + 0x2a) < 0x100) {
    uVar2 = *(undefined2 *)((int)ppuVar7 + 0x2a);
  }
  else {
    uVar2 = 0xff;
  }
  *(undefined2 *)(ppuVar7 + 10) = uVar2;
  iVar3 = FUN_0800b3a0(ppuVar7[0xe],(uint *)((int)puVar5 + (uint)uVar1),puVar6 + 10,uVar2);
  if (iVar3 == 0) {
    if ((ppuVar7[0xd] == DAT_0800e588) || (ppuVar7[0xd] == DAT_0800e58c)) {
      **ppuVar7 = **ppuVar7 | 0x40;
    }
    else {
      **ppuVar7 = **ppuVar7;
    }
    return;
  }
  FUN_0800db1c(ppuVar7,0x10);
  return;
}



/* === 0800e590 FUN_0800e590 === */

void FUN_0800e590(int param_1)

{
  ushort uVar1;
  undefined2 uVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  uint **ppuVar7;
  
  ppuVar7 = *(uint ***)(param_1 + 0x38);
  puVar4 = *ppuVar7;
  *puVar4 = *puVar4 & 0xffff7fff;
  if (*(short *)((int)ppuVar7 + 0x2a) == 0) {
    if ((ppuVar7[0xd] == DAT_0800e620) || (ppuVar7[0xd] == DAT_0800e624)) {
      uVar5 = 0x60;
    }
    else {
      uVar5 = 0x20;
    }
    *puVar4 = *puVar4 | uVar5;
    return;
  }
  puVar6 = ppuVar7[9];
  uVar1 = *(ushort *)(ppuVar7 + 10);
  ppuVar7[9] = (uint *)((int)puVar6 + (uint)uVar1);
  if (*(ushort *)((int)ppuVar7 + 0x2a) < 0x100) {
    uVar2 = *(undefined2 *)((int)ppuVar7 + 0x2a);
  }
  else {
    uVar2 = 0xff;
  }
  *(undefined2 *)(ppuVar7 + 10) = uVar2;
  iVar3 = FUN_0800b3a0(ppuVar7[0xf],puVar4 + 9,(uint *)((int)puVar6 + (uint)uVar1),uVar2);
  if (iVar3 == 0) {
    if ((ppuVar7[0xd] == DAT_0800e620) || (ppuVar7[0xd] == DAT_0800e624)) {
      **ppuVar7 = **ppuVar7 | 0x40;
    }
    else {
      **ppuVar7 = **ppuVar7;
    }
    return;
  }
  FUN_0800db1c(ppuVar7,0x10);
  return;
}



/* === 0800e628 FUN_0800e628 === */

undefined FUN_0800e628(int param_1)

{
  return *(undefined *)(param_1 + 0x41);
}



/* === 0800e630 FUN_0800e630 === */

undefined4 FUN_0800e630(uint **param_1,uint param_2)

{
  uint *puVar1;
  
  if ((*(char *)((int)param_1 + 0x41) == ' ') && (*(char *)(param_1 + 0x10) != '\x01')) {
    puVar1 = *param_1;
    *(undefined *)((int)param_1 + 0x41) = 0x24;
    *puVar1 = *puVar1 & 0xfffffffe;
    *puVar1 = *puVar1 & 0xffffefff;
    *puVar1 = param_2 | *puVar1;
    *puVar1 = *puVar1 | 1;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)(param_1 + 0x10) = 0;
    return 0;
  }
  return 2;
}



/* === 0800e684 FUN_0800e684 === */

undefined4 FUN_0800e684(uint **param_1,int param_2)

{
  uint *puVar1;
  
  if (*(char *)((int)param_1 + 0x41) != ' ') {
    return 2;
  }
  if (*(char *)(param_1 + 0x10) != '\x01') {
    puVar1 = *param_1;
    *(undefined *)((int)param_1 + 0x41) = 0x24;
    *puVar1 = *puVar1 & 0xfffffffe;
    *puVar1 = *puVar1 & 0xfffff0ff | param_2 << 8;
    *puVar1 = *puVar1 | 1;
    *(undefined *)((int)param_1 + 0x41) = 0x20;
    *(undefined *)(param_1 + 0x10) = 0;
    return 0;
  }
  return 2;
}



/* === 0800ee50 FUN_0800ee50 === */

undefined4 FUN_0800ee50(undefined4 *param_1,undefined param_2)

{
  if (*(char *)(param_1 + 0x12f) != '\x01') {
    *(undefined *)(param_1 + 0xe) = param_2;
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012b3c(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}



/* === 0800ee7c FUN_0800ee7c === */

undefined4 FUN_0800ee7c(undefined4 *param_1,uint param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  uVar3 = param_2 & 0xf;
  if ((int)(param_2 << 0x18) < 0) {
    iVar1 = uVar3 * 9 + 0xf;
    *(undefined *)((int)param_1 + uVar3 * 0x24 + 0x3d) = 1;
  }
  else {
    iVar1 = uVar3 * 9 + 0x9f;
    *(undefined *)((int)param_1 + uVar3 * 0x24 + 0x27d) = 0;
  }
  puVar2 = param_1 + iVar1;
  puVar2[2] = param_3;
  *(char *)puVar2 = (char)uVar3;
  *(char *)(puVar2 + 1) = (char)param_4;
  if (*(char *)((int)puVar2 + 1) != '\0') {
    *(short *)((int)puVar2 + 0x1a) = (short)uVar3;
  }
  if (param_4 == 2) {
    *(undefined *)((int)puVar2 + 5) = 0;
  }
  if (*(char *)(param_1 + 0x12f) != '\x01') {
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012670(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}



/* === 0800eef8 FUN_0800eef8 === */

undefined4 FUN_0800eef8(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  param_2 = param_2 & 0xf;
  param_1[param_2 * 9 + 0xa3] = param_4;
  param_1[param_2 * 9 + 0xa2] = param_3;
  *(char *)(param_1 + param_2 * 9 + 0x9f) = (char)param_2;
  param_1[param_2 * 9 + 0xa4] = 0;
  *(undefined *)((int)param_1 + param_2 * 0x24 + 0x27d) = 0;
  uVar2 = param_1[3];
  uVar1 = *param_1;
  if (uVar2 == 1) {
    param_1[param_2 * 9 + 0xa6] = param_3;
  }
  FUN_08012700(uVar1,param_1 + param_2 * 9 + 0x9f,uVar2 & 0xff,uVar2,param_4);
  return 0;
}



/* === 0800ef3c FUN_0800ef3c === */

undefined4 FUN_0800ef3c(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  uint uVar2;
  
  param_2 = param_2 & 0xf;
  param_1[param_2 * 9 + 0x13] = param_4;
  param_1[param_2 * 9 + 0x12] = param_3;
  param_1[param_2 * 9 + 0x14] = 0;
  *(char *)(param_1 + param_2 * 9 + 0xf) = (char)param_2;
  *(undefined *)((int)param_1 + param_2 * 0x24 + 0x3d) = 1;
  uVar2 = param_1[3];
  uVar1 = *param_1;
  if (uVar2 == 1) {
    param_1[param_2 * 9 + 0x16] = param_3;
  }
  FUN_08012700(uVar1,param_1 + param_2 * 9 + 0xf,uVar2 & 0xff,uVar2,param_4);
  return 0;
}



/* === 0800ef80 FUN_0800ef80 === */

undefined4 FUN_0800ef80(undefined4 *param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  
  uVar2 = param_2 & 0xf;
  if ((uint)param_1[1] < uVar2) {
    return 1;
  }
  if ((int)(param_2 << 0x18) < 0) {
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3d) = 1;
    *(char *)(param_1 + uVar2 * 9 + 0xf) = (char)uVar2;
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3e) = 1;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  else {
    *(undefined *)((int)param_1 + param_2 * 0x24 + 0x27d) = 0;
    *(char *)(param_1 + param_2 * 9 + 0x9f) = (char)uVar2;
    *(undefined *)((int)param_1 + param_2 * 0x24 + 0x27e) = 1;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  if (cVar1 != '\x01') {
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012a7c(*param_1);
    if (uVar2 == 0) {
      FUN_08012c00(*param_1,*(undefined *)(param_1 + 3),param_1 + 0x131);
    }
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}



/* === 0800f008 FUN_0800f008 === */

undefined4 FUN_0800f008(undefined4 *param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  
  uVar2 = param_2 & 0xf;
  if ((uint)param_1[1] < uVar2) {
    return 1;
  }
  if ((param_2 & 0x80) == 0) {
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x27d) = 0;
    *(char *)(param_1 + uVar2 * 9 + 0x9f) = (char)uVar2;
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x27e) = 0;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  else {
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3d) = 1;
    *(char *)(param_1 + uVar2 * 9 + 0xf) = (char)uVar2;
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3e) = 0;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  if (cVar1 != '\x01') {
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012ae4(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}



/* === 0800f080 FUN_0800f080 === */

void FUN_0800f080(void)

{
  return;
}



/* === 0800f084 FUN_0800f084 === */

int FUN_0800f084(uint param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = DAT_0800f0c8;
  if ((*(uint *)(DAT_0800f0c8 + 0xc) & 4) == 0) {
    iVar1 = (*(uint *)(DAT_0800f0c8 + 0xc) & 7) - param_1;
    if (iVar1 != 0) {
      iVar1 = 1;
    }
    return iVar1;
  }
  *(uint *)(DAT_0800f0c8 + 0xc) = param_1 | *(uint *)(DAT_0800f0c8 + 0xc) & 0xfffffff8;
  iVar2 = FUN_08009ce8();
  do {
    if (*(int *)(iVar1 + 4) << 0x12 < 0) {
      return 0;
    }
    iVar3 = FUN_08009ce8();
  } while ((uint)(iVar3 - iVar2) < 0x3e9);
  return 1;
}



/* === 0800f0cc FUN_0800f0cc === */

void FUN_0800f0cc(void)

{
  *(uint *)(DAT_0800f0d8 + 0xc) = *(uint *)(DAT_0800f0d8 + 0xc) | 0x1000000;
  return;
}



/* === 0800f0dc FUN_0800f0dc === */

void FUN_0800f0dc(int *param_1,uint *param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = param_2[9];
  if ((uVar4 != 0) && (param_3 != 0xc000000)) {
    *(uint *)(*param_1 + 0x10) = param_2[10] - 1;
  }
  uVar2 = param_2[6];
  if (uVar2 == 0) {
    if (param_2[8] == 0) {
      if (param_2[7] == 0) {
        if (uVar4 == 0) {
          return;
        }
        *(uint *)(*param_1 + 0x14) =
             param_2[0xb] | uVar4 | param_3 | param_2[0xc] | param_2[0xd] | param_2[5] << 0x12;
        return;
      }
      iVar1 = *param_1;
      uVar4 = param_2[7] | uVar4 | param_3 | param_2[0xb] | param_2[0xc];
      uVar2 = param_2[0xd];
    }
    else {
      iVar1 = *param_1;
      uVar4 = param_2[8] | uVar4;
      uVar2 = param_2[7];
      *(uint *)(iVar1 + 0x1c) = param_2[2];
      if (uVar2 == 0) {
        *(uint *)(iVar1 + 0x14) =
             uVar4 | param_3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | param_2[4] |
             param_2[5] << 0x12;
        return;
      }
      uVar4 = uVar4 | param_3 | uVar2 | param_2[0xb] | param_2[0xc] | param_2[0xd];
      uVar2 = param_2[4];
    }
    uVar4 = uVar4 | uVar2;
    uVar2 = param_2[3];
  }
  else {
    uVar3 = param_2[8];
    if (uVar3 == 0) {
      if (param_2[7] == 0) {
        *(uint *)(*param_1 + 0x14) =
             uVar4 | uVar2 | param_3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | *param_2 |
             param_2[5] << 0x12;
        return;
      }
      iVar1 = *param_1;
      *(uint *)(iVar1 + 0x14) =
           uVar4 | uVar2 | param_3 | param_2[7] | param_2[0xb] | param_2[0xc] | param_2[0xd] |
           param_2[3] | *param_2 | param_2[5] << 0x12;
      if (param_3 == 0xc000000) {
        return;
      }
      *(uint *)(iVar1 + 0x18) = param_2[1];
      return;
    }
    iVar1 = *param_1;
    *(uint *)(iVar1 + 0x1c) = param_2[2];
    if (param_2[7] == 0) {
      *(uint *)(iVar1 + 0x14) =
           uVar4 | uVar2 | param_3 | uVar3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | param_2[4]
           | *param_2 | param_2[5] << 0x12;
      return;
    }
    uVar4 = uVar2 | uVar4 | param_3 | uVar3 | param_2[7] | param_2[0xb] | param_2[0xc] |
            param_2[0xd] | param_2[4] | param_2[3];
    uVar2 = *param_2;
  }
  *(uint *)(iVar1 + 0x14) = uVar4 | uVar2 | param_2[5] << 0x12;
  if (param_3 == 0xc000000) {
    return;
  }
  *(uint *)(iVar1 + 0x18) = param_2[1];
  return;
}



/* === 0800f250 FUN_0800f250 === */

undefined4 FUN_0800f250(uint **param_1)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  
  iVar1 = FUN_08009ce8();
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x41) == '\0') {
    FUN_08008af4(param_1);
    puVar4 = (uint *)0x1388;
    param_1[0x12] = (uint *)0x1388;
  }
  else {
    puVar4 = param_1[0x12];
  }
  puVar3 = *param_1;
  *puVar3 = *puVar3 & 0xffffe0ff | ((int)param_1[2] + -1) * 0x100;
  while( true ) {
    do {
      if ((puVar3[2] & 0x20) == 0) {
        puVar4 = param_1[5];
        *puVar3 = (uint)param_1[3] | (uint)param_1[7] | (uint)param_1[8] | (int)param_1[1] << 0x18 |
                  DAT_0800f300 & *puVar3;
        puVar3[1] = (uint)param_1[6] | (uint)puVar4 | (int)param_1[4] << 0x10 |
                    DAT_0800f304 & puVar3[1];
        *puVar3 = *puVar3 | 1;
        param_1[0x11] = (uint *)0x0;
        *(undefined *)((int)param_1 + 0x41) = 1;
        return 0;
      }
    } while (puVar4 == (uint *)0xffffffff);
    iVar2 = FUN_08009ce8();
    if ((puVar4 < (uint *)(iVar2 - iVar1)) || (puVar4 == (uint *)0x0)) break;
    puVar3 = *param_1;
  }
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
  return 1;
}



/* === 0800f308 FUN_0800f308 === */

undefined4 FUN_0800f308(uint **param_1)

{
  if (param_1 != (uint **)0x0) {
    **param_1 = **param_1 & 0xfffffffe;
    FUN_08008c6c();
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0;
    return 0;
  }
  return 1;
}



/* === 0800f32c FUN_0800f32c === */

undefined FUN_0800f32c(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  undefined uVar3;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) != '\x01') {
    uVar3 = 2;
    goto LAB_0800f358;
  }
  param_1[0x11] = 0;
  *(undefined *)((int)param_1 + 0x41) = 2;
  do {
    do {
      if ((*(uint *)(*param_1 + 8) & 0x20) == 0) {
        FUN_0800f0dc(param_1,param_2,0);
        if (*(int *)(param_2 + 0x24) == 0) goto LAB_0800f3d0;
        *(undefined *)((int)param_1 + 0x41) = 1;
        uVar3 = 0;
        goto LAB_0800f358;
      }
    } while (param_3 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_3) && (param_3 != 0));
  goto LAB_0800f3a8;
LAB_0800f3d0:
  do {
    do {
      if (*(int *)(*param_1 + 8) << 0x1e < 0) {
        uVar3 = 0;
        *(undefined4 *)(*param_1 + 0xc) = 2;
        *(undefined *)((int)param_1 + 0x41) = 1;
        goto LAB_0800f358;
      }
    } while (param_3 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f3a8:
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = param_1[0x11] | 1;
  uVar3 = 1;
LAB_0800f358:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar3;
}



/* === 0800f3f0 FUN_0800f3f0 === */

undefined FUN_0800f3f0(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined uVar4;
  
  iVar1 = FUN_08009ce8();
  iVar3 = *param_1;
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    param_1[0x11] = 0;
    uVar4 = 1;
    if (param_2 == 0) {
      param_1[0x11] = param_1[0x11] | 8;
    }
    else {
      *(undefined *)((int)param_1 + 0x41) = 0x12;
      param_1[0xb] = *(int *)(iVar3 + 0x10) + 1;
      iVar2 = *(int *)(iVar3 + 0x10);
      param_1[9] = param_2;
      param_1[10] = iVar2 + 1;
      *(uint *)(iVar3 + 0x14) = *(uint *)(iVar3 + 0x14) & 0xf3ffffff;
      iVar2 = iVar3;
      if (param_1[0xb] == 0) goto LAB_0800f486;
LAB_0800f460:
      if (-1 < *(int *)(iVar2 + 8) << 0x1d) goto LAB_0800f45c;
      *(undefined *)(iVar3 + 0x20) = *(undefined *)param_1[9];
      param_1[0xb] = param_1[0xb] + -1;
      param_1[9] = param_1[9] + 1;
      if (param_1[0xb] != 0) goto LAB_0800f47e;
      do {
        iVar3 = *param_1;
LAB_0800f486:
        do {
          if (*(int *)(iVar3 + 8) << 0x1e < 0) {
            uVar4 = 0;
            *(undefined4 *)(iVar3 + 0xc) = 2;
            goto LAB_0800f494;
          }
        } while (param_3 == 0xffffffff);
        iVar3 = FUN_08009ce8();
      } while (((uint)(iVar3 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f4ba:
      *(undefined *)((int)param_1 + 0x41) = 4;
      param_1[0x11] = param_1[0x11] | 1;
LAB_0800f494:
      *(undefined *)((int)param_1 + 0x41) = 1;
    }
  }
  else {
    uVar4 = 2;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar4;
LAB_0800f45c:
  if (param_3 != 0xffffffff) {
    iVar2 = FUN_08009ce8();
    if ((param_3 < (uint)(iVar2 - iVar1)) || (param_3 == 0)) goto LAB_0800f4ba;
LAB_0800f47e:
    iVar2 = *param_1;
  }
  goto LAB_0800f460;
}



/* === 0800f4e0 FUN_0800f4e0 === */

undefined FUN_0800f4e0(int *param_1,int param_2,uint param_3)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined uVar5;
  
  iVar1 = FUN_08009ce8();
  iVar4 = *param_1;
  uVar2 = *(undefined4 *)(iVar4 + 0x18);
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    param_1[0x11] = 0;
    uVar5 = 1;
    if (param_2 == 0) {
      param_1[0x11] = param_1[0x11] | 8;
    }
    else {
      *(undefined *)((int)param_1 + 0x41) = 0x22;
      param_1[0xe] = *(int *)(iVar4 + 0x10) + 1;
      iVar3 = *(int *)(iVar4 + 0x10);
      param_1[0xc] = param_2;
      param_1[0xd] = iVar3 + 1;
      *(uint *)(iVar4 + 0x14) = *(uint *)(iVar4 + 0x14) & 0xf3ffffff | 0x4000000;
      *(undefined4 *)(iVar4 + 0x18) = uVar2;
      iVar3 = iVar4;
      if (param_1[0xe] == 0) goto LAB_0800f580;
LAB_0800f558:
      if ((*(uint *)(iVar3 + 8) & 6) == 0) goto LAB_0800f554;
      *(undefined *)param_1[0xc] = *(undefined *)(iVar4 + 0x20);
      param_1[0xe] = param_1[0xe] + -1;
      param_1[0xc] = param_1[0xc] + 1;
      if (param_1[0xe] != 0) goto LAB_0800f578;
      do {
        iVar4 = *param_1;
LAB_0800f580:
        do {
          if (*(int *)(iVar4 + 8) << 0x1e < 0) {
            uVar5 = 0;
            *(undefined4 *)(iVar4 + 0xc) = 2;
            goto LAB_0800f58e;
          }
        } while (param_3 == 0xffffffff);
        iVar4 = FUN_08009ce8();
      } while (((uint)(iVar4 - iVar1) <= param_3) && (param_3 != 0));
LAB_0800f5b4:
      *(undefined *)((int)param_1 + 0x41) = 4;
      param_1[0x11] = param_1[0x11] | 1;
LAB_0800f58e:
      *(undefined *)((int)param_1 + 0x41) = 1;
    }
  }
  else {
    uVar5 = 2;
  }
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar5;
LAB_0800f554:
  if (param_3 != 0xffffffff) {
    iVar3 = FUN_08009ce8();
    if ((param_3 < (uint)(iVar3 - iVar1)) || (param_3 == 0)) goto LAB_0800f5b4;
LAB_0800f578:
    iVar3 = *param_1;
  }
  goto LAB_0800f558;
}



/* === 0800f5d8 FUN_0800f5d8 === */

undefined FUN_0800f5d8(uint **param_1,int param_2,uint *param_3,uint param_4)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  undefined uVar5;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) != '\x01') {
    uVar5 = 2;
    goto LAB_0800f606;
  }
  param_1[0x11] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x41) = 0x42;
  do {
    puVar4 = *param_1;
    do {
      if (-1 < (int)(puVar4[2] << 0x1a)) {
        puVar4[10] = *param_3;
        puVar4[9] = param_3[1];
        puVar4[0xb] = param_3[2];
        uVar3 = param_3[3];
        *puVar4 = param_3[4] | *puVar4 & 0xff3fffff | 0x400000;
        *(uint *)(param_2 + 0x28) = uVar3;
        FUN_0800f0dc(param_1,param_2,0x8000000);
        goto LAB_0800f65a;
      }
    } while (param_4 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_4) && (param_4 != 0));
  goto LAB_0800f6a4;
LAB_0800f65a:
  do {
    do {
      if ((int)((*param_1)[2] << 0x1c) < 0) {
        uVar5 = 0;
        (*param_1)[3] = 8;
        *(undefined *)((int)param_1 + 0x41) = 1;
        goto LAB_0800f606;
      }
    } while (param_4 == 0xffffffff);
    iVar2 = FUN_08009ce8();
  } while (((uint)(iVar2 - iVar1) <= param_4) && (param_4 != 0));
LAB_0800f6a4:
  *(undefined *)((int)param_1 + 0x41) = 4;
  param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
  uVar5 = 1;
LAB_0800f606:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar5;
}



/* === 0800f6b4 FUN_0800f6b4 === */

undefined FUN_0800f6b4(uint **param_1,undefined4 param_2,uint *param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  undefined uVar6;
  
  iVar1 = FUN_08009ce8();
  if (*(char *)(param_1 + 0x10) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x10) = 1;
  if (*(char *)((int)param_1 + 0x41) == '\x01') {
    puVar5 = param_1[0x12];
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0x82;
    do {
      puVar4 = *param_1;
      do {
        if (-1 < (int)(puVar4[2] << 0x1a)) {
          uVar3 = param_3[1];
          *puVar4 = *puVar4 & 0xfffffff7 | uVar3;
          if (uVar3 == 8) {
            puVar4[0xc] = *param_3;
            puVar4[3] = 0x10;
            *puVar4 = *puVar4 | 0x100000;
          }
          uVar6 = 0;
          FUN_0800f0dc(param_1,param_2,0xc000000);
          goto LAB_0800f6e0;
        }
      } while (puVar5 == (uint *)0xffffffff);
      iVar2 = FUN_08009ce8();
    } while (((uint *)(iVar2 - iVar1) <= puVar5) && (puVar5 != (uint *)0x0));
    *(undefined *)((int)param_1 + 0x41) = 4;
    param_1[0x11] = (uint *)((uint)param_1[0x11] | 1);
    uVar6 = 1;
  }
  else {
    uVar6 = 2;
  }
LAB_0800f6e0:
  *(undefined *)(param_1 + 0x10) = 0;
  return uVar6;
}



/* === 0800f768 FUN_0800f768 === */

void FUN_0800f768(void)

{
  return;
}



/* === 0800f7a4 FUN_0800f7a4 === */

void FUN_0800f7a4(void)

{
  return;
}



/* === 0800f7a8 FUN_0800f7a8 === */

void FUN_0800f7a8(void)

{
  return;
}



/* === 0800f7ac FUN_0800f7ac === */

void FUN_0800f7ac(void)

{
  return;
}



/* === 0800f7b0 FUN_0800f7b0 === */

void FUN_0800f7b0(void)

{
  return;
}



/* === 0800f7b4 FUN_0800f7b4 === */

void FUN_0800f7b4(void)

{
  return;
}



/* === 0800f7b8 FUN_0800f7b8 === */

void FUN_0800f7b8(void)

{
  return;
}



/* === 0800f7bc FUN_0800f7bc === */

void FUN_0800f7bc(void)

{
  return;
}



/* === 0800f9d0 FUN_0800f9d0 === */

uint FUN_0800f9d0(void)

{
  uint uVar1;
  uint uVar2;
  float fVar3;
  float fVar4;
  
  uVar1 = (uint)(DAT_0800fae8[10] << 0x16) >> 0x1a;
  if ((DAT_0800fae8[10] & 0x3f0U) != 0) {
    uVar2 = DAT_0800fae8[10] & 3;
    fVar4 = (float)(longlong)
                   (int)((DAT_0800fae8[0xb] & 1U) * ((uint)(DAT_0800fae8[0xd] << 0x10) >> 0x13));
    fVar3 = DAT_0800faec;
    if (((uVar2 == 1) || (fVar3 = DAT_0800faf8, uVar2 == 2)) || (fVar3 = DAT_0800faec, uVar2 != 0))
    {
      fVar3 = (fVar3 / (float)(longlong)(int)uVar1) *
              ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0);
    }
    else if (*DAT_0800fae8 << 0x1a < 0) {
      fVar3 = ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0) *
              ((float)(longlong)(int)(DAT_0800faf4 >> ((uint)(*DAT_0800fae8 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar1);
    }
    else {
      fVar3 = (DAT_0800fafc / (float)(longlong)(int)uVar1) *
              ((float)(longlong)(int)(DAT_0800fae8[0xc] & 0x1ff) + fVar4 * DAT_0800faf0 + 1.0);
    }
    fVar3 = fVar3 / (float)(longlong)(int)(((uint)(DAT_0800fae8[0xc] << 0x10) >> 0x19) + 1);
    uVar1 = (uint)(0.0 < fVar3) * (int)fVar3;
  }
  return uVar1;
}



/* === 0800fb00 FUN_0800fb00 === */

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



/* === 080100f4 FUN_080100f4 === */

uint FUN_080100f4(void)

{
  uint uVar1;
  
  uVar1 = DAT_08010128[4] & 0x38;
  if (uVar1 == 0x10) {
    return DAT_08010130;
  }
  if (uVar1 == 0x18) {
    uVar1 = FUN_0800f9d0();
    return uVar1;
  }
  if (uVar1 != 0) {
    return DAT_0801012c;
  }
  if (*DAT_08010128 << 0x1a < 0) {
    return DAT_08010134 >> ((uint)(*DAT_08010128 << 0x1b) >> 0x1e);
  }
  return DAT_08010134;
}



/* === 08010138 FUN_08010138 === */

undefined4 FUN_08010138(int *param_1,uint param_2)

{
  byte bVar1;
  uint *puVar2;
  int *piVar3;
  int *piVar4;
  int iVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  
  puVar2 = DAT_08010370;
  if (param_1 == (int *)0x0) {
    return 1;
  }
  if (((*DAT_08010370 & 0xf) < param_2) &&
     (*DAT_08010370 = *DAT_08010370 & 0xfffffff0 | param_2, (*puVar2 & 0xf) != param_2)) {
    return 1;
  }
  iVar8 = *param_1;
  if ((iVar8 << 0x1d < 0) && ((DAT_08010374[6] & 0x70U) < (uint)param_1[4])) {
    DAT_08010374[6] = DAT_08010374[6] & 0xffffff8fU | param_1[4];
  }
  if ((iVar8 << 0x1c < 0) && ((DAT_08010374[7] & 0x70U) < (uint)param_1[5])) {
    DAT_08010374[7] = DAT_08010374[7] & 0xffffff8fU | param_1[5];
  }
  if ((iVar8 << 0x1b < 0) && ((DAT_08010374[7] & 0x700U) < (uint)param_1[6])) {
    DAT_08010374[7] = DAT_08010374[7] & 0xfffff8ffU | param_1[6];
  }
  if ((iVar8 << 0x1a < 0) && ((DAT_08010374[8] & 0x70U) < (uint)param_1[7])) {
    DAT_08010374[8] = DAT_08010374[8] & 0xffffff8fU | param_1[7];
  }
  if (iVar8 << 0x1e < 0) {
    uVar7 = param_1[3];
    if ((DAT_08010374[6] & 0xfU) < uVar7) {
      DAT_08010374[6] = DAT_08010374[6] & 0xfffffff0U | uVar7;
    }
    if (iVar8 << 0x1f < 0) goto LAB_08010200;
  }
  else {
    if (-1 < iVar8 << 0x1f) goto LAB_0801026e;
LAB_08010200:
    piVar3 = DAT_08010374;
    DAT_08010374[6] = DAT_08010374[6] & 0xfffff0ffU | param_1[2];
    piVar4 = DAT_08010374;
    uVar7 = param_1[1];
    iVar8 = *piVar3;
    if (uVar7 == 2) {
      iVar8 = iVar8 << 0xe;
    }
    else if (uVar7 == 3) {
      iVar8 = iVar8 << 6;
    }
    else if (uVar7 == 1) {
      iVar8 = iVar8 << 0x17;
    }
    else {
      iVar8 = iVar8 << 0x1d;
    }
    if (-1 < iVar8) {
      return 1;
    }
    DAT_08010374[4] = DAT_08010374[4] & 0xfffffff8U | uVar7;
    iVar8 = FUN_08009ce8();
    while ((piVar4[4] & 0x38U) != param_1[1] * 8) {
      iVar5 = FUN_08009ce8();
      if (5000 < (uint)(iVar5 - iVar8)) {
        return 3;
      }
    }
    iVar8 = *param_1;
    if (-1 < iVar8 << 0x1e) goto LAB_0801026e;
    uVar7 = param_1[3];
  }
  if (uVar7 < (DAT_08010374[6] & 0xfU)) {
    DAT_08010374[6] = uVar7 | DAT_08010374[6] & 0xfffffff0U;
  }
LAB_0801026e:
  puVar2 = DAT_08010370;
  if ((param_2 < (*DAT_08010370 & 0xf)) &&
     (*DAT_08010370 = *DAT_08010370 & 0xfffffff0 | param_2, (*puVar2 & 0xf) != param_2)) {
    return 1;
  }
  if ((iVar8 << 0x1d < 0) && ((uint)param_1[4] < (DAT_08010374[6] & 0x70U))) {
    DAT_08010374[6] = DAT_08010374[6] & 0xffffff8fU | param_1[4];
  }
  if ((iVar8 << 0x1c < 0) && ((uint)param_1[5] < (DAT_08010374[7] & 0x70U))) {
    DAT_08010374[7] = DAT_08010374[7] & 0xffffff8fU | param_1[5];
  }
  if ((iVar8 << 0x1b < 0) && ((uint)param_1[6] < (DAT_08010374[7] & 0x700U))) {
    DAT_08010374[7] = DAT_08010374[7] & 0xfffff8ffU | param_1[6];
  }
  if ((iVar8 << 0x1a < 0) && ((uint)param_1[7] < (DAT_08010374[8] & 0x70U))) {
    DAT_08010374[8] = DAT_08010374[8] & 0xffffff8fU | param_1[7];
  }
  uVar7 = FUN_080100f4();
  puVar2 = DAT_08010380;
  bVar1 = *(byte *)(DAT_08010378 + (DAT_08010374[6] & 0xfU));
  uVar7 = uVar7 >> (*(byte *)(DAT_08010378 + ((uint)(DAT_08010374[6] << 0x14) >> 0x1c)) & 0x1f);
  uVar6 = *DAT_08010384;
  *DAT_0801037c = uVar7;
  *puVar2 = uVar7 >> (bVar1 & 0x1f);
  uVar6 = FUN_08009c24(uVar6);
  return uVar6;
}



/* === 08010388 FUN_08010388 === */

void FUN_08010388(void)

{
  byte bVar1;
  uint *puVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = DAT_080103ec[4] & 0x38;
  uVar3 = DAT_08010400;
  if (uVar4 != 0x10) {
    if (uVar4 == 0x18) {
      uVar3 = FUN_0800f9d0();
    }
    else {
      uVar3 = DAT_080103f0;
      if ((uVar4 == 0) && (uVar3 = DAT_08010404, *DAT_080103ec << 0x1a < 0)) {
        uVar3 = DAT_08010404 >> ((uint)(*DAT_080103ec << 0x1b) >> 0x1e);
      }
    }
  }
  puVar2 = DAT_080103f8;
  bVar1 = *(byte *)(DAT_080103f4 + (DAT_080103ec[6] & 0xfU));
  uVar3 = uVar3 >> (*(byte *)(DAT_080103f4 + ((uint)(DAT_080103ec[6] << 0x14) >> 0x1c)) & 0x1f);
  *DAT_080103fc = uVar3;
  *puVar2 = uVar3 >> (bVar1 & 0x1f);
  return;
}



/* === 08010408 FUN_08010408 === */

uint FUN_08010408(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_08010428 + ((uint)(*(int *)(DAT_08010424 + 0x1c) << 0x19) >> 0x1d))
                  & 0x1f);
}



/* === 0801042c FUN_0801042c === */

uint FUN_0801042c(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_0801044c + ((uint)(*(int *)(DAT_08010448 + 0x1c) << 0x15) >> 0x1d))
                  & 0x1f);
}



/* === 08010450 FUN_08010450 === */

undefined4 FUN_08010450(int *param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  puVar1 = DAT_0801053c;
  if ((DAT_0801053c[10] & 3) == 3) {
    return 1;
  }
  *DAT_0801053c = *DAT_0801053c & 0xfbffffff;
  iVar2 = FUN_08009ce8();
  while ((int)(*puVar1 << 4) < 0) {
    iVar3 = FUN_08009ce8();
    if (2 < (uint)(iVar3 - iVar2)) {
      return 3;
    }
  }
  puVar1[10] = puVar1[10] & 0xfffc0fff | *param_1 << 0xc;
  puVar1[0xe] = (param_1[2] + -1) * 0x200 & 0xffffU | (param_1[3] + -1) * 0x10000 & 0x7f0000U |
                param_1[1] - 1U & 0x1ff | (param_1[4] + -1) * 0x1000000 & 0x7f000000U;
  puVar1[0xb] = puVar1[0xb] & 0xffffff3f | param_1[5];
  uVar4 = DAT_08010540;
  puVar1[0xb] = puVar1[0xb] & 0xffffffdf | param_1[6];
  puVar1[0xb] = puVar1[0xb] & 0xffffffef;
  puVar1[0xf] = uVar4 & puVar1[0xf] | param_1[7] << 3;
  puVar1[0xb] = puVar1[0xb] | 0x10;
  uVar4 = puVar1[0xb];
  if (param_2 == 0) {
    puVar1[0xb] = uVar4 | 0x80000;
  }
  else {
    if (param_2 == 1) {
      uVar4 = uVar4 | 0x100000;
    }
    else {
      uVar4 = uVar4 | 0x200000;
    }
    puVar1[0xb] = uVar4;
  }
  puVar1 = DAT_0801053c;
  *DAT_0801053c = *DAT_0801053c | 0x4000000;
  iVar2 = FUN_08009ce8();
  do {
    if ((int)(*puVar1 << 4) < 0) {
      return 0;
    }
    iVar3 = FUN_08009ce8();
  } while ((uint)(iVar3 - iVar2) < 3);
  return 3;
}



/* === 08010544 FUN_08010544 === */

undefined4 FUN_08010544(int *param_1,int param_2)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  
  puVar1 = DAT_08010630;
  if ((DAT_08010630[10] & 3) == 3) {
    return 1;
  }
  *DAT_08010630 = *DAT_08010630 & 0xefffffff;
  iVar2 = FUN_08009ce8();
  while ((int)(*puVar1 << 2) < 0) {
    iVar3 = FUN_08009ce8();
    if (2 < (uint)(iVar3 - iVar2)) {
      return 3;
    }
  }
  puVar1[10] = puVar1[10] & 0xfc0fffff | *param_1 << 0x14;
  puVar1[0x10] = (param_1[2] + -1) * 0x200 & 0xffffU | (param_1[3] + -1) * 0x10000 & 0x7f0000U |
                 param_1[1] - 1U & 0x1ff | (param_1[4] + -1) * 0x1000000 & 0x7f000000U;
  puVar1[0xb] = puVar1[0xb] & 0xfffff3ff | param_1[5];
  uVar4 = DAT_08010634;
  puVar1[0xb] = puVar1[0xb] & 0xfffffdff | param_1[6];
  puVar1[0xb] = puVar1[0xb] & 0xfffffeff;
  puVar1[0x11] = uVar4 & puVar1[0x11] | param_1[7] << 3;
  puVar1[0xb] = puVar1[0xb] | 0x100;
  uVar4 = puVar1[0xb];
  if (param_2 == 0) {
    puVar1[0xb] = uVar4 | 0x400000;
  }
  else {
    if (param_2 == 1) {
      uVar4 = uVar4 | 0x800000;
    }
    else {
      uVar4 = uVar4 | 0x1000000;
    }
    puVar1[0xb] = uVar4;
  }
  puVar1 = DAT_08010630;
  *DAT_08010630 = *DAT_08010630 | 0x10000000;
  iVar2 = FUN_08009ce8();
  do {
    if ((int)(*puVar1 << 2) < 0) {
      return 0;
    }
    iVar3 = FUN_08009ce8();
  } while ((uint)(iVar3 - iVar2) < 3);
  return 3;
}



/* === 08010638 FUN_08010638 === */

uint FUN_08010638(uint *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  uVar9 = *param_1;
  uVar8 = param_1[1];
  uVar10 = uVar9 & 0x8000000;
  if ((int)(uVar9 << 4) < 0) {
    uVar5 = param_1[0x1a];
    if (uVar5 == 0x200000) {
      uVar10 = FUN_08010544(param_1 + 10,2,uVar8,uVar9,param_4);
      if (uVar10 == 0) {
LAB_08010674:
        uVar5 = param_1[0x1a];
        uVar9 = *param_1;
        uVar8 = param_1[1];
        goto LAB_08010684;
      }
LAB_080110b0:
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else {
      if (uVar5 < 0x200001) {
        if (uVar5 != 0) {
          if (uVar5 == 0x100000) {
            uVar10 = FUN_08010450(param_1 + 2,2);
            if (uVar10 != 0) goto LAB_080110b0;
            goto LAB_08010674;
          }
LAB_080111a0:
          uVar10 = 1;
          goto LAB_08010692;
        }
        *(uint *)(DAT_08011228 + 0x2c) = *(uint *)(DAT_08011228 + 0x2c) | 0x20000;
      }
      else if (uVar5 != 0x300000) goto LAB_080111a0;
LAB_08010684:
      uVar10 = 0;
      *(uint *)(DAT_08010940 + 0x50) = uVar5 | *(uint *)(DAT_08010940 + 0x50) & 0xffcfffff;
    }
  }
LAB_08010692:
  uVar5 = uVar10;
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x17) < 0) {
    switch(param_1[0x16]) {
    case 0:
      *(uint *)(DAT_08011228 + 0x2c) = *(uint *)(DAT_08011228 + 0x2c) | 0x20000;
      break;
    case 1:
      uVar5 = FUN_08010450(param_1 + 2,0);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      break;
    case 2:
      uVar5 = FUN_08010544(param_1 + 10,0,uVar8,uVar9,param_4);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      break;
    case 3:
    case 4:
      break;
    default:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_080106ae;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010d00 + 0x50) = *(uint *)(DAT_08010d00 + 0x50) & 0xfffffff8 | param_1[0x16];
      uVar6 = uVar10;
    }
  }
LAB_080106ae:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x16) < 0) {
    uVar10 = param_1[0x17];
    if (uVar10 == 0x80) {
      uVar5 = FUN_08010544(param_1 + 10,0,uVar8,uVar9,param_4);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x81) {
      if (uVar10 == 0) {
        *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
      }
      else {
        if (uVar10 != 0x40) goto LAB_080108be;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if ((uVar10 != 0xc0) && (uVar10 != 0x100)) {
LAB_080108be:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_080106ee;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x50) = *(uint *)(DAT_08010940 + 0x50) & 0xfffffe3f | param_1[0x17];
      uVar10 = uVar6;
    }
  }
LAB_080106ee:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x15) < 0) {
    uVar6 = param_1[0x2a];
    if (uVar6 == 0x400000) {
      uVar5 = FUN_08010544(param_1 + 10,0,uVar8,uVar9,param_4);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar6 < 0x400001) {
      if (uVar6 == 0) {
        *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
      }
      else {
        if (uVar6 != 0x200000) goto LAB_080108d4;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if ((uVar6 != 0x600000) && (uVar6 != 0x800000)) {
LAB_080108d4:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_08010736;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x58) = *(uint *)(DAT_08010940 + 0x58) & 0xff1fffff | param_1[0x2a];
      uVar6 = uVar10;
    }
  }
LAB_08010736:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x14) < 0) {
    uVar10 = param_1[0x2b];
    if (uVar10 == 0x2000000) {
      uVar5 = FUN_08010544(param_1 + 10,0,uVar8,uVar9,param_4);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x2000001) {
      if (uVar10 == 0) {
        *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
      }
      else {
        if (uVar10 != 0x1000000) goto LAB_080108ea;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if ((uVar10 != 0x3000000) && (uVar10 != 0x4000000)) {
LAB_080108ea:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_0801077e;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x58) = *(uint *)(DAT_08010940 + 0x58) & 0xf8ffffff | param_1[0x2b];
      uVar10 = uVar6;
    }
  }
LAB_0801077e:
  uVar6 = uVar10;
  if ((int)(uVar9 << 6) < 0) {
    uVar6 = param_1[0x13];
    if (uVar6 == 0x20) {
      uVar5 = FUN_08010450(param_1 + 2,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar6 < 0x21) {
      if (uVar6 != 0) {
        if (uVar6 != 0x10) goto LAB_080108f6;
        *(uint *)(DAT_08010940 + 0x2c) = *(uint *)(DAT_08010940 + 0x2c) | 0x20000;
      }
    }
    else if (uVar6 != 0x30) {
LAB_080108f6:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_080107b4;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x4c) = *(uint *)(DAT_08010940 + 0x4c) & 0xffffffcf | param_1[0x13];
      uVar6 = uVar10;
    }
  }
LAB_080107b4:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x13) < 0) {
    uVar10 = param_1[0x18];
    if (uVar10 == 0x2000) {
      uVar5 = FUN_08010544(param_1 + 10,0);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x2001) {
      if (uVar10 == 0) {
        *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
      }
      else {
        if (uVar10 != 0x1000) goto LAB_0801090c;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if ((uVar10 != 0x3000) && (uVar10 != 0x4000)) {
LAB_0801090c:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_080107f8;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x50) = *(uint *)(DAT_08010940 + 0x50) & 0xffff8fff | param_1[0x18];
      uVar10 = uVar6;
    }
  }
LAB_080107f8:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x12) < 0) {
    uVar6 = param_1[0x19];
    if (uVar6 == 0x20000) {
      uVar5 = FUN_08010544(param_1 + 10,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar6 < 0x20001) {
      if (uVar6 != 0) {
        if (uVar6 != 0x10000) goto LAB_08010922;
        uVar5 = FUN_08010450(param_1 + 2,1);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if (((uVar6 & 0xfffeffff) != 0x40000) && (uVar6 != 0x30000)) {
LAB_08010922:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_08010838;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x50) = *(uint *)(DAT_08010940 + 0x50) & 0xfff8ffff | param_1[0x19];
      uVar6 = uVar10;
    }
  }
LAB_08010838:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x11) < 0) {
    uVar10 = param_1[0x2c];
    if (uVar10 == 0x20000000) {
      uVar5 = FUN_08010544(param_1 + 10,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x20000001) {
      if (uVar10 != 0) {
        if (uVar10 != 0x10000000) goto LAB_08010938;
        uVar5 = FUN_08010450(param_1 + 2,1);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if (((uVar10 & 0xefffffff) != 0x40000000) && (uVar10 != 0x30000000)) {
LAB_08010938:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_08010878;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010940 + 0x58) = *(uint *)(DAT_08010940 + 0x58) & 0x8fffffff | param_1[0x2c];
      uVar10 = uVar6;
    }
  }
LAB_08010878:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x10) < 0) {
    uVar6 = param_1[0x1c];
    if (uVar6 == 0x10000000) {
      *(uint *)(DAT_08011228 + 0x2c) = *(uint *)(DAT_08011228 + 0x2c) | 0x20000;
    }
    else if (uVar6 == 0x20000000) {
      uVar5 = FUN_08010450(param_1 + 2,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar6 != 0) {
      uVar5 = 1;
      uVar6 = 1;
      goto LAB_08010898;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010d00 + 0x50) = *(uint *)(DAT_08010d00 + 0x50) & 0xcfffffff | param_1[0x1c];
      uVar6 = uVar10;
    }
  }
LAB_08010898:
  uVar10 = uVar6;
  if ((int)(uVar9 << 7) < 0) {
    switch(param_1[0x12]) {
    case 0:
    case 3:
      break;
    case 1:
      *(uint *)(DAT_08011228 + 0x2c) = *(uint *)(DAT_08011228 + 0x2c) | 0x20000;
      uVar10 = uVar5;
      if (uVar5 != 0) goto LAB_0801095c;
      goto LAB_08010f50;
    case 2:
      uVar5 = FUN_08010450(param_1 + 2,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      break;
    default:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_0801095c;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
LAB_08010f50:
      *(uint *)(DAT_08010fe0 + 0x4c) = *(uint *)(DAT_08010fe0 + 0x4c) & 0xfffffffc | param_1[0x12];
      uVar10 = uVar6;
    }
  }
LAB_0801095c:
  puVar1 = DAT_08010fe4;
  if ((int)(uVar9 << 9) < 0) {
    *DAT_08010fe4 = *DAT_08010fe4 | 0x100;
    iVar2 = FUN_08009ce8();
    do {
      iVar3 = DAT_08010fe0;
      if ((int)(*puVar1 << 0x17) < 0) {
        if (uVar5 != 0) {
          uVar9 = *param_1;
          uVar8 = param_1[1];
          uVar10 = uVar5;
          goto LAB_08010962;
        }
        uVar6 = param_1[0x2d];
        if (((*(uint *)(DAT_08010fe0 + 0x70) ^ uVar6) & 0x300) != 0) {
          uVar9 = *(uint *)(DAT_08010fe0 + 0x70);
          *(uint *)(DAT_08010fe0 + 0x70) = *(uint *)(DAT_08010fe0 + 0x70) | 0x10000;
          *(uint *)(iVar3 + 0x70) = *(uint *)(iVar3 + 0x70) & 0xfffeffff;
          *(uint *)(iVar3 + 0x70) = uVar9 & 0xfffffcff;
        }
        if (uVar6 != 0x100) goto LAB_08010e72;
        iVar3 = FUN_08009ce8();
        iVar2 = DAT_08011228;
        goto LAB_080111e6;
      }
      iVar3 = FUN_08009ce8();
    } while ((uint)(iVar3 - iVar2) < 0x65);
LAB_08011194:
    uVar5 = 3;
    uVar9 = *param_1;
    uVar8 = param_1[1];
    uVar10 = uVar5;
  }
LAB_08010962:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x1f) < 0) {
    switch(param_1[0x1f]) {
    case 0:
    case 0x18:
    case 0x20:
    case 0x28:
      break;
    default:
      uVar5 = 1;
      uVar6 = 1;
      goto LAB_080109c6;
    case 8:
      uVar5 = FUN_08010450(param_1 + 2,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      uVar6 = uVar5;
      if (uVar5 != 0) goto LAB_080109c6;
      goto LAB_08010d6a;
    case 0x10:
      uVar5 = FUN_08010544(param_1 + 10,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
LAB_08010d6a:
      *(uint *)(DAT_08010fe0 + 0x54) = *(uint *)(DAT_08010fe0 + 0x54) & 0xffffffc7 | param_1[0x1f];
      uVar6 = uVar10;
    }
  }
LAB_080109c6:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x1e) < 0) {
    switch(param_1[0x1e]) {
    case 0:
    case 3:
    case 4:
    case 5:
      break;
    case 1:
      uVar5 = FUN_08010450(param_1 + 2,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      uVar10 = uVar5;
      if (uVar5 != 0) goto LAB_080109fa;
      goto LAB_08010f2a;
    case 2:
      uVar5 = FUN_08010544(param_1 + 10,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      break;
    default:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_080109fa;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
LAB_08010f2a:
      *(uint *)(DAT_08010fe0 + 0x54) = *(uint *)(DAT_08010fe0 + 0x54) & 0xfffffff8 | param_1[0x1e];
      uVar10 = uVar6;
    }
  }
LAB_080109fa:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x1d) < 0) {
    switch(param_1[0x25]) {
    case 0:
    case 3:
    case 4:
    case 5:
      break;
    case 1:
      uVar5 = FUN_08010450(param_1 + 2,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      uVar6 = uVar5;
      if (uVar5 != 0) goto LAB_08010a30;
      goto LAB_08010f3a;
    case 2:
      uVar5 = FUN_08010544(param_1 + 10,1);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      break;
    default:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_08010a30;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
LAB_08010f3a:
      *(uint *)(DAT_08010fe0 + 0x58) = *(uint *)(DAT_08010fe0 + 0x58) & 0xfffffff8 | param_1[0x25];
      uVar6 = uVar10;
    }
  }
LAB_08010a30:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x1a) < 0) {
    uVar10 = param_1[0x24];
    if (uVar10 == 0x20000000) {
      uVar5 = FUN_08010544(param_1 + 10,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x20000001) {
      if (uVar10 != 0) {
        if (uVar10 != 0x10000000) goto LAB_08010c6e;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if (((uVar10 & 0xefffffff) != 0x40000000) && (uVar10 != 0x30000000)) {
LAB_08010c6e:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_08010a74;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010d00 + 0x54) = *(uint *)(DAT_08010d00 + 0x54) & 0x8fffffff | param_1[0x24];
      uVar10 = uVar6;
    }
  }
LAB_08010a74:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0x19) < 0) {
    uVar6 = param_1[0x27];
    if (uVar6 == 0x800) {
      uVar5 = FUN_08010544(param_1 + 10,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar6 < 0x801) {
      if (uVar6 != 0) {
        if (uVar6 != 0x400) goto LAB_08010c88;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if (((uVar6 & 0xfffffbff) != 0x1000) && (uVar6 != 0xc00)) {
LAB_08010c88:
      uVar5 = 1;
      uVar6 = uVar5;
      goto LAB_08010ab8;
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010d00 + 0x58) = *(uint *)(DAT_08010d00 + 0x58) & 0xffffe3ff | param_1[0x27];
      uVar6 = uVar10;
    }
  }
LAB_08010ab8:
  uVar10 = uVar6;
  if ((int)(uVar9 << 0x18) < 0) {
    uVar10 = param_1[0x28];
    if (uVar10 == 0x4000) {
      uVar5 = FUN_08010544(param_1 + 10,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    else if (uVar10 < 0x4001) {
      if (uVar10 != 0) {
        if (uVar10 != 0x2000) goto LAB_08010ca2;
        uVar5 = FUN_08010450(param_1 + 2,0);
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    else if (((uVar10 & 0xffffdfff) != 0x8000) && (uVar10 != 0x6000)) {
LAB_08010ca2:
      uVar5 = 1;
      uVar10 = uVar5;
      goto LAB_08010afc;
    }
    uVar10 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010d00 + 0x58) = *(uint *)(DAT_08010d00 + 0x58) & 0xffff1fff | param_1[0x28];
      uVar10 = uVar6;
    }
  }
LAB_08010afc:
  if ((int)(uVar9 << 0x1c) < 0) {
    uVar6 = param_1[0x21];
    if (uVar6 == 0x1000) {
      iVar2 = FUN_08010544(param_1 + 10,2);
      if (iVar2 == 0) {
        uVar6 = param_1[0x21];
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
      else {
        uVar6 = param_1[0x21];
        uVar10 = 1;
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    *(uint *)(DAT_08010d00 + 0x54) = *(uint *)(DAT_08010d00 + 0x54) & 0xffffcfff | uVar6;
  }
  if ((int)(uVar9 << 0x1b) < 0) {
    uVar6 = param_1[0x26];
    if (uVar6 == 0x100) {
      iVar2 = FUN_08010544(param_1 + 10,2);
      if (iVar2 == 0) {
        uVar6 = param_1[0x26];
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
      else {
        uVar6 = param_1[0x26];
        uVar10 = 1;
        uVar9 = *param_1;
        uVar8 = param_1[1];
      }
    }
    *(uint *)(DAT_08010d00 + 0x58) = *(uint *)(DAT_08010d00 + 0x58) & 0xfffffcff | uVar6;
  }
  uVar6 = uVar10;
  if (-1 < (int)(uVar9 << 0xc)) goto LAB_08010b56;
  uVar6 = param_1[0x29];
  if (uVar6 == 0x10000) {
    uVar5 = FUN_08010544(param_1 + 10,2);
    uVar9 = *param_1;
    uVar8 = param_1[1];
LAB_08010d3c:
    uVar6 = uVar5;
    if (uVar5 == 0) {
LAB_08010d42:
      *(uint *)(DAT_08010fe0 + 0x58) = *(uint *)(DAT_08010fe0 + 0x58) & 0xfffcffff | param_1[0x29];
      uVar6 = uVar10;
    }
  }
  else {
    if (uVar6 == 0x20000) goto LAB_08010d3c;
    if (uVar6 == 0) {
      uVar5 = FUN_08010450(param_1 + 2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
      uVar6 = uVar5;
      if (uVar5 != 0) goto LAB_08010b56;
      goto LAB_08010d42;
    }
    uVar5 = 1;
    uVar6 = 1;
  }
LAB_08010b56:
  uVar10 = uVar6;
  if (-1 < (int)(uVar9 << 0xd)) goto LAB_08010b7a;
  uVar10 = param_1[0x22];
  if (uVar10 == 0x200000) {
    uVar5 = FUN_08010544(param_1 + 10,1);
    uVar9 = *param_1;
    uVar8 = param_1[1];
LAB_08010d14:
    uVar10 = uVar5;
    if (uVar5 == 0) {
LAB_08010d1a:
      *(uint *)(DAT_08010fe0 + 0x54) = *(uint *)(DAT_08010fe0 + 0x54) & 0xffcfffff | param_1[0x22];
      uVar10 = uVar6;
    }
  }
  else {
    if (uVar10 == 0x300000) goto LAB_08010d14;
    if (uVar10 == 0x100000) {
      *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
      uVar10 = uVar5;
      if (uVar5 != 0) goto LAB_08010b7a;
      goto LAB_08010d1a;
    }
    uVar5 = 1;
    uVar10 = 1;
  }
LAB_08010b7a:
  uVar6 = uVar10;
  if ((int)(uVar9 << 0xf) < 0) {
    if (param_1[0x14] == 0) {
      *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
    }
    else {
      if (param_1[0x14] != 0x10000) {
        uVar5 = 1;
        uVar6 = 1;
        goto LAB_08010b92;
      }
      uVar5 = FUN_08010450(param_1 + 2,2);
      uVar9 = *param_1;
      uVar8 = param_1[1];
    }
    uVar6 = uVar5;
    if (uVar5 == 0) {
      *(uint *)(DAT_08010fe0 + 0x4c) = *(uint *)(DAT_08010fe0 + 0x4c) & 0xfffeffff | param_1[0x14];
      uVar6 = uVar10;
    }
  }
LAB_08010b92:
  if ((int)(uVar9 << 2) < 0) {
    iVar2 = FUN_08010544(param_1 + 10,2);
    uVar9 = *param_1;
    uVar8 = param_1[1];
    if (iVar2 != 0) {
      uVar6 = 1;
    }
  }
  uVar10 = uVar6;
  if ((int)(uVar9 << 0xe) < 0) {
    uVar7 = param_1[0x20];
    if (uVar7 == 0x100) {
      *(uint *)(DAT_08010fe0 + 0x2c) = *(uint *)(DAT_08010fe0 + 0x2c) | 0x20000;
joined_r0x08010f88:
      uVar10 = uVar5;
      if (uVar5 == 0) {
        *(uint *)(DAT_08010d00 + 0x54) = uVar7 | *(uint *)(DAT_08010d00 + 0x54) & 0xfffffcff;
        uVar10 = uVar6;
      }
    }
    else {
      if (uVar7 < 0x101) {
        if (uVar7 == 0) goto joined_r0x08010f88;
      }
      else if ((uVar7 & 0xfffffeff) == 0x200) goto joined_r0x08010f88;
      uVar10 = 1;
    }
  }
  if ((int)(uVar9 << 0xb) < 0) {
    *(uint *)(DAT_08010d00 + 0x50) = *(uint *)(DAT_08010d00 + 0x50) & 0x7fffffff | param_1[0x1d];
  }
  if ((int)(uVar9 << 3) < 0) {
    *(uint *)(DAT_08010d00 + 0x10) = *(uint *)(DAT_08010d00 + 0x10) & 0xffffbfff | param_1[0x2e];
  }
  if ((int)(uVar9 << 10) < 0) {
    *(uint *)(DAT_08010d00 + 0x50) = *(uint *)(DAT_08010d00 + 0x50) & 0xfeffffff | param_1[0x1b];
  }
  iVar2 = DAT_08010d00;
  if ((int)(uVar9 << 1) < 0) {
    *(uint *)(DAT_08010d00 + 0x10) = *(uint *)(DAT_08010d00 + 0x10) & 0xffff7fff;
    *(uint *)(iVar2 + 0x10) = *(uint *)(iVar2 + 0x10) | param_1[0x2f];
  }
  if ((int)uVar9 < 0) {
    *(uint *)(DAT_08010d00 + 0x4c) = *(uint *)(DAT_08010d00 + 0x4c) & 0xcfffffff | param_1[0x15];
  }
  if ((int)(uVar9 << 8) < 0) {
    *(uint *)(DAT_08010d00 + 0x54) = *(uint *)(DAT_08010d00 + 0x54) & 0xff3fffff | param_1[0x23];
  }
  if ((int)(uVar8 << 0x1f) < 0) {
    uVar9 = FUN_08010450(param_1 + 2,0);
    uVar8 = param_1[1];
    if (uVar9 == 0) goto LAB_08010c32;
    if ((int)(uVar8 << 0x1e) < 0) goto LAB_08010dac;
LAB_08010c38:
    uVar10 = uVar9;
    if (-1 < (int)(uVar8 << 0x1d)) goto LAB_08010c3e;
LAB_08010dc6:
    uVar9 = FUN_08010450(param_1 + 2,2);
    uVar8 = param_1[1];
    if (uVar9 == 0) goto LAB_08010c3e;
    if ((int)(uVar8 << 0x1c) < 0) goto LAB_08010de0;
LAB_08010c44:
    uVar10 = uVar9;
    uVar9 = uVar10;
    if (-1 < (int)(uVar8 << 0x1b)) goto LAB_08010c4a;
LAB_08010dfa:
    uVar9 = FUN_08010544(param_1 + 10,1);
    if (uVar9 == 0) {
      uVar8 = param_1[1];
      uVar9 = uVar10;
      goto LAB_08010c4a;
    }
    if (-1 < (int)(param_1[1] << 0x1a)) {
      return 1;
    }
  }
  else {
LAB_08010c32:
    uVar9 = uVar10;
    if (-1 < (int)(uVar8 << 0x1e)) goto LAB_08010c38;
LAB_08010dac:
    uVar10 = FUN_08010450(param_1 + 2,1);
    uVar8 = param_1[1];
    if (uVar10 == 0) goto LAB_08010c38;
    if ((int)(uVar8 << 0x1d) < 0) goto LAB_08010dc6;
LAB_08010c3e:
    uVar9 = uVar10;
    if (-1 < (int)(uVar8 << 0x1c)) goto LAB_08010c44;
LAB_08010de0:
    uVar10 = FUN_08010544(param_1 + 10,0);
    uVar8 = param_1[1];
    if (uVar10 == 0) goto LAB_08010c44;
    uVar9 = uVar10;
    if ((int)(uVar8 << 0x1b) < 0) goto LAB_08010dfa;
LAB_08010c4a:
    if (-1 < (int)(uVar8 << 0x1a)) goto LAB_08010c50;
  }
  iVar2 = FUN_08010544(param_1 + 10,2);
  if (iVar2 != 0) {
    return 1;
  }
LAB_08010c50:
  if (uVar9 != 0) {
    uVar9 = 1;
  }
  return uVar9;
LAB_080111e6:
  if (-1 < *(int *)(iVar2 + 0x70) << 0x1e) goto LAB_080111dc;
  uVar6 = param_1[0x2d];
LAB_08010e72:
  if ((uVar6 & 0x300) == 0x300) {
    *(uint *)(DAT_08011228 + 0x10) =
         DAT_0801122c & uVar6 >> 4 | *(uint *)(DAT_08011228 + 0x10) & 0xffffc0ff;
  }
  else {
    *(uint *)(DAT_08010fe0 + 0x10) = *(uint *)(DAT_08010fe0 + 0x10) & 0xffffc0ff;
  }
  uVar9 = *param_1;
  uVar8 = param_1[1];
  *(uint *)(DAT_08010fe0 + 0x70) = uVar6 & 0xfff | *(uint *)(DAT_08010fe0 + 0x70);
  goto LAB_08010962;
LAB_080111dc:
  iVar4 = FUN_08009ce8();
  if (5000 < (uint)(iVar4 - iVar3)) goto LAB_08011194;
  goto LAB_080111e6;
}



/* === 08011230 FUN_08011230 === */

uint FUN_08011230(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_08011250 + ((uint)(*(int *)(DAT_0801124c + 0x20) << 0x19) >> 0x1d))
                  & 0x1f);
}



/* === 08011254 FUN_08011254 === */

void FUN_08011254(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  
  piVar1 = DAT_080113b8;
  uVar3 = (uint)(DAT_080113b8[10] << 0xe) >> 0x1a;
  if ((DAT_080113b8[10] & 0x3f000U) != 0) {
    uVar2 = DAT_080113b8[10] & 3;
    fVar5 = (float)(longlong)
                   (int)(-((DAT_080113b8[0xb] << 0x1b) >> 0x1f) *
                        ((uint)(DAT_080113b8[0xf] << 0x10) >> 0x13));
    fVar4 = DAT_080113bc;
    if (((uVar2 == 1) || (fVar4 = DAT_080113c8, uVar2 == 2)) || (fVar4 = DAT_080113bc, uVar2 != 0))
    {
      fVar4 = (fVar4 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0);
    }
    else if (*DAT_080113b8 << 0x1a < 0) {
      fVar4 = ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0) *
              ((float)(longlong)(int)(DAT_080113c4 >> ((uint)(*DAT_080113b8 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar3);
    }
    else {
      fVar4 = (DAT_080113cc / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_080113b8[0xe] & 0x1ff) + fVar5 * DAT_080113c0 + 1.0);
    }
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(DAT_080113b8[0xe] << 0x10) >> 0x19) + 1.0);
    *param_1 = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xe] << 9) >> 0x19) + 1.0);
    param_1[1] = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xe] << 1) >> 0x19) + 1.0);
    param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
    return;
  }
  *param_1 = uVar3;
  param_1[1] = uVar3;
  param_1[2] = uVar3;
  return;
}



/* === 080113d0 FUN_080113d0 === */

void FUN_080113d0(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  
  piVar1 = DAT_08011534;
  uVar3 = (uint)(DAT_08011534[10] << 6) >> 0x1a;
  if ((DAT_08011534[10] & 0x3f00000U) != 0) {
    uVar2 = DAT_08011534[10] & 3;
    fVar5 = (float)(longlong)
                   (int)(-((DAT_08011534[0xb] << 0x17) >> 0x1f) *
                        ((uint)(DAT_08011534[0x11] << 0x10) >> 0x13));
    fVar4 = DAT_08011538;
    if (((uVar2 == 1) || (fVar4 = DAT_08011544, uVar2 == 2)) || (fVar4 = DAT_08011538, uVar2 != 0))
    {
      fVar4 = (fVar4 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0);
    }
    else if (*DAT_08011534 << 0x1a < 0) {
      fVar4 = ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0) *
              ((float)(longlong)(int)(DAT_08011540 >> ((uint)(*DAT_08011534 << 0x1b) >> 0x1e)) /
              (float)(longlong)(int)uVar3);
    }
    else {
      fVar4 = (DAT_08011548 / (float)(longlong)(int)uVar3) *
              ((float)(longlong)(int)(DAT_08011534[0x10] & 0x1ff) + fVar5 * DAT_0801153c + 1.0);
    }
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(DAT_08011534[0x10] << 0x10) >> 0x19) + 1.0);
    *param_1 = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar5 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0x10] << 9) >> 0x19) + 1.0);
    param_1[1] = (uint)(0.0 < fVar5) * (int)fVar5;
    fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0x10] << 1) >> 0x19) + 1.0);
    param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
    return;
  }
  *param_1 = uVar3;
  param_1[1] = uVar3;
  param_1[2] = uVar3;
  return;
}



/* === 0801154c FUN_0801154c === */

void FUN_0801154c(uint *param_1)

{
  int *piVar1;
  uint uVar2;
  uint uVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  
  piVar1 = DAT_080116c0;
  uVar3 = (uint)(DAT_080116c0[10] << 0x16) >> 0x1a;
  if ((DAT_080116c0[10] & 0x3f0U) == 0) {
    *param_1 = uVar3;
    param_1[1] = uVar3;
    param_1[2] = uVar3;
    return;
  }
  uVar2 = DAT_080116c0[10] & 3;
  fVar4 = (float)(longlong)
                 (int)((DAT_080116c0[0xb] & 1U) * ((uint)(DAT_080116c0[0xd] << 0x10) >> 0x13));
  if (uVar2 == 1) {
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116d4;
  }
  else if (uVar2 == 2) {
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116d0;
  }
  else {
    if (uVar2 == 0) {
      if (*DAT_080116c0 << 0x1a < 0) {
        fVar4 = ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0) *
                ((float)(longlong)(int)(DAT_080116cc >> ((uint)(*DAT_080116c0 << 0x1b) >> 0x1e)) /
                (float)(longlong)(int)uVar3);
      }
      else {
        fVar4 = (DAT_080116c4 / (float)(longlong)(int)uVar3) *
                ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0);
      }
      goto LAB_080115b8;
    }
    fVar5 = (float)(longlong)(int)uVar3;
    fVar6 = DAT_080116c4;
  }
  fVar4 = (fVar6 / fVar5) *
          ((float)(longlong)(int)(DAT_080116c0[0xc] & 0x1ff) + fVar4 * DAT_080116c8 + 1.0);
LAB_080115b8:
  fVar6 = fVar4 / ((float)(longlong)(int)((uint)(DAT_080116c0[0xc] << 0x10) >> 0x19) + 1.0);
  *param_1 = (uint)(0.0 < fVar6) * (int)fVar6;
  fVar6 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xc] << 9) >> 0x19) + 1.0);
  param_1[1] = (uint)(0.0 < fVar6) * (int)fVar6;
  fVar4 = fVar4 / ((float)(longlong)(int)((uint)(piVar1[0xc] << 1) >> 0x19) + 1.0);
  param_1[2] = (uint)(0.0 < fVar4) * (int)fVar4;
  return;
}



/* === 080116d8 FUN_080116d8 === */

uint FUN_080116d8(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint local_14;
  uint local_10;
  uint local_c;
  
  if ((param_1 - 0x100U | param_2) == 0) {
    uVar2 = DAT_080119ac;
    switch(DAT_080119a4[0x14] & 7) {
    case 0:
LAB_080118a4:
      uVar2 = *DAT_080119a4;
      goto joined_r0x080118aa;
    case 1:
switchD_080117fc_caseD_1:
      uVar2 = *DAT_080119a4;
      goto joined_r0x08011886;
    case 2:
      uVar1 = *DAT_080119a4;
      break;
    case 3:
      goto LAB_080117e8;
    case 4:
LAB_0801177c:
      uVar2 = DAT_080119a4[0x13] & 0x30000000;
      if (((int)(*DAT_080119a4 << 0x1d) < 0) && (uVar2 == 0)) {
LAB_08011910:
        return DAT_080119b0 >> ((*DAT_080119a4 << 0x1b) >> 0x1e);
      }
LAB_0801178e:
      if (((int)(*DAT_080119a4 << 0x17) < 0) && (uVar2 == 0x10000000)) {
        return DAT_080119b4;
      }
      if (((int)(*DAT_080119a4 << 0xe) < 0) && (uVar2 == 0x20000000)) {
        return DAT_080119a8;
      }
switchD_080117fc_caseD_5:
      return 0;
    default:
      goto switchD_080117fc_caseD_5;
    }
  }
  else {
    if ((param_1 - 0x200U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x14] & 0x1c0;
      if (uVar2 != 0x80) {
        if (0x80 < uVar2) {
          if (uVar2 == 0xc0) {
            return DAT_080119ac;
          }
          if (uVar2 != 0x100) {
            return 0;
          }
          goto LAB_0801177c;
        }
        if (uVar2 != 0) {
          if (uVar2 != 0x40) {
            return 0;
          }
LAB_080117bc:
          uVar2 = *DAT_080119a4;
joined_r0x08011886:
          if ((uVar2 & 0x8000000) != 0) {
            FUN_08011254(&local_14);
            return local_14;
          }
          return 0;
        }
LAB_08011852:
        uVar2 = *DAT_080119a4;
joined_r0x080118aa:
        if ((uVar2 & 0x2000000) != 0) {
          FUN_0801154c(&local_14);
          return local_10;
        }
        return 0;
      }
    }
    else if ((param_1 - 0x400U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x16] & 0xe00000;
      if (uVar2 != 0x400000) {
        if (0x400000 < uVar2) {
          if (uVar2 == 0x600000) {
            return DAT_080119ac;
          }
          if (uVar2 != 0x800000) {
            return 0;
          }
          goto LAB_0801177c;
        }
        if (uVar2 != 0) {
          if (uVar2 != 0x200000) {
            return 0;
          }
          goto LAB_080117bc;
        }
        goto LAB_08011852;
      }
    }
    else if ((param_1 - 0x800U | param_2) == 0) {
      uVar2 = DAT_080119a4[0x16] & 0x7000000;
      if (uVar2 != 0x2000000) {
        if (uVar2 < 0x2000001) {
          if (uVar2 != 0) {
            if (uVar2 != 0x1000000) {
              return 0;
            }
            goto LAB_080117bc;
          }
          goto LAB_08011852;
        }
        if (uVar2 == 0x3000000) {
          return DAT_080119ac;
        }
        if (uVar2 != 0x4000000) {
          return 0;
        }
LAB_08011830:
        uVar2 = DAT_080119a4[0x13] & 0x30000000;
        if (((int)(*DAT_080119a4 << 0x1d) < 0) && (uVar2 == 0)) {
          return DAT_080119b0 >> ((*DAT_080119a4 << 0x1b) >> 0x1e);
        }
        goto LAB_0801178e;
      }
    }
    else {
      if ((param_1 - 0x1000U | param_2) != 0) {
        if ((param_1 - 0x2000U | param_2) == 0) {
          uVar2 = DAT_080119a4[0x14] & 0x70000;
          if (uVar2 == 0x30000) {
            if ((*DAT_080119a4 & 4) == 0) {
              return 0;
            }
            goto LAB_08011910;
          }
          if (uVar2 < 0x30001) {
            if (uVar2 != 0x10000) {
              if (uVar2 != 0x20000) {
                if (uVar2 == 0) {
                  uVar2 = FUN_08010408();
                  return uVar2;
                }
                return 0;
              }
LAB_08011954:
              if ((*DAT_080119a4 & 0x20000000) != 0) {
                FUN_080113d0(&local_14);
                return local_10;
              }
              return 0;
            }
LAB_08011a00:
            uVar2 = *DAT_080119a4;
joined_r0x08011a56:
            if ((uVar2 & 0x8000000) != 0) {
              FUN_08011254(&local_14);
              return local_10;
            }
            return 0;
          }
          if (uVar2 == 0x40000) {
            uVar2 = *DAT_080119a4;
joined_r0x08011a30:
            if ((uVar2 & 0x100) != 0) {
              return DAT_080119b4;
            }
            return 0;
          }
          if (uVar2 != 0x50000) {
            return 0;
          }
        }
        else {
          if ((param_1 - 0x80000U | param_2) == 0) {
            uVar2 = DAT_080119a4[0x16] & 0x30000;
            if (uVar2 == 0x10000) {
              if ((*DAT_080119a4 & 0x20000000) != 0) {
                FUN_080113d0(&local_14);
                return local_c;
              }
              return 0;
            }
            if (uVar2 != 0x20000) {
              if (uVar2 != 0) {
                return 0;
              }
              goto switchD_080117fc_caseD_1;
            }
            goto LAB_08011830;
          }
          if ((param_1 - 0x10000U | param_2) == 0) {
            if ((int)(DAT_080119a4[0x13] << 0xf) < 0) {
              if ((*DAT_080119a4 & 0x8000000) != 0) {
                FUN_08011254(&local_14);
                return local_c;
              }
              return 0;
            }
            goto LAB_080118a4;
          }
          if ((param_1 - 0x4000U | param_2) != 0) {
            if ((param_1 - 0x8000U | param_2) != 0) {
              return 0;
            }
            uVar2 = DAT_080119a4[0x14] & 0x30000000;
            if (uVar2 == 0x10000000) goto LAB_08011852;
            if (uVar2 != 0x20000000) {
              if (uVar2 != 0) {
                return 0;
              }
              uVar2 = *DAT_080119a4;
              goto joined_r0x0801175c;
            }
            goto LAB_08011a00;
          }
          uVar2 = DAT_08011a5c[0x16] & 0x70000000;
          if (uVar2 == 0x30000000) {
            if ((*DAT_08011a5c & 4) != 0) {
              return DAT_08011a64 >> ((*DAT_08011a5c << 0x1b) >> 0x1e);
            }
            return 0;
          }
          if (uVar2 < 0x30000001) {
            if (uVar2 != 0x10000000) {
              if (uVar2 != 0x20000000) {
                if (uVar2 != 0) {
                  return 0;
                }
                uVar2 = FUN_08010388();
                return uVar2 >> (*(byte *)(DAT_08011a60 + ((DAT_08011a5c[8] << 0x19) >> 0x1d)) &
                                0x1f);
              }
              goto LAB_08011954;
            }
            uVar2 = *DAT_08011a5c;
            goto joined_r0x08011a56;
          }
          if (uVar2 == 0x40000000) {
            uVar2 = *DAT_08011a5c;
            goto joined_r0x08011a30;
          }
          if (uVar2 != 0x50000000) {
            return 0;
          }
        }
        uVar2 = *DAT_080119a4;
joined_r0x0801175c:
        if ((uVar2 & 0x20000) != 0) {
          return DAT_080119a8;
        }
        return 0;
      }
      uVar2 = DAT_080119a4[0x14] & 0x7000;
      if (uVar2 != 0x2000) {
        if (uVar2 < 0x2001) {
          if (uVar2 != 0) {
            if (uVar2 != 0x1000) {
              return 0;
            }
            goto LAB_080117bc;
          }
          goto LAB_08011852;
        }
        if (uVar2 == 0x3000) {
          return DAT_080119ac;
        }
        if (uVar2 != 0x4000) {
          return 0;
        }
        goto LAB_08011830;
      }
    }
    uVar1 = *DAT_080119a4;
  }
  uVar2 = uVar1 & 0x20000000;
  if ((uVar1 & 0x20000000) != 0) {
    FUN_080113d0(&local_14);
    uVar2 = local_14;
  }
LAB_080117e8:
  return uVar2;
}



/* === 08011a68 HAL_SAI_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 HAL_SAI_Init(uint **param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  int iVar8;
  uint *puVar9;
  uint *puVar10;
  uint uVar11;
  bool bVar12;
  
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  FUN_08009d18();
  puVar4 = *param_1;
  if ((*(char *)(param_1 + 0xe) == '\x01') &&
     ((((puVar4 != DAT_08011d40 && (puVar4 != DAT_08011d40 + 0x5ffbf00)) ||
       (param_1[1] != (uint *)0x1)) || (param_1[0x11] != (uint *)0x0)))) {
    return 1;
  }
  puVar10 = DAT_08011d4c;
  if ((puVar4 != DAT_08011d40) && (puVar4 != DAT_08011d40 + 8)) {
    if ((puVar4 == DAT_08011d40 + 0x100) || (puVar4 == DAT_08011d40 + 0x108)) {
      cVar1 = *(char *)((int)param_1 + 0x91);
      puVar10 = DAT_08011d78;
      goto joined_r0x08011aee;
    }
    puVar10 = DAT_08011e18;
    if (((puVar4 != DAT_08011d40 + 0x200) && (puVar4 != DAT_08011d40 + 0x208)) &&
       ((puVar10 = DAT_08011d48, puVar4 != DAT_08011d44 && (puVar4 != DAT_08011d44 + 8)))) {
      return 1;
    }
  }
  cVar1 = *(char *)((int)param_1 + 0x91);
joined_r0x08011aee:
  if (cVar1 == '\0') {
    *(undefined *)(param_1 + 0x24) = 0;
    FUN_080090c0(param_1);
    puVar4 = *param_1;
  }
  iVar8 = (uint)((ulonglong)DAT_08011d54 * (ulonglong)*DAT_08011d50 >> 0x2c) << 2;
  *puVar4 = *puVar4 & 0xfffeffff;
  do {
    if (iVar8 == 0) {
      param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
      return 1;
    }
    iVar8 = iVar8 + -1;
  } while ((*puVar4 & 0x10000) != 0);
  *(undefined *)((int)param_1 + 0x91) = 2;
  if (param_1[3] == (uint *)0x1) {
    uVar5 = 0x10;
  }
  else if (param_1[3] == (uint *)0x2) {
    uVar5 = 0x20;
  }
  else {
    uVar5 = 0;
  }
  switch(param_1[2]) {
  case (uint *)0x1:
    uVar11 = 0x400;
    break;
  case (uint *)0x3:
    uVar5 = uVar5 | 1;
    uVar11 = 0x800;
    break;
  case (uint *)0x4:
    uVar5 = uVar5 | 2;
  case (uint *)0x2:
    uVar11 = 0x800;
    break;
  case (uint *)0x5:
    uVar5 = uVar5 | 3;
    uVar11 = 0x800;
    break;
  default:
    uVar11 = 0;
  }
  puVar9 = param_1[8];
  *puVar10 = uVar5;
  if (puVar9 != (uint *)0x0) {
    if ((puVar4 == DAT_08011d40) || (uVar2 = (uint)(puVar4 == DAT_08011d58), uVar2 != 0)) {
      uVar2 = FUN_080116d8(0x100,0,uVar5,DAT_08011d40,param_4);
      puVar4 = *param_1;
    }
    if ((puVar4 == DAT_08011d5c) || (puVar4 == DAT_08011d5c + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar4 = *param_1;
    }
    if ((puVar4 == DAT_08011d60) || (puVar4 == DAT_08011d60 + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar4 = *param_1;
    }
    if (puVar4 == DAT_08011d44) {
      uVar2 = FUN_080116d8(0x400,0);
      puVar4 = *param_1;
    }
    if (puVar4 == DAT_08011d64) {
      uVar2 = FUN_080116d8(0x800,0);
    }
    if (param_1[6] == (uint *)0x80000) {
      puVar4 = param_1[0x11];
      if (puVar4 == (uint *)&UndefinedInstruction) {
        puVar9 = (uint *)0x40;
      }
      else if (puVar4 == (uint *)&SupervisorCall) {
        puVar9 = (uint *)0x100;
      }
      else {
        puVar9 = param_1[0x15];
      }
      uVar5 = (uVar2 * 10) / (uint)((int)param_1[8] * (int)puVar9);
    }
    else {
      puVar4 = param_1[0x11];
      if (param_1[10] == (uint *)0x4000000) {
        iVar8 = 2;
      }
      else {
        iVar8 = 1;
      }
      uVar5 = (uVar2 * 10) / (uint)((int)param_1[8] * iVar8 * 0x100);
    }
    puVar9 = (uint *)((ulonglong)DAT_08011d68 * (ulonglong)uVar5 >> 0x23);
    if (uVar5 + (int)puVar9 * -10 == 9) {
      puVar9 = (uint *)((int)puVar9 + 1);
    }
    param_1[9] = puVar9;
    if (puVar4 == (uint *)&UndefinedInstruction) {
      param_1[9] = (uint *)((uint)param_1[9] >> 1);
    }
  }
  if (((uint)param_1[1] & 0xfffffffd) == 0) {
    if (param_1[0x14] == (uint *)0x1) {
      uVar5 = 0;
    }
    else {
      uVar5 = 0x200;
    }
  }
  else {
    uVar5 = 0;
    if (param_1[0x14] == (uint *)0x1) {
      uVar5 = 0x200;
    }
  }
  uVar2 = FUN_08009d18();
  puVar4 = *param_1;
  uVar6 = (uint)param_1[1] | (uint)param_1[0x11] | (uint)param_1[0x12] | (uint)param_1[0x13] |
          (uint)param_1[0xb] | (uint)param_1[5];
  if (uVar2 < 0x2000) {
    uVar6 = uVar6 | (uint)param_1[6];
    *puVar4 = DAT_08011e14 & *puVar4;
    puVar9 = param_1[10];
  }
  else {
    uVar6 = uVar6 | (uint)param_1[6] | (uint)param_1[10];
    *puVar4 = DAT_08011d6c & *puVar4;
    puVar9 = param_1[4];
  }
  uVar2 = DAT_08011d70;
  *puVar4 = uVar6 | (uint)puVar9 | *puVar4 | (int)param_1[9] << 0x14 | uVar11 | uVar5;
  puVar9 = param_1[0xc];
  puVar7 = param_1[7];
  puVar3 = param_1[0xd];
  puVar4[1] = uVar2 & puVar4[1];
  uVar5 = DAT_08011d74;
  puVar4[1] = (uint)puVar7 | (uint)puVar9 | (uint)puVar3 | puVar4[1];
  puVar9 = param_1[0x19];
  puVar4[2] = uVar5 & puVar4[2];
  puVar4[2] = (uint)puVar9 | (uint)param_1[0x17] | (uint)param_1[0x18] | (int)param_1[0x15] - 1U |
              ((int)param_1[0x16] + -1) * 0x100 | puVar4[2];
  puVar9 = param_1[0x1a];
  puVar4[3] = puVar4[3] & 0xf020;
  bVar12 = puVar4 == DAT_08011d40;
  puVar4[3] = (uint)puVar9 | (uint)param_1[0x1b] | (int)param_1[0x1d] << 0x10 |
              ((int)param_1[0x1c] + -1) * 0x100 | puVar4[3];
  if (((bVar12) || (puVar4 == DAT_08011d44)) &&
     (puVar10[0x11] = puVar10[0x11] & 0xfffffffe, *(char *)(param_1 + 0xe) == '\x01')) {
    puVar10[0x11] = (uint)param_1[0x10] | ((int)param_1[0xf] + -1) * 0x10;
    puVar10[0x11] = puVar10[0x11] | 1;
  }
  param_1[0x25] = (uint *)0x0;
  *(undefined *)(param_1 + 0x24) = 0;
  *(undefined *)((int)param_1 + 0x91) = 1;
  return 0;
}



/* === 08011e1c HAL_SAI_InitProtocol === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 HAL_SAI_InitProtocol(uint **param_1,uint param_2,undefined4 param_3,uint *param_4)

{
  char cVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  int iVar7;
  uint *puVar8;
  undefined4 unaff_r4;
  uint *puVar9;
  undefined4 unaff_r5;
  uint *puVar10;
  uint uVar11;
  bool bVar12;
  
  if (param_2 < 3) {
    param_1[0x1c] = param_4;
    param_1[0x11] = (uint *)0x0;
    param_1[0x13] = (uint *)0x0;
    param_1[0x1a] = (uint *)0x0;
    param_1[0x14] = (uint *)(uint)(((uint)param_1[1] & 0xfffffffd) != 0);
    param_1[0x17] = (uint *)0x10000;
    param_1[0x1d] = (uint *)0xffff;
    if (((uint)param_4 & 1) != 0) {
      return 1;
    }
    if (param_2 != 0) {
      param_1[0x19] = (uint *)0x0;
      param_1[0x18] = (uint *)0x20000;
      switch(param_3) {
      case 0:
        goto switchD_08011eac_caseD_0;
      case 1:
        goto switchD_08011eac_caseD_1;
      case 2:
        goto switchD_08011eac_caseD_2;
      case 3:
        goto switchD_08011eac_caseD_3;
      default:
        goto switchD_08011e66_caseD_4;
      }
    }
    param_1[0x18] = (uint *)0x0;
    param_1[0x19] = (uint *)0x40000;
    switch(param_3) {
    case 0:
switchD_08011eac_caseD_0:
      param_1[0x12] = (uint *)0x80;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 4);
      param_1[0x1b] = (uint *)0x40;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x16] = puVar9;
      break;
    case 1:
switchD_08011eac_caseD_1:
      param_1[0x12] = (uint *)0x80;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      if (param_2 == 2) {
        puVar9 = (uint *)&DataAbort;
        param_1[0x1a] = (uint *)&DataAbort;
      }
      break;
    case 2:
switchD_08011eac_caseD_2:
      param_1[0x12] = (uint *)0xc0;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      if (param_2 == 2) {
        puVar9 = (uint *)&SupervisorCall;
        param_1[0x1a] = (uint *)&SupervisorCall;
      }
      break;
    case 3:
switchD_08011eac_caseD_3:
      param_1[0x12] = (uint *)0xe0;
      puVar9 = (uint *)(((uint)param_4 >> 1) << 5);
      param_1[0x1b] = (uint *)0x80;
      param_1[0x15] = (uint *)(((uint)param_4 >> 1) << 6);
      param_1[0x16] = puVar9;
      break;
    default:
      goto switchD_08011e66_caseD_4;
    }
    goto HAL_SAI_Init;
  }
  if (1 < param_2 - 3) {
switchD_08011e66_caseD_4:
    return 1;
  }
  param_1[0x1c] = param_4;
  param_1[0x11] = (uint *)0x0;
  param_1[0x13] = (uint *)0x0;
  param_1[0x17] = (uint *)0x0;
  param_1[0x1a] = (uint *)0x0;
  param_1[0x1d] = (uint *)0xffff;
  param_1[0x14] = (uint *)(uint)(((uint)param_1[1] & 0xfffffffd) == 0);
  if (param_2 == 4) {
    puVar9 = (uint *)0x1;
  }
  else {
    puVar9 = (uint *)0xd;
  }
  param_1[0x18] = (uint *)0x20000;
  param_1[0x16] = puVar9;
  param_1[0x19] = (uint *)0x40000;
  switch(param_3) {
  case 0:
    puVar9 = (uint *)((int)param_4 << 4);
    param_1[0x12] = (uint *)0x80;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x40;
    break;
  case 1:
    puVar9 = (uint *)((int)param_4 << 5);
    param_1[0x12] = (uint *)0x80;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x80;
    break;
  case 2:
    puVar3 = (uint *)0xc0;
    goto LAB_08011ee8;
  case 3:
    puVar3 = (uint *)0xe0;
LAB_08011ee8:
    puVar9 = (uint *)((int)param_4 << 5);
    param_1[0x12] = puVar3;
    param_1[0x15] = puVar9;
    param_1[0x1b] = (uint *)0x80;
    break;
  default:
    goto switchD_08011e66_caseD_4;
  }
HAL_SAI_Init:
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  FUN_08009d18();
  puVar3 = *param_1;
  if ((*(char *)(param_1 + 0xe) == '\x01') &&
     ((((puVar3 != DAT_08011d40 && (puVar3 != DAT_08011d40 + 0x5ffbf00)) ||
       (param_1[1] != (uint *)0x1)) || (param_1[0x11] != (uint *)0x0)))) {
    return 1;
  }
  puVar10 = DAT_08011d4c;
  if ((puVar3 != DAT_08011d40) && (puVar3 != DAT_08011d40 + 8)) {
    if ((puVar3 == DAT_08011d40 + 0x100) || (puVar3 == DAT_08011d40 + 0x108)) {
      cVar1 = *(char *)((int)param_1 + 0x91);
      puVar10 = DAT_08011d78;
      goto joined_r0x08011aee;
    }
    puVar10 = DAT_08011e18;
    if (((puVar3 != DAT_08011d40 + 0x200) && (puVar3 != DAT_08011d40 + 0x208)) &&
       ((puVar10 = DAT_08011d48, puVar3 != DAT_08011d44 && (puVar3 != DAT_08011d44 + 8)))) {
      return 1;
    }
  }
  cVar1 = *(char *)((int)param_1 + 0x91);
joined_r0x08011aee:
  if (cVar1 == '\0') {
    *(undefined *)(param_1 + 0x24) = 0;
    FUN_080090c0(param_1);
    puVar3 = *param_1;
  }
  iVar7 = (uint)((ulonglong)DAT_08011d54 * (ulonglong)*DAT_08011d50 >> 0x2c) << 2;
  *puVar3 = *puVar3 & 0xfffeffff;
  do {
    if (iVar7 == 0) {
      param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
      return 1;
    }
    iVar7 = iVar7 + -1;
  } while ((*puVar3 & 0x10000) != 0);
  *(undefined *)((int)param_1 + 0x91) = 2;
  if (param_1[3] == (uint *)0x1) {
    uVar4 = 0x10;
  }
  else if (param_1[3] == (uint *)0x2) {
    uVar4 = 0x20;
  }
  else {
    uVar4 = 0;
  }
  switch(param_1[2]) {
  case (uint *)0x1:
    uVar11 = 0x400;
    break;
  case (uint *)0x3:
    uVar4 = uVar4 | 1;
    uVar11 = 0x800;
    break;
  case (uint *)0x4:
    uVar4 = uVar4 | 2;
  case (uint *)0x2:
    uVar11 = 0x800;
    break;
  case (uint *)0x5:
    uVar4 = uVar4 | 3;
    uVar11 = 0x800;
    break;
  default:
    uVar11 = 0;
  }
  puVar8 = param_1[8];
  *puVar10 = uVar4;
  if (puVar8 != (uint *)0x0) {
    if ((puVar3 == DAT_08011d40) || (uVar2 = (uint)(puVar3 == DAT_08011d58), uVar2 != 0)) {
      uVar2 = FUN_080116d8(0x100,0,uVar4,DAT_08011d40,puVar9,unaff_r4,unaff_r5);
      puVar3 = *param_1;
    }
    if ((puVar3 == DAT_08011d5c) || (puVar3 == DAT_08011d5c + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar3 = *param_1;
    }
    if ((puVar3 == DAT_08011d60) || (puVar3 == DAT_08011d60 + 8)) {
      uVar2 = FUN_080116d8(0x200,0);
      puVar3 = *param_1;
    }
    if (puVar3 == DAT_08011d44) {
      uVar2 = FUN_080116d8(0x400,0);
      puVar3 = *param_1;
    }
    if (puVar3 == DAT_08011d64) {
      uVar2 = FUN_080116d8(0x800,0);
    }
    if (param_1[6] == (uint *)0x80000) {
      puVar9 = param_1[0x11];
      if (puVar9 == (uint *)&UndefinedInstruction) {
        puVar3 = (uint *)0x40;
      }
      else if (puVar9 == (uint *)&SupervisorCall) {
        puVar3 = (uint *)0x100;
      }
      else {
        puVar3 = param_1[0x15];
      }
      uVar4 = (uVar2 * 10) / (uint)((int)param_1[8] * (int)puVar3);
    }
    else {
      puVar9 = param_1[0x11];
      if (param_1[10] == (uint *)0x4000000) {
        iVar7 = 2;
      }
      else {
        iVar7 = 1;
      }
      uVar4 = (uVar2 * 10) / (uint)((int)param_1[8] * iVar7 * 0x100);
    }
    puVar3 = (uint *)((ulonglong)DAT_08011d68 * (ulonglong)uVar4 >> 0x23);
    if (uVar4 + (int)puVar3 * -10 == 9) {
      puVar3 = (uint *)((int)puVar3 + 1);
    }
    param_1[9] = puVar3;
    if (puVar9 == (uint *)&UndefinedInstruction) {
      param_1[9] = (uint *)((uint)param_1[9] >> 1);
    }
  }
  if (((uint)param_1[1] & 0xfffffffd) == 0) {
    if (param_1[0x14] == (uint *)0x1) {
      uVar4 = 0;
    }
    else {
      uVar4 = 0x200;
    }
  }
  else {
    uVar4 = 0;
    if (param_1[0x14] == (uint *)0x1) {
      uVar4 = 0x200;
    }
  }
  uVar2 = FUN_08009d18();
  puVar9 = *param_1;
  uVar5 = (uint)param_1[1] | (uint)param_1[0x11] | (uint)param_1[0x12] | (uint)param_1[0x13] |
          (uint)param_1[0xb] | (uint)param_1[5];
  if (uVar2 < 0x2000) {
    uVar5 = uVar5 | (uint)param_1[6];
    *puVar9 = DAT_08011e14 & *puVar9;
    puVar3 = param_1[10];
  }
  else {
    uVar5 = uVar5 | (uint)param_1[6] | (uint)param_1[10];
    *puVar9 = DAT_08011d6c & *puVar9;
    puVar3 = param_1[4];
  }
  uVar2 = DAT_08011d70;
  *puVar9 = uVar5 | (uint)puVar3 | *puVar9 | (int)param_1[9] << 0x14 | uVar11 | uVar4;
  puVar3 = param_1[0xc];
  puVar6 = param_1[7];
  puVar8 = param_1[0xd];
  puVar9[1] = uVar2 & puVar9[1];
  uVar4 = DAT_08011d74;
  puVar9[1] = (uint)puVar6 | (uint)puVar3 | (uint)puVar8 | puVar9[1];
  puVar3 = param_1[0x19];
  puVar9[2] = uVar4 & puVar9[2];
  puVar9[2] = (uint)puVar3 | (uint)param_1[0x17] | (uint)param_1[0x18] | (int)param_1[0x15] - 1U |
              ((int)param_1[0x16] + -1) * 0x100 | puVar9[2];
  puVar3 = param_1[0x1a];
  puVar9[3] = puVar9[3] & 0xf020;
  bVar12 = puVar9 == DAT_08011d40;
  puVar9[3] = (uint)puVar3 | (uint)param_1[0x1b] | (int)param_1[0x1d] << 0x10 |
              ((int)param_1[0x1c] + -1) * 0x100 | puVar9[3];
  if (((bVar12) || (puVar9 == DAT_08011d44)) &&
     (puVar10[0x11] = puVar10[0x11] & 0xfffffffe, *(char *)(param_1 + 0xe) == '\x01')) {
    puVar10[0x11] = (uint)param_1[0x10] | ((int)param_1[0xf] + -1) * 0x10;
    puVar10[0x11] = puVar10[0x11] | 1;
  }
  param_1[0x25] = (uint *)0x0;
  *(undefined *)(param_1 + 0x24) = 0;
  *(undefined *)((int)param_1 + 0x91) = 1;
  return 0;
}



/* === 08011f64 FUN_08011f64 === */

uint FUN_08011f64(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  uint *puVar7;
  
  iVar2 = FUN_08009ce8();
  if (param_2 != (uint *)0x0) {
    puVar7 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar6 = (uint)*(byte *)((int)param_1 + 0x91);
      if ((*(byte *)((int)param_1 + 0x91) == 1) && (*(char *)(param_1 + 0x24) != '\x01')) {
        *(short *)(param_1 + 0x1f) = (short)param_3;
        *(short *)((int)param_1 + 0x7e) = (short)param_3;
        param_1[0x25] = puVar7;
        *(undefined *)((int)param_1 + 0x91) = 0x12;
        uVar1 = DAT_0801207c;
        puVar3 = param_1[0x20];
        puVar5 = *param_1;
        *(undefined *)(param_1 + 0x24) = 1;
        param_1[0x1e] = param_2;
        puVar3[0x10] = uVar1;
        uVar1 = DAT_08012080;
        puVar3[0x14] = (uint)puVar7;
        puVar3[0xf] = uVar1;
        puVar3[0x13] = DAT_08012084;
        iVar4 = FUN_0800b3a0(puVar3,param_2,puVar5 + 7,param_3,param_4);
        if (iVar4 != 0) {
          *(bool *)(param_1 + 0x24) = param_3 == 0;
          return uVar6;
        }
        if (param_1[0x11] == (uint *)&SupervisorCall) {
          if (((uint)param_1[1] & 0xfffffffd) == 1) {
            uVar6 = 0x11;
          }
          else {
            uVar6 = 1;
          }
        }
        puVar7 = *param_1;
        if ((int)param_1[1] - 2U < 2) {
          uVar6 = uVar6 | 0x60;
        }
        else {
          uVar6 = uVar6 | 4;
        }
        puVar7[4] = puVar7[4] | uVar6;
        *puVar7 = *puVar7 | 0x20000;
        while( true ) {
          if ((puVar7[5] & 0x70000) != 0) {
            if (-1 < (int)(*puVar7 << 0xf)) {
              *puVar7 = *puVar7 | 0x10000;
            }
            *(undefined *)(param_1 + 0x24) = 0;
            return 0;
          }
          iVar4 = FUN_08009ce8();
          if (1000 < (uint)(iVar4 - iVar2)) break;
          puVar7 = *param_1;
        }
        *(undefined *)(param_1 + 0x24) = 0;
        param_1[0x25] = (uint *)((uint)param_1[0x25] | 0x40);
        return 3;
      }
      return 2;
    }
  }
  return 1;
}



/* === 08012088 FUN_08012088 === */

uint FUN_08012088(uint **param_1,uint *param_2,int param_3,undefined4 param_4)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  
  if (param_2 != (uint *)0x0) {
    puVar6 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar5 = (uint)*(byte *)((int)param_1 + 0x91);
      if ((*(byte *)((int)param_1 + 0x91) == 1) && (*(char *)(param_1 + 0x24) != '\x01')) {
        *(undefined *)(param_1 + 0x24) = 1;
        param_1[0x1e] = param_2;
        *(short *)(param_1 + 0x1f) = (short)param_3;
        *(short *)((int)param_1 + 0x7e) = (short)param_3;
        param_1[0x25] = puVar6;
        puVar2 = param_1[0x21];
        *(undefined *)((int)param_1 + 0x91) = 0x22;
        puVar4 = *param_1;
        puVar2[0x10] = DAT_08012144;
        uVar1 = DAT_08012148;
        puVar2[0x14] = (uint)puVar6;
        puVar2[0xf] = uVar1;
        puVar2[0x13] = DAT_0801214c;
        iVar3 = FUN_0800b3a0(puVar2,puVar4 + 7,param_2,param_3,param_4);
        if (iVar3 != 0) {
          *(bool *)(param_1 + 0x24) = param_3 == 0;
          return uVar5;
        }
        if (param_1[0x11] == (uint *)&SupervisorCall) {
          if (((uint)param_1[1] & 0xfffffffd) == 1) {
            uVar5 = 0x11;
          }
          else {
            uVar5 = 1;
          }
        }
        puVar6 = *param_1;
        if ((int)param_1[1] - 2U < 2) {
          uVar5 = uVar5 | 0x60;
        }
        else {
          uVar5 = uVar5 | 4;
        }
        puVar6[4] = puVar6[4] | uVar5;
        *puVar6 = *puVar6 | 0x20000;
        if (-1 < (int)(*puVar6 << 0xf)) {
          *puVar6 = *puVar6 | 0x10000;
        }
        *(undefined *)(param_1 + 0x24) = 0;
        return 0;
      }
      return 2;
    }
  }
  return 1;
}



/* === 08012150 FUN_08012150 === */

void FUN_08012150(void)

{
  return;
}



/* === 080121b0 FUN_080121b0 === */

void FUN_080121b0(void)

{
  return;
}



/* === 08012228 FUN_08012228 === */

void FUN_08012228(void)

{
  return;
}



/* === 0801229c FUN_0801229c === */

undefined4 FUN_0801229c(undefined4 *param_1,undefined4 param_2)

{
  if (param_1 != (undefined4 *)0x0) {
    if (*(char *)(param_1 + 0xb) == '\0') {
      *(undefined *)((int)param_1 + 0x2d) = 0;
      FUN_0800627c();
    }
    *(undefined *)(param_1 + 0xb) = 2;
    FUN_08012360(*param_1,param_1 + 1);
    FUN_080123dc(*param_1,param_2,param_1[1]);
    *DAT_080122ec = *DAT_080122ec | 0x80000000;
    *(undefined *)(param_1 + 0xb) = 1;
    return 0;
  }
  return 1;
}



/* === 080122f0 FUN_080122f0 === */

byte FUN_080122f0(undefined4 *param_1,int *param_2)

{
  byte bVar1;
  
  bVar1 = *(byte *)(param_1 + 0xb);
  if (bVar1 != 2) {
    if ((bVar1 & 0xfb) == 1) {
      *(undefined *)(param_1 + 0xb) = 2;
      FUN_08012474(*param_1);
      if (*param_2 != 2) {
        *(undefined *)(param_1 + 0xb) = 1;
        return 0;
      }
      *(undefined *)(param_1 + 0xb) = 5;
      return 0;
    }
    bVar1 = 1;
  }
  return bVar1;
}



/* === 08012330 FUN_08012330 === */

char FUN_08012330(undefined4 *param_1)

{
  char cVar1;
  
  cVar1 = *(char *)(param_1 + 0xb);
  if (cVar1 != '\x02') {
    if (*(char *)(param_1 + 0xb) == '\x01') {
      *(undefined *)(param_1 + 0xb) = 2;
      FUN_080124a4(*param_1);
      *(undefined *)(param_1 + 0xb) = 1;
      return '\0';
    }
    cVar1 = '\x01';
  }
  return cVar1;
}



/* === 08012360 FUN_08012360 === */

undefined4 FUN_08012360(uint *param_1,int *param_2)

{
  uint uVar1;
  
  uVar1 = DAT_080123d8;
  if (*param_2 == 0) {
    *param_1 = *param_1 & 0xffff8000 | param_2[1] | param_2[2] | param_2[3] | param_2[4] |
               param_2[5] | param_2[6] | param_2[7] | param_2[8] | param_2[9];
    return 0;
  }
  *param_1 = *param_1 & 0xffff83ff | param_2[7] | param_2[8] | param_2[9];
  param_1[1] = uVar1 & param_1[1] | param_2[1] | param_2[2] | param_2[3] | param_2[4] | param_2[5] |
               param_2[6];
  return 0;
}



/* === 080123dc FUN_080123dc === */

undefined4 FUN_080123dc(int param_1,int *param_2,int param_3)

{
  int iVar1;
  int iVar2;
  
  if (param_3 == 0) {
    *(uint *)(param_1 + 8) =
         *(uint *)(param_1 + 8) & 0xf0000000 | (param_2[1] + -1) * 0x10 | *param_2 - 1U |
         (param_2[2] + -1) * 0x100 | (param_2[3] + -1) * 0x1000 | (param_2[4] + -1) * 0x10000 |
         (param_2[5] + -1) * 0x100000 | (param_2[6] + -1) * 0x1000000;
    return 0;
  }
  iVar1 = *param_2;
  iVar2 = param_2[1];
  *(uint *)(param_1 + 8) =
       DAT_08012470 & *(uint *)(param_1 + 8) | (param_2[3] + -1) * 0x1000 |
       (param_2[5] + -1) * 0x100000;
  *(uint *)(param_1 + 0xc) =
       iVar1 - 1U | *(uint *)(param_1 + 0xc) & 0xf0000000 | (iVar2 + -1) * 0x10 |
       (param_2[2] + -1) * 0x100 | (param_2[4] + -1) * 0x10000 | (param_2[6] + -1) * 0x1000000;
  return 0;
}



/* === 08012474 FUN_08012474 === */

undefined4 FUN_08012474(int param_1,uint *param_2)

{
  *(uint *)(param_1 + 0x10) =
       *param_2 | param_2[1] | param_2[3] << 9 | DAT_080124a0 & *(uint *)(param_1 + 0x10) |
       (param_2[2] - 1) * 0x20;
  return 0;
}



/* === 080124a4 FUN_080124a4 === */

undefined4 FUN_080124a4(int param_1,int param_2)

{
  *(uint *)(param_1 + 0x14) = DAT_080124bc & *(uint *)(param_1 + 0x14) | param_2 << 1;
  return 0;
}



/* === 080124c0 FUN_080124c0 === */

undefined4 FUN_080124c0(int param_1,int param_2,int param_3)

{
  uint uVar1;
  
  if (param_3 == 2) {
    if ((uint)(DAT_0801256c + param_2) <= DAT_08012570) {
      uVar1 = 0x3c00;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012574 + param_2) <= DAT_08012578) {
      uVar1 = 0x3800;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_0801257c + param_2) <= DAT_08012580) {
      uVar1 = 0x3400;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012584 + param_2) < DAT_08012588) {
      uVar1 = 0x3000;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_0801258c + param_2) <= DAT_08012590) {
      uVar1 = 0x2c00;
      goto LAB_080124ca;
    }
    if ((uint)(DAT_08012594 + param_2) < DAT_08012598) {
      uVar1 = 0x2800;
      goto LAB_080124ca;
    }
    if (DAT_080125a0 <= (uint)(DAT_0801259c + param_2)) {
      if ((uint)(DAT_080125a4 + param_2) < DAT_080125a8) {
        uVar1 = 0x2000;
      }
      else if ((uint)(DAT_080125ac + param_2) < DAT_080125b0) {
        uVar1 = 0x1c00;
      }
      else {
        uVar1 = 0x1800;
      }
      goto LAB_080124ca;
    }
  }
  uVar1 = 0x2400;
LAB_080124ca:
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) & 0xffffc3ff;
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | uVar1;
  return 0;
}



/* === 080125b4 FUN_080125b4 === */

undefined4 FUN_080125b4(int param_1,int param_2)

{
  uint uVar1;
  uint local_4;
  
  uVar1 = DAT_08012604;
  local_4 = 0;
  do {
    local_4 = local_4 + 1;
    if (DAT_08012604 < local_4) {
      return 3;
    }
  } while (-1 < *(int *)(param_1 + 0x10));
  local_4 = 0;
  *(uint *)(param_1 + 0x10) = param_2 << 6 | 0x20;
  do {
    local_4 = local_4 + 1;
    if (uVar1 < local_4) {
      return 3;
    }
  } while ((*(uint *)(param_1 + 0x10) & 0x20) != 0);
  return 0;
}



/* === 08012608 FUN_08012608 === */

undefined4 FUN_08012608(int param_1)

{
  uint uVar1;
  uint local_4;
  
  uVar1 = DAT_08012654;
  local_4 = 0;
  do {
    local_4 = local_4 + 1;
    if (DAT_08012654 < local_4) {
      return 3;
    }
  } while (-1 < *(int *)(param_1 + 0x10));
  local_4 = 0;
  *(undefined4 *)(param_1 + 0x10) = 0x10;
  do {
    local_4 = local_4 + 1;
    if (uVar1 < local_4) {
      return 3;
    }
  } while ((*(uint *)(param_1 + 0x10) & 0x10) != 0);
  return 0;
}



/* === 08012658 FUN_08012658 === */

uint FUN_08012658(int param_1)

{
  uint uVar1;
  
  uVar1 = *(uint *)(param_1 + 0x808) & 6;
  if (uVar1 != 0) {
    if ((*(uint *)(param_1 + 0x808) & 2) == 0) {
      uVar1 = 0xf;
    }
    else {
      uVar1 = 2;
    }
  }
  return uVar1;
}



/* === 08012670 FUN_08012670 === */

undefined4 FUN_08012670(int param_1,byte *param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = (uint)*param_2;
  if (param_2[1] == 1) {
    iVar2 = param_1 + uVar1 * 0x20;
    *(uint *)(param_1 + 0x81c) = 1 << (uVar1 & 0xf) | *(uint *)(param_1 + 0x81c);
    if (-1 < *(int *)(iVar2 + 0x900) << 0x10) {
      *(uint *)(iVar2 + 0x900) =
           DAT_080126fc |
           *(uint *)(param_2 + 8) & 0x7ff | *(uint *)(iVar2 + 0x900) | (uint)param_2[4] << 0x12 |
           uVar1 << 0x16;
      return 0;
    }
  }
  else {
    iVar2 = param_1 + uVar1 * 0x20;
    *(uint *)(param_1 + 0x81c) = 0x10000 << (uVar1 & 0xf) | *(uint *)(param_1 + 0x81c);
    if (-1 < *(int *)(iVar2 + 0xb00) << 0x10) {
      *(uint *)(iVar2 + 0xb00) =
           DAT_080126fc |
           *(uint *)(param_2 + 8) & 0x7ff | *(uint *)(iVar2 + 0xb00) | (uint)param_2[4] << 0x12;
    }
  }
  return 0;
}



/* === 08012700 FUN_08012700 === */

undefined4 FUN_08012700(int param_1,byte *param_2,int param_3)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  byte bVar12;
  undefined4 *puVar7;
  
  uVar11 = DAT_08012978;
  uVar9 = DAT_08012974;
  uVar8 = (uint)*param_2;
  if (param_2[1] != 1) {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0xb10) = DAT_08012974 & *(uint *)(iVar3 + 0xb10);
    *(uint *)(iVar3 + 0xb10) = uVar11 & *(uint *)(iVar3 + 0xb10);
    if (uVar8 == 0) {
      if (*(int *)(param_2 + 0x10) == 0) {
        uVar9 = *(uint *)(param_2 + 8);
      }
      else {
        uVar9 = *(uint *)(param_2 + 8);
        *(uint *)(param_2 + 0x10) = uVar9;
      }
      *(uint *)(param_2 + 0x20) = uVar9;
    }
    else {
      if (*(int *)(param_2 + 0x10) != 0) {
        uVar11 = *(uint *)(param_2 + 8);
        uVar9 = ((*(int *)(param_2 + 0x10) + uVar11) - 1) / uVar11 & 0xffff;
        uVar11 = uVar9 * uVar11;
        uVar9 = DAT_0801297c & uVar9 << 0x13;
        uVar8 = *(uint *)(iVar3 + 0xb10);
        *(uint *)(param_2 + 0x20) = uVar11;
        *(uint *)(iVar3 + 0xb10) = uVar9 | uVar8;
        *(uint *)(iVar3 + 0xb10) = uVar11 & 0x7ffff | *(uint *)(iVar3 + 0xb10);
        goto joined_r0x0801289c;
      }
      uVar9 = *(uint *)(param_2 + 8);
    }
    *(uint *)(iVar3 + 0xb10) = uVar9 & 0x7ffff | *(uint *)(iVar3 + 0xb10);
    *(uint *)(iVar3 + 0xb10) = *(uint *)(iVar3 + 0xb10) | 0x80000;
joined_r0x0801289c:
    if ((param_3 == 1) && (*(int *)(param_2 + 0xc) != 0)) {
      *(int *)(iVar3 + 0xb14) = *(int *)(param_2 + 0xc);
    }
    if (param_2[4] == 1) {
      if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
        uVar9 = *(uint *)(iVar3 + 0xb00) | 0x20000000;
      }
      else {
        uVar9 = *(uint *)(iVar3 + 0xb00) | 0x10000000;
      }
      *(uint *)(iVar3 + 0xb00) = uVar9;
    }
    *(uint *)(iVar3 + 0xb00) = *(uint *)(iVar3 + 0xb00) | 0x84000000;
    return 0;
  }
  uVar10 = *(uint *)(param_2 + 0x10);
  if (uVar10 == 0) {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0x910) = DAT_08012978 & *(uint *)(iVar3 + 0x910);
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x80000;
    *(uint *)(iVar3 + 0x910) = uVar9 & *(uint *)(iVar3 + 0x910);
    puVar4 = (uint *)(iVar3 + 0x900);
    bVar12 = param_2[4];
    if (param_3 != 1) {
      *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
      if (bVar12 != 1) {
        return 0;
      }
LAB_080128f0:
      if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
        uVar9 = *puVar4 | 0x20000000;
      }
      else {
        uVar9 = *puVar4 | 0x10000000;
      }
      *puVar4 = uVar9;
      if ((param_3 == 0) && (uVar1 = *(ushort *)(param_2 + 0x10), uVar1 + 3 >> 2 != 0)) {
        puVar5 = *(undefined4 **)(param_2 + 0xc);
        puVar6 = puVar5;
        do {
          puVar7 = puVar6 + 1;
          *(undefined4 *)(param_1 + uVar8 * 0x1000 + 0x1000) = *puVar6;
          puVar6 = puVar7;
        } while (puVar7 != (undefined4 *)((int)puVar5 + (uVar1 + 3 & 0xfffffffc)));
      }
      return 0;
    }
    uVar9 = *(uint *)(param_2 + 0x1c);
    if (uVar9 != 0) goto LAB_0801294c;
LAB_08012934:
    if (bVar12 != 1) goto LAB_08012938;
  }
  else {
    iVar3 = param_1 + uVar8 * 0x20;
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) & DAT_08012974;
    *(uint *)(iVar3 + 0x910) = uVar11 & *(uint *)(iVar3 + 0x910);
    puVar4 = (uint *)(iVar3 + 0x900);
    if (uVar8 == 0) {
      uVar9 = *(uint *)(param_2 + 8);
      if (uVar9 < uVar10) {
        *(uint *)(param_2 + 0x10) = uVar9;
        uVar10 = uVar9;
      }
      *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x80000;
    }
    else {
      *(uint *)(iVar3 + 0x910) =
           DAT_0801297c & ((uVar10 + *(uint *)(param_2 + 8)) - 1) / *(uint *)(param_2 + 8) << 0x13 |
           *(uint *)(iVar3 + 0x910);
    }
    *(uint *)(iVar3 + 0x910) = uVar10 & 0x7ffff | *(uint *)(iVar3 + 0x910);
    bVar12 = param_2[4];
    if (bVar12 != 1) {
      if (param_3 != 1) {
        iVar2 = *(int *)(param_2 + 0x10);
        *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
        if (iVar2 == 0) {
          return 0;
        }
        *(uint *)(param_1 + 0x834) = 1 << (uVar8 & 0xf) | *(uint *)(param_1 + 0x834);
        return 0;
      }
      uVar9 = *(uint *)(param_2 + 0x1c);
      if (uVar9 == 0) goto LAB_08012938;
LAB_0801294c:
      puVar4[5] = uVar9;
      goto LAB_08012934;
    }
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) & 0x9fffffff;
    *(uint *)(iVar3 + 0x910) = *(uint *)(iVar3 + 0x910) | 0x20000000;
    if (param_3 != 1) {
      *(uint *)(iVar3 + 0x900) = *(uint *)(iVar3 + 0x900) | 0x84000000;
      goto LAB_080128f0;
    }
    uVar9 = *(uint *)(param_2 + 0x1c);
    if (uVar9 != 0) goto LAB_0801294c;
  }
  if ((*(uint *)(param_1 + 0x808) & 0x100) == 0) {
    uVar9 = *puVar4 | 0x20000000;
  }
  else {
    uVar9 = *puVar4 | 0x10000000;
  }
  *puVar4 = uVar9;
LAB_08012938:
  *puVar4 = *puVar4 | 0x84000000;
  return 0;
}



/* === 08012980 FUN_08012980 === */

undefined4 FUN_08012980(int param_1,byte *param_2)

{
  uint *puVar1;
  uint local_4;
  
  local_4 = 0;
  if (param_2[1] == 1) {
    puVar1 = (uint *)(param_1 + 0x900 + (uint)*param_2 * 0x20);
    if ((int)*puVar1 < 0) {
      *puVar1 = *puVar1 | 0x8000000;
      *puVar1 = *puVar1 | 0x40000000;
      do {
        local_4 = local_4 + 1;
        if (10000 < local_4) {
          return 1;
        }
      } while ((int)*puVar1 < 0);
    }
  }
  else {
    puVar1 = (uint *)(param_1 + 0xb00 + (uint)*param_2 * 0x20);
    if ((int)*puVar1 < 0) {
      *puVar1 = *puVar1 | 0x8000000;
      *puVar1 = *puVar1 | 0x40000000;
      do {
        local_4 = local_4 + 1;
        if (10000 < local_4) {
          return 1;
        }
      } while ((int)*puVar1 < 0);
    }
  }
  return 0;
}



/* === 08012a08 FUN_08012a08 === */

undefined4 FUN_08012a08(int param_1,undefined4 *param_2,int param_3,int param_4,char param_5)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  if ((param_5 == '\0') && (param_4 + 3U >> 2 != 0)) {
    puVar1 = param_2;
    do {
      puVar2 = puVar1 + 1;
      *(undefined4 *)(param_1 + (param_3 + 1) * 0x1000) = *puVar1;
      puVar1 = puVar2;
    } while ((undefined4 *)((int)param_2 + (param_4 + 3U & 0xfffffffc)) != puVar2);
  }
  return 0;
}



/* === 08012a34 FUN_08012a34 === */

void FUN_08012a34(int param_1,undefined4 *param_2,uint param_3)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  
  if (param_3 >> 2 != 0) {
    uVar5 = 0;
    puVar2 = param_2;
    do {
      uVar5 = uVar5 + 1;
      *puVar2 = *(undefined4 *)(param_1 + 0x1000);
      puVar2 = puVar2 + 1;
    } while (param_3 >> 2 != uVar5);
    param_2 = (undefined4 *)((int)param_2 + (param_3 & 0xfffffffc));
  }
  if ((param_3 & 3) != 0) {
    uVar4 = 0;
    uVar5 = *(uint *)(param_1 + 0x1000);
    puVar2 = param_2;
    do {
      uVar1 = uVar4 & 0xff;
      uVar4 = uVar4 + 8;
      puVar3 = (undefined4 *)((int)puVar2 + 1);
      *(char *)puVar2 = (char)(uVar5 >> uVar1);
      puVar2 = puVar3;
    } while (puVar3 != (undefined4 *)((param_3 & 3) + (int)param_2));
  }
  return;
}



/* === 08012a7c FUN_08012a7c === */

undefined4 FUN_08012a7c(int param_1,byte *param_2)

{
  uint uVar1;
  
  uVar1 = (uint)*param_2;
  param_1 = param_1 + uVar1 * 0x20;
  if (param_2[1] != 1) {
    if ((uVar1 != 0) && (-1 < *(int *)(param_1 + 0xb00))) {
      *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) & 0xbfffffff;
    }
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x200000;
    return 0;
  }
  if ((*(int *)(param_1 + 0x900) < 0) || (uVar1 == 0)) {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x200000;
  }
  else {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) & 0xbfffffff;
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x200000;
  }
  return 0;
}



/* === 08012ae4 FUN_08012ae4 === */

undefined4 FUN_08012ae4(int param_1,byte *param_2)

{
  param_1 = param_1 + (uint)*param_2 * 0x20;
  if (param_2[1] == 1) {
    *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) & 0xffdfffff;
    if (param_2[4] - 2 < 2) {
      *(uint *)(param_1 + 0x900) = *(uint *)(param_1 + 0x900) | 0x10000000;
      return 0;
    }
  }
  else {
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) & 0xffdfffff;
    if (param_2[4] - 2 < 2) {
      *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x10000000;
      return 0;
    }
  }
  return 0;
}



/* === 08012b3c FUN_08012b3c === */

undefined4 FUN_08012b3c(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x800) = *(uint *)(param_1 + 0x800) & 0xfffff80f;
  *(uint *)(param_1 + 0x800) = (param_2 & 0x7f) << 4 | *(uint *)(param_1 + 0x800);
  return 0;
}



/* === 08012b60 FUN_08012b60 === */

uint FUN_08012b60(int param_1)

{
  return *(uint *)(param_1 + 0x18) & *(uint *)(param_1 + 0x14);
}



/* === 08012b68 FUN_08012b68 === */

uint FUN_08012b68(int param_1,int param_2)

{
  param_1 = param_1 + param_2 * 0x20;
  return *(uint *)(param_1 + 0x508) & *(uint *)(param_1 + 0x50c);
}



/* === 08012b78 FUN_08012b78 === */

uint FUN_08012b78(int param_1)

{
  return (*(uint *)(param_1 + 0x81c) & *(uint *)(param_1 + 0x818)) >> 0x10;
}



/* === 08012b88 FUN_08012b88 === */

uint FUN_08012b88(int param_1)

{
  return *(uint *)(param_1 + 0x81c) & *(uint *)(param_1 + 0x818) & 0xffff;
}



/* === 08012b98 FUN_08012b98 === */

uint FUN_08012b98(int param_1,int param_2)

{
  return *(uint *)(param_1 + 0x814) & *(uint *)(param_1 + param_2 * 0x20 + 0xb08);
}



/* === 08012bac FUN_08012bac === */

uint FUN_08012bac(int param_1,uint param_2)

{
  return *(uint *)(param_1 + param_2 * 0x20 + 0x908) &
         ((*(uint *)(param_1 + 0x834) >> (param_2 & 0xf) & 1) << 7 | *(uint *)(param_1 + 0x810));
}



/* === 08012bcc FUN_08012bcc === */

uint FUN_08012bcc(int param_1)

{
  return *(uint *)(param_1 + 0x14) & 1;
}



/* === 08012bd4 FUN_08012bd4 === */

undefined4 FUN_08012bd4(int param_1)

{
  *(uint *)(param_1 + 0x900) = DAT_08012bfc & *(uint *)(param_1 + 0x900);
  *(uint *)(param_1 + 0x804) = *(uint *)(param_1 + 0x804) | 0x100;
  return 0;
}



/* === 08012c00 FUN_08012c00 === */

undefined4 FUN_08012c00(int param_1,int param_2,undefined4 param_3)

{
  if ((DAT_08012c54 < *(uint *)(param_1 + 0x40)) && (*(int *)(param_1 + 0xb00) < 0)) {
    return 0;
  }
  *(undefined4 *)(param_1 + 0xb10) = 0;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x80000;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x18;
  *(uint *)(param_1 + 0xb10) = *(uint *)(param_1 + 0xb10) | 0x60000000;
  if (param_2 == 1) {
    *(undefined4 *)(param_1 + 0xb14) = param_3;
    *(uint *)(param_1 + 0xb00) = *(uint *)(param_1 + 0xb00) | 0x80008000;
  }
  return 0;
}



/* === 08012c58 FUN_08012c58 === */

undefined4 FUN_08012c58(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x400) = *(uint *)(param_1 + 0x400) & 0xfffffffc;
  *(uint *)(param_1 + 0x400) = param_2 & 3 | *(uint *)(param_1 + 0x400);
  if (param_2 == 1) {
    *(undefined4 *)(param_1 + 0x404) = 48000;
    return 0;
  }
  if (param_2 == 2) {
    *(undefined4 *)(param_1 + 0x404) = 6000;
    return 0;
  }
  return 1;
}



/* === 08012c9c FUN_08012c9c === */

uint FUN_08012c9c(int param_1)

{
  return *(uint *)(param_1 + 0x414) & 0xffff;
}



/* === 08012ca4 FUN_08012ca4 === */

undefined4 FUN_08012ca4(int param_1,int param_2)

{
  int iVar1;
  uint uVar2;
  uint local_c;
  
  iVar1 = param_1 + param_2 * 0x20;
  local_c = 0;
  uVar2 = *(uint *)(iVar1 + 0x500);
  if ((-1 < *(int *)(param_1 + 8) << 0x1a) || (*(int *)(iVar1 + 0x500) < 0)) {
    *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x40000000;
    if ((uVar2 >> 0x12 & 1) == 0) {
      if (*(int *)(param_1 + 8) << 0x1a < 0) {
        *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
        return 0;
      }
      uVar2 = *(uint *)(iVar1 + 0x500);
      if ((*(uint *)(param_1 + 0x2c) & 0xff0000) != 0) goto LAB_08012d5e;
      *(uint *)(iVar1 + 0x500) = uVar2 & 0x7fffffff;
      *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
      do {
        local_c = local_c + 1;
        if (1000 < local_c) {
          return 0;
        }
      } while (*(int *)(iVar1 + 0x500) < 0);
    }
    else {
      uVar2 = *(uint *)(iVar1 + 0x500);
      if ((*(uint *)(param_1 + 0x410) & 0xff0000) != 0) {
LAB_08012d5e:
        *(uint *)(iVar1 + 0x500) = uVar2 | 0x80000000;
        return 0;
      }
      *(uint *)(iVar1 + 0x500) = uVar2 & 0x7fffffff;
      *(uint *)(iVar1 + 0x500) = *(uint *)(iVar1 + 0x500) | 0x80000000;
      do {
        local_c = local_c + 1;
        if (1000 < local_c) {
          return 0;
        }
      } while (*(int *)(iVar1 + 0x500) < 0);
    }
  }
  return 0;
}



/* === 08012d88 FUN_08012d88 === */

byte FUN_08012d88(int param_1)

{
  byte bVar1;
  byte bVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint local_14;
  
  local_14 = 0;
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) & 0xfffffffe;
  bVar1 = FUN_080125b4(param_1,0x10);
  bVar2 = FUN_08012608(param_1);
  bVar2 = bVar2 | bVar1;
  puVar3 = (uint *)(param_1 + 0x500);
  puVar4 = puVar3;
  if (bVar2 != 0) {
    bVar2 = 1;
  }
  do {
    puVar5 = puVar4 + 8;
    *puVar4 = *puVar4 & 0x7fff7fff | 0x40000000;
    puVar4 = puVar5;
  } while (puVar5 != (uint *)(param_1 + 0x700));
  do {
    *puVar3 = *puVar3 & 0xffff7fff | 0xc0000000;
    do {
      local_14 = local_14 + 1;
      if (1000 < local_14) break;
    } while ((int)*puVar3 < 0);
    puVar3 = puVar3 + 8;
    if (puVar3 == (uint *)(param_1 + 0x700)) {
      *(undefined4 *)(param_1 + 0x414) = 0xffffffff;
      *(undefined4 *)(param_1 + 0x14) = 0xffffffff;
      *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 1;
      return bVar2;
    }
  } while( true );
}



/* === 08012e10 FUN_08012e10 === */

undefined4 FUN_08012e10(int param_1)

{
  undefined4 uVar1;
  
  if (*(code ***)(param_1 + 0x2b8) != (code **)0x0) {
                    /* WARNING: Could not recover jumptable at 0x08012e18. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar1 = (***(code ***)(param_1 + 0x2b8))();
    return uVar1;
  }
  return 0;
}



/* === 08012e20 FUN_08012e20 === */

int FUN_08012e20(int param_1)

{
  int iVar1;
  
  iVar1 = (**(code **)(*(int *)(param_1 + 0x2b8) + 4))();
  if (iVar1 != 0) {
    iVar1 = 3;
  }
  return iVar1;
}



/* === 08013024 FUN_08013024 === */

undefined4 FUN_08013024(int param_1,undefined param_2)

{
  *(undefined *)(param_1 + 0x10) = param_2;
  return 0;
}



/* === 0801302c FUN_0801302c === */

undefined4 FUN_0801302c(int param_1)

{
  if (*(char *)(param_1 + 0x29c) != '\x04') {
    *(undefined *)(param_1 + 0x29d) = *(undefined *)(param_1 + 0x29c);
  }
  *(undefined *)(param_1 + 0x29c) = 4;
  return 0;
}



/* === 08013104 FUN_08013104 === */

undefined4 FUN_08013104(void)

{
  return 0;
}



/* === 08013108 FUN_08013108 === */

undefined4 FUN_08013108(void)

{
  return 0;
}



/* === 08013650 FUN_08013650 === */

void FUN_08013650(undefined *param_1,undefined *param_2)

{
  *param_1 = *param_2;
  param_1[1] = param_2[1];
  *(undefined2 *)(param_1 + 2) = *(undefined2 *)(param_2 + 2);
  *(undefined2 *)(param_1 + 4) = *(undefined2 *)(param_2 + 4);
  *(undefined2 *)(param_1 + 6) = *(undefined2 *)(param_2 + 6);
  return;
}



/* === 08013668 FUN_08013668 === */

undefined4 FUN_08013668(int param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)(param_1 + 0x294) = 2;
  *(undefined4 *)(param_1 + 0x18) = param_3;
  *(undefined4 *)(param_1 + 0x1c) = param_3;
  FUN_08009af0(param_1,0,param_2);
  return 0;
}



/* === 08013684 FUN_08013684 === */

undefined4 FUN_08013684(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  FUN_08009af0(param_1,0,param_2,param_3,param_4);
  return 0;
}



/* === 08013698 FUN_08013698 === */

undefined4 FUN_08013698(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  FUN_08009b0c(param_1,0,param_2,param_3,param_4);
  return 0;
}



/* === 080136ac FUN_080136ac === */

undefined4 FUN_080136ac(int param_1)

{
  *(undefined4 *)(param_1 + 0x294) = 4;
  FUN_08009af0(param_1,0,0);
  return 0;
}



/* === 080136c4 FUN_080136c4 === */

undefined4 FUN_080136c4(int param_1)

{
  *(undefined4 *)(param_1 + 0x294) = 5;
  FUN_08009b0c(param_1,0,0);
  return 0;
}



/* === 08013750 FUN_08013750 === */

undefined4 FUN_08013750(int param_1,uint param_2)

{
  if (param_2 < 0xb) {
    param_1 = param_1 + param_2 * 4;
    *(uint *)(param_1 + 0x388) = *(uint *)(param_1 + 0x388) & 0x7fff;
  }
  return 0;
}



/* === 08013768 FUN_08013768 === */

void FUN_08013768(void)

{
  uint *puVar1;
  
  puVar1 = DAT_080137a0;
  *(uint *)(DAT_0801379c + 0xdc) = *(uint *)(DAT_0801379c + 0xdc) | 0x40;
  *puVar1 = *puVar1 & 0xffffffdf;
  *puVar1 = *puVar1 | 4;
  return;
}



/* === 080137a4 FUN_080137a4 === */

void FUN_080137a4(undefined4 param_1)

{
  switch(param_1) {
  case 0:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 1;
    break;
  case 1:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 2;
    break;
  case 2:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 4;
    break;
  case 3:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 8;
    break;
  case 4:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x10;
    break;
  case 5:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x20;
    break;
  case 6:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x40;
    break;
  case 7:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x80;
    break;
  case 8:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x100;
    break;
  case 9:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x200;
    break;
  case 10:
    *(uint *)(DAT_080138f0 + 0xe0) = *(uint *)(DAT_080138f0 + 0xe0) | 0x400;
  }
  return;
}



/* === 080138f4 FUN_080138f4 === */

void FUN_080138f4(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_08013938;
  puVar3 = DAT_08013938 + 0x1c;
  *DAT_0801393c = 0xff;
  do {
    puVar2 = puVar1 + 7;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 1;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 08013940 FUN_08013940 === */

undefined4 FUN_08013940(int param_1)

{
  undefined4 uVar1;
  
  switch(*(undefined4 *)(param_1 + 8)) {
  case 0:
    *(undefined4 *)(param_1 + 0xb4) = 0x25;
    *(undefined4 *)(param_1 + 300) = 0x26;
    return *(undefined4 *)(param_1 + 8);
  case 1:
    *(undefined4 *)(param_1 + 0xb4) = 0x27;
    *(undefined4 *)(param_1 + 300) = 0x28;
    return 0;
  case 2:
    *(undefined4 *)(param_1 + 0xb4) = 0x3d;
    *(undefined4 *)(param_1 + 300) = 0x3e;
    return 0;
  case 3:
    *(undefined4 *)(param_1 + 0xb4) = 0x53;
    *(undefined4 *)(param_1 + 300) = 0x54;
    return 0;
  case 4:
    uVar1 = 0;
    *(undefined4 *)(param_1 + 0xb4) = 0x55;
    *(undefined4 *)(param_1 + 300) = 0x56;
    break;
  default:
    uVar1 = 1;
  }
  return uVar1;
}



/* === 080139ac FUN_080139ac === */

void FUN_080139ac(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = DAT_08013a40;
  *(undefined4 *)(param_1 + 0x134) = 0;
  *(undefined4 *)(param_1 + 0xb0) = uVar1;
  *(undefined4 *)(param_1 + 0x138) = 0x400;
  *(undefined4 *)(param_1 + 0xcc) = 0;
  *(undefined4 *)(param_1 + 0xd0) = 0x30000;
  *(undefined4 *)(param_1 + 0x144) = 0;
  *(undefined4 *)(param_1 + 0x148) = 0x30000;
  *(undefined4 *)(param_1 + 0xd4) = 4;
  *(undefined4 *)(param_1 + 0x14c) = 4;
  *(undefined4 *)(param_1 + 0xd8) = 3;
  *(undefined4 *)(param_1 + 0x150) = 3;
  uVar1 = DAT_08013a44;
  *(undefined4 *)(param_1 + 0xbc) = 0;
  *(undefined4 *)(param_1 + 0xc0) = 0x400;
  *(undefined4 *)(param_1 + 0x128) = uVar1;
  *(undefined4 *)(param_1 + 0xc4) = 0;
  *(undefined4 *)(param_1 + 200) = 0;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0x13c) = 0;
  *(undefined4 *)(param_1 + 0x140) = 0;
  *(undefined4 *)(param_1 + 0x154) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  FUN_08013940();
  *(undefined4 *)(param_1 + 0xb8) = 0;
  *(undefined4 *)(param_1 + 0x130) = 0x40;
  iVar2 = FUN_0800af40(param_1 + 0xb0);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  iVar2 = FUN_0800af40(param_1 + 0x128);
  if (iVar2 != 0) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  *(int *)(param_1 + 0xa0) = param_1 + 0x128;
  *(int *)(param_1 + 0xa4) = param_1 + 0xb0;
  *(int *)(param_1 + 0xe8) = param_1 + 0x28;
  *(int *)(param_1 + 0x160) = param_1 + 0x28;
  return;
}



/* === 08013a48 FUN_08013a48 === */

undefined4
FUN_08013a48(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  undefined uVar1;
  bool bVar2;
  code **ppcVar3;
  undefined4 *puVar4;
  undefined *puVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  
  do {
    iVar6 = FUN_08015f64(param_1 + 0x28);
  } while (iVar6 != 1);
  iVar6 = FUN_080139ac(param_1);
  puVar5 = DAT_08013b0c;
  puVar4 = DAT_08013b08;
  ppcVar3 = DAT_08013b04;
  if (iVar6 == 0) {
    iVar6 = 0;
    bVar2 = (bool)isCurrentModePrivileged();
    if (bVar2) {
      iVar6 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    uVar1 = *(undefined *)(param_1 + 8);
    *DAT_08013b04 = param_5;
    *puVar5 = uVar1;
    *puVar4 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_08015734(param_1 + 0x28,param_2,param_3);
    uVar8 = 0;
    if (iVar7 != 0) {
      *ppcVar3 = (code *)0x0;
      *puVar4 = 0;
      *puVar5 = 0xff;
      if (param_5 == (code *)0x0) {
        uVar8 = 1;
      }
      else {
        uVar8 = 1;
        (*param_5)(param_6);
      }
    }
    if (iVar6 == 0) {
      enableIRQinterrupts();
    }
    return uVar8;
  }
  if (param_5 == (code *)0x0) {
    return 1;
  }
  (*param_5)(param_6,1);
  return 1;
}



/* === 08013b10 FUN_08013b10 === */

undefined4
FUN_08013b10(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  undefined uVar1;
  bool bVar2;
  code **ppcVar3;
  undefined4 *puVar4;
  undefined *puVar5;
  int iVar6;
  int iVar7;
  undefined4 uVar8;
  
  do {
    iVar6 = FUN_08015f64(param_1 + 0x28);
  } while (iVar6 != 1);
  iVar6 = FUN_080139ac(param_1);
  puVar5 = DAT_08013bd4;
  puVar4 = DAT_08013bd0;
  ppcVar3 = DAT_08013bcc;
  if (iVar6 == 0) {
    iVar6 = 0;
    bVar2 = (bool)isCurrentModePrivileged();
    if (bVar2) {
      iVar6 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    uVar1 = *(undefined *)(param_1 + 8);
    *DAT_08013bcc = param_5;
    *puVar5 = uVar1;
    *puVar4 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_080158b4(param_1 + 0x28,param_2,param_3);
    uVar8 = 0;
    if (iVar7 != 0) {
      *ppcVar3 = (code *)0x0;
      *puVar4 = 0;
      *puVar5 = 0xff;
      if (param_5 == (code *)0x0) {
        uVar8 = 1;
      }
      else {
        uVar8 = 1;
        (*param_5)(param_6);
      }
    }
    if (iVar6 == 0) {
      enableIRQinterrupts();
    }
    return uVar8;
  }
  if (param_5 == (code *)0x0) {
    return 1;
  }
  (*param_5)(param_6,1);
  return 1;
}



/* === 08013bd8 FUN_08013bd8 === */

undefined4
FUN_08013bd8(int param_1,undefined4 param_2,undefined4 param_3,undefined2 param_4,code *param_5,
            code *param_6,undefined4 param_7)

{
  bool bVar1;
  undefined *puVar2;
  code **ppcVar3;
  undefined4 *puVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  
  do {
    iVar5 = FUN_08015f64(param_1 + 0x28);
  } while (iVar5 != 1);
  iVar5 = FUN_080139ac(param_1);
  ppcVar3 = DAT_08013c9c;
  puVar2 = DAT_08013c98;
  if (iVar5 == 0) {
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08013c98 = *(undefined *)(param_1 + 8);
    puVar4 = DAT_08013ca0;
    *ppcVar3 = param_6;
    *puVar4 = param_7;
    if (param_5 != (code *)0x0) {
      (*param_5)(param_7);
    }
    iVar6 = FUN_08015a48(param_1 + 0x28,param_3,param_2,param_4);
    uVar7 = 0;
    if (iVar6 != 0) {
      *ppcVar3 = (code *)0x0;
      *puVar4 = 0;
      *puVar2 = 0xff;
      if (param_6 == (code *)0x0) {
        uVar7 = 1;
      }
      else {
        uVar7 = 1;
        (*param_6)(param_7);
      }
    }
    if (iVar5 == 0) {
      enableIRQinterrupts();
    }
    return uVar7;
  }
  if (param_6 == (code *)0x0) {
    return 1;
  }
  (*param_6)(param_7,1);
  return 1;
}



/* === 08013d74 FUN_08013d74 === */

undefined4 FUN_08013d74(int param_1,uint param_2,int param_3)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar2 = *(byte **)(DAT_08013dc0 + param_3 * 4);
  pbVar1 = pbVar2 + 9;
  do {
    if (*pbVar2 == 0xb) {
      if ((pbVar2[1] != 0) && ((param_2 & 0xff) == 0xb)) goto LAB_08013dae;
    }
    else if ((uint)*pbVar2 == (param_2 & 0xff)) {
LAB_08013dae:
      if ((uint)pbVar2[1] == (param_2 << 0x10) >> 0x18) {
        *(uint *)(param_1 + 0x10) = (uint)pbVar2[2];
        return 0;
      }
    }
    pbVar2 = pbVar2 + 3;
    if (pbVar2 == pbVar1) {
      return 1;
    }
  } while( true );
}



/* === 08013dc4 FUN_08013dc4 === */

undefined4 FUN_08013dc4(undefined4 *param_1)

{
  byte bVar1;
  bool bVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  int local_34;
  undefined4 local_30;
  undefined4 uStack_2c;
  undefined4 local_28;
  
  bVar2 = false;
  uVar4 = param_1[4];
  local_28 = 0;
  local_30 = 2;
  uStack_2c = 0;
  iVar5 = param_1[2] * 4;
  if (param_1[3] == 0) {
    iVar6 = param_1[8];
    bVar2 = (uVar4 & 0xfffffffd) == 0;
    uVar4 = uVar4 - 2;
    if (uVar4 != 0) {
      uVar4 = 1;
    }
    if (*(byte *)param_1 != 0xb) goto LAB_08013e3c;
  }
  else {
    if (uVar4 == 2) {
      uVar4 = 1;
      iVar6 = param_1[8];
      if (*(byte *)param_1 == 0xb) goto LAB_08013e04;
    }
    else {
      bVar2 = true;
      iVar6 = param_1[8];
      uVar4 = (uint)(uVar4 == 0);
      if (*(byte *)param_1 == 0xb) goto LAB_08013dfa;
    }
LAB_08013e3c:
    iVar3 = FUN_08013d74(&local_34,*param_1,iVar5);
    if (iVar3 == 1) {
      return 1;
    }
    if (*(byte *)param_1 < 0xb) {
      iVar3 = DAT_08013f28 + (uint)*(byte *)param_1 * 0x400;
    }
    else {
      iVar3 = 0;
    }
    local_34 = 1 << (uint)*(byte *)((int)param_1 + 1);
    FUN_0800c080(iVar3,&local_34);
  }
LAB_08013dfa:
  if ((*(byte *)((int)param_1 + 2) != 0xb) && (bVar2)) {
    iVar3 = FUN_08013d74(&local_34,*(undefined2 *)((int)param_1 + 2),iVar5 + 1);
    if (iVar3 == 1) {
      return 1;
    }
    if (*(byte *)((int)param_1 + 2) < 0xb) {
      iVar3 = DAT_08013f28 + (uint)*(byte *)((int)param_1 + 2) * 0x400;
    }
    else {
      iVar3 = 0;
    }
    local_34 = 1 << (uint)*(byte *)((int)param_1 + 3);
    FUN_0800c080(iVar3,&local_34);
  }
LAB_08013e04:
  if ((*(byte *)(param_1 + 1) == 0xb) || (uVar4 == 0)) {
    bVar1 = *(byte *)((int)param_1 + 6);
  }
  else {
    iVar3 = FUN_08013d74(&local_34,param_1[1],iVar5 + 2);
    if (iVar3 == 1) {
      return 1;
    }
    if (*(byte *)(param_1 + 1) < 0xb) {
      iVar3 = DAT_08013f28 + (uint)*(byte *)(param_1 + 1) * 0x400;
    }
    else {
      iVar3 = 0;
    }
    local_34 = 1 << (uint)*(byte *)((int)param_1 + 5);
    FUN_0800c080(iVar3,&local_34);
    bVar1 = *(byte *)((int)param_1 + 6);
  }
  if ((bVar1 != 0xb) && (iVar6 != 0)) {
    iVar5 = FUN_08013d74(&local_34,*(undefined2 *)((int)param_1 + 6),iVar5 + 3);
    if (iVar5 == 1) {
      return 1;
    }
    if (*(byte *)((int)param_1 + 6) < 0xb) {
      iVar5 = DAT_08013f28 + (uint)*(byte *)((int)param_1 + 6) * 0x400;
    }
    else {
      iVar5 = 0;
    }
    local_34 = 1 << (uint)*(byte *)((int)param_1 + 7);
    FUN_0800c080(iVar5,&local_34);
    return 0;
  }
  return 0;
}



/* === 08013f2c FUN_08013f2c === */

void FUN_08013f2c(uint *param_1)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  uint *puVar4;
  uint local_28 [4];
  uint local_18;
  uint uStack_14;
  
  iVar3 = 0;
  local_28[0] = *DAT_080140a4;
  local_28[1] = DAT_080140a4[1];
  local_28[2] = DAT_080140a4[2];
  local_28[3] = DAT_080140a4[3];
  local_18 = DAT_080140a4[4];
  uStack_14 = DAT_080140a4[5];
  puVar4 = local_28;
  while (*param_1 != *puVar4) {
    iVar3 = iVar3 + 1;
    puVar4 = puVar4 + 1;
    if (iVar3 == 6) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x80140a2);
      (*pcVar1)();
    }
  }
  iVar2 = DAT_080140a8 + iVar3 * 0x1a0;
  FUN_080137a4(*(undefined *)(DAT_080140a8 + iVar3 * 0x1a0));
  FUN_080137a4(*(undefined *)(iVar2 + 2));
  FUN_080137a4(*(undefined *)(iVar2 + 4));
  FUN_080137a4(*(undefined *)(iVar2 + 6));
  iVar3 = DAT_080140ac;
  switch(*(undefined4 *)(iVar2 + 8)) {
  case 0:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x1000;
    FUN_0800a968(0x23,0,0,*(uint *)(iVar3 + 0xf0) & 0x1000);
    FUN_0800a9e4(0x23);
    break;
  case 1:
    *(uint *)(DAT_080140ac + 0xe8) = *(uint *)(DAT_080140ac + 0xe8) | 0x4000;
    FUN_0800a968(0x24,0,0,*(uint *)(iVar3 + 0xe8) & 0x4000);
    FUN_0800a9e4(0x24);
    break;
  case 2:
    *(uint *)(DAT_080140ac + 0xe8) = *(uint *)(DAT_080140ac + 0xe8) | 0x8000;
    FUN_0800a968(0x33,0,0,*(uint *)(iVar3 + 0xe8) & 0x8000);
    FUN_0800a9e4(0x33);
    break;
  case 3:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x2000;
    FUN_0800a968(0x54,0,0,*(uint *)(iVar3 + 0xf0) & 0x2000);
    FUN_0800a9e4(0x54);
    break;
  case 4:
    *(uint *)(DAT_080140ac + 0xf0) = *(uint *)(DAT_080140ac + 0xf0) | 0x100000;
    FUN_0800a968(0x55,0,0,*(uint *)(iVar3 + 0xf0) & 0x100000);
    FUN_0800a9e4(0x55);
    break;
  case 5:
    *(uint *)(DAT_080140ac + 0xf4) = *(uint *)(DAT_080140ac + 0xf4) | 0x20;
    local_28[0] = *(uint *)(iVar3 + 0xf4) & 0x20;
  }
  iVar3 = FUN_08013dc4(iVar2);
  if (iVar3 == 1) {
    software_bkpt(0xff);
    do {
                    /* WARNING: Do nothing block with infinite loop */
    } while( true );
  }
  return;
}



/* === 080140b0 thunk_FUN_080138f4 === */

void thunk_FUN_080138f4(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_08013938;
  puVar3 = DAT_08013938 + 0x1c;
  *DAT_0801393c = 0xff;
  do {
    puVar2 = puVar1 + 7;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 1;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 080140b4 FUN_080140b4 === */

void FUN_080140b4(void)

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
  
  ppuVar6 = DAT_080140bc;
  puVar8 = *DAT_080140bc;
  uVar10 = puVar8[2];
  uVar9 = puVar8[4] & puVar8[5];
  cVar2 = *(char *)((int)DAT_080140bc + 0x81);
  if (((int)(puVar8[5] << 0x14) < 0) && ((int)(puVar8[4] << 0x1c) < 0)) {
    puVar8[6] = puVar8[6] | 0x800;
    FUN_08015d4c();
    return;
  }
  if ((uVar9 & 100) == 4) {
    (*(code *)DAT_080140bc[0x1d])(DAT_080140bc);
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
            DAT_080140bc[0x21] = (uint *)((uint)DAT_080140bc[0x21] | 4);
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



/* === 080140c0 FUN_080140c0 === */

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



/* === 080140cc FUN_080140cc === */

void FUN_080140cc(void)

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
  
  ppuVar6 = DAT_080140d4;
  puVar8 = *DAT_080140d4;
  uVar10 = puVar8[2];
  uVar9 = puVar8[4] & puVar8[5];
  cVar2 = *(char *)((int)DAT_080140d4 + 0x81);
  if (((int)(puVar8[5] << 0x14) < 0) && ((int)(puVar8[4] << 0x1c) < 0)) {
    puVar8[6] = puVar8[6] | 0x800;
    FUN_08015d4c();
    return;
  }
  if ((uVar9 & 100) == 4) {
    (*(code *)DAT_080140d4[0x1d])(DAT_080140d4);
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
            DAT_080140d4[0x21] = (uint *)((uint)DAT_080140d4[0x21] | 4);
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



/* === 080140d8 FUN_080140d8 === */

void FUN_080140d8(void)

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
  
  ppuVar6 = DAT_080140e0;
  puVar8 = *DAT_080140e0;
  uVar10 = puVar8[2];
  uVar9 = puVar8[4] & puVar8[5];
  cVar2 = *(char *)((int)DAT_080140e0 + 0x81);
  if (((int)(puVar8[5] << 0x14) < 0) && ((int)(puVar8[4] << 0x1c) < 0)) {
    puVar8[6] = puVar8[6] | 0x800;
    FUN_08015d4c();
    return;
  }
  if ((uVar9 & 100) == 4) {
    (*(code *)DAT_080140e0[0x1d])(DAT_080140e0);
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
            DAT_080140e0[0x21] = (uint *)((uint)DAT_080140e0[0x21] | 4);
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



/* === 080140e4 FUN_080140e4 === */

void FUN_080140e4(void)

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
  
  ppuVar6 = DAT_080140ec;
  puVar8 = *DAT_080140ec;
  uVar10 = puVar8[2];
  uVar9 = puVar8[4] & puVar8[5];
  cVar2 = *(char *)((int)DAT_080140ec + 0x81);
  if (((int)(puVar8[5] << 0x14) < 0) && ((int)(puVar8[4] << 0x1c) < 0)) {
    puVar8[6] = puVar8[6] | 0x800;
    FUN_08015d4c();
    return;
  }
  if ((uVar9 & 100) == 4) {
    (*(code *)DAT_080140ec[0x1d])(DAT_080140ec);
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
            DAT_080140ec[0x21] = (uint *)((uint)DAT_080140ec[0x21] | 4);
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



/* === 080140f0 FUN_080140f0 === */

void FUN_080140f0(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_0801411c << 0x18)) {
    FUN_0800b9d4((char)*DAT_0801411c * 0x1a0 + DAT_08014120 + 0xb0);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08014124 FUN_08014124 === */

void FUN_08014124(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_08014154 << 0x18)) {
    FUN_0800b9d4((char)*DAT_08014154 * 0x1a0 + DAT_08014158 + 0x128);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 0801415c FUN_0801415c === */

/* WARNING: Removing unreachable block (ram,0x08013d32) */

void FUN_0801415c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  pbVar3 = DAT_08013d64;
  ppcVar2 = DAT_08013d60;
  iVar11 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar11 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08013d64 = 0xff;
  pcVar7 = *ppcVar2;
  if (pcVar7 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar7)(*DAT_08013d68,0);
  }
  piVar4 = DAT_08013d70;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar9 = 0;
    piVar8 = DAT_08013d70;
    iVar10 = DAT_08013d6c;
    do {
      iVar5 = *piVar8;
      if ((iVar5 != 0) && (iVar6 = piVar8[1], iVar6 != 0)) {
        if (piVar8[6] == 1) {
          iVar5 = FUN_08013a48(iVar10,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
joined_r0x08013d4e:
          if (iVar5 != 0) goto LAB_08013d02;
        }
        else {
          if (piVar8[6] == 0) {
            iVar5 = FUN_08013b10(iVar10,iVar5,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                                 piVar8[5]);
            goto joined_r0x08013d4e;
          }
          iVar5 = FUN_08013bd8(iVar10,iVar5,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
          if (iVar5 != 0) goto LAB_08013d02;
          iVar5 = 0;
        }
        piVar4[iVar9 * 7] = iVar5;
        piVar4[iVar9 * 7 + 1] = iVar5;
        break;
      }
LAB_08013d02:
      iVar9 = iVar9 + 1;
      piVar8 = piVar8 + 7;
      iVar10 = iVar10 + 0x1a0;
    } while (iVar9 != 4);
  }
  if (iVar11 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08014164 FUN_08014164 === */

/* WARNING: Removing unreachable block (ram,0x08013d32) */

void FUN_08014164(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  pbVar3 = DAT_08013d64;
  ppcVar2 = DAT_08013d60;
  iVar11 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar11 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08013d64 = 0xff;
  pcVar7 = *ppcVar2;
  if (pcVar7 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar7)(*DAT_08013d68,0);
  }
  piVar4 = DAT_08013d70;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar9 = 0;
    piVar8 = DAT_08013d70;
    iVar10 = DAT_08013d6c;
    do {
      iVar5 = *piVar8;
      if ((iVar5 != 0) && (iVar6 = piVar8[1], iVar6 != 0)) {
        if (piVar8[6] == 1) {
          iVar5 = FUN_08013a48(iVar10,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
joined_r0x08013d4e:
          if (iVar5 != 0) goto LAB_08013d02;
        }
        else {
          if (piVar8[6] == 0) {
            iVar5 = FUN_08013b10(iVar10,iVar5,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                                 piVar8[5]);
            goto joined_r0x08013d4e;
          }
          iVar5 = FUN_08013bd8(iVar10,iVar5,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
          if (iVar5 != 0) goto LAB_08013d02;
          iVar5 = 0;
        }
        piVar4[iVar9 * 7] = iVar5;
        piVar4[iVar9 * 7 + 1] = iVar5;
        break;
      }
LAB_08013d02:
      iVar9 = iVar9 + 1;
      piVar8 = piVar8 + 7;
      iVar10 = iVar10 + 0x1a0;
    } while (iVar9 != 4);
  }
  if (iVar11 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 0801416c FUN_0801416c === */

/* WARNING: Removing unreachable block (ram,0x08013d32) */

void FUN_0801416c(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  pbVar3 = DAT_08013d64;
  ppcVar2 = DAT_08013d60;
  iVar11 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar11 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08013d64 = 0xff;
  pcVar7 = *ppcVar2;
  if (pcVar7 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar7)(*DAT_08013d68,0);
  }
  piVar4 = DAT_08013d70;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar9 = 0;
    piVar8 = DAT_08013d70;
    iVar10 = DAT_08013d6c;
    do {
      iVar5 = *piVar8;
      if ((iVar5 != 0) && (iVar6 = piVar8[1], iVar6 != 0)) {
        if (piVar8[6] == 1) {
          iVar5 = FUN_08013a48(iVar10,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
joined_r0x08013d4e:
          if (iVar5 != 0) goto LAB_08013d02;
        }
        else {
          if (piVar8[6] == 0) {
            iVar5 = FUN_08013b10(iVar10,iVar5,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                                 piVar8[5]);
            goto joined_r0x08013d4e;
          }
          iVar5 = FUN_08013bd8(iVar10,iVar5,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
          if (iVar5 != 0) goto LAB_08013d02;
          iVar5 = 0;
        }
        piVar4[iVar9 * 7] = iVar5;
        piVar4[iVar9 * 7 + 1] = iVar5;
        break;
      }
LAB_08013d02:
      iVar9 = iVar9 + 1;
      piVar8 = piVar8 + 7;
      iVar10 = iVar10 + 0x1a0;
    } while (iVar9 != 4);
  }
  if (iVar11 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08014174 FUN_08014174 === */

void FUN_08014174(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  code *pcVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  
  iVar11 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar11 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  FUN_08015580();
  pbVar3 = DAT_08013d64;
  ppcVar2 = DAT_08013d60;
  *DAT_08013d64 = 0xff;
  pcVar7 = *ppcVar2;
  if (pcVar7 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar7)(*DAT_08013d68,1);
  }
  piVar4 = DAT_08013d70;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar9 = 0;
    piVar8 = DAT_08013d70;
    iVar10 = DAT_08013d6c;
    do {
      iVar5 = *piVar8;
      if ((iVar5 != 0) && (iVar6 = piVar8[1], iVar6 != 0)) {
        if (piVar8[6] == 1) {
          iVar5 = FUN_08013a48(iVar10,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
joined_r0x08013d4e:
          if (iVar5 != 0) goto LAB_08013d02;
        }
        else {
          if (piVar8[6] == 0) {
            iVar5 = FUN_08013b10(iVar10,iVar5,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                                 piVar8[5]);
            goto joined_r0x08013d4e;
          }
          iVar5 = FUN_08013bd8(iVar10,iVar5,iVar6,*(undefined2 *)(piVar8 + 2),piVar8[3],piVar8[4],
                               piVar8[5]);
          if (iVar5 != 0) goto LAB_08013d02;
          iVar5 = 0;
        }
        piVar4[iVar9 * 7] = iVar5;
        piVar4[iVar9 * 7 + 1] = iVar5;
        break;
      }
LAB_08013d02:
      iVar9 = iVar9 + 1;
      piVar8 = piVar8 + 7;
      iVar10 = iVar10 + 0x1a0;
    } while (iVar9 != 4);
  }
  if (iVar11 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 080143d4 FUN_080143d4 === */

void FUN_080143d4(int *param_1)

{
  int *piVar1;
  char cVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 0;
  iVar4 = *param_1;
  iVar5 = DAT_0801457c;
  do {
    piVar1 = (int *)(iVar5 + 0x10);
    iVar5 = iVar5 + 100;
    if (iVar4 == *piVar1) {
      cVar2 = *(char *)(iVar6 * 100 + DAT_0801457c + 0xc);
      if (iVar4 == 0x40000000) {
        *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 1;
        if (cVar2 == '\0') {
          return;
        }
        FUN_0800a968(0x1c,0xf,0);
        uVar3 = 0x1c;
      }
      else if (iVar4 == DAT_08014580) {
        *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 2;
        if (cVar2 == '\0') {
          return;
        }
        FUN_0800a968(0x1d,0xf,0);
        uVar3 = 0x1d;
      }
      else if (iVar4 == DAT_08014584) {
        *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 4;
        if (cVar2 == '\0') {
          return;
        }
        FUN_0800a968(0x1e,0xf,0);
        uVar3 = 0x1e;
      }
      else {
        if (iVar4 != DAT_08014588) goto LAB_0801440c;
        *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 8;
        if (cVar2 == '\0') {
          return;
        }
        FUN_0800a968(0x32,0xf,0);
        uVar3 = 0x32;
      }
      FUN_0800a9e4(uVar3);
      return;
    }
    iVar6 = iVar6 + 1;
  } while (iVar6 != 4);
  if (iVar4 == 0x40000000) {
    *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 1;
  }
  else if (iVar4 == DAT_08014580) {
    *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 2;
  }
  else if (iVar4 == DAT_08014584) {
    *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 4;
  }
  else if (iVar4 == DAT_08014588) {
    *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 8;
  }
  else {
LAB_0801440c:
    if (iVar4 == DAT_0801458c) {
      *(uint *)(DAT_08014590 + 0xe8) = *(uint *)(DAT_08014590 + 0xe8) | 0x10;
    }
  }
  return;
}



/* === 08014594 FUN_08014594 === */

void FUN_08014594(int *param_1)

{
  int *piVar1;
  int iVar2;
  code *UNRECOVERED_JUMPTABLE;
  int iVar3;
  
  iVar3 = 0;
  iVar2 = DAT_080145c8;
  do {
    piVar1 = (int *)(iVar2 + 0x10);
    iVar2 = iVar2 + 100;
    if (*param_1 == *piVar1) {
      iVar2 = iVar3 * 100 + DAT_080145c8;
      UNRECOVERED_JUMPTABLE = *(code **)(iVar2 + 0x5c);
      if (UNRECOVERED_JUMPTABLE == (code *)0x0) {
        return;
      }
                    /* WARNING: Could not recover jumptable at 0x080145c4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*UNRECOVERED_JUMPTABLE)(*(undefined4 *)(iVar2 + 0x60));
      return;
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 != 4);
  return;
}



/* === 080145cc FUN_080145cc === */

int * FUN_080145cc(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  piVar1 = DAT_080145d4;
  iVar3 = *DAT_080145d4;
  iVar5 = *(int *)(iVar3 + 0xc);
  iVar4 = *(int *)(iVar3 + 0x10);
  if ((iVar4 << 0x1e < 0) && (iVar5 << 0x1e < 0)) {
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffd;
    *(undefined *)(piVar1 + 7) = 1;
    if ((*(uint *)(iVar3 + 0x18) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270();
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1d < 0) && (iVar5 << 0x1d < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffb;
    *(undefined *)(piVar1 + 7) = 2;
    if ((*(uint *)(iVar3 + 0x18) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1c < 0) && (iVar5 << 0x1c < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffff7;
    *(undefined *)(piVar1 + 7) = 4;
    if ((*(uint *)(iVar3 + 0x1c) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1b < 0) && (iVar5 << 0x1b < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xffffffef;
    *(undefined *)(piVar1 + 7) = 8;
    if ((*(uint *)(iVar3 + 0x1c) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1f < 0) && (iVar5 << 0x1f < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffffe;
    FUN_08014594(piVar1);
  }
  if (iVar4 << 0x18 < 0) {
    if (-1 < iVar5 << 0x18) goto LAB_080162b6;
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffff7f;
    FUN_08016604(piVar1);
    iVar3 = iVar4 << 0x17;
  }
  else {
    if (-1 < iVar4 << 0x17) goto LAB_080162b6;
    iVar3 = iVar5 << 0x18;
  }
  if (iVar3 < 0) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffeff;
    FUN_08016608(piVar1);
  }
LAB_080162b6:
  piVar2 = (int *)(iVar4 << 0x19);
  if (((int)piVar2 < 0) && (iVar5 << 0x19 < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffbf;
    piVar2 = (int *)FUN_08016278(piVar1);
  }
  if ((iVar4 << 0x1a < 0) && (iVar5 << 0x1a < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffdf;
    return piVar1;
  }
  return piVar2;
}



/* === 080145d8 FUN_080145d8 === */

int * FUN_080145d8(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  piVar1 = DAT_080145e0;
  iVar3 = *DAT_080145e0;
  iVar5 = *(int *)(iVar3 + 0xc);
  iVar4 = *(int *)(iVar3 + 0x10);
  if ((iVar4 << 0x1e < 0) && (iVar5 << 0x1e < 0)) {
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffd;
    *(undefined *)(piVar1 + 7) = 1;
    if ((*(uint *)(iVar3 + 0x18) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270();
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1d < 0) && (iVar5 << 0x1d < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffb;
    *(undefined *)(piVar1 + 7) = 2;
    if ((*(uint *)(iVar3 + 0x18) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1c < 0) && (iVar5 << 0x1c < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffff7;
    *(undefined *)(piVar1 + 7) = 4;
    if ((*(uint *)(iVar3 + 0x1c) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1b < 0) && (iVar5 << 0x1b < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xffffffef;
    *(undefined *)(piVar1 + 7) = 8;
    if ((*(uint *)(iVar3 + 0x1c) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1f < 0) && (iVar5 << 0x1f < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffffe;
    FUN_08014594(piVar1);
  }
  if (iVar4 << 0x18 < 0) {
    if (-1 < iVar5 << 0x18) goto LAB_080162b6;
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffff7f;
    FUN_08016604(piVar1);
    iVar3 = iVar4 << 0x17;
  }
  else {
    if (-1 < iVar4 << 0x17) goto LAB_080162b6;
    iVar3 = iVar5 << 0x18;
  }
  if (iVar3 < 0) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffeff;
    FUN_08016608(piVar1);
  }
LAB_080162b6:
  piVar2 = (int *)(iVar4 << 0x19);
  if (((int)piVar2 < 0) && (iVar5 << 0x19 < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffbf;
    piVar2 = (int *)FUN_08016278(piVar1);
  }
  if ((iVar4 << 0x1a < 0) && (iVar5 << 0x1a < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffdf;
    return piVar1;
  }
  return piVar2;
}



/* === 080145e4 FUN_080145e4 === */

int * FUN_080145e4(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  piVar1 = DAT_080145ec;
  iVar3 = *DAT_080145ec;
  iVar5 = *(int *)(iVar3 + 0xc);
  iVar4 = *(int *)(iVar3 + 0x10);
  if ((iVar4 << 0x1e < 0) && (iVar5 << 0x1e < 0)) {
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffd;
    *(undefined *)(piVar1 + 7) = 1;
    if ((*(uint *)(iVar3 + 0x18) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270();
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1d < 0) && (iVar5 << 0x1d < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffb;
    *(undefined *)(piVar1 + 7) = 2;
    if ((*(uint *)(iVar3 + 0x18) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1c < 0) && (iVar5 << 0x1c < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffff7;
    *(undefined *)(piVar1 + 7) = 4;
    if ((*(uint *)(iVar3 + 0x1c) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1b < 0) && (iVar5 << 0x1b < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xffffffef;
    *(undefined *)(piVar1 + 7) = 8;
    if ((*(uint *)(iVar3 + 0x1c) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1f < 0) && (iVar5 << 0x1f < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffffe;
    FUN_08014594(piVar1);
  }
  if (iVar4 << 0x18 < 0) {
    if (-1 < iVar5 << 0x18) goto LAB_080162b6;
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffff7f;
    FUN_08016604(piVar1);
    iVar3 = iVar4 << 0x17;
  }
  else {
    if (-1 < iVar4 << 0x17) goto LAB_080162b6;
    iVar3 = iVar5 << 0x18;
  }
  if (iVar3 < 0) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffeff;
    FUN_08016608(piVar1);
  }
LAB_080162b6:
  piVar2 = (int *)(iVar4 << 0x19);
  if (((int)piVar2 < 0) && (iVar5 << 0x19 < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffbf;
    piVar2 = (int *)FUN_08016278(piVar1);
  }
  if ((iVar4 << 0x1a < 0) && (iVar5 << 0x1a < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffdf;
    return piVar1;
  }
  return piVar2;
}



/* === 080145f0 FUN_080145f0 === */

int * FUN_080145f0(void)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  piVar1 = DAT_080145f8;
  iVar3 = *DAT_080145f8;
  iVar5 = *(int *)(iVar3 + 0xc);
  iVar4 = *(int *)(iVar3 + 0x10);
  if ((iVar4 << 0x1e < 0) && (iVar5 << 0x1e < 0)) {
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffd;
    *(undefined *)(piVar1 + 7) = 1;
    if ((*(uint *)(iVar3 + 0x18) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270();
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1d < 0) && (iVar5 << 0x1d < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffffb;
    *(undefined *)(piVar1 + 7) = 2;
    if ((*(uint *)(iVar3 + 0x18) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1c < 0) && (iVar5 << 0x1c < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xfffffff7;
    *(undefined *)(piVar1 + 7) = 4;
    if ((*(uint *)(iVar3 + 0x1c) & 3) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1b < 0) && (iVar5 << 0x1b < 0)) {
    iVar3 = *piVar1;
    *(undefined4 *)(iVar3 + 0x10) = 0xffffffef;
    *(undefined *)(piVar1 + 7) = 8;
    if ((*(uint *)(iVar3 + 0x1c) & 0x300) == 0) {
      FUN_0801626c();
      FUN_08016274(piVar1);
    }
    else {
      FUN_08016270(piVar1);
    }
    *(undefined *)(piVar1 + 7) = 0;
  }
  if ((iVar4 << 0x1f < 0) && (iVar5 << 0x1f < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffffe;
    FUN_08014594(piVar1);
  }
  if (iVar4 << 0x18 < 0) {
    if (-1 < iVar5 << 0x18) goto LAB_080162b6;
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffff7f;
    FUN_08016604(piVar1);
    iVar3 = iVar4 << 0x17;
  }
  else {
    if (-1 < iVar4 << 0x17) goto LAB_080162b6;
    iVar3 = iVar5 << 0x18;
  }
  if (iVar3 < 0) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xfffffeff;
    FUN_08016608(piVar1);
  }
LAB_080162b6:
  piVar2 = (int *)(iVar4 << 0x19);
  if (((int)piVar2 < 0) && (iVar5 << 0x19 < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffbf;
    piVar2 = (int *)FUN_08016278(piVar1);
  }
  if ((iVar4 << 0x1a < 0) && (iVar5 << 0x1a < 0)) {
    *(undefined4 *)(*piVar1 + 0x10) = 0xffffffdf;
    return piVar1;
  }
  return piVar2;
}



/* === 080145fc FUN_080145fc === */

int FUN_080145fc(int **param_1,int *param_2)

{
  int iVar1;
  int *piVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  int local_34;
  int local_30;
  int local_2c;
  int local_28 [4];
  undefined4 local_18;
  int local_14;
  int local_10;
  int iStack_c;
  
  piVar2 = (int *)(*param_2 * 100 + DAT_08014610);
  *param_1 = piVar2;
  iVar5 = *param_2;
  if (iVar5 < 4) {
    iVar4 = param_2[2];
    iVar1 = *param_2;
    piVar2[1] = param_2[1];
    *piVar2 = iVar1;
    piVar2[2] = iVar4;
    piVar6 = DAT_080143cc;
    *(undefined *)(piVar2 + 3) = *(undefined *)(param_2 + 3);
    if (piVar2[1] == 0) {
      iVar1 = 0;
    }
    else {
      iVar1 = 0x10;
    }
    piVar2[6] = iVar1;
    piVar2[5] = 0;
    local_28[0] = *piVar6;
    local_28[1] = piVar6[1];
    local_28[2] = piVar6[2];
    local_28[3] = piVar6[3];
    iVar5 = local_28[iVar5];
    piVar2[4] = iVar5;
    if ((iVar5 == 0x40000000) || (iVar5 == DAT_080143d0)) {
      uVar3 = piVar2[2];
    }
    else {
      uVar3 = (uint)*(ushort *)(piVar2 + 2);
    }
    piVar6 = piVar2 + 4;
    piVar2[10] = 0x80;
    piVar2[7] = uVar3;
    piVar2[8] = 0;
    local_34 = FUN_080164e8(piVar6);
    if (local_34 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    local_18 = 0x1000;
    local_30 = local_34;
    local_2c = local_34;
    local_14 = local_34;
    local_10 = local_34;
    iStack_c = local_34;
    local_34 = FUN_080160d4(piVar6,&local_18);
    if (local_34 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
    local_2c = local_34;
    iVar5 = FUN_0801654c(piVar6,&local_34);
    if (iVar5 != 0) {
      software_bkpt(0xff);
      do {
                    /* WARNING: Do nothing block with infinite loop */
      } while( true );
    }
  }
  else {
    iVar5 = 1;
  }
  return iVar5;
}



/* === 08014614 FUN_08014614 === */

int FUN_08014614(int *param_1)

{
  int iVar1;
  
  if (*(char *)(*param_1 + 0xc) != '\0') {
    iVar1 = FUN_0801601c();
    if (iVar1 != 0) {
      iVar1 = 1;
    }
    return iVar1;
  }
  iVar1 = FUN_08015f6c(*param_1 + 0x10);
  if (iVar1 != 0) {
    iVar1 = 1;
  }
  return iVar1;
}



/* === 08014660 FUN_08014660 === */

void FUN_08014660(code **param_1)

{
  code *pcVar1;
  code *pcVar2;
  code *pcVar3;
  code *pcVar4;
  
  pcVar1 = param_1[3];
  pcVar2 = param_1[4];
  pcVar3 = pcVar1 + -(*(uint *)(DAT_080146e0 + 0x8c) & 0xffff);
  if ((*param_1 != (code *)0x0) && (pcVar2 != pcVar3)) {
    pcVar4 = param_1[2];
    if (pcVar2 < pcVar3) {
      FUN_08015470();
      (**param_1)(pcVar4 + (int)pcVar2,(int)pcVar3 - (int)pcVar2,param_1[1],0);
      pcVar1 = param_1[3];
      param_1[4] = pcVar3;
    }
    else {
      FUN_08015470();
      (**param_1)(pcVar4 + (int)pcVar2,(int)pcVar1 - (int)pcVar2,param_1[1],0);
      FUN_08015470(pcVar4,pcVar3);
      (**param_1)(pcVar4,pcVar3,param_1[1],0);
      pcVar1 = param_1[3];
      param_1[4] = pcVar3;
    }
    if (pcVar1 == pcVar3) {
      param_1[4] = (code *)0x0;
      return;
    }
  }
  return;
}



/* === 080146e4 FUN_080146e4 === */

void FUN_080146e4(undefined4 param_1)

{
  switch(param_1) {
  case 0:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 1;
    break;
  case 1:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 2;
    break;
  case 2:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 4;
    break;
  case 3:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 8;
    break;
  case 4:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x10;
    break;
  case 5:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x20;
    break;
  case 6:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x40;
    break;
  case 7:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x80;
    break;
  case 8:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x100;
    break;
  case 9:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x200;
    break;
  case 10:
    *(uint *)(DAT_08014830 + 0xe0) = *(uint *)(DAT_08014830 + 0xe0) | 0x400;
  }
  return;
}



/* === 08014834 FUN_08014834 === */

void FUN_08014834(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_08014878;
  puVar3 = DAT_08014878 + 0x3f;
  *DAT_0801487c = 0xff;
  do {
    puVar2 = puVar1 + 7;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 1;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 08014880 FUN_08014880 === */

undefined4 FUN_08014880(int param_1)

{
  undefined4 uVar1;
  
  switch(*(undefined4 *)(param_1 + 0x1c)) {
  case 0:
    *(undefined4 *)(param_1 + 0xcc) = 0x29;
    *(undefined4 *)(param_1 + 0x144) = 0x2a;
    return *(undefined4 *)(param_1 + 0x1c);
  case 1:
    *(undefined4 *)(param_1 + 0xcc) = 0x2b;
    *(undefined4 *)(param_1 + 0x144) = 0x2c;
    return 0;
  case 2:
    *(undefined4 *)(param_1 + 0xcc) = 0x2d;
    *(undefined4 *)(param_1 + 0x144) = 0x2e;
    return 0;
  case 3:
    *(undefined4 *)(param_1 + 0xcc) = 0x3f;
    *(undefined4 *)(param_1 + 0x144) = 0x40;
    return 0;
  case 4:
    *(undefined4 *)(param_1 + 0xcc) = 0x41;
    *(undefined4 *)(param_1 + 0x144) = 0x42;
    return 0;
  case 5:
    *(undefined4 *)(param_1 + 0xcc) = 0x47;
    *(undefined4 *)(param_1 + 0x144) = 0x48;
    return 0;
  case 6:
    *(undefined4 *)(param_1 + 0xcc) = 0x4f;
    *(undefined4 *)(param_1 + 0x144) = 0x50;
    return 0;
  case 7:
    uVar1 = 0;
    *(undefined4 *)(param_1 + 0xcc) = 0x51;
    *(undefined4 *)(param_1 + 0x144) = 0x52;
    break;
  default:
    uVar1 = 1;
  }
  return uVar1;
}



/* === 08014924 FUN_08014924 === */

undefined4
FUN_08014924(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  code **ppcVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  
  iVar7 = param_1 + 0x34;
  do {
    iVar5 = FUN_08016bcc(iVar7);
  } while (iVar5 != 0x20);
  *(undefined4 *)(param_1 + 0xd8) = 0x400;
  *(undefined4 *)(param_1 + 0x150) = 0x400;
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *(undefined4 *)(param_1 + 0xe8) = 0x30000;
  *(undefined4 *)(param_1 + 0x160) = 0x30000;
  iVar5 = DAT_08014a1c;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  *(int *)(param_1 + 200) = iVar5;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(int *)(param_1 + 0x140) = iVar5 + 1000;
  *(undefined4 *)(param_1 + 0xd0) = 0;
  *(undefined4 *)(param_1 + 0x14c) = 0;
  *(undefined4 *)(param_1 + 0x15c) = 0;
  *(undefined4 *)(param_1 + 0x164) = 0;
  *(undefined4 *)(param_1 + 0x148) = 0x40;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0x154) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  FUN_08014880(param_1);
  iVar5 = FUN_0800af40(param_1 + 0x140);
  if (iVar5 == 0) {
    *(int *)(param_1 + 0xb0) = param_1 + 0x140;
    *(int *)(param_1 + 0x178) = iVar7;
    ppcVar4 = DAT_08014a28;
    puVar3 = DAT_08014a24;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08014a24 = *(undefined *)(param_1 + 0x1c);
    puVar2 = DAT_08014a20;
    *ppcVar4 = param_5;
    *puVar2 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_08016678(iVar7,param_2,param_3);
    if (iVar7 == 0) {
      uVar6 = 0;
    }
    else {
      *puVar3 = 0xff;
      *ppcVar4 = (code *)0x0;
      *puVar2 = 0;
      if (param_5 == (code *)0x0) {
        uVar6 = 1;
      }
      else {
        (*param_5)(param_6,1);
        uVar6 = 1;
      }
    }
    if (iVar5 == 0) {
      enableIRQinterrupts();
    }
    return uVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08014a2c FUN_08014a2c === */

undefined4
FUN_08014a2c(int param_1,undefined4 param_2,undefined2 param_3,code *param_4,code *param_5,
            undefined4 param_6)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined *puVar3;
  code **ppcVar4;
  int iVar5;
  undefined4 uVar6;
  int iVar7;
  
  iVar7 = param_1 + 0x34;
  do {
    iVar5 = FUN_08016bcc(iVar7);
  } while (iVar5 != 0x20);
  *(undefined4 *)(param_1 + 0xd8) = 0x400;
  *(undefined4 *)(param_1 + 0x150) = 0x400;
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *(undefined4 *)(param_1 + 0xe8) = 0x30000;
  *(undefined4 *)(param_1 + 0x160) = 0x30000;
  iVar5 = DAT_08014b24;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  *(int *)(param_1 + 200) = iVar5;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(int *)(param_1 + 0x140) = iVar5 + 1000;
  *(undefined4 *)(param_1 + 0xd0) = 0;
  *(undefined4 *)(param_1 + 0x14c) = 0;
  *(undefined4 *)(param_1 + 0x15c) = 0;
  *(undefined4 *)(param_1 + 0x164) = 0;
  *(undefined4 *)(param_1 + 0x148) = 0x40;
  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0x154) = 0;
  *(undefined4 *)(param_1 + 0x158) = 0;
  FUN_08014880(param_1);
  iVar5 = FUN_0800af40(param_1 + 200);
  if (iVar5 == 0) {
    *(int *)(param_1 + 0xb4) = param_1 + 200;
    *(int *)(param_1 + 0x100) = iVar7;
    ppcVar4 = DAT_08014b30;
    puVar3 = DAT_08014b2c;
    iVar5 = 0;
    bVar1 = (bool)isCurrentModePrivileged();
    if (bVar1) {
      iVar5 = isIRQinterruptsEnabled();
    }
    disableIRQinterrupts();
    *DAT_08014b2c = *(undefined *)(param_1 + 0x1c);
    puVar2 = DAT_08014b28;
    *ppcVar4 = param_5;
    *puVar2 = param_6;
    if (param_4 != (code *)0x0) {
      (*param_4)(param_6);
    }
    iVar7 = FUN_08017434(iVar7,param_2,param_3);
    if (iVar7 == 0) {
      uVar6 = 0;
    }
    else {
      *puVar3 = 0xff;
      *ppcVar4 = (code *)0x0;
      *puVar2 = 0;
      if (param_5 == (code *)0x0) {
        uVar6 = 1;
      }
      else {
        (*param_5)(param_6,1);
        uVar6 = 1;
      }
    }
    if (iVar5 == 0) {
      enableIRQinterrupts();
    }
    return uVar6;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08014bf4 FUN_08014bf4 === */

undefined4 FUN_08014bf4(int param_1,uint param_2,int param_3)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar1 = *(byte **)(DAT_08014c48 + param_3 * 4);
  pbVar2 = pbVar1 + 9;
  while (((((uint)*pbVar1 == (uint)*DAT_08014c4c && (pbVar1[1] == DAT_08014c4c[1])) ||
          ((uint)*pbVar1 != (param_2 & 0xff))) || ((uint)pbVar1[1] != (param_2 << 0x10) >> 0x18))) {
    pbVar1 = pbVar1 + 3;
    if (pbVar1 == pbVar2) {
      return 1;
    }
  }
  *(uint *)(param_1 + 0x10) = (uint)pbVar1[2];
  return 0;
}



/* === 08014c50 FUN_08014c50 === */

undefined4 FUN_08014c50(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_2c;
  undefined4 local_28;
  undefined4 uStack_24;
  undefined4 local_20;
  
  iVar2 = 0;
  iVar3 = *(int *)(param_1 + 0x1c) * 2;
  local_20 = 0;
  local_28 = 2;
  uStack_24 = 0;
  if (*(char *)(param_1 + 0x18) != '\v') {
    iVar1 = FUN_08014bf4(&local_2c,*(undefined4 *)(param_1 + 0x18),iVar3);
    if (iVar1 == 1) {
      return 1;
    }
    if (*(byte *)(param_1 + 0x18) < 0xb) {
      iVar2 = DAT_08014ce0 + (uint)*(byte *)(param_1 + 0x18) * 0x400;
    }
    local_2c = 1 << (uint)*(byte *)(param_1 + 0x19);
    FUN_0800c080(iVar2,&local_2c);
  }
  if (*(char *)(param_1 + 0x1a) != '\v') {
    iVar2 = FUN_08014bf4(&local_2c,*(undefined2 *)(param_1 + 0x1a),iVar3 + 1);
    if (iVar2 == 1) {
      return 1;
    }
    if (*(byte *)(param_1 + 0x1a) < 0xb) {
      iVar2 = DAT_08014ce0 + (uint)*(byte *)(param_1 + 0x1a) * 0x400;
    }
    else {
      iVar2 = 0;
    }
    local_2c = 1 << (uint)*(byte *)(param_1 + 0x1b);
    FUN_0800c080(iVar2,&local_2c);
    return 0;
  }
  return 0;
}



/* === 08014ce4 FUN_08014ce4 === */

void FUN_08014ce4(uint *param_1)

{
  code *pcVar1;
  int iVar2;
  int extraout_r1;
  int extraout_r1_00;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint local_40;
  uint local_3c [4];
  undefined4 local_2c;
  uint uStack_28;
  uint uStack_24;
  uint uStack_20;
  uint local_1c;
  
  iVar4 = DAT_08014e94;
  iVar3 = 0;
  puVar5 = &local_40;
  local_3c[0] = *DAT_08014e90;
  local_3c[1] = DAT_08014e90[1];
  local_3c[2] = DAT_08014e90[2];
  local_3c[3] = DAT_08014e90[3];
  local_2c = DAT_08014e90[4];
  uStack_28 = DAT_08014e90[5];
  uStack_24 = DAT_08014e90[6];
  uStack_20 = DAT_08014e90[7];
  local_1c = DAT_08014e90[8];
  while (puVar5 = puVar5 + 1, *param_1 != *puVar5) {
    iVar3 = iVar3 + 1;
    if (iVar3 == 9) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x8014e8e);
      (*pcVar1)();
    }
  }
  FUN_080146e4(*(undefined *)(iVar3 * 0x1b8 + DAT_08014e94 + 0x1a));
  FUN_080146e4(*(undefined *)(extraout_r1 + 0x18));
  iVar2 = DAT_08014e98;
  switch(*(undefined4 *)(extraout_r1_00 + 0x1c)) {
  case 0:
    *(uint *)(DAT_08014e98 + 0xf0) = *(uint *)(DAT_08014e98 + 0xf0) | 0x10;
    break;
  case 1:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x20000;
    break;
  case 2:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x40000;
    break;
  case 3:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x80000;
    break;
  case 4:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x100000;
    break;
  case 5:
    *(uint *)(DAT_08014e98 + 0xf0) = *(uint *)(DAT_08014e98 + 0xf0) | 0x20;
    break;
  case 6:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x40000000;
    break;
  case 7:
    *(uint *)(DAT_08014e98 + 0xe8) = *(uint *)(DAT_08014e98 + 0xe8) | 0x80000000;
    break;
  case 8:
    *(uint *)(DAT_08014e98 + 0xf4) = *(uint *)(DAT_08014e98 + 0xf4) | 8;
    local_40 = *(uint *)(iVar2 + 0xf4) & 8;
  }
  iVar2 = FUN_08014c50(extraout_r1_00);
  if (iVar2 != 1) {
    iVar4 = iVar3 * 0x1b8 + iVar4;
    local_3c[0] = *DAT_08014e9c;
    local_3c[1] = DAT_08014e9c[1];
    local_3c[2] = DAT_08014e9c[2];
    local_3c[3] = DAT_08014e9c[3];
    local_2c = CONCAT22(local_2c._2_2_,(short)DAT_08014e9c[4]);
    FUN_0800a968((int)*(short *)((int)local_3c + *(int *)(iVar4 + 0x1c) * 2),0);
    FUN_0800a9e4((int)*(short *)((int)local_3c + *(int *)(iVar4 + 0x1c) * 2));
    return;
  }
  software_bkpt(0xff);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* === 08014ea0 thunk_FUN_08014834 === */

void thunk_FUN_08014834(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar1 = DAT_08014878;
  puVar3 = DAT_08014878 + 0x3f;
  *DAT_0801487c = 0xff;
  do {
    puVar2 = puVar1 + 7;
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 1;
    puVar1 = puVar2;
  } while (puVar2 != puVar3);
  return;
}



/* === 08014ea4 FUN_08014ea4 === */

void FUN_08014ea4(void)

{
  int iVar1;
  
  iVar1 = DAT_08014ecc;
  FUN_080167e8(DAT_08014ecc + 0x34);
  if ((*(char *)(iVar1 + 0x14) != '\0') && (*(int *)(*(int *)(iVar1 + 0x34) + 0x1c) << 0x1b < 0)) {
    FUN_08014660(iVar1);
    *(undefined4 *)(*(int *)(iVar1 + 0x34) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014ed0 FUN_08014ed0 === */

void FUN_08014ed0(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f00;
  FUN_080167e8(DAT_08014f00 + 0x1ec);
  if ((*(char *)(iVar1 + 0x1cc) != '\0') && (*(int *)(*(int *)(iVar1 + 0x1ec) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x1b8);
    *(undefined4 *)(*(int *)(iVar1 + 0x1ec) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014f04 FUN_08014f04 === */

void FUN_08014f04(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f34;
  FUN_080167e8(DAT_08014f34 + 0x3a4);
  if ((*(char *)(iVar1 + 900) != '\0') && (*(int *)(*(int *)(iVar1 + 0x3a4) + 0x1c) << 0x1b < 0)) {
    FUN_08014660(iVar1 + 0x370);
    *(undefined4 *)(*(int *)(iVar1 + 0x3a4) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014f38 FUN_08014f38 === */

void FUN_08014f38(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f68;
  FUN_080167e8(DAT_08014f68 + 0x55c);
  if ((*(char *)(iVar1 + 0x53c) != '\0') && (*(int *)(*(int *)(iVar1 + 0x55c) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x528);
    *(undefined4 *)(*(int *)(iVar1 + 0x55c) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014f6c FUN_08014f6c === */

void FUN_08014f6c(void)

{
  int iVar1;
  
  iVar1 = DAT_08014f9c;
  FUN_080167e8(DAT_08014f9c + 0x714);
  if ((*(char *)(iVar1 + 0x6f4) != '\0') && (*(int *)(*(int *)(iVar1 + 0x714) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x6e0);
    *(undefined4 *)(*(int *)(iVar1 + 0x714) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014fa0 FUN_08014fa0 === */

void FUN_08014fa0(void)

{
  int iVar1;
  
  iVar1 = DAT_08014fd0;
  FUN_080167e8(DAT_08014fd0 + 0x8cc);
  if ((*(char *)(iVar1 + 0x8ac) != '\0') && (*(int *)(*(int *)(iVar1 + 0x8cc) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0x898);
    *(undefined4 *)(*(int *)(iVar1 + 0x8cc) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08014fd4 FUN_08014fd4 === */

void FUN_08014fd4(void)

{
  int iVar1;
  
  iVar1 = DAT_08015004;
  FUN_080167e8(DAT_08015004 + 0xa84);
  if ((*(char *)(iVar1 + 0xa64) != '\0') && (*(int *)(*(int *)(iVar1 + 0xa84) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xa50);
    *(undefined4 *)(*(int *)(iVar1 + 0xa84) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08015008 FUN_08015008 === */

void FUN_08015008(void)

{
  int iVar1;
  
  iVar1 = DAT_08015038;
  FUN_080167e8(DAT_08015038 + 0xc3c);
  if ((*(char *)(iVar1 + 0xc1c) != '\0') && (*(int *)(*(int *)(iVar1 + 0xc3c) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xc08);
    *(undefined4 *)(*(int *)(iVar1 + 0xc3c) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 0801503c FUN_0801503c === */

void FUN_0801503c(void)

{
  int iVar1;
  
  iVar1 = DAT_0801506c;
  FUN_080167e8(DAT_0801506c + 0xdf4);
  if ((*(char *)(iVar1 + 0xdd4) != '\0') && (*(int *)(*(int *)(iVar1 + 0xdf4) + 0x1c) << 0x1b < 0))
  {
    FUN_08014660(iVar1 + 0xdc0);
    *(undefined4 *)(*(int *)(iVar1 + 0xdf4) + 0x20) = 0x10;
    return;
  }
  return;
}



/* === 08015070 FUN_08015070 === */

void FUN_08015070(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_0801509c << 0x18)) {
    FUN_0800b9d4((char)*DAT_0801509c * 0x1b8 + DAT_080150a0 + 200);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 080150a4 FUN_080150a4 === */

void FUN_080150a4(void)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar2 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  if (-1 < (int)((uint)*DAT_080150d4 << 0x18)) {
    FUN_0800b9d4((char)*DAT_080150d4 * 0x1b8 + DAT_080150d8 + 0x140);
  }
  if (iVar2 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 080150dc FUN_080150dc === */

/* WARNING: Removing unreachable block (ram,0x08014bda) */

void FUN_080150dc(void)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int *piVar4;
  int iVar5;
  code *pcVar6;
  int *piVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  
  pbVar3 = DAT_08014be4;
  ppcVar2 = DAT_08014be0;
  iVar9 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar9 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08014be4 = 0xff;
  pcVar6 = *ppcVar2;
  if (pcVar6 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar6)(*DAT_08014be8,0);
  }
  piVar4 = DAT_08014bec;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar8 = 0;
    piVar7 = DAT_08014bec;
    iVar10 = DAT_08014bf0;
    do {
      if (piVar7[6] == 1) {
        if ((piVar7[1] != 0) &&
           (iVar5 = FUN_08014924(iVar10,piVar7[1],*(undefined2 *)(piVar7 + 2),piVar7[3],piVar7[4],
                                 piVar7[5]), iVar5 == 0)) {
          iVar5 = 0;
          goto LAB_08014ba4;
        }
      }
      else if (((piVar7[6] == 0) && (*piVar7 != 0)) &&
              (iVar5 = FUN_08014a2c(iVar10,*piVar7,*(undefined2 *)(piVar7 + 2),piVar7[3],piVar7[4],
                                    piVar7[5]), iVar5 == 0)) {
LAB_08014ba4:
        piVar4[iVar8 * 7] = iVar5;
        piVar4[iVar8 * 7 + 1] = iVar5;
        break;
      }
      iVar8 = iVar8 + 1;
      piVar7 = piVar7 + 7;
      iVar10 = iVar10 + 0x1b8;
    } while (iVar8 != 9);
  }
  if (iVar9 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 080150e4 FUN_080150e4 === */

/* WARNING: Removing unreachable block (ram,0x08014bda) */

void FUN_080150e4(int *param_1)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int iVar4;
  code *pcVar5;
  int *piVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iStack_40;
  int local_3c [4];
  undefined4 local_2c;
  int iStack_28;
  int iStack_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  pbVar3 = DAT_08014be4;
  ppcVar2 = DAT_08014be0;
  piVar8 = &iStack_40;
  iVar10 = 0;
  local_3c[0] = *DAT_08015148;
  local_3c[1] = DAT_08015148[1];
  local_3c[2] = DAT_08015148[2];
  local_3c[3] = DAT_08015148[3];
  local_2c = DAT_08015148[4];
  iStack_28 = DAT_08015148[5];
  iStack_24 = DAT_08015148[6];
  uStack_20 = DAT_08015148[7];
  local_1c = DAT_08015148[8];
  while (piVar8 = piVar8 + 1, *param_1 != *piVar8) {
    iVar10 = iVar10 + 1;
    if (iVar10 == 9) {
                    /* WARNING: Does not return */
      pcVar5 = (code *)software_udf(0xff,0x8015146);
      (*pcVar5)();
    }
  }
  if (*(char *)(iVar10 * 0x1b8 + DAT_0801514c + 0x14) != '\0') {
    FUN_08014660();
    return;
  }
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  *DAT_08014be4 = 0xff;
  pcVar5 = *ppcVar2;
  if (pcVar5 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar5)(*DAT_08014be8,0);
  }
  piVar8 = DAT_08014bec;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar7 = 0;
    piVar6 = DAT_08014bec;
    iVar9 = DAT_08014bf0;
    do {
      if (piVar6[6] == 1) {
        if (piVar6[1] != 0) {
          iStack_24 = piVar6[5];
          iStack_28 = piVar6[4];
          iVar4 = FUN_08014924(iVar9,piVar6[1],*(undefined2 *)(piVar6 + 2),piVar6[3]);
          if (iVar4 == 0) {
            iVar4 = 0;
            goto LAB_08014ba4;
          }
        }
      }
      else if ((piVar6[6] == 0) && (*piVar6 != 0)) {
        iStack_24 = piVar6[5];
        iStack_28 = piVar6[4];
        iVar4 = FUN_08014a2c(iVar9,*piVar6,*(undefined2 *)(piVar6 + 2),piVar6[3]);
        if (iVar4 == 0) {
LAB_08014ba4:
          piVar8[iVar7 * 7] = iVar4;
          piVar8[iVar7 * 7 + 1] = iVar4;
          break;
        }
      }
      iVar7 = iVar7 + 1;
      piVar6 = piVar6 + 7;
      iVar9 = iVar9 + 0x1b8;
    } while (iVar7 != 9);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08015150 FUN_08015150 === */

void FUN_08015150(int *param_1)

{
  code *pcVar1;
  int *piVar2;
  int iVar3;
  int iStack_38;
  int local_34 [4];
  undefined4 local_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 local_14;
  
  piVar2 = &iStack_38;
  iVar3 = 0;
  local_34[0] = *DAT_080151ac;
  local_34[1] = DAT_080151ac[1];
  local_34[2] = DAT_080151ac[2];
  local_34[3] = DAT_080151ac[3];
  local_24 = DAT_080151ac[4];
  uStack_20 = DAT_080151ac[5];
  uStack_1c = DAT_080151ac[6];
  uStack_18 = DAT_080151ac[7];
  local_14 = DAT_080151ac[8];
  while (piVar2 = piVar2 + 1, *param_1 != *piVar2) {
    iVar3 = iVar3 + 1;
    if (iVar3 == 9) {
                    /* WARNING: Does not return */
      pcVar1 = (code *)software_udf(0xff,0x80151a8);
      (*pcVar1)();
    }
  }
  if (*(char *)(iVar3 * 0x1b8 + DAT_080151b0 + 0x14) == '\0') {
    return;
  }
  FUN_08014660();
  return;
}



/* === 080151b4 FUN_080151b4 === */

void FUN_080151b4(int *param_1)

{
  bool bVar1;
  code **ppcVar2;
  byte *pbVar3;
  int iVar4;
  code *pcVar5;
  int *piVar6;
  int iVar7;
  int *piVar8;
  int iVar9;
  int iVar10;
  int iStack_40;
  int local_3c [4];
  undefined4 local_2c;
  int iStack_28;
  int iStack_24;
  undefined4 uStack_20;
  undefined4 local_1c;
  
  piVar8 = &iStack_40;
  iVar10 = 0;
  local_3c[0] = *DAT_08015214;
  local_3c[1] = DAT_08015214[1];
  local_3c[2] = DAT_08015214[2];
  local_3c[3] = DAT_08015214[3];
  local_2c = DAT_08015214[4];
  iStack_28 = DAT_08015214[5];
  iStack_24 = DAT_08015214[6];
  uStack_20 = DAT_08015214[7];
  local_1c = DAT_08015214[8];
  while (piVar8 = piVar8 + 1, *param_1 != *piVar8) {
    iVar10 = iVar10 + 1;
    if (iVar10 == 9) {
      NotUsed = 0;
                    /* WARNING: Does not return */
      pcVar5 = (code *)software_udf(0xff,0x8015210);
      (*pcVar5)();
    }
  }
  *(undefined *)(iVar10 * 0x1b8 + DAT_08015218 + 0x14) = 0;
  iVar10 = 0;
  bVar1 = (bool)isCurrentModePrivileged();
  if (bVar1) {
    iVar10 = isIRQinterruptsEnabled();
  }
  disableIRQinterrupts();
  FUN_08017328(param_1);
  pbVar3 = DAT_08014be4;
  ppcVar2 = DAT_08014be0;
  *DAT_08014be4 = 0xff;
  pcVar5 = *ppcVar2;
  if (pcVar5 != (code *)0x0) {
    *ppcVar2 = (code *)0x0;
    (*pcVar5)(*DAT_08014be8,1);
  }
  piVar8 = DAT_08014bec;
  if ((int)((uint)*pbVar3 << 0x18) < 0) {
    iVar7 = 0;
    piVar6 = DAT_08014bec;
    iVar9 = DAT_08014bf0;
    do {
      if (piVar6[6] == 1) {
        if (piVar6[1] != 0) {
          iStack_24 = piVar6[5];
          iStack_28 = piVar6[4];
          iVar4 = FUN_08014924(iVar9,piVar6[1],*(undefined2 *)(piVar6 + 2),piVar6[3]);
          if (iVar4 == 0) {
            iVar4 = 0;
            goto LAB_08014ba4;
          }
        }
      }
      else if ((piVar6[6] == 0) && (*piVar6 != 0)) {
        iStack_24 = piVar6[5];
        iStack_28 = piVar6[4];
        iVar4 = FUN_08014a2c(iVar9,*piVar6,*(undefined2 *)(piVar6 + 2),piVar6[3]);
        if (iVar4 == 0) {
LAB_08014ba4:
          piVar8[iVar7 * 7] = iVar4;
          piVar8[iVar7 * 7 + 1] = iVar4;
          break;
        }
      }
      iVar7 = iVar7 + 1;
      piVar6 = piVar6 + 7;
      iVar9 = iVar9 + 0x1b8;
    } while (iVar7 != 9);
  }
  if (iVar10 == 0) {
    enableIRQinterrupts();
  }
  return;
}



/* === 08015370 FUN_08015370 === */

void FUN_08015370(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_0801546c;
  *(uint *)(DAT_0801546c + 0xd8) = *(uint *)(DAT_0801546c + 0xd8) | 1;
  uVar2 = *(uint *)(iVar1 + 0xd8);
  *(uint *)(iVar1 + 0xd8) = *(uint *)(iVar1 + 0xd8) | 2;
  FUN_0800a968(0xb,0,0,*(uint *)(iVar1 + 0xd8) & 2,uVar2 & 1);
  FUN_0800a9e4(0xb);
  FUN_0800a968(0xc,0);
  FUN_0800a9e4(0xc);
  FUN_0800a968(0xd,0);
  FUN_0800a9e4(0xd);
  FUN_0800a968(0xe,0);
  FUN_0800a9e4(0xe);
  FUN_0800a968(0xf,0);
  FUN_0800a9e4(0xf);
  FUN_0800a968(0x10,0);
  FUN_0800a9e4(0x10);
  FUN_0800a968(0x3c,0);
  FUN_0800a9e4(0x3c);
  FUN_0800a968(0x11,0);
  FUN_0800a9e4(0x11);
  FUN_0800a968(0x38,0);
  FUN_0800a9e4(0x38);
  FUN_0800a968(0x39,0);
  FUN_0800a9e4(0x39);
  FUN_0800a968(0x3a,0);
  FUN_0800a9e4(0x3a);
  FUN_0800a968(0x3b,0);
  FUN_0800a9e4(0x3b);
  return;
}



/* === 08015470 FUN_08015470 === */

void FUN_08015470(uint param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = DAT_08015498;
  if (0 < param_2 + 0x20) {
    param_1 = param_1 & 0xffffffe0;
    DataSynchronizationBarrier(0xf);
    iVar2 = param_2 + 0x20 + param_1;
    do {
      *(uint *)(iVar1 + 0x25c) = param_1;
      param_1 = param_1 + 0x20;
    } while (0 < (int)(iVar2 - param_1));
    DataSynchronizationBarrier(0xf);
    InstructionSynchronizationBarrier(0xf);
  }
  return;
}



/* === 0801549c FUN_0801549c === */

undefined4 FUN_0801549c(int *param_1)

{
  if (param_1 == (int *)0x0) {
    return 1;
  }
  if (*(char *)((int)param_1 + 0x3d) != '\x02') {
    param_1[0x1a] = 0x80;
    return 1;
  }
  *(undefined *)((int)param_1 + 0x3d) = 4;
  *(uint *)(*param_1 + 0xc) = *(uint *)(*param_1 + 0xc) & 0xfffffffe;
  return 0;
}



/* === 080154c8 FUN_080154c8 === */

void FUN_080154c8(uint **param_1)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  uVar1 = DAT_0801557c;
  puVar3 = *param_1;
  uVar2 = puVar3[5];
  puVar3[6] = puVar3[6] | 8;
  puVar3[6] = puVar3[6] | 0x10;
  *puVar3 = *puVar3 & 0xfffffffe;
  puVar3[4] = uVar1 & puVar3[4];
  puVar3[2] = puVar3[2] & 0xffff3fff;
  if ((*(char *)((int)param_1 + 0x81) != '\x04') && ((int)(uVar2 << 0x1a) < 0)) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x80);
    puVar3[6] = puVar3[6] | 0x20;
  }
  if ((*(char *)((int)param_1 + 0x81) != '\x03') && ((int)(uVar2 << 0x19) < 0)) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 4);
    puVar3[6] = puVar3[6] | 0x40;
  }
  if ((int)(uVar2 << 0x16) < 0) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 1);
    puVar3[6] = puVar3[6] | 0x200;
  }
  if ((int)(uVar2 << 0x17) < 0) {
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 8);
    puVar3[6] = puVar3[6] | 0x100;
  }
  *(undefined2 *)((int)param_1 + 0x62) = 0;
  *(undefined2 *)((int)param_1 + 0x6a) = 0;
  return;
}



/* === 08015580 FUN_08015580 === */

undefined4 FUN_08015580(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  uint *puVar3;
  uint *puVar4;
  uint *puVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  uint *puVar9;
  uint *puVar10;
  uint *puVar11;
  uint uVar12;
  uint *puVar13;
  uint *puVar14;
  uint *puVar15;
  uint *puVar16;
  uint *puVar17;
  
  puVar13 = DAT_0801572c;
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  puVar3 = *param_1;
  param_1[10] = (uint *)0x0;
  if ((puVar3 == puVar13) || (puVar3 == puVar13 + -0x3e00)) {
    puVar2 = param_1[3];
    puVar13 = param_1[0xf];
    uVar12 = ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3);
    if ((puVar3 == DAT_08015730 || puVar3 == DAT_0801572c) || (puVar3 == DAT_08015730 + 0x100))
    goto LAB_080155ec;
  }
  else {
    puVar2 = param_1[3];
    if (puVar3 == puVar13 + -0x3d00) {
      puVar13 = param_1[0xf];
      uVar12 = ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3);
LAB_080155ec:
      if (0x10 < uVar12) {
        return 1;
      }
    }
    else {
      if ((uint *)0xf < puVar2) {
        return 1;
      }
      puVar13 = param_1[0xf];
      if (8 < ((uint)puVar13 >> 5) * ((uint)(puVar2 + 2) >> 3) + ((uint)(puVar2 + 2) >> 3)) {
        return 1;
      }
    }
  }
  if (*(char *)((int)param_1 + 0x81) == '\0') {
    *(undefined *)(param_1 + 0x20) = 0;
    FUN_08013f2c(param_1);
    puVar3 = *param_1;
    puVar16 = param_1[10];
    puVar2 = param_1[3];
    puVar13 = param_1[0xf];
  }
  else {
    puVar16 = (uint *)0x0;
  }
  puVar15 = param_1[6];
  puVar1 = param_1[1];
  *(undefined *)((int)param_1 + 0x81) = 2;
  puVar14 = param_1[0xe];
  *puVar3 = *puVar3 & 0xfffffffe;
  if (puVar15 == (uint *)0x4000000) {
    puVar17 = puVar1;
    if (puVar1 == (uint *)0x400000) {
      if (puVar14 == (uint *)0x0) {
LAB_080156f2:
        *puVar3 = *puVar3 | 0x1000;
        goto LAB_08015622;
      }
      goto LAB_08015628;
    }
    if (puVar1 != (uint *)0x0) goto LAB_08015622;
    if (puVar14 == (uint *)0x10000000) goto LAB_080156f2;
  }
  else {
LAB_08015622:
    puVar17 = (uint *)((uint)puVar1 & 0x400000);
    if (puVar17 != (uint *)0x0) {
LAB_08015628:
      if ((uint *)0x6 < puVar2) {
        *puVar3 = *puVar3 & 0xfffffeff | (uint)param_1[0x14];
        goto LAB_08015634;
      }
    }
  }
  *puVar3 = *puVar3 & 0xfffffeff;
LAB_08015634:
  puVar4 = param_1[0xd];
  puVar5 = param_1[9];
  puVar6 = param_1[4];
  puVar7 = param_1[5];
  puVar8 = param_1[8];
  puVar9 = param_1[0x13];
  puVar10 = param_1[2];
  puVar11 = param_1[0x12];
  puVar3[2] = (uint)puVar16 | (uint)param_1[7] | (uint)puVar13 | puVar3[2] & 0x1f0000 | (uint)puVar2
  ;
  puVar3[3] = (uint)puVar14 |
              (uint)puVar15 | (uint)puVar4 | (uint)puVar5 | (uint)puVar6 | (uint)puVar7 |
              (uint)puVar8 | (uint)puVar9 | (uint)puVar10 | (uint)puVar11 | (uint)param_1[0x16] |
              (uint)puVar1;
  if (puVar1 == (uint *)0x0) {
    puVar3[2] = puVar3[2] & 0xffffe7ff | 0x800;
    puVar3[2] = puVar3[2] & 0xfffff9ff | 0x400;
    puVar3[0x14] = puVar3[0x14] & 0xfffffffe;
  }
  else {
    puVar3[0x14] = puVar3[0x14] & 0xfffffffe;
    if (puVar17 != (uint *)0x0) {
      puVar3[3] = puVar3[3] & 0x7fffffff | (uint)param_1[0x15];
    }
  }
  param_1[0x21] = (uint *)0x0;
  *(undefined *)((int)param_1 + 0x81) = 1;
  return 0;
}



/* === 08015734 FUN_08015734 === */

undefined FUN_08015734(uint **param_1,uint *param_2,uint param_3)

{
  ushort uVar1;
  int iVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  if (param_2 == (uint *)0x0) {
LAB_08015822:
    *(undefined *)(param_1 + 0x20) = 0;
    return 1;
  }
  puVar4 = (uint *)(uint)(param_3 == 0);
  if (param_3 == 0) goto LAB_08015822;
  param_1[0x19] = puVar4;
  uVar1 = (ushort)(param_3 == 0);
  *(ushort *)(param_1 + 0x1a) = uVar1;
  *(undefined *)((int)param_1 + 0x81) = 3;
  param_1[0x21] = puVar4;
  *(short *)((int)param_1 + 0x62) = (short)param_3;
  *(ushort *)((int)param_1 + 0x6a) = uVar1;
  puVar3 = *param_1;
  param_1[0x17] = param_2;
  *(short *)(param_1 + 0x18) = (short)param_3;
  param_1[0x1c] = puVar4;
  param_1[0x1d] = puVar4;
  if (param_1[2] == (uint *)0x60000) {
    *puVar3 = *puVar3 | 0x800;
  }
  else {
    puVar3[3] = puVar3[3] & 0xfff9ffff | 0x20000;
  }
  puVar4 = param_1[0x1e];
  if (&DataAbort <= param_1[3]) {
    if (puVar4[6] == 0x4000) goto LAB_080157aa;
    goto LAB_08015822;
  }
  uVar5 = puVar4[6];
  if (param_1[3] < (uint *)0x8) {
    if (uVar5 != 0x2000) {
      if (uVar5 == 0x4000) {
        *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 3 >> 2);
      }
      goto LAB_080157aa;
    }
  }
  else if (uVar5 != 0x4000) {
    if (uVar5 != 0x2000) goto LAB_08015822;
    goto LAB_080157aa;
  }
  *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
LAB_080157aa:
  uVar5 = puVar3[2];
  puVar4[0x10] = DAT_080158a4;
  puVar4[0xf] = DAT_080158a8;
  puVar4[0x13] = DAT_080158ac;
  puVar4[0x14] = 0;
  puVar3[2] = uVar5 & 0xffff7fff;
  iVar2 = FUN_0800b3a0();
  if (iVar2 != 0) {
    *(undefined *)(param_1 + 0x20) = 0;
    param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
    *(undefined *)((int)param_1 + 0x81) = 1;
    return 1;
  }
  puVar4 = *param_1;
  uVar5 = DAT_080158b0 & puVar4[1];
  if (param_1[0x1e][7] != 0x100) {
    uVar5 = uVar5 | param_3;
  }
  puVar4[1] = uVar5;
  puVar3 = param_1[1];
  puVar4[2] = puVar4[2] | 0x8000;
  puVar4[4] = puVar4[4] | 800;
  *puVar4 = *puVar4 | 1;
  if (puVar3 == (uint *)0x400000) {
    *puVar4 = *puVar4 | 0x200;
  }
  *(undefined *)(param_1 + 0x20) = 0;
  return 0;
}



/* === 080158b4 FUN_080158b4 === */

undefined FUN_080158b4(uint **param_1,uint *param_2,uint param_3,undefined4 param_4)

{
  undefined2 uVar1;
  ushort uVar2;
  uint *puVar3;
  int iVar4;
  uint uVar5;
  uint *puVar6;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  if (param_2 != (uint *)0x0) {
    puVar6 = (uint *)(uint)(param_3 == 0);
    if (param_3 != 0) {
      uVar2 = (ushort)(param_3 == 0);
      *(ushort *)(param_1 + 0x18) = uVar2;
      param_1[0x19] = param_2;
      *(undefined *)((int)param_1 + 0x81) = 4;
      param_1[0x21] = puVar6;
      *(short *)((int)param_1 + 0x6a) = (short)param_3;
      *(ushort *)((int)param_1 + 0x62) = uVar2;
      *(short *)(param_1 + 0x1a) = (short)param_3;
      param_1[0x1c] = puVar6;
      param_1[0x1d] = puVar6;
      puVar6 = *param_1;
      if (param_1[2] == (uint *)0x60000) {
        *puVar6 = *puVar6 & 0xfffff7ff;
      }
      else {
        puVar6[3] = puVar6[3] & 0xfff9ffff | 0x40000;
      }
      if (param_1[3] < &DataAbort) {
        if (param_1[3] < (uint *)0x8) {
          puVar3 = param_1[0x1f];
          puVar6[2] = puVar6[2] & 0xffffbfff;
          if (puVar3[6] != 0x2000) {
            if (puVar3[6] == 0x4000) {
              *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 3 >> 2);
            }
            goto LAB_08015930;
          }
        }
        else {
          puVar3 = param_1[0x1f];
          uVar5 = puVar3[6];
          if ((uVar5 != 0x4000) && (uVar5 != 0x2000)) goto LAB_080159a0;
          puVar6[2] = puVar6[2] & 0xffffbfff;
          if (uVar5 != 0x4000) goto LAB_08015930;
        }
        *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 1 >> 1);
      }
      else {
        puVar3 = param_1[0x1f];
        if (puVar3[6] != 0x4000) goto LAB_080159a0;
        puVar6[2] = puVar6[2] & 0xffffbfff;
      }
LAB_08015930:
      uVar1 = *(undefined2 *)((int)param_1 + 0x6a);
      puVar3[0x10] = DAT_08015a38;
      puVar3[0xf] = DAT_08015a3c;
      puVar3[0x13] = DAT_08015a40;
      puVar3[0x14] = 0;
      iVar4 = FUN_0800b3a0(puVar3,puVar6 + 0xc,param_2,uVar1,param_4);
      if (iVar4 != 0) {
        *(undefined *)(param_1 + 0x20) = 0;
        param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
        *(undefined *)((int)param_1 + 0x81) = 1;
        return 1;
      }
      puVar6 = *param_1;
      uVar5 = DAT_08015a44 & puVar6[1];
      if (param_1[0x1f][7] != 0x100) {
        uVar5 = uVar5 | param_3;
      }
      puVar6[1] = uVar5;
      puVar3 = param_1[1];
      puVar6[2] = puVar6[2] | 0x4000;
      puVar6[4] = puVar6[4] | 0x340;
      *puVar6 = *puVar6 | 1;
      if (puVar3 == (uint *)0x400000) {
        *puVar6 = *puVar6 | 0x200;
      }
      *(undefined *)(param_1 + 0x20) = 0;
      return 0;
    }
  }
LAB_080159a0:
  *(undefined *)(param_1 + 0x20) = 0;
  return 1;
}



/* === 08015a48 FUN_08015a48 === */

undefined FUN_08015a48(uint **param_1,uint *param_2,uint *param_3,uint param_4)

{
  undefined2 uVar1;
  uint uVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  uint *puVar7;
  undefined uVar8;
  
  if (*(char *)(param_1 + 0x20) == '\x01') {
    return 2;
  }
  *(undefined *)(param_1 + 0x20) = 1;
  if (*(char *)((int)param_1 + 0x81) != '\x01') {
    *(undefined *)(param_1 + 0x20) = 0;
    return 2;
  }
  uVar8 = 1;
  if ((param_4 == 0 || param_3 == (uint *)0x0) ||
     (puVar7 = (uint *)(uint)(param_2 == (uint *)0x0), param_2 == (uint *)0x0)) {
LAB_08015b02:
    *(undefined *)(param_1 + 0x20) = 0;
  }
  else {
    param_1[0x17] = param_2;
    uVar1 = (undefined2)param_4;
    *(undefined2 *)(param_1 + 0x18) = uVar1;
    puVar4 = *param_1;
    param_1[0x19] = param_3;
    *(undefined2 *)(param_1 + 0x1a) = uVar1;
    param_1[0x1c] = puVar7;
    param_1[0x1d] = puVar7;
    *(undefined *)((int)param_1 + 0x81) = 5;
    param_1[0x21] = puVar7;
    *(undefined2 *)((int)param_1 + 0x62) = uVar1;
    *(undefined2 *)((int)param_1 + 0x6a) = uVar1;
    puVar4[3] = puVar4[3] & 0xfff9ffff;
    puVar4[2] = puVar4[2] & 0xffff3fff;
    if (param_1[3] < &DataAbort) {
      if (param_1[3] < (uint *)0x8) {
        if (param_1[0x1e][6] == 0x2000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
        }
        else if (param_1[0x1e][6] == 0x4000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 3 >> 2);
        }
        puVar7 = param_1[0x1f];
        if (puVar7[6] == 0x2000) {
LAB_08015c0c:
          *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 1 >> 1);
        }
        else if (puVar7[6] == 0x4000) {
          *(short *)((int)param_1 + 0x6a) = (short)(*(ushort *)((int)param_1 + 0x6a) + 3 >> 2);
        }
      }
      else {
        puVar7 = param_1[0x1f];
        uVar6 = puVar7[6];
        if ((uVar6 != 0x4000) && (uVar6 != 0x2000)) goto LAB_08015b02;
        if (param_1[0x1e][6] == 0x4000) {
          *(short *)((int)param_1 + 0x62) = (short)(*(ushort *)((int)param_1 + 0x62) + 1 >> 1);
        }
        if (uVar6 == 0x4000) goto LAB_08015c0c;
      }
    }
    else {
      puVar7 = param_1[0x1f];
      if (puVar7[6] != 0x4000) goto LAB_08015b02;
    }
    uVar1 = *(undefined2 *)((int)param_1 + 0x6a);
    puVar7[0x10] = DAT_08015c4c;
    uVar2 = DAT_08015c58;
    uVar6 = DAT_08015c50;
    puVar7[0x13] = DAT_08015c58;
    puVar7[0xf] = uVar6;
    puVar7[0x14] = 0;
    uVar6 = param_4;
    iVar3 = FUN_0800b3a0(puVar7,puVar4 + 0xc,param_3,uVar1,param_4);
    if (iVar3 == 0) {
      puVar5 = *param_1;
      puVar7 = param_1[0x1e];
      puVar4 = param_1[0x17];
      puVar5[2] = puVar5[2] | 0x4000;
      uVar1 = *(undefined2 *)((int)param_1 + 0x62);
      puVar7[0x13] = uVar2;
      puVar7[0x14] = 0;
      puVar7[0xf] = 0;
      puVar7[0x10] = 0;
      iVar3 = FUN_0800b3a0(puVar7,puVar4,puVar5 + 8,uVar1,uVar6);
      if (iVar3 == 0) {
        puVar7 = *param_1;
        uVar6 = DAT_08015c54 & puVar7[1];
        if (param_1[0x1e][7] != 0x100) {
          uVar6 = uVar6 | param_4;
        }
        puVar7[1] = uVar6;
        puVar4 = param_1[1];
        puVar7[2] = puVar7[2] | 0x8000;
        puVar7[4] = puVar7[4] | 0x360;
        *puVar7 = *puVar7 | 1;
        if (puVar4 == (uint *)0x400000) {
          *puVar7 = *puVar7 | 0x200;
        }
        uVar8 = 0;
        *(undefined *)(param_1 + 0x20) = 0;
      }
      else {
        FUN_0800b550(param_1[0x1f]);
        *(undefined *)(param_1 + 0x20) = 0;
        param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
        *(undefined *)((int)param_1 + 0x81) = 1;
      }
    }
    else {
      *(undefined *)(param_1 + 0x20) = 0;
      param_1[0x21] = (uint *)((uint)param_1[0x21] | 0x10);
      *(undefined *)((int)param_1 + 0x81) = 1;
    }
  }
  return uVar8;
}



/* === 08015cd4 FUN_08015cd4 === */

void FUN_08015cd4(void)

{
  return;
}



/* === 08015ce4 FUN_08015ce4 === */

void FUN_08015ce4(void)

{
  return;
}



/* === 08015cf4 FUN_08015cf4 === */

void FUN_08015cf4(void)

{
  return;
}



/* === 08015d4c FUN_08015d4c === */

void FUN_08015d4c(void)

{
  return;
}



/* === 08015f64 FUN_08015f64 === */

undefined FUN_08015f64(int param_1)

{
  return *(undefined *)(param_1 + 0x81);
}



/* === 08015f6c FUN_08015f6c === */

bool FUN_08015f6c(uint **param_1)

{
  uint *puVar1;
  bool bVar2;
  uint *puVar3;
  bool bVar4;
  
  puVar1 = DAT_08016008;
  if (*(char *)((int)param_1 + 0x3d) != '\x01') {
    return true;
  }
  puVar3 = *param_1;
  bVar2 = puVar3 == DAT_08016004;
  bVar4 = puVar3 == DAT_0801600c;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  if ((puVar3 == puVar1 + 0x400 ||
       (puVar3 == DAT_08016014 ||
       (puVar3 == DAT_08016010 ||
       (puVar3 == puVar1 || (bVar4 || (puVar3 == (uint *)0x40000000 || bVar2)))))) ||
     (bVar2 = puVar3 == DAT_08016014 + 0xf00, bVar2)) {
    if (((DAT_08016018 & puVar3[2]) == 6) || ((DAT_08016018 & puVar3[2]) == 0x10000)) {
      return false;
    }
    bVar2 = false;
    *puVar3 = *puVar3 | 1;
  }
  else {
    *puVar3 = *puVar3 | 1;
  }
  return bVar2;
}



/* === 0801601c FUN_0801601c === */

bool FUN_0801601c(uint **param_1)

{
  uint *puVar1;
  uint *puVar2;
  bool bVar3;
  uint *puVar4;
  bool bVar5;
  
  puVar1 = DAT_080160c4;
  if (*(char *)((int)param_1 + 0x3d) != '\x01') {
    return true;
  }
  puVar4 = *param_1;
  bVar3 = puVar4 == DAT_080160bc;
  bVar5 = puVar4 == DAT_080160c0;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  puVar2 = DAT_080160c8;
  puVar4[3] = puVar4[3] | 1;
  if ((puVar4 == DAT_080160cc ||
       (puVar4 == puVar2 ||
       (puVar4 == puVar1 + 0x100 ||
       (puVar4 == puVar1 || (bVar5 || (puVar4 == (uint *)0x40000000 || bVar3)))))) ||
     (bVar3 = puVar4 == puVar2 + 0xf00, bVar3)) {
    if (((DAT_080160d0 & puVar4[2]) == 6) || ((DAT_080160d0 & puVar4[2]) == 0x10000)) {
      return false;
    }
    bVar3 = false;
    *puVar4 = *puVar4 | 1;
  }
  else {
    *puVar4 = *puVar4 | 1;
  }
  return bVar3;
}



/* === 080160d4 FUN_080160d4 === */

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



/* === 0801626c FUN_0801626c === */

void FUN_0801626c(void)

{
  return;
}



/* === 08016270 FUN_08016270 === */

void FUN_08016270(void)

{
  return;
}



/* === 08016274 FUN_08016274 === */

void FUN_08016274(void)

{
  return;
}



/* === 08016278 FUN_08016278 === */

void FUN_08016278(void)

{
  return;
}



/* === 080163d0 FUN_080163d0 === */

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



/* === 080164e8 FUN_080164e8 === */

undefined4 FUN_080164e8(undefined4 *param_1)

{
  if (param_1 != (undefined4 *)0x0) {
    if (*(char *)((int)param_1 + 0x3d) == '\0') {
      *(undefined *)(param_1 + 0xf) = 0;
      FUN_080143d4();
    }
    *(undefined *)((int)param_1 + 0x3d) = 2;
    FUN_080163d0(*param_1,param_1 + 1);
    *(undefined *)(param_1 + 0x12) = 1;
    *(undefined *)((int)param_1 + 0x3e) = 1;
    *(undefined *)((int)param_1 + 0x3f) = 1;
    *(undefined *)(param_1 + 0x10) = 1;
    *(undefined *)((int)param_1 + 0x41) = 1;
    *(undefined *)((int)param_1 + 0x42) = 1;
    *(undefined *)((int)param_1 + 0x43) = 1;
    *(undefined *)(param_1 + 0x11) = 1;
    *(undefined *)((int)param_1 + 0x45) = 1;
    *(undefined *)((int)param_1 + 0x46) = 1;
    *(undefined *)((int)param_1 + 0x47) = 1;
    *(undefined *)((int)param_1 + 0x3d) = 1;
    return 0;
  }
  return 1;
}



/* === 0801654c FUN_0801654c === */

undefined4 FUN_0801654c(int *param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  iVar3 = DAT_080165f0;
  if (*(char *)(param_1 + 0xf) == '\x01') {
    return 2;
  }
  iVar2 = *param_1;
  *(undefined *)((int)param_1 + 0x3d) = 2;
  iVar1 = DAT_080165f8;
  if ((iVar2 == iVar3) || (iVar2 == iVar3 + 0x400)) {
    *(uint *)(iVar2 + 4) = (*(uint *)(iVar2 + 4) & 0xff0fffff | param_2[1]) & 0xffffff8f | *param_2;
  }
  else {
    bVar5 = iVar2 != DAT_080165f4;
    iVar3 = DAT_080165f4 + 0x400;
    iVar4 = DAT_080165f4 + 0x800;
    *(uint *)(iVar2 + 4) = *(uint *)(iVar2 + 4) & 0xffffff8f | *param_2;
    if ((iVar2 != iVar1 && (iVar2 != iVar4 && (iVar2 != iVar3 && (bVar5 && iVar2 != 0x40000000))))
       && (iVar2 != DAT_080165fc)) goto LAB_080165da;
  }
  *(uint *)(iVar2 + 8) = *(uint *)(iVar2 + 8) & 0xffffff7f | param_2[2];
LAB_080165da:
  *(undefined *)((int)param_1 + 0x3d) = 1;
  *(undefined *)(param_1 + 0xf) = 0;
  return 0;
}



/* === 08016604 FUN_08016604 === */

void FUN_08016604(void)

{
  return;
}



/* === 08016608 FUN_08016608 === */

void FUN_08016608(void)

{
  return;
}



/* === 0801660c FUN_0801660c === */

void FUN_0801660c(uint **param_1)

{
  bool bVar1;
  uint *puVar2;
  
  puVar2 = *param_1;
  do {
    ExclusiveAccess(puVar2);
    bVar1 = (bool)hasExclusiveAccess(puVar2);
  } while (!bVar1);
  *puVar2 = *puVar2 & 0xfffffedf;
  do {
    ExclusiveAccess(puVar2 + 2);
    bVar1 = (bool)hasExclusiveAccess(puVar2 + 2);
  } while (!bVar1);
  puVar2[2] = puVar2[2] & DAT_08016674;
  if (param_1[0x1b] == (uint *)0x1) {
    do {
      ExclusiveAccess(puVar2);
      bVar1 = (bool)hasExclusiveAccess(puVar2);
      if (bVar1) {
        *puVar2 = *puVar2 & 0xffffffef;
        goto LAB_0801663e;
      }
      ExclusiveAccess(puVar2);
      bVar1 = (bool)hasExclusiveAccess(puVar2);
    } while (!bVar1);
    *puVar2 = *puVar2 & 0xffffffef;
  }
LAB_0801663e:
  param_1[0x23] = (uint *)0x20;
  param_1[0x1d] = (uint *)0x0;
  param_1[0x1b] = (uint *)0x0;
  return;
}



/* === 08016678 FUN_08016678 === */

undefined4 FUN_08016678(int *param_1,int param_2,int param_3)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  
  if (param_1[0x22] != 0x20) {
    return 2;
  }
  if ((param_2 != 0) && (param_3 != 0)) {
    *(short *)((int)param_1 + 0x56) = (short)param_3;
    param_1[0x24] = (uint)(param_3 == 0);
    param_1[0x14] = param_2;
    *(short *)(param_1 + 0x15) = (short)param_3;
    param_1[0x22] = 0x21;
    iVar2 = param_1[0x1f];
    if (iVar2 != 0) {
      *(uint *)(iVar2 + 0x50) = (uint)(param_3 == 0);
      iVar3 = *param_1;
      *(undefined4 *)(iVar2 + 0x3c) = DAT_080166f8;
      *(undefined4 *)(iVar2 + 0x40) = DAT_080166fc;
      *(undefined4 *)(iVar2 + 0x4c) = DAT_08016700;
      iVar2 = FUN_0800b3a0(iVar2,param_2,iVar3 + 0x28,param_3);
      if (iVar2 != 0) {
        param_1[0x24] = 0x10;
        param_1[0x22] = 0x20;
        return 1;
      }
    }
    iVar2 = *param_1;
    *(undefined4 *)(iVar2 + 0x20) = 0x40;
    do {
      ExclusiveAccess((uint *)(iVar2 + 8));
      bVar1 = (bool)hasExclusiveAccess((uint *)(iVar2 + 8));
    } while (!bVar1);
    *(uint *)(iVar2 + 8) = *(uint *)(iVar2 + 8) | 0x80;
    return 0;
  }
  return 1;
}



/* === 08016748 FUN_08016748 === */

void FUN_08016748(void)

{
  return;
}



/* === 080167e4 FUN_080167e4 === */

void FUN_080167e4(void)

{
  return;
}



/* === 080167e8 FUN_080167e8 === */

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



/* === 08016bcc FUN_08016bcc === */

uint FUN_08016bcc(int param_1)

{
  return *(uint *)(param_1 + 0x8c) | *(uint *)(param_1 + 0x88);
}



/* === 08016bd8 FUN_08016bd8 === */

undefined4 FUN_08016bd8(uint **param_1)

{
  byte bVar1;
  char cVar2;
  int *piVar3;
  uint *puVar4;
  undefined4 uVar5;
  uint *puVar6;
  uint *puVar7;
  uint *puVar8;
  uint uVar9;
  bool bVar10;
  undefined8 uVar11;
  undefined auStack_28 [4];
  uint local_24;
  undefined auStack_1c [4];
  uint local_18;
  
  puVar8 = *param_1;
  puVar4 = param_1[7];
  puVar6 = param_1[3];
  bVar10 = puVar8 != DAT_08016ee8;
  *puVar8 = (uint)param_1[2] | (uint)param_1[4] | (uint)param_1[5] | (uint)puVar4 |
            DAT_08016ee4 & *puVar8;
  puVar7 = param_1[6];
  puVar8[1] = puVar8[1] & 0xffffcfff | (uint)puVar6;
  piVar3 = DAT_08016f10;
  if (bVar10) {
    puVar6 = param_1[9];
    puVar8[2] = (uint)puVar7 | (uint)param_1[8] | DAT_08016eec & puVar8[2];
    puVar8[0xb] = puVar8[0xb] & 0xfffffff0 | (uint)puVar6;
    if (puVar8 == DAT_08016ef0) {
      if ((DAT_08016f10[0x15] & 0x38U) < 0x29) {
        cVar2 = *(char *)(DAT_08016f14 + (DAT_08016f10[0x15] & 0x38U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016ef4) {
      if ((DAT_08016f10[0x15] & 7U) < 6) {
        cVar2 = *(char *)(DAT_08016f18 + (DAT_08016f10[0x15] & 7U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016ef8) {
      if ((DAT_080170f4[0x15] & 7U) < 6) {
        cVar2 = *(char *)(DAT_080170f8 + (DAT_080170f4[0x15] & 7U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016efc) {
      if ((DAT_080170f4[0x15] & 7U) < 6) {
        cVar2 = *(char *)(DAT_08017100 + (DAT_080170f4[0x15] & 7U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016f00) {
      if ((DAT_08016f10[0x15] & 7U) < 6) {
        cVar2 = *(char *)(DAT_08016f20 + (DAT_08016f10[0x15] & 7U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016f04) {
      if ((DAT_080170f4[0x15] & 0x38U) < 0x29) {
        cVar2 = *(char *)(DAT_08017104 + (DAT_080170f4[0x15] & 0x38U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (puVar8 == DAT_08016f08) {
      if ((DAT_080170f4[0x15] & 7U) < 6) {
        cVar2 = *(char *)(DAT_08017114 + (DAT_080170f4[0x15] & 7U));
        goto joined_r0x08016d38;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if ((puVar8 != DAT_08016f0c) || (5 < (DAT_080170f4[0x15] & 7U))) goto switchD_08016c9a_caseD_2;
    cVar2 = *(char *)(DAT_0801710c + (DAT_080170f4[0x15] & 7U));
joined_r0x08016d38:
    if (puVar4 == (uint *)0x8000) {
      switch(cVar2) {
      case '\0':
        local_18 = FUN_08010408();
        break;
      case '\x01':
        local_18 = FUN_0801042c();
        break;
      case '\x02':
      case '\x03':
      case '\x05':
      case '\x06':
      case '\a':
      case '\t':
      case '\n':
      case '\v':
      case '\f':
      case '\r':
      case '\x0e':
      case '\x0f':
      case '\x11':
      case '\x12':
      case '\x13':
      case '\x14':
      case '\x15':
      case '\x16':
      case '\x17':
      case '\x18':
      case '\x19':
      case '\x1a':
      case '\x1b':
      case '\x1c':
      case '\x1d':
      case '\x1e':
      case '\x1f':
        goto switchD_08016c9a_caseD_2;
      case '\x04':
        FUN_08011254(auStack_28);
        local_18 = local_24;
        break;
      case '\b':
        FUN_080113d0(auStack_1c);
        break;
      case '\x10':
        local_18 = DAT_08017108;
        if (*DAT_080170f4 << 0x1a < 0) {
          local_18 = DAT_08017108 >> ((uint)(*DAT_080170f4 << 0x1b) >> 0x1e);
        }
        goto LAB_08016fd2;
      case ' ':
        local_18 = DAT_08017110;
        goto LAB_08016fd2;
      default:
        local_18 = 0x8000;
        if (cVar2 == '@') goto LAB_08016fd2;
        goto switchD_08016c9a_caseD_2;
      }
      if (local_18 != 0) {
        puVar6 = param_1[9];
LAB_08016fd2:
        uVar9 = (((uint)param_1[1] >> 1) +
                (local_18 / *(ushort *)(DAT_080170f0 + (int)puVar6 * 2)) * 2) / (uint)param_1[1];
        if (uVar9 - 0x10 < 0xfff0) {
          uVar5 = 0;
          (*param_1)[3] = uVar9 & 0xfff0 | (uVar9 << 0x1c) >> 0x1d;
          goto LAB_08016c68;
        }
        goto switchD_08016c9a_caseD_2;
      }
      goto LAB_08016ec4;
    }
    switch(cVar2) {
    case '\0':
      local_18 = FUN_08010408();
      break;
    case '\x01':
      local_18 = FUN_0801042c();
      break;
    case '\x02':
    case '\x03':
    case '\x05':
    case '\x06':
    case '\a':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case '\x0e':
    case '\x0f':
    case '\x11':
    case '\x12':
    case '\x13':
    case '\x14':
    case '\x15':
    case '\x16':
    case '\x17':
    case '\x18':
    case '\x19':
    case '\x1a':
    case '\x1b':
    case '\x1c':
    case '\x1d':
    case '\x1e':
    case '\x1f':
      goto switchD_08016c9a_caseD_2;
    case '\x04':
      FUN_08011254(auStack_28);
      local_18 = local_24;
      if (local_24 == 0) goto LAB_08016ec4;
      goto LAB_08016ed4;
    case '\b':
      FUN_080113d0(auStack_1c);
      break;
    case '\x10':
      local_18 = DAT_08017108;
      if (*DAT_08016f10 << 0x1a < 0) {
        local_18 = DAT_08016f24 >> ((uint)(*DAT_08016f10 << 0x1b) >> 0x1e);
      }
      goto LAB_08016f32;
    case ' ':
      local_18 = DAT_08017110;
      goto LAB_08016f32;
    default:
      if (cVar2 == '@') {
        local_18 = 0x8000;
        goto LAB_08016f32;
      }
      goto switchD_08016c9a_caseD_2;
    }
    if (local_18 != 0) {
LAB_08016ed4:
      puVar6 = param_1[9];
LAB_08016f32:
      uVar9 = (local_18 / *(ushort *)(DAT_080170f0 + (int)puVar6 * 2) + ((uint)param_1[1] >> 1)) /
              (uint)param_1[1];
      if (uVar9 - 0x10 < 0xfff0) goto LAB_08016f54;
      goto switchD_08016c9a_caseD_2;
    }
    goto LAB_08016ec4;
  }
  puVar8[2] = DAT_08016eec & puVar8[2] | (uint)puVar7;
  puVar8[0xb] = puVar8[0xb] & 0xfffffff0 | (uint)param_1[9];
  if (5 < (piVar3[0x16] & 7U)) goto switchD_08016c9a_caseD_2;
  bVar1 = *(byte *)(DAT_08016f1c + (piVar3[0x16] & 7U));
  if (0x20 < bVar1) {
    if (bVar1 == 0x40) {
      local_18 = 0x8000;
      goto LAB_08016f7a;
    }
    goto switchD_08016c9a_caseD_2;
  }
  if (bVar1 < 2) goto switchD_08016c9a_caseD_2;
  switch(bVar1) {
  case 2:
    local_18 = FUN_08011230();
    if (local_18 != 0) break;
    goto LAB_08016ec4;
  default:
    goto switchD_08016c9a_caseD_2;
  case 4:
    FUN_08011254(auStack_28);
    local_18 = local_24;
    goto joined_r0x0801706c;
  case 8:
    FUN_080113d0(auStack_1c);
joined_r0x0801706c:
    if (local_18 == 0) {
LAB_08016ec4:
      uVar5 = 0;
      goto LAB_08016c68;
    }
    break;
  case 0x10:
    local_18 = DAT_08017108;
    if (*DAT_080170f4 << 0x1a < 0) {
      local_18 = DAT_08017108 >> ((uint)(*DAT_080170f4 << 0x1b) >> 0x1e);
    }
    break;
  case 0x20:
    local_18 = DAT_08017110;
  }
LAB_08016f7a:
  puVar4 = param_1[1];
  uVar9 = local_18 / *(ushort *)(DAT_080170f0 + (int)param_1[9] * 2);
  if (((uint)((int)puVar4 * 3) <= uVar9) && (uVar9 <= (uint)((int)puVar4 * 0x1000))) {
    uVar11 = FUN_080006f8(local_18,0);
    uVar9 = (uint)uVar11 * 0x100;
    uVar9 = FUN_080006f8(uVar9 + ((uint)puVar4 >> 1),
                         ((int)((ulonglong)uVar11 >> 0x20) << 8 | (uint)uVar11 >> 0x18) +
                         (uint)CARRY4(uVar9,(uint)puVar4 >> 1),puVar4,0);
    if (uVar9 - 0x300 <= DAT_080170fc) {
LAB_08016f54:
      uVar5 = 0;
      (*param_1)[3] = uVar9;
      goto LAB_08016c68;
    }
  }
switchD_08016c9a_caseD_2:
  uVar5 = 1;
LAB_08016c68:
  param_1[0x1d] = (uint *)0x0;
  param_1[0x1a] = (uint *)0x10001;
  param_1[0x1e] = (uint *)0x0;
  return uVar5;
}



/* === 08017118 FUN_08017118 === */

void FUN_08017118(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = param_1[10];
  if (iVar2 << 0x1c < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xffff7fff | param_1[0xe];
  }
  if (iVar2 << 0x1f < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffdffff | param_1[0xb];
  }
  if (iVar2 << 0x1e < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffeffff | param_1[0xc];
  }
  if (iVar2 << 0x1d < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffbffff | param_1[0xd];
  }
  if (iVar2 << 0x1b < 0) {
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xffffefff | param_1[0xf];
  }
  if (iVar2 << 0x1a < 0) {
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xffffdfff | param_1[0x10];
  }
  if (iVar2 << 0x19 < 0) {
    iVar1 = *param_1;
    uVar3 = param_1[0x11];
    *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) & 0xffefffff | uVar3;
    if (uVar3 == 0x100000) {
      *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) & 0xff9fffff | param_1[0x12];
    }
  }
  if (iVar2 << 0x18 < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfff7ffff | param_1[0x13];
  }
  return;
}



/* === 080171c8 FUN_080171c8 === */

undefined4 FUN_080171c8(int **param_1,uint param_2,uint param_3,int param_4,uint param_5)

{
  int iVar1;
  int *piVar2;
  
  piVar2 = *param_1;
  do {
    do {
      do {
        if (((param_2 & ~piVar2[7]) == 0) != param_3) {
          return 0;
        }
      } while (param_5 == 0xffffffff);
      iVar1 = FUN_08009ce8();
      if ((param_5 < (uint)(iVar1 - param_4)) || (param_5 == 0)) {
        return 3;
      }
      piVar2 = *param_1;
    } while (((-1 < *piVar2 << 0x1d) || (param_2 == 0x80)) || (param_2 == 0x40));
    if ((piVar2[7] & 8U) != 0) {
      piVar2[8] = 8;
      FUN_0801660c(param_1);
      param_1[0x24] = (int *)&SupervisorCall;
      *(bool *)(param_1 + 0x21) = param_5 == 0;
      return 1;
    }
  } while (-1 < piVar2[7] << 0x14);
  piVar2[8] = 0x800;
  FUN_0801660c(param_1);
  *(undefined *)(param_1 + 0x21) = 0;
  param_1[0x24] = (int *)0x20;
  return 3;
}



/* === 08017328 FUN_08017328 === */

undefined4 FUN_08017328(uint **param_1)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  uint *puVar4;
  
  if (param_1 == (uint **)0x0) {
    return 1;
  }
  if (param_1[0x22] == (uint *)0x0) {
    *(undefined *)(param_1 + 0x21) = 0;
    FUN_08014ce4();
  }
  puVar4 = param_1[10];
  param_1[0x22] = (uint *)0x24;
  **param_1 = **param_1 & 0xfffffffe;
  if (puVar4 != (uint *)0x0) {
    FUN_08017118(param_1);
  }
  iVar3 = FUN_08016bd8(param_1);
  if (iVar3 != 1) {
    puVar4 = *param_1;
    puVar4[1] = puVar4[1] & 0xffffb7ff;
    puVar4[2] = puVar4[2] & 0xffffffd5;
    *puVar4 = *puVar4 | 1;
    param_1[0x24] = (uint *)0x0;
    uVar2 = FUN_08009ce8();
    puVar4 = *param_1;
    if ((int)(*puVar4 << 0x1c) < 0) {
      iVar3 = FUN_080171c8(param_1,0x200000,0,uVar2,0x1ffffff);
      puVar4 = *param_1;
      if (iVar3 != 0) {
        do {
          ExclusiveAccess(puVar4);
          bVar1 = (bool)hasExclusiveAccess(puVar4);
        } while (!bVar1);
        *puVar4 = *puVar4 & 0xffffff7f;
        *(bool *)(param_1 + 0x21) = !bVar1;
        param_1[0x22] = (uint *)0x20;
        return 3;
      }
    }
    if (((int)(*puVar4 << 0x1d) < 0) &&
       (iVar3 = FUN_080171c8(param_1,0x400000,0,uVar2,0x1ffffff), iVar3 != 0)) {
      puVar4 = *param_1;
      do {
        ExclusiveAccess(puVar4);
        bVar1 = (bool)hasExclusiveAccess(puVar4);
      } while (!bVar1);
      *puVar4 = *puVar4 & 0xfffffedf;
      do {
        ExclusiveAccess(puVar4 + 2);
        bVar1 = (bool)hasExclusiveAccess(puVar4 + 2);
      } while (!bVar1);
      puVar4[2] = puVar4[2] & 0xfffffffe;
      uVar2 = 3;
      *(bool *)(param_1 + 0x21) = !bVar1;
      param_1[0x23] = (uint *)0x20;
    }
    else {
      uVar2 = 0;
      param_1[0x22] = (uint *)0x20;
      *(undefined *)(param_1 + 0x21) = 0;
      param_1[0x23] = (uint *)0x20;
      param_1[0x1b] = (uint *)0x0;
      param_1[0x1c] = (uint *)0x0;
    }
    return uVar2;
  }
  return 1;
}



/* === 08017434 FUN_08017434 === */

undefined4 FUN_08017434(uint **param_1,uint *param_2,int param_3)

{
  bool bVar1;
  uint *puVar2;
  int iVar3;
  uint **ppuVar4;
  uint **ppuVar5;
  uint *puVar6;
  
  if (param_1[0x23] != (uint *)0x20) {
    return 2;
  }
  if ((param_2 != (uint *)0x0) && (param_3 != 0)) {
    ppuVar5 = (uint **)*param_1;
    param_1[0x1b] = (uint *)(uint)(param_3 == 0);
    ppuVar4 = DAT_08017478;
    if ((ppuVar5 != DAT_08017478) && (ppuVar4 = (uint **)((int)ppuVar5[1] << 8), (int)ppuVar4 < 0))
    {
      do {
        ExclusiveAccess(ppuVar5);
        ppuVar4 = (uint **)((uint)*ppuVar5 | 0x4000000);
        bVar1 = (bool)hasExclusiveAccess(ppuVar5);
      } while (!bVar1);
      *ppuVar5 = (uint *)ppuVar4;
    }
    param_1[0x24] = (uint *)0x0;
    param_1[0x23] = (uint *)0x22;
    puVar2 = param_1[0x20];
    param_1[0x16] = param_2;
    *(short *)(param_1 + 0x17) = (short)param_3;
    if (puVar2 != (uint *)0x0) {
      puVar6 = *param_1;
      puVar2[0x14] = 0;
      puVar2[0xf] = DAT_08017428;
      puVar2[0x10] = DAT_0801742c;
      puVar2[0x13] = DAT_08017430;
      iVar3 = FUN_0800b3a0(puVar2,puVar6 + 9,param_2,param_3,ppuVar4);
      if (iVar3 != 0) {
        param_1[0x24] = (uint *)&DataAbort;
        param_1[0x23] = (uint *)0x20;
        return 1;
      }
    }
    if (param_1[4] == (uint *)0x0) {
      puVar2 = *param_1;
    }
    else {
      puVar2 = *param_1;
      do {
        ExclusiveAccess(puVar2);
        bVar1 = (bool)hasExclusiveAccess(puVar2);
      } while (!bVar1);
      *puVar2 = *puVar2 | 0x100;
    }
    do {
      ExclusiveAccess(puVar2 + 2);
      bVar1 = (bool)hasExclusiveAccess(puVar2 + 2);
    } while (!bVar1);
    puVar2[2] = puVar2[2] | 1;
    do {
      ExclusiveAccess(puVar2 + 2);
      bVar1 = (bool)hasExclusiveAccess(puVar2 + 2);
    } while (!bVar1);
    puVar2[2] = puVar2[2] | 0x40;
    return 0;
  }
  return 1;
}



/* === 08017488 DaisySP_CrossFade_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DaisySP_CrossFade_Process(float *param_1,float *param_2,float *param_3)

{
  float fVar1;
  float fVar2;
  float fVar3;
  
  fVar1 = DAT_08017554;
  fVar2 = DAT_08017558;
  switch(*(undefined *)(param_1 + 1)) {
  case 0:
    fVar1 = *param_1;
    break;
  case 1:
    fVar3 = *param_1;
    fVar2 = (float)libm_sinf(fVar3 * DAT_08017554);
    fVar1 = (float)libm_sinf((1.0 - fVar3) * fVar1);
    return fVar2 * *param_3 + *param_2 * fVar1;
  case 2:
    fVar1 = (float)libm_expf(DAT_08017550 + *param_1 * DAT_0801754c);
    break;
  case 3:
    fVar1 = *param_1 * *param_1;
    fVar2 = fVar1 * *param_3 + *param_2 * (1.0 - fVar1);
  default:
    return fVar2;
  }
  return fVar1 * *param_3 + *param_2 * (1.0 - fVar1);
}



/* === 0801755c InitSingleFloatHalf_unknown === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void InitSingleFloatHalf_unknown(undefined4 *param_1)

{
  *param_1 = 0x3f000000;
  return;
}



/* === 08017564 DaisySP_Decimator_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Decimator_Init(int param_1)

{
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 4) = 0x3f800000;
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined *)(param_1 + 0x20) = 0;
  *(undefined4 *)(param_1 + 0x24) = 0x3f800000;
  return;
}



/* === 08017580 DaisySP_Decimator_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Decimator_Process(float param_1,int param_2)

{
  uint uVar1;
  undefined4 uVar2;
  float fVar3;
  uint uVar4;
  
  uVar1 = *(int *)(param_2 + 0x18) + 1;
  fVar3 = *(float *)(param_2 + 4) * *(float *)(param_2 + 4) * DAT_0801760c;
  *(uint *)(param_2 + 0x18) = uVar1;
  uVar4 = (uint)(0.0 < fVar3) * (int)fVar3;
  *(uint *)(param_2 + 0x1c) = uVar4;
  if (uVar4 < uVar1) {
    *(float *)(param_2 + 0x10) = param_1;
    *(undefined4 *)(param_2 + 0x18) = 0;
  }
  else {
    param_1 = *(float *)(param_2 + 0x10);
  }
  uVar1 = *(uint *)(param_2 + 0xc);
  if (*(char *)(param_2 + 0x20) != '\0') {
    *(float *)(param_2 + 0x14) =
         (float)(longlong)
                (((int)(*(float *)(param_2 + 0x24) * param_1 * DAT_08017610) >> (uVar1 + 1 & 0xff))
                << (uVar1 + 1 & 0xff)) / (*(float *)(param_2 + 0x24) * DAT_08017610);
    return;
  }
  uVar2 = FixedToFP(((int)(param_1 * DAT_08017610) >> (uVar1 & 0xff)) << (uVar1 & 0xff),0x20,0x20,
                    0x10,0,0);
  *(undefined4 *)(param_2 + 0x14) = uVar2;
  return;
}



/* === 08017614 DaisySP_Svf_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_Init(float param_1,float *param_2)

{
  float fVar1;
  
  fVar1 = DAT_08017658;
  *param_2 = param_1;
  param_2[2] = 0.5;
  param_2[1] = fVar1;
  param_2[3] = 0.5;
  param_2[0x11] = 0.5;
  param_2[4] = 0.25;
  param_2[5] = 0.0;
  param_2[6] = 0.0;
  param_2[7] = 0.0;
  param_2[8] = 0.0;
  param_2[9] = 0.0;
  param_2[10] = 0.0;
  param_2[0xb] = 0.0;
  param_2[0x10] = 0.0;
  param_2[0xc] = 0.0;
  param_2[0xd] = 0.0;
  param_2[0xf] = 0.0;
  param_2[0xe] = 0.0;
  param_2[0x12] = param_1 / 3.0;
  return;
}



/* === 0801765c DaisySP_Svf_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_Process(float param_1,int param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  
  fVar6 = *(float *)(param_2 + 0x24);
  fVar3 = *(float *)(param_2 + 0x10);
  fVar1 = param_1 + -*(float *)(param_2 + 0x14) * fVar6;
  fVar4 = *(float *)(param_2 + 0x1c) + fVar6 * fVar3;
  *(float *)(param_2 + 0x2c) = param_1;
  fVar2 = fVar1 - fVar4;
  fVar5 = fVar6 + fVar3 * fVar2 + -fVar6 * fVar6 * *(float *)(param_2 + 0xc) * fVar6;
  param_1 = param_1 + -*(float *)(param_2 + 0x14) * fVar5;
  fVar7 = fVar4 + fVar3 * fVar5;
  *(float *)(param_2 + 0x18) = param_1;
  fVar6 = param_1 - fVar7;
  *(float *)(param_2 + 0x1c) = fVar7;
  *(float *)(param_2 + 0x20) = fVar6;
  *(float *)(param_2 + 0x30) = fVar7 * 0.5 + fVar4 * 0.5;
  fVar3 = fVar5 + fVar3 * fVar6 + -fVar5 * *(float *)(param_2 + 0xc) * fVar5 * fVar5;
  *(float *)(param_2 + 0x40) = param_1 * 0.5 + fVar1 * 0.5;
  *(float *)(param_2 + 0x3c) = (fVar7 - fVar6) * 0.5 + (fVar4 - fVar2) * 0.5;
  *(float *)(param_2 + 0x24) = fVar3;
  *(float *)(param_2 + 0x34) = fVar6 * 0.5 + fVar2 * 0.5;
  *(float *)(param_2 + 0x38) = fVar3 * 0.5 + fVar5 * 0.5;
  return;
}



/* === 08017724 DaisySP_Svf_SetFreq === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetFreq(undefined4 param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;
  float fVar4;
  
  uVar3 = FPMaxNum(param_1,DAT_0801780c);
  fVar4 = (float)FPMinNum(uVar3,param_2[0x12]);
  fVar2 = fVar4 / (*param_2 + *param_2);
  param_2[1] = fVar4;
  fVar4 = param_2[2];
  if (fVar2 == 0.25 || fVar2 < 0.25 != NAN(fVar2)) {
    fVar2 = (float)libm_sinf(fVar2 * DAT_08017810);
    fVar2 = fVar2 + fVar2;
    param_2[4] = fVar2;
    fVar1 = (float)libm_powf(fVar4,0x3e800000);
    fVar1 = (1.0 - fVar1) + (1.0 - fVar1);
    fVar2 = 2.0 / fVar2 + -fVar2 * 0.5;
    fVar2 = (float)((uint)(fVar2 != 2.0) * 0x40000000 + (uint)(fVar2 == 2.0) * (int)fVar2);
    if (fVar2 != fVar1 && fVar2 < fVar1 == (NAN(fVar2) || NAN(fVar1))) {
LAB_080177e8:
      fVar2 = (float)libm_powf(fVar4,0x3e800000);
      param_2[5] = (1.0 - fVar2) + (1.0 - fVar2);
      return;
    }
  }
  else {
    param_2[4] = DAT_08017814;
    fVar2 = (float)libm_powf(fVar4);
    fVar1 = (1.0 - fVar2) + (1.0 - fVar2);
    fVar2 = DAT_08017818;
    if (DAT_08017818 != fVar1 && DAT_08017818 < fVar1 == (NAN(DAT_08017818) || NAN(fVar1)))
    goto LAB_080177e8;
  }
  param_2[5] = fVar2;
  return;
}



/* === 0801781c DaisySP_Svf_SetRes === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetRes(undefined4 param_1,int param_2)

{
  float fVar1;
  float fVar2;
  undefined4 uVar3;
  float fVar4;
  
  uVar3 = FPMaxNum(param_1,DAT_080178a4);
  fVar4 = (float)FPMinNum(uVar3,0x3f800000);
  *(float *)(param_2 + 8) = fVar4;
  fVar1 = (float)libm_powf(fVar4,0x3e800000);
  fVar2 = 2.0 / *(float *)(param_2 + 0x10) + -*(float *)(param_2 + 0x10) * 0.5;
  fVar2 = (float)((uint)(fVar2 != 2.0) * 0x40000000 + (uint)(fVar2 == 2.0) * (int)fVar2);
  if ((int)((uint)((1.0 - fVar1) + (1.0 - fVar1) < fVar2) << 0x1f) < 0) {
    fVar1 = (float)libm_powf(fVar4,0x3e800000);
    fVar2 = (1.0 - fVar1) + (1.0 - fVar1);
  }
  *(float *)(param_2 + 0x14) = fVar2;
  *(float *)(param_2 + 0xc) = *(float *)(param_2 + 0x44) * fVar4;
  return;
}



/* === 080178a8 DaisySP_Svf_SetDrive === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Svf_SetDrive(float param_1,int param_2)

{
  undefined4 uVar1;
  float fVar2;
  
  uVar1 = FPMaxNum(param_1 * DAT_080178d4,DAT_080178d8);
  fVar2 = (float)FPMinNum(uVar1,0x3f800000);
  *(float *)(param_2 + 0x44) = fVar2;
  *(float *)(param_2 + 0xc) = *(float *)(param_2 + 8) * fVar2;
  return;
}



/* === 080178dc DaisySP_DcBlock_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_DcBlock_Init(float param_1,undefined4 *param_2)

{
  param_2[1] = 0;
  *param_2 = 0;
  param_2[2] = 1.0 - 10.0 / param_1;
  return;
}



/* === 080178f8 DaisySP_Compressor_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Compressor_Init(float param_1,undefined4 *param_2)

{
  float fVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 uVar4;
  float fVar5;
  undefined4 uVar6;
  float fVar7;
  undefined *puVar8;
  
  fVar1 = DAT_080179ec;
  if (param_1 == 1.0 || param_1 < 1.0 != NAN(param_1)) {
    iVar3 = 1;
    fVar5 = 1.0;
    fVar7 = 2.0;
    puVar8 = &UNK_c1200000;
  }
  else {
    iVar3 = DAT_080179e8;
    fVar5 = DAT_080179e4;
    fVar7 = DAT_080179e0;
    puVar8 = DAT_080179dc;
    if (param_1 == DAT_080179d8 || param_1 < DAT_080179d8 != (NAN(param_1) || NAN(DAT_080179d8))) {
      fVar7 = (float)(longlong)(int)param_1;
      fVar5 = 1.0 / fVar7;
      iVar3 = (int)param_1;
      fVar7 = 2.0 / fVar7;
      puVar8 = (undefined *)-(fVar5 / DAT_080179ec);
    }
  }
  param_2[0xc] = iVar3;
  *param_2 = 0x40000000;
  param_2[0xe] = fVar5;
  param_2[0xd] = fVar7;
  param_2[2] = fVar1;
  uVar4 = libm_expf(puVar8);
  param_2[10] = uVar4;
  fVar5 = (float)libm_expf(-(fVar7 / fVar1));
  param_2[3] = fVar1;
  param_2[8] = fVar5;
  param_2[9] = (1.0 - fVar5) * -0.5;
  uVar6 = libm_expf(puVar8);
  uVar2 = DAT_080179f4;
  uVar4 = DAT_080179f0;
  param_2[7] = fVar1;
  param_2[6] = fVar1;
  param_2[0xb] = uVar6;
  param_2[1] = uVar4;
  *(undefined *)(param_2 + 0xf) = 1;
  param_2[4] = uVar2;
  return;
}



/* === 080179f8 DaisySP_ATone_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_ATone_Init(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_08017a0c;
  *(undefined4 *)(param_2 + 4) = 0;
  *(undefined4 *)(param_2 + 0xc) = uVar1;
  *(undefined4 *)(param_2 + 0x10) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x14) = param_1;
  return;
}



/* === 08017a10 DaisySP_ATone_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_ATone_Process(int param_1,float *param_2)

{
  *(float *)(param_1 + 4) =
       (*param_2 + *(float *)(param_1 + 4)) * *(float *)(param_1 + 0x10) - *param_2;
  return;
}



/* === 08017a30 DaisySP_ATone_CalculateCoefficients === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_ATone_CalculateCoefficients(int param_1)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (float)libm_cosf((*(float *)(param_1 + 0xc) * DAT_08017a8c) / *(float *)(param_1 + 0x14));
  fVar1 = 2.0 - fVar1;
  fVar2 = fVar1 * fVar1 + -1.0;
  if ((int)((uint)(fVar2 < 0.0) << 0x1f) < 0) {
    fVar2 = (float)FUN_080184c8();
  }
  else {
    fVar2 = SQRT(fVar2);
  }
  *(float *)(param_1 + 0x10) = fVar1 - fVar2;
  return;
}



/* === 08017a90 DaisySP_Tone_Init === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Tone_Init(undefined4 param_1,int param_2)

{
  undefined4 uVar1;
  
  uVar1 = DAT_08017aa8;
  *(undefined4 *)(param_2 + 4) = 0;
  *(undefined4 *)(param_2 + 0xc) = uVar1;
  *(undefined4 *)(param_2 + 0x10) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x14) = 0x3f000000;
  *(undefined4 *)(param_2 + 0x18) = param_1;
  return;
}



/* === 08017aac DaisySP_Tone_Process === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float DaisySP_Tone_Process(float param_1,int param_2)

{
  float fVar1;
  
  fVar1 = *(float *)(param_2 + 0x14) * *(float *)(param_2 + 4) +
          *(float *)(param_2 + 0x10) * param_1;
  *(float *)(param_2 + 4) = fVar1;
  return fVar1;
}



/* === 08017acc DaisySP_Tone_CalculateCoefficients === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void DaisySP_Tone_CalculateCoefficients(int param_1)

{
  float fVar1;
  float fVar2;
  
  fVar1 = (float)libm_cosf((*(float *)(param_1 + 0xc) * DAT_08017b34) / *(float *)(param_1 + 0x18));
  fVar1 = 2.0 - fVar1;
  fVar2 = fVar1 * fVar1 + -1.0;
  if ((int)((uint)(fVar2 < 0.0) << 0x1f) < 0) {
    fVar2 = (float)FUN_080184c8();
  }
  else {
    fVar2 = SQRT(fVar2);
  }
  *(float *)(param_1 + 0x14) = fVar1 - fVar2;
  *(float *)(param_1 + 0x10) = 1.0 - (fVar1 - fVar2);
  return;
}



/* === 08017b38 FUN_08017b38 === */

undefined8 FUN_08017b38(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (DAT_080188d4 == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = 2;
    param_1 = param_2;
  }
  return CONCAT44(param_1,uVar1);
}



/* === 08017b48 libm_cosf === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_cosf(float param_1)

{
  longlong lVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 in_r3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  double dVar7;
  double dVar8;
  double dVar9;
  double dVar10;
  double dVar11;
  double dVar12;
  float fVar13;
  double dVar14;
  
  uVar4 = (uint)((int)param_1 << 1) >> 0x15;
  dVar14 = (double)param_1;
  if (uVar4 < 0x3f4) {
    dVar14 = dVar14 * dVar14;
    if (0x397 < uVar4) {
      return (float)(*(double *)(DAT_08017cf8 + 0x30) + dVar14 * *(double *)(DAT_08017cf8 + 0x38) +
                     dVar14 * dVar14 * *(double *)(DAT_08017cf8 + 0x40) +
                    (*(double *)(DAT_08017cf8 + 0x48) + dVar14 * *(double *)(DAT_08017cf8 + 0x50)) *
                    dVar14 * dVar14 * dVar14);
    }
    return 1.0;
  }
  if (uVar4 < 0x42f) {
    uVar4 = (int)(longlong)(dVar14 * *(double *)(DAT_08017cf8 + 0x20)) + 0x800000 >> 0x18;
    iVar3 = DAT_08017cf8 + 0x70;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08017cf8;
    }
    dVar14 = dVar14 + -(double)(longlong)(int)uVar4 * *(double *)(DAT_08017cf8 + 0x28);
    dVar12 = dVar14 * dVar14;
    if (-1 < (int)(uVar4 << 0x1f)) {
      dVar7 = *(double *)(iVar3 + 0x50);
      dVar11 = *(double *)(iVar3 + 0x48);
      dVar8 = *(double *)(iVar3 + 0x38);
      dVar14 = *(double *)(iVar3 + 0x30);
      dVar9 = *(double *)(iVar3 + 0x40);
LAB_08017c28:
      return (float)(dVar14 + dVar12 * dVar8 + dVar12 * dVar12 * dVar9 +
                    (dVar11 + dVar12 * dVar7) * dVar12 * dVar12 * dVar12);
    }
    dVar7 = *(double *)(DAT_08017cf8 + (uVar4 & 3) * 8);
    dVar9 = *(double *)(iVar3 + 0x68);
    dVar11 = *(double *)(iVar3 + 0x60);
    dVar8 = *(double *)(iVar3 + 0x58);
  }
  else {
    if (0x7f7 < uVar4) {
      fVar13 = (param_1 - param_1) / (param_1 - param_1);
      if (!NAN(param_1)) {
        puVar2 = (undefined4 *)FUN_080188d8();
        *puVar2 = 0x21;
        return fVar13;
      }
      return fVar13;
    }
    uVar4 = (uint)((int)param_1 << 2) >> 0x1c;
    iVar3 = DAT_08017cfc + uVar4 * 4;
    uVar5 = ((uint)param_1 & 0x7fffff | 0x800000) << ((uint)((int)param_1 << 6) >> 0x1d);
    lVar1 = (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x10) +
            ((ulonglong)(uVar5 * *(int *)(DAT_08017cfc + uVar4 * 4)) << 0x20 |
            (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x20) >> 0x20);
    iVar3 = (int)((ulonglong)lVar1 >> 0x20);
    uVar6 = iVar3 + 0x20000000U >> 0x1e;
    dVar14 = (double)FUN_0800069c((int)lVar1,iVar3 - (iVar3 + 0x20000000U & 0xc0000000),iVar3,uVar5,
                                  in_r3);
    uVar4 = uVar6 - ((int)param_1 >> 0x1f);
    iVar3 = DAT_08017d00;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08017d00 + -0x70;
    }
    dVar14 = dVar14 * DAT_08017cf0;
    dVar12 = dVar14 * dVar14;
    if (-1 < (int)(uVar6 << 0x1f)) {
      dVar7 = *(double *)(iVar3 + 0x50);
      dVar11 = *(double *)(iVar3 + 0x48);
      dVar8 = *(double *)(iVar3 + 0x38);
      dVar14 = *(double *)(iVar3 + 0x30);
      dVar9 = *(double *)(iVar3 + 0x40);
      goto LAB_08017c28;
    }
    dVar7 = *(double *)(DAT_08017d00 + -0x70 + (uVar4 & 3) * 8);
    dVar9 = *(double *)(iVar3 + 0x68);
    dVar11 = *(double *)(iVar3 + 0x60);
    dVar8 = *(double *)(iVar3 + 0x58);
  }
  dVar10 = dVar14 * dVar7 * dVar12;
  return (float)(dVar14 * dVar7 + dVar10 * dVar8 + (dVar11 + dVar12 * dVar9) * dVar12 * dVar10);
}



/* === 08017da4 libm_expf === */

/* WARNING: Removing unreachable block (ram,0x08017d1e) */
/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_expf(float param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  float fVar3;
  uint uVar4;
  double dVar5;
  
  uVar2 = (uint)((int)param_1 << 1) >> 0x15;
  if (0x42a < uVar2) {
    if (param_1 == -INFINITY) {
      return DAT_08017e84;
    }
    if (0x7f7 < uVar2) {
      return param_1 + param_1;
    }
    fVar3 = DAT_08017d5c;
    if (((param_1 != DAT_08017e78 && param_1 < DAT_08017e78 == (NAN(param_1) || NAN(DAT_08017e78)))
        || (fVar3 = DAT_08017d44, (int)((uint)(param_1 < DAT_08017e7c) << 0x1f) < 0)) ||
       (fVar3 = DAT_08017d50, (int)((uint)(param_1 < DAT_08017e80) << 0x1f) < 0)) {
      puVar1 = (undefined4 *)FUN_080188d8();
      *puVar1 = 0x22;
      return fVar3 * fVar3;
    }
  }
  dVar5 = *(double *)(DAT_08017e74 + 0x120) + *(double *)(DAT_08017e74 + 0x128) * (double)param_1;
  uVar4 = SUB84(dVar5,0);
  uVar2 = uVar4 & 0x1f;
  dVar5 = -(dVar5 - *(double *)(DAT_08017e74 + 0x120)) +
          *(double *)(DAT_08017e74 + 0x128) * (double)param_1;
  return (float)((*(double *)(DAT_08017e74 + 0x140) * dVar5 + 1.0 +
                 (*(double *)(DAT_08017e74 + 0x138) + *(double *)(DAT_08017e74 + 0x130) * dVar5) *
                 dVar5 * dVar5) *
                (double)CONCAT44(*(int *)(DAT_08017e74 + uVar2 * 8 + 4) + uVar4 * 0x8000,
                                 *(undefined4 *)(DAT_08017e74 + uVar2 * 8)));
}



/* === 08017e88 libm_powf === */

/* WARNING: Type propagation algorithm not settling */
/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_powf(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  uint uVar3;
  int iVar4;
  double *pdVar5;
  uint uVar6;
  undefined *puVar7;
  float fVar8;
  float fVar9;
  longlong in_d0;
  float fVar10;
  double dVar11;
  double dVar12;
  uint uVar13;
  
  fVar9 = (float)in_d0;
  fVar10 = (float)((ulonglong)in_d0 >> 0x20);
  iVar4 = (int)fVar10 * 2;
  fVar8 = fVar9;
  if ((int)fVar9 - 0x800000U < 0x7f000000) {
    if (iVar4 - 1U < 0xfeffffff) {
      uVar3 = 0;
LAB_08017eba:
      puVar7 = &UNK_c0cd0000 + (int)fVar8;
      pdVar5 = (double *)(DAT_080181a0 + ((uint)((int)puVar7 * 0x200) >> 0x1c) * 0x10);
      dVar11 = *pdVar5 * (double)(float)((int)fVar8 - ((uint)puVar7 & 0xff800000)) + -1.0;
      dVar12 = dVar11 * dVar11;
      dVar11 = (double)fVar10 *
               ((double)(longlong)((int)puVar7 >> 0x17) + pdVar5[1] +
                dVar11 * *(double *)(DAT_080181a0 + 0x120) +
                dVar12 * (*(double *)(DAT_080181a0 + 0x118) +
                         dVar11 * *(double *)(DAT_080181a0 + 0x110)) +
               (*(double *)(DAT_080181a0 + 0x108) + dVar11 * *(double *)(DAT_080181a0 + 0x100)) *
               dVar12 * dVar12);
      if (((uint)((int)((ulonglong)dVar11 >> 0x20) << 1) >> 0x10 < 0x80bf) ||
         (((fVar8 = DAT_08017d5c,
           dVar11 == DAT_08018180 || dVar11 < DAT_08018180 != (NAN(dVar11) || NAN(DAT_08018180)) &&
           (fVar8 = DAT_08017d44, DAT_08018190 < dVar11)) &&
          (fVar8 = DAT_08017d50, -1 < (int)((uint)(dVar11 < DAT_08018198) << 0x1f))))) {
        dVar12 = dVar11 + *(double *)(DAT_080181a4 + 0x100);
        uVar13 = SUB84(dVar12,0);
        uVar6 = uVar13 & 0x1f;
        dVar11 = dVar11 - (dVar12 - *(double *)(DAT_080181a4 + 0x100));
        return (float)((dVar11 * *(double *)(DAT_080181a4 + 0x118) + 1.0 +
                       (*(double *)(DAT_080181a4 + 0x110) +
                       dVar11 * *(double *)(DAT_080181a4 + 0x108)) * dVar11 * dVar11) *
                      (double)CONCAT44(*(int *)(DAT_080181a4 + uVar6 * 8 + 4) +
                                       (uVar3 + uVar13) * 0x8000,
                                       *(undefined4 *)(DAT_080181a4 + uVar6 * 8)));
      }
      if (uVar3 == 0) {
        fVar8 = fVar8 * fVar8;
        uVar2 = 0x22;
      }
      else {
        fVar8 = -fVar8 * fVar8;
        uVar2 = 0x22;
      }
      goto LAB_08017d04;
    }
    if ((iVar4 != 0) && (fVar8 = fVar10, fVar9 != 1.0)) goto LAB_080180b4;
  }
  else {
    if (iVar4 - 1U < 0xfeffffff) {
      if ((int)fVar9 * 2 - 1U < 0xfeffffff) {
        if ((int)fVar9 < 0) {
          uVar3 = (uint)((int)fVar10 << 1) >> 0x18;
          if (uVar3 < 0x7f) {
LAB_08017d84:
            fVar8 = (fVar9 - fVar9) / (fVar9 - fVar9);
            if (NAN(fVar9)) {
              return fVar8;
            }
            uVar2 = 0x21;
            goto LAB_08017d04;
          }
          if (uVar3 < 0x97) {
            uVar3 = 1 << (0x96 - uVar3 & 0xff);
            if ((uVar3 - 1 & (uint)fVar10) != 0) goto LAB_08017d84;
            uVar3 = uVar3 & (uint)fVar10;
            if (uVar3 != 0) {
              uVar3 = 0x10000;
            }
          }
          else {
            uVar3 = 0;
          }
          fVar8 = (float)((uint)fVar9 & 0x7fffffff);
        }
        else {
          uVar3 = 0;
        }
        if ((uint)fVar8 < 0x800000) {
          fVar8 = (float)(((uint)(fVar9 * DAT_080181a8) & 0x7fffffff) + 0xf4800000);
        }
        goto LAB_08017eba;
      }
      fVar9 = fVar9 * fVar9;
      if (((int)fVar9 < 0) && (uVar3 = (uint)((int)fVar10 << 1) >> 0x18, uVar3 - 0x7f < 0x18)) {
        uVar3 = 1 << (0x96 - uVar3 & 0xff);
        if ((uVar3 - 1 & (uint)fVar10) != 0) goto LAB_080180ea;
        uVar3 = uVar3 & (uint)fVar10;
        if (uVar3 != 0) {
          fVar9 = -fVar9;
          uVar3 = 1;
        }
      }
      else {
LAB_080180ea:
        uVar3 = 0;
      }
      if ((int)fVar9 * 2 == 0) {
        if (in_d0 < 0) {
          uVar2 = 0x22;
          fVar8 = (float)((uint)(uVar3 == 0) * 0x3f800000 + (uint)(uVar3 != 0) * -0x40800000) /
                  DAT_08017d80;
LAB_08017d04:
          puVar1 = (undefined4 *)FUN_080188d8();
          *puVar1 = uVar2;
          return fVar8;
        }
      }
      else if (in_d0 < 0) {
        fVar9 = 1.0 / fVar9;
      }
      return fVar9;
    }
    if (iVar4 != 0) {
LAB_080180b4:
      uVar3 = (int)fVar9 * 2;
      if ((uVar3 < 0xff000001) && (iVar4 == -0x1000000)) {
        if (uVar3 == 0x7f000000) {
          return (float)0x3f800000;
        }
        if ((uint)(0x7effffff < uVar3) == -((int)~(uint)fVar10 >> 0x1f)) {
          return fVar10 * fVar10;
        }
        return DAT_080181ac;
      }
      goto LAB_0801811a;
    }
  }
  if (((uint)fVar8 ^ 0x400000) * 2 < 0xff800001) {
    return 1.0;
  }
LAB_0801811a:
  return fVar9 + fVar10;
}



/* === 080181b0 libm_tanhf === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_tanhf(float param_1)

{
  undefined *puVar1;
  float fVar2;
  
  puVar1 = (undefined *)((uint)param_1 & 0x7fffffff);
  if ((undefined *)0x7f7fffff < puVar1) {
    if (-1 < (int)param_1) {
      return 1.0 / param_1 + 1.0;
    }
    return 1.0 / param_1 - 1.0;
  }
  if (DAT_08018264 < (int)puVar1) {
    fVar2 = 1.0;
  }
  else {
    if (puVar1 < &DAT_24000000) {
      return (param_1 + 1.0) * param_1;
    }
    if (puVar1 < (undefined *)0x3f800000) {
      fVar2 = (float)FUN_080188ac();
      fVar2 = (float)FUN_08018640(fVar2 * -2.0);
      fVar2 = -fVar2 / (fVar2 + 2.0);
    }
    else {
      fVar2 = (float)FUN_080188ac();
      fVar2 = (float)FUN_08018640(fVar2 + fVar2);
      fVar2 = 1.0 - 2.0 / (fVar2 + 2.0);
    }
  }
  if ((int)param_1 < 0) {
    fVar2 = -fVar2;
  }
  return fVar2;
}



/* === 08018268 libm_sinf === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_sinf(float param_1)

{
  longlong lVar1;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  double dVar6;
  double dVar7;
  double dVar8;
  float fVar9;
  
  uVar4 = (uint)((int)param_1 << 1) >> 0x15;
  dVar8 = (double)param_1;
  if (uVar4 < 0x3f4) {
    dVar6 = dVar8 * dVar8;
    if (0x397 < uVar4) {
      return (float)(dVar8 + dVar8 * dVar6 * *(double *)(DAT_08018468 + 0x58) +
                    (*(double *)(DAT_08018468 + 0x60) + dVar6 * *(double *)(DAT_08018468 + 0x68)) *
                    dVar6 * dVar8 * dVar6);
    }
  }
  else if (uVar4 < 0x42f) {
    uVar4 = (int)(longlong)(dVar8 * *(double *)(DAT_08018468 + 0x20)) + 0x800000 >> 0x18;
    iVar3 = DAT_08018468 + 0x70;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08018468;
    }
    dVar8 = dVar8 + -(double)(longlong)(int)uVar4 * *(double *)(DAT_08018468 + 0x28);
    dVar6 = dVar8 * dVar8;
    if ((int)(uVar4 << 0x1f) < 0) {
      return (float)(*(double *)(iVar3 + 0x30) + dVar6 * *(double *)(iVar3 + 0x38) +
                     dVar6 * dVar6 * *(double *)(iVar3 + 0x40) +
                    (*(double *)(iVar3 + 0x48) + dVar6 * *(double *)(iVar3 + 0x50)) *
                    dVar6 * dVar6 * dVar6);
    }
    dVar8 = dVar8 * *(double *)(DAT_08018468 + (uVar4 & 3) * 8);
    dVar7 = dVar8 * dVar6;
    param_1 = (float)(dVar8 + dVar7 * *(double *)(iVar3 + 0x58) +
                     (*(double *)(iVar3 + 0x60) + dVar6 * *(double *)(iVar3 + 0x68)) * dVar6 * dVar7
                     );
  }
  else {
    if (0x7f7 < uVar4) {
      fVar9 = (param_1 - param_1) / (param_1 - param_1);
      if (!NAN(param_1)) {
        puVar2 = (undefined4 *)FUN_080188d8();
        *puVar2 = 0x21;
        return fVar9;
      }
      return fVar9;
    }
    uVar4 = (uint)((int)param_1 << 2) >> 0x1c;
    iVar3 = DAT_0801846c + uVar4 * 4;
    uVar5 = ((uint)param_1 & 0x7fffff | 0x800000) << ((uint)((int)param_1 << 6) >> 0x1d);
    lVar1 = (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x10) +
            ((ulonglong)(uVar5 * *(int *)(DAT_0801846c + uVar4 * 4)) << 0x20 |
            (ulonglong)uVar5 * (ulonglong)*(uint *)(iVar3 + 0x20) >> 0x20);
    iVar3 = (int)((ulonglong)lVar1 >> 0x20);
    uVar4 = iVar3 + 0x20000000;
    uVar5 = uVar4 >> 0x1e;
    dVar8 = (double)FUN_0800069c((int)lVar1,iVar3 - (uVar4 & 0xc0000000));
    uVar4 = uVar5 - ((int)param_1 >> 0x1f);
    iVar3 = DAT_08018470;
    if ((uVar4 & 2) == 0) {
      iVar3 = DAT_08018470 + -0x70;
    }
    dVar8 = dVar8 * DAT_08018460;
    dVar6 = dVar8 * dVar8;
    if ((int)(uVar5 << 0x1f) < 0) {
      param_1 = (float)(*(double *)(iVar3 + 0x30) + dVar6 * *(double *)(iVar3 + 0x38) +
                        dVar6 * dVar6 * *(double *)(iVar3 + 0x40) +
                       (*(double *)(iVar3 + 0x48) + dVar6 * *(double *)(iVar3 + 0x50)) *
                       dVar6 * dVar6 * dVar6);
    }
    else {
      dVar8 = dVar8 * *(double *)(DAT_08018470 + -0x70 + (uVar4 & 3) * 8);
      dVar7 = dVar8 * dVar6;
      param_1 = (float)(dVar8 + dVar7 * *(double *)(iVar3 + 0x58) +
                       (*(double *)(iVar3 + 0x60) + dVar6 * *(double *)(iVar3 + 0x68)) *
                       dVar6 * dVar7);
    }
  }
  return param_1;
}



/* === 08018474 libm_fmodf === */

/* WARNING: Removing unreachable block (ram,0x0801849a) */
/* WARNING: Removing unreachable block (ram,0x080184ae) */
/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

undefined4 libm_fmodf(void)

{
  undefined4 uVar1;
  
  uVar1 = FUN_08018518();
  return uVar1;
}



/* === 080184c8 FUN_080184c8 === */

float FUN_080184c8(float param_1)

{
  float fVar1;
  undefined4 *puVar2;
  float fVar3;
  
  fVar3 = (float)FUN_08018638();
  fVar1 = DAT_08018514;
  if (((*DAT_08018510 != -1) && (!NAN(param_1))) &&
     ((int)((uint)(param_1 < DAT_08018514) << 0x1f) < 0)) {
    puVar2 = (undefined4 *)FUN_080188d8();
    *puVar2 = 0x21;
    return fVar1 / fVar1;
  }
  return fVar3;
}



/* === 08018518 FUN_08018518 === */

float FUN_08018518(float param_1,float param_2)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  
  uVar1 = (uint)param_2 & 0x7fffffff;
  if (((uVar1 == 0) || (uVar5 = (uint)param_1 & 0x7fffffff, 0x7f7fffff < uVar5)) ||
     (0x7f800000 < uVar1)) {
    param_1 = (param_1 * param_2) / (param_1 * param_2);
  }
  else if (uVar1 <= uVar5) {
    uVar6 = (uint)param_1 & 0x80000000;
    if (uVar5 != uVar1) {
      if (((uint)param_1 & 0x7f800000) == 0) {
        iVar4 = -0x7e;
        for (iVar2 = uVar5 * 0x100; 0 < iVar2; iVar2 = iVar2 * 2) {
          iVar4 = iVar4 + -1;
        }
      }
      else {
        iVar4 = ((int)uVar5 >> 0x17) + -0x7f;
      }
      if (((uint)param_2 & 0x7f800000) == 0) {
        iVar2 = -0x7e;
        for (iVar3 = uVar1 << 8; -1 < iVar3; iVar3 = iVar3 << 1) {
          iVar2 = iVar2 + -1;
        }
      }
      else {
        iVar2 = ((int)uVar1 >> 0x17) + -0x7f;
      }
      if (iVar4 + 0x7e < 0 == SCARRY4(iVar4,0x7e)) {
        uVar5 = (uint)param_1 & 0x7fffff | 0x800000;
      }
      else {
        uVar5 = uVar5 << (-iVar4 - 0x7eU & 0xff);
      }
      if (iVar2 + 0x7e < 0 == SCARRY4(iVar2,0x7e)) {
        uVar1 = (uint)param_2 & 0x7fffff | 0x800000;
      }
      else {
        uVar1 = uVar1 << (-iVar2 - 0x7eU & 0xff);
      }
      for (iVar4 = iVar4 - iVar2; iVar4 != 0; iVar4 = iVar4 + -1) {
        iVar3 = uVar5 - uVar1;
        if (iVar3 < 0) {
          uVar5 = uVar5 << 1;
        }
        else {
          if (iVar3 == 0) goto LAB_080185bc;
          uVar5 = iVar3 * 2;
        }
      }
      if (-1 < (int)(uVar5 - uVar1)) {
        uVar5 = uVar5 - uVar1;
      }
      if (uVar5 != 0) {
        for (; (int)uVar5 < 0x800000; uVar5 = uVar5 << 1) {
          iVar2 = iVar2 + -1;
        }
        if (iVar2 + 0x7e < 0 == SCARRY4(iVar2,0x7e)) {
          return (float)(uVar5 - 0x800000 | uVar6 | (iVar2 + 0x7f) * 0x800000);
        }
        return (float)((int)uVar5 >> (-iVar2 - 0x7eU & 0xff) | uVar6);
      }
    }
LAB_080185bc:
    return *(float *)(DAT_08018634 + (uVar6 >> 0x1d));
  }
  return param_1;
}



/* === 08018638 FUN_08018638 === */

float FUN_08018638(float param_1)

{
  return SQRT(param_1);
}



/* === 08018640 FUN_08018640 === */

/* WARNING: Removing unreachable block (ram,0x08017d1e) */

float FUN_08018640(float param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  float fVar3;
  float fVar4;
  float in_s15;
  float fVar5;
  
  uVar2 = (uint)param_1 & 0x7fffffff;
  if (DAT_08018870 < uVar2) {
    if (0x7f800000 < uVar2) {
      return param_1 + param_1;
    }
    if (uVar2 == 0x7f800000) {
      return (float)((uint)(-1 < (int)param_1) * (int)param_1 +
                    (uint)(-1 >= (int)param_1) * -0x40800000);
    }
    if ((int)param_1 < 0) {
      if ((int)((uint)(param_1 + DAT_080188a0 < 0.0) << 0x1f) < 0) {
        return -1.0;
      }
      fVar5 = -0.5;
    }
    else {
      if (DAT_08018874 < uVar2) {
        fVar5 = DAT_08017d5c * DAT_08017d5c;
        puVar1 = (undefined4 *)FUN_080188d8();
        *puVar1 = 0x22;
        return fVar5;
      }
      fVar5 = 0.5;
    }
LAB_08018672:
    uVar2 = (uint)(fVar5 + param_1 * DAT_08018878);
    fVar3 = param_1 + -(float)(longlong)(int)uVar2 * DAT_0801887c;
    fVar5 = (float)(longlong)(int)uVar2 * DAT_08018880;
  }
  else {
    if (uVar2 <= DAT_08018884) {
      if (uVar2 < 0x33000000) {
        return param_1 - ((param_1 + DAT_080188a4) - (param_1 + DAT_080188a4));
      }
      uVar2 = 0;
      goto LAB_080186b4;
    }
    if (DAT_0801889c < uVar2) {
      fVar5 = (float)((uint)(-1 < (int)param_1) * 0x3f000000 +
                     (uint)(-1 >= (int)param_1) * -0x41000000);
      goto LAB_08018672;
    }
    if ((int)param_1 < 0) {
      fVar3 = param_1 + DAT_0801887c;
      uVar2 = 0xffffffff;
      fVar5 = DAT_080188a8;
    }
    else {
      fVar3 = param_1 - DAT_0801887c;
      uVar2 = 1;
      fVar5 = DAT_08018880;
    }
  }
  param_1 = fVar3 - fVar5;
  in_s15 = (fVar3 - param_1) - fVar5;
LAB_080186b4:
  fVar4 = param_1 * param_1 * 0.5;
  fVar3 = (DAT_08018894 +
          (DAT_08018898 + (DAT_08018890 + (DAT_0801888c + fVar4 * DAT_08018888) * fVar4) * fVar4) *
          fVar4) * fVar4 + 1.0;
  fVar5 = -(param_1 * 0.5) * fVar3 + 3.0;
  fVar5 = ((fVar3 - fVar5) / (-param_1 * fVar5 + 6.0)) * fVar4;
  if (uVar2 == 0) {
    return param_1 - (-fVar4 + param_1 * fVar5);
  }
  fVar4 = (-in_s15 + (fVar5 - in_s15) * param_1) - fVar4;
  if (uVar2 == 0xffffffff) {
    return (param_1 - fVar4) * 0.5 + -0.5;
  }
  if (uVar2 == 1) {
    if (-1 < (int)((uint)(param_1 < -0.25) << 0x1f)) {
      return (param_1 - fVar4) * 2.0 + 1.0;
    }
    return (fVar4 - (param_1 + 0.5)) * -2.0;
  }
  if (0x39 < uVar2 + 1) {
    return (float)((int)(1.0 - (fVar4 - param_1)) + uVar2 * 0x800000) - 1.0;
  }
  if (0x16 < (int)uVar2) {
    return (float)((int)((param_1 - (fVar4 + (float)((0x7f - uVar2) * 0x800000))) + 1.0) +
                  uVar2 * 0x800000);
  }
  return (float)((int)((float)(0x3f800000 - (0x1000000 >> (uVar2 & 0xff))) - (fVar4 - param_1)) +
                uVar2 * 0x800000);
}



/* === 080188ac FUN_080188ac === */

uint FUN_080188ac(uint param_1)

{
  return param_1 & 0x7fffffff;
}



/* === 080188d8 FUN_080188d8 === */

undefined4 FUN_080188d8(void)

{
  return *DAT_080188e0;
}



/* === 080188e4 libc_init_array === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void libc_init_array(void)

{
  code **ppcVar1;
  int iVar2;
  int iVar3;
  code **ppcVar4;
  int iVar5;
  
  iVar3 = DAT_08018920 - (int)DAT_0801891c;
  ppcVar4 = DAT_0801891c;
  for (iVar5 = 0; iVar2 = DAT_08018928, ppcVar1 = DAT_08018924, iVar5 != iVar3 >> 2;
      iVar5 = iVar5 + 1) {
    (**ppcVar4)();
    ppcVar4 = ppcVar4 + 1;
  }
  FUN_0801a51c();
  ppcVar4 = ppcVar1;
  for (iVar3 = 0; iVar3 != iVar2 - (int)ppcVar1 >> 2; iVar3 = iVar3 + 1) {
    (**ppcVar4)();
    ppcVar4 = ppcVar4 + 1;
  }
  return;
}



/* === 0801892c libc_memcpy === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void libc_memcpy(int param_1,undefined *param_2,int param_3)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar2 = param_2 + param_3;
  puVar3 = (undefined *)(param_1 + -1);
  if (param_2 != puVar2) {
    do {
      puVar1 = param_2 + 1;
      puVar3 = puVar3 + 1;
      *puVar3 = *param_2;
      param_2 = puVar1;
    } while (puVar1 != puVar2);
    return;
  }
  return;
}



/* === 08018948 memset === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void memset(undefined *param_1,undefined param_2,int param_3)

{
  undefined *puVar1;
  
  puVar1 = param_1 + param_3;
  for (; param_1 != puVar1; param_1 = param_1 + 1) {
    *param_1 = param_2;
  }
  return;
}



/* === 08018958 newlib_rand_LCG64 === */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

uint newlib_rand_LCG64(void)

{
  longlong lVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  iVar6 = *DAT_080189b4;
  if (*(int *)(iVar6 + 0x38) == 0) {
    puVar3 = (undefined4 *)FUN_08018c24(0x18);
    *(undefined4 **)(iVar6 + 0x38) = puVar3;
    if (puVar3 == (undefined4 *)0x0) {
      puVar3 = (undefined4 *)FUN_080189d4(DAT_080189bc,0x4e,0,DAT_080189b8);
    }
    uVar2 = DAT_080189c4;
    *puVar3 = DAT_080189c0;
    puVar3[1] = uVar2;
    puVar3[2] = DAT_080189c8;
    *(undefined2 *)(puVar3 + 3) = 0xb;
    puVar3[4] = 1;
    puVar3[5] = 0;
  }
  iVar6 = *(int *)(iVar6 + 0x38);
  lVar1 = (ulonglong)*(uint *)(iVar6 + 0x10) * (ulonglong)DAT_080189d0;
  uVar4 = (uint)lVar1;
  uVar5 = DAT_080189d0 * *(int *)(iVar6 + 0x14) + *(uint *)(iVar6 + 0x10) * DAT_080189cc +
          (int)((ulonglong)lVar1 >> 0x20) + (uint)(0xfffffffe < uVar4);
  *(uint *)(iVar6 + 0x10) = uVar4 + 1;
  *(uint *)(iVar6 + 0x14) = uVar5;
  return uVar5 & 0x7fffffff;
}



/* === 080189d4 FUN_080189d4 === */

void FUN_080189d4(void)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)(*DAT_08018a00 + 0xc);
  do {
    FUN_08018bbc(uVar1,DAT_08018a08);
    uVar1 = FUN_080195a8();
  } while( true );
}



/* === 08018a10 FUN_08018a10 === */

void FUN_08018a10(undefined4 *param_1,undefined2 param_2,undefined2 param_3)

{
  undefined4 uVar1;
  
  *param_1 = 0;
  param_1[1] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[2] = 0;
  *(undefined2 *)(param_1 + 3) = param_2;
  param_1[0x19] = 0;
  *(undefined2 *)((int)param_1 + 0xe) = param_3;
  param_1[6] = 0;
  memset(param_1 + 0x17,0,8);
  param_1[9] = DAT_08018a48;
  param_1[10] = DAT_08018a4c;
  param_1[0xb] = DAT_08018a50;
  uVar1 = DAT_08018a54;
  param_1[8] = param_1;
  param_1[0xc] = uVar1;
  return;
}



/* === 08018a64 FUN_08018a64 === */

undefined4 * FUN_08018a64(undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = (param_2 + -1) * 0x68;
  puVar1 = (undefined4 *)FUN_08018ccc(param_1,iVar2 + 0x74);
  if (puVar1 != (undefined4 *)0x0) {
    *puVar1 = 0;
    puVar1[1] = param_2;
    puVar1[2] = puVar1 + 3;
    memset(puVar1 + 3,0,iVar2 + 0x68);
  }
  return puVar1;
}



/* === 08018a90 FUN_08018a90 === */

void FUN_08018a90(void)

{
  FUN_08018c20(DAT_08018a98);
  return;
}



/* === 08018a9c FUN_08018a9c === */

void FUN_08018a9c(void)

{
  FUN_08018c22(DAT_08018aa4);
  return;
}



/* === 08018aa8 FUN_08018aa8 === */

void FUN_08018aa8(void)

{
  FUN_08018c20(DAT_08018ab0);
  return;
}



/* === 08018ac0 FUN_08018ac0 === */

void FUN_08018ac0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  bool bVar3;
  
  FUN_08018aa8();
  if (*(int *)(param_1 + 0x18) == 0) {
    *(undefined4 *)(param_1 + 0x48) = 0;
    *(undefined4 *)(param_1 + 0x4c) = 0;
    *(undefined4 *)(param_1 + 0x50) = 0;
    iVar2 = *DAT_08018b28;
    *(undefined4 *)(param_1 + 0x28) = DAT_08018b2c;
    bVar3 = iVar2 == param_1;
    if (bVar3) {
      iVar2 = 1;
    }
    if (bVar3) {
      *(int *)(param_1 + 0x18) = iVar2;
    }
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 4) = uVar1;
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 8) = uVar1;
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 0xc) = uVar1;
    FUN_08018a10(*(undefined4 *)(param_1 + 4),4,0);
    FUN_08018a10(*(undefined4 *)(param_1 + 8),9,1);
    FUN_08018a10(*(undefined4 *)(param_1 + 0xc),0x12,2);
    *(undefined4 *)(param_1 + 0x18) = 1;
  }
  FUN_08018c22(DAT_08018abc);
  return;
}



/* === 08018b30 FUN_08018b30 === */

int * FUN_08018b30(undefined4 *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int **ppiVar4;
  
  FUN_08018a90();
  iVar3 = *DAT_08018bb4;
  if (*(int *)(iVar3 + 0x18) == 0) {
    FUN_08018ac0(iVar3);
  }
  ppiVar4 = (int **)(iVar3 + 0x48);
  do {
    piVar1 = ppiVar4[1];
    piVar2 = ppiVar4[2];
    while (piVar1 = (int *)((int)piVar1 + -1), -1 < (int)piVar1) {
      if (*(short *)(piVar2 + 3) == 0) {
        piVar2[3] = DAT_08018bb8;
        piVar2[0x19] = 0;
        FUN_08018c1e(piVar2 + 0x16);
        FUN_08018a9c();
        piVar2[1] = 0;
        piVar2[2] = 0;
        piVar2[4] = 0;
        piVar2[5] = 0;
        *piVar2 = 0;
        piVar2[6] = 0;
        memset(piVar2 + 0x17,0,8);
        piVar2[0xd] = 0;
        piVar2[0xe] = 0;
        piVar2[0x12] = 0;
        piVar2[0x13] = 0;
        return piVar2;
      }
      piVar2 = piVar2 + 0x1a;
    }
    if (*ppiVar4 == (int *)0x0) {
      piVar1 = (int *)FUN_08018a64(param_1,4);
      *ppiVar4 = piVar1;
      if (piVar1 == (int *)0x0) {
        FUN_08018a9c();
        *param_1 = 0xc;
        return (int *)0x0;
      }
    }
    ppiVar4 = (int **)*ppiVar4;
  } while( true );
}



/* === 08018bbc FUN_08018bbc === */

void FUN_08018bbc(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uStack_8 = param_3;
  uStack_4 = param_4;
  FUN_08018dd4(*DAT_08018bdc,param_1,param_2,&uStack_8,param_1,&uStack_8);
  return;
}



/* === 08018c1e FUN_08018c1e === */

void FUN_08018c1e(void)

{
  return;
}



/* === 08018c20 FUN_08018c20 === */

void FUN_08018c20(void)

{
  return;
}



/* === 08018c22 FUN_08018c22 === */

void FUN_08018c22(void)

{
  return;
}



/* === 08018c24 FUN_08018c24 === */

void FUN_08018c24(undefined4 param_1)

{
  FUN_08018ccc(*DAT_08018c30,param_1);
  return;
}



/* === 08018c34 FUN_08018c34 === */

void FUN_08018c34(undefined4 *param_1,int param_2)

{
  int **ppiVar1;
  int iVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  int iVar6;
  int *piVar7;
  bool bVar8;
  
  if (param_2 == 0) {
    return;
  }
  piVar5 = (int *)(param_2 + -4);
  if (*(int *)(param_2 + -4) < 0) {
    piVar5 = (int *)((int)piVar5 + *(int *)(param_2 + -4));
  }
  FUN_0801984c();
  ppiVar1 = DAT_08018cc8;
  piVar3 = *DAT_08018cc8;
  if (piVar3 != (int *)0x0) {
    if (piVar3 <= piVar5) {
      do {
        piVar4 = piVar3;
        piVar3 = (int *)piVar4[1];
        if (piVar3 == (int *)0x0) break;
      } while (piVar3 <= piVar5);
      piVar7 = (int *)((int)piVar4 + *piVar4);
      if (piVar7 == piVar5) {
        iVar2 = *piVar4 + *piVar5;
        *piVar4 = iVar2;
        if (piVar3 == (int *)((int)piVar4 + iVar2)) {
          iVar6 = *piVar3;
          piVar4[1] = piVar3[1];
          *piVar4 = iVar2 + iVar6;
        }
      }
      else if (piVar5 < piVar7) {
        *param_1 = 0xc;
      }
      else {
        piVar7 = (int *)((int)piVar5 + *piVar5);
        bVar8 = piVar3 == piVar7;
        if (bVar8) {
          piVar7 = (int *)*piVar3;
          piVar3 = (int *)piVar3[1];
        }
        piVar5[1] = (int)piVar3;
        if (bVar8) {
          *piVar5 = (int)piVar7 + *piVar5;
        }
        piVar4[1] = (int)piVar5;
      }
      goto LAB_08018c5a;
    }
    if (piVar3 == (int *)((int)piVar5 + *piVar5)) {
      iVar2 = *piVar3;
      piVar3 = (int *)piVar3[1];
      *piVar5 = iVar2 + *piVar5;
    }
  }
  piVar5[1] = (int)piVar3;
  *ppiVar1 = piVar5;
LAB_08018c5a:
  FUN_08019858();
  return;
}



/* === 08018ccc FUN_08018ccc === */

uint FUN_08018ccc(undefined4 *param_1,uint param_2,undefined4 param_3,undefined4 param_4)

{
  int *piVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint uVar7;
  
  uVar7 = (param_2 + 3 & 0xfffffffc) + 8;
  if (uVar7 < 0xc) {
    uVar7 = 0xc;
  }
  if (((int)uVar7 < 0) || (uVar7 < param_2)) {
    *param_1 = 0xc;
  }
  else {
    FUN_0801984c();
    piVar1 = DAT_08018d7c;
    puVar3 = *DAT_08018d78;
    for (puVar6 = *DAT_08018d78; puVar6 != (uint *)0x0; puVar6 = (uint *)puVar6[1]) {
      uVar4 = *puVar6 - uVar7;
      if (-1 < (int)uVar4) {
        if (0xb < uVar4) {
          *puVar6 = uVar4;
          puVar6 = (uint *)((int)puVar6 + uVar4);
          goto LAB_08018d30;
        }
        puVar5 = (uint *)puVar6[1];
        if (puVar3 == puVar6) {
          *DAT_08018d78 = puVar5;
        }
        if (puVar3 != puVar6) {
          puVar3[1] = (uint)puVar5;
        }
        goto LAB_08018d3e;
      }
      puVar3 = puVar6;
    }
    if (*DAT_08018d7c == 0) {
      iVar2 = FUN_0801935c(param_1,0,puVar3,0,param_4);
      *piVar1 = iVar2;
    }
    puVar3 = (uint *)FUN_0801935c(param_1,uVar7);
    if ((puVar3 != (uint *)0xffffffff) &&
       ((puVar6 = (uint *)((int)puVar3 + 3U & 0xfffffffc), puVar3 == puVar6 ||
        (iVar2 = FUN_0801935c(param_1,(int)puVar6 - (int)puVar3), iVar2 != -1)))) {
LAB_08018d30:
      *puVar6 = uVar7;
LAB_08018d3e:
      FUN_08019858(param_1);
      uVar7 = (int)puVar6 + 0xbU & 0xfffffff8;
      iVar2 = uVar7 - (int)(puVar6 + 1);
      if (iVar2 == 0) {
        return uVar7;
      }
      *(uint *)((int)puVar6 + iVar2) = (int)(puVar6 + 1) - uVar7;
      return uVar7;
    }
    *param_1 = 0xc;
    FUN_08019858(param_1);
  }
  return 0;
}



/* === 08018d80 FUN_08018d80 === */

uint FUN_08018d80(int param_1,uint param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined *puVar4;
  
  iVar2 = param_3[2] + -1;
  param_3[2] = iVar2;
  if ((-1 < iVar2) || ((param_3[6] <= iVar2 && (param_2 != 10)))) {
    puVar4 = (undefined *)*param_3;
    *param_3 = (int)(puVar4 + 1);
    *puVar4 = (char)param_2;
    return param_2;
  }
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    FUN_08018ac0();
  }
  if (param_3 == DAT_0801949c) {
    param_3 = *(int **)(param_1 + 4);
  }
  else if (param_3 == DAT_080194a0) {
    param_3 = *(int **)(param_1 + 8);
  }
  else if (param_3 == DAT_080194a4) {
    param_3 = *(int **)(param_1 + 0xc);
  }
  param_3[2] = param_3[6];
  uVar3 = (uint)*(ushort *)(param_3 + 3);
  iVar1 = uVar3 << 0x1c;
  if (((iVar1 < 0) && (uVar3 = param_3[4], uVar3 != 0)) ||
     (iVar2 = FUN_080194cc(param_1,param_3,iVar1,uVar3,iVar2), iVar2 == 0)) {
    iVar2 = *param_3 - param_3[4];
    uVar3 = param_2 & 0xff;
    if ((iVar2 < param_3[5]) || (iVar2 = FUN_080196e4(param_1,param_3), iVar2 == 0)) {
      param_3[2] = param_3[2] + -1;
      puVar4 = (undefined *)*param_3;
      *param_3 = (int)(puVar4 + 1);
      *puVar4 = (char)param_2;
      if (param_3[5] != iVar2 + 1) {
        if (-1 < (int)((uint)*(ushort *)(param_3 + 3) << 0x1f)) {
          return uVar3;
        }
        if (uVar3 != 10) {
          return uVar3;
        }
      }
      iVar2 = FUN_080196e4(param_1,param_3);
      if (iVar2 == 0) {
        return uVar3;
      }
    }
  }
  return 0xffffffff;
}



/* === 08018dae FUN_08018dae === */

int FUN_08018dae(undefined4 param_1,undefined4 param_2,undefined *param_3,int param_4)

{
  int iVar1;
  undefined *puVar2;
  int iVar3;
  
  puVar2 = param_3 + param_4;
  iVar3 = param_4;
  do {
    if (param_3 == puVar2) {
      return 0;
    }
    iVar1 = FUN_08018d80(param_1,*param_3,param_2,param_4,iVar3);
    param_4 = iVar1 + 1;
    param_3 = param_3 + 1;
  } while (param_4 != 0);
  return iVar1;
}



/* === 08018dd4 FUN_08018dd4 === */

int FUN_08018dd4(int param_1,int param_2,byte *param_3,int *param_4)

{
  int iVar1;
  int iVar2;
  int *piVar3;
  bool bVar4;
  byte *pbVar5;
  int unaff_r7;
  byte *pbVar6;
  int iVar7;
  int *local_8c;
  uint local_88;
  int local_84;
  undefined4 uStack_80;
  int local_7c;
  int local_74;
  byte local_70;
  undefined local_6f;
  undefined local_6e;
  undefined local_45;
  undefined4 local_30;
  
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    FUN_08018ac0();
  }
  if (param_2 == DAT_08019014) {
    param_2 = *(int *)(param_1 + 4);
  }
  else if (param_2 == DAT_08019018) {
    param_2 = *(int *)(param_1 + 8);
  }
  else if (param_2 == DAT_0801901c) {
    param_2 = *(int *)(param_1 + 0xc);
  }
  if ((-1 < *(int *)(param_2 + 100) << 0x1f) &&
     (-1 < (int)((uint)*(ushort *)(param_2 + 0xc) << 0x16))) {
    FUN_08018c20(*(undefined4 *)(param_2 + 0x58));
  }
  if (((-1 < (int)((uint)*(ushort *)(param_2 + 0xc) << 0x1c)) || (*(int *)(param_2 + 0x10) == 0)) &&
     (iVar1 = FUN_080194cc(param_1,param_2), iVar1 != 0)) {
    if ((-1 < *(int *)(param_2 + 100) << 0x1f) &&
       (-1 < (int)((uint)*(ushort *)(param_2 + 0xc) << 0x16))) {
      FUN_08018c22(*(undefined4 *)(param_2 + 0x58));
    }
    return -1;
  }
  iVar1 = DAT_08019020;
  local_74 = 0;
  local_6f = 0x20;
  local_6e = 0x30;
  pbVar6 = param_3;
  local_8c = param_4;
LAB_08018e6a:
  pbVar5 = pbVar6;
  if (*pbVar5 != 0) goto code_r0x08018e72;
  goto LAB_08018e76;
code_r0x08018e72:
  pbVar6 = pbVar5 + 1;
  if (*pbVar5 != 0x25) goto LAB_08018e6a;
LAB_08018e76:
  iVar7 = (int)pbVar5 - (int)param_3;
  if (iVar7 != 0) {
    iVar2 = FUN_08018dae(param_1,param_2,param_3,iVar7);
    if (iVar2