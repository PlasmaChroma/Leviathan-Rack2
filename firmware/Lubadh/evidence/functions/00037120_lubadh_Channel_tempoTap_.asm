; lubadh::Channel::tempoTap()
; VA 0x37120 size 300

   37120: e92d40f0     	push	{r4, r5, r6, r7, lr}
   37124: e1a04000     	mov	r4, r0
   37128: ed2d8b02     	vpush	{d8}
   3712c: e24dd00c     	sub	sp, sp, #12
   37130: e1a0000d     	mov	r0, sp
   37134: ebff79f7     	bl	0x15918     @ imm = #-0x21824 ; _ZNSt6chrono3_V212system_clock3nowEv
   37138: e5d431e0     	ldrb	r3, [r4, #0x1e0]
   3713c: e1cd60d0     	ldrd	r6, r7, [sp]
   37140: e3530000     	cmp	r3, #0
   37144: 0a000006     	beq	0x37164
   37148: e3a03000     	mov	r3, #0
   3714c: e58461d0     	str	r6, [r4, #0x1d0]
   37150: e58471d4     	str	r7, [r4, #0x1d4]
   37154: e5c431e0     	strb	r3, [r4, #0x1e0]
   37158: e28dd00c     	add	sp, sp, #12
   3715c: ecbd8b02     	vpop	{d8}
   37160: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   37164: e2845a2a     	add	r5, r4, #172032
   37168: e2841e1d     	add	r1, r4, #464
   3716c: e1c100d0     	ldrd	r0, r1, [r1]
   37170: e59534f4     	ldr	r3, [r5, #0x4f4]
   37174: ee073a90     	vmov	s15, r3
   37178: e0560000     	subs	r0, r6, r0
   3717c: e0c71001     	sbc	r1, r7, r1
   37180: eeb88ae7     	vcvt.f32.s32	s16, s15
   37184: ebff7b48     	bl	0x15eac    @ imm = #-0x212e0 ; __aeabi_l2f
   37188: ed9f6a2b     	vldr	s12, [pc, #172]         @ 0x3723c ; float 1000000000
   3718c: ee050a90     	vmov	s11, r0
   37190: eddf7a2a     	vldr	s15, [pc, #168]         @ 0x37240 ; float 49170.2539062
   37194: eef16a00     	vmov.f32	s13, #4.000000e+00
   37198: ee857a86     	vdiv.f32	s14, s11, s12
   3719c: ee277a27     	vmul.f32	s14, s14, s15
   371a0: eec87a07     	vdiv.f32	s15, s16, s14
   371a4: eef47ae6     	vcmpe.f32	s15, s13
   371a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   371ac: 5ef07a66     	vmovpl.f32	s15, s13
   371b0: 4a000017     	bmi	0x37214
   371b4: e59430e8     	ldr	r3, [r4, #0xe8]
   371b8: e59320b0     	ldr	r2, [r3, #0xb0]
   371bc: e5922040     	ldr	r2, [r2, #0x40]
   371c0: e3520003     	cmp	r2, #3
   371c4: 0a000017     	beq	0x37228
   371c8: e5952498     	ldr	r2, [r5, #0x498]
   371cc: e3520b02     	cmp	r2, #2048
   371d0: ba000017     	blt	0x37234
   371d4: edc47a0b     	vstr	s15, [r4, #44]
   371d8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   371dc: edd36a2b     	vldr	s13, [r3, #172]
   371e0: ed947ab6     	vldr	s14, [r4, #728]
   371e4: eddf5a16     	vldr	s11, [pc, #88]          @ 0x37244 ; float 2.70000004768
   371e8: eefd6ae6     	vcvt.s32.f32	s13, s13
   371ec: ee777ac7     	vsub.f32	s15, s15, s14
   371f0: eef86ae6     	vcvt.f32.s32	s13, s13
   371f4: ee867aa5     	vdiv.f32	s14, s13, s11
   371f8: ee377a06     	vadd.f32	s14, s14, s12
   371fc: eebd7ac7     	vcvt.s32.f32	s14, s14
   37200: eef86ac7     	vcvt.f32.s32	s13, s14
   37204: ed847ab8     	vstr	s14, [r4, #736]
   37208: ee877aa6     	vdiv.f32	s14, s15, s13
   3720c: ed847ab7     	vstr	s14, [r4, #732]
   37210: eaffffcc     	b	0x37148
   37214: ed9f7a0b     	vldr	s14, [pc, #44]          @ 0x37248 ; float 0.00999999977648
   37218: eef47ac7     	vcmpe.f32	s15, s14
   3721c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37220: def07a47     	vmovle.f32	s15, s14
   37224: eaffffe2     	b	0x371b4
   37228: e59420ec     	ldr	r2, [r4, #0xec]
   3722c: e3520000     	cmp	r2, #0
   37230: 0affffe7     	beq	0x371d4
   37234: eef17a67     	vneg.f32	s15, s15
   37238: eaffffe5     	b	0x371d4
   3723c: 28 6b 6e 4e  	.word	0x4e6e6b28
   37240: 41 12 40 47  	.word	0x47401241
   37244: cd cc 2c 40  	.word	0x402ccccd
   37248: 0a d7 23 3c  	.word	0x3c23d70a
