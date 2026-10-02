/* 0800d9e8 FUN_0800d9e8; analyst naming is provisional. */

void FUN_0800d9e8(uint **param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  
  if ((*(byte *)((int)param_1 + 0x41) & 0x28) != 0x28) {
    (*param_1)[7] = 8;
    *(undefined *)(param_1 + 0x10) = 0;
    return;
  }
  puVar3 = *param_1;
  uVar4 = puVar3[2];
  uVar1 = (puVar3[6] << 0xf) >> 0x1f;
  uVar2 = puVar3[6] >> 0x10 & 0xfe;
  if (param_1[3] == (uint *)0x2) {
    if (((uVar2 ^ uVar4 >> 7) & 6) == 0) {
      param_1[0x12] = (uint *)((int)param_1[0x12] + 1);
      if (param_1[0x12] != (uint *)0x2) {
        return;
      }
      param_1[0x12] = (uint *)0x0;
      puVar3[7] = 8;
      *(undefined *)(param_1 + 0x10) = 0;
      FUN_0800d9e4(param_1,uVar1,uVar4 & 0x3ff,param_1,param_4);
      return;
    }
    uVar2 = puVar3[3] & 0xfe;
  }
  *puVar3 = *puVar3 & 0xffffff47;
  *(undefined *)(param_1 + 0x10) = 0;
  FUN_0800d9e4(param_1,uVar1,uVar2,param_1,param_4);
  return;
}


