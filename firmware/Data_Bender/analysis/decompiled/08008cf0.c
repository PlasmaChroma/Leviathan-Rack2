/* 08008cf0 FUN_08008cf0; analyst naming is provisional. */

void FUN_08008cf0(void)

{
  uint **ppuVar1;
  int iVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  uint *puVar6;
  
  ppuVar1 = DAT_08008cf8;
  puVar6 = *DAT_08008cf8;
  uVar5 = puVar6[2];
  uVar4 = *puVar6;
  if ((-1 < (int)(uVar5 << 0x1d)) || (-1 < (int)(uVar4 << 0xd))) {
    if (((int)(uVar5 << 0x1e) < 0) && ((int)(uVar4 << 0xe) < 0)) {
      puVar6[3] = 2;
      *puVar6 = *puVar6 & 0xfff8ffff;
      if (*(char *)((int)ppuVar1 + 0x41) == '\x12') {
        if ((int)(*puVar6 << 0x1d) < 0) {
          uVar4 = *ppuVar1[0xf];
          *puVar6 = *puVar6 & 0xfffffffb;
          *(uint *)(uVar4 + 0xc) = *(uint *)(uVar4 + 0xc) & 0xfffffffe;
        }
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7b0(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) == '\"') {
        if ((int)(*puVar6 << 0x1d) < 0) {
          uVar4 = *ppuVar1[0xf];
          *puVar6 = *puVar6 & 0xfffffffb;
          *(uint *)(uVar4 + 0xc) = *(uint *)(uVar4 + 0xc) & 0xfffffffe;
        }
        else {
          uVar4 = puVar6[2];
          while (((uVar4 & 0x3f00) != 0 && (ppuVar1[0xe] != (uint *)0x0))) {
            *(undefined *)ppuVar1[0xc] = *(undefined *)(puVar6 + 8);
            ppuVar1[0xe] = (uint *)((int)ppuVar1[0xe] + -1);
            uVar4 = (*ppuVar1)[2];
            ppuVar1[0xc] = (uint *)((int)ppuVar1[0xc] + 1);
          }
        }
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7ac(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) == '\x02') {
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f7a8(ppuVar1);
        return;
      }
      if (*(char *)((int)ppuVar1 + 0x41) != '\b') {
        return;
      }
      puVar6[5] = puVar6[5] & 0xf3ffffff;
      *(undefined *)((int)ppuVar1 + 0x41) = 1;
      if (ppuVar1[0x11] == (uint *)0x0) {
        FUN_0800f7a4();
        return;
      }
    }
    else {
      if (((int)(uVar5 << 0x1c) < 0) && ((int)(uVar4 << 0xc) < 0)) {
        puVar6[3] = 8;
        if ((int)(*puVar6 << 9) < 0) {
          *puVar6 = *puVar6 & 0xfff6ffff;
          *(undefined *)((int)ppuVar1 + 0x41) = 1;
        }
        FUN_0800f7b8(ppuVar1);
        return;
      }
      if ((-1 < (int)(uVar5 << 0x1f)) || (-1 < (int)(uVar4 << 0xf))) {
        if (-1 < (int)(uVar5 << 0x1b)) {
          return;
        }
        if (-1 < (int)(uVar4 << 0xb)) {
          return;
        }
        puVar6[3] = 0x10;
        FUN_0800f7bc(ppuVar1);
        return;
      }
      puVar6[3] = 1;
      *puVar6 = *puVar6 & 0xfff0ffff;
      ppuVar1[0x11] = (uint *)((uint)ppuVar1[0x11] | 2);
      if (-1 < (int)(*puVar6 << 0x1d)) {
        *(undefined *)((int)ppuVar1 + 0x41) = 1;
        FUN_0800f768(ppuVar1);
        return;
      }
      puVar3 = ppuVar1[0xf];
      *puVar6 = *puVar6 & 0xfffffffb;
      puVar3[0x16] = DAT_0800f9cc;
      iVar2 = FUN_0801549c();
      if (iVar2 == 0) {
        return;
      }
      ppuVar1[0x11] = (uint *)((uint)ppuVar1[0x11] | 4);
      *(undefined *)((int)ppuVar1 + 0x41) = 1;
    }
    FUN_0800f768(ppuVar1);
    return;
  }
  puVar3 = puVar6;
  if (*(char *)((int)DAT_08008cf8 + 0x41) == '\x12') {
    uVar4 = puVar6[2];
    while ((int)(uVar4 << 0x1d) < 0) {
      if (ppuVar1[0xb] == (uint *)0x0) goto LAB_0800f8f4;
      *(undefined *)(puVar6 + 8) = *(undefined *)ppuVar1[9];
      puVar3 = *ppuVar1;
      ppuVar1[0xb] = (uint *)((int)ppuVar1[0xb] + -1);
      uVar4 = puVar3[2];
      ppuVar1[9] = (uint *)((int)ppuVar1[9] + 1);
    }
  }
  else if (*(char *)((int)DAT_08008cf8 + 0x41) == '\"') {
    uVar4 = puVar6[2];
    while ((int)(uVar4 << 0x1d) < 0) {
      if (ppuVar1[0xe] == (uint *)0x0) goto LAB_0800f8f4;
      *(undefined *)ppuVar1[0xc] = *(undefined *)(puVar6 + 8);
      puVar3 = *ppuVar1;
      ppuVar1[0xe] = (uint *)((int)ppuVar1[0xe] + -1);
      uVar4 = puVar3[2];
      ppuVar1[0xc] = (uint *)((int)ppuVar1[0xc] + 1);
    }
  }
LAB_0800f7e4:
  FUN_0800f7b4(ppuVar1);
  return;
LAB_0800f8f4:
  *puVar3 = *puVar3 & 0xfffbffff;
  goto LAB_0800f7e4;
}


