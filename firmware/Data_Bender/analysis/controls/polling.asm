080023a0: 2de9f041   push.w     {r4, r5, r6, r7, r8, lr}
080023a4: 0446       mov        r4, r0
080023a6: 2ded028b   vpush      {d8}
080023aa: a2b0       sub        sp, #0x88
080023ac: 07f0aef8   bl         #0x800950c
080023b0: d4f80c80   ldr.w      r8, [r4, #0xc]
080023b4: 0746       mov        r7, r0
080023b6: 08f5e876   add.w      r6, r8, #0x1d0
080023ba: 08f52475   add.w      r5, r8, #0x290
080023be: 3046       mov        r0, r6
080023c0: 2036       adds       r6, #0x20
080023c2: 04f059fe   bl         #0x8007078
080023c6: b542       cmp        r5, r6
080023c8: f9d1       bne        #0x80023be
080023ca: 08f55478   add.w      r8, r8, #0x350
080023ce: 2846       mov        r0, r5
080023d0: 2035       adds       r5, #0x20
080023d2: 04f051fe   bl         #0x8007078
080023d6: a845       cmp        r8, r5
080023d8: f9d1       bne        #0x80023ce
080023da: 4ff07408   mov.w      r8, #0x74
080023de: 0025       movs       r5, #0
080023e0: 18e0       b          #0x8002414
080023e2: 0db9       cbnz       r5, #0x80023e8
080023e4: c4f80072   str.w      r7, [r4, #0x200]
080023e8: d4f8f411   ldr.w      r1, [r4, #0x1f4]
080023ec: 4b1c       adds       r3, r1, #1
080023ee: 02ebc106   add.w      r6, r2, r1, lsl #3
080023f2: 02f831c0   strb.w     ip, [r2, r1, lsl #3]
080023f6: 03f01f03   and        r3, r3, #0x1f
080023fa: 7580       strh       r5, [r6, #2]
080023fc: 7060       str        r0, [r6, #4]
080023fe: c4f8f431   str.w      r3, [r4, #0x1f4]
08002402: 07f083f8   bl         #0x800950c
08002406: c4f8ec00   str.w      r0, [r4, #0xec]
0800240a: 0135       adds       r5, #1
0800240c: 08f12408   add.w      r8, r8, #0x24
08002410: 072d       cmp        r5, #7
08002412: 35d0       beq        #0x8002480
08002414: e668       ldr        r6, [r4, #0xc]
08002416: 06eb0800   add.w      r0, r6, r8
0800241a: 04f08ffe   bl         #0x800713c
0800241e: 05ebc503   add.w      r3, r5, r5, lsl #3
08002422: 06eb8306   add.w      r6, r6, r3, lsl #2
08002426: 96f87830   ldrb.w     r3, [r6, #0x78]
0800242a: 002b       cmp        r3, #0
0800242c: edd0       beq        #0x800240a
0800242e: 96f89030   ldrb.w     r3, [r6, #0x90]
08002432: 04f1f002   add.w      r2, r4, #0xf0
08002436: 4ff0040c   mov.w      ip, #4
0800243a: 0020       movs       r0, #0
0800243c: 7f2b       cmp        r3, #0x7f
0800243e: d0d0       beq        #0x80023e2
08002440: 802b       cmp        r3, #0x80
08002442: e2d1       bne        #0x800240a
08002444: 0db9       cbnz       r5, #0x800244a
08002446: c4f80072   str.w      r7, [r4, #0x200]
0800244a: d4f8f431   ldr.w      r3, [r4, #0x1f4]
0800244e: 04f1f002   add.w      r2, r4, #0xf0
08002452: 0420       movs       r0, #4
08002454: 4ff07e51   mov.w      r1, #0x3f800000
08002458: 08f12408   add.w      r8, r8, #0x24
0800245c: 02f83300   strb.w     r0, [r2, r3, lsl #3]
08002460: 02ebc302   add.w      r2, r2, r3, lsl #3
08002464: 0133       adds       r3, #1
08002466: 5580       strh       r5, [r2, #2]
08002468: 0135       adds       r5, #1
0800246a: 03f01f03   and        r3, r3, #0x1f
0800246e: 5160       str        r1, [r2, #4]
08002470: c4f8f431   str.w      r3, [r4, #0x1f4]
08002474: 07f04af8   bl         #0x800950c
08002478: 072d       cmp        r5, #7
0800247a: c4f8ec00   str.w      r0, [r4, #0xec]
0800247e: c9d1       bne        #0x8002414
08002480: e068       ldr        r0, [r4, #0xc]
08002482: 2646       mov        r6, r4
08002484: 04f18408   add.w      r8, r4, #0x84
08002488: 0025       movs       r5, #0
0800248a: 00eb4512   add.w      r2, r0, r5, lsl #5
0800248e: dfed737a   vldr       s15, [pc, #0x1cc] ; literal @0800265c = 0x3f866666 / float 1.05
08002492: 032d       cmp        r5, #3
08002494: 96ed295a   vldr       s10, [r6, #0xa4]
08002498: d2ed776a   vldr       s13, [r2, #0x1dc]
0800249c: 05ebc503   add.w      r3, r5, r5, lsl #3
080024a0: 96ed277a   vldr       s14, [r6, #0x9c]
080024a4: 06f10c06   add.w      r6, r6, #0xc
080024a8: 18bf       it         ne
080024aa: 66eea76a   vmulne.f32 s13, s13, s15
080024ae: d6ed257a   vldr       s15, [r6, #0x94]
080024b2: 00eb8303   add.w      r3, r0, r3, lsl #2
080024b6: 9fed6a6a   vldr       s12, [pc, #0x1a8] ; literal @08002660 = 0x3b03126f / float 0.002
080024ba: 67ee857a   vmul.f32   s15, s15, s10
080024be: dfed695a   vldr       s11, [pc, #0x1a4] ; literal @08002664 = 0x3ba3d70a / float 0.005
080024c2: e6ee877a   vfma.f32   s15, s13, s14
080024c6: 36eee77a   vsub.f32   s14, s13, s15
080024ca: c6ed267a   vstr       s15, [r6, #0x98]
080024ce: a8ec017a   vstmia     r8!, {s14}
080024d2: b0eec77a   vabs.f32   s14, s14
080024d6: 93f89030   ldrb.w     r3, [r3, #0x90]
080024da: ff2b       cmp        r3, #0xff
080024dc: 46fe257a   vseleq.f32 s15, s12, s11
080024e0: b4eee77a   vcmpe.f32  s14, s15
080024e4: f1ee10fa   vmrs       apsr_nzcv, fpscr
080024e8: 04dd       ble        #0x80024f4
080024ea: d4f80032   ldr.w      r3, [r4, #0x200]
080024ee: fb1a       subs       r3, r7, r3
080024f0: c82b       cmp        r3, #0xc8
080024f2: 68d8       bhi        #0x80025c6
080024f4: 0135       adds       r5, #1
080024f6: 062d       cmp        r5, #6
080024f8: c7d1       bne        #0x800248a
080024fa: d0f8bc22   ldr.w      r2, [r0, #0x2bc]
080024fe: 00f5dc70   add.w      r0, r0, #0x1b8
08002502: 2369       ldr        r3, [r4, #0x10]
08002504: 5a66       str        r2, [r3, #0x64]
08002506: d0f82421   ldr.w      r2, [r0, #0x124]
0800250a: 9a66       str        r2, [r3, #0x68]
0800250c: d0f84421   ldr.w      r2, [r0, #0x144]
08002510: 5a65       str        r2, [r3, #0x54]
08002512: d0f86421   ldr.w      r2, [r0, #0x164]
08002516: 9a65       str        r2, [r3, #0x58]
08002518: d0f88421   ldr.w      r2, [r0, #0x184]
0800251c: da65       str        r2, [r3, #0x5c]
0800251e: d0f8e420   ldr.w      r2, [r0, #0xe4]
08002522: 6369       ldr        r3, [r4, #0x14]
08002524: da63       str        r2, [r3, #0x3c]
08002526: 04f0ddfd   bl         #0x80070e4
0800252a: 0028       cmp        r0, #0
0800252c: 40f0b881   bne.w      #0x80028a0
08002530: 2669       ldr        r6, [r4, #0x10]
08002532: 94f82a52   ldrb.w     r5, [r4, #0x22a]
08002536: e068       ldr        r0, [r4, #0xc]
08002538: 002d       cmp        r5, #0
0800253a: 5bd0       beq        #0x80025f4
0800253c: 90f89e31   ldrb.w     r3, [r0, #0x19e]
08002540: 00f5c470   add.w      r0, r0, #0x188
08002544: 002b       cmp        r3, #0
08002546: 40f0ca81   bne.w      #0x80028de
0800254a: 05f001fa   bl         #0x8007950
0800254e: d4e90335   ldrd       r3, r5, [r4, #0xc]
08002552: b074       strb       r0, [r6, #0x12]
08002554: 93f88621   ldrb.w     r2, [r3, #0x186]
08002558: 03f5b870   add.w      r0, r3, #0x170
0800255c: 002a       cmp        r2, #0
0800255e: 00f0cd81   beq.w      #0x80028fc
08002562: 05f0f5f9   bl         #0x8007950
08002566: 80f00100   eor        r0, r0, #1
0800256a: d4e90336   ldrd       r3, r6, [r4, #0xc]
0800256e: c0b2       uxtb       r0, r0
08002570: 93f8b621   ldrb.w     r2, [r3, #0x1b6]
08002574: 6875       strb       r0, [r5, #0x15]
08002576: 03f5d070   add.w      r0, r3, #0x1a0
0800257a: 002a       cmp        r2, #0
0800257c: 00f0ca81   beq.w      #0x8002914
08002580: 05f0e6f9   bl         #0x8007950
08002584: 80f00100   eor        r0, r0, #1
08002588: c0b2       uxtb       r0, r0
0800258a: 2269       ldr        r2, [r4, #0x10]
0800258c: f074       strb       r0, [r6, #0x13]
0800258e: 5368       ldr        r3, [r2, #4]
08002590: 2bb1       cbz        r3, #0x800259e
08002592: d2eda17a   vldr       s15, [r2, #0x284]
08002596: fceee77a   vcvt.u32.f32 s15, s15
0800259a: 17ee903a   vmov       r3, s15
0800259e: d4f81c22   ldr.w      r2, [r4, #0x21c]
080025a2: 94f8f851   ldrb.w     r5, [r4, #0x1f8]
080025a6: 9a42       cmp        r2, r3
080025a8: c4f81c32   str.w      r3, [r4, #0x21c]
080025ac: 18bf       it         ne
080025ae: c4f82072   strne.w    r7, [r4, #0x220]
080025b2: 042d       cmp        r5, #4
080025b4: 00f26f81   bhi.w      #0x8002896
080025b8: dfe815f0   tbh        [pc, r5, lsl #1]
080025bc: e202       lsls       r2, r4, #0xb
080025be: 5e00       lsls       r6, r3, #1
080025c0: f301       lsls       r3, r6, #7
080025c2: 4203       lsls       r2, r0, #0xd
080025c4: af01       lsls       r7, r5, #6
080025c6: d4f8f431   ldr.w      r3, [r4, #0x1f4]
080025ca: 04f1f002   add.w      r2, r4, #0xf0
080025ce: 0021       movs       r1, #0
080025d0: 02f83310   strb.w     r1, [r2, r3, lsl #3]
080025d4: 02ebc302   add.w      r2, r2, r3, lsl #3
080025d8: 0133       adds       r3, #1
080025da: 5580       strh       r5, [r2, #2]
080025dc: 03f01f03   and        r3, r3, #0x1f
080025e0: c2ed016a   vstr       s13, [r2, #4]
080025e4: c4f8f431   str.w      r3, [r4, #0x1f4]
080025e8: 06f090ff   bl         #0x800950c
080025ec: c4f8ec00   str.w      r0, [r4, #0xec]
080025f0: e068       ldr        r0, [r4, #0xc]
080025f2: 7fe7       b          #0x80024f4
080025f4: 00f5c470   add.w      r0, r0, #0x188
080025f8: 7582       strh       r5, [r6, #0x12]
080025fa: b582       strh       r5, [r6, #0x14]
080025fc: 04f072fd   bl         #0x80070e4
08002600: 60b1       cbz        r0, #0x800261c
08002602: 2369       ldr        r3, [r4, #0x10]
08002604: 5a68       ldr        r2, [r3, #4]
08002606: 002a       cmp        r2, #0
08002608: 40f01884   bne.w      #0x8002e3c
0800260c: 1a7b       ldrb       r2, [r3, #0xc]
0800260e: 002a       cmp        r2, #0
08002610: 40f02685   bne.w      #0x8003060
08002614: 9a7c       ldrb       r2, [r3, #0x12]
08002616: 82f00102   eor        r2, r2, #1
0800261a: 1a73       strb       r2, [r3, #0xc]
0800261c: e068       ldr        r0, [r4, #0xc]
0800261e: 00f5d070   add.w      r0, r0, #0x1a0
08002622: 04f05ffd   bl         #0x80070e4
08002626: 50b1       cbz        r0, #0x800263e
08002628: 2369       ldr        r3, [r4, #0x10]
0800262a: 5a68       ldr        r2, [r3, #4]
0800262c: 002a       cmp        r2, #0
0800262e: 40f01784   bne.w      #0x8002e60
08002632: d97b       ldrb       r1, [r3, #0xf]
08002634: 11b9       cbnz       r1, #0x800263c
08002636: da7c       ldrb       r2, [r3, #0x13]
08002638: 82f00102   eor        r2, r2, #1
0800263c: da73       strb       r2, [r3, #0xf]
0800263e: e068       ldr        r0, [r4, #0xc]
08002640: 00f5b870   add.w      r0, r0, #0x170
08002644: 04f04efd   bl         #0x80070e4
08002648: 2269       ldr        r2, [r4, #0x10]
0800264a: 0028       cmp        r0, #0
0800264c: 9fd0       beq        #0x800258e
0800264e: 537c       ldrb       r3, [r2, #0x11]
08002650: 002b       cmp        r3, #0
08002652: 00f0ed83   beq.w      #0x8002e30
08002656: 5574       strb       r5, [r2, #0x11]
08002658: 99e7       b          #0x800258e
0800265a: 00bf       nop        
0800265c: 6666       str        r6, [r4, #0x64]
0800265e: 863f       subs       r7, #0x86
08002660: 6f12       asrs       r7, r5, #9
08002662: 033b       subs       r3, #3
08002664: 0ad7       bvc        #0x800267c
08002666: a33b       subs       r3, #0xa3
08002668: 0080       strh       r0, [r0]
0800266a: ff43       mvns       r7, r7
0800266c: 0a4c       ldr        r4, [pc, #0x28] ; literal @08002698 = 0x23007ac7 / float 6.96489e-18
0800266e: 1f3e       subs       r6, #0x1f
08002670: 7c55       strb       r4, [r7, r5]
08002672: 023e       subs       r6, #2
08002674: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002676: cc3e       subs       r6, #0xcc
08002678: 06f048ff   bl         #0x800950c
0800267c: c0f30800   ubfx       r0, r0, #0, #9
08002680: 5fed075a   vldr       s11, [pc, #-0x1c] ; literal @08002668 = 0x43ff8000 / float 511
08002684: f0ee006a   vmov.f32   s13, #2.000000e+00
08002688: 07ee100a   vmov       s14, r0
0800268c: ffee007a   vmov.f32   s15, #-1.000000e+00
08002690: 2269       ldr        r2, [r4, #0x10]
08002692: b7ee008a   vmov.f32   s16, #1.000000e+00
08002696: b8eec77a   vcvt.f32.s32 s14, s14
0800269a: 0023       movs       r3, #0
0800269c: 92ed1c1a   vldr       s2, [r2, #0x70]
080026a0: 0a93       str        r3, [sp, #0x28]
080026a2: 87ee256a   vdiv.f32   s12, s14, s11
080026a6: 0b93       str        r3, [sp, #0x2c]
080026a8: 0c93       str        r3, [sp, #0x30]
080026aa: b5ee401a   vcmp.f32   s2, #0
080026ae: f1ee10fa   vmrs       apsr_nzcv, fpscr
080026b2: e6ee267a   vfma.f32   s15, s12, s13
080026b6: f0eee77a   vabs.f32   s15, s15
080026ba: 38ee678a   vsub.f32   s16, s16, s15
080026be: 00f00785   beq.w      #0x80030d0
080026c2: 5fed167a   vldr       s15, [pc, #-0x58] ; literal @0800266c = 0x3e1f4c0a / float 0.155564
080026c6: b4eee71a   vcmpe.f32  s2, s15
080026ca: f1ee10fa   vmrs       apsr_nzcv, fpscr
080026ce: 07d5       bpl        #0x80026e0
080026d0: 5fed197a   vldr       s15, [pc, #-0x64] ; literal @08002670 = 0x3e02557c / float 0.127279
080026d4: b4eee71a   vcmpe.f32  s2, s15
080026d8: f1ee10fa   vmrs       apsr_nzcv, fpscr
080026dc: 00f39f85   bgt.w      #0x800321e
080026e0: 0df12808   add.w      r8, sp, #0x28
080026e4: f0ee410a   vmov.f32   s1, s2
080026e8: b0ee410a   vmov.f32   s0, s2
080026ec: 4046       mov        r0, r8
080026ee: 07f04ffa   bl         #0x8009b90
080026f2: e068       ldr        r0, [r4, #0xc]
080026f4: 0a9b       ldr        r3, [sp, #0x28]
080026f6: 00f5af60   add.w      r0, r0, #0x578
080026fa: c360       str        r3, [r0, #0xc]
080026fc: 0b9b       ldr        r3, [sp, #0x2c]
080026fe: 0361       str        r3, [r0, #0x10]
08002700: 0c9b       ldr        r3, [sp, #0x30]
08002702: 4361       str        r3, [r0, #0x14]
08002704: fef752fa   bl         #0x8000bac
08002708: 94f82832   ldrb.w     r3, [r4, #0x228]
0800270c: 4046       mov        r0, r8
0800270e: 002b       cmp        r3, #0
08002710: 14bf       ite        ne
08002712: 2946       movne      r1, r5
08002714: 0221       moveq      r1, #2
08002716: 07f02bfa   bl         #0x8009b70
0800271a: 9ded0b7a   vldr       s14, [sp, #0x2c]
0800271e: dded0c7a   vldr       s15, [sp, #0x30]
08002722: dded0a6a   vldr       s13, [sp, #0x28]
08002726: 28ee077a   vmul.f32   s14, s16, s14
0800272a: e068       ldr        r0, [r4, #0xc]
0800272c: 68ee277a   vmul.f32   s15, s16, s15
08002730: 66ee886a   vmul.f32   s13, s13, s16
08002734: 00f5c160   add.w      r0, r0, #0x608
08002738: c0ed036a   vstr       s13, [r0, #0xc]
0800273c: 80ed047a   vstr       s14, [r0, #0x10]
08002740: c0ed057a   vstr       s15, [r0, #0x14]
08002744: fef732fa   bl         #0x8000bac
08002748: 2369       ldr        r3, [r4, #0x10]
0800274a: 4046       mov        r0, r8
0800274c: d3f80031   ldr.w      r3, [r3, #0x100]
08002750: 002b       cmp        r3, #0
08002752: 14bf       ite        ne
08002754: 2946       movne      r1, r5
08002756: 0221       moveq      r1, #2
08002758: 07f00afa   bl         #0x8009b70
0800275c: 2369       ldr        r3, [r4, #0x10]
0800275e: 1fed3b6a   vldr       s12, [pc, #-0xec] ; literal @08002674 = 0x3ecccccd / float 0.4
08002762: d3ed0f8a   vldr       s17, [r3, #0x3c]
08002766: dded0b6a   vldr       s13, [sp, #0x2c]
0800276a: 78ee868a   vadd.f32   s17, s17, s12
0800276e: 9ded0c7a   vldr       s14, [sp, #0x30]
08002772: dded0a7a   vldr       s15, [sp, #0x28]
08002776: e068       ldr        r0, [r4, #0xc]
08002778: 68ee888a   vmul.f32   s17, s17, s16
0800277c: 00f5b860   add.w      r0, r0, #0x5c0
08002780: 68eea66a   vmul.f32   s13, s17, s13
08002784: 28ee877a   vmul.f32   s14, s17, s14
08002788: 67eea87a   vmul.f32   s15, s15, s17
0800278c: c0ed046a   vstr       s13, [r0, #0x10]
08002790: 80ed057a   vstr       s14, [r0, #0x14]
08002794: c0ed037a   vstr       s15, [r0, #0xc]
08002798: fef708fa   bl         #0x8000bac
0800279c: 2369       ldr        r3, [r4, #0x10]
0800279e: 9b68       ldr        r3, [r3, #8]
080027a0: 002b       cmp        r3, #0
080027a2: 40f04e84   bne.w      #0x8003042
080027a6: d4ed076a   vldr       s13, [r4, #0x1c]
080027aa: d4ed087a   vldr       s15, [r4, #0x20]
080027ae: 94ed067a   vldr       s14, [r4, #0x18]
080027b2: e068       ldr        r0, [r4, #0xc]
080027b4: 68ee266a   vmul.f32   s13, s16, s13
080027b8: 68ee277a   vmul.f32   s15, s16, s15
080027bc: 27ee087a   vmul.f32   s14, s14, s16
080027c0: 00f5b560   add.w      r0, r0, #0x5a8
080027c4: c0ed046a   vstr       s13, [r0, #0x10]
080027c8: 80ed037a   vstr       s14, [r0, #0xc]
080027cc: c0ed057a   vstr       s15, [r0, #0x14]
080027d0: fef7ecf9   bl         #0x8000bac
080027d4: 94f82932   ldrb.w     r3, [r4, #0x229]
080027d8: 002b       cmp        r3, #0
080027da: 40f01984   bne.w      #0x8003010
080027de: 2369       ldr        r3, [r4, #0x10]
080027e0: 5fed5c5a   vldr       s11, [pc, #-0x170] ; literal @08002674 = 0x3ecccccd / float 0.4
080027e4: d3ed117a   vldr       s15, [r3, #0x44]
080027e8: 94ed076a   vldr       s12, [r4, #0x1c]
080027ec: 77eea57a   vadd.f32   s15, s15, s11
080027f0: d4ed086a   vldr       s13, [r4, #0x20]
080027f4: 94ed067a   vldr       s14, [r4, #0x18]
080027f8: e068       ldr        r0, [r4, #0xc]
080027fa: 67ee887a   vmul.f32   s15, s15, s16
080027fe: 00f5be60   add.w      r0, r0, #0x5f0
08002802: 27ee866a   vmul.f32   s12, s15, s12
08002806: 67eea66a   vmul.f32   s13, s15, s13
0800280a: 67ee277a   vmul.f32   s15, s14, s15
0800280e: 80ed046a   vstr       s12, [r0, #0x10]
08002812: c0ed056a   vstr       s13, [r0, #0x14]
08002816: c0ed037a   vstr       s15, [r0, #0xc]
0800281a: fef7c7f9   bl         #0x8000bac
0800281e: d4f81432   ldr.w      r3, [r4, #0x214]
08002822: a268       ldr        r2, [r4, #8]
08002824: ff1a       subs       r7, r7, r3
08002826: 9742       cmp        r7, r2
08002828: 80f0dd83   bhs.w      #0x8002fe6
0800282c: c94b       ldr        r3, [pc, #0x324] ; literal @08002b54 = 0x621b97c3 / float 7.17545e+20
0800282e: a722       movs       r2, #0xa7
08002830: e068       ldr        r0, [r4, #0xc]
08002832: a3fb0713   umull      r1, r3, r3, r7
08002836: 00f5bb60   add.w      r0, r0, #0x5d8
0800283a: 9b09       lsrs       r3, r3, #6
0800283c: 02fb1377   mls        r7, r2, r3, r7
08002840: 0023       movs       r3, #0
08002842: 582f       cmp        r7, #0x58
08002844: c360       str        r3, [r0, #0xc]
08002846: 0361       str        r3, [r0, #0x10]
08002848: 94bf       ite        ls
0800284a: 0027       movls      r7, #0
0800284c: 0127       movhi      r7, #1
0800284e: 07ee907a   vmov       s15, r7
08002852: f8ee677a   vcvt.f32.u32 s15, s15
08002856: c0ed057a   vstr       s15, [r0, #0x14]
0800285a: fef7a7f9   bl         #0x8000bac
0800285e: 94f82a32   ldrb.w     r3, [r4, #0x22a]
08002862: 002b       cmp        r3, #0
08002864: 40f0b883   bne.w      #0x8002fd8
08002868: d4ed076a   vldr       s13, [r4, #0x1c]
0800286c: d4ed087a   vldr       s15, [r4, #0x20]
08002870: 94ed067a   vldr       s14, [r4, #0x18]
08002874: e068       ldr        r0, [r4, #0xc]
08002876: 68ee266a   vmul.f32   s13, s16, s13
0800287a: 68ee277a   vmul.f32   s15, s16, s15
0800287e: 27ee088a   vmul.f32   s16, s14, s16
08002882: 00f5b260   add.w      r0, r0, #0x590
08002886: c0ed046a   vstr       s13, [r0, #0x10]
0800288a: 80ed038a   vstr       s16, [r0, #0xc]
0800288e: c0ed057a   vstr       s15, [r0, #0x14]
08002892: fef78bf9   bl         #0x8000bac
08002896: 22b0       add        sp, #0x88
08002898: bdec028b   vpop       {d8}
0800289c: bde8f081   pop.w      {r4, r5, r6, r7, r8, pc}
080028a0: 94f82932   ldrb.w     r3, [r4, #0x229]
080028a4: 43b1       cbz        r3, #0x80028b8
080028a6: 6369       ldr        r3, [r4, #0x14]
080028a8: 1a68       ldr        r2, [r3]
080028aa: 012a       cmp        r2, #1
080028ac: 00f0e282   beq.w      #0x8002e74
080028b0: 0022       movs       r2, #0
080028b2: 2669       ldr        r6, [r4, #0x10]
080028b4: da60       str        r2, [r3, #0xc]
080028b6: 3ce6       b          #0x8002532
080028b8: 2669       ldr        r6, [r4, #0x10]
080028ba: b268       ldr        r2, [r6, #8]
080028bc: d6f88436   ldr.w      r3, [r6, #0x684]
080028c0: 012a       cmp        r2, #1
080028c2: 03f10103   add.w      r3, r3, #1
080028c6: 0cbf       ite        eq
080028c8: 0422       moveq      r2, #4
080028ca: 0622       movne      r2, #6
080028cc: 93fbf2f1   sdiv       r1, r3, r2
080028d0: 02fb1133   mls        r3, r2, r1, r3
080028d4: 03b9       cbnz       r3, #0x80028d8
080028d6: 0123       movs       r3, #1
080028d8: c6f88436   str.w      r3, [r6, #0x684]
080028dc: 29e6       b          #0x8002532
080028de: 05f037f8   bl         #0x8007950
080028e2: 80f00100   eor        r0, r0, #1
080028e6: d4e90335   ldrd       r3, r5, [r4, #0xc]
080028ea: c0b2       uxtb       r0, r0
080028ec: 93f88621   ldrb.w     r2, [r3, #0x186]
080028f0: b074       strb       r0, [r6, #0x12]
080028f2: 03f5b870   add.w      r0, r3, #0x170
080028f6: 002a       cmp        r2, #0
080028f8: 7ff433ae   bne.w      #0x8002562
080028fc: 05f028f8   bl         #0x8007950
08002900: d4e90336   ldrd       r3, r6, [r4, #0xc]
08002904: 6875       strb       r0, [r5, #0x15]
08002906: 93f8b621   ldrb.w     r2, [r3, #0x1b6]
0800290a: 03f5d070   add.w      r0, r3, #0x1a0
0800290e: 002a       cmp        r2, #0
08002910: 7ff436ae   bne.w      #0x8002580
08002914: 05f01cf8   bl         #0x8007950
08002918: 37e6       b          #0x800258a
0800291a: 94f83532   ldrb.w     r3, [r4, #0x235]
0800291e: 002b       cmp        r3, #0
08002920: 40f0ca83   bne.w      #0x80030b8
08002924: 94f83432   ldrb.w     r3, [r4, #0x234]
08002928: e068       ldr        r0, [r4, #0xc]
0800292a: 002b       cmp        r3, #0
0800292c: 00f0b983   beq.w      #0x80030a2
08002930: 00f5b860   add.w      r0, r0, #0x5c0
08002934: 4ff07e53   mov.w      r3, #0x3f800000
08002938: 0022       movs       r2, #0
0800293a: c360       str        r3, [r0, #0xc]
0800293c: 0361       str        r3, [r0, #0x10]
0800293e: 4261       str        r2, [r0, #0x14]
08002940: fef734f9   bl         #0x8000bac
08002944: e068       ldr        r0, [r4, #0xc]
08002946: 4ff07e53   mov.w      r3, #0x3f800000
0800294a: 00f5bb60   add.w      r0, r0, #0x5d8
0800294e: c360       str        r3, [r0, #0xc]
08002950: 0361       str        r3, [r0, #0x10]
08002952: 4361       str        r3, [r0, #0x14]
08002954: fef72af9   bl         #0x8000bac
08002958: d4f81832   ldr.w      r3, [r4, #0x218]
0800295c: a268       ldr        r2, [r4, #8]
0800295e: fb1a       subs       r3, r7, r3
08002960: 9342       cmp        r3, r2
08002962: 98d2       bhs        #0x8002896
08002964: ba42       cmp        r2, r7
08002966: 96d2       bhs        #0x8002896
08002968: d4f81432   ldr.w      r3, [r4, #0x214]
0800296c: a721       movs       r1, #0xa7
0800296e: 794a       ldr        r2, [pc, #0x1e4] ; literal @08002b54 = 0x621b97c3 / float 7.17545e+20
08002970: ff1a       subs       r7, r7, r3
08002972: e068       ldr        r0, [r4, #0xc]
08002974: a2fb0723   umull      r2, r3, r2, r7
08002978: 00f5bb60   add.w      r0, r0, #0x5d8
0800297c: 9b09       lsrs       r3, r3, #6
0800297e: 01fb1377   mls        r7, r1, r3, r7
08002982: 0023       movs       r3, #0
08002984: 582f       cmp        r7, #0x58
08002986: c360       str        r3, [r0, #0xc]
08002988: 0361       str        r3, [r0, #0x10]
0800298a: 94bf       ite        ls
0800298c: 0027       movls      r7, #0
0800298e: 0127       movhi      r7, #1
08002990: 07ee907a   vmov       s15, r7
08002994: f8ee677a   vcvt.f32.u32 s15, s15
08002998: c0ed057a   vstr       s15, [r0, #0x14]
0800299c: fef706f9   bl         #0x8000bac
080029a0: 79e7       b          #0x8002896
080029a2: 6d4b       ldr        r3, [pc, #0x1b4] ; literal @08002b58 = 0x08019c10 / float 3.9003e-34
080029a4: 06ad       add        r5, sp, #0x18
080029a6: b6ee000a   vmov.f32   s0, #5.000000e-01
080029aa: 9fed6c1a   vldr       s2, [pc, #0x1b0] ; literal @08002b5c = 0x3f4ccccd / float 0.8
080029ae: dfed6c0a   vldr       s1, [pc, #0x1b0] ; literal @08002b60 = 0x00000000 / float 0
080029b2: 0df12808   add.w      r8, sp, #0x28
080029b6: 0fcb       ldm        r3, {r0, r1, r2, r3}
080029b8: 85e80f00   stm.w      r5, {r0, r1, r2, r3}
080029bc: 04f13c00   add.w      r0, r4, #0x3c
080029c0: 07f0e6f8   bl         #0x8009b90
080029c4: b7ee000a   vmov.f32   s0, #1.000000e+00
080029c8: 9fed661a   vldr       s2, [pc, #0x198] ; literal @08002b64 = 0x3e4ccccd / float 0.2
080029cc: 04f14800   add.w      r0, r4, #0x48
080029d0: dfed650a   vldr       s1, [pc, #0x194] ; literal @08002b68 = 0x3f19999a / float 0.6
080029d4: 07f0dcf8   bl         #0x8009b90
080029d8: 4346       mov        r3, r8
080029da: 22aa       add        r2, sp, #0x88
080029dc: 0c33       adds       r3, #0xc
080029de: 9fed608a   vldr       s16, [pc, #0x180] ; literal @08002b60 = 0x00000000 / float 0
080029e2: 9a42       cmp        r2, r3
080029e4: 03ed038a   vstr       s16, [r3, #-0xc]
080029e8: 03ed028a   vstr       s16, [r3, #-8]
080029ec: 03ed018a   vstr       s16, [r3, #-4]
080029f0: f4d1       bne        #0x80029dc
080029f2: 0721       movs       r1, #7
080029f4: 4046       mov        r0, r8
080029f6: 07f0bbf8   bl         #0x8009b70
080029fa: 0221       movs       r1, #2
080029fc: 0da8       add        r0, sp, #0x34
080029fe: 07f0b7f8   bl         #0x8009b70
08002a02: 0121       movs       r1, #1
08002a04: 10a8       add        r0, sp, #0x40
08002a06: 07f0b3f8   bl         #0x8009b70
08002a0a: 0021       movs       r1, #0
08002a0c: 13a8       add        r0, sp, #0x4c
08002a0e: 07f0aff8   bl         #0x8009b70
08002a12: 564a       ldr        r2, [pc, #0x158] ; literal @08002b6c = 0x3f50a3d7 / float 0.815
08002a14: 564b       ldr        r3, [pc, #0x158] ; literal @08002b70 = 0x3eb645a2 / float 0.356
08002a16: b6ee000a   vmov.f32   s0, #5.000000e-01
08002a1a: 5649       ldr        r1, [pc, #0x158] ; literal @08002b74 = 0x3f70f27c / float 0.9412
08002a1c: f0ee480a   vmov.f32   s1, s16
08002a20: 9fed4e1a   vldr       s2, [pc, #0x138] ; literal @08002b5c = 0x3f4ccccd / float 0.8
08002a24: 19a8       add        r0, sp, #0x64
08002a26: 1792       str        r2, [sp, #0x5c]
08002a28: 1893       str        r3, [sp, #0x60]
08002a2a: 1691       str        r1, [sp, #0x58]
08002a2c: 07f0b0f8   bl         #0x8009b90
08002a30: 0321       movs       r1, #3
08002a32: 1ca8       add        r0, sp, #0x70
08002a34: 07f09cf8   bl         #0x8009b70
08002a38: b7ee000a   vmov.f32   s0, #1.000000e+00
08002a3c: 9fed491a   vldr       s2, [pc, #0x124] ; literal @08002b64 = 0x3e4ccccd / float 0.2
08002a40: 1fa8       add        r0, sp, #0x7c
08002a42: dfed490a   vldr       s1, [pc, #0x124] ; literal @08002b68 = 0x3f19999a / float 0.6
08002a46: 07f0a3f8   bl         #0x8009b90
08002a4a: e068       ldr        r0, [r4, #0xc]
08002a4c: 0d99       ldr        r1, [sp, #0x34]
08002a4e: 00f28453   addw       r3, r0, #0x584
08002a52: 0e9a       ldr        r2, [sp, #0x38]
08002a54: 1960       str        r1, [r3]
08002a56: 00f5b163   add.w      r3, r0, #0x588
08002a5a: 1a60       str        r2, [r3]
08002a5c: 00f28c53   addw       r3, r0, #0x58c
08002a60: 0f9a       ldr        r2, [sp, #0x3c]
08002a62: 00f5af60   add.w      r0, r0, #0x578
08002a66: 1a60       str        r2, [r3]
08002a68: fef7a0f8   bl         #0x8000bac
08002a6c: e068       ldr        r0, [r4, #0xc]
08002a6e: 1699       ldr        r1, [sp, #0x58]
08002a70: 00f29c53   addw       r3, r0, #0x59c
08002a74: 179a       ldr        r2, [sp, #0x5c]
08002a76: 1960       str        r1, [r3]
08002a78: 00f5b463   add.w      r3, r0, #0x5a0
08002a7c: 1a60       str        r2, [r3]
08002a7e: 00f2a453   addw       r3, r0, #0x5a4
08002a82: 189a       ldr        r2, [sp, #0x60]
08002a84: 00f5b260   add.w      r0, r0, #0x590
08002a88: 1a60       str        r2, [r3]
08002a8a: fef78ff8   bl         #0x8000bac
08002a8e: e068       ldr        r0, [r4, #0xc]
08002a90: 1f99       ldr        r1, [sp, #0x7c]
08002a92: 00f2b453   addw       r3, r0, #0x5b4
08002a96: 209a       ldr        r2, [sp, #0x80]
08002a98: 1960       str        r1, [r3]
08002a9a: 00f5b763   add.w      r3, r0, #0x5b8
08002a9e: 1a60       str        r2, [r3]
08002aa0: 00f2bc53   addw       r3, r0, #0x5bc
08002aa4: 219a       ldr        r2, [sp, #0x84]
08002aa6: 00f5b560   add.w      r0, r0, #0x5a8
08002aaa: 1a60       str        r2, [r3]
08002aac: fef77ef8   bl         #0x8000bac
08002ab0: 06f02cfd   bl         #0x800950c
08002ab4: c0f30907   ubfx       r7, r0, #0, #0xa
08002ab8: 07f57f76   add.w      r6, r7, #0x3fc
08002abc: 2e4b       ldr        r3, [pc, #0xb8] ; literal @08002b78 = 0x00401005 / float 5.88322e-39
08002abe: b0ee007a   vmov.f32   s14, #2.000000e+00
08002ac2: 9fed2e6a   vldr       s12, [pc, #0xb8] ; literal @08002b7c = 0x447fc000 / float 1023
08002ac6: bfee001a   vmov.f32   s2, #-1.000000e+00
08002aca: a3fb0723   umull      r2, r3, r3, r7
08002ace: f7ee000a   vmov.f32   s1, #1.000000e+00
08002ad2: 9fed230a   vldr       s0, [pc, #0x8c] ; literal @08002b60 = 0x00000000 / float 0
08002ad6: 03a8       add        r0, sp, #0xc
08002ad8: fa1a       subs       r2, r7, r3
08002ada: 8ded030a   vstr       s0, [sp, #0xc]
08002ade: 03eb5203   add.w      r3, r3, r2, lsr #1
08002ae2: 8ded040a   vstr       s0, [sp, #0x10]
08002ae6: 8ded050a   vstr       s0, [sp, #0x14]
08002aea: 5b0a       lsrs       r3, r3, #9
08002aec: c3eb8323   rsb        r3, r3, r3, lsl #10
08002af0: fb1a       subs       r3, r7, r3
08002af2: ff37       adds       r7, #0xff
08002af4: 07ee903a   vmov       s15, r3
08002af8: f8eee77a   vcvt.f32.s32 s15, s15
08002afc: c7ee866a   vdiv.f32   s13, s15, s12
08002b00: a6ee871a   vfma.f32   s2, s13, s14
08002b04: b0eec11a   vabs.f32   s2, s2
08002b08: 30eec11a   vsub.f32   s2, s1, s2
08002b0c: 70eec10a   vsub.f32   s1, s1, s2
08002b10: 07f03ef8   bl         #0x8009b90
08002b14: 55f8040b   ldr        r0, [r5], #4
08002b18: e268       ldr        r2, [r4, #0xc]
08002b1a: 00eb4000   add.w      r0, r0, r0, lsl #1
08002b1e: ddf80cc0   ldr.w      ip, [sp, #0xc]
08002b22: 0499       ldr        r1, [sp, #0x10]
08002b24: 02ebc003   add.w      r3, r2, r0, lsl #3
08002b28: 03f28452   addw       r2, r3, #0x584
08002b2c: 03f5af60   add.w      r0, r3, #0x578
08002b30: c2f800c0   str.w      ip, [r2]
08002b34: 03f5b162   add.w      r2, r3, #0x588
08002b38: 03f28c53   addw       r3, r3, #0x58c
08002b3c: 1160       str        r1, [r2]
08002b3e: 059a       ldr        r2, [sp, #0x14]
08002b40: 1a60       str        r2, [r3]
08002b42: fef733f8   bl         #0x8000bac
08002b46: be42       cmp        r6, r7
08002b48: b8d1       bne        #0x8002abc
08002b4a: 22b0       add        sp, #0x88
08002b4c: bdec028b   vpop       {d8}
08002b50: bde8f081   pop.w      {r4, r5, r6, r7, r8, pc}
08002b54: c397       str        r7, [sp, #0x30c]
08002b56: 1b62       str        r3, [r3, #0x20]
08002b58: 109c       ldr        r4, [sp, #0x40]
08002b5a: 0108       lsrs       r1, r0, #0x20
08002b5c: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002b5e: 4c3f       subs       r7, #0x4c
08002b60: 0000       movs       r0, r0
08002b62: 0000       movs       r0, r0
08002b64: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002b66: 4c3e       subs       r6, #0x4c
08002b68: 9a99       ldr        r1, [sp, #0x268]
08002b6a: 193f       subs       r7, #0x19
08002b6c: d7a3       adr        r3, #0x35c
08002b6e: 503f       subs       r7, #0x50
08002b70: a245       cmp        sl, r4
08002b72: b63e       subs       r6, #0xb6
08002b74: 7cf2       .byte      0x7c, 0xf2
08002b76: 703f       subs       r7, #0x70
08002b78: 0510       asrs       r5, r0, #0x20
08002b7a: 4000       lsls       r0, r0, #1
08002b7c: 00c0       .byte      0x00, 0xc0
08002b7e: 7f44       add        r7, pc
08002b80: 94f81032   ldrb.w     r3, [r4, #0x210]
08002b84: 002b       cmp        r3, #0
08002b86: 40f0b282   bne.w      #0x80030ee
08002b8a: 6669       ldr        r6, [r4, #0x14]
08002b8c: 3368       ldr        r3, [r6]
08002b8e: 002b       cmp        r3, #0
08002b90: 00f07f81   beq.w      #0x8002e92
08002b94: e568       ldr        r5, [r4, #0xc]
08002b96: 06f0b9fc   bl         #0x800950c
08002b9a: f7ee007a   vmov.f32   s15, #1.000000e+00
08002b9e: 96ed127a   vldr       s14, [r6, #0x48]
08002ba2: 336a       ldr        r3, [r6, #0x20]
08002ba4: 04f15408   add.w      r8, r4, #0x54
08002ba8: 05f5b265   add.w      r5, r5, #0x590
08002bac: b4eee77a   vcmpe.f32  s14, s15
08002bb0: c01a       subs       r0, r0, r3
08002bb2: 736a       ldr        r3, [r6, #0x24]
08002bb4: f1ee10fa   vmrs       apsr_nzcv, fpscr
08002bb8: 84bf       itt        hi
08002bba: b0fbf3f2   udivhi     r2, r0, r3
08002bbe: 03fb1200   mlshi      r0, r3, r2, r0
08002bc2: b0eb530f   cmp.w      r0, r3, lsr #1
08002bc6: 80f07581   bhs.w      #0x8002eb4
08002bca: d8f80030   ldr.w      r3, [r8]
08002bce: 2846       mov        r0, r5
08002bd0: d8f80420   ldr.w      r2, [r8, #4]
08002bd4: eb60       str        r3, [r5, #0xc]
08002bd6: d8f80830   ldr.w      r3, [r8, #8]
08002bda: 2a61       str        r2, [r5, #0x10]
08002bdc: 6b61       str        r3, [r5, #0x14]
08002bde: fdf7e5ff   bl         #0x8000bac
08002be2: 6369       ldr        r3, [r4, #0x14]
08002be4: 1a68       ldr        r2, [r3]
08002be6: 012a       cmp        r2, #1
08002be8: 00f03e82   beq.w      #0x8003068
08002bec: e068       ldr        r0, [r4, #0xc]
08002bee: 636e       ldr        r3, [r4, #0x64]
08002bf0: 00f5af60   add.w      r0, r0, #0x578
08002bf4: 226e       ldr        r2, [r4, #0x60]
08002bf6: 0361       str        r3, [r0, #0x10]
08002bf8: a36e       ldr        r3, [r4, #0x68]
08002bfa: c260       str        r2, [r0, #0xc]
08002bfc: 4361       str        r3, [r0, #0x14]
08002bfe: fdf7d5ff   bl         #0x8000bac
08002c02: 2369       ldr        r3, [r4, #0x10]
08002c04: 5a68       ldr        r2, [r3, #4]
08002c06: 002a       cmp        r2, #0
08002c08: 00f06281   beq.w      #0x8002ed0
08002c0c: 5a7b       ldrb       r2, [r3, #0xd]
08002c0e: 002a       cmp        r2, #0
08002c10: 40f0b482   bne.w      #0x800317c
08002c14: 9a7c       ldrb       r2, [r3, #0x12]
08002c16: 002a       cmp        r2, #0
08002c18: 40f0b082   bne.w      #0x800317c
08002c1c: 93f87530   ldrb.w     r3, [r3, #0x75]
08002c20: 002b       cmp        r3, #0
08002c22: 00f00e83   beq.w      #0x8003242
08002c26: e068       ldr        r0, [r4, #0xc]
08002c28: 0023       movs       r3, #0
08002c2a: 4ff07e52   mov.w      r2, #0x3f800000
08002c2e: 00f5b860   add.w      r0, r0, #0x5c0
08002c32: c360       str        r3, [r0, #0xc]
08002c34: a14b       ldr        r3, [pc, #0x284] ; literal @08002ebc = 0x3f4ccccd / float 0.8
08002c36: 4261       str        r2, [r0, #0x14]
08002c38: 0361       str        r3, [r0, #0x10]
08002c3a: fdf7b7ff   bl         #0x8000bac
08002c3e: 58e1       b          #0x8002ef2
08002c40: 0df12808   add.w      r8, sp, #0x28
08002c44: 1faa       add        r2, sp, #0x7c
08002c46: 4646       mov        r6, r8
08002c48: 4346       mov        r3, r8
08002c4a: 0c33       adds       r3, #0xc
08002c4c: 9fed9c8a   vldr       s16, [pc, #0x270] ; literal @08002ec0 = 0x00000000 / float 0
08002c50: 9a42       cmp        r2, r3
08002c52: 03ed038a   vstr       s16, [r3, #-0xc]
08002c56: 03ed028a   vstr       s16, [r3, #-8]
08002c5a: 03ed018a   vstr       s16, [r3, #-4]
08002c5e: f4d1       bne        #0x8002c4a
08002c60: 06f054fc   bl         #0x800950c
08002c64: d4f82432   ldr.w      r3, [r4, #0x224]
08002c68: e268       ldr        r2, [r4, #0xc]
08002c6a: b0ee481a   vmov.f32   s2, s16
08002c6e: c31a       subs       r3, r0, r3
08002c70: 4046       mov        r0, r8
08002c72: d2eda70a   vldr       s1, [r2, #0x29c]
08002c76: 632b       cmp        r3, #0x63
08002c78: 92ed770a   vldr       s0, [r2, #0x1dc]
08002c7c: 8cbf       ite        hi
08002c7e: 0025       movhi      r5, #0
08002c80: 0125       movls      r5, #1
08002c82: 06f085ff   bl         #0x8009b90
08002c86: e368       ldr        r3, [r4, #0xc]
08002c88: 07ee905a   vmov       s15, r5
08002c8c: 0da8       add        r0, sp, #0x34
08002c8e: d3edaf0a   vldr       s1, [r3, #0x2bc]
08002c92: b8ee671a   vcvt.f32.u32 s2, s15
08002c96: 93ed7f0a   vldr       s0, [r3, #0x1fc]
08002c9a: 06f079ff   bl         #0x8009b90
08002c9e: e368       ldr        r3, [r4, #0xc]
08002ca0: 10a8       add        r0, sp, #0x40
08002ca2: b0ee481a   vmov.f32   s2, s16
08002ca6: d3edb70a   vldr       s1, [r3, #0x2dc]
08002caa: 93ed870a   vldr       s0, [r3, #0x21c]
08002cae: 06f06fff   bl         #0x8009b90
08002cb2: e068       ldr        r0, [r4, #0xc]
08002cb4: 90ed8f0a   vldr       s0, [r0, #0x23c]
08002cb8: 00f5c470   add.w      r0, r0, #0x188
08002cbc: d0ed5d0a   vldr       s1, [r0, #0x174]
08002cc0: 837d       ldrb       r3, [r0, #0x16]
08002cc2: 8ded010a   vstr       s0, [sp, #4]
08002cc6: cded000a   vstr       s1, [sp]
08002cca: 002b       cmp        r3, #0
08002ccc: 00f07b81   beq.w      #0x8002fc6
08002cd0: 04f03efe   bl         #0x8007950
08002cd4: 80f00100   eor        r0, r0, #1
08002cd8: dded000a   vldr       s1, [sp]
08002cdc: c3b2       uxtb       r3, r0
08002cde: 9ded010a   vldr       s0, [sp, #4]
08002ce2: 01ee103a   vmov       s2, r3
08002ce6: 13a8       add        r0, sp, #0x4c
08002ce8: b8ee411a   vcvt.f32.u32 s2, s2
08002cec: 06f050ff   bl         #0x8009b90
08002cf0: e068       ldr        r0, [r4, #0xc]
08002cf2: 90ed970a   vldr       s0, [r0, #0x25c]
08002cf6: 00f5d070   add.w      r0, r0, #0x1a0
08002cfa: d0ed5f0a   vldr       s1, [r0, #0x17c]
08002cfe: 837d       ldrb       r3, [r0, #0x16]
08002d00: 8ded010a   vstr       s0, [sp, #4]
08002d04: cded000a   vstr       s1, [sp]
08002d08: 002b       cmp        r3, #0
08002d0a: 00f05381   beq.w      #0x8002fb4
08002d0e: 04f01ffe   bl         #0x8007950
08002d12: 80f00100   eor        r0, r0, #1
08002d16: dded000a   vldr       s1, [sp]
08002d1a: c3b2       uxtb       r3, r0
08002d1c: 9ded010a   vldr       s0, [sp, #4]
08002d20: 01ee103a   vmov       s2, r3
08002d24: 16a8       add        r0, sp, #0x58
08002d26: b8ee411a   vcvt.f32.u32 s2, s2
08002d2a: 06f031ff   bl         #0x8009b90
08002d2e: e068       ldr        r0, [r4, #0xc]
08002d30: 90ed9f0a   vldr       s0, [r0, #0x27c]
08002d34: 00f5dc70   add.w      r0, r0, #0x1b8
08002d38: d0ed610a   vldr       s1, [r0, #0x184]
08002d3c: 837d       ldrb       r3, [r0, #0x16]
08002d3e: 8ded010a   vstr       s0, [sp, #4]
08002d42: cded000a   vstr       s1, [sp]
08002d46: 002b       cmp        r3, #0
08002d48: 00f02b81   beq.w      #0x8002fa2
08002d4c: 04f000fe   bl         #0x8007950
08002d50: 80f00100   eor        r0, r0, #1
08002d54: dded000a   vldr       s1, [sp]
08002d58: c3b2       uxtb       r3, r0
08002d5a: 9ded010a   vldr       s0, [sp, #4]
08002d5e: 01ee103a   vmov       s2, r3
08002d62: 19a8       add        r0, sp, #0x64
08002d64: b8ee411a   vcvt.f32.u32 s2, s2
08002d68: 06f012ff   bl         #0x8009b90
08002d6c: e068       ldr        r0, [r4, #0xc]
08002d6e: 90f88631   ldrb.w     r3, [r0, #0x186]
08002d72: 00f5b870   add.w      r0, r0, #0x170
08002d76: 002b       cmp        r3, #0
08002d78: 00f00e81   beq.w      #0x8002f98
08002d7c: 04f0e8fd   bl         #0x8007950
08002d80: 80f00100   eor        r0, r0, #1
08002d84: c3b2       uxtb       r3, r0
08002d86: 01ee103a   vmov       s2, r3
08002d8a: dfed4d0a   vldr       s1, [pc, #0x134] ; literal @08002ec0 = 0x00000000 / float 0
08002d8e: 1ca8       add        r0, sp, #0x70
08002d90: b8ee411a   vcvt.f32.u32 s2, s2
08002d94: b0ee600a   vmov.f32   s0, s1
08002d98: 06f0fafe   bl         #0x8009b90
08002d9c: e068       ldr        r0, [r4, #0xc]
08002d9e: 90f89030   ldrb.w     r3, [r0, #0x90]
08002da2: ff2b       cmp        r3, #0xff
08002da4: 00f03582   beq.w      #0x8003212
08002da8: 90f8b430   ldrb.w     r3, [r0, #0xb4]
08002dac: ff2b       cmp        r3, #0xff
08002dae: 00f02a82   beq.w      #0x8003206
08002db2: 90f8d830   ldrb.w     r3, [r0, #0xd8]
08002db6: ff2b       cmp        r3, #0xff
08002db8: 00f01f82   beq.w      #0x80031fa
08002dbc: 90f82031   ldrb.w     r3, [r0, #0x120]
08002dc0: ff2b       cmp        r3, #0xff
08002dc2: 00f01482   beq.w      #0x80031ee
08002dc6: 90f84431   ldrb.w     r3, [r0, #0x144]
08002dca: ff2b       cmp        r3, #0xff
08002dcc: 00f00982   beq.w      #0x80031e2
08002dd0: 90f86831   ldrb.w     r3, [r0, #0x168]
08002dd4: ff2b       cmp        r3, #0xff
08002dd6: 00f0fe81   beq.w      #0x80031d6
08002dda: 90f8fc30   ldrb.w     r3, [r0, #0xfc]
08002dde: ff2b       cmp        r3, #0xff
08002de0: 00f0f381   beq.w      #0x80031ca
08002de4: 4ff4af67   mov.w      r7, #0x578
08002de8: 0025       movs       r5, #0
08002dea: 00e0       b          #0x8002dee
08002dec: e068       ldr        r0, [r4, #0xc]
08002dee: 05eb4503   add.w      r3, r5, r5, lsl #1
08002df2: d6f800e0   ldr.w      lr, [r6]
08002df6: d6f804c0   ldr.w      ip, [r6, #4]
08002dfa: 0135       adds       r5, #1
08002dfc: 00ebc303   add.w      r3, r0, r3, lsl #3
08002e00: b268       ldr        r2, [r6, #8]
08002e02: 3844       add        r0, r7
08002e04: 0c36       adds       r6, #0xc
08002e06: 03f28451   addw       r1, r3, #0x584
08002e0a: 1837       adds       r7, #0x18
08002e0c: c1f800e0   str.w      lr, [r1]
08002e10: 03f5b161   add.w      r1, r3, #0x588
08002e14: 03f28c53   addw       r3, r3, #0x58c
08002e18: c1f800c0   str.w      ip, [r1]
08002e1c: 1a60       str        r2, [r3]
08002e1e: fdf7c5fe   bl         #0x8000bac
08002e22: 072d       cmp        r5, #7
08002e24: e2d1       bne        #0x8002dec
08002e26: 22b0       add        sp, #0x88
08002e28: bdec028b   vpop       {d8}
08002e2c: bde8f081   pop.w      {r4, r5, r6, r7, r8, pc}
08002e30: 557d       ldrb       r5, [r2, #0x15]
08002e32: 85f00105   eor        r5, r5, #1
08002e36: 5574       strb       r5, [r2, #0x11]
08002e38: fff7a9bb   b.w        #0x800258e
08002e3c: 5a7b       ldrb       r2, [r3, #0xd]
08002e3e: 002a       cmp        r2, #0
08002e40: 40f00b81   bne.w      #0x800305a
08002e44: 9a7c       ldrb       r2, [r3, #0x12]
08002e46: 82f00102   eor        r2, r2, #1
08002e4a: 5a73       strb       r2, [r3, #0xd]
08002e4c: e068       ldr        r0, [r4, #0xc]
08002e4e: 00f5d070   add.w      r0, r0, #0x1a0
08002e52: 04f047f9   bl         #0x80070e4
08002e56: 0028       cmp        r0, #0
08002e58: 3ff4f1ab   beq.w      #0x800263e
08002e5c: fff7e4bb   b.w        #0x8002628
08002e60: 9a7b       ldrb       r2, [r3, #0xe]
08002e62: 002a       cmp        r2, #0
08002e64: 40f0f580   bne.w      #0x8003052
08002e68: da7c       ldrb       r2, [r3, #0x13]
08002e6a: 82f00102   eor        r2, r2, #1
08002e6e: 9a73       strb       r2, [r3, #0xe]
08002e70: fff7e5bb   b.w        #0x800263e
08002e74: f7ee006a   vmov.f32   s13, #1.000000e+00
08002e78: 93ed127a   vldr       s14, [r3, #0x48]
08002e7c: 2669       ldr        r6, [r4, #0x10]
08002e7e: 86fec77a   vminnm.f32 s14, s13, s14
08002e82: c6ee877a   vdiv.f32   s15, s13, s14
08002e86: fceee77a   vcvt.u32.f32 s15, s15
08002e8a: c3ed0b7a   vstr       s15, [r3, #0x2c]
08002e8e: fff750bb   b.w        #0x8002532
08002e92: 96ed036a   vldr       s12, [r6, #0xc]
08002e96: f6ee007a   vmov.f32   s15, #5.000000e-01
08002e9a: dfed0a6a   vldr       s13, [pc, #0x28] ; literal @08002ec4 = 0x40c90fdb / float 6.28319
08002e9e: e568       ldr        r5, [r4, #0xc]
08002ea0: 86ee267a   vdiv.f32   s14, s12, s13
08002ea4: 05f5b265   add.w      r5, r5, #0x590
08002ea8: b4eee77a   vcmpe.f32  s14, s15
08002eac: f1ee10fa   vmrs       apsr_nzcv, fpscr
08002eb0: 00f13d81   bmi.w      #0x800312e
08002eb4: 04f16008   add.w      r8, r4, #0x60
08002eb8: 87e6       b          #0x8002bca
08002eba: 00bf       nop        
08002ebc: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002ebe: 4c3f       subs       r7, #0x4c
08002ec0: 0000       movs       r0, r0
08002ec2: 0000       movs       r0, r0
08002ec4: db0f       lsrs       r3, r3, #0x1f
08002ec6: c940       lsrs       r1, r1
08002ec8: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002eca: cc3e       subs       r6, #0xcc
08002ecc: cdcc       ldm        r4!, {r0, r2, r3, r6, r7}
08002ece: 4c3e       subs       r6, #0x4c
08002ed0: e068       ldr        r0, [r4, #0xc]
08002ed2: 1a7b       ldrb       r2, [r3, #0xc]
08002ed4: 00f5b860   add.w      r0, r0, #0x5c0
08002ed8: 002a       cmp        r2, #0
08002eda: 00f02181   beq.w      #0x8003120
08002ede: 04f11803   add.w      r3, r4, #0x18
08002ee2: 1a68       ldr        r2, [r3]
08002ee4: c260       str        r2, [r0, #0xc]
08002ee6: 5a68       ldr        r2, [r3, #4]
08002ee8: 9b68       ldr        r3, [r3, #8]
08002eea: 0261       str        r2, [r0, #0x10]
08002eec: 4361       str        r3, [r0, #0x14]
08002eee: fdf75dfe   bl         #0x8000bac
08002ef2: d4e90303   ldrd       r0, r3, [r4, #0xc]
08002ef6: 5a68       ldr        r2, [r3, #4]
08002ef8: 00f5bb60   add.w      r0, r0, #0x5d8
08002efc: 002a       cmp        r2, #0
08002efe: 40f04d81   bne.w      #0x800319c
08002f02: da7b       ldrb       r2, [r3, #0xf]
08002f04: 002a       cmp        r2, #0
08002f06: 00f04d81   beq.w      #0x80031a4
08002f0a: 04f11803   add.w      r3, r4, #0x18
08002f0e: 1a68       ldr        r2, [r3]
08002f10: c260       str        r2, [r0, #0xc]
08002f12: 5a68       ldr        r2, [r3, #4]
08002f14: 9b68       ldr        r3, [r3, #8]
08002f16: 0261       str        r2, [r0, #0x10]
08002f18: 4361       str        r3, [r0, #0x14]
08002f1a: fdf747fe   bl         #0x8000bac
08002f1e: d4f82032   ldr.w      r3, [r4, #0x220]
08002f22: fb1a       subs       r3, r7, r3
08002f24: 4f2b       cmp        r3, #0x4f
08002f26: 40f24481   bls.w      #0x80031b2
08002f2a: d4e90303   ldrd       r0, r3, [r4, #0xc]
08002f2e: 5a7c       ldrb       r2, [r3, #0x11]
08002f30: 00f5c160   add.w      r0, r0, #0x608
08002f34: 002a       cmp        r2, #0
08002f36: 40f02b81   bne.w      #0x8003190
08002f3a: 5b7d       ldrb       r3, [r3, #0x15]
08002f3c: 002b       cmp        r3, #0
08002f3e: 40f02781   bne.w      #0x8003190
08002f42: 04f16003   add.w      r3, r4, #0x60
08002f46: 1968       ldr        r1, [r3]
08002f48: 5a68       ldr        r2, [r3, #4]
08002f4a: 9b68       ldr        r3, [r3, #8]
08002f4c: c160       str        r1, [r0, #0xc]
08002f4e: 0261       str        r2, [r0, #0x10]
08002f50: 4361       str        r3, [r0, #0x14]
08002f52: fdf72bfe   bl         #0x8000bac
08002f56: d4e90303   ldrd       r0, r3, [r4, #0xc]
08002f5a: 5b68       ldr        r3, [r3, #4]
08002f5c: 00f5b560   add.w      r0, r0, #0x5a8
08002f60: 002b       cmp        r3, #0
08002f62: 40f01881   bne.w      #0x8003196
08002f66: 04f11803   add.w      r3, r4, #0x18
08002f6a: 1968       ldr        r1, [r3]
08002f6c: 5a68       ldr        r2, [r3, #4]
08002f6e: 9b68       ldr        r3, [r3, #8]
08002f70: c160       str        r1, [r0, #0xc]
08002f72: 4361       str        r3, [r0, #0x14]
08002f74: 0261       str        r2, [r0, #0x10]
08002f76: fdf719fe   bl         #0x8000bac
08002f7a: 2369       ldr        r3, [r4, #0x10]
08002f7c: e068       ldr        r0, [r4, #0xc]
08002f7e: d3f8d432   ldr.w      r3, [r3, #0x2d4]
08002f82: 013b       subs       r3, #1
08002f84: 042b       cmp        r3, #4
08002f86: 00f26881   bhi.w      #0x800325a
08002f8a: dfe813f0   tbh        [pc, r3, lsl #1]
08002f8e: ee00       lsls       r6, r5, #3
08002f90: ea00       lsls       r2, r5, #3
08002f92: e600       lsls       r6, r4, #3
08002f94: e200       lsls       r2, r4, #3
08002f96: d300       lsls       r3, r2, #3
08002f98: 04f0dafc   bl         #0x8007950
08002f9c: 01ee100a   vmov       s2, r0
08002fa0: f3e6       b          #0x8002d8a
08002fa2: 04f0d5fc   bl         #0x8007950
08002fa6: 9ded010a   vldr       s0, [sp, #4]
08002faa: 01ee100a   vmov       s2, r0
08002fae: dded000a   vldr       s1, [sp]
08002fb2: d6e6       b          #0x8002d62
08002fb4: 04f0ccfc   bl         #0x8007950
08002fb8: 9ded010a   vldr       s0, [sp, #4]
08002fbc: 01ee100a   vmov       s2, r0
08002fc0: dded000a   vldr       s1, [sp]
08002fc4: aee6       b          #0x8002d24
08002fc6: 04f0c3fc   bl         #0x8007950
08002fca: 9ded010a   vldr       s0, [sp, #4]
08002fce: 01ee100a   vmov       s2, r0
08002fd2: dded000a   vldr       s1, [sp]
08002fd6: 86e6       b          #0x8002ce6
08002fd8: d4ed0a6a   vldr       s13, [r4, #0x28]
08002fdc: d4ed0b7a   vldr       s15, [r4, #0x2c]
08002fe0: 94ed097a   vldr       s14, [r4, #0x24]
08002fe4: 46e4       b          #0x8002874
08002fe6: 2369       ldr        r3, [r4, #0x10]
08002fe8: 1fed497a   vldr       s14, [pc, #-0x124] ; literal @08002ec8 = 0x3ecccccd / float 0.4
08002fec: d3ed107a   vldr       s15, [r3, #0x40]
08002ff0: e068       ldr        r0, [r4, #0xc]
08002ff2: 77ee877a   vadd.f32   s15, s15, s14
08002ff6: 00f5bb60   add.w      r0, r0, #0x5d8
08002ffa: 67ee887a   vmul.f32   s15, s15, s16
08002ffe: c0ed037a   vstr       s15, [r0, #0xc]
08003002: c0ed047a   vstr       s15, [r0, #0x10]
08003006: c0ed057a   vstr       s15, [r0, #0x14]
0800300a: fdf7cffd   bl         #0x8000bac
0800300e: 26e4       b          #0x800285e
08003010: d4ed0a6a   vldr       s13, [r4, #0x28]
08003014: d4ed0b7a   vldr       s15, [r4, #0x2c]
08003018: 94ed097a   vldr       s14, [r4, #0x24]
0800301c: 68eea66a   vmul.f32   s13, s17, s13
08003020: e068       ldr        r0, [r4, #0xc]
08003022: 68eea77a   vmul.f32   s15, s17, s15
08003026: 67ee288a   vmul.f32   s17, s14, s17
0800302a: 00f5be60   add.w      r0, r0, #0x5f0
0800302e: c0ed046a   vstr       s13, [r0, #0x10]
08003032: c0ed038a   vstr       s17, [r0, #0xc]
08003036: c0ed057a   vstr       s15, [r0, #0x14]
0800303a: fdf7b7fd   bl         #0x8000bac
0800303e: fff7eebb   b.w        #0x800281e
08003042: d4ed0a6a   vldr       s13, [r4, #0x28]
08003046: d4ed0b7a   vldr       s15, [r4, #0x2c]
0800304a: 94ed097a   vldr       s14, [r4, #0x24]
0800304e: fff7b0bb   b.w        #0x80027b2
08003052: 0022       movs       r2, #0
08003054: 9a73       strb       r2, [r3, #0xe]
08003056: fff7f2ba   b.w        #0x800263e
0800305a: 2a46       mov        r2, r5
0800305c: 5a73       strb       r2, [r3, #0xd]
0800305e: f5e6       b          #0x8002e4c
08003060: 2a46       mov        r2, r5
08003062: 1a73       strb       r2, [r3, #0xc]
08003064: fff7daba   b.w        #0x800261c
08003068: d3ed127a   vldr       s15, [r3, #0x48]
0800306c: 94ed3a7a   vldr       s14, [r4, #0xe8]
08003070: b4ee677a   vcmp.f32   s14, s15
08003074: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003078: 7bd1       bne        #0x8003172
0800307a: d4f8e430   ldr.w      r3, [r4, #0xe4]
0800307e: c4ed3a7a   vstr       s15, [r4, #0xe8]
08003082: fb1a       subs       r3, r7, r3
08003084: 4f2b       cmp        r3, #0x4f
08003086: 3ff6b1ad   bhi.w      #0x8002bec
0800308a: e068       ldr        r0, [r4, #0xc]
0800308c: 636b       ldr        r3, [r4, #0x34]
0800308e: 00f5b260   add.w      r0, r0, #0x590
08003092: 226b       ldr        r2, [r4, #0x30]
08003094: 0361       str        r3, [r0, #0x10]
08003096: a36b       ldr        r3, [r4, #0x38]
08003098: c260       str        r2, [r0, #0xc]
0800309a: 4361       str        r3, [r0, #0x14]
0800309c: fdf786fd   bl         #0x8000bac
080030a0: a4e5       b          #0x8002bec
080030a2: 00f5b860   add.w      r0, r0, #0x5c0
080030a6: 0023       movs       r3, #0
080030a8: 4ff07e52   mov.w      r2, #0x3f800000
080030ac: 0361       str        r3, [r0, #0x10]
080030ae: c260       str        r2, [r0, #0xc]
080030b0: 4361       str        r3, [r0, #0x14]
080030b2: fdf77bfd   bl         #0x8000bac
080030b6: 45e4       b          #0x8002944
080030b8: e068       ldr        r0, [r4, #0xc]
080030ba: 0023       movs       r3, #0
080030bc: 4ff07e52   mov.w      r2, #0x3f800000
080030c0: 00f5b860   add.w      r0, r0, #0x5c0
080030c4: c360       str        r3, [r0, #0xc]
080030c6: 0261       str        r2, [r0, #0x10]
080030c8: 4361       str        r3, [r0, #0x14]
080030ca: fdf76ffd   bl         #0x8000bac
080030ce: 39e4       b          #0x8002944
080030d0: e068       ldr        r0, [r4, #0xc]
080030d2: 0df12808   add.w      r8, sp, #0x28
080030d6: 236e       ldr        r3, [r4, #0x60]
080030d8: 00f5af60   add.w      r0, r0, #0x578
080030dc: c360       str        r3, [r0, #0xc]
080030de: 636e       ldr        r3, [r4, #0x64]
080030e0: 0361       str        r3, [r0, #0x10]
080030e2: a36e       ldr        r3, [r4, #0x68]
080030e4: 4361       str        r3, [r0, #0x14]
080030e6: fdf761fd   bl         #0x8000bac
080030ea: fff70dbb   b.w        #0x8002708
080030ee: 1fed891a   vldr       s2, [pc, #-0x224] ; literal @08002ecc = 0x3e4ccccd / float 0.2
080030f2: 0023       movs       r3, #0
080030f4: 0aa8       add        r0, sp, #0x28
080030f6: f0ee410a   vmov.f32   s1, s2
080030fa: 0a93       str        r3, [sp, #0x28]
080030fc: b0ee410a   vmov.f32   s0, s2
08003100: 0b93       str        r3, [sp, #0x2c]
08003102: 0c93       str        r3, [sp, #0x30]
08003104: 06f044fd   bl         #0x8009b90
08003108: e068       ldr        r0, [r4, #0xc]
0800310a: 0b9b       ldr        r3, [sp, #0x2c]
0800310c: 00f5b260   add.w      r0, r0, #0x590
08003110: 0a9a       ldr        r2, [sp, #0x28]
08003112: 0361       str        r3, [r0, #0x10]
08003114: 0c9b       ldr        r3, [sp, #0x30]
08003116: c260       str        r2, [r0, #0xc]
08003118: 4361       str        r3, [r0, #0x14]
0800311a: fdf747fd   bl         #0x8000bac
0800311e: 60e5       b          #0x8002be2
08003120: 9b7c       ldrb       r3, [r3, #0x12]
08003122: 002b       cmp        r3, #0
08003124: 7ff4dbae   bne.w      #0x8002ede
08003128: 04f16003   add.w      r3, r4, #0x60
0800312c: d9e6       b          #0x8002ee2
0800312e: 04f11808   add.w      r8, r4, #0x18
08003132: 4ae5       b          #0x8002bca
08003134: a16c       ldr        r1, [r4, #0x48]
08003136: e26c       ldr        r2, [r4, #0x4c]
08003138: 236d       ldr        r3, [r4, #0x50]
0800313a: 00f5be60   add.w      r0, r0, #0x5f0
0800313e: c160       str        r1, [r0, #0xc]
08003140: 0261       str        r2, [r0, #0x10]
08003142: 4361       str        r3, [r0, #0x14]
08003144: fdf732fd   bl         #0x8000bac
08003148: 22b0       add        sp, #0x88
0800314a: bdec028b   vpop       {d8}
0800314e: bde8f081   pop.w      {r4, r5, r6, r7, r8, pc}
08003152: e16b       ldr        r1, [r4, #0x3c]
08003154: 226c       ldr        r2, [r4, #0x40]
08003156: 636c       ldr        r3, [r4, #0x44]
08003158: efe7       b          #0x800313a
0800315a: 216b       ldr        r1, [r4, #0x30]
0800315c: 626b       ldr        r2, [r4, #0x34]
0800315e: a36b       ldr        r3, [r4, #0x38]
08003160: ebe7       b          #0x800313a
08003162: 616a       ldr        r1, [r4, #0x24]
08003164: a26a       ldr        r2, [r4, #0x28]
08003166: e36a       ldr        r3, [r4, #0x2c]
08003168: e7e7       b          #0x800313a
0800316a: a169       ldr        r1, [r4, #0x18]
0800316c: e269       ldr        r2, [r4, #0x1c]
0800316e: 236a       ldr        r3, [r4, #0x20]
08003170: e3e7       b          #0x800313a
08003172: c4f8e470   str.w      r7, [r4, #0xe4]
08003176: c4ed3a7a   vstr       s15, [r4, #0xe8]
0800317a: 86e7       b          #0x800308a
0800317c: e068       ldr        r0, [r4, #0xc]
0800317e: 93f87530   ldrb.w     r3, [r3, #0x75]
08003182: 00f5b860   add.w      r0, r0, #0x5c0
08003186: 002b       cmp        r3, #0
08003188: 58d0       beq        #0x800323c
0800318a: 04f13003   add.w      r3, r4, #0x30
0800318e: a8e6       b          #0x8002ee2
08003190: 04f11803   add.w      r3, r4, #0x18
08003194: d7e6       b          #0x8002f46
08003196: 04f12403   add.w      r3, r4, #0x24
0800319a: e6e6       b          #0x8002f6a
0800319c: 9a7b       ldrb       r2, [r3, #0xe]
0800319e: 002a       cmp        r2, #0
080031a0: 7ff4b3ae   bne.w      #0x8002f0a
080031a4: db7c       ldrb       r3, [r3, #0x13]
080031a6: 002b       cmp        r3, #0
080031a8: 7ff4afae   bne.w      #0x8002f0a
080031ac: 04f16003   add.w      r3, r4, #0x60
080031b0: ade6       b          #0x8002f0e
080031b2: e068       ldr        r0, [r4, #0xc]
080031b4: 216b       ldr        r1, [r4, #0x30]
080031b6: 626b       ldr        r2, [r4, #0x34]
080031b8: 00f5bb60   add.w      r0, r0, #0x5d8
080031bc: a36b       ldr        r3, [r4, #0x38]
080031be: c160       str        r1, [r0, #0xc]
080031c0: 0261       str        r2, [r0, #0x10]
080031c2: 4361       str        r3, [r0, #0x14]
080031c4: fdf7f2fc   bl         #0x8000bac
080031c8: afe6       b          #0x8002f2a
080031ca: 1ca8       add        r0, sp, #0x70
080031cc: 0321       movs       r1, #3
080031ce: 06f0cffc   bl         #0x8009b70
080031d2: e068       ldr        r0, [r4, #0xc]
080031d4: 06e6       b          #0x8002de4
080031d6: 19a8       add        r0, sp, #0x64
080031d8: 0321       movs       r1, #3
080031da: 06f0c9fc   bl         #0x8009b70
080031de: e068       ldr        r0, [r4, #0xc]
080031e0: fbe5       b          #0x8002dda
080031e2: 16a8       add        r0, sp, #0x58
080031e4: 0321       movs       r1, #3
080031e6: 06f0c3fc   bl         #0x8009b70
080031ea: e068       ldr        r0, [r4, #0xc]
080031ec: f0e5       b          #0x8002dd0
080031ee: 13a8       add        r0, sp, #0x4c
080031f0: 0321       movs       r1, #3
080031f2: 06f0bdfc   bl         #0x8009b70
080031f6: e068       ldr        r0, [r4, #0xc]
080031f8: e5e5       b          #0x8002dc6
080031fa: 10a8       add        r0, sp, #0x40
080031fc: 0321       movs       r1, #3
080031fe: 06f0b7fc   bl         #0x8009b70
08003202: e068       ldr        r0, [r4, #0xc]
08003204: dae5       b          #0x8002dbc
08003206: 0da8       add        r0, sp, #0x34
08003208: 0321       movs       r1, #3
0800320a: 06f0b1fc   bl         #0x8009b70
0800320e: e068       ldr        r0, [r4, #0xc]
08003210: cfe5       b          #0x8002db2
08003212: 4046       mov        r0, r8
08003214: 0321       movs       r1, #3
08003216: 06f0abfc   bl         #0x8009b70
0800321a: e068       ldr        r0, [r4, #0xc]
0800321c: c4e5       b          #0x8002da8
0800321e: e068       ldr        r0, [r4, #0xc]
08003220: 0df12808   add.w      r8, sp, #0x28
08003224: a369       ldr        r3, [r4, #0x18]
08003226: 00f5af60   add.w      r0, r0, #0x578
0800322a: c360       str        r3, [r0, #0xc]
0800322c: e369       ldr        r3, [r4, #0x1c]
0800322e: 0361       str        r3, [r0, #0x10]
08003230: 236a       ldr        r3, [r4, #0x20]
08003232: 4361       str        r3, [r0, #0x14]
08003234: fdf7bafc   bl         #0x8000bac
08003238: fff766ba   b.w        #0x8002708
0800323c: 04f12403   add.w      r3, r4, #0x24
08003240: 4fe6       b          #0x8002ee2
08003242: e068       ldr        r0, [r4, #0xc]
08003244: 0023       movs       r3, #0
08003246: 4ff07e52   mov.w      r2, #0x3f800000
0800324a: 00f5b860   add.w      r0, r0, #0x5c0
0800324e: c360       str        r3, [r0, #0xc]
08003250: 0361       str        r3, [r0, #0x10]
08003252: 4261       str        r2, [r0, #0x14]
08003254: fdf7aafc   bl         #0x8000bac
08003258: 4be6       b          #0x8002ef2
0800325a: 216e       ldr        r1, [r4, #0x60]
0800325c: 626e       ldr        r2, [r4, #0x64]
0800325e: a36e       ldr        r3, [r4, #0x68]
08003260: 6be7       b          #0x800313a
08003262: 00bf       nop        
