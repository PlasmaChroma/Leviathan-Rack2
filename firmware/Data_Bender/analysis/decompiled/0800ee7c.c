/* 0800ee7c FUN_0800ee7c; analyst naming is provisional. */

undefined4 FUN_0800ee7c(undefined4 *param_1,uint param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  undefined4 *puVar2;
  uint uVar3;
  
  uVar3 = param_2 & 0xf;
  if ((int)(param_2 << 0x18) < 0) {
    iVar1 = uVar3 * 9 + 0xf;
    *(undefined *)((int)param_1 + uVar3 * 0x24 + 0x3d) = 1;
  }
  else {
    iVar1 = uVar3 * 9 + 0x9f;
    *(undefined *)((int)param_1 + uVar3 * 0x24 + 0x27d) = 0;
  }
  puVar2 = param_1 + iVar1;
  puVar2[2] = param_3;
  *(char *)puVar2 = (char)uVar3;
  *(char *)(puVar2 + 1) = (char)param_4;
  if (*(char *)((int)puVar2 + 1) != '\0') {
    *(short *)((int)puVar2 + 0x1a) = (short)uVar3;
  }
  if (param_4 == 2) {
    *(undefined *)((int)puVar2 + 5) = 0;
  }
  if (*(char *)(param_1 + 0x12f) != '\x01') {
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012670(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}


