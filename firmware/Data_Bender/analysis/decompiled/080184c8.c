/* 080184c8 FUN_080184c8; analyst naming is provisional. */

float FUN_080184c8(float param_1)

{
  float fVar1;
  undefined4 *puVar2;
  float fVar3;
  
  fVar3 = (float)FUN_08018638();
  fVar1 = DAT_08018514;
  if (((*DAT_08018510 != -1) && (!NAN(param_1))) &&
     ((int)((uint)(param_1 < DAT_08018514) << 0x1f) < 0)) {
    puVar2 = (undefined4 *)FUN_080188d8();
    *puVar2 = 0x21;
    return fVar1 / fVar1;
  }
  return fVar3;
}


