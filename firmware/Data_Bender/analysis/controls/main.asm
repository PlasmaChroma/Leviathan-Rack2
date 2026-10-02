08003878: b14d       ldr        r5, [pc, #0x2c4] ; literal @08003b40 = 0x24000468 / float 2.77593e-17
0800387a: b24e       ldr        r6, [pc, #0x2c8] ; literal @08003b44 = 0x24000d68 / float 2.77669e-17
0800387c: 2846       mov        r0, r5
0800387e: 9fedb28a   vldr       s16, [pc, #0x2c8] ; literal @08003b48 = 0x47bb8e00 / float 96028
08003882: b24c       ldr        r4, [pc, #0x2c8] ; literal @08003b4c = 0x47147000 / float 38000
08003884: 2de98048   push.w     {r7, fp, lr}
08003888: 91b0       sub        sp, #0x44
0800388a: 06f29467   addw       r7, r6, #0x694
0800388e: 01f04dff   bl         #0x800572c
08003892: af49       ldr        r1, [pc, #0x2bc] ; literal @08003b50 = 0xc0000000 / float -2
08003894: af4a       ldr        r2, [pc, #0x2bc] ; literal @08003b54 = 0x00dbca68 / float 2.01846e-38
08003896: b0ee480a   vmov.f32   s0, s16
0800389a: af4b       ldr        r3, [pc, #0x2bc] ; literal @08003b58 = 0xc1b794d0 / float -22.9477
0800389c: 06f19800   add.w      r0, r6, #0x98
080038a0: 86ed008a   vstr       s16, [r6]
080038a4: c6e92012   strd       r1, r2, [r6, #0x80]
080038a8: ac4a       ldr        r2, [pc, #0x2b0] ; literal @08003b5c = 0x006de534 / float 1.00923e-38
080038aa: c6e92213   strd       r1, r3, [r6, #0x88]
080038ae: 01f0ddf8   bl         #0x8004a6c
080038b2: 96ed000a   vldr       s0, [r6]
080038b6: 06f53570   add.w      r0, r6, #0x2d4
080038ba: fdf763fd   bl         #0x8001384
080038be: 0123       movs       r3, #1
080038c0: 96ed000a   vldr       s0, [r6]
080038c4: 06f5d160   add.w      r0, r6, #0x688
080038c8: c6f8d432   str.w      r3, [r6, #0x2d4]
080038cc: 86f8e032   strb.w     r3, [r6, #0x2e0]
080038d0: 14f0def8   bl         #0x8017a90
080038d4: 96ed000a   vldr       s0, [r6]
080038d8: 06f2a460   addw       r0, r6, #0x6a4
080038dc: 14f0d8f8   bl         #0x8017a90
080038e0: 3846       mov        r0, r7
080038e2: 0027       movs       r7, #0
080038e4: 40f80c49   str        r4, [r0], #-0xc
080038e8: 14f0f0f8   bl         #0x8017acc
080038ec: 06f5d663   add.w      r3, r6, #0x6b0
080038f0: 06f2a460   addw       r0, r6, #0x6a4
080038f4: 1c60       str        r4, [r3]
080038f6: 0024       movs       r4, #0
080038f8: 14f0e8f8   bl         #0x8017acc
080038fc: 06f5d860   add.w      r0, r6, #0x6c0
08003900: 13f02cfe   bl         #0x801755c
08003904: 06f2c460   addw       r0, r6, #0x6c4
08003908: 13f028fe   bl         #0x801755c
0800390c: 06f5da62   add.w      r2, r6, #0x6d0
08003910: 4ff07c53   mov.w      r3, #0x3f000000
08003914: 86f87470   strb.w     r7, [r6, #0x74]
08003918: 3467       str        r4, [r6, #0x70]
0800391a: 1360       str        r3, [r2]
0800391c: 904b       ldr        r3, [pc, #0x240] ; literal @08003b60 = 0x24000cf8 / float 2.77666e-17
0800391e: b767       str        r7, [r6, #0x78]
08003920: 5f60       str        r7, [r3, #4]
08003922: 86f87c70   strb.w     r7, [r6, #0x7c]
08003926: 86f8d476   strb.w     r7, [r6, #0x6d4]
0800392a: 05f0effd   bl         #0x800950c
0800392e: 8c4b       ldr        r3, [pc, #0x230] ; literal @08003b60 = 0x24000cf8 / float 2.77666e-17
08003930: 1862       str        r0, [r3, #0x20]
08003932: 05f0edfd   bl         #0x8009510
08003936: 8a4b       ldr        r3, [pc, #0x228] ; literal @08003b60 = 0x24000cf8 / float 2.77666e-17
08003938: 1a46       mov        r2, r3
0800393a: dc60       str        r4, [r3, #0xc]
0800393c: df62       str        r7, [r3, #0x2c]
0800393e: 9861       str        r0, [r3, #0x18]
08003940: 83ed048a   vstr       s16, [r3, #0x10]
08003944: 4ff07e53   mov.w      r3, #0x3f800000
08003948: 864c       ldr        r4, [pc, #0x218] ; literal @08003b64 = 0x24000ab8 / float 2.77647e-17
0800394a: 9360       str        r3, [r2, #8]
0800394c: 864b       ldr        r3, [pc, #0x218] ; literal @08003b68 = 0x0007a120 / float 7.00649e-40
0800394e: c4f8f471   str.w      r7, [r4, #0x1f4]
08003952: 1363       str        r3, [r2, #0x30]
08003954: d361       str        r3, [r2, #0x1c]
08003956: c4f8f071   str.w      r7, [r4, #0x1f0]
0800395a: 04f14807   add.w      r7, r4, #0x48
0800395e: 6261       str        r2, [r4, #0x14]
08003960: c2e91533   strd       r3, r3, [r2, #0x54]
08003964: c2e91333   strd       r3, r3, [r2, #0x4c]
08003968: 804b       ldr        r3, [pc, #0x200] ; literal @08003b6c = 0x388937e0 / float 6.54308e-05
0800396a: 5361       str        r3, [r2, #0x14]
0800396c: 0223       movs       r3, #2
0800396e: c4e90356   strd       r5, r6, [r4, #0xc]
08003972: 84f8f831   strb.w     r3, [r4, #0x1f8]
08003976: 05f0c9fd   bl         #0x800950c
0800397a: 2346       mov        r3, r4
0800397c: c4f8fc01   str.w      r0, [r4, #0x1fc]
08003980: 0c33       adds       r3, #0xc
08003982: 9fed7b8a   vldr       s16, [pc, #0x1ec] ; literal @08003b70 = 0x00000000 / float 0
08003986: 7b49       ldr        r1, [pc, #0x1ec] ; literal @08003b74 = 0x3f7faa4b / float 0.998692
08003988: 7b4a       ldr        r2, [pc, #0x1ec] ; literal @08003b78 = 0x3aab6a00 / float 0.00130779
0800398a: bb42       cmp        r3, r7
0800398c: 83ed268a   vstr       s16, [r3, #0x98]
08003990: c3f89410   str.w      r1, [r3, #0x94]
08003994: c3f89020   str.w      r2, [r3, #0x90]
08003998: f2d1       bne        #0x8003980
0800399a: 0221       movs       r1, #2
0800399c: 7748       ldr        r0, [pc, #0x1dc] ; literal @08003b7c = 0x24000ad0 / float 2.77647e-17
0800399e: 06f0e7f8   bl         #0x8009b70
080039a2: 0121       movs       r1, #1
080039a4: 7648       ldr        r0, [pc, #0x1d8] ; literal @08003b80 = 0x24000adc / float 2.77648e-17
080039a6: 06f0e3f8   bl         #0x8009b70
080039aa: 764b       ldr        r3, [pc, #0x1d8] ; literal @08003b84 = 0x3f70f27c / float 0.9412
080039ac: b6ee000a   vmov.f32   s0, #5.000000e-01
080039b0: 9fed751a   vldr       s2, [pc, #0x1d4] ; literal @08003b88 = 0x3f4ccccd / float 0.8
080039b4: 2363       str        r3, [r4, #0x30]
080039b6: f0ee480a   vmov.f32   s1, s16
080039ba: 744b       ldr        r3, [pc, #0x1d0] ; literal @08003b8c = 0x3f50a3d7 / float 0.815
080039bc: 7448       ldr        r0, [pc, #0x1d0] ; literal @08003b90 = 0x24000af4 / float 2.77649e-17
080039be: 6363       str        r3, [r4, #0x34]
080039c0: 744b       ldr        r3, [pc, #0x1d0] ; literal @08003b94 = 0x3eb645a2 / float 0.356
080039c2: a363       str        r3, [r4, #0x38]
080039c4: 06f0e4f8   bl         #0x8009b90
080039c8: b7ee000a   vmov.f32   s0, #1.000000e+00
080039cc: 9fed721a   vldr       s2, [pc, #0x1c8] ; literal @08003b98 = 0x3e4ccccd / float 0.2
080039d0: dfed720a   vldr       s1, [pc, #0x1c8] ; literal @08003b9c = 0x3f19999a / float 0.6
080039d4: 7248       ldr        r0, [pc, #0x1c8] ; literal @08003ba0 = 0x24000b00 / float 2.77649e-17
080039d6: 06f0dbf8   bl         #0x8009b90
080039da: 0321       movs       r1, #3
080039dc: 7148       ldr        r0, [pc, #0x1c4] ; literal @08003ba4 = 0x24000b0c / float 2.77649e-17
080039de: 06f0c7f8   bl         #0x8009b70
080039e2: 0721       movs       r1, #7
080039e4: 7048       ldr        r0, [pc, #0x1c0] ; literal @08003ba8 = 0x24000b18 / float 2.7765e-17
080039e6: 06f0c3f8   bl         #0x8009b70
080039ea: 05f08ffd   bl         #0x800950c
080039ee: 2369       ldr        r3, [r4, #0x10]
080039f0: c4f81402   str.w      r0, [r4, #0x214]
080039f4: 0120       movs       r0, #1
080039f6: 4ff07e52   mov.w      r2, #0x3f800000
080039fa: e168       ldr        r1, [r4, #0xc]
080039fc: c3f88406   str.w      r0, [r3, #0x684]
08003a00: c3f8d402   str.w      r0, [r3, #0x2d4]
08003a04: 01f5af68   add.w      r8, r1, #0x578
08003a08: 6848       ldr        r0, [pc, #0x1a0] ; literal @08003bac = 0x3e10d0c3 / float 0.141421
08003a0a: 01f5c469   add.w      sb, r1, #0x620
08003a0e: c3f83c21   str.w      r2, [r3, #0x13c]
08003a12: 1867       str        r0, [r3, #0x70]
08003a14: 6648       ldr        r0, [pc, #0x198] ; literal @08003bb0 = 0x3ca3d70b / float 0.02
08003a16: da63       str        r2, [r3, #0x3c]
08003a18: 1a64       str        r2, [r3, #0x40]
08003a1a: 5a64       str        r2, [r3, #0x44]
08003a1c: 0022       movs       r2, #0
08003a1e: c3f8a401   str.w      r0, [r3, #0x1a4]
08003a22: 03f5da60   add.w      r0, r3, #0x6d0
08003a26: 83ed488a   vstr       s16, [r3, #0x120]
08003a2a: 83ed568a   vstr       s16, [r3, #0x158]
08003a2e: c3f80021   str.w      r2, [r3, #0x100]
08003a32: 80ed008a   vstr       s16, [r0]
08003a36: da60       str        r2, [r3, #0xc]
08003a38: 1a82       strh       r2, [r3, #0x10]
08003a3a: c3e90122   strd       r2, r2, [r3, #4]
08003a3e: a4f82822   strh.w     r2, [r4, #0x228]
08003a42: 84f82a22   strb.w     r2, [r4, #0x22a]
08003a46: 4046       mov        r0, r8
08003a48: 5a4b       ldr        r3, [pc, #0x168] ; literal @08003bb4 = 0x3f333333 / float 0.7
08003a4a: 08f11808   add.w      r8, r8, #0x18
08003a4e: 48f8103c   str        r3, [r8, #-0x10]
08003a52: fdf7abf8   bl         #0x8000bac
08003a56: c145       cmp        sb, r8
08003a58: f5d1       bne        #0x8003a46
08003a5a: d4f80c80   ldr.w      r8, [r4, #0xc]
08003a5e: 08f1e800   add.w      r0, r8, #0xe8
08003a62: 03f075ff   bl         #0x8007950
08003a66: 98f8fd30   ldrb.w     r3, [r8, #0xfd]
08003a6a: 13b1       cbz        r3, #0x8003a72
08003a6c: 80f00100   eor        r0, r0, #1
08003a70: c0b2       uxtb       r0, r0
08003a72: 0028       cmp        r0, #0
08003a74: 00f06385   beq.w      #0x800453e
08003a78: 0323       movs       r3, #3
08003a7a: 84f8f831   strb.w     r3, [r4, #0x1f8]
08003a7e: 3048       ldr        r0, [pc, #0xc0] ; literal @08003b40 = 0x24000468 / float 2.77593e-17
08003a80: 4ff00008   mov.w      r8, #0
08003a84: 02f0ccf8   bl         #0x8005c20
08003a88: 2d48       ldr        r0, [pc, #0xb4] ; literal @08003b40 = 0x24000468 / float 2.77593e-17
08003a8a: f0ee408a   vmov.f32   s17, s0
08003a8e: 02f0e7f8   bl         #0x8005c60
08003a92: 07ee900a   vmov       s15, r0
08003a96: dff834a1   ldr.w      sl, [pc, #0x134] ; literal @08003bcc = 0x2400045c / float 2.77593e-17
08003a9a: b8eee78a   vcvt.f32.s32 s16, s15
08003a9e: 05f0ddfe   bl         #0x800985c
08003aa2: dfed455a   vldr       s11, [pc, #0x114] ; literal @08003bb8 = 0x40c90fdb / float 6.28319
08003aa6: 07ee900a   vmov       s15, r0
08003aaa: b7ee007a   vmov.f32   s14, #1.000000e+00
08003aae: 0323       movs       r3, #3
08003ab0: 88ee886a   vdiv.f32   s12, s17, s16
08003ab4: 414a       ldr        r2, [pc, #0x104] ; literal @08003bbc = 0x24001460 / float 2.77728e-17
08003ab6: 8df82c30   strb.w     r3, [sp, #0x2c]
08003aba: 0423       movs       r3, #4
08003abc: 4049       ldr        r1, [pc, #0x100] ; literal @08003bc0 = 0x7fc00000 / float nan
08003abe: 06a8       add        r0, sp, #0x18
08003ac0: 8df82d30   strb.w     r3, [sp, #0x2d]
08003ac4: 0123       movs       r3, #1
08003ac6: cdf80080   str.w      r8, [sp]
08003aca: 88ee289a   vdiv.f32   s18, s16, s17
08003ace: 1370       strb       r3, [r2]
08003ad0: d160       str        r1, [r2, #0xc]
08003ad2: 1161       str        r1, [r2, #0x10]
08003ad4: 5161       str        r1, [r2, #0x14]
08003ad6: 0b99       ldr        r1, [sp, #0x2c]
08003ad8: f8ee677a   vcvt.f32.u32 s15, s15
08003adc: cdf82480   str.w      r8, [sp, #0x24]
08003ae0: cde90788   strd       r8, r8, [sp, #0x1c]
08003ae4: c5ee866a   vdiv.f32   s13, s11, s12
08003ae8: 67ee897a   vmul.f32   s15, s15, s18
08003aec: 87ee276a   vdiv.f32   s12, s14, s15
08003af0: 36ee877a   vadd.f32   s14, s13, s14
08003af4: c6ee877a   vdiv.f32   s15, s13, s14
08003af8: 82ed016a   vstr       s12, [r2, #4]
08003afc: c2ed067a   vstr       s15, [r2, #0x18]
08003b00: 4ff60b72   movw       r2, #0xff0b
08003b04: 0692       str        r2, [sp, #0x18]
08003b06: 4246       mov        r2, r8
08003b08: 03f016ff   bl         #0x8007938
08003b0c: 06a8       add        r0, sp, #0x18
08003b0e: 03f01fff   bl         #0x8007950
08003b12: 2c4a       ldr        r2, [pc, #0xb0] ; literal @08003bc4 = 0x24000410 / float 2.7759e-17
08003b14: 80f00100   eor        r0, r0, #1
08003b18: 4346       mov        r3, r8
08003b1a: dff8b480   ldr.w      r8, [pc, #0xb4] ; literal @08003bd0 = 0x24001444 / float 2.77727e-17
08003b1e: 1070       strb       r0, [r2]
08003b20: 0c22       movs       r2, #0xc
08003b22: c8f80050   str.w      r5, [r8]
08003b26: c8f80420   str.w      r2, [r8, #4]
08003b2a: 3822       movs       r2, #0x38
08003b2c: 264d       ldr        r5, [pc, #0x98] ; literal @08003bc8 = 0x24000420 / float 2.77591e-17
08003b2e: c8f80820   str.w      r2, [r8, #8]
08003b32: 5246       mov        r2, sl
08003b34: c8f80ca0   str.w      sl, [r8, #0xc]
08003b38: c8f81050   str.w      r5, [r8, #0x10]
08003b3c: 4ce0       b          #0x8003bd8
08003b3e: 00bf       nop        
08003b40: 6804       lsls       r0, r5, #0x11
08003b42: 0024       movs       r4, #0
08003b44: 680d       lsrs       r0, r5, #0x15
08003b46: 0024       movs       r4, #0
08003b48: 008e       ldrh       r0, [r0, #0x30]
08003b4a: bb47       .byte      0xbb, 0x47
08003b4c: 0070       strb       r0, [r0]
08003b4e: 1447       bxns       r2
08003b50: 0000       movs       r0, r0
08003b52: 00c0       .byte      0x00, 0xc0
08003b54: 68ca       ldm        r2!, {r3, r5, r6}
08003b56: db00       lsls       r3, r3, #3
08003b58: d094       str        r4, [sp, #0x340]
08003b5a: b7c1       stm        r1!, {r0, r1, r2, r4, r5, r7}
08003b5c: 34e5       b          #0x80035c8
08003b5e: 6d00       lsls       r5, r5, #1
08003b60: f80c       lsrs       r0, r7, #0x13
08003b62: 0024       movs       r4, #0
08003b64: b80a       lsrs       r0, r7, #0xa
08003b66: 0024       movs       r4, #0
08003b68: 20a1       adr        r1, #0x80
08003b6a: 0700       movs       r7, r0
08003b6c: e037       adds       r7, #0xe0
08003b6e: 8938       subs       r0, #0x89
08003b70: 0000       movs       r0, r0
08003b72: 0000       movs       r0, r0
08003b74: 4baa       add        r2, sp, #0x12c
08003b76: 7f3f       subs       r7, #0x7f
08003b78: 006a       ldr        r0, [r0, #0x20]
08003b7a: ab3a       subs       r2, #0xab
08003b7c: d00a       lsrs       r0, r2, #0xb
08003b7e: 0024       movs       r4, #0
08003b80: dc0a       lsrs       r4, r3, #0xb
08003b82: 0024       movs       r4, #0
08003b84: 7cf2       .byte      0x7c, 0xf2
08003b86: 703f       subs       r7, #0x70
08003b88: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08003b8a: 4c3f       subs       r7, #0x4c
08003b8c: d7a3       adr        r3, #0x35c
08003b8e: 503f       subs       r7, #0x50
08003b90: f40a       lsrs       r4, r6, #0xb
08003b92: 0024       movs       r4, #0
08003b94: a245       cmp        sl, r4
08003b96: b63e       subs       r6, #0xb6
08003b98: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08003b9a: 4c3e       subs       r6, #0x4c
08003b9c: 9a99       ldr        r1, [sp, #0x268]
08003b9e: 193f       subs       r7, #0x19
08003ba0: 000b       lsrs       r0, r0, #0xc
08003ba2: 0024       movs       r4, #0
08003ba4: 0c0b       lsrs       r4, r1, #0xc
08003ba6: 0024       movs       r4, #0
08003ba8: 180b       lsrs       r0, r3, #0xc
08003baa: 0024       movs       r4, #0
08003bac: c3d0       beq        #0x8003b36
08003bae: 103e       subs       r6, #0x10
08003bb0: 0bd7       bvc        #0x8003bca
08003bb2: a33c       subs       r4, #0xa3
08003bb4: 3333       adds       r3, #0x33
08003bb6: 333f       subs       r7, #0x33
08003bb8: db0f       lsrs       r3, r3, #0x1f
08003bba: c940       lsrs       r1, r1
08003bbc: 6014       asrs       r0, r4, #0x11
08003bbe: 0024       movs       r4, #0
08003bc0: 0000       movs       r0, r0
08003bc2: c07f       ldrb       r0, [r0, #0x1f]
08003bc4: 1004       lsls       r0, r2, #0x10
08003bc6: 0024       movs       r4, #0
08003bc8: 2004       lsls       r0, r4, #0x10
08003bca: 0024       movs       r4, #0
08003bcc: 5c04       lsls       r4, r3, #0x11
08003bce: 0024       movs       r4, #0
08003bd0: 4414       asrs       r4, r0, #0x11
08003bd2: 0024       movs       r4, #0
08003bd4: d8f80c20   ldr.w      r2, [r8, #0xc]
08003bd8: 03f19021   add.w      r1, r3, #-0x6fff7000
08003bdc: 0978       ldrb       r1, [r1]
08003bde: d154       strb       r1, [r2, r3]
08003be0: 0133       adds       r3, #1
08003be2: 0c2b       cmp        r3, #0xc
08003be4: f6d1       bne        #0x8003bd4
08003be6: 0023       movs       r3, #0
08003be8: ac49       ldr        r1, [pc, #0x2b0] ; literal @08003e9c = 0x90001000 / float -2.52559e-29
08003bea: d8f81020   ldr.w      r2, [r8, #0x10]
08003bee: 595c       ldrb       r1, [r3, r1]
08003bf0: d154       strb       r1, [r2, r3]
08003bf2: 0133       adds       r3, #1
08003bf4: 382b       cmp        r3, #0x38
08003bf6: f7d1       bne        #0x8003be8
08003bf8: 2b78       ldrb       r3, [r5]
08003bfa: 042b       cmp        r3, #4
08003bfc: 14d0       beq        #0x8003c28
08003bfe: 4ff07e53   mov.w      r3, #0x3f800000
08003c02: eb60       str        r3, [r5, #0xc]
08003c04: 2b61       str        r3, [r5, #0x10]
08003c06: 6b61       str        r3, [r5, #0x14]
08003c08: 2b63       str        r3, [r5, #0x30]
08003c0a: 0023       movs       r3, #0
08003c0c: 6b60       str        r3, [r5, #4]
08003c0e: 2b72       strb       r3, [r5, #8]
08003c10: 6b62       str        r3, [r5, #0x24]
08003c12: 6b63       str        r3, [r5, #0x34]
08003c14: c5e90733   strd       r3, r3, [r5, #0x1c]
08003c18: 0423       movs       r3, #4
08003c1a: 2b60       str        r3, [r5]
08003c1c: 0123       movs       r3, #1
08003c1e: ab62       str        r3, [r5, #0x28]
08003c20: 9f4b       ldr        r3, [pc, #0x27c] ; literal @08003ea0 = 0x3e10d0c3 / float 0.141421
08003c22: ab61       str        r3, [r5, #0x18]
08003c24: 9f4b       ldr        r3, [pc, #0x27c] ; literal @08003ea4 = 0x3f333333 / float 0.7
08003c26: eb62       str        r3, [r5, #0x2c]
08003c28: 9af80030   ldrb.w     r3, [sl]
08003c2c: 052b       cmp        r3, #5
08003c2e: 40f06184   bne.w      #0x80044f4
08003c32: 0bab       add        r3, sp, #0x2c
08003c34: 08ee903a   vmov       s17, r3
08003c38: ab6a       ldr        r3, [r5, #0x28]
08003c3a: f7ee006a   vmov.f32   s13, #1.000000e+00
08003c3e: 95ed0b7a   vldr       s14, [r5, #0x2c]
08003c42: c6f88436   str.w      r3, [r6, #0x684]
08003c46: c6f8d432   str.w      r3, [r6, #0x2d4]
08003c4a: 6b68       ldr        r3, [r5, #4]
08003c4c: 9fed968a   vldr       s16, [pc, #0x258] ; literal @08003ea8 = 0x3e4ccccd / float 0.2
08003c50: f360       str        r3, [r6, #0xc]
08003c52: eb69       ldr        r3, [r5, #0x1c]
08003c54: b4eec87a   vcmpe.f32  s14, s16
08003c58: 944a       ldr        r2, [pc, #0x250] ; literal @08003eac = 0x24000cf8 / float 2.77666e-17
08003c5a: 7360       str        r3, [r6, #4]
08003c5c: 2b6a       ldr        r3, [r5, #0x20]
08003c5e: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003c62: dff870b2   ldr.w      fp, [pc, #0x270] ; literal @08003ed4 = 0x240009e0 / float 2.77639e-17
08003c66: c6f80031   str.w      r3, [r6, #0x100]
08003c6a: 6b6a       ldr        r3, [r5, #0x24]
08003c6c: 58bf       it         pl
08003c6e: b0ee478a   vmovpl.f32 s16, s14
08003c72: d5ed067a   vldr       s15, [r5, #0x18]
08003c76: 0bf1a809   add.w      sb, fp, #0xa8
08003c7a: 1360       str        r3, [r2]
08003c7c: ab78       ldrb       r3, [r5, #2]
08003c7e: 9fed8c6a   vldr       s12, [pc, #0x230] ; literal @08003eb0 = 0x00000000 / float 0
08003c82: 84f82932   strb.w     r3, [r4, #0x229]
08003c86: 6b78       ldrb       r3, [r5, #1]
08003c88: c7fe867a   vmaxnm.f32 s15, s15, s12
08003c8c: c7fee67a   vminnm.f32 s15, s15, s13
08003c90: 67eea76a   vmul.f32   s13, s15, s15
08003c94: 84f82a32   strb.w     r3, [r4, #0x22a]
08003c98: eb78       ldrb       r3, [r5, #3]
08003c9a: c6ed1c7a   vstr       s15, [r6, #0x70]
08003c9e: c6ed696a   vstr       s13, [r6, #0x1a4]
08003ca2: 84f82832   strb.w     r3, [r4, #0x228]
08003ca6: 8bed028a   vstr       s16, [fp, #8]
08003caa: 5846       mov        r0, fp
08003cac: 0bf1180b   add.w      fp, fp, #0x18
08003cb0: fcf77cff   bl         #0x8000bac
08003cb4: d945       cmp        sb, fp
08003cb6: f6d1       bne        #0x8003ca6
08003cb8: d5ed0c7a   vldr       s15, [r5, #0x30]
08003cbc: f7ee006a   vmov.f32   s13, #1.000000e+00
08003cc0: 9fed7c6a   vldr       s12, [pc, #0x1f0] ; literal @08003eb4 = 0x3f8ccccd / float 1.1
08003cc4: b6ee007a   vmov.f32   s14, #5.000000e-01
08003cc8: 7b4b       ldr        r3, [pc, #0x1ec] ; literal @08003eb8 = 0x24001438 / float 2.77727e-17
08003cca: 67ee867a   vmul.f32   s15, s15, s12
08003cce: 7b4a       ldr        r2, [pc, #0x1ec] ; literal @08003ebc = 0x24001434 / float 2.77727e-17
08003cd0: f4ee667a   vcmp.f32   s15, s13
08003cd4: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003cd8: 76fea77a   vselgt.f32 s15, s13, s15
08003cdc: a7eec77a   vfms.f32   s14, s15, s14
08003ce0: 83ed007a   vstr       s14, [r3]
08003ce4: eb68       ldr        r3, [r5, #0xc]
08003ce6: f363       str        r3, [r6, #0x3c]
08003ce8: 2b69       ldr        r3, [r5, #0x10]
08003cea: 3364       str        r3, [r6, #0x40]
08003cec: 6b69       ldr        r3, [r5, #0x14]
08003cee: 7364       str        r3, [r6, #0x44]
08003cf0: daf80830   ldr.w      r3, [sl, #8]
08003cf4: 1360       str        r3, [r2]
08003cf6: daf80430   ldr.w      r3, [sl, #4]
08003cfa: 42f8043d   str        r3, [r2, #-0x4]!
08003cfe: 0392       str        r2, [sp, #0xc]
08003d00: d6f88426   ldr.w      r2, [r6, #0x684]
08003d04: 6b6b       ldr        r3, [r5, #0x34]
08003d06: 032a       cmp        r2, #3
08003d08: b360       str        r3, [r6, #8]
08003d0a: 03dd       ble        #0x8003d14
08003d0c: 012b       cmp        r3, #1
08003d0e: 08bf       it         eq
08003d10: c6f88436   streq.w    r3, [r6, #0x684]
08003d14: 6a48       ldr        r0, [pc, #0x1a8] ; literal @08003ec0 = 0x24000480 / float 2.77594e-17
08003d16: 03f043fc   bl         #0x80075a0
08003d1a: 6a49       ldr        r1, [pc, #0x1a8] ; literal @08003ec4 = 0x080032fd / float 3.85785e-34
08003d1c: 6a48       ldr        r0, [pc, #0x1a8] ; literal @08003ec8 = 0x24000468 / float 2.77593e-17
08003d1e: 01f07bff   bl         #0x8005c18
08003d22: d4f8f431   ldr.w      r3, [r4, #0x1f4]
08003d26: d4f8f021   ldr.w      r2, [r4, #0x1f0]
08003d2a: 9b1a       subs       r3, r3, r2
08003d2c: da06       lsls       r2, r3, #0x1b
08003d2e: 00f01381   beq.w      #0x8003f58
08003d32: d4f8f031   ldr.w      r3, [r4, #0x1f0]
08003d36: 03f11e01   add.w      r1, r3, #0x1e
08003d3a: 0133       adds       r3, #1
08003d3c: 04ebc102   add.w      r2, r4, r1, lsl #3
08003d40: 14f83110   ldrb.w     r1, [r4, r1, lsl #3]
08003d44: 03f01f03   and        r3, r3, #0x1f
08003d48: 0429       cmp        r1, #4
08003d4a: d2ed017a   vldr       s15, [r2, #4]
08003d4e: 5288       ldrh       r2, [r2, #2]
08003d50: c4f8f031   str.w      r3, [r4, #0x1f0]
08003d54: 39d0       beq        #0x8003dca
08003d56: 0029       cmp        r1, #0
08003d58: e3d1       bne        #0x8003d22
08003d5a: e368       ldr        r3, [r4, #0xc]
08003d5c: 93f89010   ldrb.w     r1, [r3, #0x90]
08003d60: 052a       cmp        r2, #5
08003d62: ded8       bhi        #0x8003d22
08003d64: dfe802f0   tbb        [pc, r2]
08003d68: 2922       movs       r2, #0x29
08003d6a: 1b13       asrs       r3, r3, #0xc
08003d6c: 0b03       lsls       r3, r1, #0xc
08003d6e: 2369       ldr        r3, [r4, #0x10]
08003d70: ff29       cmp        r1, #0xff
08003d72: 0cbf       ite        eq
08003d74: c3ed117a   vstreq     s15, [r3, #0x44]
08003d78: c3ed0e7a   vstrne     s15, [r3, #0x38]
08003d7c: d1e7       b          #0x8003d22
08003d7e: 2369       ldr        r3, [r4, #0x10]
08003d80: ff29       cmp        r1, #0xff
08003d82: 0cbf       ite        eq
08003d84: c3ed107a   vstreq     s15, [r3, #0x40]
08003d88: c3ed0d7a   vstrne     s15, [r3, #0x34]
08003d8c: c9e7       b          #0x8003d22
08003d8e: 2369       ldr        r3, [r4, #0x10]
08003d90: ff29       cmp        r1, #0xff
08003d92: 0cbf       ite        eq
08003d94: c3ed0f7a   vstreq     s15, [r3, #0x3c]
08003d98: c3ed0c7a   vstrne     s15, [r3, #0x30]
08003d9c: c1e7       b          #0x8003d22
08003d9e: ff29       cmp        r1, #0xff
08003da0: 00f0ac82   beq.w      #0x80042fc
08003da4: 2369       ldr        r3, [r4, #0x10]
08003da6: c3ed147a   vstr       s15, [r3, #0x50]
08003daa: bae7       b          #0x8003d22
08003dac: ff29       cmp        r1, #0xff
08003dae: 00f0ff82   beq.w      #0x80043b0
08003db2: 2369       ldr        r3, [r4, #0x10]
08003db4: c3ed137a   vstr       s15, [r3, #0x4c]
08003db8: b3e7       b          #0x8003d22
08003dba: ff29       cmp        r1, #0xff
08003dbc: 00f08c82   beq.w      #0x80042d8
08003dc0: d3f8dc21   ldr.w      r2, [r3, #0x1dc]
08003dc4: 6369       ldr        r3, [r4, #0x14]
08003dc6: 9a63       str        r2, [r3, #0x38]
08003dc8: abe7       b          #0x8003d22
08003dca: f5ee407a   vcmp.f32   s15, #0
08003dce: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003dd2: 19d1       bne        #0x8003e08
08003dd4: 002a       cmp        r2, #0
08003dd6: 40f08b81   bne.w      #0x80040f0
08003dda: 94f8f831   ldrb.w     r3, [r4, #0x1f8]
08003dde: 042b       cmp        r3, #4
08003de0: 00f0dc81   beq.w      #0x800419c
08003de4: 0123       movs       r3, #1
08003de6: 84f8f831   strb.w     r3, [r4, #0x1f8]
08003dea: 05f08ffb   bl         #0x800950c
08003dee: 2346       mov        r3, r4
08003df0: c4f8fc01   str.w      r0, [r4, #0x1fc]
08003df4: 354a       ldr        r2, [pc, #0xd4] ; literal @08003ecc = 0x3f72f1c8 / float 0.949002
08003df6: 0c33       adds       r3, #0xc
08003df8: c3f89420   str.w      r2, [r3, #0x94]
08003dfc: 344a       ldr        r2, [pc, #0xd0] ; literal @08003ed0 = 0x3d50e380 / float 0.0509982
08003dfe: c3f89020   str.w      r2, [r3, #0x90]
08003e02: bb42       cmp        r3, r7
08003e04: f6d1       bne        #0x8003df4
08003e06: 8ce7       b          #0x8003d22
08003e08: e368       ldr        r3, [r4, #0xc]
08003e0a: 93f89010   ldrb.w     r1, [r3, #0x90]
08003e0e: 062a       cmp        r2, #6
08003e10: 87d8       bhi        #0x8003d22
08003e12: dfe802f0   tbb        [pc, r2]
08003e16: 8c82       strh       r4, [r1, #0x14]
08003e18: 7161       str        r1, [r6, #0x14]
08003e1a: 2f1b       subs       r7, r5, r4
08003e1c: 0400       movs       r4, r0
08003e1e: ff29       cmp        r1, #0xff
08003e20: 00f09582   beq.w      #0x800434e
08003e24: 2269       ldr        r2, [r4, #0x10]
08003e26: 9368       ldr        r3, [r2, #8]
08003e28: 012b       cmp        r3, #1
08003e2a: d2f88436   ldr.w      r3, [r2, #0x684]
08003e2e: 08bf       it         eq
08003e30: 0421       moveq      r1, #4
08003e32: 03f10103   add.w      r3, r3, #1
08003e36: 18bf       it         ne
08003e38: 0621       movne      r1, #6
08003e3a: 93fbf1f0   sdiv       r0, r3, r1
08003e3e: 01fb1033   mls        r3, r1, r0, r3
08003e42: 03b9       cbnz       r3, #0x8003e46
08003e44: 0123       movs       r3, #1
08003e46: c2f88436   str.w      r3, [r2, #0x684]
08003e4a: 6ae7       b          #0x8003d22
08003e4c: 94f8f831   ldrb.w     r3, [r4, #0x1f8]
08003e50: 042b       cmp        r3, #4
08003e52: 00f0d782   beq.w      #0x8004404
08003e56: ff29       cmp        r1, #0xff
08003e58: 00f00883   beq.w      #0x800446c
08003e5c: 2369       ldr        r3, [r4, #0x10]
08003e5e: 5a68       ldr        r2, [r3, #4]
08003e60: 002a       cmp        r2, #0
08003e62: 40f09c82   bne.w      #0x800439e
08003e66: d97b       ldrb       r1, [r3, #0xf]
08003e68: 11b9       cbnz       r1, #0x8003e70
08003e6a: da7c       ldrb       r2, [r3, #0x13]
08003e6c: 82f00102   eor        r2, r2, #1
08003e70: da73       strb       r2, [r3, #0xf]
08003e72: 56e7       b          #0x8003d22
08003e74: 94f8f821   ldrb.w     r2, [r4, #0x1f8]
08003e78: 042a       cmp        r2, #4
08003e7a: 00f07782   beq.w      #0x800436c
08003e7e: ff29       cmp        r1, #0xff
08003e80: 2369       ldr        r3, [r4, #0x10]
08003e82: 00f06b82   beq.w      #0x800435c
08003e86: 5a68       ldr        r2, [r3, #4]
08003e88: 002a       cmp        r2, #0
08003e8a: 40f07f82   bne.w      #0x800438c
08003e8e: 197b       ldrb       r1, [r3, #0xc]
08003e90: 11b9       cbnz       r1, #0x8003e98
08003e92: 9a7c       ldrb       r2, [r3, #0x12]
08003e94: 82f00102   eor        r2, r2, #1
08003e98: 1a73       strb       r2, [r3, #0xc]
08003e9a: 42e7       b          #0x8003d22
08003e9c: 0010       asrs       r0, r0, #0x20
08003e9e: 0090       str        r0, [sp]
08003ea0: c3d0       beq        #0x8003e2a
08003ea2: 103e       subs       r6, #0x10
08003ea4: 3333       adds       r3, #0x33
08003ea6: 333f       subs       r7, #0x33
08003ea8: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08003eaa: 4c3e       subs       r6, #0x4c
08003eac: f80c       lsrs       r0, r7, #0x13
08003eae: 0024       movs       r4, #0
08003eb0: 0000       movs       r0, r0
08003eb2: 0000       movs       r0, r0
08003eb4: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08003eb6: 8c3f       subs       r7, #0x8c
08003eb8: 3814       asrs       r0, r7, #0x10
08003eba: 0024       movs       r4, #0
08003ebc: 3414       asrs       r4, r6, #0x10
08003ebe: 0024       movs       r4, #0
08003ec0: 8004       lsls       r0, r0, #0x12
08003ec2: 0024       movs       r4, #0
08003ec4: fd32       adds       r2, #0xfd
08003ec6: 0008       lsrs       r0, r0, #0x20
08003ec8: 6804       lsls       r0, r5, #0x11
08003eca: 0024       movs       r4, #0
08003ecc: c8f1723f   rsb.w      pc, r8, #0x72727272
08003ed0: 80e3       b          #0x80045d4
08003ed2: 503d       subs       r5, #0x50
08003ed4: e009       lsrs       r0, r4, #7
08003ed6: 0024       movs       r4, #0
08003ed8: ff29       cmp        r1, #0xff
08003eda: 94f82832   ldrb.w     r3, [r4, #0x228]
08003ede: 00f03182   beq.w      #0x8004344
08003ee2: 002b       cmp        r3, #0
08003ee4: 40f04e82   bne.w      #0x8004384
08003ee8: 2269       ldr        r2, [r4, #0x10]
08003eea: 517c       ldrb       r1, [r2, #0x11]
08003eec: 11b9       cbnz       r1, #0x8003ef4
08003eee: 537d       ldrb       r3, [r2, #0x15]
08003ef0: 83f00103   eor        r3, r3, #1
08003ef4: 5374       strb       r3, [r2, #0x11]
08003ef6: 14e7       b          #0x8003d22
08003ef8: ff29       cmp        r1, #0xff
08003efa: 2269       ldr        r2, [r4, #0x10]
08003efc: 00f07482   beq.w      #0x80043e8
08003f00: 1379       ldrb       r3, [r2, #4]
08003f02: 0133       adds       r3, #1
08003f04: 03f00103   and        r3, r3, #1
08003f08: 5360       str        r3, [r2, #4]
08003f0a: 002b       cmp        r3, #0
08003f0c: 7ff409af   bne.w      #0x8003d22
08003f10: 4ff07e53   mov.w      r3, #0x3f800000
08003f14: c2f83c31   str.w      r3, [r2, #0x13c]
08003f18: 03e7       b          #0x8003d22
08003f1a: ff29       cmp        r1, #0xff
08003f1c: 00f00b82   beq.w      #0x8004336
08003f20: 6269       ldr        r2, [r4, #0x14]
08003f22: 1368       ldr        r3, [r2]
08003f24: b3fa83f3   clz        r3, r3
08003f28: 5b09       lsrs       r3, r3, #5
08003f2a: 1360       str        r3, [r2]
08003f2c: f9e6       b          #0x8003d22
08003f2e: 0023       movs       r3, #0
08003f30: 84f8f831   strb.w     r3, [r4, #0x1f8]
08003f34: 05f0eafa   bl         #0x800950c
08003f38: 0123       movs       r3, #1
08003f3a: c4f8fc01   str.w      r0, [r4, #0x1fc]
08003f3e: 84f81132   strb.w     r3, [r4, #0x211]
08003f42: 2346       mov        r3, r4
08003f44: b64a       ldr        r2, [pc, #0x2d8] ; literal @08004220 = 0x3f7faa4b / float 0.998692
08003f46: 0c33       adds       r3, #0xc
08003f48: c3f89420   str.w      r2, [r3, #0x94]
08003f4c: b54a       ldr        r2, [pc, #0x2d4] ; literal @08004224 = 0x3aab6a00 / float 0.00130779
08003f4e: c3f89020   str.w      r2, [r3, #0x90]
08003f52: bb42       cmp        r3, r7
08003f54: f6d1       bne        #0x8003f44
08003f56: e4e6       b          #0x8003d22
08003f58: 05f0d8fa   bl         #0x800950c
08003f5c: 94f8f831   ldrb.w     r3, [r4, #0x1f8]
08003f60: 8146       mov        sb, r0
08003f62: da1e       subs       r2, r3, #3
08003f64: 012a       cmp        r2, #1
08003f66: 0cd9       bls        #0x8003f82
08003f68: 012b       cmp        r3, #1
08003f6a: 0ad9       bls        #0x8003f82
08003f6c: d4f8fc31   ldr.w      r3, [r4, #0x1fc]
08003f70: 6268       ldr        r2, [r4, #4]
08003f72: c31a       subs       r3, r0, r3
08003f74: 9342       cmp        r3, r2
08003f76: 04d9       bls        #0x8003f82
08003f78: 0023       movs       r3, #0
08003f7a: c4f8fc01   str.w      r0, [r4, #0x1fc]
08003f7e: 84f8f831   strb.w     r3, [r4, #0x1f8]
08003f82: d4f80c32   ldr.w      r3, [r4, #0x20c]
08003f86: a9eb0303   sub.w      r3, sb, r3
08003f8a: 022b       cmp        r3, #2
08003f8c: 00f2c280   bhi.w      #0x8004114
08003f90: 94f81132   ldrb.w     r3, [r4, #0x211]
08003f94: 002b       cmp        r3, #0
08003f96: 3ad0       beq        #0x800400e
08003f98: 7368       ldr        r3, [r6, #4]
08003f9a: 327b       ldrb       r2, [r6, #0xc]
08003f9c: 002b       cmp        r3, #0
08003f9e: 40f0ef80   bne.w      #0x8004180
08003fa2: 02b9       cbnz       r2, #0x8003fa6
08003fa4: b27c       ldrb       r2, [r6, #0x12]
08003fa6: 2a71       strb       r2, [r5, #4]
08003fa8: 727b       ldrb       r2, [r6, #0xd]
08003faa: 6a71       strb       r2, [r5, #5]
08003fac: b27b       ldrb       r2, [r6, #0xe]
08003fae: aa71       strb       r2, [r5, #6]
08003fb0: f27b       ldrb       r2, [r6, #0xf]
08003fb2: 02b9       cbnz       r2, #0x8003fb6
08003fb4: f27c       ldrb       r2, [r6, #0x13]
08003fb6: eb61       str        r3, [r5, #0x1c]
08003fb8: b0ee007a   vmov.f32   s14, #2.000000e+00
08003fbc: 9a4b       ldr        r3, [pc, #0x268] ; literal @08004228 = 0x24001438 / float 2.77727e-17
08003fbe: f7ee007a   vmov.f32   s15, #1.000000e+00
08003fc2: ea71       strb       r2, [r5, #7]
08003fc4: d3ed006a   vldr       s13, [r3]
08003fc8: 336f       ldr        r3, [r6, #0x70]
08003fca: e6eec77a   vfms.f32   s15, s13, s14
08003fce: ab61       str        r3, [r5, #0x18]
08003fd0: d6f8d432   ldr.w      r3, [r6, #0x2d4]
08003fd4: ab62       str        r3, [r5, #0x28]
08003fd6: d6f80031   ldr.w      r3, [r6, #0x100]
08003fda: c5ed0c7a   vstr       s15, [r5, #0x30]
08003fde: 2b62       str        r3, [r5, #0x20]
08003fe0: 924b       ldr        r3, [pc, #0x248] ; literal @0800422c = 0x24000cf8 / float 2.77666e-17
08003fe2: 1b68       ldr        r3, [r3]
08003fe4: 6b62       str        r3, [r5, #0x24]
08003fe6: 94f82932   ldrb.w     r3, [r4, #0x229]
08003fea: ab70       strb       r3, [r5, #2]
08003fec: 94f82a32   ldrb.w     r3, [r4, #0x22a]
08003ff0: 6b70       strb       r3, [r5, #1]
08003ff2: 94f82832   ldrb.w     r3, [r4, #0x228]
08003ff6: eb70       strb       r3, [r5, #3]
08003ff8: 8d4b       ldr        r3, [pc, #0x234] ; literal @08004230 = 0x240009e8 / float 2.7764e-17
08003ffa: 1b68       ldr        r3, [r3]
08003ffc: eb62       str        r3, [r5, #0x2c]
08003ffe: f36b       ldr        r3, [r6, #0x3c]
08004000: eb60       str        r3, [r5, #0xc]
08004002: 336c       ldr        r3, [r6, #0x40]
08004004: 2b61       str        r3, [r5, #0x10]
08004006: 736c       ldr        r3, [r6, #0x44]
08004008: 6b61       str        r3, [r5, #0x14]
0800400a: b368       ldr        r3, [r6, #8]
0800400c: 6b63       str        r3, [r5, #0x34]
0800400e: 94f81232   ldrb.w     r3, [r4, #0x212]
08004012: 73b3       cbz        r3, #0x8004072
08004014: d4f83832   ldr.w      r3, [r4, #0x238]
08004018: caf80430   str.w      r3, [sl, #4]
0800401c: d4f83c32   ldr.w      r3, [r4, #0x23c]
08004020: caf80830   str.w      r3, [sl, #8]
08004024: 0523       movs       r3, #5
08004026: 8af80030   strb.w     r3, [sl]
0800402a: 94f81332   ldrb.w     r3, [r4, #0x213]
0800402e: 002b       cmp        r3, #0
08004030: 40f00c81   bne.w      #0x800424c
08004034: d8f80420   ldr.w      r2, [r8, #4]
08004038: 4ff09021   mov.w      r1, #-0x6fff7000
0800403c: d8f80000   ldr.w      r0, [r8]
08004040: 02f19022   add.w      r2, r2, #-0x6fff7000
08004044: 04f028fd   bl         #0x8008a98
08004048: d8f80c30   ldr.w      r3, [r8, #0xc]
0800404c: d8f80420   ldr.w      r2, [r8, #4]
08004050: 4ff09021   mov.w      r1, #-0x6fff7000
08004054: d8f80000   ldr.w      r0, [r8]
08004058: 04f01afd   bl         #0x8008a90
0800405c: 0023       movs       r3, #0
0800405e: 754a       ldr        r2, [pc, #0x1d4] ; literal @08004234 = 0x24001434 / float 2.77727e-17
08004060: a4f81232   strh.w     r3, [r4, #0x212]
08004064: daf80830   ldr.w      r3, [sl, #8]
08004068: 1360       str        r3, [r2]
0800406a: daf80430   ldr.w      r3, [sl, #4]
0800406e: 039a       ldr        r2, [sp, #0xc]
08004070: 1360       str        r3, [r2]
08004072: 05f04bfa   bl         #0x800950c
08004076: d8f81430   ldr.w      r3, [r8, #0x14]
0800407a: c01a       subs       r0, r0, r3
0800407c: b0f5fa6f   cmp.w      r0, #0x7d0
08004080: 7ff64fae   bls.w      #0x8003d22
08004084: d8f80820   ldr.w      r2, [r8, #8]
08004088: 002a       cmp        r2, #0
0800408a: 3ff44aae   beq.w      #0x8003d22
0800408e: d8f81000   ldr.w      r0, [r8, #0x10]
08004092: 0121       movs       r1, #1
08004094: 4ff0000e   mov.w      lr, #0
08004098: 0138       subs       r0, #1
0800409a: 04e0       b          #0x80040a6
0800409c: 9142       cmp        r1, r2
0800409e: 4ff0010e   mov.w      lr, #1
080040a2: 0ed2       bhs        #0x80040c2
080040a4: 0131       adds       r1, #1
080040a6: 644b       ldr        r3, [pc, #0x190] ; literal @08004238 = 0x90001000 / float -2.52559e-29
080040a8: 10f801cf   ldrb       ip, [r0, #1]!
080040ac: 0b44       add        r3, r1
080040ae: 13f8013c   ldrb       r3, [r3, #-0x1]
080040b2: 6345       cmp        r3, ip
080040b4: f2d1       bne        #0x800409c
080040b6: 9142       cmp        r1, r2
080040b8: f4d3       blo        #0x80040a4
080040ba: bef1000f   cmp.w      lr, #0
080040be: 3ff430ae   beq.w      #0x8003d22
080040c2: 5d49       ldr        r1, [pc, #0x174] ; literal @08004238 = 0x90001000 / float -2.52559e-29
080040c4: d8f80000   ldr.w      r0, [r8]
080040c8: 0a44       add        r2, r1
080040ca: 04f0e5fc   bl         #0x8008a98
080040ce: d8f81030   ldr.w      r3, [r8, #0x10]
080040d2: 5949       ldr        r1, [pc, #0x164] ; literal @08004238 = 0x90001000 / float -2.52559e-29
080040d4: d8f80820   ldr.w      r2, [r8, #8]
080040d8: d8f80000   ldr.w      r0, [r8]
080040dc: 04f0d8fc   bl         #0x8008a90
080040e0: 05f014fa   bl         #0x800950c
080040e4: 0023       movs       r3, #0
080040e6: c8f81400   str.w      r0, [r8, #0x14]
080040ea: 84f81132   strb.w     r3, [r4, #0x211]
080040ee: 18e6       b          #0x8003d22
080040f0: 032a       cmp        r2, #3
080040f2: 7ff416ae   bne.w      #0x8003d22
080040f6: e368       ldr        r3, [r4, #0xc]
080040f8: 93f89030   ldrb.w     r3, [r3, #0x90]
080040fc: ff2b       cmp        r3, #0xff
080040fe: 3ff410ae   beq.w      #0x8003d22
08004102: 94f82832   ldrb.w     r3, [r4, #0x228]
08004106: 002b       cmp        r3, #0
08004108: 3ff40bae   beq.w      #0x8003d22
0800410c: 2369       ldr        r3, [r4, #0x10]
0800410e: 0122       movs       r2, #1
08004110: 5a74       strb       r2, [r3, #0x11]
08004112: 06e6       b          #0x8003d22
08004114: d4f80cc0   ldr.w      ip, [r4, #0xc]
08004118: 9cf87433   ldrb.w     r3, [ip, #0x374]
0800411c: 1b06       lsls       r3, r3, #0x18
0800411e: fbd5       bpl        #0x8004118
08004120: 0023       movs       r3, #0
08004122: dce9d521   ldrd       r2, r1, [ip, #0x354]
08004126: cce9d512   strd       r1, r2, [ip, #0x354]
0800412a: 02eb8300   add.w      r0, r2, r3, lsl #2
0800412e: b0f803e0   ldrh.w     lr, [r0, #3]
08004132: 01eb8300   add.w      r0, r1, r3, lsl #2
08004136: 0133       adds       r3, #1
08004138: a0f803e0   strh.w     lr, [r0, #3]
0800413c: 102b       cmp        r3, #0x10
0800413e: f4d1       bne        #0x800412a
08004140: 0023       movs       r3, #0
08004142: 02eb8300   add.w      r0, r2, r3, lsl #2
08004146: b0f844e0   ldrh.w     lr, [r0, #0x44]
0800414a: 01eb8300   add.w      r0, r1, r3, lsl #2
0800414e: 0133       adds       r3, #1
08004150: a0f844e0   strh.w     lr, [r0, #0x44]
08004154: 102b       cmp        r3, #0x10
08004156: f4d1       bne        #0x8004142
08004158: ff21       movs       r1, #0xff
0800415a: 8cf87413   strb.w     r1, [ip, #0x374]
0800415e: 9cf87433   ldrb.w     r3, [ip, #0x374]
08004162: 0133       adds       r3, #1
08004164: 5bb2       sxtb       r3, r3
08004166: 8cf87433   strb.w     r3, [ip, #0x374]
0800416a: 9cf87433   ldrb.w     r3, [ip, #0x374]
0800416e: 5bb2       sxtb       r3, r3
08004170: 012b       cmp        r3, #1
08004172: 40f38c80   ble.w      #0x800428e
08004176: 8cf87413   strb.w     r1, [ip, #0x374]
0800417a: c4f80c92   str.w      sb, [r4, #0x20c]
0800417e: 07e7       b          #0x8003f90
08004180: 012b       cmp        r3, #1
08004182: 2a71       strb       r2, [r5, #4]
08004184: 727b       ldrb       r2, [r6, #0xd]
08004186: 40f0d380   bne.w      #0x8004330
0800418a: 02b9       cbnz       r2, #0x800418e
0800418c: b27c       ldrb       r2, [r6, #0x12]
0800418e: 6a71       strb       r2, [r5, #5]
08004190: b27b       ldrb       r2, [r6, #0xe]
08004192: 02b9       cbnz       r2, #0x8004196
08004194: f27c       ldrb       r2, [r6, #0x13]
08004196: aa71       strb       r2, [r5, #6]
08004198: f27b       ldrb       r2, [r6, #0xf]
0800419a: 0ce7       b          #0x8003fb6
0800419c: 94f83432   ldrb.w     r3, [r4, #0x234]
080041a0: cbb1       cbz        r3, #0x80041d6
080041a2: d4ed8c7a   vldr       s15, [r4, #0x230]
080041a6: f5ee006a   vmov.f32   s13, #2.500000e-01
080041aa: 94ed8b7a   vldr       s14, [r4, #0x22c]
080041ae: 94f83532   ldrb.w     r3, [r4, #0x235]
080041b2: f4eec77a   vcmpe.f32  s15, s14
080041b6: 77eec77a   vsub.f32   s15, s15, s14
080041ba: 03f00103   and        r3, r3, #1
080041be: f1ee10fa   vmrs       apsr_nzcv, fpscr
080041c2: f4eee67a   vcmpe.f32  s15, s13
080041c6: d8bf       it         le
080041c8: 0023       movle      r3, #0
080041ca: f1ee10fa   vmrs       apsr_nzcv, fpscr
080041ce: 03f00103   and        r3, r3, #1
080041d2: d8bf       it         le
080041d4: 0023       movle      r3, #0
080041d6: d4ed8f7a   vldr       s15, [r4, #0x23c]
080041da: 9fed187a   vldr       s14, [pc, #0x60] ; literal @0800423c = 0x3f59999a / float 0.85
080041de: f4eec77a   vcmpe.f32  s15, s14
080041e2: f1ee10fa   vmrs       apsr_nzcv, fpscr
080041e6: 75dd       ble        #0x80042d4
080041e8: 9fed157a   vldr       s14, [pc, #0x54] ; literal @08004240 = 0x3f933333 / float 1.15
080041ec: f4eec77a   vcmpe.f32  s15, s14
080041f0: f1ee10fa   vmrs       apsr_nzcv, fpscr
080041f4: 58bf       it         pl
080041f6: 0023       movpl      r3, #0
080041f8: d4ed8e7a   vldr       s15, [r4, #0x238]
080041fc: 9fed117a   vldr       s14, [pc, #0x44] ; literal @08004244 = 0xbe19999a / float -0.15
08004200: f4eec77a   vcmpe.f32  s15, s14
08004204: f1ee10fa   vmrs       apsr_nzcv, fpscr
08004208: 62dd       ble        #0x80042d0
0800420a: 9fed0f7a   vldr       s14, [pc, #0x3c] ; literal @08004248 = 0x3e19999a / float 0.15
0800420e: f4eec77a   vcmpe.f32  s15, s14
08004212: f1ee10fa   vmrs       apsr_nzcv, fpscr
08004216: 58bf       it         pl
08004218: 0023       movpl      r3, #0
0800421a: 84f81232   strb.w     r3, [r4, #0x212]
0800421e: e1e5       b          #0x8003de4
08004220: 4baa       add        r2, sp, #0x12c
08004222: 7f3f       subs       r7, #0x7f
08004224: 006a       ldr        r0, [r0, #0x20]
08004226: ab3a       subs       r2, #0xab
08004228: 3814       asrs       r0, r7, #0x10
0800422a: 0024       movs       r4, #0
0800422c: f80c       lsrs       r0, r7, #0x13
0800422e: 0024       movs       r4, #0
08004230: e809       lsrs       r0, r5, #7
08004232: 0024       movs       r4, #0
08004234: 3414       asrs       r4, r6, #0x10
08004236: 0024       movs       r4, #0
08004238: 0010       asrs       r0, r0, #0x20
0800423a: 0090       str        r0, [sp]
0800423c: 9a99       ldr        r1, [sp, #0x268]
0800423e: 593f       subs       r7, #0x59
08004240: 3333       adds       r3, #0x33
08004242: 933f       subs       r7, #0x93
08004244: 9a99       ldr        r1, [sp, #0x268]
08004246: 19be       bkpt       #0x19
08004248: 9a99       ldr        r1, [sp, #0x268]
0800424a: 193e       subs       r6, #0x19
0800424c: 0323       movs       r3, #3
0800424e: 0022       movs       r2, #0
08004250: 18ee900a   vmov       r0, s17
08004254: 8df81430   strb.w     r3, [sp, #0x14]
08004258: 0423       movs       r3, #4
0800425a: 0092       str        r2, [sp]
0800425c: 8df81530   strb.w     r3, [sp, #0x15]
08004260: 4ff60b73   movw       r3, #0xff0b
08004264: 0599       ldr        r1, [sp, #0x14]
08004266: 0b93       str        r3, [sp, #0x2c]
08004268: 0123       movs       r3, #1
0800426a: 0e92       str        r2, [sp, #0x38]
0800426c: cde90c22   strd       r2, r2, [sp, #0x30]
08004270: 03f062fb   bl         #0x8007938
08004274: 18ee900a   vmov       r0, s17
08004278: 03f06afb   bl         #0x8007950
0800427c: 0028       cmp        r0, #0
0800427e: 50d0       beq        #0x8004322
08004280: bd4b       ldr        r3, [pc, #0x2f4] ; literal @08004578 = 0x3bf5c28f / float 0.0075
08004282: caf80430   str.w      r3, [sl, #4]
08004286: bd4b       ldr        r3, [pc, #0x2f4] ; literal @0800457c = 0x3f7f5c29 / float 0.9975
08004288: caf80830   str.w      r3, [sl, #8]
0800428c: d2e6       b          #0x8004034
0800428e: 9cf87433   ldrb.w     r3, [ip, #0x374]
08004292: 0cf5547b   add.w      fp, ip, #0x350
08004296: 4cfa83fc   sxtab      ip, ip, r3
0800429a: 5bb2       sxtb       r3, r3
0800429c: 5846       mov        r0, fp
0800429e: 03eb8313   add.w      r3, r3, r3, lsl #6
080042a2: 9cf85c13   ldrb.w     r1, [ip, #0x35c]
080042a6: cdf804b0   str.w      fp, [sp, #4]
080042aa: 1a44       add        r2, r3
080042ac: b44b       ldr        r3, [pc, #0x2d0] ; literal @08004580 = 0x08000a61 / float 3.85308e-34
080042ae: 41f04001   orr        r1, r1, #0x40
080042b2: 0093       str        r3, [sp]
080042b4: 4123       movs       r3, #0x41
080042b6: 03f0cffe   bl         #0x8008058
080042ba: 0028       cmp        r0, #0
080042bc: 3ff45daf   beq.w      #0x800417a
080042c0: 5846       mov        r0, fp
080042c2: 03f0c3fe   bl         #0x800804c
080042c6: 0146       mov        r1, r0
080042c8: 5846       mov        r0, fp
080042ca: 03f0b3fe   bl         #0x8008034
080042ce: 54e7       b          #0x800417a
080042d0: 0023       movs       r3, #0
080042d2: a2e7       b          #0x800421a
080042d4: 0023       movs       r3, #0
080042d6: 8fe7       b          #0x80041f8
080042d8: b7ee007a   vmov.f32   s14, #1.000000e+00
080042dc: d3ed777a   vldr       s15, [r3, #0x1dc]
080042e0: dfeda86a   vldr       s13, [pc, #0x2a0] ; literal @08004584 = 0x00000000 / float 0
080042e4: c7fea67a   vmaxnm.f32 s15, s15, s13
080042e8: c7fec77a   vminnm.f32 s15, s15, s14
080042ec: 27eea77a   vmul.f32   s14, s15, s15
080042f0: 2369       ldr        r3, [r4, #0x10]
080042f2: c3ed1c7a   vstr       s15, [r3, #0x70]
080042f6: 83ed697a   vstr       s14, [r3, #0x1a4]
080042fa: 12e5       b          #0x8003d22
080042fc: dfeda26a   vldr       s13, [pc, #0x288] ; literal @08004588 = 0x3f8ccccd / float 1.1
08004300: b7ee007a   vmov.f32   s14, #1.000000e+00
08004304: 2369       ldr        r3, [r4, #0x10]
08004306: 67eea67a   vmul.f32   s15, s15, s13
0800430a: f4eec77a   vcmpe.f32  s15, s14
0800430e: f1ee10fa   vmrs       apsr_nzcv, fpscr
08004312: 64dd       ble        #0x80043de
08004314: 9fed9b7a   vldr       s14, [pc, #0x26c] ; literal @08004584 = 0x00000000 / float 0
08004318: 03f5da63   add.w      r3, r3, #0x6d0
0800431c: 83ed007a   vstr       s14, [r3]
08004320: ffe4       b          #0x8003d22
08004322: 9a4b       ldr        r3, [pc, #0x268] ; literal @0800458c = 0x3d23d70a / float 0.04
08004324: caf80430   str.w      r3, [sl, #4]
08004328: 994b       ldr        r3, [pc, #0x264] ; literal @08004590 = 0x3f75c28f / float 0.96
0800432a: caf80830   str.w      r3, [sl, #8]
0800432e: 81e6       b          #0x8004034
08004330: 6a71       strb       r2, [r5, #5]
08004332: b27b       ldrb       r2, [r6, #0xe]
08004334: 2fe7       b          #0x8004196
08004336: 94f82a32   ldrb.w     r3, [r4, #0x22a]
0800433a: 83f00103   eor        r3, r3, #1
0800433e: 84f82a32   strb.w     r3, [r4, #0x22a]
08004342: eee4       b          #0x8003d22
08004344: 83f00103   eor        r3, r3, #1
08004348: 84f82832   strb.w     r3, [r4, #0x228]
0800434c: e9e4       b          #0x8003d22
0800434e: 94f82932   ldrb.w     r3, [r4, #0x229]
08004352: 83f00103   eor        r3, r3, #1
08004356: 84f82932   strb.w     r3, [r4, #0x229]
0800435a: e2e4       b          #0x8003d22
0800435c: d3f80021   ldr.w      r2, [r3, #0x100]
08004360: 002a       cmp        r2, #0
08004362: 62d1       bne        #0x800442a
08004364: 0122       movs       r2, #1
08004366: c3f80021   str.w      r2, [r3, #0x100]
0800436a: dae4       b          #0x8003d22
0800436c: 94f83422   ldrb.w     r2, [r4, #0x234]
08004370: 002a       cmp        r2, #0
08004372: 5ed1       bne        #0x8004432
08004374: d3f8fc32   ldr.w      r3, [r3, #0x2fc]
08004378: c4f82c32   str.w      r3, [r4, #0x22c]
0800437c: 0123       movs       r3, #1
0800437e: 84f83432   strb.w     r3, [r4, #0x234]
08004382: 7ce5       b          #0x8003e7e
08004384: 2369       ldr        r3, [r4, #0x10]
08004386: 0022       movs       r2, #0
08004388: 5a74       strb       r2, [r3, #0x11]
0800438a: cae4       b          #0x8003d22
0800438c: 5a7b       ldrb       r2, [r3, #0xd]
0800438e: 002a       cmp        r2, #0
08004390: 40f0aa80   bne.w      #0x80044e8
08004394: 9a7c       ldrb       r2, [r3, #0x12]
08004396: 82f00102   eor        r2, r2, #1
0800439a: 5a73       strb       r2, [r3, #0xd]
0800439c: c1e4       b          #0x8003d22
0800439e: 9a7b       ldrb       r2, [r3, #0xe]
080043a0: 002a       cmp        r2, #0
080043a2: 40f0a480   bne.w      #0x80044ee
080043a6: da7c       ldrb       r2, [r3, #0x13]
080043a8: 82f00102   eor        r2, r2, #1
080043ac: 9a73       strb       r2, [r3, #0xe]
080043ae: b8e4       b          #0x8003d22
080043b0: 9fed788a   vldr       s16, [pc, #0x1e0] ; literal @08004594 = 0x3e4ccccd / float 0.2
080043b4: 03f5af6b   add.w      fp, r3, #0x578
080043b8: 03f5c469   add.w      sb, r3, #0x620
080043bc: f4eec87a   vcmpe.f32  s15, s16
080043c0: f1ee10fa   vmrs       apsr_nzcv, fpscr
080043c4: 58bf       it         pl
080043c6: b0ee678a   vmovpl.f32 s16, s15
080043ca: 8bed028a   vstr       s16, [fp, #8]
080043ce: 5846       mov        r0, fp
080043d0: 0bf1180b   add.w      fp, fp, #0x18
080043d4: fcf7eafb   bl         #0x8000bac
080043d8: cb45       cmp        fp, sb
080043da: f6d1       bne        #0x80043ca
080043dc: a1e4       b          #0x8003d22
080043de: b6ee007a   vmov.f32   s14, #5.000000e-01
080043e2: a7eec77a   vfms.f32   s14, s15, s14
080043e6: 97e7       b          #0x8004318
080043e8: 137a       ldrb       r3, [r2, #8]
080043ea: d2f88416   ldr.w      r1, [r2, #0x684]
080043ee: 0133       adds       r3, #1
080043f0: 0329       cmp        r1, #3
080043f2: 03f00103   and        r3, r3, #1
080043f6: 9360       str        r3, [r2, #8]
080043f8: 7ff793ac   ble.w      #0x8003d22
080043fc: 002b       cmp        r3, #0
080043fe: 3ff490ac   beq.w      #0x8003d22
08004402: 1fe5       b          #0x8003e44
08004404: 05f082f8   bl         #0x800950c
08004408: 4ff07e53   mov.w      r3, #0x3f800000
0800440c: c4f81802   str.w      r0, [r4, #0x218]
08004410: c4f83c32   str.w      r3, [r4, #0x23c]
08004414: 0123       movs       r3, #1
08004416: 84f81332   strb.w     r3, [r4, #0x213]
0800441a: 40f20113   movw       r3, #0x101
0800441e: a4f83432   strh.w     r3, [r4, #0x234]
08004422: 0023       movs       r3, #0
08004424: c4f83832   str.w      r3, [r4, #0x238]
08004428: 7be4       b          #0x8003d22
0800442a: 0022       movs       r2, #0
0800442c: c3f80021   str.w      r2, [r3, #0x100]
08004430: 77e4       b          #0x8003d22
08004432: 94f83522   ldrb.w     r2, [r4, #0x235]
08004436: 002a       cmp        r2, #0
08004438: 7ff421ad   bne.w      #0x8003e7e
0800443c: 93edbf7a   vldr       s14, [r3, #0x2fc]
08004440: 0123       movs       r3, #1
08004442: d4ed8b6a   vldr       s13, [r4, #0x22c]
08004446: dfed545a   vldr       s11, [pc, #0x150] ; literal @08004598 = 0x3ecccccd / float 0.4
0800444a: 37ee666a   vsub.f32   s12, s14, s13
0800444e: 84ed8c7a   vstr       s14, [r4, #0x230]
08004452: dfed507a   vldr       s15, [pc, #0x140] ; literal @08004594 = 0x3e4ccccd / float 0.2
08004456: 84f83532   strb.w     r3, [r4, #0x235]
0800445a: 85ee867a   vdiv.f32   s14, s11, s12
0800445e: e6eec77a   vfms.f32   s15, s13, s14
08004462: 84ed8f7a   vstr       s14, [r4, #0x23c]
08004466: c4ed8e7a   vstr       s15, [r4, #0x238]
0800446a: 08e5       b          #0x8003e7e
0800446c: 05f04ef8   bl         #0x800950c
08004470: 4ff0010c   mov.w      ip, #1
08004474: c4f81402   str.w      r0, [r4, #0x214]
08004478: 4ff07e52   mov.w      r2, #0x3f800000
0800447c: 0021       movs       r1, #0
0800447e: d4e90303   ldrd       r0, r3, [r4, #0xc]
08004482: c3f884c6   str.w      ip, [r3, #0x684]
08004486: 00f5af69   add.w      sb, r0, #0x578
0800448a: c3f8d4c2   str.w      ip, [r3, #0x2d4]
0800448e: 00f5c46b   add.w      fp, r0, #0x620
08004492: dff80cc1   ldr.w      ip, [pc, #0x10c] ; literal @080045a0 = 0x3e10d0c3 / float 0.141421
08004496: c3f83c21   str.w      r2, [r3, #0x13c]
0800449a: c3f870c0   str.w      ip, [r3, #0x70]
0800449e: dff804c1   ldr.w      ip, [pc, #0x104] ; literal @080045a4 = 0x3ca3d70b / float 0.02
080044a2: da63       str        r2, [r3, #0x3c]
080044a4: 1a64       str        r2, [r3, #0x40]
080044a6: 5a64       str        r2, [r3, #0x44]
080044a8: 0022       movs       r2, #0
080044aa: c3f8a4c1   str.w      ip, [r3, #0x1a4]
080044ae: 03f5da6c   add.w      ip, r3, #0x6d0
080044b2: c3f80021   str.w      r2, [r3, #0x100]
080044b6: c3f82011   str.w      r1, [r3, #0x120]
080044ba: c3f85811   str.w      r1, [r3, #0x158]
080044be: ccf80010   str.w      r1, [ip]
080044c2: da60       str        r2, [r3, #0xc]
080044c4: 1a82       strh       r2, [r3, #0x10]
080044c6: c3e90122   strd       r2, r2, [r3, #4]
080044ca: a4f82822   strh.w     r2, [r4, #0x228]
080044ce: 84f82a22   strb.w     r2, [r4, #0x22a]
080044d2: 324b       ldr        r3, [pc, #0xc8] ; literal @0800459c = 0x3f333333 / float 0.7
080044d4: 4846       mov        r0, sb
080044d6: 09f11809   add.w      sb, sb, #0x18
080044da: 49f8103c   str        r3, [sb, #-0x10]
080044de: fcf765fb   bl         #0x8000bac
080044e2: cb45       cmp        fp, sb
080044e4: f5d1       bne        #0x80044d2
080044e6: 1ce4       b          #0x8003d22
080044e8: 0022       movs       r2, #0
080044ea: 5a73       strb       r2, [r3, #0xd]
080044ec: 19e4       b          #0x8003d22
080044ee: 0022       movs       r2, #0
080044f0: 9a73       strb       r2, [r3, #0xe]
080044f2: 16e4       b          #0x8003d22
080044f4: 0323       movs       r3, #3
080044f6: 0ba8       add        r0, sp, #0x2c
080044f8: 0022       movs       r2, #0
080044fa: 8df81430   strb.w     r3, [sp, #0x14]
080044fe: 0423       movs       r3, #4
08004500: 08ee900a   vmov       s17, r0
08004504: 0092       str        r2, [sp]
08004506: 8df81530   strb.w     r3, [sp, #0x15]
0800450a: 0523       movs       r3, #5
0800450c: 0599       ldr        r1, [sp, #0x14]
0800450e: 8af80030   strb.w     r3, [sl]
08004512: 4ff60b73   movw       r3, #0xff0b
08004516: 0e92       str        r2, [sp, #0x38]
08004518: 0b93       str        r3, [sp, #0x2c]
0800451a: 0123       movs       r3, #1
0800451c: cde90c22   strd       r2, r2, [sp, #0x30]
08004520: 03f00afa   bl         #0x8007938
08004524: 18ee900a   vmov       r0, s17
08004528: 03f012fa   bl         #0x8007950
0800452c: d8b9       cbnz       r0, #0x8004566
0800452e: 174b       ldr        r3, [pc, #0x5c] ; literal @0800458c = 0x3d23d70a / float 0.04
08004530: caf80430   str.w      r3, [sl, #4]
08004534: 164b       ldr        r3, [pc, #0x58] ; literal @08004590 = 0x3f75c28f / float 0.96
08004536: caf80830   str.w      r3, [sl, #8]
0800453a: fff77dbb   b.w        #0x8003c38
0800453e: d4f80c80   ldr.w      r8, [r4, #0xc]
08004542: 08f1c400   add.w      r0, r8, #0xc4
08004546: 03f003fa   bl         #0x8007950
0800454a: 98f8d930   ldrb.w     r3, [r8, #0xd9]
0800454e: 13b1       cbz        r3, #0x8004556
08004550: 80f00100   eor        r0, r0, #1
08004554: c0b2       uxtb       r0, r0
08004556: 0028       cmp        r0, #0
08004558: 3ff491aa   beq.w      #0x8003a7e
0800455c: 0423       movs       r3, #4
0800455e: 84f8f831   strb.w     r3, [r4, #0x1f8]
08004562: fff78cba   b.w        #0x8003a7e
08004566: 044b       ldr        r3, [pc, #0x10] ; literal @08004578 = 0x3bf5c28f / float 0.0075
08004568: caf80430   str.w      r3, [sl, #4]
0800456c: 034b       ldr        r3, [pc, #0xc] ; literal @0800457c = 0x3f7f5c29 / float 0.9975
0800456e: caf80830   str.w      r3, [sl, #8]
08004572: fff761bb   b.w        #0x8003c38
08004576: 00bf       nop        
08004578: 8fc2       stm        r2!, {r0, r1, r2, r3, r7}
0800457a: f53b       subs       r3, #0xf5
0800457c: 295c       ldrb       r1, [r5, r0]
0800457e: 7f3f       subs       r7, #0x7f
08004580: 610a       lsrs       r1, r4, #9
08004582: 0008       lsrs       r0, r0, #0x20
08004584: 0000       movs       r0, r0
08004586: 0000       movs       r0, r0
08004588: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
0800458a: 8c3f       subs       r7, #0x8c
0800458c: 0ad7       bvc        #0x80045a4
0800458e: 233d       subs       r5, #0x23
08004590: 8fc2       stm        r2!, {r0, r1, r2, r3, r7}
08004592: 753f       subs       r7, #0x75
08004594: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08004596: 4c3e       subs       r6, #0x4c
08004598: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
0800459a: cc3e       subs       r6, #0xcc
0800459c: 3333       adds       r3, #0x33
0800459e: 333f       subs       r7, #0x33
080045a0: c3d0       beq        #0x800452a
080045a2: 103e       subs       r6, #0x10
080045a4: 0bd7       bvc        #0x80045be
080045a6: a33c       subs       r4, #0xa3
