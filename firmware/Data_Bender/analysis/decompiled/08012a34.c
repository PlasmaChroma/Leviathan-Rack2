/* 08012a34 FUN_08012a34; analyst naming is provisional. */

void FUN_08012a34(int param_1,undefined4 *param_2,uint param_3)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  uint uVar4;
  uint uVar5;
  
  if (param_3 >> 2 != 0) {
    uVar5 = 0;
    puVar2 = param_2;
    do {
      uVar5 = uVar5 + 1;
      *puVar2 = *(undefined4 *)(param_1 + 0x1000);
      puVar2 = puVar2 + 1;
    } while (param_3 >> 2 != uVar5);
    param_2 = (undefined4 *)((int)param_2 + (param_3 & 0xfffffffc));
  }
  if ((param_3 & 3) != 0) {
    uVar4 = 0;
    uVar5 = *(uint *)(param_1 + 0x1000);
    puVar2 = param_2;
    do {
      uVar1 = uVar4 & 0xff;
      uVar4 = uVar4 + 8;
      puVar3 = (undefined4 *)((int)puVar2 + 1);
      *(char *)puVar2 = (char)(uVar5 >> uVar1);
      puVar2 = puVar3;
    } while (puVar3 != (undefined4 *)((param_3 & 3) + (int)param_2));
  }
  return;
}


