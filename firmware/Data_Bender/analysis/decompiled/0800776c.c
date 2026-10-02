/* 0800776c FUN_0800776c; analyst naming is provisional. */

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


