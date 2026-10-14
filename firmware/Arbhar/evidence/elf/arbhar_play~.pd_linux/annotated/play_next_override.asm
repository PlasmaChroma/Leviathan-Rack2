0000393c <play_next_override>:
    393c: e3520008     	cmp	r2, #8
    3940: d12fff1e     	bxle	lr
    3944: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    3948: e1a0a102     	lsl	r10, r2, #2
    394c: ed2d8b0a     	vpush	{d8, d9, d10, d11, d12}
    3950: e1a08000     	mov	r8, r0
    3954: e1a0000a     	mov	r0, r10
    3958: e0839182     	add	r9, r3, r2, lsl #3
    395c: e1a07002     	mov	r7, r2
    3960: e1a04003     	mov	r4, r3
    3964: e24dd00c     	sub	sp, sp, #12
    3968: ebfffac5     	bl	0x2484 <.plt+0xa4>      @ imm = #-0x14ec  // CALL getbytes
    396c: e0493004     	sub	r3, r9, r4
    3970: e1a06000     	mov	r6, r0
    3974: e2405004     	sub	r5, r0, #4
    3978: e2430008     	sub	r0, r3, #8
    397c: e1a011a0     	lsr	r1, r0, #3
    3980: e2812001     	add	r2, r1, #1
    3984: e212b007     	ands	r11, r2, #7
    3988: 0a000025     	beq	0x3a24 <play_next_override+0xe8> @ imm = #0x94
    398c: e35b0001     	cmp	r11, #1
    3990: 0a00001d     	beq	0x3a0c <play_next_override+0xd0> @ imm = #0x74
    3994: e35b0002     	cmp	r11, #2
    3998: 0a000017     	beq	0x39fc <play_next_override+0xc0> @ imm = #0x5c
    399c: e35b0003     	cmp	r11, #3
    39a0: 0a000011     	beq	0x39ec <play_next_override+0xb0> @ imm = #0x44
    39a4: e35b0004     	cmp	r11, #4
    39a8: 0a00000b     	beq	0x39dc <play_next_override+0xa0> @ imm = #0x2c
    39ac: e35b0005     	cmp	r11, #5
    39b0: 0a000005     	beq	0x39cc <play_next_override+0x90> @ imm = #0x14
    39b4: e35b0006     	cmp	r11, #6
    39b8: 1a0000d5     	bne	0x3d14 <play_next_override+0x3d8> @ imm = #0x354
    39bc: e1a00004     	mov	r0, r4
    39c0: e2844008     	add	r4, r4, #8
    39c4: ebfffb0b     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x13d4  // CALL atom_getint
    39c8: e5a50004     	str	r0, [r5, #0x4]!
    39cc: e1a00004     	mov	r0, r4
    39d0: e2844008     	add	r4, r4, #8
    39d4: ebfffb07     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x13e4  // CALL atom_getint
    39d8: e5a50004     	str	r0, [r5, #0x4]!
    39dc: e1a00004     	mov	r0, r4
    39e0: e2844008     	add	r4, r4, #8
    39e4: ebfffb03     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x13f4  // CALL atom_getint
    39e8: e5a50004     	str	r0, [r5, #0x4]!
    39ec: e1a00004     	mov	r0, r4
    39f0: e2844008     	add	r4, r4, #8
    39f4: ebfffaff     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1404  // CALL atom_getint
    39f8: e5a50004     	str	r0, [r5, #0x4]!
    39fc: e1a00004     	mov	r0, r4
    3a00: e2844008     	add	r4, r4, #8
    3a04: ebfffafb     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1414  // CALL atom_getint
    3a08: e5a50004     	str	r0, [r5, #0x4]!
    3a0c: e1a00004     	mov	r0, r4
    3a10: e2844008     	add	r4, r4, #8
    3a14: ebfffaf7     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1424  // CALL atom_getint
    3a18: e1590004     	cmp	r9, r4
    3a1c: e5a50004     	str	r0, [r5, #0x4]!
    3a20: 0a00001e     	beq	0x3aa0 <play_next_override+0x164> @ imm = #0x78
    3a24: e1a00004     	mov	r0, r4
    3a28: e284b008     	add	r11, r4, #8
    3a2c: ebfffaf1     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x143c  // CALL atom_getint
    3a30: e285c004     	add	r12, r5, #4
    3a34: e58dc004     	str	r12, [sp, #0x4]
    3a38: e5850004     	str	r0, [r5, #0x4]
    3a3c: e1a0000b     	mov	r0, r11
    3a40: ebfffaec     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1450  // CALL atom_getint
    3a44: e59d3004     	ldr	r3, [sp, #0x4]
    3a48: e5830004     	str	r0, [r3, #0x4]
    3a4c: e2840010     	add	r0, r4, #16
    3a50: ebfffae8     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1460  // CALL atom_getint
    3a54: e585000c     	str	r0, [r5, #0xc]
    3a58: e2840018     	add	r0, r4, #24
    3a5c: ebfffae5     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x146c  // CALL atom_getint
    3a60: e5850010     	str	r0, [r5, #0x10]
    3a64: e2840020     	add	r0, r4, #32
    3a68: ebfffae2     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1478  // CALL atom_getint
    3a6c: e5850014     	str	r0, [r5, #0x14]
    3a70: e2840028     	add	r0, r4, #40
    3a74: ebfffadf     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1484  // CALL atom_getint
    3a78: e5850018     	str	r0, [r5, #0x18]
    3a7c: e2840030     	add	r0, r4, #48
    3a80: ebfffadc     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x1490  // CALL atom_getint
    3a84: e585001c     	str	r0, [r5, #0x1c]
    3a88: e2840038     	add	r0, r4, #56
    3a8c: ebfffad9     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x149c  // CALL atom_getint
    3a90: e2844040     	add	r4, r4, #64
    3a94: e1590004     	cmp	r9, r4
    3a98: e5a50020     	str	r0, [r5, #0x20]!
    3a9c: 1affffe0     	bne	0x3a24 <play_next_override+0xe8> @ imm = #-0x80
    3aa0: edd67a05     	vldr	s15, [r6, #20]
    3aa4: e596e000     	ldr	lr, [r6]
    3aa8: e5969008     	ldr	r9, [r6, #0x8]
    3aac: ed9f7aa5     	vldr	s14, [pc, #660]         @ 0x3d48 <play_next_override+0x40c>  // f32=61
    3ab0: e5960004     	ldr	r0, [r6, #0x4]
    3ab4: e596200c     	ldr	r2, [r6, #0xc]
    3ab8: eeb81ae7     	vcvt.f32.s32	s2, s15
    3abc: e180138e     	orr	r1, r0, lr, lsl #7
    3ac0: e1825389     	orr	r5, r2, r9, lsl #7
    3ac4: ed9f0b9b     	vldr	d0, [pc, #620]          @ 0x3d38 <play_next_override+0x3fc>  // f64=0.083333333333333329
    3ac8: e596b010     	ldr	r11, [r6, #0x10]
    3acc: ee095a90     	vmov	s19, r5
    3ad0: ee021a90     	vmov	s5, r1
    3ad4: ed9f2a9c     	vldr	s4, [pc, #624]          @ 0x3d4c <play_next_override+0x410>  // f32=8.78959846
    3ad8: ee711a47     	vsub.f32	s3, s2, s14
    3adc: eddfba9b     	vldr	s23, [pc, #620]         @ 0x3d50 <play_next_override+0x414>  // f32=29.2986641
    3ae0: ed9f9a9b     	vldr	s18, [pc, #620]         @ 0x3d54 <play_next_override+0x418>  // f32=0.0118110236
    3ae4: eef70ae1     	vcvt.f64.f32	d16, s3
    3ae8: eeb83ae2     	vcvt.f32.s32	s6, s5
    3aec: eef83ae9     	vcvt.f32.s32	s7, s19
    3af0: ee200b80     	vmul.f64	d0, d16, d0
    3af4: ee63ba2b     	vmul.f32	s23, s6, s23
    3af8: ee639a82     	vmul.f32	s19, s7, s4
    3afc: ebfffaf9     	bl	0x26e8 <.plt+0x308>     @ imm = #-0x141c  // CALL __exp2_finite
    3b00: edd64a06     	vldr	s9, [r6, #24]
    3b04: e3570009     	cmp	r7, #9
    3b08: ed965a07     	vldr	s10, [r6, #28]
    3b0c: ed968a08     	vldr	s16, [r6, #32]
    3b10: eddf5a90     	vldr	s11, [pc, #576]         @ 0x3d58 <play_next_override+0x41c>  // f32=0.0157480314
    3b14: eeb74a00     	vmov.f32	s8, #1.000000e+00
    3b18: eeb86ae4     	vcvt.f32.s32	s12, s9
    3b1c: eef86ac5     	vcvt.f32.s32	s13, s10
    3b20: eef88ac8     	vcvt.f32.s32	s17, s16
    3b24: eeb0ba44     	vmov.f32	s22, s8
    3b28: eeb0ca44     	vmov.f32	s24, s8
    3b2c: eef7cbc0     	vcvt.f32.f64	s25, d0
    3b30: ee16ba25     	vnmls.f32	s22, s12, s11
    3b34: ee16caa5     	vnmls.f32	s24, s13, s11
    3b38: ee289a89     	vmul.f32	s18, s17, s18
    3b3c: 0a000015     	beq	0x3b98 <play_next_override+0x25c> @ imm = #0x54
    3b40: ed96aa09     	vldr	s20, [r6, #36]
    3b44: e357000a     	cmp	r7, #10
    3b48: eddf0a83     	vldr	s1, [pc, #524]          @ 0x3d5c <play_next_override+0x420>  // f32=0.00787401572
    3b4c: eef8aaca     	vcvt.f32.s32	s21, s20
    3b50: ee2a8aa0     	vmul.f32	s16, s21, s1
    3b54: 0a000074     	beq	0x3d2c <play_next_override+0x3f0> @ imm = #0x1d0
    3b58: ed967a0a     	vldr	s14, [r6, #40]
    3b5c: e357000b     	cmp	r7, #11
    3b60: eef87ac7     	vcvt.f32.s32	s15, s14
    3b64: ee27aaa0     	vmul.f32	s20, s15, s1
    3b68: da000070     	ble	0x3d30 <play_next_override+0x3f4> @ imm = #0x1c0
    3b6c: ed960a0b     	vldr	s0, [r6, #44]
    3b70: ed9f1a7a     	vldr	s2, [pc, #488]          @ 0x3d60 <play_next_override+0x424>  // f32=3.6666243e-12
    3b74: eef81ac0     	vcvt.f32.s32	s3, s0
    3b78: ee61aa81     	vmul.f32	s21, s3, s2
    3b7c: e357000c     	cmp	r7, #12
    3b80: da000007     	ble	0x3ba4 <play_next_override+0x268> @ imm = #0x1c
    3b84: edd62a0c     	vldr	s5, [r6, #48]
    3b88: ed9f2a73     	vldr	s4, [pc, #460]          @ 0x3d5c <play_next_override+0x420>  // f32=0.00787401572
    3b8c: eeb83ae2     	vcvt.f32.s32	s6, s5
    3b90: ee638a02     	vmul.f32	s17, s6, s4
    3b94: ea000003     	b	0x3ba8 <play_next_override+0x26c> @ imm = #0xc
    3b98: eeb6aa00     	vmov.f32	s20, #5.000000e-01
    3b9c: eddfaa70     	vldr	s21, [pc, #448]         @ 0x3d64 <play_next_override+0x428>  // f32=0
    3ba0: eeb08a44     	vmov.f32	s16, s8
    3ba4: eddf8a6e     	vldr	s17, [pc, #440]         @ 0x3d64 <play_next_override+0x428>  // f32=0
    3ba8: e1a0100a     	mov	r1, r10
    3bac: e1a00006     	mov	r0, r6
    3bb0: ebfffac9     	bl	0x26dc <.plt+0x2fc>     @ imm = #-0x14dc  // CALL freebytes
    3bb4: e2887a02     	add	r7, r8, #8192
    3bb8: e3a0c001     	mov	r12, #1
    3bbc: e1d83af0     	ldrsh	r3, [r8, #160]
    3bc0: e597a6e0     	ldr	r10, [r7, #0x6e0]
    3bc4: e3a06074     	mov	r6, #116
    3bc8: eefd5acb     	vcvt.s32.f32	s11, s22
    3bcc: e3a00074     	mov	r0, #116
    3bd0: e08a400c     	add	r4, r10, r12
    3bd4: e3540052     	cmp	r4, #82
    3bd8: a3a04000     	movge	r4, #0
    3bdc: e3530000     	cmp	r3, #0
    3be0: e0298496     	mla	r9, r6, r4, r8
    3be4: e3a06074     	mov	r6, #116
    3be8: 1ef73a00     	vmovne.f32	s7, #1.000000e+00
    3bec: e0218490     	mla	r1, r0, r4, r8
    3bf0: 1d89aa4e     	vstrne	s20, [r9, #312]
    3bf4: e589c12c     	str	r12, [r9, #0x12c]
    3bf8: ed899a47     	vstr	s18, [r9, #284]
    3bfc: eeb79a00     	vmov.f32	s18, #1.000000e+00
    3c00: 0d89aa4c     	vstreq	s20, [r9, #304]
    3c04: ed898a48     	vstr	s16, [r9, #288]
    3c08: eefdbaeb     	vcvt.s32.f32	s23, s23
    3c0c: 1e33aaca     	vsubne.f32	s20, s7, s20
    3c10: eefd9ae9     	vcvt.s32.f32	s19, s19
    3c14: 1d89aa4d     	vstrne	s20, [r9, #308]
    3c18: e3a09001     	mov	r9, #1
    3c1c: e581b13c     	str	r11, [r1, #0x13c]
    3c20: e3a0b000     	mov	r11, #0
    3c24: edc1ca46     	vstr	s25, [r1, #280]
    3c28: ed81ca51     	vstr	s24, [r1, #324]
    3c2c: ed819a55     	vstr	s18, [r1, #340]
    3c30: edc1ba43     	vstr	s23, [r1, #268]
    3c34: edc19a42     	vstr	s19, [r1, #264]
    3c38: edc15a50     	vstr	s11, [r1, #320]
    3c3c: ebfffaa0     	bl	0x26c4 <.plt+0x2e4>     @ imm = #-0x1580  // CALL random
    3c40: e3a02074     	mov	r2, #116
    3c44: e0258492     	mla	r5, r2, r4, r8
    3c48: ed98ba35     	vldr	s22, [r8, #212]
    3c4c: e287ce6b     	add	r12, r7, #1712
    3c50: e0288496     	mla	r8, r6, r4, r8
    3c54: e3a03000     	mov	r3, #0
    3c58: eddf1b38     	vldr	d17, [pc, #224]         @ 0x3d40 <play_next_override+0x404>  // f64=0.10000000000000001
    3c5c: e595a140     	ldr	r10, [r5, #0x140]
    3c60: eeb8cacb     	vcvt.f32.s32	s24, s22
    3c64: e585b164     	str	r11, [r5, #0x164]
    3c68: ed988a42     	vldr	s16, [r8, #264]
    3c6c: e585b168     	str	r11, [r5, #0x168]
    3c70: ed955a46     	vldr	s10, [r5, #280]
    3c74: eeb8aac8     	vcvt.f32.s32	s20, s16
    3c78: eddc0a02     	vldr	s1, [r12, #8]
    3c7c: ee8c7a0a     	vdiv.f32	s14, s24, s20
    3c80: ee040a10     	vmov	s8, r0
    3c84: e3a00008     	mov	r0, #8
    3c88: eef84ac4     	vcvt.f32.s32	s9, s8
    3c8c: ee448aaa     	vmla.f32	s17, s9, s21
    3c90: eef72ac5     	vcvt.f64.f32	d18, s10
    3c94: ee723ba1     	vadd.f64	d19, d18, d17
    3c98: eeb76a00     	vmov.f32	s12, #1.000000e+00
    3c9c: eef48ac9     	vcmpe.f32	s17, s18
    3ca0: eef7cbe3     	vcvt.f32.f64	s25, d19
    3ca4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3ca8: cef04a00     	vmovgt.f32	s9, #2.000000e+00
    3cac: eef4cac6     	vcmpe.f32	s25, s12
    3cb0: ce748ae8     	vsubgt.f32	s17, s9, s17
    3cb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3cb8: 8ef0ca46     	vmovhi.f32	s25, s12
    3cbc: e37a0001     	cmn	r10, #1
    3cc0: edc58a5d     	vstr	s17, [r5, #372]
    3cc4: 03a0b903     	moveq	r11, #49152
    3cc8: 0344b400     	movteq	r11, #0x4400
    3ccc: ee677a20     	vmul.f32	s15, s14, s1
    3cd0: edc5ca5b     	vstr	s25, [r5, #364]
    3cd4: edc5ca5c     	vstr	s25, [r5, #368]
    3cd8: ee756a06     	vadd.f32	s13, s10, s12
    3cdc: 0d956a42     	vldreq	s12, [r5, #264]
    3ce0: 1e06ba10     	vmovne	s12, r11
    3ce4: 0eb86ac6     	vcvteq.f32.s32	s12, s12
    3ce8: edc56a54     	vstr	s13, [r5, #336]
    3cec: e588b128     	str	r11, [r8, #0x128]
    3cf0: e588915c     	str	r9, [r8, #0x15c]
    3cf4: e5880160     	str	r0, [r8, #0x160]
    3cf8: e5883114     	str	r3, [r8, #0x114]
    3cfc: edc87a52     	vstr	s15, [r8, #328]
    3d00: ed886a49     	vstr	s12, [r8, #292]
    3d04: e58746e0     	str	r4, [r7, #0x6e0]
    3d08: e28dd00c     	add	sp, sp, #12
    3d0c: ecbd8b0a     	vpop	{d8, d9, d10, d11, d12}
    3d10: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3d14: e1a00004     	mov	r0, r4
    3d18: e1a05006     	mov	r5, r6
    3d1c: ebfffa35     	bl	0x25f8 <.plt+0x218>     @ imm = #-0x172c  // CALL atom_getint
    3d20: e2844008     	add	r4, r4, #8
    3d24: e5860000     	str	r0, [r6]
    3d28: eaffff23     	b	0x39bc <play_next_override+0x80> @ imm = #-0x374
    3d2c: eeb6aa00     	vmov.f32	s20, #5.000000e-01
    3d30: eddfaa0b     	vldr	s21, [pc, #44]          @ 0x3d64 <play_next_override+0x428>  // f32=0
    3d34: eaffff90     	b	0x3b7c <play_next_override+0x240> @ imm = #-0x1c0
    3d38: 55 55 55 55  	.word	0x55555555
    3d3c: 55 55 b5 3f  	.word	0x3fb55555
    3d40: 9a 99 99 99  	.word	0x9999999a
    3d44: 99 99 b9 3f  	.word	0x3fb99999
    3d48: 00 00 74 42  	.word	0x42740000
    3d4c: 32 a2 0c 41  	.word	0x410ca232
    3d50: aa 63 ea 41  	.word	0x41ea63aa
    3d54: 06 83 41 3c  	.word	0x3c418306
    3d58: 04 02 81 3c  	.word	0x3c810204
    3d5c: 04 02 01 3c  	.word	0x3c010204
    3d60: 04 02 81 2c  	.word	0x2c810204
    3d64: 00 00 00 00  	.word	0x00000000

