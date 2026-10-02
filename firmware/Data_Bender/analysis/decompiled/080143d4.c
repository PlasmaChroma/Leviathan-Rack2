/* 080143d4 FUN_080143d4; analyst naming is provisional. */

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


