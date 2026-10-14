00001ccc <fftwObject_tilde_attack>:
    1ccc: ed9f6add     	vldr	s12, [pc, #884]         @ 0x2048 <fftwObject_tilde_attack+0x37c>  // f32=0
    1cd0: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1cd4: e2803866     	add	r3, r0, #6684672
    1cd8: eeb77a00     	vmov.f32	s14, #1.000000e+00
    1cdc: e1a07000     	mov	r7, r0
    1ce0: e5936d10     	ldr	r6, [r3, #0xd10]
    1ce4: e2832a01     	add	r2, r3, #4096
    1ce8: ed2d8b08     	vpush	{d8, d9, d10, d11}
    1cec: ee076a90     	vmov	s15, r6
    1cf0: edd26a11     	vldr	s13, [r2, #68]
    1cf4: ed929a12     	vldr	s18, [r2, #72]
    1cf8: eeb40ac6     	vcmpe.f32	s0, s12
    1cfc: ed928a13     	vldr	s16, [r2, #76]
    1d00: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1d04: beb00a46     	vmovlt.f32	s0, s12
    1d08: eeb40ac7     	vcmpe.f32	s0, s14
    1d0c: eef80ae7     	vcvt.f32.s32	s1, s15
    1d10: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1d14: 8eb00a47     	vmovhi.f32	s0, s14
    1d18: ee200a20     	vmul.f32	s0, s0, s1
    1d1c: eebd1ac0     	vcvt.s32.f32	s2, s0
    1d20: ee110a10     	vmov	r0, s2
    1d24: e0461000     	sub	r1, r6, r0
    1d28: e1560000     	cmp	r6, r0
    1d2c: ee011a90     	vmov	s3, r1
    1d30: b1a0a006     	movlt	r10, r6
    1d34: a1a0a000     	movge	r10, r0
    1d38: e582a03c     	str	r10, [r2, #0x3c]
    1d3c: eeb82ae1     	vcvt.f32.s32	s4, s3
    1d40: ee622a26     	vmul.f32	s5, s4, s13
    1d44: eebd3ae2     	vcvt.s32.f32	s6, s5
    1d48: ee134a10     	vmov	r4, s6
    1d4c: e1560004     	cmp	r6, r4
    1d50: b1a01006     	movlt	r1, r6
    1d54: a1a01004     	movge	r1, r4
    1d58: e35a0000     	cmp	r10, #0
    1d5c: e5821040     	str	r1, [r2, #0x40]
    1d60: e0466001     	sub	r6, r6, r1
    1d64: da0000ec     	ble	0x211c <fftwObject_tilde_attack+0x450> @ imm = #0x3b0
    1d68: eeb84ac1     	vcvt.f32.s32	s8, s2
    1d6c: e21a9003     	ands	r9, r10, #3
    1d70: e3005d1c     	movw	r5, #0xd1c
    1d74: e3405066     	movt	r5, #0x66
    1d78: e0875005     	add	r5, r7, r5
    1d7c: e3a08000     	mov	r8, #0
    1d80: eec78a04     	vdiv.f32	s17, s14, s8
    1d84: eeb7aac9     	vcvt.f64.f32	d10, s18
    1d88: 0a00001d     	beq	0x1e04 <fftwObject_tilde_attack+0x138> @ imm = #0x74
    1d8c: e3590001     	cmp	r9, #1
    1d90: 0a000010     	beq	0x1dd8 <fftwObject_tilde_attack+0x10c> @ imm = #0x40
    1d94: e3590002     	cmp	r9, #2
    1d98: 0a000005     	beq	0x1db4 <fftwObject_tilde_attack+0xe8> @ imm = #0x14
    1d9c: eeb01b4a     	vmov.f64	d1, d10
    1da0: e3a08001     	mov	r8, #1
    1da4: ed9f0ba5     	vldr	d0, [pc, #660]          @ 0x2040 <fftwObject_tilde_attack+0x374>  // f64=0
    1da8: ebfffa55     	bl	0x704 <.plt+0xec>       @ imm = #-0x16ac  // CALL __pow_finite
    1dac: eef74bc0     	vcvt.f32.f64	s9, d0
    1db0: ece54a01     	vstmia	r5!, {s9}
    1db4: ee058a10     	vmov	s10, r8
    1db8: e2888001     	add	r8, r8, #1
    1dbc: eeb01b4a     	vmov.f64	d1, d10
    1dc0: eef85ac5     	vcvt.f32.s32	s11, s10
    1dc4: ee659aa8     	vmul.f32	s19, s11, s17
    1dc8: eeb70ae9     	vcvt.f64.f32	d0, s19
    1dcc: ebfffa4c     	bl	0x704 <.plt+0xec>       @ imm = #-0x16d0  // CALL __pow_finite
    1dd0: eeb7bbc0     	vcvt.f32.f64	s22, d0
    1dd4: eca5ba01     	vstmia	r5!, {s22}
    1dd8: ee0b8a90     	vmov	s23, r8
    1ddc: e2888001     	add	r8, r8, #1
    1de0: eeb01b4a     	vmov.f64	d1, d10
    1de4: eeb87aeb     	vcvt.f32.s32	s14, s23
    1de8: ee276a28     	vmul.f32	s12, s14, s17
    1dec: eeb70ac6     	vcvt.f64.f32	d0, s12
    1df0: ebfffa43     	bl	0x704 <.plt+0xec>       @ imm = #-0x16f4  // CALL __pow_finite
    1df4: e15a0008     	cmp	r10, r8
    1df8: eef76bc0     	vcvt.f32.f64	s13, d0
    1dfc: ece56a01     	vstmia	r5!, {s13}
    1e00: 0a000027     	beq	0x1ea4 <fftwObject_tilde_attack+0x1d8> @ imm = #0x9c
    1e04: e2889001     	add	r9, r8, #1
    1e08: e1a0b005     	mov	r11, r5
    1e0c: e2855010     	add	r5, r5, #16
    1e10: ee098a10     	vmov	s18, r8
    1e14: eeb01b4a     	vmov.f64	d1, d10
    1e18: eef87ac9     	vcvt.f32.s32	s15, s18
    1e1c: ee670aa8     	vmul.f32	s1, s15, s17
    1e20: eeb70ae0     	vcvt.f64.f32	d0, s1
    1e24: ebfffa36     	bl	0x704 <.plt+0xec>       @ imm = #-0x1728  // CALL __pow_finite
    1e28: ee029a10     	vmov	s4, r9
    1e2c: eef82ac2     	vcvt.f32.s32	s5, s4
    1e30: ee223aa8     	vmul.f32	s6, s5, s17
    1e34: eeb01b4a     	vmov.f64	d1, d10
    1e38: eef73bc0     	vcvt.f32.f64	s7, d0
    1e3c: eeb70ac3     	vcvt.f64.f32	d0, s6
    1e40: eceb3a01     	vstmia	r11!, {s7}
    1e44: ebfffa2e     	bl	0x704 <.plt+0xec>       @ imm = #-0x1748  // CALL __pow_finite
    1e48: e288c002     	add	r12, r8, #2
    1e4c: ee04ca10     	vmov	s8, r12
    1e50: eef84ac4     	vcvt.f32.s32	s9, s8
    1e54: ee245aa8     	vmul.f32	s10, s9, s17
    1e58: eeb01b4a     	vmov.f64	d1, d10
    1e5c: eef75bc0     	vcvt.f32.f64	s11, d0
    1e60: eeb70ac5     	vcvt.f64.f32	d0, s10
    1e64: ed455a03     	vstr	s11, [r5, #-12]
    1e68: ebfffa25     	bl	0x704 <.plt+0xec>       @ imm = #-0x176c  // CALL __pow_finite
    1e6c: e2883003     	add	r3, r8, #3
    1e70: e2888004     	add	r8, r8, #4
    1e74: ee093a90     	vmov	s19, r3
    1e78: eeb8bae9     	vcvt.f32.s32	s22, s19
    1e7c: ee6bba28     	vmul.f32	s23, s22, s17
    1e80: eeb01b4a     	vmov.f64	d1, d10
    1e84: eeb77bc0     	vcvt.f32.f64	s14, d0
    1e88: eeb70aeb     	vcvt.f64.f32	d0, s23
    1e8c: ed8b7a01     	vstr	s14, [r11, #4]
    1e90: ebfffa1b     	bl	0x704 <.plt+0xec>       @ imm = #-0x1794  // CALL __pow_finite
    1e94: e15a0008     	cmp	r10, r8
    1e98: eeb70bc0     	vcvt.f32.f64	s0, d0
    1e9c: ed050a01     	vstr	s0, [r5, #-4]
    1ea0: 1affffd7     	bne	0x1e04 <fftwObject_tilde_attack+0x138> @ imm = #-0xa4
    1ea4: e15a0006     	cmp	r10, r6
    1ea8: aa00002e     	bge	0x1f68 <fftwObject_tilde_attack+0x29c> @ imm = #0xb8
    1eac: e3082347     	movw	r2, #0x8347
    1eb0: e3402019     	movt	r2, #0x19
    1eb4: e08aa002     	add	r10, r10, r2
    1eb8: e3000d1c     	movw	r0, #0xd1c
    1ebc: e3400066     	movt	r0, #0x66
    1ec0: e3a015fe     	mov	r1, #1065353216
    1ec4: e0878000     	add	r8, r7, r0
    1ec8: e087310a     	add	r3, r7, r10, lsl #2
    1ecc: e0889106     	add	r9, r8, r6, lsl #2
    1ed0: e049b003     	sub	r11, r9, r3
    1ed4: e24bc004     	sub	r12, r11, #4
    1ed8: e1a0512c     	lsr	r5, r12, #2
    1edc: e2852001     	add	r2, r5, #1
    1ee0: e212a007     	ands	r10, r2, #7
    1ee4: 0a000013     	beq	0x1f38 <fftwObject_tilde_attack+0x26c> @ imm = #0x4c
    1ee8: e35a0001     	cmp	r10, #1
    1eec: 0a00000e     	beq	0x1f2c <fftwObject_tilde_attack+0x260> @ imm = #0x38
    1ef0: e35a0002     	cmp	r10, #2
    1ef4: 0a00000b     	beq	0x1f28 <fftwObject_tilde_attack+0x25c> @ imm = #0x2c
    1ef8: e35a0003     	cmp	r10, #3
    1efc: 0a000008     	beq	0x1f24 <fftwObject_tilde_attack+0x258> @ imm = #0x20
    1f00: e35a0004     	cmp	r10, #4
    1f04: 0a000005     	beq	0x1f20 <fftwObject_tilde_attack+0x254> @ imm = #0x14
    1f08: e35a0005     	cmp	r10, #5
    1f0c: 0a000002     	beq	0x1f1c <fftwObject_tilde_attack+0x250> @ imm = #0x8
    1f10: e35a0006     	cmp	r10, #6
    1f14: 1a00007e     	bne	0x2114 <fftwObject_tilde_attack+0x448> @ imm = #0x1f8
    1f18: e4831004     	str	r1, [r3], #4
    1f1c: e4831004     	str	r1, [r3], #4
    1f20: e4831004     	str	r1, [r3], #4
    1f24: e4831004     	str	r1, [r3], #4
    1f28: e4831004     	str	r1, [r3], #4
    1f2c: e4831004     	str	r1, [r3], #4
    1f30: e1590003     	cmp	r9, r3
    1f34: 0a00000b     	beq	0x1f68 <fftwObject_tilde_attack+0x29c> @ imm = #0x2c
    1f38: e1a00003     	mov	r0, r3
    1f3c: e2833020     	add	r3, r3, #32
    1f40: e4801004     	str	r1, [r0], #4
    1f44: e503101c     	str	r1, [r3, #-0x1c]
    1f48: e5801004     	str	r1, [r0, #0x4]
    1f4c: e5031014     	str	r1, [r3, #-0x14]
    1f50: e5031010     	str	r1, [r3, #-0x10]
    1f54: e503100c     	str	r1, [r3, #-0xc]
    1f58: e5031008     	str	r1, [r3, #-0x8]
    1f5c: e5031004     	str	r1, [r3, #-0x4]
    1f60: e1590003     	cmp	r9, r3
    1f64: 1afffff3     	bne	0x1f38 <fftwObject_tilde_attack+0x26c> @ imm = #-0x34
    1f68: e3540000     	cmp	r4, #0
    1f6c: da000066     	ble	0x210c <fftwObject_tilde_attack+0x440> @ imm = #0x198
    1f70: eef7aa00     	vmov.f32	s21, #1.000000e+00
    1f74: e308e347     	movw	lr, #0x8347
    1f78: ee014a10     	vmov	s2, r4
    1f7c: e340e019     	movt	lr, #0x19
    1f80: e086800e     	add	r8, r6, lr
    1f84: e2141003     	ands	r1, r4, #3
    1f88: e3a06000     	mov	r6, #0
    1f8c: e0875108     	add	r5, r7, r8, lsl #2
    1f90: eef81ac1     	vcvt.f32.s32	s3, s2
    1f94: eef80bc1     	vcvt.f64.s32	d16, s2
    1f98: ee8abaa1     	vdiv.f32	s22, s21, s3
    1f9c: ee809ba0     	vdiv.f64	d9, d16, d16
    1fa0: eeb78ac8     	vcvt.f64.f32	d8, s16
    1fa4: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    1fa8: 0a000027     	beq	0x204c <fftwObject_tilde_attack+0x380> @ imm = #0x9c
    1fac: e3510001     	cmp	r1, #1
    1fb0: 0a000014     	beq	0x2008 <fftwObject_tilde_attack+0x33c> @ imm = #0x50
    1fb4: e3510002     	cmp	r1, #2
    1fb8: 0a000007     	beq	0x1fdc <fftwObject_tilde_attack+0x310> @ imm = #0x1c
    1fbc: ed9f0b1f     	vldr	d0, [pc, #124]          @ 0x2040 <fftwObject_tilde_attack+0x374>  // f64=0
    1fc0: e3a06001     	mov	r6, #1
    1fc4: eeb01b48     	vmov.f64	d1, d8
    1fc8: ebfff9cd     	bl	0x704 <.plt+0xec>       @ imm = #-0x18cc  // CALL __pow_finite
    1fcc: ee7a1b40     	vsub.f64	d17, d10, d0
    1fd0: ee290b21     	vmul.f64	d0, d9, d17
    1fd4: eeb76bc0     	vcvt.f32.f64	s12, d0
    1fd8: eca56a01     	vstmia	r5!, {s12}
    1fdc: ee066a90     	vmov	s13, r6
    1fe0: e2866001     	add	r6, r6, #1
    1fe4: eeb01b48     	vmov.f64	d1, d8
    1fe8: eef87ae6     	vcvt.f32.s32	s15, s13
    1fec: ee670a8b     	vmul.f32	s1, s15, s22
    1ff0: eeb70ae0     	vcvt.f64.f32	d0, s1
    1ff4: ebfff9c2     	bl	0x704 <.plt+0xec>       @ imm = #-0x18f8  // CALL __pow_finite
    1ff8: ee7a2b40     	vsub.f64	d18, d10, d0
    1ffc: ee290b22     	vmul.f64	d0, d9, d18
    2000: eeb72bc0     	vcvt.f32.f64	s4, d0
    2004: eca52a01     	vstmia	r5!, {s4}
    2008: ee026a90     	vmov	s5, r6
    200c: e2866001     	add	r6, r6, #1
    2010: eeb01b48     	vmov.f64	d1, d8
    2014: eeb83ae2     	vcvt.f32.s32	s6, s5
    2018: ee633a0b     	vmul.f32	s7, s6, s22
    201c: eeb70ae3     	vcvt.f64.f32	d0, s7
    2020: ebfff9b7     	bl	0x704 <.plt+0xec>       @ imm = #-0x1924  // CALL __pow_finite
    2024: e1540006     	cmp	r4, r6
    2028: ee7a3b40     	vsub.f64	d19, d10, d0
    202c: ee290b23     	vmul.f64	d0, d9, d19
    2030: eeb74bc0     	vcvt.f32.f64	s8, d0
    2034: eca54a01     	vstmia	r5!, {s8}
    2038: 0a000033     	beq	0x210c <fftwObject_tilde_attack+0x440> @ imm = #0xcc
    203c: ea000002     	b	0x204c <fftwObject_tilde_attack+0x380> @ imm = #0x8
    2040: 00 00 00 00  	.word	0x00000000
    2044: 00 00 00 00  	.word	0x00000000
    2048: 00 00 00 00  	.word	0x00000000
    204c: e2867001     	add	r7, r6, #1
    2050: e1a09005     	mov	r9, r5
    2054: e286b002     	add	r11, r6, #2
    2058: e2855010     	add	r5, r5, #16
    205c: ee046a90     	vmov	s9, r6
    2060: eeb01b48     	vmov.f64	d1, d8
    2064: ee0b7a90     	vmov	s23, r7
    2068: eeb85ae4     	vcvt.f32.s32	s10, s9
    206c: ee655a0b     	vmul.f32	s11, s10, s22
    2070: eeb70ae5     	vcvt.f64.f32	d0, s11
    2074: ebfff9a2     	bl	0x704 <.plt+0xec>       @ imm = #-0x1978  // CALL __pow_finite
    2078: eeb87aeb     	vcvt.f32.s32	s14, s23
    207c: ee276a0b     	vmul.f32	s12, s14, s22
    2080: eeb01b48     	vmov.f64	d1, d8
    2084: ee7a4b40     	vsub.f64	d20, d10, d0
    2088: ee695b24     	vmul.f64	d21, d9, d20
    208c: eeb70ac6     	vcvt.f64.f32	d0, s12
    2090: eef76be5     	vcvt.f32.f64	s13, d21
    2094: ece96a01     	vstmia	r9!, {s13}
    2098: ebfff999     	bl	0x704 <.plt+0xec>       @ imm = #-0x199c  // CALL __pow_finite
    209c: ee01ba10     	vmov	s2, r11
    20a0: eef87ac1     	vcvt.f32.s32	s15, s2
    20a4: ee272a8b     	vmul.f32	s4, s15, s22
    20a8: eeb01b48     	vmov.f64	d1, d8
    20ac: ee7a6b40     	vsub.f64	d22, d10, d0
    20b0: ee697b26     	vmul.f64	d23, d9, d22
    20b4: eeb70ac2     	vcvt.f64.f32	d0, s4
    20b8: eef72be7     	vcvt.f32.f64	s5, d23
    20bc: ed452a03     	vstr	s5, [r5, #-12]
    20c0: ebfff98f     	bl	0x704 <.plt+0xec>       @ imm = #-0x19c4  // CALL __pow_finite
    20c4: e286c003     	add	r12, r6, #3
    20c8: e2866004     	add	r6, r6, #4
    20cc: ee01ca90     	vmov	s3, r12
    20d0: eeb83ae1     	vcvt.f32.s32	s6, s3
    20d4: ee633a0b     	vmul.f32	s7, s6, s22
    20d8: eeb01b48     	vmov.f64	d1, d8
    20dc: ee7a8b40     	vsub.f64	d24, d10, d0
    20e0: ee699b28     	vmul.f64	d25, d9, d24
    20e4: eeb70ae3     	vcvt.f64.f32	d0, s7
    20e8: eeb74be9     	vcvt.f32.f64	s8, d25
    20ec: ed894a01     	vstr	s8, [r9, #4]
    20f0: ebfff983     	bl	0x704 <.plt+0xec>       @ imm = #-0x19f4  // CALL __pow_finite
    20f4: e1540006     	cmp	r4, r6
    20f8: ee7aab40     	vsub.f64	d26, d10, d0
    20fc: ee290b2a     	vmul.f64	d0, d9, d26
    2100: eeb70bc0     	vcvt.f32.f64	s0, d0
    2104: ed050a01     	vstr	s0, [r5, #-4]
    2108: 1affffcf     	bne	0x204c <fftwObject_tilde_attack+0x380> @ imm = #-0xc4
    210c: ecbd8b08     	vpop	{d8, d9, d10, d11}
    2110: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2114: e4831004     	str	r1, [r3], #4
    2118: eaffff7e     	b	0x1f18 <fftwObject_tilde_attack+0x24c> @ imm = #-0x208
    211c: e3a0a000     	mov	r10, #0
    2120: eaffff5f     	b	0x1ea4 <fftwObject_tilde_attack+0x1d8> @ imm = #-0x284

