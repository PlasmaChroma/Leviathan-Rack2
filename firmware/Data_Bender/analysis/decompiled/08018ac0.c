/* 08018ac0 FUN_08018ac0; analyst naming is provisional. */

void FUN_08018ac0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  bool bVar3;
  
  FUN_08018aa8();
  if (*(int *)(param_1 + 0x18) == 0) {
    *(undefined4 *)(param_1 + 0x48) = 0;
    *(undefined4 *)(param_1 + 0x4c) = 0;
    *(undefined4 *)(param_1 + 0x50) = 0;
    iVar2 = *DAT_08018b28;
    *(undefined4 *)(param_1 + 0x28) = DAT_08018b2c;
    bVar3 = iVar2 == param_1;
    if (bVar3) {
      iVar2 = 1;
    }
    if (bVar3) {
      *(int *)(param_1 + 0x18) = iVar2;
    }
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 4) = uVar1;
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 8) = uVar1;
    uVar1 = FUN_08018b30(param_1);
    *(undefined4 *)(param_1 + 0xc) = uVar1;
    FUN_08018a10(*(undefined4 *)(param_1 + 4),4,0);
    FUN_08018a10(*(undefined4 *)(param_1 + 8),9,1);
    FUN_08018a10(*(undefined4 *)(param_1 + 0xc),0x12,2);
    *(undefined4 *)(param_1 + 0x18) = 1;
  }
  FUN_08018c22(DAT_08018abc);
  return;
}


