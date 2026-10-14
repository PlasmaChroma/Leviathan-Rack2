0000187c <fftwObject_tilde_release>:
    187c: eef77a00     	vmov.f32	s15, #1.000000e+00
    1880: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1884: e1a08000     	mov	r8, r0
    1888: ed2d8b08     	vpush	{d8, d9, d10, d11}
    188c: eeb40ae7     	vcmpe.f32	s0, s15
    1890: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1894: 8a000003     	bhi	0x18a8 <fftwObject_tilde_release+0x2c> @ imm = #0xc
    1898: eeb50ac0     	vcmpe.f32	s0, #0
    189c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    18a0: ae777ac0     	vsubge.f32	s15, s15, s0
    18a4: bddf7ad1     	vldrlt	s15, [pc, #836]         @ 0x1bf0 <fftwObject_tilde_release+0x374>
    18a8: e2882866     	add	r2, r8, #6684672
    18ac: e2823a01     	add	r3, r2, #4096
    18b0: e5926d10     	ldr	r6, [r2, #0xd10]
    18b4: e593003c     	ldr	r0, [r3, #0x3c]
    18b8: edc37a11     	vstr	s15, [r3, #68]
    18bc: e0461000     	sub	r1, r6, r0
    18c0: e1560000     	cmp	r6, r0
    18c4: ee001a10     	vmov	s0, r1
    18c8: b1a0a006     	movlt	r10, r6
    18cc: a1a0a000     	movge	r10, r0
    18d0: ed939a12     	vldr	s18, [r3, #72]
    18d4: e583a03c     	str	r10, [r3, #0x3c]
    18d8: eef80ac0     	vcvt.f32.s32	s1, s0
    18dc: ed938a13     	vldr	s16, [r3, #76]
    18e0: ee201aa7     	vmul.f32	s2, s1, s15
    18e4: eefd1ac1     	vcvt.s32.f32	s3, s2
    18e8: ee114a90     	vmov	r4, s3
    18ec: e1560004     	cmp	r6, r4
    18f0: b1a01006     	movlt	r1, r6
    18f4: a1a01004     	movge	r1, r4
    18f8: e35a0000     	cmp	r10, #0
    18fc: e5831040     	str	r1, [r3, #0x40]
    1900: e0466001     	sub	r6, r6, r1
    1904: da0000ee     	ble	0x1cc4 <fftwObject_tilde_release+0x448> @ imm = #0x3b8
    1908: eeb77a00     	vmov.f32	s14, #1.000000e+00
    190c: e21a9003     	ands	r9, r10, #3
    1910: ee020a10     	vmov	s4, r0
    1914: e3007d1c     	movw	r7, #0xd1c
    1918: e3407066     	movt	r7, #0x66
    191c: e3a05000     	mov	r5, #0
    1920: e0887007     	add	r7, r8, r7
    1924: eef82ac2     	vcvt.f32.s32	s5, s4
    1928: eeb7aac9     	vcvt.f64.f32	d10, s18
    192c: eec78a22     	vdiv.f32	s17, s14, s5
    1930: 0a00001d     	beq	0x19ac <fftwObject_tilde_release+0x130> @ imm = #0x74
    1934: e3590001     	cmp	r9, #1
    1938: 0a000010     	beq	0x1980 <fftwObject_tilde_release+0x104> @ imm = #0x40
    193c: e3590002     	cmp	r9, #2
    1940: 0a000005     	beq	0x195c <fftwObject_tilde_release+0xe0> @ imm = #0x14
    1944: ed9f0ba7     	vldr	d0, [pc, #668]          @ 0x1be8 <fftwObject_tilde_release+0x36c>
    1948: e3a05001     	mov	r5, #1
    194c: eeb01b4a     	vmov.f64	d1, d10
    1950: ebfffb6b     	bl	0x704 <.plt+0xec>       @ imm = #-0x1254
    1954: eeb73bc0     	vcvt.f32.f64	s6, d0
    1958: eca73a01     	vstmia	r7!, {s6}
    195c: ee035a90     	vmov	s7, r5
    1960: e2855001     	add	r5, r5, #1
    1964: eeb01b4a     	vmov.f64	d1, d10
    1968: eeb84ae3     	vcvt.f32.s32	s8, s7
    196c: ee644a28     	vmul.f32	s9, s8, s17
    1970: eeb70ae4     	vcvt.f64.f32	d0, s9
    1974: ebfffb62     	bl	0x704 <.plt+0xec>       @ imm = #-0x1278
    1978: eeb75bc0     	vcvt.f32.f64	s10, d0
    197c: eca75a01     	vstmia	r7!, {s10}
    1980: ee055a90     	vmov	s11, r5
    1984: e2855001     	add	r5, r5, #1
    1988: eeb01b4a     	vmov.f64	d1, d10
    198c: eeb86ae5     	vcvt.f32.s32	s12, s11
    1990: ee666a28     	vmul.f32	s13, s12, s17
    1994: eeb70ae6     	vcvt.f64.f32	d0, s13
    1998: ebfffb59     	bl	0x704 <.plt+0xec>       @ imm = #-0x129c
    199c: e15a0005     	cmp	r10, r5
    19a0: eef79bc0     	vcvt.f32.f64	s19, d0
    19a4: ece79a01     	vstmia	r7!, {s19}
    19a8: 0a000027     	beq	0x1a4c <fftwObject_tilde_release+0x1d0> @ imm = #0x9c
    19ac: e2859001     	add	r9, r5, #1
    19b0: e1a0b007     	mov	r11, r7
    19b4: e2877010     	add	r7, r7, #16
    19b8: ee0b5a10     	vmov	s22, r5
    19bc: eeb01b4a     	vmov.f64	d1, d10
    19c0: ee099a10     	vmov	s18, r9
    19c4: eef8bacb     	vcvt.f32.s32	s23, s22
    19c8: ee6b7aa8     	vmul.f32	s15, s23, s17
    19cc: eeb70ae7     	vcvt.f64.f32	d0, s15
    19d0: ebfffb4b     	bl	0x704 <.plt+0xec>       @ imm = #-0x12d4
    19d4: eeb87ac9     	vcvt.f32.s32	s14, s18
    19d8: ee272a28     	vmul.f32	s4, s14, s17
    19dc: eeb01b4a     	vmov.f64	d1, d10
    19e0: eef72bc0     	vcvt.f32.f64	s5, d0
    19e4: eeb70ac2     	vcvt.f64.f32	d0, s4
    19e8: eceb2a01     	vstmia	r11!, {s5}
    19ec: ebfffb44     	bl	0x704 <.plt+0xec>       @ imm = #-0x12f0
    19f0: e285c002     	add	r12, r5, #2
    19f4: ee03ca10     	vmov	s6, r12
    19f8: eef83ac3     	vcvt.f32.s32	s7, s6
    19fc: ee234aa8     	vmul.f32	s8, s7, s17
    1a00: eeb01b4a     	vmov.f64	d1, d10
    1a04: eef74bc0     	vcvt.f32.f64	s9, d0
    1a08: eeb70ac4     	vcvt.f64.f32	d0, s8
    1a0c: ed474a03     	vstr	s9, [r7, #-12]
    1a10: ebfffb3b     	bl	0x704 <.plt+0xec>       @ imm = #-0x1314
    1a14: e2852003     	add	r2, r5, #3
    1a18: e2855004     	add	r5, r5, #4
    1a1c: ee052a10     	vmov	s10, r2
    1a20: eef85ac5     	vcvt.f32.s32	s11, s10
    1a24: ee256aa8     	vmul.f32	s12, s11, s17
    1a28: eeb01b4a     	vmov.f64	d1, d10
    1a2c: eef76bc0     	vcvt.f32.f64	s13, d0
    1a30: eeb70ac6     	vcvt.f64.f32	d0, s12
    1a34: edcb6a01     	vstr	s13, [r11, #4]
    1a38: ebfffb31     	bl	0x704 <.plt+0xec>       @ imm = #-0x133c
    1a3c: e15a0005     	cmp	r10, r5
    1a40: eeb70bc0     	vcvt.f32.f64	s0, d0
    1a44: ed070a01     	vstr	s0, [r7, #-4]
    1a48: 1affffd7     	bne	0x19ac <fftwObject_tilde_release+0x130> @ imm = #-0xa4
    1a4c: e15a0006     	cmp	r10, r6
    1a50: aa00002e     	bge	0x1b10 <fftwObject_tilde_release+0x294> @ imm = #0xb8
    1a54: e3083347     	movw	r3, #0x8347
    1a58: e3403019     	movt	r3, #0x19
    1a5c: e08aa003     	add	r10, r10, r3
    1a60: e3000d1c     	movw	r0, #0xd1c
    1a64: e3400066     	movt	r0, #0x66
    1a68: e3a015fe     	mov	r1, #1065353216
    1a6c: e0885000     	add	r5, r8, r0
    1a70: e088310a     	add	r3, r8, r10, lsl #2
    1a74: e0859106     	add	r9, r5, r6, lsl #2
    1a78: e049b003     	sub	r11, r9, r3
    1a7c: e24bc004     	sub	r12, r11, #4
    1a80: e1a0212c     	lsr	r2, r12, #2
    1a84: e2827001     	add	r7, r2, #1
    1a88: e217a007     	ands	r10, r7, #7
    1a8c: 0a000013     	beq	0x1ae0 <fftwObject_tilde_release+0x264> @ imm = #0x4c
    1a90: e35a0001     	cmp	r10, #1
    1a94: 0a00000e     	beq	0x1ad4 <fftwObject_tilde_release+0x258> @ imm = #0x38
    1a98: e35a0002     	cmp	r10, #2
    1a9c: 0a00000b     	beq	0x1ad0 <fftwObject_tilde_release+0x254> @ imm = #0x2c
    1aa0: e35a0003     	cmp	r10, #3
    1aa4: 0a000008     	beq	0x1acc <fftwObject_tilde_release+0x250> @ imm = #0x20
    1aa8: e35a0004     	cmp	r10, #4
    1aac: 0a000005     	beq	0x1ac8 <fftwObject_tilde_release+0x24c> @ imm = #0x14
    1ab0: e35a0005     	cmp	r10, #5
    1ab4: 0a000002     	beq	0x1ac4 <fftwObject_tilde_release+0x248> @ imm = #0x8
    1ab8: e35a0006     	cmp	r10, #6
    1abc: 1a00007e     	bne	0x1cbc <fftwObject_tilde_release+0x440> @ imm = #0x1f8
    1ac0: e4831004     	str	r1, [r3], #4
    1ac4: e4831004     	str	r1, [r3], #4
    1ac8: e4831004     	str	r1, [r3], #4
    1acc: e4831004     	str	r1, [r3], #4
    1ad0: e4831004     	str	r1, [r3], #4
    1ad4: e4831004     	str	r1, [r3], #4
    1ad8: e1590003     	cmp	r9, r3
    1adc: 0a00000b     	beq	0x1b10 <fftwObject_tilde_release+0x294> @ imm = #0x2c
    1ae0: e1a00003     	mov	r0, r3
    1ae4: e2833020     	add	r3, r3, #32
    1ae8: e4801004     	str	r1, [r0], #4
    1aec: e503101c     	str	r1, [r3, #-0x1c]
    1af0: e5801004     	str	r1, [r0, #0x4]
    1af4: e5031014     	str	r1, [r3, #-0x14]
    1af8: e5031010     	str	r1, [r3, #-0x10]
    1afc: e503100c     	str	r1, [r3, #-0xc]
    1b00: e5031008     	str	r1, [r3, #-0x8]
    1b04: e5031004     	str	r1, [r3, #-0x4]
    1b08: e1590003     	cmp	r9, r3
    1b0c: 1afffff3     	bne	0x1ae0 <fftwObject_tilde_release+0x264> @ imm = #-0x34
    1b10: e3540000     	cmp	r4, #0
    1b14: da000066     	ble	0x1cb4 <fftwObject_tilde_release+0x438> @ imm = #0x198
    1b18: eef7aa00     	vmov.f32	s21, #1.000000e+00
    1b1c: e308e347     	movw	lr, #0x8347
    1b20: ee004a90     	vmov	s1, r4
    1b24: e340e019     	movt	lr, #0x19
    1b28: e086500e     	add	r5, r6, lr
    1b2c: e2141003     	ands	r1, r4, #3
    1b30: e3a06000     	mov	r6, #0
    1b34: e0887105     	add	r7, r8, r5, lsl #2
    1b38: eeb81ae0     	vcvt.f32.s32	s2, s1
    1b3c: eef80be0     	vcvt.f64.s32	d16, s1
    1b40: ee8aba81     	vdiv.f32	s22, s21, s2
    1b44: ee809ba0     	vdiv.f64	d9, d16, d16
    1b48: eeb78ac8     	vcvt.f64.f32	d8, s16
    1b4c: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    1b50: 0a000027     	beq	0x1bf4 <fftwObject_tilde_release+0x378> @ imm = #0x9c
    1b54: e3510001     	cmp	r1, #1
    1b58: 0a000014     	beq	0x1bb0 <fftwObject_tilde_release+0x334> @ imm = #0x50
    1b5c: e3510002     	cmp	r1, #2
    1b60: 0a000007     	beq	0x1b84 <fftwObject_tilde_release+0x308> @ imm = #0x1c
    1b64: ed9f0b1f     	vldr	d0, [pc, #124]          @ 0x1be8 <fftwObject_tilde_release+0x36c>
    1b68: e3a06001     	mov	r6, #1
    1b6c: eeb01b48     	vmov.f64	d1, d8
    1b70: ebfffae3     	bl	0x704 <.plt+0xec>       @ imm = #-0x1474
    1b74: ee7a1b40     	vsub.f64	d17, d10, d0
    1b78: ee290b21     	vmul.f64	d0, d9, d17
    1b7c: eef7bbc0     	vcvt.f32.f64	s23, d0
    1b80: ece7ba01     	vstmia	r7!, {s23}
    1b84: ee076a90     	vmov	s15, r6
    1b88: e2866001     	add	r6, r6, #1
    1b8c: eeb01b48     	vmov.f64	d1, d8
    1b90: eeb87ae7     	vcvt.f32.s32	s14, s15
    1b94: ee272a0b     	vmul.f32	s4, s14, s22
    1b98: eeb70ac2     	vcvt.f64.f32	d0, s4
    1b9c: ebfffad8     	bl	0x704 <.plt+0xec>       @ imm = #-0x14a0
    1ba0: ee7a2b40     	vsub.f64	d18, d10, d0
    1ba4: ee290b22     	vmul.f64	d0, d9, d18
    1ba8: eef72bc0     	vcvt.f32.f64	s5, d0
    1bac: ece72a01     	vstmia	r7!, {s5}
    1bb0: ee036a10     	vmov	s6, r6
    1bb4: e2866001     	add	r6, r6, #1
    1bb8: eeb01b48     	vmov.f64	d1, d8
    1bbc: eef83ac3     	vcvt.f32.s32	s7, s6
    1bc0: ee234a8b     	vmul.f32	s8, s7, s22
    1bc4: eeb70ac4     	vcvt.f64.f32	d0, s8
    1bc8: ebfffacd     	bl	0x704 <.plt+0xec>       @ imm = #-0x14cc
    1bcc: e1540006     	cmp	r4, r6
    1bd0: ee7a3b40     	vsub.f64	d19, d10, d0
    1bd4: ee290b23     	vmul.f64	d0, d9, d19
    1bd8: eef74bc0     	vcvt.f32.f64	s9, d0
    1bdc: ece74a01     	vstmia	r7!, {s9}
    1be0: 0a000033     	beq	0x1cb4 <fftwObject_tilde_release+0x438> @ imm = #0xcc
    1be4: ea000002     	b	0x1bf4 <fftwObject_tilde_release+0x378> @ imm = #0x8
    1be8: 00 00 00 00  	.word	0x00000000
    1bec: 00 00 00 00  	.word	0x00000000
    1bf0: 00 00 00 00  	.word	0x00000000
    1bf4: e2869001     	add	r9, r6, #1
    1bf8: e1a08007     	mov	r8, r7
    1bfc: e286b002     	add	r11, r6, #2
    1c00: e2877010     	add	r7, r7, #16
    1c04: ee056a10     	vmov	s10, r6
    1c08: eeb01b48     	vmov.f64	d1, d8
    1c0c: eef85ac5     	vcvt.f32.s32	s11, s10
    1c10: ee256a8b     	vmul.f32	s12, s11, s22
    1c14: eeb70ac6     	vcvt.f64.f32	d0, s12
    1c18: ebfffab9     	bl	0x704 <.plt+0xec>       @ imm = #-0x151c
    1c1c: ee069a90     	vmov	s13, r9
    1c20: eef8bae6     	vcvt.f32.s32	s23, s13
    1c24: ee6b7a8b     	vmul.f32	s15, s23, s22
    1c28: eeb01b48     	vmov.f64	d1, d8
    1c2c: ee7a4b40     	vsub.f64	d20, d10, d0
    1c30: ee695b24     	vmul.f64	d21, d9, d20
    1c34: eeb70ae7     	vcvt.f64.f32	d0, s15
    1c38: eeb77be5     	vcvt.f32.f64	s14, d21
    1c3c: eca87a01     	vstmia	r8!, {s14}
    1c40: ebfffaaf     	bl	0x704 <.plt+0xec>       @ imm = #-0x1544
    1c44: ee01ba10     	vmov	s2, r11
    1c48: eeb82ac1     	vcvt.f32.s32	s4, s2
    1c4c: ee622a0b     	vmul.f32	s5, s4, s22
    1c50: eeb01b48     	vmov.f64	d1, d8
    1c54: ee7a6b40     	vsub.f64	d22, d10, d0
    1c58: ee697b26     	vmul.f64	d23, d9, d22
    1c5c: eeb70ae2     	vcvt.f64.f32	d0, s5
    1c60: eeb73be7     	vcvt.f32.f64	s6, d23
    1c64: ed073a03     	vstr	s6, [r7, #-12]
    1c68: ebfffaa5     	bl	0x704 <.plt+0xec>       @ imm = #-0x156c
    1c6c: e286c003     	add	r12, r6, #3
    1c70: e2866004     	add	r6, r6, #4
    1c74: ee01ca90     	vmov	s3, r12
    1c78: eef83ae1     	vcvt.f32.s32	s7, s3
    1c7c: ee234a8b     	vmul.f32	s8, s7, s22
    1c80: eeb01b48     	vmov.f64	d1, d8
    1c84: ee7a8b40     	vsub.f64	d24, d10, d0
    1c88: ee699b28     	vmul.f64	d25, d9, d24
    1c8c: eeb70ac4     	vcvt.f64.f32	d0, s8
    1c90: eef74be9     	vcvt.f32.f64	s9, d25
    1c94: edc84a01     	vstr	s9, [r8, #4]
    1c98: ebfffa99     	bl	0x704 <.plt+0xec>       @ imm = #-0x159c
    1c9c: e1540006     	cmp	r4, r6
    1ca0: ee7aab40     	vsub.f64	d26, d10, d0
    1ca4: ee290b2a     	vmul.f64	d0, d9, d26
    1ca8: eeb70bc0     	vcvt.f32.f64	s0, d0
    1cac: ed070a01     	vstr	s0, [r7, #-4]
    1cb0: 1affffcf     	bne	0x1bf4 <fftwObject_tilde_release+0x378> @ imm = #-0xc4
    1cb4: ecbd8b08     	vpop	{d8, d9, d10, d11}
    1cb8: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1cbc: e4831004     	str	r1, [r3], #4
    1cc0: eaffff7e     	b	0x1ac0 <fftwObject_tilde_release+0x244> @ imm = #-0x208
    1cc4: e3a0a000     	mov	r10, #0
    1cc8: eaffff5f     	b	0x1a4c <fftwObject_tilde_release+0x1d0> @ imm = #-0x284

