/* 0800aa54 FUN_0800aa54; analyst naming is provisional. */

void FUN_0800aa54(void)

{
  int iVar1;
  
  iVar1 = DAT_0800aa6c;
  DataMemoryBarrier(0x1f);
  *(uint *)(DAT_0800aa6c + 0x24) = *(uint *)(DAT_0800aa6c + 0x24) & 0xfffeffff;
  *(undefined4 *)(iVar1 + 0x94) = 0;
  return;
}


