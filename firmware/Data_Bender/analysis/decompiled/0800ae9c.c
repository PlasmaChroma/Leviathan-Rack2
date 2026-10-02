/* 0800ae9c FUN_0800ae9c; analyst naming is provisional. */

void FUN_0800ae9c(int *param_1)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(param_1 + 1);
  if (7 < uVar2 - 1) {
    return;
  }
  iVar1 = *param_1;
  if ((iVar1 == DAT_0800af28 + 100 ||
       (iVar1 == DAT_0800af28 + 0x50 ||
       (iVar1 == DAT_0800af28 + 0x3c ||
       (iVar1 == DAT_0800af28 + 0x28 ||
       (iVar1 == DAT_0800af28 + 0x14 || (iVar1 == DAT_0800af28 || iVar1 == DAT_0800af24)))))) ||
     (iVar1 == DAT_0800af2c)) {
    iVar1 = DAT_0800af30 + uVar2;
    param_1[0x1c] = DAT_0800af34;
    param_1[0x1b] = iVar1 * 4;
  }
  else {
    iVar1 = DAT_0800af38 + uVar2;
    param_1[0x1c] = DAT_0800af3c;
    param_1[0x1b] = iVar1 * 4;
  }
  param_1[0x1d] = 1 << (uVar2 - 1 & 0xff);
  return;
}


