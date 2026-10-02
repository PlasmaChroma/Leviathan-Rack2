/* 0800f008 FUN_0800f008; analyst naming is provisional. */

undefined4 FUN_0800f008(undefined4 *param_1,uint param_2)

{
  char cVar1;
  uint uVar2;
  
  uVar2 = param_2 & 0xf;
  if ((uint)param_1[1] < uVar2) {
    return 1;
  }
  if ((param_2 & 0x80) == 0) {
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x27d) = 0;
    *(char *)(param_1 + uVar2 * 9 + 0x9f) = (char)uVar2;
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x27e) = 0;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  else {
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3d) = 1;
    *(char *)(param_1 + uVar2 * 9 + 0xf) = (char)uVar2;
    *(undefined *)((int)param_1 + uVar2 * 0x24 + 0x3e) = 0;
    cVar1 = *(char *)(param_1 + 0x12f);
  }
  if (cVar1 != '\x01') {
    *(undefined *)(param_1 + 0x12f) = 1;
    FUN_08012ae4(*param_1);
    *(undefined *)(param_1 + 0x12f) = 0;
    return 0;
  }
  return 2;
}


