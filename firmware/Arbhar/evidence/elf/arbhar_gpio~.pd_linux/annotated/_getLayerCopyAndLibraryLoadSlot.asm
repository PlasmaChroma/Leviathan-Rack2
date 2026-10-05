0000df28 <_getLayerCopyAndLibraryLoadSlot>:
    df28: e92d4070     	push	{r4, r5, r6, lr}
    df2c: e1a05000     	mov	r5, r0
    df30: ed2d8b02     	vpush	{d8}
    df34: ed9f0ade     	vldr	s0, [pc, #888]          @ 0xe2b4 <_getLayerCopyAndLibraryLoadSlot+0x38c>  // f32=43
    df38: ebffd62a     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xa758  // CALL readFromSharedMem
    df3c: e2853c16     	add	r3, r5, #5632
    df40: e3000fff     	movw	r0, #0xfff
    df44: ed938a0c     	vldr	s16, [r3, #48]
    df48: eefd8ac0     	vcvt.s32.f32	s17, s0
    df4c: eef87ae8     	vcvt.f32.s32	s15, s17
    df50: eebd0ae7     	vcvt.s32.f32	s0, s15
    df54: ee104a10     	vmov	r4, s0
    df58: ee101a10     	vmov	r1, s0
    df5c: eb001398     	bl	0x12dc4 <__divsi3>      @ imm = #0x4e60
    df60: e3540000     	cmp	r4, #0
    df64: da0000ad     	ble	0xe220 <_getLayerCopyAndLibraryLoadSlot+0x2f8> @ imm = #0x2b4
    df68: e59fc350     	ldr	r12, [pc, #0x350]       @ 0xe2c0 <_getLayerCopyAndLibraryLoadSlot+0x398>  // u32=0x19438; f32?=1.45006365e-40
    df6c: ee000a90     	vmov	s1, r0
    df70: e2141007     	ands	r1, r4, #7
    df74: e3a03000     	mov	r3, #0
    df78: e08f600c     	add	r6, pc, r12
    df7c: eeb81ae0     	vcvt.f32.s32	s2, s1
    df80: e28600f0     	add	r0, r6, #240
    df84: e1a02000     	mov	r2, r0
    df88: 0a00002b     	beq	0xe03c <_getLayerCopyAndLibraryLoadSlot+0x114> @ imm = #0xac
    df8c: e3510001     	cmp	r1, #1
    df90: 0a000022     	beq	0xe020 <_getLayerCopyAndLibraryLoadSlot+0xf8> @ imm = #0x88
    df94: e3510002     	cmp	r1, #2
    df98: 0a00001b     	beq	0xe00c <_getLayerCopyAndLibraryLoadSlot+0xe4> @ imm = #0x6c
    df9c: e3510003     	cmp	r1, #3
    dfa0: 0a000014     	beq	0xdff8 <_getLayerCopyAndLibraryLoadSlot+0xd0> @ imm = #0x50
    dfa4: e3510004     	cmp	r1, #4
    dfa8: 0a00000d     	beq	0xdfe4 <_getLayerCopyAndLibraryLoadSlot+0xbc> @ imm = #0x34
    dfac: e3510005     	cmp	r1, #5
    dfb0: 0a000006     	beq	0xdfd0 <_getLayerCopyAndLibraryLoadSlot+0xa8> @ imm = #0x18
    dfb4: e3510006     	cmp	r1, #6
    dfb8: 1a0000b8     	bne	0xe2a0 <_getLayerCopyAndLibraryLoadSlot+0x378> @ imm = #0x2e0
    dfbc: ee073a10     	vmov	s14, r3
    dfc0: e2833001     	add	r3, r3, #1
    dfc4: eef81ac7     	vcvt.f32.s32	s3, s14
    dfc8: ee212a81     	vmul.f32	s4, s3, s2
    dfcc: eca22a01     	vstmia	r2!, {s4}
    dfd0: ee023a90     	vmov	s5, r3
    dfd4: e2833001     	add	r3, r3, #1
    dfd8: eeb83ae2     	vcvt.f32.s32	s6, s5
    dfdc: ee633a01     	vmul.f32	s7, s6, s2
    dfe0: ece23a01     	vstmia	r2!, {s7}
    dfe4: ee043a10     	vmov	s8, r3
    dfe8: e2833001     	add	r3, r3, #1
    dfec: eef84ac4     	vcvt.f32.s32	s9, s8
    dff0: ee245a81     	vmul.f32	s10, s9, s2
    dff4: eca25a01     	vstmia	r2!, {s10}
    dff8: ee053a90     	vmov	s11, r3
    dffc: e2833001     	add	r3, r3, #1
    e000: eeb86ae5     	vcvt.f32.s32	s12, s11
    e004: ee666a01     	vmul.f32	s13, s12, s2
    e008: ece26a01     	vstmia	r2!, {s13}
    e00c: ee073a90     	vmov	s15, r3
    e010: e2833001     	add	r3, r3, #1
    e014: eeb80ae7     	vcvt.f32.s32	s0, s15
    e018: ee600a01     	vmul.f32	s1, s0, s2
    e01c: ece20a01     	vstmia	r2!, {s1}
    e020: ee073a10     	vmov	s14, r3
    e024: e2833001     	add	r3, r3, #1
    e028: e1540003     	cmp	r4, r3
    e02c: eef81ac7     	vcvt.f32.s32	s3, s14
    e030: ee212a81     	vmul.f32	s4, s3, s2
    e034: eca22a01     	vstmia	r2!, {s4}
    e038: 0a00002b     	beq	0xe0ec <_getLayerCopyAndLibraryLoadSlot+0x1c4> @ imm = #0xac
    e03c: e2836001     	add	r6, r3, #1
    e040: e2831003     	add	r1, r3, #3
    e044: e283e004     	add	lr, r3, #4
    e048: e283c002     	add	r12, r3, #2
    e04c: ee023a90     	vmov	s5, r3
    e050: ee041a90     	vmov	s9, r1
    e054: e2831005     	add	r1, r3, #5
    e058: eeb83ae2     	vcvt.f32.s32	s6, s5
    e05c: ee05ea10     	vmov	s10, lr
    e060: e283e006     	add	lr, r3, #6
    e064: ee066a90     	vmov	s13, r6
    e068: e2836007     	add	r6, r3, #7
    e06c: ee03ca90     	vmov	s7, r12
    e070: e1a0c002     	mov	r12, r2
    e074: ee051a90     	vmov	s11, r1
    e078: e2833008     	add	r3, r3, #8
    e07c: ee06ea10     	vmov	s12, lr
    e080: e1540003     	cmp	r4, r3
    e084: ee006a10     	vmov	s0, r6
    e088: e2822020     	add	r2, r2, #32
    e08c: eeb84ae3     	vcvt.f32.s32	s8, s7
    e090: eef87ae6     	vcvt.f32.s32	s15, s13
    e094: eeb82ae5     	vcvt.f32.s32	s4, s11
    e098: eef80ae4     	vcvt.f32.s32	s1, s9
    e09c: eef81ac5     	vcvt.f32.s32	s3, s10
    e0a0: eef82ac6     	vcvt.f32.s32	s5, s12
    e0a4: eef85ac0     	vcvt.f32.s32	s11, s0
    e0a8: ee237a01     	vmul.f32	s14, s6, s2
    e0ac: ee673a81     	vmul.f32	s7, s15, s2
    e0b0: ecac7a01     	vstmia	r12!, {s14}
    e0b4: ee243a01     	vmul.f32	s6, s8, s2
    e0b8: ed423a07     	vstr	s7, [r2, #-28]
    e0bc: ee604a81     	vmul.f32	s9, s1, s2
    e0c0: ed8c3a01     	vstr	s6, [r12, #4]
    e0c4: ee215a81     	vmul.f32	s10, s3, s2
    e0c8: ed424a05     	vstr	s9, [r2, #-20]
    e0cc: ee224a01     	vmul.f32	s8, s4, s2
    e0d0: ed025a04     	vstr	s10, [r2, #-16]
    e0d4: ee226a81     	vmul.f32	s12, s5, s2
    e0d8: ed024a03     	vstr	s8, [r2, #-12]
    e0dc: ee656a81     	vmul.f32	s13, s11, s2
    e0e0: ed026a02     	vstr	s12, [r2, #-8]
    e0e4: ed426a01     	vstr	s13, [r2, #-4]
    e0e8: 1affffd3     	bne	0xe03c <_getLayerCopyAndLibraryLoadSlot+0x114> @ imm = #-0xb4
    e0ec: e59f21d0     	ldr	r2, [pc, #0x1d0]        @ 0xe2c4 <_getLayerCopyAndLibraryLoadSlot+0x39c>  // u32=0x192b4; f32?=1.44462661e-40
    e0f0: e214c003     	ands	r12, r4, #3
    e0f4: e3a03000     	mov	r3, #0
    e0f8: eef57a00     	vmov.f32	s15, #2.500000e-01
    e0fc: e08fe002     	add	lr, pc, r2
    e100: e1a01003     	mov	r1, r3
    e104: e59e6198     	ldr	r6, [lr, #0x198]
    e108: ee211a27     	vmul.f32	s2, s2, s15
    e10c: 0a00001d     	beq	0xe188 <_getLayerCopyAndLibraryLoadSlot+0x260> @ imm = #0x74
    e110: e35c0001     	cmp	r12, #1
    e114: 0a000011     	beq	0xe160 <_getLayerCopyAndLibraryLoadSlot+0x238> @ imm = #0x44
    e118: e35c0002     	cmp	r12, #2
    e11c: 0a000007     	beq	0xe140 <_getLayerCopyAndLibraryLoadSlot+0x218> @ imm = #0x1c
    e120: ecb00a01     	vldmia	r0!, {s0}
    e124: e3a01001     	mov	r1, #1
    e128: ee700a48     	vsub.f32	s1, s0, s16
    e12c: eef01ae0     	vabs.f32	s3, s1
    e130: eeb41a61     	vcmp.f32	s2, s3
    e134: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e138: c1a06003     	movgt	r6, r3
    e13c: c1a03001     	movgt	r3, r1
    e140: ecb02a01     	vldmia	r0!, {s4}
    e144: ee722a48     	vsub.f32	s5, s4, s16
    e148: eef05ae2     	vabs.f32	s11, s5
    e14c: eeb41a65     	vcmp.f32	s2, s11
    e150: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e154: c1a06001     	movgt	r6, r1
    e158: e2811001     	add	r1, r1, #1
    e15c: c3a03001     	movgt	r3, #1
    e160: ecb07a01     	vldmia	r0!, {s14}
    e164: ee373a48     	vsub.f32	s6, s14, s16
    e168: eef03ac3     	vabs.f32	s7, s6
    e16c: eeb41a63     	vcmp.f32	s2, s7
    e170: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e174: c1a06001     	movgt	r6, r1
    e178: e2811001     	add	r1, r1, #1
    e17c: c3a03001     	movgt	r3, #1
    e180: e1540001     	cmp	r4, r1
    e184: 0a000021     	beq	0xe210 <_getLayerCopyAndLibraryLoadSlot+0x2e8> @ imm = #0x84
    e188: e1a02000     	mov	r2, r0
    e18c: edd04a01     	vldr	s9, [r0, #4]
    e190: e281e001     	add	lr, r1, #1
    e194: e2800010     	add	r0, r0, #16
    e198: ecb25a01     	vldmia	r2!, {s10}
    e19c: ee344ac8     	vsub.f32	s8, s9, s16
    e1a0: ed926a01     	vldr	s12, [r2, #4]
    e1a4: ed507a01     	vldr	s15, [r0, #-4]
    e1a8: ee350a48     	vsub.f32	s0, s10, s16
    e1ac: ee766a48     	vsub.f32	s13, s12, s16
    e1b0: eef00ac0     	vabs.f32	s1, s0
    e1b4: ee771ac8     	vsub.f32	s3, s15, s16
    e1b8: eeb41a60     	vcmp.f32	s2, s1
    e1bc: eeb02ac4     	vabs.f32	s4, s8
    e1c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e1c4: c1a06001     	movgt	r6, r1
    e1c8: c3a03001     	movgt	r3, #1
    e1cc: eeb41a42     	vcmp.f32	s2, s4
    e1d0: eef02ae6     	vabs.f32	s5, s13
    e1d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e1d8: c3a03001     	movgt	r3, #1
    e1dc: c1a0600e     	movgt	r6, lr
    e1e0: eeb41a62     	vcmp.f32	s2, s5
    e1e4: eef05ae1     	vabs.f32	s11, s3
    e1e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e1ec: c28e6001     	addgt	r6, lr, #1
    e1f0: c3a03001     	movgt	r3, #1
    e1f4: eeb41a65     	vcmp.f32	s2, s11
    e1f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e1fc: c2816003     	addgt	r6, r1, #3
    e200: e2811004     	add	r1, r1, #4
    e204: c3a03001     	movgt	r3, #1
    e208: e1540001     	cmp	r4, r1
    e20c: 1affffdd     	bne	0xe188 <_getLayerCopyAndLibraryLoadSlot+0x260> @ imm = #-0x8c
    e210: e3530000     	cmp	r3, #0
    e214: 159fc0ac     	ldrne	r12, [pc, #0xac]        @ 0xe2c8 <_getLayerCopyAndLibraryLoadSlot+0x3a0>
    e218: 108f300c     	addne	r3, pc, r12
    e21c: 15836198     	strne	r6, [r3, #0x198]
    e220: ed9f1a24     	vldr	s2, [pc, #144]          @ 0xe2b8 <_getLayerCopyAndLibraryLoadSlot+0x390>  // f32=4090
    e224: eeb48ac1     	vcmpe.f32	s16, s2
    e228: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e22c: ca000016     	bgt	0xe28c <_getLayerCopyAndLibraryLoadSlot+0x364> @ imm = #0x58
    e230: e59f4094     	ldr	r4, [pc, #0x94]         @ 0xe2cc <_getLayerCopyAndLibraryLoadSlot+0x3a4>  // u32=0x1917c; f32?=1.44025456e-40
    e234: e08f6004     	add	r6, pc, r4
    e238: e596e198     	ldr	lr, [r6, #0x198]
    e23c: ee180a90     	vmov	r0, s17
    e240: eeb18a08     	vmov.f32	s16, #6.000000e+00
    e244: ee07ea10     	vmov	s14, lr
    e248: eddf8a1b     	vldr	s17, [pc, #108]         @ 0xe2bc <_getLayerCopyAndLibraryLoadSlot+0x394>  // f32=42
    e24c: eeb83ac7     	vcvt.f32.s32	s6, s14
    e250: e3500006     	cmp	r0, #6
    e254: e1a00005     	mov	r0, r5
    e258: e59f5070     	ldr	r5, [pc, #0x70]         @ 0xe2d0 <_getLayerCopyAndLibraryLoadSlot+0x3a8>  // u32=0x19150; f32?=1.43963799e-40
    e25c: ceb08a68     	vmovgt.f32	s16, s17
    e260: e08f4005     	add	r4, pc, r5
    e264: ed843a67     	vstr	s6, [r4, #412]
    e268: ee783a43     	vsub.f32	s7, s16, s6
    e26c: eefd4ae3     	vcvt.s32.f32	s9, s7
    e270: ee14ca90     	vmov	r12, s9
    e274: e1cc3fcc     	bic	r3, r12, r12, asr #31
    e278: e6ef1073     	uxtb	r1, r3
    e27c: ebffd586     	bl	0x389c <.plt+0x1a0>     @ imm = #-0xa9e8  // CALL _setCopyOrFileSlot
    e280: ecbd8b02     	vpop	{d8}
    e284: ed940a67     	vldr	s0, [r4, #412]
    e288: e8bd8070     	pop	{r4, r5, r6, pc}
    e28c: e59f1040     	ldr	r1, [pc, #0x40]         @ 0xe2d4 <_getLayerCopyAndLibraryLoadSlot+0x3ac>  // u32=0x1911c; f32?=1.43890932e-40
    e290: e244e001     	sub	lr, r4, #1
    e294: e08f2001     	add	r2, pc, r1
    e298: e582e198     	str	lr, [r2, #0x198]
    e29c: eaffffe6     	b	0xe23c <_getLayerCopyAndLibraryLoadSlot+0x314> @ imm = #-0x68
    e2a0: e3a0e000     	mov	lr, #0
    e2a4: e28620f4     	add	r2, r6, #244
    e2a8: e3a03001     	mov	r3, #1
    e2ac: e580e000     	str	lr, [r0]
    e2b0: eaffff41     	b	0xdfbc <_getLayerCopyAndLibraryLoadSlot+0x94> @ imm = #-0x2fc
    e2b4: 00 00 2c 42  	.word	0x422c0000
    e2b8: 00 a0 7f 45  	.word	0x457fa000
    e2bc: 00 00 28 42  	.word	0x42280000
    e2c0: 38 94 01 00  	.word	0x00019438
    e2c4: b4 92 01 00  	.word	0x000192b4
    e2c8: 98 91 01 00  	.word	0x00019198
    e2cc: 7c 91 01 00  	.word	0x0001917c
    e2d0: 50 91 01 00  	.word	0x00019150
    e2d4: 1c 91 01 00  	.word	0x0001911c

