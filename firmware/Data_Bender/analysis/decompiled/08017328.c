/* 08017328 FUN_08017328; analyst naming is provisional. */

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


