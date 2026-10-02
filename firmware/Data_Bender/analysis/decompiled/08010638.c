/* 08010638 FUN_08010638; analyst naming is provisional. */

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


