/* 08016678 FUN_08016678; analyst naming is provisional. */

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


