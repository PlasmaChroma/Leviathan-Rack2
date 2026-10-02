/* 0800e4f8 FUN_0800e4f8; analyst naming is provisional. */

void FUN_0800e4f8(int param_1)

{
  ushort uVar1;
  undefined2 uVar2;
  int iVar3;
  uint uVar4;
  uint *puVar5;
  uint *puVar6;
  uint **ppuVar7;
  
  ppuVar7 = *(uint ***)(param_1 + 0x38);
  puVar6 = *ppuVar7;
  *puVar6 = *puVar6 & 0xffffbfff;
  if (*(short *)((int)ppuVar7 + 0x2a) == 0) {
    if ((ppuVar7[0xd] == DAT_0800e588) || (ppuVar7[0xd] == DAT_0800e58c)) {
      uVar4 = 0x60;
    }
    else {
      uVar4 = 0x20;
    }
    *puVar6 = *puVar6 | uVar4;
    return;
  }
  puVar5 = ppuVar7[9];
  uVar1 = *(ushort *)(ppuVar7 + 10);
  ppuVar7[9] = (uint *)((int)puVar5 + (uint)uVar1);
  if (*(ushort *)((int)ppuVar7 + 0x2a) < 0x100) {
    uVar2 = *(undefined2 *)((int)ppuVar7 + 0x2a);
  }
  else {
    uVar2 = 0xff;
  }
  *(undefined2 *)(ppuVar7 + 10) = uVar2;
  iVar3 = FUN_0800b3a0(ppuVar7[0xe],(uint *)((int)puVar5 + (uint)uVar1),puVar6 + 10,uVar2);
  if (iVar3 == 0) {
    if ((ppuVar7[0xd] == DAT_0800e588) || (ppuVar7[0xd] == DAT_0800e58c)) {
      **ppuVar7 = **ppuVar7 | 0x40;
    }
    else {
      **ppuVar7 = **ppuVar7;
    }
    return;
  }
  FUN_0800db1c(ppuVar7,0x10);
  return;
}


