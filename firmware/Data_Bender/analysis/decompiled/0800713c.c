/* 0800713c Switch_Debounce; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void Switch_Debounce(int *param_1)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  
  iVar2 = System_GetNow();
  *(undefined *)(param_1 + 1) = 0;
  if (*param_1 != iVar2) {
    *param_1 = iVar2;
    *(undefined *)(param_1 + 1) = 1;
    bVar1 = GPIO_Read(param_1 + 2);
    if (*(char *)((int)param_1 + 0x1d) != '\0') {
      bVar1 = bVar1 ^ 1;
    }
    bVar1 = *(char *)(param_1 + 7) << 1 | bVar1;
    *(byte *)(param_1 + 7) = bVar1;
    if (bVar1 == 0x7f) {
      uVar3 = System_GetNow();
      param_1[8] = (int)(float)(ulonglong)uVar3;
      return;
    }
  }
  return;
}


