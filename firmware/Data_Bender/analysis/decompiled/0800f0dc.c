/* 0800f0dc FUN_0800f0dc; analyst naming is provisional. */

void FUN_0800f0dc(int *param_1,uint *param_2,uint param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = param_2[9];
  if ((uVar4 != 0) && (param_3 != 0xc000000)) {
    *(uint *)(*param_1 + 0x10) = param_2[10] - 1;
  }
  uVar2 = param_2[6];
  if (uVar2 == 0) {
    if (param_2[8] == 0) {
      if (param_2[7] == 0) {
        if (uVar4 == 0) {
          return;
        }
        *(uint *)(*param_1 + 0x14) =
             param_2[0xb] | uVar4 | param_3 | param_2[0xc] | param_2[0xd] | param_2[5] << 0x12;
        return;
      }
      iVar1 = *param_1;
      uVar4 = param_2[7] | uVar4 | param_3 | param_2[0xb] | param_2[0xc];
      uVar2 = param_2[0xd];
    }
    else {
      iVar1 = *param_1;
      uVar4 = param_2[8] | uVar4;
      uVar2 = param_2[7];
      *(uint *)(iVar1 + 0x1c) = param_2[2];
      if (uVar2 == 0) {
        *(uint *)(iVar1 + 0x14) =
             uVar4 | param_3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | param_2[4] |
             param_2[5] << 0x12;
        return;
      }
      uVar4 = uVar4 | param_3 | uVar2 | param_2[0xb] | param_2[0xc] | param_2[0xd];
      uVar2 = param_2[4];
    }
    uVar4 = uVar4 | uVar2;
    uVar2 = param_2[3];
  }
  else {
    uVar3 = param_2[8];
    if (uVar3 == 0) {
      if (param_2[7] == 0) {
        *(uint *)(*param_1 + 0x14) =
             uVar4 | uVar2 | param_3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | *param_2 |
             param_2[5] << 0x12;
        return;
      }
      iVar1 = *param_1;
      *(uint *)(iVar1 + 0x14) =
           uVar4 | uVar2 | param_3 | param_2[7] | param_2[0xb] | param_2[0xc] | param_2[0xd] |
           param_2[3] | *param_2 | param_2[5] << 0x12;
      if (param_3 == 0xc000000) {
        return;
      }
      *(uint *)(iVar1 + 0x18) = param_2[1];
      return;
    }
    iVar1 = *param_1;
    *(uint *)(iVar1 + 0x1c) = param_2[2];
    if (param_2[7] == 0) {
      *(uint *)(iVar1 + 0x14) =
           uVar4 | uVar2 | param_3 | uVar3 | param_2[0xb] | param_2[0xc] | param_2[0xd] | param_2[4]
           | *param_2 | param_2[5] << 0x12;
      return;
    }
    uVar4 = uVar2 | uVar4 | param_3 | uVar3 | param_2[7] | param_2[0xb] | param_2[0xc] |
            param_2[0xd] | param_2[4] | param_2[3];
    uVar2 = *param_2;
  }
  *(uint *)(iVar1 + 0x14) = uVar4 | uVar2 | param_2[5] << 0x12;
  if (param_3 == 0xc000000) {
    return;
  }
  *(uint *)(iVar1 + 0x18) = param_2[1];
  return;
}


