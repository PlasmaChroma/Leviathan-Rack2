08003264: 10b5       push       {r4, lr}
08003266: 1e4c       ldr        r4, [pc, #0x78] ; literal @080032e0 = 0x24000cf8 / float 2.77666e-17
08003268: 2368       ldr        r3, [r4]
0800326a: 012b       cmp        r3, #1
0800326c: 14d0       beq        #0x8003298
0800326e: 1d4b       ldr        r3, [pc, #0x74] ; literal @080032e4 = 0x24000458 / float 2.77593e-17
08003270: 0022       movs       r2, #0
08003272: 1a70       strb       r2, [r3]
08003274: 06f04cf9   bl         #0x8009510
08003278: 0446       mov        r4, r0
0800327a: 06f047f9   bl         #0x800950c
0800327e: 1a4a       ldr        r2, [pc, #0x68] ; literal @080032e8 = 0x2400040c / float 2.7759e-17
08003280: 1a4b       ldr        r3, [pc, #0x68] ; literal @080032ec = 0x24001440 / float 2.77727e-17
08003282: 1b49       ldr        r1, [pc, #0x6c] ; literal @080032f0 = 0x2400041c / float 2.77591e-17
08003284: 1460       str        r4, [r2]
08003286: 0122       movs       r2, #1
08003288: 0860       str        r0, [r1]
0800328a: 1a70       strb       r2, [r3]
0800328c: 06f03ef9   bl         #0x800950c
08003290: 184b       ldr        r3, [pc, #0x60] ; literal @080032f4 = 0x24000ab8 / float 2.77647e-17
08003292: c3f82402   str.w      r0, [r3, #0x224]
08003296: 10bd       pop        {r4, pc}
08003298: 06f03af9   bl         #0x8009510
0800329c: a369       ldr        r3, [r4, #0x18]
0800329e: 164a       ldr        r2, [pc, #0x58] ; literal @080032f8 = 0x51eb851f / float 1.26444e+11
080032a0: c31a       subs       r3, r0, r3
080032a2: 616d       ldr        r1, [r4, #0x54]
080032a4: a061       str        r0, [r4, #0x18]
080032a6: a2fb0323   umull      r2, r3, r2, r3
080032aa: 226d       ldr        r2, [r4, #0x50]
080032ac: a165       str        r1, [r4, #0x58]
080032ae: 9b09       lsrs       r3, r3, #6
080032b0: 6265       str        r2, [r4, #0x54]
080032b2: 9a42       cmp        r2, r3
080032b4: e361       str        r3, [r4, #0x1c]
080032b6: 2365       str        r3, [r4, #0x50]
080032b8: 09d3       blo        #0x80032ce
080032ba: 9942       cmp        r1, r3
080032bc: 0ed9       bls        #0x80032dc
080032be: 8a42       cmp        r2, r1
080032c0: 28bf       it         hs
080032c2: 0a46       movhs      r2, r1
080032c4: 0123       movs       r3, #1
080032c6: e264       str        r2, [r4, #0x4c]
080032c8: 84f85c30   strb.w     r3, [r4, #0x5c]
080032cc: cfe7       b          #0x800326e
080032ce: 9142       cmp        r1, r2
080032d0: f8d9       bls        #0x80032c4
080032d2: 9942       cmp        r1, r3
080032d4: 0a46       mov        r2, r1
080032d6: 28bf       it         hs
080032d8: 1a46       movhs      r2, r3
080032da: f3e7       b          #0x80032c4
080032dc: 1a46       mov        r2, r3
080032de: f1e7       b          #0x80032c4
080032e0: f80c       lsrs       r0, r7, #0x13
080032e2: 0024       movs       r4, #0
080032e4: 5804       lsls       r0, r3, #0x11
080032e6: 0024       movs       r4, #0
080032e8: 0c04       lsls       r4, r1, #0x10
080032ea: 0024       movs       r4, #0
080032ec: 4014       asrs       r0, r0, #0x11
080032ee: 0024       movs       r4, #0
080032f0: 1c04       lsls       r4, r3, #0x10
080032f2: 0024       movs       r4, #0
080032f4: b80a       lsrs       r0, r7, #0xa
080032f6: 0024       movs       r4, #0
080032f8: 1f85       strh       r7, [r3, #0x28]
080032fa: eb51       str        r3, [r5, r7]
080032fc: 2de9f04f   push.w     {r4, r5, r6, r7, r8, sb, sl, fp, lr}
08003300: dff8d093   ldr.w      sb, [pc, #0x3d0] ; literal @080036d4 = 0x24001460 / float 2.77728e-17
08003304: 0c46       mov        r4, r1
08003306: 1646       mov        r6, r2
08003308: 2ded028b   vpush      {d8}
0800330c: 83b0       sub        sp, #0xc
0800330e: 0190       str        r0, [sp, #4]
08003310: 06f0fef8   bl         #0x8009510
08003314: c9f80800   str.w      r0, [sb, #8]
08003318: 06f0f8f8   bl         #0x800950c
0800331c: 0546       mov        r5, r0
0800331e: d948       ldr        r0, [pc, #0x364] ; literal @08003684 = 0x24000a9c / float 2.77646e-17
08003320: 03f0e0fe   bl         #0x80070e4
08003324: 0028       cmp        r0, #0
08003326: 40f07082   bne.w      #0x800380a
0800332a: d74f       ldr        r7, [pc, #0x35c] ; literal @08003688 = 0x24001440 / float 2.77727e-17
0800332c: 3b78       ldrb       r3, [r7]
0800332e: 002b       cmp        r3, #0
08003330: 40f06682   bne.w      #0x8003800
08003334: dff8a0a3   ldr.w      sl, [pc, #0x3a0] ; literal @080036d8 = 0x24000cf8 / float 2.77666e-17
08003338: d448       ldr        r0, [pc, #0x350] ; literal @0800368c = 0x24000ab8 / float 2.77647e-17
0800333a: fff731f8   bl         #0x80023a0
0800333e: daf80010   ldr.w      r1, [sl]
08003342: 0129       cmp        r1, #1
08003344: 00f01182   beq.w      #0x800376a
08003348: 0121       movs       r1, #1
0800334a: d14f       ldr        r7, [pc, #0x344] ; literal @08003690 = 0x24000d68 / float 2.77669e-17
0800334c: 0022       movs       r2, #0
0800334e: c7f8bc11   str.w      r1, [r7, #0x1bc]
08003352: d049       ldr        r1, [pc, #0x340] ; literal @08003694 = 0x24000458 / float 2.77593e-17
08003354: 0a70       strb       r2, [r1]
08003356: dff834b3   ldr.w      fp, [pc, #0x334] ; literal @0800368c = 0x24000ab8 / float 2.77647e-17
0800335a: 8bf81022   strb.w     r2, [fp, #0x210]
0800335e: 002e       cmp        r6, #0
08003360: 00f03d82   beq.w      #0x80037de
08003364: 0025       movs       r5, #0
08003366: dff87483   ldr.w      r8, [pc, #0x374] ; literal @080036dc = 0x24000408 / float 2.7759e-17
0800336a: 39e0       b          #0x80033e0
0800336c: f7ee004a   vmov.f32   s9, #1.000000e+00
08003370: daed0d6a   vldr       s13, [sl, #0x34]
08003374: 9fedc86a   vldr       s12, [pc, #0x320] ; literal @08003698 = 0x40c90fdb / float 6.28319
08003378: 9aed045a   vldr       s10, [sl, #0x10]
0800337c: 84eea67a   vdiv.f32   s14, s9, s13
08003380: caed026a   vstr       s13, [sl, #8]
08003384: daed037a   vldr       s15, [sl, #0xc]
08003388: dfedc45a   vldr       s11, [pc, #0x310] ; literal @0800369c = 0x49742400 / float 1e+06
0800338c: 66ee864a   vmul.f32   s9, s13, s12
08003390: c4ee856a   vdiv.f32   s13, s9, s10
08003394: 27ee257a   vmul.f32   s14, s14, s11
08003398: bceec77a   vcvt.u32.f32 s14, s14
0800339c: 8aed0c7a   vstr       s14, [sl, #0x30]
080033a0: 76eea77a   vadd.f32   s15, s13, s15
080033a4: caed056a   vstr       s13, [sl, #0x14]
080033a8: f4eec67a   vcmpe.f32  s15, s12
080033ac: caed037a   vstr       s15, [sl, #0xc]
080033b0: f1ee10fa   vmrs       apsr_nzcv, fpscr
080033b4: 80f2a780   bge.w      #0x8003506
080033b8: 06f0a8f8   bl         #0x800950c
080033bc: daed0c7a   vldr       s15, [sl, #0x30]
080033c0: f7ee006a   vmov.f32   s13, #1.000000e+00
080033c4: 9fedb67a   vldr       s14, [pc, #0x2d8] ; literal @080036a0 = 0x358637bd / float 1e-06
080033c8: f8ee677a   vcvt.f32.u32 s15, s15
080033cc: 67ee877a   vmul.f32   s15, s15, s14
080033d0: 86eea77a   vdiv.f32   s14, s13, s15
080033d4: 87ed3b7a   vstr       s14, [r7, #0xec]
080033d8: 0235       adds       r5, #2
080033da: ae42       cmp        r6, r5
080033dc: 40f2f380   bls.w      #0x80035c6
080033e0: daed0c7a   vldr       s15, [sl, #0x30]
080033e4: f1ee044a   vmov.f32   s9, #5.000000e+00
080033e8: 9fedae6a   vldr       s12, [pc, #0x2b8] ; literal @080036a4 = 0x3f7ff76d / float 0.999869
080033ec: f8ee677a   vcvt.f32.u32 s15, s15
080033f0: 9aed1a7a   vldr       s14, [sl, #0x68]
080033f4: dfedac5a   vldr       s11, [pc, #0x2b0] ; literal @080036a8 = 0x3a83126f / float 0.001
080033f8: 27ee067a   vmul.f32   s14, s14, s12
080033fc: dfedab6a   vldr       s13, [pc, #0x2ac] ; literal @080036ac = 0x39093000 / float 0.000130832
08003400: 67eea57a   vmul.f32   s15, s15, s11
08003404: 9aed0f4a   vldr       s8, [sl, #0x3c]
08003408: 9aed0e8a   vldr       s16, [sl, #0x38]
0800340c: 9aed105a   vldr       s10, [sl, #0x40]
08003410: a7eea67a   vfma.f32   s14, s15, s13
08003414: dfeda63a   vldr       s7, [pc, #0x298] ; literal @080036b0 = 0x00000000 / float 0
08003418: 38ee048a   vadd.f32   s16, s16, s8
0800341c: 8aed196a   vstr       s12, [sl, #0x64]
08003420: b7ee004a   vmov.f32   s8, #1.000000e+00
08003424: caed186a   vstr       s13, [sl, #0x60]
08003428: 88fe238a   vmaxnm.f32 s16, s16, s7
0800342c: 88fe448a   vminnm.f32 s16, s16, s8
08003430: 77eec77a   vsub.f32   s15, s15, s14
08003434: 8aed1a7a   vstr       s14, [sl, #0x68]
08003438: b0eee77a   vabs.f32   s14, s15
0800343c: caed1b7a   vstr       s15, [sl, #0x6c]
08003440: b4eee47a   vcmpe.f32  s14, s9
08003444: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003448: b4ee458a   vcmp.f32   s16, s10
0800344c: ccbf       ite        gt
0800344e: 0122       movgt      r2, #1
08003450: 0022       movle      r2, #0
08003452: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003456: 88f80020   strb.w     r2, [r8]
0800345a: 1bd1       bne        #0x8003494
0800345c: daf80020   ldr.w      r2, [sl]
08003460: 002a       cmp        r2, #0
08003462: 83d0       beq        #0x800336c
08003464: 06f054f8   bl         #0x8009510
08003468: 9af85c20   ldrb.w     r2, [sl, #0x5c]
0800346c: 002a       cmp        r2, #0
0800346e: 40f0de80   bne.w      #0x800362e
08003472: f7ee006a   vmov.f32   s13, #1.000000e+00
08003476: daed127a   vldr       s15, [sl, #0x48]
0800347a: 86eea77a   vdiv.f32   s14, s13, s15
0800347e: daed137a   vldr       s15, [sl, #0x4c]
08003482: f8ee677a   vcvt.f32.u32 s15, s15
08003486: 67ee877a   vmul.f32   s15, s15, s14
0800348a: fceee77a   vcvt.u32.f32 s15, s15
0800348e: caed0c7a   vstr       s15, [sl, #0x30]
08003492: 91e7       b          #0x80033b8
08003494: daed117a   vldr       s15, [sl, #0x44]
08003498: 8aed108a   vstr       s16, [sl, #0x40]
0800349c: 37eea57a   vadd.f32   s14, s15, s11
080034a0: b4eec78a   vcmpe.f32  s16, s14
080034a4: f1ee10fa   vmrs       apsr_nzcv, fpscr
080034a8: 06dc       bgt        #0x80034b8
080034aa: 77eee57a   vsub.f32   s15, s15, s11
080034ae: b4eee78a   vcmpe.f32  s16, s15
080034b2: f1ee10fa   vmrs       apsr_nzcv, fpscr
080034b6: d1d5       bpl        #0x800345c
080034b8: dfed7d8a   vldr       s17, [pc, #0x1f4] ; literal @080036b0 = 0x00000000 / float 0
080034bc: dfed7d7a   vldr       s15, [pc, #0x1f4] ; literal @080036b4 = 0x40e4f29c / float 7.15462
080034c0: b0ee680a   vmov.f32   s0, s17
080034c4: 8aed118a   vstr       s16, [sl, #0x44]
080034c8: a8ee270a   vfma.f32   s0, s16, s15
080034cc: 14f06afc   bl         #0x8017da4
080034d0: 9fed797a   vldr       s14, [pc, #0x1e4] ; literal @080036b8 = 0x41040000 / float 8.25
080034d4: f2ee007a   vmov.f32   s15, #8.000000e+00
080034d8: 784a       ldr        r2, [pc, #0x1e0] ; literal @080036bc = 0x08019d20 / float 3.90042e-34
080034da: 28ee078a   vmul.f32   s16, s16, s14
080034de: dfed786a   vldr       s13, [pc, #0x1e0] ; literal @080036c0 = 0x3d800000 / float 0.0625
080034e2: 88fe288a   vmaxnm.f32 s16, s16, s17
080034e6: 88fe678a   vminnm.f32 s16, s16, s15
080034ea: bdeec88a   vcvt.s32.f32 s16, s16
080034ee: 20ee260a   vmul.f32   s0, s0, s13
080034f2: 18ee103a   vmov       r3, s16
080034f6: 8aed0d0a   vstr       s0, [sl, #0x34]
080034fa: 02eb8302   add.w      r2, r2, r3, lsl #2
080034fe: 1268       ldr        r2, [r2]
08003500: caf84820   str.w      r2, [sl, #0x48]
08003504: aae7       b          #0x800345c
08003506: 77eec67a   vsub.f32   s15, s15, s12
0800350a: caed037a   vstr       s15, [sl, #0xc]
0800350e: 05f0ffff   bl         #0x8009510
08003512: daf81820   ldr.w      r2, [sl, #0x18]
08003516: 6b49       ldr        r1, [pc, #0x1ac] ; literal @080036c4 = 0x51eb851f / float 1.26444e+11
08003518: 821a       subs       r2, r0, r2
0800351a: caf81800   str.w      r0, [sl, #0x18]
0800351e: a1fb0232   umull      r3, r2, r1, r2
08003522: 9209       lsrs       r2, r2, #6
08003524: caf81c20   str.w      r2, [sl, #0x1c]
08003528: 05f0f0ff   bl         #0x800950c
0800352c: f7ee006a   vmov.f32   s13, #1.000000e+00
08003530: 9aed127a   vldr       s14, [sl, #0x48]
08003534: daf82020   ldr.w      r2, [sl, #0x20]
08003538: b4eee67a   vcmpe.f32  s14, s13
0800353c: 821a       subs       r2, r0, r2
0800353e: caf82420   str.w      r2, [sl, #0x24]
08003542: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003546: 0bdd       ble        #0x8003560
08003548: 86ee876a   vdiv.f32   s12, s13, s14
0800354c: 07ee902a   vmov       s15, r2
08003550: f8ee677a   vcvt.f32.u32 s15, s15
08003554: 67ee867a   vmul.f32   s15, s15, s12
08003558: fceee77a   vcvt.u32.f32 s15, s15
0800355c: caed097a   vstr       s15, [sl, #0x24]
08003560: caf82000   str.w      r0, [sl, #0x20]
08003564: 05f0d4ff   bl         #0x8009510
08003568: dff874c1   ldr.w      ip, [pc, #0x174] ; literal @080036e0 = 0x24000414 / float 2.7759e-17
0800356c: dfed566a   vldr       s13, [pc, #0x158] ; literal @080036c8 = 0x43480000 / float 200
08003570: dcf80030   ldr.w      r3, [ip]
08003574: 5549       ldr        r1, [pc, #0x154] ; literal @080036cc = 0x2400147c / float 2.77729e-17
08003576: cce90003   strd       r0, r3, [ip]
0800357a: c01a       subs       r0, r0, r3
0800357c: dff864c1   ldr.w      ip, [pc, #0x164] ; literal @080036e4 = 0x24000400 / float 2.7759e-17
08003580: 0a68       ldr        r2, [r1]
08003582: 07ee900a   vmov       s15, r0
08003586: dcf80000   ldr.w      r0, [ip]
0800358a: 4a60       str        r2, [r1, #4]
0800358c: f8ee677a   vcvt.f32.u32 s15, s15
08003590: ccf80400   str.w      r0, [ip, #4]
08003594: 98f80030   ldrb.w     r3, [r8]
08003598: 87eea67a   vdiv.f32   s14, s15, s13
0800359c: fdeec77a   vcvt.s32.f32 s15, s14
080035a0: 17ee900a   vmov       r0, s15
080035a4: c1ed007a   vstr       s15, [r1]
080035a8: 121a       subs       r2, r2, r0
080035aa: ccf80020   str.w      r2, [ip]
080035ae: 002b       cmp        r3, #0
080035b0: 7ff412af   bne.w      #0x80033d8
080035b4: 0235       adds       r5, #2
080035b6: 0122       movs       r2, #1
080035b8: c7f89030   str.w      r3, [r7, #0x90]
080035bc: ae42       cmp        r6, r5
080035be: 87f87420   strb.w     r2, [r7, #0x74]
080035c2: 3ff60daf   bhi.w      #0x80033e0
080035c6: 98f80030   ldrb.w     r3, [r8]
080035ca: 9bf82822   ldrb.w     r2, [fp, #0x228]
080035ce: 87f89430   strb.w     r3, [r7, #0x94]
080035d2: 87f87c20   strb.w     r2, [r7, #0x7c]
080035d6: 2246       mov        r2, r4
080035d8: 87f8a831   strb.w     r3, [r7, #0x1a8]
080035dc: 3346       mov        r3, r6
080035de: 0199       ldr        r1, [sp, #4]
080035e0: 2b48       ldr        r0, [pc, #0xac] ; literal @08003690 = 0x24000d68 / float 2.77669e-17
080035e2: fef733fc   bl         #0x8001e4c
080035e6: 3a4b       ldr        r3, [pc, #0xe8] ; literal @080036d0 = 0x24000410 / float 2.7759e-17
080035e8: 1b78       ldrb       r3, [r3]
080035ea: 002b       cmp        r3, #0
080035ec: 40f0a180   bne.w      #0x8003732
080035f0: 05f08eff   bl         #0x8009510
080035f4: d9f80820   ldr.w      r2, [sb, #8]
080035f8: 99ed017a   vldr       s14, [sb, #4]
080035fc: 821a       subs       r2, r0, r2
080035fe: 99f80030   ldrb.w     r3, [sb]
08003602: 07ee902a   vmov       s15, r2
08003606: f8ee677a   vcvt.f32.u32 s15, s15
0800360a: 67ee877a   vmul.f32   s15, s15, s14
0800360e: 002b       cmp        r3, #0
08003610: 6ad0       beq        #0x80036e8
08003612: 0023       movs       r3, #0
08003614: c9ed057a   vstr       s15, [sb, #0x14]
08003618: c9ed037a   vstr       s15, [sb, #0xc]
0800361c: c9ed047a   vstr       s15, [sb, #0x10]
08003620: 89f80030   strb.w     r3, [sb]
08003624: 03b0       add        sp, #0xc
08003626: bdec028b   vpop       {d8}
0800362a: bde8f08f   pop.w      {r4, r5, r6, r7, r8, sb, sl, fp, pc}
0800362e: b7ee007a   vmov.f32   s14, #1.000000e+00
08003632: 9aed128a   vldr       s16, [sl, #0x48]
08003636: 0023       movs       r3, #0
08003638: b4ee478a   vcmp.f32   s16, s14
0800363c: 8af85c30   strb.w     r3, [sl, #0x5c]
08003640: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003644: 00f0e480   beq.w      #0x8003810
08003648: c7ee086a   vdiv.f32   s13, s14, s16
0800364c: daed137a   vldr       s15, [sl, #0x4c]
08003650: f8ee677a   vcvt.f32.u32 s15, s15
08003654: b4eec78a   vcmpe.f32  s16, s14
08003658: f1ee10fa   vmrs       apsr_nzcv, fpscr
0800365c: 67eea67a   vmul.f32   s15, s15, s13
08003660: fceee77a   vcvt.u32.f32 s15, s15
08003664: 17ee902a   vmov       r2, s15
08003668: caed0c7a   vstr       s15, [sl, #0x30]
0800366c: 71d4       bmi        #0x8003752
0800366e: 05f04dff   bl         #0x800950c
08003672: f7ee007a   vmov.f32   s15, #1.000000e+00
08003676: b4eee78a   vcmpe.f32  s16, s15
0800367a: f1ee10fa   vmrs       apsr_nzcv, fpscr
0800367e: bff655af   bge.w      #0x800352c
08003682: 9be6       b          #0x80033bc
08003684: 9c0a       lsrs       r4, r3, #0xa
08003686: 0024       movs       r4, #0
08003688: 4014       asrs       r0, r0, #0x11
0800368a: 0024       movs       r4, #0
0800368c: b80a       lsrs       r0, r7, #0xa
0800368e: 0024       movs       r4, #0
08003690: 680d       lsrs       r0, r5, #0x15
08003692: 0024       movs       r4, #0
08003694: 5804       lsls       r0, r3, #0x11
08003696: 0024       movs       r4, #0
08003698: db0f       lsrs       r3, r3, #0x1f
0800369a: c940       lsrs       r1, r1
0800369c: 0024       movs       r4, #0
0800369e: 7449       ldr        r1, [pc, #0x1d0] ; literal @08003870 = 0x51eb851f / float 1.26444e+11
080036a0: bd37       adds       r7, #0xbd
080036a2: 8635       adds       r5, #0x86
080036a4: 6df7       .byte      0x6d, 0xf7
080036a6: 7f3f       subs       r7, #0x7f
080036a8: 6f12       asrs       r7, r5, #9
080036aa: 833a       subs       r2, #0x83
080036ac: 0030       adds       r0, #0
080036ae: 0939       subs       r1, #9
080036b0: 0000       movs       r0, r0
080036b2: 0000       movs       r0, r0
080036b4: 9cf2       .byte      0x9c, 0xf2
080036b6: e440       lsrs       r4, r4
080036b8: 0000       movs       r0, r0
080036ba: 0441       asrs       r4, r0
080036bc: 209d       ldr        r5, [sp, #0x80]
080036be: 0108       lsrs       r1, r0, #0x20
080036c0: 0000       movs       r0, r0
080036c2: 803d       subs       r5, #0x80
080036c4: 1f85       strh       r7, [r3, #0x28]
080036c6: eb51       str        r3, [r5, r7]
080036c8: 0000       movs       r0, r0
080036ca: 4843       muls       r0, r1, r0
080036cc: 7c14       asrs       r4, r7, #0x11
080036ce: 0024       movs       r4, #0
080036d0: 1004       lsls       r0, r2, #0x10
080036d2: 0024       movs       r4, #0
080036d4: 6014       asrs       r0, r4, #0x11
080036d6: 0024       movs       r4, #0
080036d8: f80c       lsrs       r0, r7, #0x13
080036da: 0024       movs       r4, #0
080036dc: 0804       lsls       r0, r1, #0x10
080036de: 0024       movs       r4, #0
080036e0: 1404       lsls       r4, r2, #0x10
080036e2: 0024       movs       r4, #0
080036e4: 0004       lsls       r0, r0, #0x10
080036e6: 0024       movs       r4, #0
080036e8: 99ed047a   vldr       s14, [sb, #0x10]
080036ec: d9ed066a   vldr       s13, [sb, #0x18]
080036f0: f4eec77a   vcmpe.f32  s15, s14
080036f4: 99ed037a   vldr       s14, [sb, #0xc]
080036f8: 99ed056a   vldr       s12, [sb, #0x14]
080036fc: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003700: f4eec77a   vcmpe.f32  s15, s14
08003704: b7ee007a   vmov.f32   s14, #1.000000e+00
08003708: c8bf       it         gt
0800370a: c9ed047a   vstrgt     s15, [sb, #0x10]
0800370e: f1ee10fa   vmrs       apsr_nzcv, fpscr
08003712: 37ee667a   vsub.f32   s14, s14, s13
08003716: 48bf       it         mi
08003718: c9ed037a   vstrmi     s15, [sb, #0xc]
0800371c: 27ee067a   vmul.f32   s14, s14, s12
08003720: a7eea67a   vfma.f32   s14, s15, s13
08003724: 89ed057a   vstr       s14, [sb, #0x14]
08003728: 03b0       add        sp, #0xc
0800372a: bdec028b   vpop       {d8}
0800372e: bde8f08f   pop.w      {r4, r5, r6, r7, r8, sb, sl, fp, pc}
08003732: 013e       subs       r6, #1
08003734: 04f10803   add.w      r3, r4, #8
08003738: 7608       lsrs       r6, r6, #1
0800373a: 03ebc603   add.w      r3, r3, r6, lsl #3
0800373e: d4ed017a   vldr       s15, [r4, #4]
08003742: 0834       adds       r4, #8
08003744: f1ee677a   vneg.f32   s15, s15
08003748: a342       cmp        r3, r4
0800374a: 44ed017a   vstr       s15, [r4, #-4]
0800374e: f6d1       bne        #0x800373e
08003750: 4ee7       b          #0x80035f0
08003752: fceee66a   vcvt.u32.f32 s13, s13
08003756: daf82c10   ldr.w      r1, [sl, #0x2c]
0800375a: 0131       adds       r1, #1
0800375c: 16ee90ca   vmov       ip, s13
08003760: 6145       cmp        r1, ip
08003762: 67d2       bhs        #0x8003834
08003764: caf82c10   str.w      r1, [sl, #0x2c]
08003768: 81e7       b          #0x800366e
0800376a: f7ee007a   vmov.f32   s15, #1.000000e+00
0800376e: 9aed127a   vldr       s14, [sl, #0x48]
08003772: b4eee77a   vcmpe.f32  s14, s15
08003776: f1ee10fa   vmrs       apsr_nzcv, fpscr
0800377a: 07dd       ble        #0x800378c
0800377c: fceec77a   vcvt.u32.f32 s15, s14
08003780: f8ee677a   vcvt.f32.u32 s15, s15
08003784: fceee77a   vcvt.u32.f32 s15, s15
08003788: 17ee901a   vmov       r1, s15
0800378c: daf83020   ldr.w      r2, [sl, #0x30]
08003790: f1ee006a   vmov.f32   s13, #4.000000e+00
08003794: 3148       ldr        r0, [pc, #0xc4] ; literal @0800385c = 0x10624dd3 / float 4.46306e-29
08003796: 324f       ldr        r7, [pc, #0xc8] ; literal @08003860 = 0x24000d68 / float 2.77669e-17
08003798: a0fb0232   umull      r3, r2, r0, r2
0800379c: c7f8bc11   str.w      r1, [r7, #0x1bc]
080037a0: 9309       lsrs       r3, r2, #6
080037a2: 304a       ldr        r2, [pc, #0xc0] ; literal @08003864 = 0x2400041c / float 2.77591e-17
080037a4: 07ee903a   vmov       s15, r3
080037a8: 1268       ldr        r2, [r2]
080037aa: f8eee77a   vcvt.f32.s32 s15, s15
080037ae: ad1a       subs       r5, r5, r2
080037b0: 67ee877a   vmul.f32   s15, s15, s14
080037b4: 07ee105a   vmov       s14, r5
080037b8: b8ee477a   vcvt.f32.u32 s14, s14
080037bc: 67eea67a   vmul.f32   s15, s15, s13
080037c0: b4eee77a   vcmpe.f32  s14, s15
080037c4: f1ee10fa   vmrs       apsr_nzcv, fpscr
080037c8: 44dd       ble        #0x8003854
080037ca: 0122       movs       r2, #1
080037cc: 2649       ldr        r1, [pc, #0x98] ; literal @08003868 = 0x24000458 / float 2.77593e-17
080037ce: dff8a4b0   ldr.w      fp, [pc, #0xa4] ; literal @08003874 = 0x24000ab8 / float 2.77647e-17
080037d2: 0a70       strb       r2, [r1]
080037d4: 8bf81022   strb.w     r2, [fp, #0x210]
080037d8: 002e       cmp        r6, #0
080037da: 7ff4c3ad   bne.w      #0x8003364
080037de: 2348       ldr        r0, [pc, #0x8c] ; literal @0800386c = 0x24000408 / float 2.7759e-17
080037e0: 2246       mov        r2, r4
080037e2: 9bf82832   ldrb.w     r3, [fp, #0x228]
080037e6: 0078       ldrb       r0, [r0]
080037e8: 87f87c30   strb.w     r3, [r7, #0x7c]
080037ec: 3346       mov        r3, r6
080037ee: 87f89400   strb.w     r0, [r7, #0x94]
080037f2: 87f8a801   strb.w     r0, [r7, #0x1a8]
080037f6: 0199       ldr        r1, [sp, #4]
080037f8: 1948       ldr        r0, [pc, #0x64] ; literal @08003860 = 0x24000d68 / float 2.77669e-17
080037fa: fef727fb   bl         #0x8001e4c
080037fe: f7e6       b          #0x80035f0
08003800: 05f086fe   bl         #0x8009510
08003804: 0023       movs       r3, #0
08003806: 3b70       strb       r3, [r7]
08003808: 94e5       b          #0x8003334
0800380a: fff72bfd   bl         #0x8003264
0800380e: 8ce5       b          #0x800332a
08003810: daf84c20   ldr.w      r2, [sl, #0x4c]
08003814: caf82800   str.w      r0, [sl, #0x28]
08003818: caf83020   str.w      r2, [sl, #0x30]
0800381c: 05f076fe   bl         #0x800950c
08003820: daf82020   ldr.w      r2, [sl, #0x20]
08003824: 801a       subs       r0, r0, r2
08003826: caf82400   str.w      r0, [sl, #0x24]
0800382a: 05f06ffe   bl         #0x800950c
0800382e: caf82000   str.w      r0, [sl, #0x20]
08003832: 97e6       b          #0x8003564
08003834: daf82810   ldr.w      r1, [sl, #0x28]
08003838: 0132       adds       r2, #1
0800383a: caf82c30   str.w      r3, [sl, #0x2c]
0800383e: 401a       subs       r0, r0, r1
08003840: 0b49       ldr        r1, [pc, #0x2c] ; literal @08003870 = 0x51eb851f / float 1.26444e+11
08003842: a1fb0030   umull      r3, r0, r1, r0
08003846: 02eb9010   add.w      r0, r2, r0, lsr #6
0800384a: caf82800   str.w      r0, [sl, #0x28]
0800384e: 05f05dfe   bl         #0x800950c
08003852: 6be6       b          #0x800352c
08003854: 044a       ldr        r2, [pc, #0x10] ; literal @08003868 = 0x24000458 / float 2.77593e-17
08003856: 1278       ldrb       r2, [r2]
08003858: 7de5       b          #0x8003356
0800385a: 00bf       nop        
0800385c: d34d       ldr        r5, [pc, #0x34c] ; literal @08003bac = 0x3e10d0c3 / float 0.141421
0800385e: 6210       asrs       r2, r4, #1
08003860: 680d       lsrs       r0, r5, #0x15
08003862: 0024       movs       r4, #0
08003864: 1c04       lsls       r4, r3, #0x10
08003866: 0024       movs       r4, #0
08003868: 5804       lsls       r0, r3, #0x11
0800386a: 0024       movs       r4, #0
0800386c: 0804       lsls       r0, r1, #0x10
0800386e: 0024       movs       r4, #0
08003870: 1f85       strh       r7, [r3, #0x28]
08003872: eb51       str        r3, [r5, r7]
08003874: b80a       lsrs       r0, r7, #0xa
08003876: 0024       movs       r4, #0
