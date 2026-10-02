/* 08012bd4 FUN_08012bd4; analyst naming is provisional. */

undefined4 FUN_08012bd4(int param_1)

{
  *(uint *)(param_1 + 0x900) = DAT_08012bfc & *(uint *)(param_1 + 0x900);
  *(uint *)(param_1 + 0x804) = *(uint *)(param_1 + 0x804) | 0x100;
  return 0;
}


