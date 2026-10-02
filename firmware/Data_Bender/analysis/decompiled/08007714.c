/* 08007714 FUN_08007714; analyst naming is provisional. */

void FUN_08007714(int *param_1)

{
  if (*param_1 != DAT_08007724) {
    return;
  }
  software_bkpt(0xff);
  return;
}


