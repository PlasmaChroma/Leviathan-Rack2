/* 08014bf4 FUN_08014bf4; analyst naming is provisional. */

undefined4 FUN_08014bf4(int param_1,uint param_2,int param_3)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar1 = *(byte **)(DAT_08014c48 + param_3 * 4);
  pbVar2 = pbVar1 + 9;
  while (((((uint)*pbVar1 == (uint)*DAT_08014c4c && (pbVar1[1] == DAT_08014c4c[1])) ||
          ((uint)*pbVar1 != (param_2 & 0xff))) || ((uint)pbVar1[1] != (param_2 << 0x10) >> 0x18))) {
    pbVar1 = pbVar1 + 3;
    if (pbVar1 == pbVar2) {
      return 1;
    }
  }
  *(uint *)(param_1 + 0x10) = (uint)pbVar1[2];
  return 0;
}


