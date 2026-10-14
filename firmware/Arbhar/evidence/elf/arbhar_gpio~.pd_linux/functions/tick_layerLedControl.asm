0000ec04 <tick_layerLedControl>:
    ec04: e92d4070     	push	{r4, r5, r6, lr}
    ec08: e1a04000     	mov	r4, r0
    ec0c: e5d030b8     	ldrb	r3, [r0, #0xb8]
    ec10: e24dd020     	sub	sp, sp, #32
    ec14: e3530001     	cmp	r3, #1
    ec18: 0a000067     	beq	0xedbc <tick_layerLedControl+0x1b8> @ imm = #0x19c
    ec1c: e59f0430     	ldr	r0, [pc, #0x430]        @ 0xf054 <tick_layerLedControl+0x450>
    ec20: e08f1000     	add	r1, pc, r0
    ec24: e1c128d4     	ldrd	r2, r3, [r1, #132]
    ec28: e1520003     	cmp	r2, r3
    ec2c: ba000076     	blt	0xee0c <tick_layerLedControl+0x208> @ imm = #0x1d8
    ec30: e5d42030     	ldrb	r2, [r4, #0x30]
    ec34: e3520063     	cmp	r2, #99
    ec38: 0a00005d     	beq	0xedb4 <tick_layerLedControl+0x1b0> @ imm = #0x174
    ec3c: e5d4e03c     	ldrb	lr, [r4, #0x3c]
    ec40: e35e0003     	cmp	lr, #3
    ec44: 8a00007e     	bhi	0xee44 <tick_layerLedControl+0x240> @ imm = #0x1f8
    ec48: e35e0000     	cmp	lr, #0
    ec4c: 0a000021     	beq	0xecd8 <tick_layerLedControl+0xd4> @ imm = #0x84
    ec50: e35e0001     	cmp	lr, #1
    ec54: 0a0000df     	beq	0xefd8 <tick_layerLedControl+0x3d4> @ imm = #0x37c
    ec58: e35e0003     	cmp	lr, #3
    ec5c: 0a0000e1     	beq	0xefe8 <tick_layerLedControl+0x3e4> @ imm = #0x384
    ec60: edd47a30     	vldr	s15, [r4, #192]
    ec64: e59f33ec     	ldr	r3, [pc, #0x3ec]        @ 0xf058 <tick_layerLedControl+0x454>
    ec68: e08f1003     	add	r1, pc, r3
    ec6c: eefd0ae7     	vcvt.s32.f32	s1, s15
    ec70: e59161b4     	ldr	r6, [r1, #0x1b4]
    ec74: ee100a90     	vmov	r0, s1
    ec78: e2805018     	add	r5, r0, #24
    ec7c: e1550006     	cmp	r5, r6
    ec80: 0a0000b4     	beq	0xef58 <tick_layerLedControl+0x354> @ imm = #0x2d0
    ec84: e30acaab     	movw	r12, #0xaaab
    ec88: e342caaa     	movt	r12, #0x2aaa
    ec8c: e1a0efc5     	asr	lr, r5, #31
    ec90: e3a0000c     	mov	r0, #12
    ec94: e0c3c59c     	smull	r12, r3, r12, r5
    ec98: e06e10c3     	rsb	r1, lr, r3, asr #1
    ec9c: e0665190     	mls	r6, r0, r1, r5
    eca0: e3560000     	cmp	r6, #0
    eca4: 0a0000ad     	beq	0xef60 <tick_layerLedControl+0x35c> @ imm = #0x2b4
    eca8: e3560006     	cmp	r6, #6
    ecac: ca0000dd     	bgt	0xf028 <tick_layerLedControl+0x424> @ imm = #0x374
    ecb0: e3a01040     	mov	r1, #64
    ecb4: e2463001     	sub	r3, r6, #1
    ecb8: e1a06351     	asr	r6, r1, r3
    ecbc: e1560001     	cmp	r6, r1
    ecc0: cddf0adf     	vldrgt	s1, [pc, #892]          @ 0xf044 <tick_layerLedControl+0x440>
    ecc4: ceb07a60     	vmovgt.f32	s14, s1
    ecc8: de016a10     	vmovle	s2, r6
    eccc: dddf0adc     	vldrle	s1, [pc, #880]          @ 0xf044 <tick_layerLedControl+0x440>
    ecd0: deb87ac1     	vcvtle.f32.s32	s14, s2
    ecd4: ea0000a3     	b	0xef68 <tick_layerLedControl+0x364> @ imm = #0x28c
    ecd8: e1a00004     	mov	r0, r4
    ecdc: e2846a01     	add	r6, r4, #4096
    ece0: ebffd3f8     	bl	0x3cc8 <.plt+0x5cc>     @ imm = #-0xb020
    ece4: e300c81a     	movw	r12, #0x81a
    ece8: e19620bc     	ldrh	r2, [r6, r12]
    ecec: e1a00004     	mov	r0, r4
    ecf0: ee012a90     	vmov	s3, r2
    ecf4: eeb80a61     	vcvt.f32.u32	s0, s3
    ecf8: ebffd41c     	bl	0x3d70 <.plt+0x674>     @ imm = #-0xaf90
    ecfc: e300e81a     	movw	lr, #0x81a
    ed00: e1a00004     	mov	r0, r4
    ed04: e19630be     	ldrh	r3, [r6, lr]
    ed08: e59f534c     	ldr	r5, [pc, #0x34c]        @ 0xf05c <tick_layerLedControl+0x458>
    ed0c: ee033a10     	vmov	s6, r3
    ed10: e08f5005     	add	r5, pc, r5
    ed14: eeb80a43     	vcvt.f32.u32	s0, s6
    ed18: ebffd339     	bl	0x3a04 <.plt+0x308>     @ imm = #-0xb31c
    ed1c: e3000d82     	movw	r0, #0xd82
    ed20: e194c0b0     	ldrh	r12, [r4, r0]
    ed24: e1a00004     	mov	r0, r4
    ed28: ee03ca90     	vmov	s7, r12
    ed2c: eeb80a63     	vcvt.f32.u32	s0, s7
    ed30: ebffd339     	bl	0x3a1c <.plt+0x320>     @ imm = #-0xb31c
    ed34: e3001672     	movw	r1, #0x672
    ed38: e19420b1     	ldrh	r2, [r4, r1]
    ed3c: e1a00004     	mov	r0, r4
    ed40: ee042a10     	vmov	s8, r2
    ed44: eeb80a44     	vcvt.f32.u32	s0, s8
    ed48: ebffd273     	bl	0x371c <.plt+0x20>      @ imm = #-0xb634
    ed4c: e30039de     	movw	r3, #0x9de
    ed50: e19660b3     	ldrh	r6, [r6, r3]
    ed54: e1a00004     	mov	r0, r4
    ed58: ee046a90     	vmov	s9, r6
    ed5c: eeb80a64     	vcvt.f32.u32	s0, s9
    ed60: ebffd375     	bl	0x3b3c <.plt+0x440>     @ imm = #-0xb22c
    ed64: e59501b8     	ldr	r0, [r5, #0x1b8]
    ed68: e280c001     	add	r12, r0, #1
    ed6c: e585c1b8     	str	r12, [r5, #0x1b8]
    ed70: e35c0005     	cmp	r12, #5
    ed74: da000008     	ble	0xed9c <tick_layerLedControl+0x198> @ imm = #0x20
    ed78: eeb70a00     	vmov.f32	s0, #1.000000e+00
    ed7c: e5d4e069     	ldrb	lr, [r4, #0x69]
    ed80: ed9f5aaf     	vldr	s10, [pc, #700]         @ 0xf044 <tick_layerLedControl+0x440>
    ed84: e1a00004     	mov	r0, r4
    ed88: e35e0003     	cmp	lr, #3
    ed8c: 9eb00a45     	vmovls.f32	s0, s10
    ed90: ebffe00f     	bl	0x6dd4 <led_menuPagesIndicator> @ imm = #-0x7fc4
    ed94: e3a01000     	mov	r1, #0
    ed98: e58511b8     	str	r1, [r5, #0x1b8]
    ed9c: e5d4503c     	ldrb	r5, [r4, #0x3c]
    eda0: e3550003     	cmp	r5, #3
    eda4: 8a000002     	bhi	0xedb4 <tick_layerLedControl+0x1b0> @ imm = #0x8
    eda8: e59400d8     	ldr	r0, [r4, #0xd8]
    edac: eeb20b0e     	vmov.f64	d0, #1.500000e+01
    edb0: ebffd2dd     	bl	0x392c <.plt+0x230>     @ imm = #-0xb48c
    edb4: e28dd020     	add	sp, sp, #32
    edb8: e8bd8070     	pop	{r4, r5, r6, pc}
    edbc: e59f229c     	ldr	r2, [pc, #0x29c]        @ 0xf060 <tick_layerLedControl+0x45c>
    edc0: e3a05000     	mov	r5, #0
    edc4: e59f6298     	ldr	r6, [pc, #0x298]        @ 0xf064 <tick_layerLedControl+0x460>
    edc8: e08f3002     	add	r3, pc, r2
    edcc: e59f0294     	ldr	r0, [pc, #0x294]        @ 0xf068 <tick_layerLedControl+0x464>
    edd0: e08fc006     	add	r12, pc, r6
    edd4: e3a06000     	mov	r6, #0
    edd8: e59311ac     	ldr	r1, [r3, #0x1ac]
    eddc: e1a02006     	mov	r2, r6
    ede0: e58c5084     	str	r5, [r12, #0x84]
    ede4: e1510005     	cmp	r1, r5
    ede8: e08f5000     	add	r5, pc, r0
    edec: e5c460b8     	strb	r6, [r4, #0xb8]
    edf0: d59c3088     	ldrle	r3, [r12, #0x88]
    edf4: c3a03064     	movgt	r3, #100
    edf8: c58c3088     	strgt	r3, [r12, #0x88]
    edfc: e281c001     	add	r12, r1, #1
    ee00: e1520003     	cmp	r2, r3
    ee04: e585c1ac     	str	r12, [r5, #0x1ac]
    ee08: aaffff88     	bge	0xec30 <tick_layerLedControl+0x2c> @ imm = #-0x1e0
    ee0c: e59420b4     	ldr	r2, [r4, #0xb4]
    ee10: e1a00004     	mov	r0, r4
    ee14: e5d21000     	ldrb	r1, [r2]
    ee18: ebffd34d     	bl	0x3b54 <.plt+0x458>     @ imm = #-0xb2cc
    ee1c: e59f3248     	ldr	r3, [pc, #0x248]        @ 0xf06c <tick_layerLedControl+0x468>
    ee20: e59400d8     	ldr	r0, [r4, #0xd8]
    ee24: eeb20b0e     	vmov.f64	d0, #1.500000e+01
    ee28: e08f6003     	add	r6, pc, r3
    ee2c: e5964084     	ldr	r4, [r6, #0x84]
    ee30: e284c001     	add	r12, r4, #1
    ee34: e586c084     	str	r12, [r6, #0x84]
    ee38: ebffd2bb     	bl	0x392c <.plt+0x230>     @ imm = #-0xb514
    ee3c: e28dd020     	add	sp, sp, #32
    ee40: e8bd8070     	pop	{r4, r5, r6, pc}
    ee44: e1a00004     	mov	r0, r4
    ee48: ebffd30e     	bl	0x3a88 <.plt+0x38c>     @ imm = #-0xb3c8
    ee4c: e1a00004     	mov	r0, r4
    ee50: eebd2ac0     	vcvt.s32.f32	s4, s0
    ee54: ee123a10     	vmov	r3, s4
    ee58: eeb80ac2     	vcvt.f32.s32	s0, s4
    ee5c: e263602a     	rsb	r6, r3, #42
    ee60: ebffd311     	bl	0x3aac <.plt+0x3b0>     @ imm = #-0xb3bc
    ee64: e59f0204     	ldr	r0, [pc, #0x204]        @ 0xf070 <tick_layerLedControl+0x46c>
    ee68: e08f1000     	add	r1, pc, r0
    ee6c: e591508c     	ldr	r5, [r1, #0x8c]
    ee70: e1560005     	cmp	r6, r5
    ee74: 0a00004f     	beq	0xefb8 <tick_layerLedControl+0x3b4> @ imm = #0x13c
    ee78: e1a01006     	mov	r1, r6
    ee7c: e1a00004     	mov	r0, r4
    ee80: ebffd279     	bl	0x386c <.plt+0x170>     @ imm = #-0xb61c
    ee84: e59fe1e8     	ldr	lr, [pc, #0x1e8]        @ 0xf074 <tick_layerLedControl+0x470>
    ee88: e1a00004     	mov	r0, r4
    ee8c: ed9f0a6d     	vldr	s0, [pc, #436]          @ 0xf048 <tick_layerLedControl+0x444>
    ee90: e08fc00e     	add	r12, pc, lr
    ee94: e58c608c     	str	r6, [r12, #0x8c]
    ee98: ebffd252     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xb6b8
    ee9c: eeb50ac0     	vcmpe.f32	s0, #0
    eea0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    eea4: da000007     	ble	0xeec8 <tick_layerLedControl+0x2c4> @ imm = #0x1c
    eea8: e1a00004     	mov	r0, r4
    eeac: e2846a01     	add	r6, r4, #4096
    eeb0: ebffd2a3     	bl	0x3944 <.plt+0x248>     @ imm = #-0xb574
    eeb4: e59fc1bc     	ldr	r12, [pc, #0x1bc]       @ 0xf078 <tick_layerLedControl+0x474>
    eeb8: e3a02001     	mov	r2, #1
    eebc: e08f100c     	add	r1, pc, r12
    eec0: e58121b0     	str	r2, [r1, #0x1b0]
    eec4: eaffff8c     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x1d0
    eec8: ed9f0a5f     	vldr	s0, [pc, #380]          @ 0xf04c <tick_layerLedControl+0x448>
    eecc: e1a00004     	mov	r0, r4
    eed0: ebffd244     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xb6f0
    eed4: eeb50ac0     	vcmpe.f32	s0, #0
    eed8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    eedc: cafffff1     	bgt	0xeea8 <tick_layerLedControl+0x2a4> @ imm = #-0x3c
    eee0: e59f2194     	ldr	r2, [pc, #0x194]        @ 0xf07c <tick_layerLedControl+0x478>
    eee4: e08f5002     	add	r5, pc, r2
    eee8: e59531b0     	ldr	r3, [r5, #0x1b0]
    eeec: e3530000     	cmp	r3, #0
    eef0: 0a000018     	beq	0xef58 <tick_layerLedControl+0x354> @ imm = #0x60
    eef4: e59f6184     	ldr	r6, [pc, #0x184]        @ 0xf080 <tick_layerLedControl+0x47c>
    eef8: e08f0006     	add	r0, pc, r6
    eefc: e2846a01     	add	r6, r4, #4096
    ef00: ebffd31c     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xb390
    ef04: e59f0178     	ldr	r0, [pc, #0x178]        @ 0xf084 <tick_layerLedControl+0x480>
    ef08: e3a03001     	mov	r3, #1
    ef0c: e3a02000     	mov	r2, #0
    ef10: e08f0000     	add	r0, pc, r0
    ef14: e58521b0     	str	r2, [r5, #0x1b0]
    ef18: e58d3000     	str	r3, [sp]
    ef1c: e3a0c000     	mov	r12, #0
    ef20: e5965dac     	ldr	r5, [r6, #0xdac]
    ef24: e344c190     	movt	r12, #0x4190
    ef28: e58d3008     	str	r3, [sp, #0x8]
    ef2c: e3a01000     	mov	r1, #0
    ef30: e58dc004     	str	r12, [sp, #0x4]
    ef34: e344130f     	movt	r1, #0x430f
    ef38: e58d100c     	str	r1, [sp, #0xc]
    ef3c: ebffd1f9     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb81c
    ef40: e1a0300d     	mov	r3, sp
    ef44: e3a02002     	mov	r2, #2
    ef48: e1a01000     	mov	r1, r0
    ef4c: e1a00005     	mov	r0, r5
    ef50: ebffd347     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xb2e4
    ef54: eaffff68     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x260
    ef58: e2846a01     	add	r6, r4, #4096
    ef5c: eaffff66     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x268
    ef60: eef70a00     	vmov.f32	s1, #1.000000e+00
    ef64: eeb07a60     	vmov.f32	s14, s1
    ef68: e3520062     	cmp	r2, #98
    ef6c: e3a0e001     	mov	lr, #1
    ef70: e3a0c000     	mov	r12, #0
    ef74: e3a02000     	mov	r2, #0
    ef78: e344c312     	movt	r12, #0x4312
    ef7c: e3442313     	movt	r2, #0x4313
    ef80: ed8d7a01     	vstr	s14, [sp, #4]
    ef84: e58dc00c     	str	r12, [sp, #0xc]
    ef88: e58de000     	str	lr, [sp]
    ef8c: edcd0a05     	vstr	s1, [sp, #20]
    ef90: e58de008     	str	lr, [sp, #0x8]
    ef94: e58d201c     	str	r2, [sp, #0x1c]
    ef98: e58de010     	str	lr, [sp, #0x10]
    ef9c: e58de018     	str	lr, [sp, #0x18]
    efa0: 9a000016     	bls	0xf000 <tick_layerLedControl+0x3fc> @ imm = #0x58
    efa4: e59f30dc     	ldr	r3, [pc, #0xdc]         @ 0xf088 <tick_layerLedControl+0x484>
    efa8: e2846a01     	add	r6, r4, #4096
    efac: e08f1003     	add	r1, pc, r3
    efb0: e58151b4     	str	r5, [r1, #0x1b4]
    efb4: eaffff50     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x2c0
    efb8: ed9f0a24     	vldr	s0, [pc, #144]          @ 0xf050 <tick_layerLedControl+0x44c>
    efbc: e1a00004     	mov	r0, r4
    efc0: ebffd208     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xb7e0
    efc4: eef72a00     	vmov.f32	s5, #1.000000e+00
    efc8: eeb40a62     	vcmp.f32	s0, s5
    efcc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    efd0: 0affffa8     	beq	0xee78 <tick_layerLedControl+0x274> @ imm = #-0x160
    efd4: eaffffaa     	b	0xee84 <tick_layerLedControl+0x280> @ imm = #-0x158
    efd8: e1a00004     	mov	r0, r4
    efdc: e2846a01     	add	r6, r4, #4096
    efe0: ebffd338     	bl	0x3cc8 <.plt+0x5cc>     @ imm = #-0xb320
    efe4: eaffff44     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x2f0
    efe8: e2846a01     	add	r6, r4, #4096
    efec: e3005492     	movw	r5, #0x492
    eff0: e1a00004     	mov	r0, r4
    eff4: e19610b5     	ldrh	r1, [r6, r5]
    eff8: ebffd356     	bl	0x3d58 <.plt+0x65c>     @ imm = #-0xb2a8
    effc: eaffff3e     	b	0xecfc <tick_layerLedControl+0xf8> @ imm = #-0x308
    f000: e59f0084     	ldr	r0, [pc, #0x84]         @ 0xf08c <tick_layerLedControl+0x488>
    f004: e5946070     	ldr	r6, [r4, #0x70]
    f008: e08f0000     	add	r0, pc, r0
    f00c: ebffd1c5     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb8ec
    f010: e1a0300d     	mov	r3, sp
    f014: e3a02004     	mov	r2, #4
    f018: e1a01000     	mov	r1, r0
    f01c: e1a00006     	mov	r0, r6
    f020: ebffd313     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xb3b4
    f024: eaffffde     	b	0xefa4 <tick_layerLedControl+0x3a0> @ imm = #-0x88
    f028: e266c00c     	rsb	r12, r6, #12
    f02c: e3a0e001     	mov	lr, #1
    f030: e1a00c1e     	lsl	r0, lr, r12
    f034: ed9f7a02     	vldr	s14, [pc, #8]           @ 0xf044 <tick_layerLedControl+0x440>
    f038: ee000a10     	vmov	s0, r0
    f03c: eef80ac0     	vcvt.f32.s32	s1, s0
    f040: eaffffc8     	b	0xef68 <tick_layerLedControl+0x364> @ imm = #-0xe0
    f044: 00 00 00 00  	.word	0x00000000
    f048: 00 00 f6 42  	.word	0x42f60000
    f04c: 00 00 67 43  	.word	0x43670000
    f050: 00 00 30 42  	.word	0x42300000
    f054: c4 86 01 00  	.word	0x000186c4
    f058: 48 87 01 00  	.word	0x00018748
    f05c: a0 86 01 00  	.word	0x000186a0
    f060: e8 85 01 00  	.word	0x000185e8
    f064: 14 85 01 00  	.word	0x00018514
    f068: c8 85 01 00  	.word	0x000185c8
    f06c: bc 84 01 00  	.word	0x000184bc
    f070: 7c 84 01 00  	.word	0x0001847c
    f074: 54 84 01 00  	.word	0x00018454
    f078: f4 84 01 00  	.word	0x000184f4
    f07c: cc 84 01 00  	.word	0x000184cc
    f080: 08 6b 00 00  	.word	0x00006b08
    f084: a4 5b 00 00  	.word	0x00005ba4
    f088: 04 84 01 00  	.word	0x00018404
    f08c: ac 5a 00 00  	.word	0x00005aac

