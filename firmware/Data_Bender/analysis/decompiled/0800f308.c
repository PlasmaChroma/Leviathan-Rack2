/* 0800f308 FUN_0800f308; analyst naming is provisional. */

undefined4 FUN_0800f308(uint **param_1)

{
  if (param_1 != (uint **)0x0) {
    **param_1 = **param_1 & 0xfffffffe;
    FUN_08008c6c();
    param_1[0x11] = (uint *)0x0;
    *(undefined *)((int)param_1 + 0x41) = 0;
    return 0;
  }
  return 1;
}


