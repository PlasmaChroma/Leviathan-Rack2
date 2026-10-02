/* 08012a08 FUN_08012a08; analyst naming is provisional. */

undefined4 FUN_08012a08(int param_1,undefined4 *param_2,int param_3,int param_4,char param_5)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  if ((param_5 == '\0') && (param_4 + 3U >> 2 != 0)) {
    puVar1 = param_2;
    do {
      puVar2 = puVar1 + 1;
      *(undefined4 *)(param_1 + (param_3 + 1) * 0x1000) = *puVar1;
      puVar1 = puVar2;
    } while ((undefined4 *)((int)param_2 + (param_4 + 3U & 0xfffffffc)) != puVar2);
  }
  return 0;
}


