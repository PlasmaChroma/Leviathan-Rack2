00000968 <fftwObject_tilde_new>:
     968: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
     96c: e3a0b000     	mov	r11, #0
     970: e59f34b8     	ldr	r3, [pc, #0x4b8]        @ 0xe30 <fftwObject_tilde_new+0x4c8>  // u32=0x146f8; f32?=1.17294287e-40
     974: e3a08000     	mov	r8, #0
     978: e30c75ad     	movw	r7, #0xc5ad
     97c: e3a09000     	mov	r9, #0
     980: e08f0003     	add	r0, pc, r3
     984: e34379a7     	movt	r7, #0x39a7
     988: e34490a0     	movt	r9, #0x40a0
     98c: e59f54a0     	ldr	r5, [pc, #0x4a0]        @ 0xe34 <fftwObject_tilde_new+0x4cc>  // u32=0x14650; f32?=1.17058869e-40
     990: e5900000     	ldr	r0, [r0]
     994: ebffff30     	bl	0x65c <.plt+0x44>       @ imm = #-0x340  // CALL pd_new
     998: e3001d0c     	movw	r1, #0xd0c
     99c: e3a025fe     	mov	r2, #1065353216
     9a0: e30430c8     	movw	r3, #0x40c8
     9a4: e3403006     	movt	r3, #0x6
     9a8: e08f5005     	add	r5, pc, r5
     9ac: e2804866     	add	r4, r0, #6684672
     9b0: e1a06000     	mov	r6, r0
     9b4: e284aece     	add	r10, r4, #3296
     9b8: e284ced2     	add	r12, r4, #3360
     9bc: e184b0b1     	strh	r11, [r4, r1]
     9c0: e284eed1     	add	lr, r4, #3344
     9c4: e584bcc4     	str	r11, [r4, #0xcc4]
     9c8: e1a0100c     	mov	r1, r12
     9cc: e584bcc8     	str	r11, [r4, #0xcc8]
     9d0: e1a0000c     	mov	r0, r12
     9d4: e584bccc     	str	r11, [r4, #0xccc]
     9d8: e584bcd0     	str	r11, [r4, #0xcd0]
     9dc: e584bcd4     	str	r11, [r4, #0xcd4]
     9e0: e584bcf4     	str	r11, [r4, #0xcf4]
     9e4: e5c4bcd8     	strb	r11, [r4, #0xcd8]
     9e8: e58a2000     	str	r2, [r10]
     9ec: e284aa01     	add	r10, r4, #4096
     9f0: e584bce4     	str	r11, [r4, #0xce4]
     9f4: e584bce8     	str	r11, [r4, #0xce8]
     9f8: e584bd00     	str	r11, [r4, #0xd00]
     9fc: e584bcfc     	str	r11, [r4, #0xcfc]
     a00: e584bcf8     	str	r11, [r4, #0xcf8]
     a04: e3a0b0c8     	mov	r11, #200
     a08: e5843d18     	str	r3, [r4, #0xd18]
     a0c: e3a03005     	mov	r3, #5
     a10: e584bd14     	str	r11, [r4, #0xd14]
     a14: e584bd10     	str	r11, [r4, #0xd10]
     a18: e584bd04     	str	r11, [r4, #0xd04]
     a1c: e3a0b019     	mov	r11, #25
     a20: e58a303c     	str	r3, [r10, #0x3c]
     a24: e2843ed3     	add	r3, r4, #3376
     a28: e58a9048     	str	r9, [r10, #0x48]
     a2c: e58a2044     	str	r2, [r10, #0x44]
     a30: e58ab040     	str	r11, [r10, #0x40]
     a34: e58a204c     	str	r2, [r10, #0x4c]
     a38: e58e800c     	str	r8, [lr, #0xc]
     a3c: e304e0a5     	movw	lr, #0x40a5
     a40: e58c7000     	str	r7, [r12]
     a44: e343ed9f     	movt	lr, #0x3d9f
     a48: e30c75ad     	movw	r7, #0xc5ad
     a4c: e3437c27     	movt	r7, #0x3c27
     a50: e58c7004     	str	r7, [r12, #0x4]
     a54: e30cc5ad     	movw	r12, #0xc5ad
     a58: e581e008     	str	lr, [r1, #0x8]
     a5c: e3001fd8     	movw	r1, #0xfd8
     a60: e3401066     	movt	r1, #0x66
     a64: e343cea7     	movt	r12, #0x3ea7
     a68: e0869001     	add	r9, r6, r1
     a6c: e580c00c     	str	r12, [r0, #0xc]
     a70: e1a00003     	mov	r0, r3
     a74: e2833028     	add	r3, r3, #40
     a78: e4802004     	str	r2, [r0], #4
     a7c: e5032024     	str	r2, [r3, #-0x24]
     a80: e5802004     	str	r2, [r0, #0x4]
     a84: e503201c     	str	r2, [r3, #-0x1c]
     a88: e5032018     	str	r2, [r3, #-0x18]
     a8c: e5032014     	str	r2, [r3, #-0x14]
     a90: e5032010     	str	r2, [r3, #-0x10]
     a94: e503200c     	str	r2, [r3, #-0xc]
     a98: e5032008     	str	r2, [r3, #-0x8]
     a9c: e5032004     	str	r2, [r3, #-0x4]
     aa0: e1530009     	cmp	r3, r9
     aa4: 1afffff1     	bne	0xa70 <fftwObject_tilde_new+0x108> @ imm = #-0x3c
     aa8: eef75b00     	vmov.f64	d21, #1.000000e+00
     aac: e3a01001     	mov	r1, #1
     ab0: eddf5add     	vldr	s11, [pc, #884]         @ 0xe2c <fftwObject_tilde_new+0x4c4>  // f32=0.0399999991
     ab4: e3a025fe     	mov	r2, #1065353216
     ab8: e4832004     	str	r2, [r3], #4
     abc: e281b001     	add	r11, r1, #1
     ac0: e2817002     	add	r7, r1, #2
     ac4: ee071a90     	vmov	s15, r1
     ac8: e281e003     	add	lr, r1, #3
     acc: e281c004     	add	r12, r1, #4
     ad0: e2819005     	add	r9, r1, #5
     ad4: e2810006     	add	r0, r1, #6
     ad8: e2812007     	add	r2, r1, #7
     adc: ee047a10     	vmov	s8, r7
     ae0: e1a08003     	mov	r8, r3
     ae4: ee04ea90     	vmov	s9, lr
     ae8: e2811008     	add	r1, r1, #8
     aec: ee05ca10     	vmov	s10, r12
     af0: e3510019     	cmp	r1, #25
     af4: eef83ae7     	vcvt.f32.s32	s7, s15
     af8: e2833020     	add	r3, r3, #32
     afc: ee069a10     	vmov	s12, r9
     b00: ee060a90     	vmov	s13, r0
     b04: ee00ba90     	vmov	s1, r11
     b08: ee072a10     	vmov	s14, r2
     b0c: eeb81ae0     	vcvt.f32.s32	s2, s1
     b10: eeb83ae6     	vcvt.f32.s32	s6, s13
     b14: eef80ac7     	vcvt.f32.s32	s1, s14
     b18: eeb80ac4     	vcvt.f32.s32	s0, s8
     b1c: eef81ae4     	vcvt.f32.s32	s3, s9
     b20: eeb82ac5     	vcvt.f32.s32	s4, s10
     b24: eef82ac6     	vcvt.f32.s32	s5, s12
     b28: ee633aa5     	vmul.f32	s7, s7, s11
     b2c: ee204a25     	vmul.f32	s8, s0, s11
     b30: ee617a25     	vmul.f32	s15, s2, s11
     b34: ee614aa5     	vmul.f32	s9, s3, s11
     b38: ee225a25     	vmul.f32	s10, s4, s11
     b3c: ee226aa5     	vmul.f32	s12, s5, s11
     b40: ee230a25     	vmul.f32	s0, s6, s11
     b44: ee606aa5     	vmul.f32	s13, s1, s11
     b48: eef78ae3     	vcvt.f64.f32	d24, s7
     b4c: eef77ac4     	vcvt.f64.f32	d23, s8
     b50: eef70ae7     	vcvt.f64.f32	d16, s15
     b54: eef76ae4     	vcvt.f64.f32	d22, s9
     b58: eef74ac5     	vcvt.f64.f32	d20, s10
     b5c: eef73ac6     	vcvt.f64.f32	d19, s12
     b60: eef72ac0     	vcvt.f64.f32	d18, s0
     b64: eef71ae6     	vcvt.f64.f32	d17, s13
     b68: ee759be8     	vsub.f64	d25, d21, d24
     b6c: ee75abe7     	vsub.f64	d26, d21, d23
     b70: ee75bbe0     	vsub.f64	d27, d21, d16
     b74: ee75cbe6     	vsub.f64	d28, d21, d22
     b78: ee75dbe4     	vsub.f64	d29, d21, d20
     b7c: ee75ebe3     	vsub.f64	d30, d21, d19
     b80: ee75fbe2     	vsub.f64	d31, d21, d18
     b84: ee351be1     	vsub.f64	d1, d21, d17
     b88: eeb72be9     	vcvt.f32.f64	s4, d25
     b8c: eeb77bea     	vcvt.f32.f64	s14, d26
     b90: eca82a01     	vstmia	r8!, {s4}
     b94: eef72beb     	vcvt.f32.f64	s5, d27
     b98: eeb73bec     	vcvt.f32.f64	s6, d28
     b9c: ed432a07     	vstr	s5, [r3, #-28]
     ba0: ed887a01     	vstr	s14, [r8, #4]
     ba4: eef70bed     	vcvt.f32.f64	s1, d29
     ba8: ed033a05     	vstr	s6, [r3, #-20]
     bac: eef73bee     	vcvt.f32.f64	s7, d30
     bb0: ed430a04     	vstr	s1, [r3, #-16]
     bb4: eeb74bef     	vcvt.f32.f64	s8, d31
     bb8: ed433a03     	vstr	s7, [r3, #-12]
     bbc: eef71bc1     	vcvt.f32.f64	s3, d1
     bc0: ed034a02     	vstr	s8, [r3, #-8]
     bc4: ed431a01     	vstr	s3, [r3, #-4]
     bc8: 1affffbb     	bne	0xabc <fftwObject_tilde_new+0x154> @ imm = #-0x114
     bcc: e59fb264     	ldr	r11, [pc, #0x264]       @ 0xe38 <fftwObject_tilde_new+0x4d0>  // u32=0x70; f32?=1.56945428e-43
     bd0: e1a01006     	mov	r1, r6
     bd4: e1a00006     	mov	r0, r6
     bd8: e2868902     	add	r8, r6, #32768
     bdc: e2888020     	add	r8, r8, #32
     be0: e2869020     	add	r9, r6, #32
     be4: e795b00b     	ldr	r11, [r5, r11]
     be8: e2865903     	add	r5, r6, #49152
     bec: e2855028     	add	r5, r5, #40
     bf0: e2867865     	add	r7, r6, #6619136
     bf4: e2877ecb     	add	r7, r7, #3248
     bf8: e1a0300b     	mov	r3, r11
     bfc: e1a0200b     	mov	r2, r11
     c00: ebfffe9b     	bl	0x674 <.plt+0x5c>       @ imm = #-0x594  // CALL inlet_new
     c04: e1a0100b     	mov	r1, r11
     c08: e58a0050     	str	r0, [r10, #0x50]
     c0c: e1a00006     	mov	r0, r6
     c10: ebfffeaf     	bl	0x6d4 <.plt+0xbc>       @ imm = #-0x544  // CALL outlet_new
     c14: e1a02008     	mov	r2, r8
     c18: e1a01009     	mov	r1, r9
     c1c: e3a03000     	mov	r3, #0
     c20: e58a0054     	str	r0, [r10, #0x54]
     c24: e3a00a01     	mov	r0, #4096
     c28: ebfffe97     	bl	0x68c <.plt+0x74>       @ imm = #-0x5a4  // CALL fftwf_plan_dft_r2c_1d
     c2c: e286c901     	add	r12, r6, #16384
     c30: e28c1020     	add	r1, r12, #32
     c34: e1a02005     	mov	r2, r5
     c38: e3a03000     	mov	r3, #0
     c3c: e5840cb8     	str	r0, [r4, #0xcb8]
     c40: e3a00a01     	mov	r0, #4096
     c44: ebfffe90     	bl	0x68c <.plt+0x74>       @ imm = #-0x5c0  // CALL fftwf_plan_dft_r2c_1d
     c48: e3042cb8     	movw	r2, #0x4cb8
     c4c: e3402065     	movt	r2, #0x65
     c50: e1a01007     	mov	r1, r7
     c54: e0862002     	add	r2, r6, r2
     c58: e3a03000     	mov	r3, #0
     c5c: e5840cbc     	str	r0, [r4, #0xcbc]
     c60: e3a00a01     	mov	r0, #4096
     c64: ebfffe91     	bl	0x6b0 <.plt+0x98>       @ imm = #-0x5bc  // CALL fftwf_plan_dft_c2r_1d
     c68: e2882008     	add	r2, r8, #8
     c6c: e287c008     	add	r12, r7, #8
     c70: e2851008     	add	r1, r5, #8
     c74: e3a03000     	mov	r3, #0
     c78: e5840cc0     	str	r0, [r4, #0xcc0]
     c7c: e5883000     	str	r3, [r8]
     c80: e5883004     	str	r3, [r8, #0x4]
     c84: e5853000     	str	r3, [r5]
     c88: e5853004     	str	r3, [r5, #0x4]
     c8c: e5873000     	str	r3, [r7]
     c90: e5873004     	str	r3, [r7, #0x4]
     c94: e5823000     	str	r3, [r2]
     c98: e2824008     	add	r4, r2, #8
     c9c: e5823004     	str	r3, [r2, #0x4]
     ca0: e2822040     	add	r2, r2, #64
     ca4: e5813000     	str	r3, [r1]
     ca8: e28cc040     	add	r12, r12, #64
     cac: e5813004     	str	r3, [r1, #0x4]
     cb0: e2811040     	add	r1, r1, #64
     cb4: e50c3040     	str	r3, [r12, #-0x40]
     cb8: e50c303c     	str	r3, [r12, #-0x3c]
     cbc: e5023038     	str	r3, [r2, #-0x38]
     cc0: e5023034     	str	r3, [r2, #-0x34]
     cc4: e5013038     	str	r3, [r1, #-0x38]
     cc8: e5013034     	str	r3, [r1, #-0x34]
     ccc: e50c3038     	str	r3, [r12, #-0x38]
     cd0: e50c3034     	str	r3, [r12, #-0x34]
     cd4: e5023030     	str	r3, [r2, #-0x30]
     cd8: e502302c     	str	r3, [r2, #-0x2c]
     cdc: e5013030     	str	r3, [r1, #-0x30]
     ce0: e501302c     	str	r3, [r1, #-0x2c]
     ce4: e50c3030     	str	r3, [r12, #-0x30]
     ce8: e50c302c     	str	r3, [r12, #-0x2c]
     cec: e5023028     	str	r3, [r2, #-0x28]
     cf0: e5023024     	str	r3, [r2, #-0x24]
     cf4: e5013028     	str	r3, [r1, #-0x28]
     cf8: e5013024     	str	r3, [r1, #-0x24]
     cfc: e50c3028     	str	r3, [r12, #-0x28]
     d00: e50c3024     	str	r3, [r12, #-0x24]
     d04: e5023020     	str	r3, [r2, #-0x20]
     d08: e502301c     	str	r3, [r2, #-0x1c]
     d0c: e5013020     	str	r3, [r1, #-0x20]
     d10: e501301c     	str	r3, [r1, #-0x1c]
     d14: e50c3020     	str	r3, [r12, #-0x20]
     d18: e50c301c     	str	r3, [r12, #-0x1c]
     d1c: e5023018     	str	r3, [r2, #-0x18]
     d20: e5023014     	str	r3, [r2, #-0x14]
     d24: e5013018     	str	r3, [r1, #-0x18]
     d28: e5013014     	str	r3, [r1, #-0x14]
     d2c: e50c3018     	str	r3, [r12, #-0x18]
     d30: e50c3014     	str	r3, [r12, #-0x14]
     d34: e5023010     	str	r3, [r2, #-0x10]
     d38: e502300c     	str	r3, [r2, #-0xc]
     d3c: e5013010     	str	r3, [r1, #-0x10]
     d40: e501300c     	str	r3, [r1, #-0xc]
     d44: e50c3010     	str	r3, [r12, #-0x10]
     d48: e50c300c     	str	r3, [r12, #-0xc]
     d4c: e5023008     	str	r3, [r2, #-0x8]
     d50: e5023004     	str	r3, [r2, #-0x4]
     d54: e1520005     	cmp	r2, r5
     d58: e5013008     	str	r3, [r1, #-0x8]
     d5c: e5013004     	str	r3, [r1, #-0x4]
     d60: e50c3008     	str	r3, [r12, #-0x8]
     d64: e50c3004     	str	r3, [r12, #-0x4]
     d68: 1affffc9     	bne	0xc94 <fftwObject_tilde_new+0x32c> @ imm = #-0xdc
     d6c: e3a02902     	mov	r2, #32768
     d70: e3a01000     	mov	r1, #0
     d74: e1a00009     	mov	r0, r9
     d78: ebfffe4f     	bl	0x6bc <.plt+0xa4>       @ imm = #-0x6c4  // CALL memset
     d7c: e2860833     	add	r0, r6, #3342336
     d80: e2801e67     	add	r1, r0, #1648
     d84: e2862801     	add	r2, r6, #65536
     d88: e2822030     	add	r2, r2, #48
     d8c: e3a03000     	mov	r3, #0
     d90: e1a0e001     	mov	lr, r1
     d94: e5823000     	str	r3, [r2]
     d98: e2822040     	add	r2, r2, #64
     d9c: e502303c     	str	r3, [r2, #-0x3c]
     da0: e2811040     	add	r1, r1, #64
     da4: e5013040     	str	r3, [r1, #-0x40]
     da8: e501303c     	str	r3, [r1, #-0x3c]
     dac: e5023038     	str	r3, [r2, #-0x38]
     db0: e5023034     	str	r3, [r2, #-0x34]
     db4: e5013038     	str	r3, [r1, #-0x38]
     db8: e5013034     	str	r3, [r1, #-0x34]
     dbc: e5023030     	str	r3, [r2, #-0x30]
     dc0: e502302c     	str	r3, [r2, #-0x2c]
     dc4: e5013030     	str	r3, [r1, #-0x30]
     dc8: e501302c     	str	r3, [r1, #-0x2c]
     dcc: e5023028     	str	r3, [r2, #-0x28]
     dd0: e5023024     	str	r3, [r2, #-0x24]
     dd4: e5013028     	str	r3, [r1, #-0x28]
     dd8: e5013024     	str	r3, [r1, #-0x24]
     ddc: e5023020     	str	r3, [r2, #-0x20]
     de0: e502301c     	str	r3, [r2, #-0x1c]
     de4: e5013020     	str	r3, [r1, #-0x20]
     de8: e501301c     	str	r3, [r1, #-0x1c]
     dec: e5023018     	str	r3, [r2, #-0x18]
     df0: e5023014     	str	r3, [r2, #-0x14]
     df4: e5013018     	str	r3, [r1, #-0x18]
     df8: e5013014     	str	r3, [r1, #-0x14]
     dfc: e5023010     	str	r3, [r2, #-0x10]
     e00: e502300c     	str	r3, [r2, #-0xc]
     e04: e5013010     	str	r3, [r1, #-0x10]
     e08: e501300c     	str	r3, [r1, #-0xc]
     e0c: e5023008     	str	r3, [r2, #-0x8]
     e10: e5023004     	str	r3, [r2, #-0x4]
     e14: e152000e     	cmp	r2, lr
     e18: e5013008     	str	r3, [r1, #-0x8]
     e1c: e5013004     	str	r3, [r1, #-0x4]
     e20: 1affffdb     	bne	0xd94 <fftwObject_tilde_new+0x42c> @ imm = #-0x94
     e24: e1a00006     	mov	r0, r6
     e28: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
     e2c: 0a d7 23 3d  	.word	0x3d23d70a
     e30: f8 46 01 00  	.word	0x000146f8
     e34: 50 46 01 00  	.word	0x00014650
     e38: 70 00 00 00  	.word	0x00000070

