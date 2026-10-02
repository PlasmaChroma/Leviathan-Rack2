/* 080099cc FUN_080099cc; analyst naming is provisional. */

undefined4 FUN_080099cc(int param_1)

{
  int iVar1;
  code *pcVar2;
  
  if (*(char *)(*(int *)(param_1 + 0x508) + 0x29c) != '\x03') {
    return 0;
  }
  iVar1 = *(int *)(*(int *)(param_1 + 0x508) + 0x2b8);
  if ((iVar1 != 0) && (pcVar2 = *(code **)(iVar1 + 0x1c), pcVar2 != (code *)0x0)) {
    (*pcVar2)();
  }
  return 0;
}


