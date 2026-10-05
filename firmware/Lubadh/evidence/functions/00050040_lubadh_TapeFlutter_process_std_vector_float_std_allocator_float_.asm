; lubadh::TapeFlutter::process(std::vector<float, std::allocator<float> >&)
; VA 0x50040 size 644

   50040: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   50044: e1a05001     	mov	r5, r1
   50048: e591e000     	ldr	lr, [r1]
   5004c: e5917004     	ldr	r7, [r1, #0x4]
   50050: e1a04000     	mov	r4, r0
   50054: ed2d8b04     	vpush	{d8, d9}
   50058: e15e0007     	cmp	lr, r7
   5005c: 0a000040     	beq	0x50164
   50060: e5903010     	ldr	r3, [r0, #0x10]
   50064: e3086081     	movw	r6, #0x8081
   50068: e3486080     	movt	r6, #0x8080
   5006c: e5902004     	ldr	r2, [r0, #0x4]
   50070: eddf3a8c     	vldr	s7, [pc, #560]          @ 0x502a8>&)+0x268> ; float 49170.2539062
   50074: eef77a00     	vmov.f32	s15, #1.000000e+00
   50078: ed9f4a8b     	vldr	s8, [pc, #556]          @ 0x502ac>&)+0x26c> ; float 255
   5007c: eeb63a00     	vmov.f32	s6, #5.000000e-01
   50080: ed946a00     	vldr	s12, [r4]
   50084: edd46a02     	vldr	s13, [r4, #8]
   50088: ee867a23     	vdiv.f32	s14, s12, s7
   5008c: eee76a04     	vfma.f32	s13, s14, s8
   50090: eef46ae7     	vcmpe.f32	s13, s15
   50094: edc46a02     	vstr	s13, [r4, #8]
   50098: eef1fa10     	vmrs	APSR_nzcv, fpscr
   5009c: ba000005     	blt	0x500b8
   500a0: ee766ae7     	vsub.f32	s13, s13, s15
   500a4: e2822001     	add	r2, r2, #1
   500a8: eef46ae7     	vcmpe.f32	s13, s15
   500ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   500b0: aafffffa     	bge	0x500a0
   500b4: edc46a02     	vstr	s13, [r4, #8]
   500b8: ed946a03     	vldr	s12, [r4, #12]
   500bc: e0810296     	umull	r0, r1, r6, r2
   500c0: ed947a05     	vldr	s14, [r4, #20]
   500c4: ee865a23     	vdiv.f32	s10, s12, s7
   500c8: e1a013a1     	lsr	r1, r1, #7
   500cc: e0611401     	rsb	r1, r1, r1, lsl #8
   500d0: e0422001     	sub	r2, r2, r1
   500d4: e5842004     	str	r2, [r4, #0x4]
   500d8: e0840102     	add	r0, r4, r2, lsl #2
   500dc: ed906a1f     	vldr	s12, [r0, #124]
   500e0: edd05a20     	vldr	s11, [r0, #128]
   500e4: ee755ac6     	vsub.f32	s11, s11, s12
   500e8: eea57a04     	vfma.f32	s14, s10, s8
   500ec: eea56aa6     	vfma.f32	s12, s11, s13
   500f0: eeb47ae7     	vcmpe.f32	s14, s15
   500f4: ed847a05     	vstr	s14, [r4, #20]
   500f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   500fc: ba000007     	blt	0x50120
   50100: e283c001     	add	r12, r3, #1
   50104: ee377a67     	vsub.f32	s14, s14, s15
   50108: e1a0300c     	mov	r3, r12
   5010c: e28cc001     	add	r12, r12, #1
   50110: eeb47ae7     	vcmpe.f32	s14, s15
   50114: eef1fa10     	vmrs	APSR_nzcv, fpscr
   50118: aafffff9     	bge	0x50104
   5011c: ed847a05     	vstr	s14, [r4, #20]
   50120: e0801396     	umull	r1, r0, r6, r3
   50124: edd44a06     	vldr	s9, [r4, #24]
   50128: e1a003a0     	lsr	r0, r0, #7
   5012c: ee646a83     	vmul.f32	s13, s9, s6
   50130: e0600400     	rsb	r0, r0, r0, lsl #8
   50134: e0433000     	sub	r3, r3, r0
   50138: e5843010     	str	r3, [r4, #0x10]
   5013c: e0840103     	add	r0, r4, r3, lsl #2
   50140: edd05a1f     	vldr	s11, [r0, #124]
   50144: ed905a20     	vldr	s10, [r0, #128]
   50148: ee355a65     	vsub.f32	s10, s10, s11
   5014c: eee55a07     	vfma.f32	s11, s10, s14
   50150: ee267aa5     	vmul.f32	s14, s13, s11
   50154: eea47a86     	vfma.f32	s14, s9, s12
   50158: ecae7a01     	vstmia	lr!, {s14}
   5015c: e157000e     	cmp	r7, lr
   50160: 1affffc6     	bne	0x50080
   50164: e2847a01     	add	r7, r4, #4096
   50168: e597680c     	ldr	r6, [r7, #0x80c]
   5016c: e5978810     	ldr	r8, [r7, #0x810]
   50170: e1580006     	cmp	r8, r6
   50174: 0a000021     	beq	0x50200
   50178: e2849d39     	add	r9, r4, #3648
   5017c: eddf8a4b     	vldr	s17, [pc, #300]         @ 0x502b0>&)+0x270> ; float 0
   50180: ed9f8a4b     	vldr	s16, [pc, #300]         @ 0x502b4>&)+0x274> ; float 2.32830643654e-10
   50184: ed9f9a4b     	vldr	s18, [pc, #300]         @ 0x502b8>&)+0x278> ; float 0.999999940395
   50188: ea000006     	b	0x501a8
   5018c: ed927a00     	vldr	s14, [r2]
   50190: edd36a00     	vldr	s13, [r3]
   50194: ee766ac7     	vsub.f32	s13, s13, s14
   50198: eea67aa7     	vfma.f32	s14, s13, s15
   5019c: eca67a01     	vstmia	r6!, {s14}
   501a0: e1580006     	cmp	r8, r6
   501a4: 0a000015     	beq	0x50200
   501a8: e1a00009     	mov	r0, r9
   501ac: eb0000e7     	bl	0x50550
   501b0: ee070a90     	vmov	s15, r0
   501b4: eeb77a00     	vmov.f32	s14, #1.000000e+00
   501b8: e2872b02     	add	r2, r7, #2048
   501bc: eef87a67     	vcvt.f32.u32	s15, s15
   501c0: e1a03002     	mov	r3, r2
   501c4: e2833008     	add	r3, r3, #8
   501c8: e2822004     	add	r2, r2, #4
   501cc: ee777aa8     	vadd.f32	s15, s15, s17
   501d0: ee677a88     	vmul.f32	s15, s15, s16
   501d4: eef47ac7     	vcmpe.f32	s15, s14
   501d8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   501dc: baffffea     	blt	0x5018c
   501e0: e2872b02     	add	r2, r7, #2048
   501e4: edd27a01     	vldr	s15, [r2, #4]
   501e8: ed927a02     	vldr	s14, [r2, #8]
   501ec: ee377a67     	vsub.f32	s14, s14, s15
   501f0: eee77a09     	vfma.f32	s15, s14, s18
   501f4: ece67a01     	vstmia	r6!, {s15}
   501f8: e1580006     	cmp	r8, r6
   501fc: 1affffe9     	bne	0x501a8
   50200: e2846b06     	add	r6, r4, #6144
   50204: e2840024     	add	r0, r4, #36
   50208: e286600c     	add	r6, r6, #12
   5020c: e1a01006     	mov	r1, r6
   50210: ebffeb82     	bl	0x4b020
   50214: e1a01006     	mov	r1, r6
   50218: e2840050     	add	r0, r4, #80
   5021c: ebffeb7f     	bl	0x4b020
   50220: e597280c     	ldr	r2, [r7, #0x80c]
   50224: e5971810     	ldr	r1, [r7, #0x810]
   50228: e1510002     	cmp	r1, r2
   5022c: 11a03002     	movne	r3, r2
   50230: 1eb27a04     	vmovne.f32	s14, #1.000000e+01
   50234: 0a000004     	beq	0x5024c
   50238: edd37a00     	vldr	s15, [r3]
   5023c: ee677a87     	vmul.f32	s15, s15, s14
   50240: ece37a01     	vstmia	r3!, {s15}
   50244: e1510003     	cmp	r1, r3
   50248: 1afffffa     	bne	0x50238
   5024c: e5953000     	ldr	r3, [r5]
   50250: e5951004     	ldr	r1, [r5, #0x4]
   50254: e0411003     	sub	r1, r1, r3
   50258: e1b00121     	lsrs	r0, r1, #2
   5025c: 0a00000f     	beq	0x502a0
   50260: e0831001     	add	r1, r3, r1
   50264: eddf4a14     	vldr	s9, [pc, #80]           @ 0x502bc>&)+0x27c> ; float 0.0833339691162
   50268: ed9f5a14     	vldr	s10, [pc, #80]          @ 0x502c0>&)+0x280> ; float 0.958333015442
   5026c: eef05a04     	vmov.f32	s11, #2.500000e+00
   50270: eeb16a04     	vmov.f32	s12, #5.000000e+00
   50274: ed947a07     	vldr	s14, [r4, #28]
   50278: ecf26a01     	vldmia	r2!, {s13}
   5027c: edd37a00     	vldr	s15, [r3]
   50280: eee67a87     	vfma.f32	s15, s13, s14
   50284: ee777aa5     	vadd.f32	s15, s15, s11
   50288: ee877a86     	vdiv.f32	s14, s15, s12
   5028c: eef07a45     	vmov.f32	s15, s10
   50290: eee77a24     	vfma.f32	s15, s14, s9
   50294: ece37a01     	vstmia	r3!, {s15}
   50298: e1510003     	cmp	r1, r3
   5029c: 1afffff4     	bne	0x50274
   502a0: ecbd8b04     	vpop	{d8, d9}
   502a4: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   502a8: 41 12 40 47  	.word	0x47401241
   502ac: 00 00 7f 43  	.word	0x437f0000
   502b0: 00 00 00 00  	.word	0x00000000
   502b4: 00 00 80 2f  	.word	0x2f800000
   502b8: ff ff 7f 3f  	.word	0x3f7fffff
   502bc: 00 ab aa 3d  	.word	0x3daaab00
   502c0: 50 55 75 3f  	.word	0x3f755550
