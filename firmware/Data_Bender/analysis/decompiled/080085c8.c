/* 080085c8 FUN_080085c8; analyst naming is provisional. */

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


