/* 08016bd8 FUN_08016bd8; analyst naming is provisional. */

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


