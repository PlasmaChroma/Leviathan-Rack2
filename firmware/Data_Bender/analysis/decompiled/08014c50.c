/* 08014c50 FUN_08014c50; analyst naming is provisional. */

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


