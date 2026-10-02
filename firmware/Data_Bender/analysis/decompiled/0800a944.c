/* 0800a944 FUN_0800a944; analyst naming is provisional. */

void FUN_0800a944(uint param_1)

{
  *(uint *)(DAT_0800a960 + 0xc) =
       DAT_0800a964 | (param_1 & 7) << 8 | *(uint *)(DAT_0800a960 + 0xc) & 0xf8ff;
  return;
}


