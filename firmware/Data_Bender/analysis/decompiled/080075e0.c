/* 080075e0 FUN_080075e0; analyst naming is provisional. */

void FUN_080075e0(int *param_1)

{
  int iVar1;
  int iVar2;
  
  if (*param_1 != DAT_08007624) {
    return;
  }
  *(uint *)(DAT_08007628 + 0xd8) = *(uint *)(DAT_08007628 + 0xd8) | 0x20;
  FUN_08007190();
  iVar1 = DAT_0800762c;
  iVar2 = DAT_0800762c + 0x58c;
  *(int *)(DAT_0800762c + 0x5d8) = DAT_0800762c + 0x5f0;
  *(int *)(iVar1 + 0x628) = iVar2;
  return;
}


