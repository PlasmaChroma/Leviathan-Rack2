0800572c: 2de9f04f   push.w     {r4, r5, r6, r7, r8, sb, sl, fp, lr}
08005730: adf2e44d   subw       sp, sp, #0x4e4
08005734: 5a4e       ldr        r6, [pc, #0x168] ; literal @080058a0 = 0x08019c20 / float 3.9003e-34
08005736: 0446       mov        r4, r0
08005738: 00f5b877   add.w      r7, r0, #0x170
0800573c: 1aab       add        r3, sp, #0x68
0800573e: 0df1800b   add.w      fp, sp, #0x80
08005742: 0df1280c   add.w      ip, sp, #0x28
08005746: 0df15009   add.w      sb, sp, #0x50
0800574a: 0293       str        r3, [sp, #8]
0800574c: 00f5e873   add.w      r3, r0, #0x1d0
08005750: 5d46       mov        r5, fp
08005752: e046       mov        r8, ip
08005754: 0393       str        r3, [sp, #0xc]
08005756: 0fce       ldm        r6!, {r0, r1, r2, r3}
08005758: 0fc5       stm        r5!, {r0, r1, r2, r3}
0800575a: 06f10c0e   add.w      lr, r6, #0xc
0800575e: 2b46       mov        r3, r5
08005760: 06f1200a   add.w      sl, r6, #0x20
08005764: 06f13805   add.w      r5, r6, #0x38
08005768: 96e80700   ldm.w      r6, {r0, r1, r2}
0800576c: 83e80700   stm.w      r3, {r0, r1, r2}
08005770: bee80f00   ldm.w      lr!, {r0, r1, r2, r3}
08005774: ace80f00   stm.w      ip!, {r0, r1, r2, r3}
08005778: def80030   ldr.w      r3, [lr]
0800577c: ccf80030   str.w      r3, [ip]
08005780: bae80f00   ldm.w      sl!, {r0, r1, r2, r3}
08005784: a9e80f00   stm.w      sb!, {r0, r1, r2, r3}
08005788: 9ae80300   ldm.w      sl, {r0, r1}
0800578c: 029e       ldr        r6, [sp, #8]
0800578e: 89e80300   stm.w      sb, {r0, r1}
08005792: 0fcd       ldm        r5!, {r0, r1, r2, r3}
08005794: 0fc6       stm        r6!, {r0, r1, r2, r3}
08005796: 95e80300   ldm.w      r5, {r0, r1}
0800579a: 3d46       mov        r5, r7
0800579c: c6e90001   strd       r0, r1, [r6]
080057a0: 2046       mov        r0, r4
080057a2: 00f021fa   bl         #0x8005be8
080057a6: 0021       movs       r1, #0
080057a8: 2046       mov        r0, r4
080057aa: 00f029fb   bl         #0x8005e00
080057ae: 6021       movs       r1, #0x60
080057b0: 2046       mov        r0, r4
080057b2: c4f84c16   str.w      r1, [r4, #0x64c]
080057b6: 00f037fa   bl         #0x8005c28
080057ba: 039e       ldr        r6, [sp, #0xc]
080057bc: 58f8040b   ldr        r0, [r8], #4
080057c0: c0b2       uxtb       r0, r0
080057c2: 00f013fa   bl         #0x8005bec
080057c6: 0346       mov        r3, r0
080057c8: 2846       mov        r0, r5
080057ca: 1835       adds       r5, #0x18
080057cc: adf89c30   strh.w     r3, [sp, #0x9c]
080057d0: 0122       movs       r2, #1
080057d2: 2799       ldr        r1, [sp, #0x9c]
080057d4: 01f074fc   bl         #0x80070c0
080057d8: b542       cmp        r5, r6
080057da: efd1       bne        #0x80057bc
080057dc: 0d20       movs       r0, #0xd
080057de: 1fae       add        r6, sp, #0x7c
080057e0: 00f004fa   bl         #0x8005bec
080057e4: 0023       movs       r3, #0
080057e6: adf81400   strh.w     r0, [sp, #0x14]
080057ea: 04f5c460   add.w      r0, r4, #0x620
080057ee: 0599       ldr        r1, [sp, #0x14]
080057f0: 1a46       mov        r2, r3
080057f2: 0093       str        r3, [sp]
080057f4: 04f17405   add.w      r5, r4, #0x74
080057f8: 02f09ef8   bl         #0x8007938
080057fc: 0d20       movs       r0, #0xd
080057fe: 00f0f5f9   bl         #0x8005bec
08005802: adf81800   strh.w     r0, [sp, #0x18]
08005806: 0699       ldr        r1, [sp, #0x18]
08005808: 0122       movs       r2, #1
0800580a: 04f23460   addw       r0, r4, #0x634
0800580e: 01f057fc   bl         #0x80070c0
08005812: 56f8040f   ldr        r0, [r6, #4]!
08005816: c0b2       uxtb       r0, r0
08005818: 00f0e8f9   bl         #0x8005bec
0800581c: d4f84c36   ldr.w      r3, [r4, #0x64c]
08005820: 9fed200a   vldr       s0, [pc, #0x80] ; literal @080058a4 = 0x47bb8e00 / float 96028
08005824: 07ee903a   vmov       s15, r3
08005828: adf89c00   strh.w     r0, [sp, #0x9c]
0800582c: 2846       mov        r0, r5
0800582e: 2435       adds       r5, #0x24
08005830: f8ee677a   vcvt.f32.u32 s15, s15
08005834: 2799       ldr        r1, [sp, #0x9c]
08005836: 80ee270a   vdiv.f32   s0, s0, s15
0800583a: 01f067fc   bl         #0x800710c
0800583e: bd42       cmp        r5, r7
08005840: e7d1       bne        #0x8005812
08005842: 6ff0f413   mvn        r3, #0xf400f4
08005846: 0b20       movs       r0, #0xb
08005848: 0025       movs       r5, #0
0800584a: 1093       str        r3, [sp, #0x40]
0800584c: 1023       movs       r3, #0x10
0800584e: 8df84c30   strb.w     r3, [sp, #0x4c]
08005852: 00f0cbf9   bl         #0x8005bec
08005856: adf84000   strh.w     r0, [sp, #0x40]
0800585a: 0c20       movs       r0, #0xc
0800585c: 00f0c6f9   bl         #0x8005bec
08005860: 0223       movs       r3, #2
08005862: 0fa9       add        r1, sp, #0x3c
08005864: adf84200   strh.w     r0, [sp, #0x42]
08005868: 09a8       add        r0, sp, #0x24
0800586a: 1295       str        r5, [sp, #0x48]
0800586c: 0f95       str        r5, [sp, #0x3c]
0800586e: 0995       str        r5, [sp, #0x24]
08005870: 1193       str        r3, [sp, #0x44]
08005872: 02f0dffb   bl         #0x8008034
08005876: 0c48       ldr        r0, [pc, #0x30] ; literal @080058a8 = 0x30000000 / float 4.65661e-10
08005878: 2946       mov        r1, r5
0800587a: 099b       ldr        r3, [sp, #0x24]
0800587c: 04f55475   add.w      r5, r4, #0x350
08005880: 0646       mov        r6, r0
08005882: c4f85403   str.w      r0, [r4, #0x354]
08005886: c4f85033   str.w      r3, [r4, #0x350]
0800588a: 00f18403   add.w      r3, r0, #0x84
0800588e: c4f85833   str.w      r3, [r4, #0x358]
08005892: 064b       ldr        r3, [pc, #0x18] ; literal @080058ac = 0x000b0100 / float 1.01055e-39
08005894: c4f85c33   str.w      r3, [r4, #0x35c]
08005898: ff23       movs       r3, #0xff
0800589a: 84f87433   strb.w     r3, [r4, #0x374]
0800589e: 09e0       b          #0x80058b4
080058a0: 209c       ldr        r4, [sp, #0x80]
080058a2: 0108       lsrs       r1, r0, #0x20
080058a4: 008e       ldrh       r0, [r0, #0x30]
080058a6: bb47       .byte      0xbb, 0x47
080058a8: 0000       movs       r0, r0
080058aa: 0030       adds       r0, #0
080058ac: 0001       lsls       r0, r0, #4
080058ae: 0b00       movs       r3, r1
080058b0: d4f85463   ldr.w      r6, [r4, #0x354]
080058b4: 0a11       asrs       r2, r1, #4
080058b6: 4ff0060c   mov.w      ip, #6
080058ba: 01f00f00   and        r0, r1, #0xf
080058be: 8b00       lsls       r3, r1, #2
080058c0: 02eb8212   add.w      r2, r2, r2, lsl #6
080058c4: 0131       adds       r1, #1
080058c6: 9bb2       uxth       r3, r3
080058c8: 06f802c0   strb.w     ip, [r6, r2]
080058cc: 02eb8000   add.w      r0, r2, r0, lsl #2
080058d0: 2029       cmp        r1, #0x20
080058d2: d4e9d567   ldrd       r6, r7, [r4, #0x354]
080058d6: 0644       add        r6, r0
080058d8: a6f80130   strh.w     r3, [r6, #1]
080058dc: a6f80330   strh.w     r3, [r6, #3]
080058e0: 07f802c0   strb.w     ip, [r7, r2]
080058e4: d4f85823   ldr.w      r2, [r4, #0x358]
080058e8: 0244       add        r2, r0
080058ea: a2f80130   strh.w     r3, [r2, #1]
080058ee: a2f80330   strh.w     r3, [r2, #3]
080058f2: ddd1       bne        #0x80058b0
080058f4: 94f85e33   ldrb.w     r3, [r4, #0x35e]
080058f8: 0b2b       cmp        r3, #0xb
080058fa: 0dd0       beq        #0x8005918
080058fc: 0026       movs       r6, #0
080058fe: 04f55870   add.w      r0, r4, #0x360
08005902: 0122       movs       r2, #1
08005904: 0096       str        r6, [sp]
08005906: 3346       mov        r3, r6
08005908: e989       ldrh       r1, [r5, #0xe]
0800590a: 0290       str        r0, [sp, #8]
0800590c: 02f014f8   bl         #0x8007938
08005910: 3146       mov        r1, r6
08005912: 0298       ldr        r0, [sp, #8]
08005914: 02f028f8   bl         #0x8007968
08005918: 04f55778   add.w      r8, r4, #0x35c
0800591c: 04f25e39   addw       sb, r4, #0x35e
08005920: 18f8011b   ldrb       r1, [r8], #1
08005924: 0127       movs       r7, #1
08005926: 0026       movs       r6, #0
08005928: 0223       movs       r3, #2
0800592a: 41f04001   orr        r1, r1, #0x40
0800592e: 27aa       add        r2, sp, #0x9c
08005930: 2846       mov        r0, r5
08005932: 0097       str        r7, [sp]
08005934: 0291       str        r1, [sp, #8]
08005936: 8df89c60   strb.w     r6, [sp, #0x9c]
0800593a: 8df89d60   strb.w     r6, [sp, #0x9d]
0800593e: 02f087fb   bl         #0x8008050
08005942: 1420       movs       r0, #0x14
08005944: 03f0eafd   bl         #0x800951c
08005948: 0223       movs       r3, #2
0800594a: 27aa       add        r2, sp, #0x9c
0800594c: 0299       ldr        r1, [sp, #8]
0800594e: 2846       mov        r0, r5
08005950: 0097       str        r7, [sp]
08005952: 8df89c60   strb.w     r6, [sp, #0x9c]
08005956: 8df89d60   strb.w     r6, [sp, #0x9d]
0800595a: 02f079fb   bl         #0x8008050
0800595e: 1420       movs       r0, #0x14
08005960: 03f0dcfd   bl         #0x800951c
08005964: 2023       movs       r3, #0x20
08005966: 27aa       add        r2, sp, #0x9c
08005968: 0299       ldr        r1, [sp, #8]
0800596a: 8df89d30   strb.w     r3, [sp, #0x9d]
0800596e: 2846       mov        r0, r5
08005970: 0223       movs       r3, #2
08005972: 0097       str        r7, [sp]
08005974: 8df89c60   strb.w     r6, [sp, #0x9c]
08005978: 02f06afb   bl         #0x8008050
0800597c: 1420       movs       r0, #0x14
0800597e: 0526       movs       r6, #5
08005980: 03f0ccfd   bl         #0x800951c
08005984: 3623       movs       r3, #0x36
08005986: 0299       ldr        r1, [sp, #8]
08005988: 27aa       add        r2, sp, #0x9c
0800598a: 8df89d30   strb.w     r3, [sp, #0x9d]
0800598e: 2846       mov        r0, r5
08005990: 0223       movs       r3, #2
08005992: 0096       str        r6, [sp]
08005994: 8df89c70   strb.w     r7, [sp, #0x9c]
08005998: 02f05afb   bl         #0x8008050
0800599c: c145       cmp        sb, r8
0800599e: bfd1       bne        #0x8005920
080059a0: 4ff48078   mov.w      r8, #0x100
080059a4: 4ff00009   mov.w      sb, #0
080059a8: dff820a2   ldr.w      sl, [pc, #0x220] ; literal @08005bcc = 0x3f333333 / float 0.7
080059ac: 04f5af6e   add.w      lr, r4, #0x578
080059b0: a4f87c85   strh.w     r8, [r4, #0x57c]
080059b4: 4ff00208   mov.w      r8, #2
080059b8: c4f87855   str.w      r5, [r4, #0x578]
080059bc: 04f5b26c   add.w      ip, r4, #0x590
080059c0: 84f87e85   strb.w     r8, [r4, #0x57e]
080059c4: 04f5b567   add.w      r7, r4, #0x5a8
080059c8: cef81490   str.w      sb, [lr, #0x14]
080059cc: 04f5b860   add.w      r0, r4, #0x5c0
080059d0: cef81090   str.w      sb, [lr, #0x10]
080059d4: 04f5bb61   add.w      r1, r4, #0x5d8
080059d8: cef80c90   str.w      sb, [lr, #0xc]
080059dc: 04f5be62   add.w      r2, r4, #0x5f0
080059e0: cef808a0   str.w      sl, [lr, #8]
080059e4: 04f5c163   add.w      r3, r4, #0x608
080059e8: 84f89665   strb.w     r6, [r4, #0x596]
080059ec: 40f20346   movw       r6, #0x403
080059f0: c4f89055   str.w      r5, [r4, #0x590]
080059f4: a4f89465   strh.w     r6, [r4, #0x594]
080059f8: 40f20676   movw       r6, #0x706
080059fc: ccf81490   str.w      sb, [ip, #0x14]
08005a00: ccf81090   str.w      sb, [ip, #0x10]
08005a04: ccf80c90   str.w      sb, [ip, #0xc]
08005a08: ccf808a0   str.w      sl, [ip, #8]
08005a0c: a4f8ac65   strh.w     r6, [r4, #0x5ac]
08005a10: 0826       movs       r6, #8
08005a12: c4f8a855   str.w      r5, [r4, #0x5a8]
08005a16: 84f8ae65   strb.w     r6, [r4, #0x5ae]
08005a1a: 40f60926   movw       r6, #0xa09
08005a1e: c7f81490   str.w      sb, [r7, #0x14]
08005a22: c7f81090   str.w      sb, [r7, #0x10]
08005a26: c7f80c90   str.w      sb, [r7, #0xc]
08005a2a: c7f808a0   str.w      sl, [r7, #8]
08005a2e: 0df5a667   add.w      r7, sp, #0x530
08005a32: a4f8c465   strh.w     r6, [r4, #0x5c4]
08005a36: 0b26       movs       r6, #0xb
08005a38: c4f8c055   str.w      r5, [r4, #0x5c0]
08005a3c: 84f8c665   strb.w     r6, [r4, #0x5c6]
08005a40: 27ae       add        r6, sp, #0x9c
08005a42: c0f81490   str.w      sb, [r0, #0x14]
08005a46: c0f81090   str.w      sb, [r0, #0x10]
08005a4a: c0f80c90   str.w      sb, [r0, #0xc]
08005a4e: c0f808a0   str.w      sl, [r0, #8]
08005a52: 40f60c50   movw       r0, #0xd0c
08005a56: c4f8d855   str.w      r5, [r4, #0x5d8]
08005a5a: a4f8dc05   strh.w     r0, [r4, #0x5dc]
08005a5e: 0e20       movs       r0, #0xe
08005a60: 84f8de05   strb.w     r0, [r4, #0x5de]
08005a64: c1f81490   str.w      sb, [r1, #0x14]
08005a68: c1f81090   str.w      sb, [r1, #0x10]
08005a6c: c1f80c90   str.w      sb, [r1, #0xc]
08005a70: c1f808a0   str.w      sl, [r1, #8]
08005a74: 41f21011   movw       r1, #0x1110
08005a78: c4f8f055   str.w      r5, [r4, #0x5f0]
08005a7c: a4f8f415   strh.w     r1, [r4, #0x5f4]
08005a80: 1221       movs       r1, #0x12
08005a82: 84f8f615   strb.w     r1, [r4, #0x5f6]
08005a86: c2f81490   str.w      sb, [r2, #0x14]
08005a8a: c2f81090   str.w      sb, [r2, #0x10]
08005a8e: c2f80c90   str.w      sb, [r2, #0xc]
08005a92: c2f808a0   str.w      sl, [r2, #8]
08005a96: 41f21342   movw       r2, #0x1413
08005a9a: c4f80856   str.w      r5, [r4, #0x608]
08005a9e: a4f80c26   strh.w     r2, [r4, #0x60c]
08005aa2: 1522       movs       r2, #0x15
08005aa4: 84f80e26   strb.w     r2, [r4, #0x60e]
08005aa8: 3baa       add        r2, sp, #0xec
08005aaa: c3f808a0   str.w      sl, [r3, #8]
08005aae: c3f81490   str.w      sb, [r3, #0x14]
08005ab2: c3f81090   str.w      sb, [r3, #0x10]
08005ab6: c3f80c90   str.w      sb, [r3, #0xc]
08005aba: ff23       movs       r3, #0xff
08005abc: 0021       movs       r1, #0
08005abe: 0b20       movs       r0, #0xb
08005ac0: 02f84f3c   strb       r3, [r2, #-0x4f]
08005ac4: a2f13c03   sub.w      r3, r2, #0x3c
08005ac8: 02f8500c   strb       r0, [r2, #-0x50]
08005acc: 42f84c1c   str        r1, [r2, #-0x4c]
08005ad0: 42e91211   strd       r1, r1, [r2, #-0x48]
08005ad4: 0021       movs       r1, #0
08005ad6: 0b20       movs       r0, #0xb
08005ad8: 5960       str        r1, [r3, #4]
08005ada: 1870       strb       r0, [r3]
08005adc: ff20       movs       r0, #0xff
08005ade: c3e90211   strd       r1, r1, [r3, #8]
08005ae2: 1433       adds       r3, #0x14
08005ae4: 03f8130c   strb       r0, [r3, #-0x13]
08005ae8: 9a42       cmp        r2, r3
08005aea: f3d1       bne        #0x8005ad4
08005aec: 5432       adds       r2, #0x54
08005aee: 9742       cmp        r7, r2
08005af0: e3d1       bne        #0x8005aba
08005af2: 14af       add        r7, sp, #0x50
08005af4: 57f8040b   ldr        r0, [r7], #4
08005af8: c0b2       uxtb       r0, r0
08005afa: 00f077f8   bl         #0x8005bec
08005afe: adf81c00   strh.w     r0, [sp, #0x1c]
08005b02: 0222       movs       r2, #2
08005b04: 3046       mov        r0, r6
08005b06: 0799       ldr        r1, [sp, #0x1c]
08005b08: 5436       adds       r6, #0x54
08005b0a: 01f073fb   bl         #0x80071f4
08005b0e: 1aab       add        r3, sp, #0x68
08005b10: 9f42       cmp        r7, r3
08005b12: efd1       bne        #0x8005af4
08005b14: a5ae       add        r6, sp, #0x294
08005b16: 57f8040b   ldr        r0, [r7], #4
08005b1a: c0b2       uxtb       r0, r0
08005b1c: 00f066f8   bl         #0x8005bec
08005b20: 0346       mov        r3, r0
08005b22: 0222       movs       r2, #2
08005b24: 3046       mov        r0, r6
08005b26: adf82030   strh.w     r3, [sp, #0x20]
08005b2a: 5436       adds       r6, #0x54
08005b2c: 0899       ldr        r1, [sp, #0x20]
08005b2e: 01f061fb   bl         #0x80071f4
08005b32: bb45       cmp        fp, r7
08005b34: efd1       bne        #0x8005b16
08005b36: 04f11808   add.w      r8, r4, #0x18
08005b3a: 27a9       add        r1, sp, #0x9c
08005b3c: 0423       movs       r3, #4
08005b3e: 0c22       movs       r2, #0xc
08005b40: 4046       mov        r0, r8
08005b42: 039f       ldr        r7, [sp, #0xc]
08005b44: 0026       movs       r6, #0
08005b46: 01f065fb   bl         #0x8007214
08005b4a: f1b2       uxtb       r1, r6
08005b4c: 4046       mov        r0, r8
08005b4e: 01f03dfd   bl         #0x80075cc
08005b52: d4f84c26   ldr.w      r2, [r4, #0x64c]
08005b56: 0023       movs       r3, #0
08005b58: 9fed190a   vldr       s0, [pc, #0x64] ; literal @08005bc0 = 0x47bb8e00 / float 96028
08005b5c: 07ee902a   vmov       s15, r2
08005b60: 0136       adds       r6, #1
08005b62: 0146       mov        r1, r0
08005b64: dfed170a   vldr       s1, [pc, #0x5c] ; literal @08005bc4 = 0x3b03126f / float 0.002
08005b68: f8ee677a   vcvt.f32.u32 s15, s15
08005b6c: 3846       mov        r0, r7
08005b6e: 1a46       mov        r2, r3
08005b70: 2037       adds       r7, #0x20
08005b72: 80ee270a   vdiv.f32   s0, s0, s15
08005b76: 01f017fa   bl         #0x8006fa8
08005b7a: 062e       cmp        r6, #6
08005b7c: e5d1       bne        #0x8005b4a
08005b7e: 04f52477   add.w      r7, r4, #0x290
08005b82: 3146       mov        r1, r6
08005b84: 4046       mov        r0, r8
08005b86: 01f021fd   bl         #0x80075cc
08005b8a: d4f84c36   ldr.w      r3, [r4, #0x64c]
08005b8e: 9fed0c0a   vldr       s0, [pc, #0x30] ; literal @08005bc0 = 0x47bb8e00 / float 96028
08005b92: 0146       mov        r1, r0
08005b94: 07ee903a   vmov       s15, r3
08005b98: 3846       mov        r0, r7
08005b9a: 2037       adds       r7, #0x20
08005b9c: 0136       adds       r6, #1
08005b9e: f8ee677a   vcvt.f32.u32 s15, s15
08005ba2: f6b2       uxtb       r6, r6
08005ba4: 80ee270a   vdiv.f32   s0, s0, s15
08005ba8: 01f030fa   bl         #0x800700c
08005bac: bd42       cmp        r5, r7
08005bae: e8d1       bne        #0x8005b82
08005bb0: 054b       ldr        r3, [pc, #0x14] ; literal @08005bc8 = 0x3dcccccd / float 0.1
08005bb2: c4f89432   str.w      r3, [r4, #0x294]
08005bb6: 0df2e44d   addw       sp, sp, #0x4e4
08005bba: bde8f08f   pop.w      {r4, r5, r6, r7, r8, sb, sl, fp, pc}
08005bbe: 00bf       nop        
08005bc0: 008e       ldrh       r0, [r0, #0x30]
08005bc2: bb47       .byte      0xbb, 0x47
08005bc4: 6f12       asrs       r7, r5, #9
08005bc6: 033b       subs       r3, #3
08005bc8: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08005bca: cc3d       subs       r5, #0xcc
08005bcc: 3333       adds       r3, #0x33
08005bce: 333f       subs       r7, #0x33
08005bd0: 044b       ldr        r3, [pc, #0x10] ; literal @08005be4 = 0x30000000 / float 4.65661e-10
08005bd2: 0622       movs       r2, #6
08005bd4: 1a70       strb       r2, [r3]
08005bd6: 83f84120   strb.w     r2, [r3, #0x41]
08005bda: 83f88420   strb.w     r2, [r3, #0x84]
08005bde: 83f8c520   strb.w     r2, [r3, #0xc5]
08005be2: 7047       bx         lr
08005be4: 0000       movs       r0, r0
08005be6: 0030       adds       r0, #0
08005be8: 7047       bx         lr
08005bea: 00bf       nop        
08005bec: 2028       cmp        r0, #0x20
08005bee: 82b0       sub        sp, #8
08005bf0: 09d8       bhi        #0x8005c06
08005bf2: 084b       ldr        r3, [pc, #0x20] ; literal @08005c14 = 0x08019e08 / float 3.90053e-34
08005bf4: 03eb4002   add.w      r2, r3, r0, lsl #1
08005bf8: 13f81030   ldrb.w     r3, [r3, r0, lsl #1]
08005bfc: 5078       ldrb       r0, [r2, #1]
08005bfe: 43ea0020   orr.w      r0, r3, r0, lsl #8
08005c02: 02b0       add        sp, #8
08005c04: 7047       bx         lr
08005c06: 0123       movs       r3, #1
08005c08: 0c20       movs       r0, #0xc
08005c0a: 43ea0020   orr.w      r0, r3, r0, lsl #8
08005c0e: 02b0       add        sp, #8
08005c10: 7047       bx         lr
08005c12: 00bf       nop        
08005c14: 089e       ldr        r6, [sp, #0x20]
08005c16: 0108       lsrs       r1, r0, #0x20
08005c18: 1430       adds       r0, #0x14
08005c1a: 01f09db9   b.w        #0x8006f58
08005c1e: 00bf       nop        
08005c20: 1430       adds       r0, #0x14
08005c22: 01f095b9   b.w        #0x8006f50
08005c26: 00bf       nop        
08005c28: 38b5       push       {r3, r4, r5, lr}
08005c2a: 00f11405   add.w      r5, r0, #0x14
08005c2e: 0446       mov        r4, r0
08005c30: 2ded028b   vpush      {d8}
08005c34: 2846       mov        r0, r5
08005c36: 01f07df9   bl         #0x8006f34
08005c3a: 2846       mov        r0, r5
08005c3c: 01f088f9   bl         #0x8006f50
08005c40: 2846       mov        r0, r5
08005c42: b0ee408a   vmov.f32   s16, s0
08005c46: 01f071f9   bl         #0x8006f2c
08005c4a: d0ed007a   vldr       s15, [r0]
08005c4e: f8ee677a   vcvt.f32.u32 s15, s15
08005c52: 88ee277a   vdiv.f32   s14, s16, s15
08005c56: bdec028b   vpop       {d8}
08005c5a: 84ed1b7a   vstr       s14, [r4, #0x6c]
08005c5e: 38bd       pop        {r3, r4, r5, pc}
08005c60: 08b5       push       {r3, lr}
08005c62: 1430       adds       r0, #0x14
08005c64: 01f062f9   bl         #0x8006f2c
08005c68: 0068       ldr        r0, [r0]
08005c6a: 08bd       pop        {r3, pc}
08005c6c: 10b5       push       {r4, lr}
08005c6e: 0323       movs       r3, #3
