/* 08016bcc FUN_08016bcc; analyst naming is provisional. */

uint FUN_08016bcc(int param_1)

{
  return *(uint *)(param_1 + 0x8c) | *(uint *)(param_1 + 0x88);
}


