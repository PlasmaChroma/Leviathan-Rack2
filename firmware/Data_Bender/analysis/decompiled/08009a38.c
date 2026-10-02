/* 08009a38 FUN_08009a38; analyst naming is provisional. */

undefined4 FUN_08009a38(int param_1)

{
  int iVar1;
  int iVar2;
  code *pcVar3;
  
  iVar1 = *(int *)(param_1 + 0x508);
  iVar2 = *(int *)(iVar1 + (*(int *)(iVar1 + 0x2d4) + 0xae) * 4);
  if (iVar2 == 0) {
    return 3;
  }
  if (*(char *)(iVar1 + 0x29c) != '\x03') {
    return 0;
  }
  pcVar3 = *(code **)(iVar2 + 0x20);
  if (pcVar3 != (code *)0x0) {
    (*pcVar3)();
    return 0;
  }
  return 0;
}


