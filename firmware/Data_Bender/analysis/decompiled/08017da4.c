/* 08017da4 libm_expf; analyst naming is provisional. */

/* WARNING: Removing unreachable block (ram,0x08017d1e) */
/* Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP
   conditional expressions against ARM instructions and emulator evidence. */

float libm_expf(float param_1)

{
  undefined4 *puVar1;
  uint uVar2;
  float fVar3;
  uint uVar4;
  double dVar5;
  
  uVar2 = (uint)((int)param_1 << 1) >> 0x15;
  if (0x42a < uVar2) {
    if (param_1 == -INFINITY) {
      return DAT_08017e84;
    }
    if (0x7f7 < uVar2) {
      return param_1 + param_1;
    }
    fVar3 = DAT_08017d5c;
    if (((param_1 != DAT_08017e78 && param_1 < DAT_08017e78 == (NAN(param_1) || NAN(DAT_08017e78)))
        || (fVar3 = DAT_08017d44, (int)((uint)(param_1 < DAT_08017e7c) << 0x1f) < 0)) ||
       (fVar3 = DAT_08017d50, (int)((uint)(param_1 < DAT_08017e80) << 0x1f) < 0)) {
      puVar1 = (undefined4 *)FUN_080188d8();
      *puVar1 = 0x22;
      return fVar3 * fVar3;
    }
  }
  dVar5 = *(double *)(DAT_08017e74 + 0x120) + *(double *)(DAT_08017e74 + 0x128) * (double)param_1;
  uVar4 = SUB84(dVar5,0);
  uVar2 = uVar4 & 0x1f;
  dVar5 = -(dVar5 - *(double *)(DAT_08017e74 + 0x120)) +
          *(double *)(DAT_08017e74 + 0x128) * (double)param_1;
  return (float)((*(double *)(DAT_08017e74 + 0x140) * dVar5 + 1.0 +
                 (*(double *)(DAT_08017e74 + 0x138) + *(double *)(DAT_08017e74 + 0x130) * dVar5) *
                 dVar5 * dVar5) *
                (double)CONCAT44(*(int *)(DAT_08017e74 + uVar2 * 8 + 4) + uVar4 * 0x8000,
                                 *(undefined4 *)(DAT_08017e74 + uVar2 * 8)));
}


