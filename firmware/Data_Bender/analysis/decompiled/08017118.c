/* 08017118 FUN_08017118; analyst naming is provisional. */

void FUN_08017118(int *param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = param_1[10];
  if (iVar2 << 0x1c < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xffff7fff | param_1[0xe];
  }
  if (iVar2 << 0x1f < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffdffff | param_1[0xb];
  }
  if (iVar2 << 0x1e < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffeffff | param_1[0xc];
  }
  if (iVar2 << 0x1d < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfffbffff | param_1[0xd];
  }
  if (iVar2 << 0x1b < 0) {
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xffffefff | param_1[0xf];
  }
  if (iVar2 << 0x1a < 0) {
    *(uint *)(*param_1 + 8) = *(uint *)(*param_1 + 8) & 0xffffdfff | param_1[0x10];
  }
  if (iVar2 << 0x19 < 0) {
    iVar1 = *param_1;
    uVar3 = param_1[0x11];
    *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) & 0xffefffff | uVar3;
    if (uVar3 == 0x100000) {
      *(uint *)(iVar1 + 4) = *(uint *)(iVar1 + 4) & 0xff9fffff | param_1[0x12];
    }
  }
  if (iVar2 << 0x18 < 0) {
    *(uint *)(*param_1 + 4) = *(uint *)(*param_1 + 4) & 0xfff7ffff | param_1[0x13];
  }
  return;
}


