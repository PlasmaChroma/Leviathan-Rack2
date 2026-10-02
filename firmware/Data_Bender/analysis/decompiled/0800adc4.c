/* 0800adc4 FUN_0800adc4; analyst naming is provisional. */

void FUN_0800adc4(uint *param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  iVar1 = DAT_0800ae84;
  uVar2 = *param_1;
  if ((uVar2 == DAT_0800ae70 + 0x50 ||
       (uVar2 == DAT_0800ae78 + 0x28 ||
       (uVar2 == DAT_0800ae70 + 0x28 ||
       (uVar2 == DAT_0800ae78 ||
       (uVar2 == DAT_0800ae70 || (uVar2 == DAT_0800ae74 || uVar2 == DAT_0800ae6c)))))) ||
     (uVar2 == DAT_0800ae7c)) {
    uVar2 = (uint)((ulonglong)DAT_0800ae80 * (ulonglong)((uVar2 & 0xff) - 8) >> 0x20);
    param_1[0x19] = DAT_0800ae88;
    param_1[0x18] = (iVar1 + (uVar2 >> 4)) * 4;
    param_1[0x1a] = 1 << ((uVar2 << 0x17) >> 0x1b);
  }
  else {
    uVar3 = (uint)((ulonglong)DAT_0800ae90 * (ulonglong)((uVar2 & 0xff) - 0x10) >> 0x24);
    if (DAT_0800ae8c + uVar2 < 0xa9) {
      uVar3 = uVar3 + 8;
    }
    iVar1 = DAT_0800ae94 + uVar3;
    param_1[0x19] = DAT_0800ae98;
    param_1[0x1a] = 1 << (uVar3 & 0x1f);
    param_1[0x18] = iVar1 * 4;
  }
  return;
}


