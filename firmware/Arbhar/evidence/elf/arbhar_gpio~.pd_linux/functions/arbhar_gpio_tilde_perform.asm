0000b21c <arbhar_gpio_tilde_perform>:
    b21c: e92d4070     	push	{r4, r5, r6, lr}
    b220: e1a05000     	mov	r5, r0
    b224: e5904004     	ldr	r4, [r0, #0x4]
    b228: e590600c     	ldr	r6, [r0, #0xc]
    b22c: e1a00004     	mov	r0, r4
    b230: ebffe136     	bl	0x3710 <.plt+0x14>      @ imm = #-0x7b28
    b234: e5d43030     	ldrb	r3, [r4, #0x30]
    b238: e3530063     	cmp	r3, #99
    b23c: 0a00002c     	beq	0xb2f4 <arbhar_gpio_tilde_perform+0xd8> @ imm = #0xb0
    b240: e5d40054     	ldrb	r0, [r4, #0x54]
    b244: e3500000     	cmp	r0, #0
    b248: 0a00001a     	beq	0xb2b8 <arbhar_gpio_tilde_perform+0x9c> @ imm = #0x68
    b24c: ee076a90     	vmov	s15, r6
    b250: e5d41067     	ldrb	r1, [r4, #0x67]
    b254: edd46a17     	vldr	s13, [r4, #92]
    b258: e3510000     	cmp	r1, #0
    b25c: eeb80ae7     	vcvt.f32.s32	s0, s15
    b260: 03a01c53     	moveq	r1, #21248
    b264: edd47a16     	vldr	s15, [r4, #88]
    b268: 03401007     	movteq	r1, #0x7
    b26c: ee407a26     	vmla.f32	s15, s0, s13
    b270: eef00a67     	vmov.f32	s1, s15
    b274: edc47a16     	vstr	s15, [r4, #88]
    b278: 1a000029     	bne	0xb324 <arbhar_gpio_tilde_perform+0x108> @ imm = #0xa4
    b27c: e5d42066     	ldrb	r2, [r4, #0x66]
    b280: e3520000     	cmp	r2, #0
    b284: 0a00001c     	beq	0xb2fc <arbhar_gpio_tilde_perform+0xe0> @ imm = #0x70
    b288: e241c001     	sub	r12, r1, #1
    b28c: ee03ca10     	vmov	s6, r12
    b290: eef83ac3     	vcvt.f32.s32	s7, s6
    b294: eef43ae0     	vcmpe.f32	s7, s1
    b298: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b29c: 5a000029     	bpl	0xb348 <arbhar_gpio_tilde_perform+0x12c> @ imm = #0xa4
    b2a0: e594e0b4     	ldr	lr, [r4, #0xb4]
    b2a4: e5de3006     	ldrb	r3, [lr, #0x6]
    b2a8: e3530000     	cmp	r3, #0
    b2ac: 1a00002f     	bne	0xb370 <arbhar_gpio_tilde_perform+0x154> @ imm = #0xbc
    b2b0: e3a00000     	mov	r0, #0
    b2b4: e5840058     	str	r0, [r4, #0x58]
    b2b8: e2841a01     	add	r1, r4, #4096
    b2bc: e5912ddc     	ldr	r2, [r1, #0xddc]
    b2c0: e282c001     	add	r12, r2, #1
    b2c4: e581cddc     	str	r12, [r1, #0xddc]
    b2c8: e5d4e024     	ldrb	lr, [r4, #0x24]
    b2cc: e35e0000     	cmp	lr, #0
    b2d0: 0a000007     	beq	0xb2f4 <arbhar_gpio_tilde_perform+0xd8> @ imm = #0x1c
    b2d4: edd45a0b     	vldr	s11, [r4, #44]
    b2d8: ed9f6a2a     	vldr	s12, [pc, #168]         @ 0xb388 <arbhar_gpio_tilde_perform+0x16c>
    b2dc: ed9f0a2a     	vldr	s0, [pc, #168]          @ 0xb38c <arbhar_gpio_tilde_perform+0x170>
    b2e0: ee756a86     	vadd.f32	s13, s11, s12
    b2e4: eef45ac0     	vcmpe.f32	s11, s0
    b2e8: edc46a0b     	vstr	s13, [r4, #44]
    b2ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b2f0: ca000007     	bgt	0xb314 <arbhar_gpio_tilde_perform+0xf8> @ imm = #0x1c
    b2f4: e2850010     	add	r0, r5, #16
    b2f8: e8bd8070     	pop	{r4, r5, r6, pc}
    b2fc: ee021a10     	vmov	s4, r1
    b300: eef82ac2     	vcvt.f32.s32	s5, s4
    b304: eef42ae0     	vcmpe.f32	s5, s1
    b308: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b30c: 9dc42a16     	vstrls	s5, [r4, #88]
    b310: eaffffe8     	b	0xb2b8 <arbhar_gpio_tilde_perform+0x9c> @ imm = #-0x60
    b314: e1a00004     	mov	r0, r4
    b318: ebffe180     	bl	0x3920 <.plt+0x224>     @ imm = #-0x7a00
    b31c: e2850010     	add	r0, r5, #16
    b320: e8bd8070     	pop	{r4, r5, r6, pc}
    b324: e1a00004     	mov	r0, r4
    b328: ed9f0a18     	vldr	s0, [pc, #96]           @ 0xb390 <arbhar_gpio_tilde_perform+0x174>
    b32c: ebffe12d     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x7b4c
    b330: ed9f7a17     	vldr	s14, [pc, #92]          @ 0xb394 <arbhar_gpio_tilde_perform+0x178>
    b334: edd40a16     	vldr	s1, [r4, #88]
    b338: ee201a07     	vmul.f32	s2, s0, s14
    b33c: eefd1ac1     	vcvt.s32.f32	s3, s2
    b340: ee111a90     	vmov	r1, s3
    b344: eaffffcc     	b	0xb27c <arbhar_gpio_tilde_perform+0x60> @ imm = #-0xd0
    b348: eef50ac0     	vcmpe.f32	s1, #0
    b34c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b350: 5affffd8     	bpl	0xb2b8 <arbhar_gpio_tilde_perform+0x9c> @ imm = #-0xa0
    b354: ed944a17     	vldr	s8, [r4, #92]
    b358: eeb54ac0     	vcmpe.f32	s8, #0
    b35c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b360: 4e041a90     	vmovmi	s9, r1
    b364: 4eb85ae4     	vcvtmi.f32.s32	s10, s9
    b368: 4d845a16     	vstrmi	s10, [r4, #88]
    b36c: eaffffd1     	b	0xb2b8 <arbhar_gpio_tilde_perform+0x9c> @ imm = #-0xbc
    b370: eeb70a00     	vmov.f32	s0, #1.000000e+00
    b374: ebffe1d5     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x78ac
    b378: e59400d0     	ldr	r0, [r4, #0xd0]
    b37c: eeb20b00     	vmov.f64	d0, #8.000000e+00
    b380: ebffe169     	bl	0x392c <.plt+0x230>     @ imm = #-0x7a5c
    b384: eaffffc9     	b	0xb2b0 <arbhar_gpio_tilde_perform+0x94> @ imm = #-0xdc
    b388: 00 00 80 42  	.word	0x42800000
    b38c: 00 54 18 49  	.word	0x49185400
    b390: 00 00 18 42  	.word	0x42180000
    b394: 00 00 40 42  	.word	0x42400000

