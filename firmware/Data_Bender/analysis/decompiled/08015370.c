/* 08015370 FUN_08015370; analyst naming is provisional. */

void FUN_08015370(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = DAT_0801546c;
  *(uint *)(DAT_0801546c + 0xd8) = *(uint *)(DAT_0801546c + 0xd8) | 1;
  uVar2 = *(uint *)(iVar1 + 0xd8);
  *(uint *)(iVar1 + 0xd8) = *(uint *)(iVar1 + 0xd8) | 2;
  FUN_0800a968(0xb,0,0,*(uint *)(iVar1 + 0xd8) & 2,uVar2 & 1);
  FUN_0800a9e4(0xb);
  FUN_0800a968(0xc,0);
  FUN_0800a9e4(0xc);
  FUN_0800a968(0xd,0);
  FUN_0800a9e4(0xd);
  FUN_0800a968(0xe,0);
  FUN_0800a9e4(0xe);
  FUN_0800a968(0xf,0);
  FUN_0800a9e4(0xf);
  FUN_0800a968(0x10,0);
  FUN_0800a9e4(0x10);
  FUN_0800a968(0x3c,0);
  FUN_0800a9e4(0x3c);
  FUN_0800a968(0x11,0);
  FUN_0800a9e4(0x11);
  FUN_0800a968(0x38,0);
  FUN_0800a9e4(0x38);
  FUN_0800a968(0x39,0);
  FUN_0800a9e4(0x39);
  FUN_0800a968(0x3a,0);
  FUN_0800a9e4(0x3a);
  FUN_0800a968(0x3b,0);
  FUN_0800a9e4(0x3b);
  return;
}


