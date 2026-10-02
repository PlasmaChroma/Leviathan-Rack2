/* 08011230 FUN_08011230; analyst naming is provisional. */

uint FUN_08011230(void)

{
  uint uVar1;
  
  uVar1 = FUN_08010388();
  return uVar1 >> (*(byte *)(DAT_08011250 + ((uint)(*(int *)(DAT_0801124c + 0x20) << 0x19) >> 0x1d))
                  & 0x1f);
}


