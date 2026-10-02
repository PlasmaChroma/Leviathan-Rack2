/* 08017434 FUN_08017434; analyst naming is provisional. */

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


