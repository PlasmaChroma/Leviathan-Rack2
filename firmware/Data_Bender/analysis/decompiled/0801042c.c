/* 0801042c FUN_0801042c; analyst naming is provisional. */

uint FUN_0801042c(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_0801044c + ((uint)(*(int *)(DAT_08010448 + 0x1c) << 0x15) >> 0x1d))
                  & 0x1f);
}


