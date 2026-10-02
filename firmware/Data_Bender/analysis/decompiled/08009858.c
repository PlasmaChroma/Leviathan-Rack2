/* 08009858 thunk_FUN_08010408; analyst naming is provisional. */

uint thunk_FUN_08010408(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_08010428 + ((uint)(*(int *)(DAT_08010424 + 0x1c) << 0x19) >> 0x1d))
                  & 0x1f);
}


