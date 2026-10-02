/* 0800e590 FUN_0800e590; analyst naming is provisional. */

void FUN_0800e590(int param_1)

{
  ushort uVar1;
  undefined2 uVar2;
  int iVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  uint **ppuVar7;
  
  ppuVar7 = *(uint ***)(param_1 + 0x38);
  puVar4 = *ppuVar7;
  *puVar4 = *puVar4 & 0xffff7fff;
  if (*(short *)((int)ppuVar7 + 0x2a) == 0) {
    if ((ppuVar7[0xd] == DAT_0800e620) || (ppuVar7[0xd] == DAT_0800e624)) {
      uVar5 = 0x60;
    }
    else {
      uVar5 = 0x20;
    }
    *puVar4 = *puVar4 | uVar5;
    return;
  }
  puVar6 = ppuVar7[9];
  uVar1 = *(ushort *)(ppuVar7 + 10);
  ppuVar7[9] = (uint *)((int)puVar6 + (uint)uVar1);
  if (*(ushort *)((int)ppuVar7 + 0x2a) < 0x100) {
    uVar2 = *(undefined2 *)((int)ppuVar7 + 0x2a);
  }
  else {
    uVar2 = 0xff;
  }
  *(undefined2 *)(ppuVar7 + 10) = uVar2;
  iVar3 = FUN_0800b3a0(ppuVar7[0xf],puVar4 + 9,(uint *)((int)puVar6 + (uint)uVar1),uVar2);
  if (iVar3 == 0) {
    if ((ppuVar7[0xd] == DAT_0800e620) || (ppuVar7[0xd] == DAT_0800e624)) {
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


