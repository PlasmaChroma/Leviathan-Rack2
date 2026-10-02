/* 0801892c libc_memcpy; analyst naming is provisional. */

/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

void libc_memcpy(int param_1,undefined *param_2,int param_3)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  
  puVar2 = param_2 + param_3;
  puVar3 = (undefined *)(param_1 + -1);
  if (param_2 != puVar2) {
    do {
      puVar1 = param_2 + 1;
      puVar3 = puVar3 + 1;
      *puVar3 = *param_2;
      param_2 = puVar1;
    } while (puVar1 != puVar2);
    return;
  }
  return;
}


