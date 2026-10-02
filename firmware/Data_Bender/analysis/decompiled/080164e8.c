/* 080164e8 FUN_080164e8; analyst naming is provisional. */

undefined4 FUN_080164e8(undefined4 *param_1)

{
  if (param_1 != (undefined4 *)0x0) {
    if (*(char *)((int)param_1 + 0x3d) == '\0') {
      *(undefined *)(param_1 + 0xf) = 0;
      FUN_080143d4();
    }
    *(undefined *)((int)param_1 + 0x3d) = 2;
    FUN_080163d0(*param_1,param_1 + 1);
    *(undefined *)(param_1 + 0x12) = 1;
    *(undefined *)((int)param_1 + 0x3e) = 1;
    *(undefined *)((int)param_1 + 0x3f) = 1;
    *(undefined *)(param_1 + 0x10) = 1;
    *(undefined *)((int)param_1 + 0x41) = 1;
    *(undefined *)((int)param_1 + 0x42) = 1;
    *(undefined *)((int)param_1 + 0x43) = 1;
    *(undefined *)(param_1 + 0x11) = 1;
    *(undefined *)((int)param_1 + 0x45) = 1;
    *(undefined *)((int)param_1 + 0x46) = 1;
    *(undefined *)((int)param_1 + 0x47) = 1;
    *(undefined *)((int)param_1 + 0x3d) = 1;
    return 0;
  }
  return 1;
}


