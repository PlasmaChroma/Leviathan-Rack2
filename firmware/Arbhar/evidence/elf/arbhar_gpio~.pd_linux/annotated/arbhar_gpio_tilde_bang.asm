0000ce18 <arbhar_gpio_tilde_bang>:
    ce18: e59f34a4     	ldr	r3, [pc, #0x4a4]        @ 0xd2c4 <arbhar_gpio_tilde_bang+0x4ac>  // u32=0x1a4c4; f32?=1.50942265e-40
    ce1c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ce20: e08f1003     	add	r1, pc, r3
    ce24: ed2d8b08     	vpush	{d8, d9, d10, d11}
    ce28: e1a04000     	mov	r4, r0
    ce2c: e591001c     	ldr	r0, [r1, #0x1c]
    ce30: e3500000     	cmp	r0, #0
    ce34: e24dd064     	sub	sp, sp, #100
    ce38: da000008     	ble	0xce60 <arbhar_gpio_tilde_bang+0x48> @ imm = #0x20
    ce3c: e2845a01     	add	r5, r4, #4096
    ce40: e5d52d61     	ldrb	r2, [r5, #0xd61]
    ce44: e3520000     	cmp	r2, #0
    ce48: 1a00018a     	bne	0xd478 <arbhar_gpio_tilde_bang+0x660> @ imm = #0x628
    ce4c: e59f8474     	ldr	r8, [pc, #0x474]        @ 0xd2c8 <arbhar_gpio_tilde_bang+0x4b0>  // u32=0x1a48c; f32?=1.50863793e-40
    ce50: e3a0a000     	mov	r10, #0
    ce54: e5c4a075     	strb	r10, [r4, #0x75]
    ce58: e08f9008     	add	r9, pc, r8
    ce5c: e589a01c     	str	r10, [r9, #0x1c]
    ce60: e5d42030     	ldrb	r2, [r4, #0x30]
    ce64: e3520062     	cmp	r2, #98
    ce68: 9a0001a0     	bls	0xd4f0 <arbhar_gpio_tilde_bang+0x6d8> @ imm = #0x680
    ce6c: e3520064     	cmp	r2, #100
    ce70: 0a000254     	beq	0xd7c8 <arbhar_gpio_tilde_bang+0x9b0> @ imm = #0x950
    ce74: e59f7450     	ldr	r7, [pc, #0x450]        @ 0xd2cc <arbhar_gpio_tilde_bang+0x4b4>  // u32=0x1a46c; f32?=1.50818951e-40
    ce78: e08fa007     	add	r10, pc, r7
    ce7c: e59a9024     	ldr	r9, [r10, #0x24]
    ce80: e35900fa     	cmp	r9, #250
    ce84: ca000143     	bgt	0xd398 <arbhar_gpio_tilde_bang+0x580> @ imm = #0x50c
    ce88: e2897001     	add	r7, r9, #1
    ce8c: e5d4e030     	ldrb	lr, [r4, #0x30]
    ce90: e59fa438     	ldr	r10, [pc, #0x438]       @ 0xd2d0 <arbhar_gpio_tilde_bang+0x4b8>  // u32=0x1a44c; f32?=1.5077411e-40
    ce94: e35e0062     	cmp	lr, #98
    ce98: e08f900a     	add	r9, pc, r10
    ce9c: e5897024     	str	r7, [r9, #0x24]
    cea0: 9a000180     	bls	0xd4a8 <arbhar_gpio_tilde_bang+0x690> @ imm = #0x600
    cea4: e35e0064     	cmp	lr, #100
    cea8: 1a000157     	bne	0xd40c <arbhar_gpio_tilde_bang+0x5f4> @ imm = #0x55c
    ceac: e3007126     	movw	r7, #0x126
    ceb0: e30082ea     	movw	r8, #0x2ea
    ceb4: e19460b7     	ldrh	r6, [r4, r7]
    ceb8: e300cfff     	movw	r12, #0xfff
    cebc: e194b0b8     	ldrh	r11, [r4, r8]
    cec0: e30014ae     	movw	r1, #0x4ae
    cec4: e04c2006     	sub	r2, r12, r6
    cec8: eddf9afc     	vldr	s19, [pc, #1008]        @ 0xd2c0 <arbhar_gpio_tilde_bang+0x4a8>  // f32=0.00048828125
    cecc: e04c300b     	sub	r3, r12, r11
    ced0: e59f03fc     	ldr	r0, [pc, #0x3fc]        @ 0xd2d4 <arbhar_gpio_tilde_bang+0x4bc>  // u32=0x1a404; f32?=1.50673216e-40
    ced4: e19470b1     	ldrh	r7, [r4, r1]
    ced8: e300cfff     	movw	r12, #0xfff
    cedc: ee092a10     	vmov	s18, r2
    cee0: e08f5000     	add	r5, pc, r0
    cee4: ee0a3a10     	vmov	s20, r3
    cee8: e04c2007     	sub	r2, r12, r7
    ceec: eef8aac9     	vcvt.f32.s32	s21, s18
    cef0: e3006672     	movw	r6, #0x672
    cef4: ed95ba0c     	vldr	s22, [r5, #48]
    cef8: e300b836     	movw	r11, #0x836
    cefc: e19480b6     	ldrh	r8, [r4, r6]
    cf00: e59fe3d0     	ldr	lr, [pc, #0x3d0]        @ 0xd2d8 <arbhar_gpio_tilde_bang+0x4c0>  // u32=0x1a4a4; f32?=1.50897424e-40
    cf04: edd5ba0d     	vldr	s23, [r5, #52]
    cf08: e04c3008     	sub	r3, r12, r8
    cf0c: e08fa00e     	add	r10, pc, lr
    cf10: e300e9fa     	movw	lr, #0x9fa
    cf14: e19470be     	ldrh	r7, [r4, lr]
    cf18: e300ed82     	movw	lr, #0xd82
    cf1c: eeb87aca     	vcvt.f32.s32	s14, s20
    cf20: e59f63b4     	ldr	r6, [pc, #0x3b4]        @ 0xd2dc <arbhar_gpio_tilde_bang+0x4c4>  // u32=0x1a3b0; f32?=1.50555507e-40
    cf24: ee022a90     	vmov	s5, r2
    cf28: e3002bbe     	movw	r2, #0xbbe
    cf2c: ee033a10     	vmov	s6, r3
    cf30: e19430b2     	ldrh	r3, [r4, r2]
    cf34: e08f8006     	add	r8, pc, r6
    cf38: e3006f46     	movw	r6, #0xf46
    cf3c: e19420b6     	ldrh	r2, [r4, r6]
    cf40: edd86a0e     	vldr	s13, [r8, #56]
    cf44: e59a00e4     	ldr	r0, [r10, #0xe4]
    cf48: ee6a7aa9     	vmul.f32	s15, s21, s19
    cf4c: edd8aa0f     	vldr	s21, [r8, #60]
    cf50: ed989a10     	vldr	s18, [r8, #64]
    cf54: edd88a11     	vldr	s17, [r8, #68]
    cf58: eddf2bd6     	vldr	d18, [pc, #856]         @ 0xd2b8 <arbhar_gpio_tilde_bang+0x4a0>  // f64=0.02
    cf5c: ee270a29     	vmul.f32	s0, s14, s19
    cf60: edc57a0c     	vstr	s15, [r5, #48]
    cf64: e19450bb     	ldrh	r5, [r4, r11]
    cf68: e04cb007     	sub	r11, r12, r7
    cf6c: ed9f7ad3     	vldr	s14, [pc, #844]         @ 0xd2c0 <arbhar_gpio_tilde_bang+0x4a8>  // f32=0.00048828125
    cf70: e04c1005     	sub	r1, r12, r5
    cf74: e04c5003     	sub	r5, r12, r3
    cf78: e300310a     	movw	r3, #0x10a
    cf7c: ee041a10     	vmov	s8, r1
    cf80: e19410be     	ldrh	r1, [r4, lr]
    cf84: ee04ba90     	vmov	s9, r11
    cf88: e284ba01     	add	r11, r4, #4096
    cf8c: e04c7001     	sub	r7, r12, r1
    cf90: e300e2ce     	movw	lr, #0x2ce
    cf94: e19b10be     	ldrh	r1, [r11, lr]
    cf98: eddf4bc6     	vldr	d20, [pc, #792]         @ 0xd2b8 <arbhar_gpio_tilde_bang+0x4a0>  // f64=0.02
    cf9c: ee7b0a67     	vsub.f32	s1, s22, s15
    cfa0: ed98ba13     	vldr	s22, [r8, #76]
    cfa4: ed98aa14     	vldr	s20, [r8, #80]
    cfa8: ed880a0d     	vstr	s0, [r8, #52]
    cfac: ee3b1ac0     	vsub.f32	s2, s23, s0
    cfb0: edd8ba12     	vldr	s23, [r8, #72]
    cfb4: edd89a15     	vldr	s19, [r8, #84]
    cfb8: ed988a16     	vldr	s16, [r8, #88]
    cfbc: eef83ae2     	vcvt.f32.s32	s7, s5
    cfc0: ed980a17     	vldr	s0, [r8, #92]
    cfc4: eef85ac3     	vcvt.f32.s32	s11, s6
    cfc8: ee637a87     	vmul.f32	s15, s7, s14
    cfcc: eef01ae0     	vabs.f32	s3, s1
    cfd0: ee005a90     	vmov	s1, r5
    cfd4: e19b50b3     	ldrh	r5, [r11, r3]
    cfd8: edc87a0e     	vstr	s15, [r8, #56]
    cfdc: e04c6005     	sub	r6, r12, r5
    cfe0: e04c5001     	sub	r5, r12, r1
    cfe4: e3001656     	movw	r1, #0x656
    cfe8: eeb02ac1     	vabs.f32	s4, s2
    cfec: ee017a10     	vmov	s2, r7
    cff0: e04c7002     	sub	r7, r12, r2
    cff4: e3002492     	movw	r2, #0x492
    cff8: ee037a90     	vmov	s7, r7
    cffc: e19b30b2     	ldrh	r3, [r11, r2]
    d000: eeb85ac4     	vcvt.f32.s32	s10, s8
    d004: e19b70b1     	ldrh	r7, [r11, r1]
    d008: ee046a10     	vmov	s8, r6
    d00c: e04ce003     	sub	lr, r12, r3
    d010: e04c6007     	sub	r6, r12, r7
    d014: e300281a     	movw	r2, #0x81a
    d018: eef71ae1     	vcvt.f64.f32	d17, s3
    d01c: ee651a87     	vmul.f32	s3, s11, s14
    d020: ee056a90     	vmov	s11, r6
    d024: e3006ba2     	movw	r6, #0xba2
    d028: eef73ac2     	vcvt.f64.f32	d19, s4
    d02c: edc81a0f     	vstr	s3, [r8, #60]
    d030: eeb82ae4     	vcvt.f32.s32	s4, s9
    d034: ee045a90     	vmov	s9, r5
    d038: ee256a07     	vmul.f32	s12, s10, s14
    d03c: eef82ae0     	vcvt.f32.s32	s5, s1
    d040: ee00ea90     	vmov	s1, lr
    d044: e300e9de     	movw	lr, #0x9de
    d048: ed886a10     	vstr	s12, [r8, #64]
    d04c: ee767ae7     	vsub.f32	s15, s13, s15
    d050: ee626a07     	vmul.f32	s13, s4, s14
    d054: eeb83ac1     	vcvt.f32.s32	s6, s2
    d058: edc86a11     	vstr	s13, [r8, #68]
    d05c: ee622a87     	vmul.f32	s5, s5, s14
    d060: ee7aaae1     	vsub.f32	s21, s21, s3
    d064: edc82a12     	vstr	s5, [r8, #72]
    d068: e19b30b2     	ldrh	r3, [r11, r2]
    d06c: e19b10be     	ldrh	r1, [r11, lr]
    d070: e04c5003     	sub	r5, r12, r3
    d074: e19b20b6     	ldrh	r2, [r11, r6]
    d078: e04c7001     	sub	r7, r12, r1
    d07c: edd81a1a     	vldr	s3, [r8, #104]
    d080: e04cc002     	sub	r12, r12, r2
    d084: eeb81ae3     	vcvt.f32.s32	s2, s7
    d088: ee233a07     	vmul.f32	s6, s6, s14
    d08c: ee399a46     	vsub.f32	s18, s18, s12
    d090: ee065a10     	vmov	s12, r5
    d094: ed883a13     	vstr	s6, [r8, #76]
    d098: eeb82ac4     	vcvt.f32.s32	s4, s8
    d09c: eef07ae7     	vabs.f32	s15, s15
    d0a0: ee613a07     	vmul.f32	s7, s2, s14
    d0a4: ed981a19     	vldr	s2, [r8, #100]
    d0a8: ee788ae6     	vsub.f32	s17, s17, s13
    d0ac: edc83a14     	vstr	s7, [r8, #80]
    d0b0: eef41be2     	vcmpe.f64	d17, d18
    d0b4: eef84ae4     	vcvt.f32.s32	s9, s9
    d0b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d0bc: 91a09000     	movls	r9, r0
    d0c0: c3a0a001     	movgt	r10, #1
    d0c4: 83a09000     	movhi	r9, #0
    d0c8: d3a0a000     	movle	r10, #0
    d0cc: eef06aea     	vabs.f32	s13, s21
    d0d0: eef75ae7     	vcvt.f64.f32	d21, s15
    d0d4: ee07ca90     	vmov	s15, r12
    d0d8: ee224a07     	vmul.f32	s8, s4, s14
    d0dc: ed982a1b     	vldr	s4, [r8, #108]
    d0e0: ee7bbae2     	vsub.f32	s23, s23, s5
    d0e4: ed884a15     	vstr	s8, [r8, #84]
    d0e8: eef43be2     	vcmpe.f64	d19, d18
    d0ec: eeb85ae0     	vcvt.f32.s32	s10, s1
    d0f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d0f4: c3a0a001     	movgt	r10, #1
    d0f8: edd80a18     	vldr	s1, [r8, #96]
    d0fc: c1a0900a     	movgt	r9, r10
    d100: eef0aac9     	vabs.f32	s21, s18
    d104: ee097a10     	vmov	s18, r7
    d108: eef76ae6     	vcvt.f64.f32	d22, s13
    d10c: ee644a87     	vmul.f32	s9, s9, s14
    d110: ee3bba43     	vsub.f32	s22, s22, s6
    d114: edc84a16     	vstr	s9, [r8, #88]
    d118: eef45b64     	vcmp.f64	d21, d20
    d11c: eef85ae5     	vcvt.f32.s32	s11, s11
    d120: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d124: c3a0a001     	movgt	r10, #1
    d128: c3a09002     	movgt	r9, #2
    d12c: eef02ae8     	vabs.f32	s5, s17
    d130: eef77aea     	vcvt.f64.f32	d23, s21
    d134: ee255a07     	vmul.f32	s10, s10, s14
    d138: ee3aaa63     	vsub.f32	s20, s20, s7
    d13c: ed885a17     	vstr	s10, [r8, #92]
    d140: eef46b64     	vcmp.f64	d22, d20
    d144: eef83ac6     	vcvt.f32.s32	s7, s12
    d148: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d14c: c3a0a001     	movgt	r10, #1
    d150: c3a09003     	movgt	r9, #3
    d154: eef78ae2     	vcvt.f64.f32	d24, s5
    d158: eeb03aeb     	vabs.f32	s6, s23
    d15c: ee658a87     	vmul.f32	s17, s11, s14
    d160: ee799ac4     	vsub.f32	s19, s19, s8
    d164: edc88a18     	vstr	s17, [r8, #96]
    d168: eef47b64     	vcmp.f64	d23, d20
    d16c: eeb84ac9     	vcvt.f32.s32	s8, s18
    d170: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d174: c3a0a001     	movgt	r10, #1
    d178: c3a09004     	movgt	r9, #4
    d17c: eef06acb     	vabs.f32	s13, s22
    d180: eef79ac3     	vcvt.f64.f32	d25, s6
    d184: ee236a87     	vmul.f32	s12, s7, s14
    d188: ee388a64     	vsub.f32	s16, s16, s9
    d18c: ed886a19     	vstr	s12, [r8, #100]
    d190: eef48b64     	vcmp.f64	d24, d20
    d194: eef84ae7     	vcvt.f32.s32	s9, s15
    d198: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d19c: c3a0a001     	movgt	r10, #1
    d1a0: c3a09005     	movgt	r9, #5
    d1a4: eef0baca     	vabs.f32	s23, s20
    d1a8: eef7aae6     	vcvt.f64.f32	d26, s13
    d1ac: ee64aa07     	vmul.f32	s21, s8, s14
    d1b0: ee300a45     	vsub.f32	s0, s0, s10
    d1b4: edc8aa1a     	vstr	s21, [r8, #104]
    d1b8: eef49b64     	vcmp.f64	d25, d20
    d1bc: eef7baeb     	vcvt.f64.f32	d27, s23
    d1c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d1c4: c3a0a001     	movgt	r10, #1
    d1c8: c3a09006     	movgt	r9, #6
    d1cc: eeb05ae9     	vabs.f32	s10, s19
    d1d0: ee24ba87     	vmul.f32	s22, s9, s14
    d1d4: ee700ae8     	vsub.f32	s1, s1, s17
    d1d8: eef4ab64     	vcmp.f64	d26, d20
    d1dc: eef7cac5     	vcvt.f64.f32	d28, s10
    d1e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d1e4: c3a0a001     	movgt	r10, #1
    d1e8: c3a09007     	movgt	r9, #7
    d1ec: eeb07ac8     	vabs.f32	s14, s16
    d1f0: ee311a46     	vsub.f32	s2, s2, s12
    d1f4: eef4bb64     	vcmp.f64	d27, d20
    d1f8: eef7dac7     	vcvt.f64.f32	d29, s14
    d1fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d200: c3a0a001     	movgt	r10, #1
    d204: c3a09008     	movgt	r9, #8
    d208: eef05ac0     	vabs.f32	s11, s0
    d20c: ee711aea     	vsub.f32	s3, s3, s21
    d210: eef4cb64     	vcmp.f64	d28, d20
    d214: eef7eae5     	vcvt.f64.f32	d30, s11
    d218: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d21c: c3a0a001     	movgt	r10, #1
    d220: c3a09009     	movgt	r9, #9
    d224: eeb09ae0     	vabs.f32	s18, s1
    d228: ee322a4b     	vsub.f32	s4, s4, s22
    d22c: eef4db64     	vcmp.f64	d29, d20
    d230: eef7fac9     	vcvt.f64.f32	d31, s18
    d234: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d238: c3a0a001     	movgt	r10, #1
    d23c: c3a0900a     	movgt	r9, #10
    d240: eef02ac1     	vabs.f32	s5, s2
    d244: eef4eb64     	vcmp.f64	d30, d20
    d248: eef70ae2     	vcvt.f64.f32	d16, s5
    d24c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d250: c3a0a001     	movgt	r10, #1
    d254: c3a0900b     	movgt	r9, #11
    d258: eeb0aae1     	vabs.f32	s20, s3
    d25c: eef4fb64     	vcmp.f64	d31, d20
    d260: eef72aca     	vcvt.f64.f32	d18, s20
    d264: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d268: c3a0a001     	movgt	r10, #1
    d26c: c3a0900c     	movgt	r9, #12
    d270: eef03ac2     	vabs.f32	s7, s4
    d274: eef40b64     	vcmp.f64	d16, d20
    d278: eef71ae3     	vcvt.f64.f32	d17, s7
    d27c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d280: c3a0a001     	movgt	r10, #1
    d284: c3a0900d     	movgt	r9, #13
    d288: eef42b64     	vcmp.f64	d18, d20
    d28c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d290: eef41be4     	vcmpe.f64	d17, d20
    d294: c3a0a001     	movgt	r10, #1
    d298: c3a0900e     	movgt	r9, #14
    d29c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d2a0: cd88ba1b     	vstrgt	s22, [r8, #108]
    d2a4: c3a0000f     	movgt	r0, #15
    d2a8: da0000e8     	ble	0xd650 <arbhar_gpio_tilde_bang+0x838> @ imm = #0x3a0
    d2ac: e59fa02c     	ldr	r10, [pc, #0x2c]        @ 0xd2e0 <arbhar_gpio_tilde_bang+0x4c8>  // u32=0x1a100; f32?=1.49591414e-40
    d2b0: e08f900a     	add	r9, pc, r10
    d2b4: ea00000a     	b	0xd2e4 <arbhar_gpio_tilde_bang+0x4cc> @ imm = #0x28
    d2b8: 7b 14 ae 47  	.word	0x47ae147b
    d2bc: e1 7a 94 3f  	.word	0x3f947ae1
    d2c0: 00 00 00 3a  	.word	0x3a000000
    d2c4: c4 a4 01 00  	.word	0x0001a4c4
    d2c8: 8c a4 01 00  	.word	0x0001a48c
    d2cc: 6c a4 01 00  	.word	0x0001a46c
    d2d0: 4c a4 01 00  	.word	0x0001a44c
    d2d4: 04 a4 01 00  	.word	0x0001a404
    d2d8: a4 a4 01 00  	.word	0x0001a4a4
    d2dc: b0 a3 01 00  	.word	0x0001a3b0
    d2e0: 00 a1 01 00  	.word	0x0001a100
    d2e4: e58900e4     	str	r0, [r9, #0xe4]
    d2e8: e3a08f71     	mov	r8, #452
    d2ec: e300e126     	movw	lr, #0x126
    d2f0: e0234098     	mla	r3, r8, r0, r4
    d2f4: eddf3bf9     	vldr	d19, [pc, #996]         @ 0xd6e0 <arbhar_gpio_tilde_bang+0x8c8>  // f64=35.5
    d2f8: e59f53f8     	ldr	r5, [pc, #0x3f8]        @ 0xd6f8 <arbhar_gpio_tilde_bang+0x8e0>  // u32=0x77ac; f32?=4.29301798e-41
    d2fc: e3a01001     	mov	r1, #1
    d300: e3a02443     	mov	r2, #1124073472
    d304: ed9f3af7     	vldr	s6, [pc, #988]          @ 0xd6e8 <arbhar_gpio_tilde_bang+0x8d0>  // f32=0
    d308: e08f0005     	add	r0, pc, r5
    d30c: e59bbdac     	ldr	r11, [r11, #0xdac]
    d310: e58d1040     	str	r1, [sp, #0x40]
    d314: e19370be     	ldrh	r7, [r3, lr]
    d318: eddf9af3     	vldr	s19, [pc, #972]         @ 0xd6ec <arbhar_gpio_tilde_bang+0x8d4>  // f32=71
    d31c: e2676eff     	rsb	r6, r7, #4080
    d320: e58d1048     	str	r1, [sp, #0x48]
    d324: e286c00f     	add	r12, r6, #15
    d328: e58d204c     	str	r2, [sp, #0x4c]
    d32c: ee07ca90     	vmov	s15, r12
    d330: eefa7aea     	vcvt.f32.s32	s15, s15, #11
    d334: eef74ae7     	vcvt.f64.f32	d20, s15
    d338: ee645ba3     	vmul.f64	d21, d20, d19
    d33c: eefd8be5     	vcvt.s32.f64	s17, d21
    d340: eeb84ae8     	vcvt.f32.s32	s8, s17
    d344: eeb44ac3     	vcmpe.f32	s8, s6
    d348: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d34c: beb04a43     	vmovlt.f32	s8, s6
    d350: eeb44ae9     	vcmpe.f32	s8, s19
    d354: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d358: 8eb04a69     	vmovhi.f32	s8, s19
    d35c: eefd6ac4     	vcvt.s32.f32	s13, s8
    d360: eeb88ae6     	vcvt.f32.s32	s16, s13
    d364: ed8d8a11     	vstr	s16, [sp, #68]
    d368: ebffd8ee     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x9c48  // CALL gensym
    d36c: e28d3040     	add	r3, sp, #64
    d370: e3a02002     	mov	r2, #2
    d374: e1a01000     	mov	r1, r0
    d378: e1a0000b     	mov	r0, r11
    d37c: ebffda3c     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x9710  // CALL outlet_list
    d380: e5d40030     	ldrb	r0, [r4, #0x30]
    d384: e3500062     	cmp	r0, #98
    d388: 8a00001f     	bhi	0xd40c <arbhar_gpio_tilde_bang+0x5f4> @ imm = #0x7c
    d38c: e28dd064     	add	sp, sp, #100
    d390: ecbd8b08     	vpop	{d8, d9, d10, d11}
    d394: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    d398: e59f135c     	ldr	r1, [pc, #0x35c]        @ 0xd6fc <arbhar_gpio_tilde_bang+0x8e4>  // u32=0x1a014; f32?=1.49260707e-40
    d39c: e08f6001     	add	r6, pc, r1
    d3a0: e596c0dc     	ldr	r12, [r6, #0xdc]
    d3a4: e35c0002     	cmp	r12, #2
    d3a8: cafffeb6     	bgt	0xce88 <arbhar_gpio_tilde_bang+0x70> @ imm = #-0x528
    d3ac: e59f834c     	ldr	r8, [pc, #0x34c]        @ 0xd700 <arbhar_gpio_tilde_bang+0x8e8>  // u32=0x85b0; f32?=4.79580386e-41
    d3b0: e3a07001     	mov	r7, #1
    d3b4: e5941020     	ldr	r1, [r4, #0x20]
    d3b8: e08f0008     	add	r0, pc, r8
    d3bc: e59fb340     	ldr	r11, [pc, #0x340]       @ 0xd704 <arbhar_gpio_tilde_bang+0x8ec>  // u32=0x85ac; f32?=4.79524334e-41
    d3c0: ebffd9ec     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x9850  // CALL post
    d3c4: e1a00004     	mov	r0, r4
    d3c8: ed9f0ac8     	vldr	s0, [pc, #800]          @ 0xd6f0 <arbhar_gpio_tilde_bang+0x8d8>  // f32=104
    d3cc: ebffd905     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x9bec  // CALL readFromSharedMem
    d3d0: e08f000b     	add	r0, pc, r11
    d3d4: eef70ac0     	vcvt.f64.f32	d16, s0
    d3d8: ec532b30     	vmov	r2, r3, d16
    d3dc: ebffd9e5     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x986c  // CALL post
    d3e0: e2843d77     	add	r3, r4, #7616
    d3e4: e59f231c     	ldr	r2, [pc, #0x31c]        @ 0xd708 <arbhar_gpio_tilde_bang+0x8f0>  // u32=0x85a4; f32?=4.79412231e-41
    d3e8: ed936a02     	vldr	s12, [r3, #8]
    d3ec: e08f0002     	add	r0, pc, r2
    d3f0: eeb78ac6     	vcvt.f64.f32	d8, s12
    d3f4: ec532b18     	vmov	r2, r3, d8
    d3f8: ebffd9de     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x9888  // CALL post
    d3fc: e59600dc     	ldr	r0, [r6, #0xdc]
    d400: e0805007     	add	r5, r0, r7
    d404: e58650dc     	str	r5, [r6, #0xdc]
    d408: eafffe9f     	b	0xce8c <arbhar_gpio_tilde_bang+0x74> @ imm = #-0x584
    d40c: e3a00021     	mov	r0, #33
    d410: ebffd8dc     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x9c90  // CALL bcm2835_gpio_lev
    d414: e1a0a000     	mov	r10, r0
    d418: e3a0002a     	mov	r0, #42
    d41c: ebffd8d9     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x9c9c  // CALL bcm2835_gpio_lev
    d420: e16f9f10     	clz	r9, r0
    d424: e1a08000     	mov	r8, r0
    d428: e3a00026     	mov	r0, #38
    d42c: ebffd8d5     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x9cac  // CALL bcm2835_gpio_lev
    d430: e5d43030     	ldrb	r3, [r4, #0x30]
    d434: e1a052a9     	lsr	r5, r9, #5
    d438: e3530063     	cmp	r3, #99
    d43c: e1a0b000     	mov	r11, r0
    d440: 0a000175     	beq	0xda1c <arbhar_gpio_tilde_bang+0xc04> @ imm = #0x5d4
    d444: e3530064     	cmp	r3, #100
    d448: 0a000184     	beq	0xda60 <arbhar_gpio_tilde_bang+0xc48> @ imm = #0x610
    d44c: e35a0000     	cmp	r10, #0
    d450: 11a03005     	movne	r3, r5
    d454: 03a03000     	moveq	r3, #0
    d458: e3530000     	cmp	r3, #0
    d45c: 0a0000cc     	beq	0xd794 <arbhar_gpio_tilde_bang+0x97c> @ imm = #0x330
    d460: e35b0000     	cmp	r11, #0
    d464: 0affffc8     	beq	0xd38c <arbhar_gpio_tilde_bang+0x574> @ imm = #-0xe0
    d468: e5941020     	ldr	r1, [r4, #0x20]
    d46c: e1a00004     	mov	r0, r4
    d470: ebffd9f0     	bl	0x3c38 <.plt+0x53c>     @ imm = #-0x9840  // CALL _setVpo3VoltValue
    d474: eaffffc4     	b	0xd38c <arbhar_gpio_tilde_bang+0x574> @ imm = #-0xf0
    d478: e5d56d71     	ldrb	r6, [r5, #0xd71]
    d47c: e3560000     	cmp	r6, #0
    d480: 0afffe71     	beq	0xce4c <arbhar_gpio_tilde_bang+0x34> @ imm = #-0x63c
    d484: e5d57d81     	ldrb	r7, [r5, #0xd81]
    d488: e3570000     	cmp	r7, #0
    d48c: 0afffe6e     	beq	0xce4c <arbhar_gpio_tilde_bang+0x34> @ imm = #-0x648
    d490: e59fb274     	ldr	r11, [pc, #0x274]       @ 0xd70c <arbhar_gpio_tilde_bang+0x8f4>  // u32=0x848c; f32?=4.75488595e-41
    d494: e08f000b     	add	r0, pc, r11
    d498: ebffd9b6     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x9928  // CALL post
    d49c: e3a0c001     	mov	r12, #1
    d4a0: e5c4c075     	strb	r12, [r4, #0x75]
    d4a4: eafffe6d     	b	0xce60 <arbhar_gpio_tilde_bang+0x48> @ imm = #-0x64c
    d4a8: e5d41037     	ldrb	r1, [r4, #0x37]
    d4ac: e1a00004     	mov	r0, r4
    d4b0: e281607c     	add	r6, r1, #124
    d4b4: ee066a90     	vmov	s13, r6
    d4b8: eeb80ae6     	vcvt.f32.s32	s0, s13
    d4bc: ebffd8c9     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x9cdc  // CALL readFromSharedMem
    d4c0: e5d4c03c     	ldrb	r12, [r4, #0x3c]
    d4c4: e35c0001     	cmp	r12, #1
    d4c8: 0a000107     	beq	0xd8ec <arbhar_gpio_tilde_bang+0xad4> @ imm = #0x41c
    d4cc: e59f623c     	ldr	r6, [pc, #0x23c]        @ 0xd710 <arbhar_gpio_tilde_bang+0x8f8>  // u32=0x19ee0; f32?=1.48829107e-40
    d4d0: e08f8006     	add	r8, pc, r6
    d4d4: e598b0e0     	ldr	r11, [r8, #0xe0]
    d4d8: e35b0001     	cmp	r11, #1
    d4dc: 0a000168     	beq	0xda84 <arbhar_gpio_tilde_bang+0xc6c> @ imm = #0x5a0
    d4e0: e5d40030     	ldrb	r0, [r4, #0x30]
    d4e4: e3500064     	cmp	r0, #100
    d4e8: 1affffa5     	bne	0xd384 <arbhar_gpio_tilde_bang+0x56c> @ imm = #-0x16c
    d4ec: eafffe6e     	b	0xceac <arbhar_gpio_tilde_bang+0x94> @ imm = #-0x648
    d4f0: e3a01029     	mov	r1, #41
    d4f4: e1a00004     	mov	r0, r4
    d4f8: ebffd97a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9a18  // CALL writeToSharedMem
    d4fc: e3a0102a     	mov	r1, #42
    d500: e5d4203c     	ldrb	r2, [r4, #0x3c]
    d504: e1a00004     	mov	r0, r4
    d508: ebffd976     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9a28  // CALL writeToSharedMem
    d50c: e5d4103c     	ldrb	r1, [r4, #0x3c]
    d510: e2845a01     	add	r5, r4, #4096
    d514: e300310a     	movw	r3, #0x10a
    d518: e3510002     	cmp	r1, #2
    d51c: e19500b3     	ldrh	r0, [r5, r3]
    d520: 0a0000dc     	beq	0xd898 <arbhar_gpio_tilde_bang+0xa80> @ imm = #0x370
    d524: e59fe1e8     	ldr	lr, [pc, #0x1e8]        @ 0xd714 <arbhar_gpio_tilde_bang+0x8fc>  // u32=0x19dbc; f32?=1.48419928e-40
    d528: e08f200e     	add	r2, pc, lr
    d52c: e5926020     	ldr	r6, [r2, #0x20]
    d530: e0467000     	sub	r7, r6, r0
    d534: e3570000     	cmp	r7, #0
    d538: b2677000     	rsblt	r7, r7, #0
    d53c: e3570078     	cmp	r7, #120
    d540: ca0000bc     	bgt	0xd838 <arbhar_gpio_tilde_bang+0xa20> @ imm = #0x2f0
    d544: e1a00004     	mov	r0, r4
    d548: ed9f0a69     	vldr	s0, [pc, #420]          @ 0xd6f4 <arbhar_gpio_tilde_bang+0x8dc>  // f32=158
    d54c: ebffd8a5     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x9d6c  // CALL readFromSharedMem
    d550: e59f01c0     	ldr	r0, [pc, #0x1c0]        @ 0xd718 <arbhar_gpio_tilde_bang+0x900>  // u32=0x19e58; f32?=1.48638531e-40
    d554: e5d49054     	ldrb	r9, [r4, #0x54]
    d558: e08f6000     	add	r6, pc, r0
    d55c: e59670bc     	ldr	r7, [r6, #0xbc]
    d560: eefd3ac0     	vcvt.s32.f32	s7, s0
    d564: ee138a90     	vmov	r8, s7
    d568: e1570008     	cmp	r7, r8
    d56c: 158680bc     	strne	r8, [r6, #0xbc]
    d570: e3590000     	cmp	r9, #0
    d574: 0a00003a     	beq	0xd664 <arbhar_gpio_tilde_bang+0x84c> @ imm = #0xe8
    d578: e5d4103c     	ldrb	r1, [r4, #0x3c]
    d57c: e2513001     	subs	r3, r1, #1
    d580: 13a03001     	movne	r3, #1
    d584: e3510003     	cmp	r1, #3
    d588: 83a03000     	movhi	r3, #0
    d58c: e3530000     	cmp	r3, #0
    d590: 13a01001     	movne	r1, #1
    d594: 1a0000eb     	bne	0xd948 <arbhar_gpio_tilde_bang+0xb30> @ imm = #0x3ac
    d598: e5d57df5     	ldrb	r7, [r5, #0xdf5]
    d59c: e3570000     	cmp	r7, #0
    d5a0: 0a00003f     	beq	0xd6a4 <arbhar_gpio_tilde_bang+0x88c> @ imm = #0xfc
    d5a4: e3510003     	cmp	r1, #3
    d5a8: 0a000146     	beq	0xdac8 <arbhar_gpio_tilde_bang+0xcb0> @ imm = #0x518
    d5ac: e59f8168     	ldr	r8, [pc, #0x168]        @ 0xd71c <arbhar_gpio_tilde_bang+0x904>  // u32=0x19dfc; f32?=1.48509611e-40
    d5b0: e3510004     	cmp	r1, #4
    d5b4: e08fa008     	add	r10, pc, r8
    d5b8: e5ca90c0     	strb	r9, [r10, #0xc0]
    d5bc: e5ca70c1     	strb	r7, [r10, #0xc1]
    d5c0: 0a0000ec     	beq	0xd978 <arbhar_gpio_tilde_bang+0xb60> @ imm = #0x3b0
    d5c4: e5d55de1     	ldrb	r5, [r5, #0xde1]
    d5c8: e3550000     	cmp	r5, #0
    d5cc: 1a0000d9     	bne	0xd938 <arbhar_gpio_tilde_bang+0xb20> @ imm = #0x364
    d5d0: e5d42030     	ldrb	r2, [r4, #0x30]
    d5d4: e3520062     	cmp	r2, #98
    d5d8: 8afffe23     	bhi	0xce6c <arbhar_gpio_tilde_bang+0x54> @ imm = #-0x774
    d5dc: e5d4e03c     	ldrb	lr, [r4, #0x3c]
    d5e0: e35e0000     	cmp	lr, #0
    d5e4: 0a00009e     	beq	0xd864 <arbhar_gpio_tilde_bang+0xa4c> @ imm = #0x278
    d5e8: e35e0003     	cmp	lr, #3
    d5ec: 859f012c     	ldrhi	r0, [pc, #0x12c]        @ 0xd720 <arbhar_gpio_tilde_bang+0x908>
    d5f0: 83a07001     	movhi	r7, #1
    d5f4: 808f5000     	addhi	r5, pc, r0
    d5f8: 85c570c8     	strbhi	r7, [r5, #0xc8]
    d5fc: e1a00004     	mov	r0, r4
    d600: ebffd980     	bl	0x3c08 <.plt+0x50c>     @ imm = #-0x9a00  // CALL led_directionControl
    d604: e5d4c030     	ldrb	r12, [r4, #0x30]
    d608: e35c0064     	cmp	r12, #100
    d60c: 1afffe18     	bne	0xce74 <arbhar_gpio_tilde_bang+0x5c> @ imm = #-0x7a0
    d610: e59f810c     	ldr	r8, [pc, #0x10c]        @ 0xd724 <arbhar_gpio_tilde_bang+0x90c>  // u32=0x19d9c; f32?=1.48375087e-40
    d614: e08fb008     	add	r11, pc, r8
    d618: e59b30d4     	ldr	r3, [r11, #0xd4]
    d61c: e2832001     	add	r2, r3, #1
    d620: e58b20d4     	str	r2, [r11, #0xd4]
    d624: e352001e     	cmp	r2, #30
    d628: dafffe11     	ble	0xce74 <arbhar_gpio_tilde_bang+0x5c> @ imm = #-0x7bc
    d62c: e59be0d8     	ldr	lr, [r11, #0xd8]
    d630: e3a00000     	mov	r0, #0
    d634: e58b00d4     	str	r0, [r11, #0xd4]
    d638: e26e5001     	rsb	r5, lr, #1
    d63c: e58b50d8     	str	r5, [r11, #0xd8]
    d640: ee055a90     	vmov	s11, r5
    d644: eeb80ae5     	vcvt.f32.s32	s0, s11
    d648: ebffd920     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x9b80  // CALL _setTriggerOut
    d64c: eafffe08     	b	0xce74 <arbhar_gpio_tilde_bang+0x5c> @ imm = #-0x7e0
    d650: e35a0000     	cmp	r10, #0
    d654: ed88ba1b     	vstr	s22, [r8, #108]
    d658: 0affff22     	beq	0xd2e8 <arbhar_gpio_tilde_bang+0x4d0> @ imm = #-0x378
    d65c: e1a00009     	mov	r0, r9
    d660: eaffff11     	b	0xd2ac <arbhar_gpio_tilde_bang+0x494> @ imm = #-0x3bc
    d664: e5d57df5     	ldrb	r7, [r5, #0xdf5]
    d668: e3570000     	cmp	r7, #0
    d66c: 0a0000bb     	beq	0xd960 <arbhar_gpio_tilde_bang+0xb48> @ imm = #0x2ec
    d670: e5d4103c     	ldrb	r1, [r4, #0x3c]
    d674: e3510003     	cmp	r1, #3
    d678: 0a000112     	beq	0xdac8 <arbhar_gpio_tilde_bang+0xcb0> @ imm = #0x448
    d67c: e59fc0a4     	ldr	r12, [pc, #0xa4]        @ 0xd728 <arbhar_gpio_tilde_bang+0x910>  // u32=0x19d30; f32?=1.48223746e-40
    d680: e08fe00c     	add	lr, pc, r12
    d684: e5de90c0     	ldrb	r9, [lr, #0xc0]
    d688: e3590000     	cmp	r9, #0
    d68c: 0affffc6     	beq	0xd5ac <arbhar_gpio_tilde_bang+0x794> @ imm = #-0xe8
    d690: e3a01000     	mov	r1, #0
    d694: e1a00004     	mov	r0, r4
    d698: ebffd93c     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9b10  // CALL _setStrikeLed
    d69c: e5d57df5     	ldrb	r7, [r5, #0xdf5]
    d6a0: e5d4103c     	ldrb	r1, [r4, #0x3c]
    d6a4: e59f2080     	ldr	r2, [pc, #0x80]         @ 0xd72c <arbhar_gpio_tilde_bang+0x914>  // u32=0x19d08; f32?=1.48167694e-40
    d6a8: e08f0002     	add	r0, pc, r2
    d6ac: e5d060c1     	ldrb	r6, [r0, #0xc1]
    d6b0: e0569007     	subs	r9, r6, r7
    d6b4: 13a09001     	movne	r9, #1
    d6b8: e3570000     	cmp	r7, #0
    d6bc: 13a09000     	movne	r9, #0
    d6c0: e3590000     	cmp	r9, #0
    d6c4: 05d49054     	ldrbeq	r9, [r4, #0x54]
    d6c8: 0affffb7     	beq	0xd5ac <arbhar_gpio_tilde_bang+0x794> @ imm = #-0x124
    d6cc: e3510003     	cmp	r1, #3
    d6d0: 0a00009b     	beq	0xd944 <arbhar_gpio_tilde_bang+0xb2c> @ imm = #0x26c
    d6d4: e5d49054     	ldrb	r9, [r4, #0x54]
    d6d8: e3a07000     	mov	r7, #0
    d6dc: eaffffb2     	b	0xd5ac <arbhar_gpio_tilde_bang+0x794> @ imm = #-0x138
    d6e0: 00 00 00 00  	.word	0x00000000
    d6e4: 00 c0 41 40  	.word	0x4041c000
    d6e8: 00 00 00 00  	.word	0x00000000
    d6ec: 00 00 8e 42  	.word	0x428e0000
    d6f0: 00 00 d0 42  	.word	0x42d00000
    d6f4: 00 00 1e 43  	.word	0x431e0000
    d6f8: ac 77 00 00  	.word	0x000077ac
    d6fc: 14 a0 01 00  	.word	0x0001a014
    d700: b0 85 00 00  	.word	0x000085b0
    d704: ac 85 00 00  	.word	0x000085ac
    d708: a4 85 00 00  	.word	0x000085a4
    d70c: 8c 84 00 00  	.word	0x0000848c
    d710: e0 9e 01 00  	.word	0x00019ee0
    d714: bc 9d 01 00  	.word	0x00019dbc
    d718: 58 9e 01 00  	.word	0x00019e58
    d71c: fc 9d 01 00  	.word	0x00019dfc
    d720: bc 9d 01 00  	.word	0x00019dbc
    d724: 9c 9d 01 00  	.word	0x00019d9c
    d728: 30 9d 01 00  	.word	0x00019d30
    d72c: 08 9d 01 00  	.word	0x00019d08
    d730: fc 9b 01 00  	.word	0x00019bfc
    d734: dc 9b 01 00  	.word	0x00019bdc
    d738: 9c 9b 01 00  	.word	0x00019b9c
    d73c: 68 9b 01 00  	.word	0x00019b68
    d740: 3c 9b 01 00  	.word	0x00019b3c
    d744: 08 9b 01 00  	.word	0x00019b08
    d748: 08 9a 01 00  	.word	0x00019a08
    d74c: b4 9a 01 00  	.word	0x00019ab4
    d750: b8 99 01 00  	.word	0x000199b8
    d754: 4c 9a 01 00  	.word	0x00019a4c
    d758: c4 98 01 00  	.word	0x000198c4
    d75c: 5c 99 01 00  	.word	0x0001995c
    d760: fc 98 01 00  	.word	0x000198fc
    d764: bc 98 01 00  	.word	0x000198bc
    d768: 98 98 01 00  	.word	0x00019898
    d76c: 64 98 01 00  	.word	0x00019864
    d770: ac 54 00 00  	.word	0x000054ac
    d774: 4c 6f 00 00  	.word	0x00006f4c
    d778: 2c 6f 00 00  	.word	0x00006f2c
    d77c: 44 6f 00 00  	.word	0x00006f44
    d780: 6c 6e 00 00  	.word	0x00006e6c
    d784: 1c 6e 00 00  	.word	0x00006e1c
    d788: 5c 96 01 00  	.word	0x0001965c
    d78c: 00 00 44 3c  	.word	0x3c440000
    d790: 00 00 2c 42  	.word	0x422c0000
    d794: e35a0000     	cmp	r10, #0
    d798: 03a05001     	moveq	r5, #1
    d79c: e35b0000     	cmp	r11, #0
    d7a0: 03a05001     	moveq	r5, #1
    d7a4: e3550000     	cmp	r5, #0
    d7a8: 1a0000cc     	bne	0xdae0 <arbhar_gpio_tilde_bang+0xcc8> @ imm = #0x330
    d7ac: e51f7084     	ldr	r7, [pc, #-0x84]        @ 0xd730 <arbhar_gpio_tilde_bang+0x918>  // u32=0x19bfc; f32?=1.47792146e-40
    d7b0: e3a09001     	mov	r9, #1
    d7b4: e08f3007     	add	r3, pc, r7
    d7b8: e58390ec     	str	r9, [r3, #0xec]
    d7bc: e28dd064     	add	sp, sp, #100
    d7c0: ecbd8b08     	vpop	{d8, d9, d10, d11}
    d7c4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    d7c8: e3a00020     	mov	r0, #32
    d7cc: e51f70a0     	ldr	r7, [pc, #-0xa0]        @ 0xd734 <arbhar_gpio_tilde_bang+0x91c>  // u32=0x19bdc; f32?=1.47747305e-40
    d7d0: ebffd7ec     	bl	0x3788 <.plt+0x8c>      @ imm = #-0xa050  // CALL bcm2835_gpio_lev
    d7d4: e08fa007     	add	r10, pc, r7
    d7d8: e16f9f10     	clz	r9, r0
    d7dc: e3a00024     	mov	r0, #36
    d7e0: ebffd7e8     	bl	0x3788 <.plt+0x8c>      @ imm = #-0xa060  // CALL bcm2835_gpio_lev
    d7e4: e59a10cc     	ldr	r1, [r10, #0xcc]
    d7e8: e1a062a9     	lsr	r6, r9, #5
    d7ec: e1510006     	cmp	r1, r6
    d7f0: e16fcf10     	clz	r12, r0
    d7f4: e1a082ac     	lsr	r8, r12, #5
    d7f8: 0a000004     	beq	0xd810 <arbhar_gpio_tilde_bang+0x9f8> @ imm = #0x10
    d7fc: ee056a10     	vmov	s10, r6
    d800: e1a00004     	mov	r0, r4
    d804: eeb80ac5     	vcvt.f32.s32	s0, s10
    d808: ebffd937     	bl	0x3cec <.plt+0x5f0>     @ imm = #-0x9b24  // CALL _setCaptureLed
    d80c: e58a60cc     	str	r6, [r10, #0xcc]
    d810: e51f30e0     	ldr	r3, [pc, #-0xe0]        @ 0xd738 <arbhar_gpio_tilde_bang+0x920>  // u32=0x19b9c; f32?=1.47657622e-40
    d814: e08fb003     	add	r11, pc, r3
    d818: e59b20d0     	ldr	r2, [r11, #0xd0]
    d81c: e1520008     	cmp	r2, r8
    d820: 0affff77     	beq	0xd604 <arbhar_gpio_tilde_bang+0x7ec> @ imm = #-0x224
    d824: e1a01008     	mov	r1, r8
    d828: e1a00004     	mov	r0, r4
    d82c: ebffd8d7     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9ca4  // CALL _setStrikeLed
    d830: e58b80d0     	str	r8, [r11, #0xd0]
    d834: eaffff72     	b	0xd604 <arbhar_gpio_tilde_bang+0x7ec> @ imm = #-0x238
    d838: e51f9104     	ldr	r9, [pc, #-0x104]       @ 0xd73c <arbhar_gpio_tilde_bang+0x924>  // u32=0x19b68; f32?=1.47584754e-40
    d83c: e3a02000     	mov	r2, #0
    d840: e3a0109e     	mov	r1, #158
    d844: e1a00004     	mov	r0, r4
    d848: e08fa009     	add	r10, pc, r9
    d84c: ebffd8a5     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9d6c  // CALL writeToSharedMem
    d850: e3a08000     	mov	r8, #0
    d854: e3a0b000     	mov	r11, #0
    d858: e58480c0     	str	r8, [r4, #0xc0]
    d85c: e58ab0b8     	str	r11, [r10, #0xb8]
    d860: eaffff37     	b	0xd544 <arbhar_gpio_tilde_bang+0x72c> @ imm = #-0x324
    d864: e51fa12c     	ldr	r10, [pc, #-0x12c]      @ 0xd740 <arbhar_gpio_tilde_bang+0x928>  // u32=0x19b3c; f32?=1.47523097e-40
    d868: e1a00004     	mov	r0, r4
    d86c: ebffd861     	bl	0x39f8 <.plt+0x2fc>     @ imm = #-0x9e7c  // CALL led_ScanLengthControl
    d870: e1a00004     	mov	r0, r4
    d874: e08f900a     	add	r9, pc, r10
    d878: ebffd813     	bl	0x38cc <.plt+0x1d0>     @ imm = #-0x9fb4  // CALL led_PlayPosControl
    d87c: e5d910c8     	ldrb	r1, [r9, #0xc8]
    d880: e3510000     	cmp	r1, #0
    d884: 0affff5c     	beq	0xd5fc <arbhar_gpio_tilde_bang+0x7e4> @ imm = #-0x290
    d888: e5d4603c     	ldrb	r6, [r4, #0x3c]
    d88c: e3560000     	cmp	r6, #0
    d890: 05c960c8     	strbeq	r6, [r9, #0xc8]
    d894: eaffff58     	b	0xd5fc <arbhar_gpio_tilde_bang+0x7e4> @ imm = #-0x2a0
    d898: e260cb02     	rsb	r12, r0, #2048
    d89c: ed1f7a46     	vldr	s14, [pc, #-280]        @ 0xd78c <arbhar_gpio_tilde_bang+0x974>  // f32=0.0119628906
    d8a0: e51f1164     	ldr	r1, [pc, #-0x164]       @ 0xd744 <arbhar_gpio_tilde_bang+0x92c>  // u32=0x19b08; f32?=1.4745023e-40
    d8a4: ee07ca90     	vmov	s15, r12
    d8a8: e08fe001     	add	lr, pc, r1
    d8ac: eeb80ae7     	vcvt.f32.s32	s0, s15
    d8b0: edde1a2e     	vldr	s3, [lr, #184]
    d8b4: ee600a07     	vmul.f32	s1, s0, s14
    d8b8: eebd1ae0     	vcvt.s32.f32	s2, s1
    d8bc: eeb82ae1     	vcvt.f32.s32	s4, s3
    d8c0: eef82ac1     	vcvt.f32.s32	s5, s2
    d8c4: eef42a42     	vcmp.f32	s5, s4
    d8c8: edc42a30     	vstr	s5, [r4, #192]
    d8cc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    d8d0: 0affff1b     	beq	0xd544 <arbhar_gpio_tilde_bang+0x72c> @ imm = #-0x394
    d8d4: eebd3ae2     	vcvt.s32.f32	s6, s5
    d8d8: e51f3198     	ldr	r3, [pc, #-0x198]       @ 0xd748 <arbhar_gpio_tilde_bang+0x930>  // u32=0x19a08; f32?=1.47091497e-40
    d8dc: e08f2003     	add	r2, pc, r3
    d8e0: e5820020     	str	r0, [r2, #0x20]
    d8e4: ed8e3a2e     	vstr	s6, [lr, #184]
    d8e8: eaffff15     	b	0xd544 <arbhar_gpio_tilde_bang+0x72c> @ imm = #-0x3ac
    d8ec: eefd8ac0     	vcvt.s32.f32	s17, s0
    d8f0: e5990028     	ldr	r0, [r9, #0x28]
    d8f4: e51f51b0     	ldr	r5, [pc, #-0x1b0]       @ 0xd74c <arbhar_gpio_tilde_bang+0x934>  // u32=0x19ab4; f32?=1.47332521e-40
    d8f8: e1500006     	cmp	r0, r6
    d8fc: e08fe005     	add	lr, pc, r5
    d900: e58ec0e0     	str	r12, [lr, #0xe0]
    d904: ee187a90     	vmov	r7, s17
    d908: 0a000070     	beq	0xdad0 <arbhar_gpio_tilde_bang+0xcb8> @ imm = #0x1c0
    d90c: e3570001     	cmp	r7, #1
    d910: 0a000083     	beq	0xdb24 <arbhar_gpio_tilde_bang+0xd0c> @ imm = #0x20c
    d914: e3570002     	cmp	r7, #2
    d918: 1a000055     	bne	0xda74 <arbhar_gpio_tilde_bang+0xc5c> @ imm = #0x154
    d91c: e3a01007     	mov	r1, #7
    d920: e1a00004     	mov	r0, r4
    d924: ebffd899     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9d9c  // CALL _setStrikeLed
    d928: e51f91e0     	ldr	r9, [pc, #-0x1e0]       @ 0xd750 <arbhar_gpio_tilde_bang+0x938>  // u32=0x199b8; f32?=1.46979393e-40
    d92c: e08f1009     	add	r1, pc, r9
    d930: e1c162f8     	strd	r6, r7, [r1, #40]
    d934: eafffee9     	b	0xd4e0 <arbhar_gpio_tilde_bang+0x6c8> @ imm = #-0x45c
    d938: e1a00004     	mov	r0, r4
    d93c: ebffd899     	bl	0x3ba8 <.plt+0x4ac>     @ imm = #-0x9d9c  // CALL checkFileProcessProgress
    d940: eaffff22     	b	0xd5d0 <arbhar_gpio_tilde_bang+0x7b8> @ imm = #-0x378
    d944: e3a01000     	mov	r1, #0
    d948: e1a00004     	mov	r0, r4
    d94c: ebffd88f     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9dc4  // CALL _setStrikeLed
    d950: e5d49054     	ldrb	r9, [r4, #0x54]
    d954: e5d57df5     	ldrb	r7, [r5, #0xdf5]
    d958: e5d4103c     	ldrb	r1, [r4, #0x3c]
    d95c: eaffff12     	b	0xd5ac <arbhar_gpio_tilde_bang+0x794> @ imm = #-0x3b8
    d960: e51fa214     	ldr	r10, [pc, #-0x214]      @ 0xd754 <arbhar_gpio_tilde_bang+0x93c>  // u32=0x19a4c; f32?=1.47186785e-40
    d964: e08fb00a     	add	r11, pc, r10
    d968: e5db70c0     	ldrb	r7, [r11, #0xc0]
    d96c: e3570000     	cmp	r7, #0
    d970: 0affff4a     	beq	0xd6a0 <arbhar_gpio_tilde_bang+0x888> @ imm = #-0x2d8
    d974: eaffff45     	b	0xd690 <arbhar_gpio_tilde_bang+0x878> @ imm = #-0x2ec
    d978: ed1f0a7c     	vldr	s0, [pc, #-496]         @ 0xd790 <arbhar_gpio_tilde_bang+0x978>  // f32=43
    d97c: e1a00004     	mov	r0, r4
    d980: ebffd798     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xa1a0  // CALL readFromSharedMem
    d984: eebd4ac0     	vcvt.s32.f32	s8, s0
    d988: ee141a10     	vmov	r1, s8
    d98c: e3510006     	cmp	r1, #6
    d990: daffff0b     	ble	0xd5c4 <arbhar_gpio_tilde_bang+0x7ac> @ imm = #-0x3d4
    d994: e5d4906a     	ldrb	r9, [r4, #0x6a]
    d998: e3590000     	cmp	r9, #0
    d99c: 1a000064     	bne	0xdb34 <arbhar_gpio_tilde_bang+0xd1c> @ imm = #0x190
    d9a0: e5d5bd61     	ldrb	r11, [r5, #0xd61]
    d9a4: e35b0000     	cmp	r11, #0
    d9a8: 0a000061     	beq	0xdb34 <arbhar_gpio_tilde_bang+0xd1c> @ imm = #0x184
    d9ac: e5d5cd71     	ldrb	r12, [r5, #0xd71]
    d9b0: e35c0000     	cmp	r12, #0
    d9b4: 1a00005e     	bne	0xdb34 <arbhar_gpio_tilde_bang+0xd1c> @ imm = #0x178
    d9b8: e5d57d81     	ldrb	r7, [r5, #0xd81]
    d9bc: e3570000     	cmp	r7, #0
    d9c0: 1a00005b     	bne	0xdb34 <arbhar_gpio_tilde_bang+0xd1c> @ imm = #0x16c
    d9c4: e5d4e034     	ldrb	lr, [r4, #0x34]
    d9c8: e30a6aab     	movw	r6, #0xaaab
    d9cc: e34a6aaa     	movt	r6, #0xaaaa
    d9d0: e59a80c4     	ldr	r8, [r10, #0xc4]
    d9d4: e0891e96     	umull	r1, r9, r6, lr
    d9d8: e288b001     	add	r11, r8, #1
    d9dc: e58ab0c4     	str	r11, [r10, #0xc4]
    d9e0: e1a0a129     	lsr	r10, r9, #2
    d9e4: e28a9001     	add	r9, r10, #1
    d9e8: e3590006     	cmp	r9, #6
    d9ec: ca000005     	bgt	0xda08 <arbhar_gpio_tilde_bang+0xbf0> @ imm = #0x14
    d9f0: e5d41035     	ldrb	r1, [r4, #0x35]
    d9f4: e3510000     	cmp	r1, #0
    d9f8: 135b0032     	cmpne	r11, #50
    d9fc: ca0000e0     	bgt	0xdd84 <arbhar_gpio_tilde_bang+0xf6c> @ imm = #0x380
    da00: e3510000     	cmp	r1, #0
    da04: 0afffeee     	beq	0xd5c4 <arbhar_gpio_tilde_bang+0x7ac> @ imm = #-0x448
    da08: e3a02000     	mov	r2, #0
    da0c: e3a01079     	mov	r1, #121
    da10: e1a00004     	mov	r0, r4
    da14: ebffd833     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x9f34  // CALL writeToSharedMem
    da18: eafffee9     	b	0xd5c4 <arbhar_gpio_tilde_bang+0x7ac> @ imm = #-0x45c
    da1c: e51fe2cc     	ldr	lr, [pc, #-0x2cc]       @ 0xd758 <arbhar_gpio_tilde_bang+0x940>  // u32=0x198c4; f32?=1.46637477e-40
    da20: e08f700e     	add	r7, pc, lr
    da24: ed976a1c     	vldr	s12, [r7, #112]
    da28: eeb80ac6     	vcvt.f32.s32	s0, s12
    da2c: ebffd773     	bl	0x3800 <.plt+0x104>     @ imm = #-0xa234  // CALL _setFwdLed
    da30: edd74a1c     	vldr	s9, [r7, #112]
    da34: eeb80ae4     	vcvt.f32.s32	s0, s9
    da38: ebffd79a     	bl	0x38a8 <.plt+0x1ac>     @ imm = #-0xa198  // CALL _setRevLed
    da3c: e5971070     	ldr	r1, [r7, #0x70]
    da40: e3510000     	cmp	r1, #0
    da44: da000017     	ble	0xdaa8 <arbhar_gpio_tilde_bang+0xc90> @ imm = #0x5c
    da48: e51f92f4     	ldr	r9, [pc, #-0x2f4]       @ 0xd75c <arbhar_gpio_tilde_bang+0x944>  // u32=0x1995c; f32?=1.46850474e-40
    da4c: e3a0e000     	mov	lr, #0
    da50: e587e070     	str	lr, [r7, #0x70]
    da54: e08f3009     	add	r3, pc, r9
    da58: e583e0e8     	str	lr, [r3, #0xe8]
    da5c: eafffe7a     	b	0xd44c <arbhar_gpio_tilde_bang+0x634> @ imm = #-0x618
    da60: eeb70a00     	vmov.f32	s0, #1.000000e+00
    da64: ebffd765     	bl	0x3800 <.plt+0x104>     @ imm = #-0xa26c  // CALL _setFwdLed
    da68: eeb70a00     	vmov.f32	s0, #1.000000e+00
    da6c: ebffd78d     	bl	0x38a8 <.plt+0x1ac>     @ imm = #-0xa1cc  // CALL _setRevLed
    da70: eafffe75     	b	0xd44c <arbhar_gpio_tilde_bang+0x634> @ imm = #-0x62c
    da74: e3a01000     	mov	r1, #0
    da78: e1a00004     	mov	r0, r4
    da7c: ebffd843     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9ef4  // CALL _setStrikeLed
    da80: eaffffa8     	b	0xd928 <arbhar_gpio_tilde_bang+0xb10> @ imm = #-0x160
    da84: e3a01000     	mov	r1, #0
    da88: e1a00004     	mov	r0, r4
    da8c: ebffd83f     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9f04  // CALL _setStrikeLed
    da90: e3e03000     	mvn	r3, #0
    da94: e3a02000     	mov	r2, #0
    da98: e5893028     	str	r3, [r9, #0x28]
    da9c: e58820e0     	str	r2, [r8, #0xe0]
    daa0: e589302c     	str	r3, [r9, #0x2c]
    daa4: eafffe8d     	b	0xd4e0 <arbhar_gpio_tilde_bang+0x6c8> @ imm = #-0x5cc
    daa8: e51f6350     	ldr	r6, [pc, #-0x350]       @ 0xd760 <arbhar_gpio_tilde_bang+0x948>  // u32=0x198fc; f32?=1.46715949e-40
    daac: e3a02002     	mov	r2, #2
    dab0: e5872070     	str	r2, [r7, #0x70]
    dab4: e08fc006     	add	r12, pc, r6
    dab8: e59c00e8     	ldr	r0, [r12, #0xe8]
    dabc: e2803001     	add	r3, r0, #1
    dac0: e58c30e8     	str	r3, [r12, #0xe8]
    dac4: eafffe60     	b	0xd44c <arbhar_gpio_tilde_bang+0x634> @ imm = #-0x680
    dac8: e3a01003     	mov	r1, #3
    dacc: eaffff9d     	b	0xd948 <arbhar_gpio_tilde_bang+0xb30> @ imm = #-0x18c
    dad0: e599a02c     	ldr	r10, [r9, #0x2c]
    dad4: e15a0007     	cmp	r10, r7
    dad8: 1affff8b     	bne	0xd90c <arbhar_gpio_tilde_bang+0xaf4> @ imm = #-0x1d4
    dadc: eafffe7f     	b	0xd4e0 <arbhar_gpio_tilde_bang+0x6c8> @ imm = #-0x604
    dae0: e18aa008     	orr	r10, r10, r8
    dae4: e18b800a     	orr	r8, r11, r10
    dae8: e31800ff     	tst	r8, #255
    daec: 1afffe26     	bne	0xd38c <arbhar_gpio_tilde_bang+0x574> @ imm = #-0x768
    daf0: e51fb394     	ldr	r11, [pc, #-0x394]      @ 0xd764 <arbhar_gpio_tilde_bang+0x94c>  // u32=0x198bc; f32?=1.46626266e-40
    daf4: e08f300b     	add	r3, pc, r11
    daf8: e59350ec     	ldr	r5, [r3, #0xec]
    dafc: e3550000     	cmp	r5, #0
    db00: dafffe21     	ble	0xd38c <arbhar_gpio_tilde_bang+0x574> @ imm = #-0x77c
    db04: e593e000     	ldr	lr, [r3]
    db08: e35e0000     	cmp	lr, #0
    db0c: 0a000011     	beq	0xdb58 <arbhar_gpio_tilde_bang+0xd40> @ imm = #0x44
    db10: e51fb3b0     	ldr	r11, [pc, #-0x3b0]      @ 0xd768 <arbhar_gpio_tilde_bang+0x950>  // u32=0x19898; f32?=1.46575819e-40
    db14: e28e2001     	add	r2, lr, #1
    db18: e08f000b     	add	r0, pc, r11
    db1c: e5802000     	str	r2, [r0]
    db20: eafffe19     	b	0xd38c <arbhar_gpio_tilde_bang+0x574> @ imm = #-0x79c
    db24: e3a01006     	mov	r1, #6
    db28: e1a00004     	mov	r0, r4
    db2c: ebffd817     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x9fa4  // CALL _setStrikeLed
    db30: eaffff7c     	b	0xd928 <arbhar_gpio_tilde_bang+0xb10> @ imm = #-0x210
    db34: e3a02000     	mov	r2, #0
    db38: e3a01079     	mov	r1, #121
    db3c: e1a00004     	mov	r0, r4
    db40: ebffd7e8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xa060  // CALL writeToSharedMem
    db44: e51f33e0     	ldr	r3, [pc, #-0x3e0]       @ 0xd76c <arbhar_gpio_tilde_bang+0x954>  // u32=0x19864; f32?=1.46502952e-40
    db48: e3a02000     	mov	r2, #0
    db4c: e08f0003     	add	r0, pc, r3
    db50: e58020c4     	str	r2, [r0, #0xc4]
    db54: eafffe9a     	b	0xd5c4 <arbhar_gpio_tilde_bang+0x7ac> @ imm = #-0x598
    db58: e1a00004     	mov	r0, r4
    db5c: e51f73f4     	ldr	r7, [pc, #-0x3f4]       @ 0xd770 <arbhar_gpio_tilde_bang+0x958>  // u32=0x54ac; f32?=3.03745455e-41
    db60: ebffd816     	bl	0x3bc0 <.plt+0x4c4>     @ imm = #-0x9fa8  // CALL _learnMid
    db64: e51f13f8     	ldr	r1, [pc, #-0x3f8]       @ 0xd774 <arbhar_gpio_tilde_bang+0x95c>  // u32=0x6f4c; f32?=3.99257958e-41
    db68: e08f6007     	add	r6, pc, r7
    db6c: e28da040     	add	r10, sp, #64
    db70: e08f0001     	add	r0, pc, r1
    db74: e2869010     	add	r9, r6, #16
    db78: ebffd7fe     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa008  // CALL post
    db7c: e51f040c     	ldr	r0, [pc, #-0x40c]       @ 0xd778 <arbhar_gpio_tilde_bang+0x960>  // u32=0x6f2c; f32?=3.98809543e-41
    db80: e51fc40c     	ldr	r12, [pc, #-0x40c]      @ 0xd77c <arbhar_gpio_tilde_bang+0x964>  // u32=0x6f44; f32?=3.99145855e-41
    db84: e58d9018     	str	r9, [sp, #0x18]
    db88: e08f8000     	add	r8, pc, r0
    db8c: e2849a01     	add	r9, r4, #4096
    db90: e08f200c     	add	r2, pc, r12
    db94: e58da014     	str	r10, [sp, #0x14]
    db98: e58d201c     	str	r2, [sp, #0x1c]
    db9c: e4d61001     	ldrb	r1, [r6], #1
    dba0: e3a0ef71     	mov	lr, #452
    dba4: e59d001c     	ldr	r0, [sp, #0x1c]
    dba8: e3a05001     	mov	r5, #1
    dbac: e02b419e     	mla	r11, lr, r1, r4
    dbb0: eddbba4a     	vldr	s23, [r11, #296]
    dbb4: e5db211c     	ldrb	r2, [r11, #0x11c]
    dbb8: e282a0c0     	add	r10, r2, #192
    dbbc: eebd0aeb     	vcvt.s32.f32	s0, s23
    dbc0: edcbbab0     	vstr	s23, [r11, #704]
    dbc4: e58da000     	str	r10, [sp]
    dbc8: ee103a10     	vmov	r3, s0
    dbcc: ed8d0a03     	vstr	s0, [sp, #12]
    dbd0: e203703f     	and	r7, r3, #63
    dbd4: e1a0b343     	asr	r11, r3, #6
    dbd8: e58d7008     	str	r7, [sp, #0x8]
    dbdc: e58db004     	str	r11, [sp, #0x4]
    dbe0: ee0a7a90     	vmov	s21, r7
    dbe4: ebffd7e3     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa074  // CALL post
    dbe8: ee00ba90     	vmov	s1, r11
    dbec: ee07aa10     	vmov	s14, r10
    dbf0: e1a00008     	mov	r0, r8
    dbf4: eeb85aea     	vcvt.f32.s32	s10, s21
    dbf8: e5997db0     	ldr	r7, [r9, #0xdb0]
    dbfc: e58d5040     	str	r5, [sp, #0x40]
    dc00: e58d5048     	str	r5, [sp, #0x48]
    dc04: e58d5050     	str	r5, [sp, #0x50]
    dc08: eeb8bae0     	vcvt.f32.s32	s22, s1
    dc0c: ed8d5a13     	vstr	s10, [sp, #76]
    dc10: eeb81ac7     	vcvt.f32.s32	s2, s14
    dc14: ed8dba15     	vstr	s22, [sp, #84]
    dc18: ed8d1a11     	vstr	s2, [sp, #68]
    dc1c: ebffd6c1     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa4fc  // CALL gensym
    dc20: e59d3014     	ldr	r3, [sp, #0x14]
    dc24: e3a02003     	mov	r2, #3
    dc28: e1a01000     	mov	r1, r0
    dc2c: e1a00007     	mov	r0, r7
    dc30: ebffd80f     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x9fc4  // CALL outlet_list
    dc34: e59d3018     	ldr	r3, [sp, #0x18]
    dc38: e1560003     	cmp	r6, r3
    dc3c: 1affffd6     	bne	0xdb9c <arbhar_gpio_tilde_bang+0xd84> @ imm = #-0xa8
    dc40: e5946020     	ldr	r6, [r4, #0x20]
    dc44: e1a00008     	mov	r0, r8
    dc48: e599bdb0     	ldr	r11, [r9, #0xdb0]
    dc4c: e3a01000     	mov	r1, #0
    dc50: e206c03f     	and	r12, r6, #63
    dc54: e3441350     	movt	r1, #0x4350
    dc58: e1a0a346     	asr	r10, r6, #6
    dc5c: e58d1044     	str	r1, [sp, #0x44]
    dc60: ee09ca10     	vmov	s18, r12
    dc64: e58d5040     	str	r5, [sp, #0x40]
    dc68: ee01aa90     	vmov	s3, r10
    dc6c: e58d5048     	str	r5, [sp, #0x48]
    dc70: eef85ac9     	vcvt.f32.s32	s11, s18
    dc74: e58d5050     	str	r5, [sp, #0x50]
    dc78: eeb82ae1     	vcvt.f32.s32	s4, s3
    dc7c: edcd5a13     	vstr	s11, [sp, #76]
    dc80: ed8d2a15     	vstr	s4, [sp, #84]
    dc84: ebffd6a7     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa564  // CALL gensym
    dc88: e59d3014     	ldr	r3, [sp, #0x14]
    dc8c: e3a02003     	mov	r2, #3
    dc90: e1a01000     	mov	r1, r0
    dc94: e1a0000b     	mov	r0, r11
    dc98: ebffd7f5     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa02c  // CALL outlet_list
    dc9c: e51f0524     	ldr	r0, [pc, #-0x524]       @ 0xd780 <arbhar_gpio_tilde_bang+0x968>  // u32=0x6e6c; f32?=3.9611905e-41
    dca0: e1a0300a     	mov	r3, r10
    dca4: e1a01006     	mov	r1, r6
    dca8: e58d6004     	str	r6, [sp, #0x4]
    dcac: e3a020d0     	mov	r2, #208
    dcb0: e08f0000     	add	r0, pc, r0
    dcb4: ed8d9a00     	vstr	s18, [sp]
    dcb8: ebffd7ae     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa148  // CALL post
    dcbc: e1a00008     	mov	r0, r8
    dcc0: e5997db0     	ldr	r7, [r9, #0xdb0]
    dcc4: e3a02000     	mov	r2, #0
    dcc8: e58d5030     	str	r5, [sp, #0x30]
    dccc: e344237f     	movt	r2, #0x437f
    dcd0: e58d5038     	str	r5, [sp, #0x38]
    dcd4: e3a06064     	mov	r6, #100
    dcd8: e58d2034     	str	r2, [sp, #0x34]
    dcdc: e58d203c     	str	r2, [sp, #0x3c]
    dce0: ebffd690     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa5c0  // CALL gensym
    dce4: e3a02002     	mov	r2, #2
    dce8: e28d3030     	add	r3, sp, #48
    dcec: e1a01000     	mov	r1, r0
    dcf0: e1a00007     	mov	r0, r7
    dcf4: ebffd7de     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa088  // CALL outlet_list
    dcf8: e1a00008     	mov	r0, r8
    dcfc: e5999db0     	ldr	r9, [r9, #0xdb0]
    dd00: e3a03000     	mov	r3, #0
    dd04: e58d5020     	str	r5, [sp, #0x20]
    dd08: e344337e     	movt	r3, #0x437e
    dd0c: e58d3024     	str	r3, [sp, #0x24]
    dd10: ebffd684     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa5f0  // CALL gensym
    dd14: e28d3020     	add	r3, sp, #32
    dd18: e1a02005     	mov	r2, r5
    dd1c: e1a01000     	mov	r1, r0
    dd20: e1a00009     	mov	r0, r9
    dd24: ebffd7d2     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa0b8  // CALL outlet_list
    dd28: e51f15ac     	ldr	r1, [pc, #-0x5ac]       @ 0xd784 <arbhar_gpio_tilde_bang+0x96c>  // u32=0x6e1c; f32?=3.94998011e-41
    dd2c: e5c46030     	strb	r6, [r4, #0x30]
    dd30: e3a0c002     	mov	r12, #2
    dd34: e08f0001     	add	r0, pc, r1
    dd38: e58dc028     	str	r12, [sp, #0x28]
    dd3c: ebffd679     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa61c  // CALL gensym
    dd40: e5d4a030     	ldrb	r10, [r4, #0x30]
    dd44: e35a0062     	cmp	r10, #98
    dd48: e58d002c     	str	r0, [sp, #0x2c]
    dd4c: 8a000003     	bhi	0xdd60 <arbhar_gpio_tilde_bang+0xf48> @ imm = #0xc
    dd50: e51f85d0     	ldr	r8, [pc, #-0x5d0]       @ 0xd788 <arbhar_gpio_tilde_bang+0x970>  // u32=0x1965c; f32?=1.45774277e-40
    dd54: e08f5008     	add	r5, pc, r8
    dd58: e595e000     	ldr	lr, [r5]
    dd5c: eaffff6b     	b	0xdb10 <arbhar_gpio_tilde_bang+0xcf8> @ imm = #-0x254
    dd60: e1a00008     	mov	r0, r8
    dd64: e594406c     	ldr	r4, [r4, #0x6c]
    dd68: ebffd66e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xa648  // CALL gensym
    dd6c: e1a02005     	mov	r2, r5
    dd70: e28d3028     	add	r3, sp, #40
    dd74: e1a01000     	mov	r1, r0
    dd78: e1a00004     	mov	r0, r4
    dd7c: ebffd7bc     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa110  // CALL outlet_list
    dd80: eafffff2     	b	0xdd50 <arbhar_gpio_tilde_bang+0xf38> @ imm = #-0x38
    dd84: e3a0c006     	mov	r12, #6
    dd88: e3a0300a     	mov	r3, #10
    dd8c: e062ea9c     	mls	r2, r12, r10, lr
    dd90: e3a01079     	mov	r1, #121
    dd94: e1a00004     	mov	r0, r4
    dd98: e6efe072     	uxtb	lr, r2
    dd9c: e28e6001     	add	r6, lr, #1
    dda0: e0286993     	mla	r8, r3, r9, r6
    dda4: e1a02008     	mov	r2, r8
    dda8: ebffd74e     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xa2c8  // CALL writeToSharedMem
    ddac: e1a00004     	mov	r0, r4
    ddb0: ed9f0a0c     	vldr	s0, [pc, #48]           @ 0xdde8 <arbhar_gpio_tilde_bang+0xfd0>  // f32=121
    ddb4: e5d4b035     	ldrb	r11, [r4, #0x35]
    ddb8: ebffd68a     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xa5d8  // CALL readFromSharedMem
    ddbc: e59f0028     	ldr	r0, [pc, #0x28]         @ 0xddec <arbhar_gpio_tilde_bang+0xfd4>  // u32=0x7b60; f32?=4.42586107e-41
    ddc0: e1a02006     	mov	r2, r6
    ddc4: e58d8004     	str	r8, [sp, #0x4]
    ddc8: e1a01009     	mov	r1, r9
    ddcc: e1a0300b     	mov	r3, r11
    ddd0: e08f0000     	add	r0, pc, r0
    ddd4: eefd4ac0     	vcvt.s32.f32	s9, s0
    ddd8: edcd4a00     	vstr	s9, [sp]
    dddc: ebffd765     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa26c  // CALL post
    dde0: e5c47035     	strb	r7, [r4, #0x35]
    dde4: eafffdf6     	b	0xd5c4 <arbhar_gpio_tilde_bang+0x7ac> @ imm = #-0x828
    dde8: 00 00 f2 42  	.word	0x42f20000
    ddec: 60 7b 00 00  	.word	0x00007b60

