/* 08013dc4 FUN_08013dc4; analyst naming is provisional. */

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


