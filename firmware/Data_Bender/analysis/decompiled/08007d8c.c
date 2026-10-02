/* 08007d8c FUN_08007d8c; analyst naming is provisional. */

void FUN_08007d8c(int *param_1)

{
  byte bVar1;
  int iVar2;
  int local_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = 0;
  local_18 = 0x12;
  uStack_14 = 0;
  iVar2 = *param_1;
  if (iVar2 < 3) {
    if (-1 < iVar2) {
      local_c = 4;
    }
  }
  else if (iVar2 == 3) {
    local_c = 6;
    bVar1 = *(byte *)(param_1 + 1);
    goto joined_r0x08007dee;
  }
  bVar1 = *(byte *)(param_1 + 1);
joined_r0x08007dee:
  if (bVar1 < 0xb) {
    iVar2 = DAT_08007df8 + (uint)bVar1 * 0x400;
  }
  else {
    iVar2 = 0;
  }
  local_1c = 1 << (uint)*(byte *)((int)param_1 + 5);
  FUN_0800c080(iVar2,&local_1c);
  if (*(byte *)((int)param_1 + 6) < 0xb) {
    iVar2 = DAT_08007df8 + (uint)*(byte *)((int)param_1 + 6) * 0x400;
  }
  else {
    iVar2 = 0;
  }
  local_1c = 1 << (uint)*(byte *)((int)param_1 + 7);
  FUN_0800c080(iVar2,&local_1c);
  return;
}


