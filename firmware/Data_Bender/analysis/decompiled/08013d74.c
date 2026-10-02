/* 08013d74 FUN_08013d74; analyst naming is provisional. */

undefined4 FUN_08013d74(int param_1,uint param_2,int param_3)

{
  byte *pbVar1;
  byte *pbVar2;
  
  pbVar2 = *(byte **)(DAT_08013dc0 + param_3 * 4);
  pbVar1 = pbVar2 + 9;
  do {
    if (*pbVar2 == 0xb) {
      if ((pbVar2[1] != 0) && ((param_2 & 0xff) == 0xb)) goto LAB_08013dae;
    }
    else if ((uint)*pbVar2 == (param_2 & 0xff)) {
LAB_08013dae:
      if ((uint)pbVar2[1] == (param_2 << 0x10) >> 0x18) {
        *(uint *)(param_1 + 0x10) = (uint)pbVar2[2];
        return 0;
      }
    }
    pbVar2 = pbVar2 + 3;
    if (pbVar2 == pbVar1) {
      return 1;
    }
  } while( true );
}


