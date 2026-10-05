; lubadh::Application::updateDisplay()
; VA 0x297e0 size 1848

   297e0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   297e4: e280a048     	add	r10, r0, #72
   297e8: e2806a2a     	add	r6, r0, #172032
   297ec: ed2d8b04     	vpush	{d8, d9}
   297f0: e30a9538     	movw	r9, #0xa538
   297f4: e3409002     	movt	r9, #0x2
   297f8: e24dd00c     	sub	sp, sp, #12
   297fc: e1a07000     	mov	r7, r0
   29800: e1a0400a     	mov	r4, r10
   29804: e2866f8e     	add	r6, r6, #568
   29808: e3a08000     	mov	r8, #0
   2980c: ed9f9afb     	vldr	s18, [pc, #1004]        @ 0x29c00 ; float 100
   29810: eddf8afb     	vldr	s17, [pc, #1004]        @ 0x29c04 ; float 9.99999997475e-07
   29814: e1a00006     	mov	r0, r6
   29818: e2285001     	eor	r5, r8, #1
   2981c: eb003e85     	bl	0x39238
   29820: e3500000     	cmp	r0, #0
   29824: 1a0000ea     	bne	0x29bd4
   29828: e5d43286     	ldrb	r3, [r4, #0x286]
   2982c: e3530000     	cmp	r3, #0
   29830: 1a0000e0     	bne	0x29bb8
   29834: e5d43288     	ldrb	r3, [r4, #0x288]
   29838: e3530000     	cmp	r3, #0
   2983c: 1a0000d6     	bne	0x29b9c
   29840: e5d42240     	ldrb	r2, [r4, #0x240]
   29844: e5943290     	ldr	r3, [r4, #0x290]
   29848: e3520000     	cmp	r2, #0
   2984c: 12833002     	addne	r3, r3, #2
   29850: 15843290     	strne	r3, [r4, #0x290]
   29854: e3530000     	cmp	r3, #0
   29858: da000080     	ble	0x29a60
   2985c: e5941294     	ldr	r1, [r4, #0x294]
   29860: e2433001     	sub	r3, r3, #1
   29864: e5843290     	str	r3, [r4, #0x290]
   29868: e712f113     	sdiv	r2, r3, r1
   2986c: e0633291     	mls	r3, r1, r2, r3
   29870: e3530000     	cmp	r3, #0
   29874: 0a000124     	beq	0x29d0c
   29878: e594329c     	ldr	r3, [r4, #0x29c]
   2987c: e3530000     	cmp	r3, #0
   29880: ca000053     	bgt	0x299d4
   29884: e5d43285     	ldrb	r3, [r4, #0x285]
   29888: e3530000     	cmp	r3, #0
   2988c: 1a00006a     	bne	0x29a3c
   29890: e1a00006     	mov	r0, r6
   29894: eb003e61     	bl	0x39220
   29898: e3500000     	cmp	r0, #0
   2989c: 0a0000a6     	beq	0x29b3c
   298a0: e59632e0     	ldr	r3, [r6, #0x2e0]
   298a4: e3530003     	cmp	r3, #3
   298a8: 0a00011e     	beq	0x29d28
   298ac: e3530004     	cmp	r3, #4
   298b0: 0a000126     	beq	0x29d50
   298b4: e59420e8     	ldr	r2, [r4, #0xe8]
   298b8: e5921014     	ldr	r1, [r2, #0x14]
   298bc: e1a00004     	mov	r0, r4
   298c0: ee071a90     	vmov	s15, r1
   298c4: e592301c     	ldr	r3, [r2, #0x1c]
   298c8: e5922028     	ldr	r2, [r2, #0x28]
   298cc: eeb87ae7     	vcvt.f32.s32	s14, s15
   298d0: ee072a90     	vmov	s15, r2
   298d4: e0533001     	subs	r3, r3, r1
   298d8: eef86ae7     	vcvt.f32.s32	s13, s15
   298dc: 41a01002     	movmi	r1, r2
   298e0: 53a01000     	movpl	r1, #0
   298e4: e0833001     	add	r3, r3, r1
   298e8: ee073a90     	vmov	s15, r3
   298ec: ee876a26     	vdiv.f32	s12, s14, s13
   298f0: eef87ae7     	vcvt.f32.s32	s15, s15
   298f4: ee877aa6     	vdiv.f32	s14, s15, s13
   298f8: eef07a46     	vmov.f32	s15, s12
   298fc: eefe7acd     	vcvt.s32.f32	s15, s15, #6
   29900: edcd7a01     	vstr	s15, [sp, #4]
   29904: eef07a47     	vmov.f32	s15, s14
   29908: eefe7acd     	vcvt.s32.f32	s15, s15, #6
   2990c: ee17ba90     	vmov	r11, s15
   29910: e27b3000     	rsbs	r3, r11, #0
   29914: e20bb03f     	and	r11, r11, #63
   29918: e203303f     	and	r3, r3, #63
   2991c: 5263b000     	rsbpl	r11, r3, #0
   29920: eb003b29     	bl	0x385cc
   29924: e59430e8     	ldr	r3, [r4, #0xe8]
   29928: e1a05000     	mov	r5, r0
   2992c: edd37a00     	vldr	s15, [r3]
   29930: eef57a40     	vcmp.f32	s15, #0
   29934: eef1fa10     	vmrs	APSR_nzcv, fpscr
   29938: 0a000071     	beq	0x29b04
   2993c: e5932028     	ldr	r2, [r3, #0x28]
   29940: e1a05305     	lsl	r5, r5, #6
   29944: e5d6129d     	ldrb	r1, [r6, #0x29d]
   29948: e1a00007     	mov	r0, r7
   2994c: e715f215     	sdiv	r5, r5, r2
   29950: e5dd2004     	ldrb	r2, [sp, #0x4]
   29954: e3550000     	cmp	r5, #0
   29958: b2653000     	rsblt	r3, r5, #0
   2995c: b3c3303f     	biclt	r3, r3, #63
   29960: b2833040     	addlt	r3, r3, #64
   29964: b0855003     	addlt	r5, r5, r3
   29968: eb010c09     	bl	0x6c994
   2996c: e5d6129e     	ldrb	r1, [r6, #0x29e]
   29970: e6ef207b     	uxtb	r2, r11
   29974: e1a00007     	mov	r0, r7
   29978: eb010c05     	bl	0x6c994
   2997c: e2753000     	rsbs	r3, r5, #0
   29980: e203303f     	and	r3, r3, #63
   29984: e205203f     	and	r2, r5, #63
   29988: 52632000     	rsbpl	r2, r3, #0
   2998c: e5d6129c     	ldrb	r1, [r6, #0x29c]
   29990: e1a00007     	mov	r0, r7
   29994: e6ef2072     	uxtb	r2, r2
   29998: eb010bfd     	bl	0x6c994
   2999c: e5d43287     	ldrb	r3, [r4, #0x287]
   299a0: e3530000     	cmp	r3, #0
   299a4: 1a00004d     	bne	0x29ae0
   299a8: e2866ba9     	add	r6, r6, #173056
   299ac: e2844ba9     	add	r4, r4, #173056
   299b0: e2866f4e     	add	r6, r6, #312
   299b4: e2844f4e     	add	r4, r4, #312
   299b8: e3580001     	cmp	r8, #1
   299bc: 1a000002     	bne	0x299cc
   299c0: e28dd00c     	add	sp, sp, #12
   299c4: ecbd8b04     	vpop	{d8, d9}
   299c8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   299cc: e3a08001     	mov	r8, #1
   299d0: eaffff8f     	b	0x29814
   299d4: e59432a8     	ldr	r3, [r4, #0x2a8]
   299d8: e1a00007     	mov	r0, r7
   299dc: e5d422a4     	ldrb	r2, [r4, #0x2a4]
   299e0: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   299e4: e1620382     	smulbb	r2, r2, r3
   299e8: e6ef2072     	uxtb	r2, r2
   299ec: eb010be8     	bl	0x6c994
   299f0: e594329c     	ldr	r3, [r4, #0x29c]
   299f4: e59412a0     	ldr	r1, [r4, #0x2a0]
   299f8: e712f113     	sdiv	r2, r3, r1
   299fc: e0623291     	mls	r2, r1, r2, r3
   29a00: e2433001     	sub	r3, r3, #1
   29a04: e584329c     	str	r3, [r4, #0x29c]
   29a08: e3520000     	cmp	r2, #0
   29a0c: 05d422a4     	ldrbeq	r2, [r4, #0x2a4]
   29a10: 02222001     	eoreq	r2, r2, #1
   29a14: 05c422a4     	strbeq	r2, [r4, #0x2a4]
   29a18: e3530000     	cmp	r3, #0
   29a1c: 03a03001     	moveq	r3, #1
   29a20: 05c43285     	strbeq	r3, [r4, #0x285]
   29a24: 0a000004     	beq	0x29a3c
   29a28: e5d42285     	ldrb	r2, [r4, #0x285]
   29a2c: e3520000     	cmp	r2, #0
   29a30: 0affff96     	beq	0x29890
   29a34: e3530000     	cmp	r3, #0
   29a38: caffff94     	bgt	0x29890
   29a3c: e5943278     	ldr	r3, [r4, #0x278]
   29a40: e5933004     	ldr	r3, [r3, #0x4]
   29a44: e3530003     	cmp	r3, #3
   29a48: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   29a4c: ea0000ab     	b	0x29d00
   29a50: ac 9c 02 00  	.word	0x00029cac
   29a54: e0 9d 02 00  	.word	0x00029de0
   29a58: b4 9d 02 00  	.word	0x00029db4
   29a5c: 8c 9c 02 00  	.word	0x00029c8c
   29a60: e5d432bc     	ldrb	r3, [r4, #0x2bc]
   29a64: e59420e8     	ldr	r2, [r4, #0xe8]
   29a68: e3530000     	cmp	r3, #0
   29a6c: e5d230c0     	ldrb	r3, [r2, #0xc0]
   29a70: 1d947aae     	vldrne	s14, [r4, #696]
   29a74: 0d927a02     	vldreq	s14, [r2, #8]
   29a78: e3530000     	cmp	r3, #0
   29a7c: 0a00006b     	beq	0x29c30
   29a80: e59612a8     	ldr	r1, [r6, #0x2a8]
   29a84: e3003feb     	movw	r3, #0xfeb
   29a88: e1510003     	cmp	r1, r3
   29a8c: da000067     	ble	0x29c30
   29a90: eeb57ac0     	vcmpe.f32	s14, #0
   29a94: eeb18a00     	vmov.f32	s16, #4.000000e+00
   29a98: eef97a00     	vmov.f32	s15, #-4.000000e+00
   29a9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   29aa0: deb08a67     	vmovle.f32	s16, s15
   29aa4: e5d432b0     	ldrb	r3, [r4, #0x2b0]
   29aa8: e3530000     	cmp	r3, #0
   29aac: 0a000003     	beq	0x29ac0
   29ab0: edd47aad     	vldr	s15, [r4, #692]
   29ab4: eef47a48     	vcmp.f32	s15, s16
   29ab8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   29abc: 1a0000d0     	bne	0x29e04
   29ac0: e5d612a0     	ldrb	r1, [r6, #0x2a0]
   29ac4: e3a02001     	mov	r2, #1
   29ac8: e1a00007     	mov	r0, r7
   29acc: eb010bb0     	bl	0x6c994
   29ad0: e3a03001     	mov	r3, #1
   29ad4: ed848aad     	vstr	s16, [r4, #692]
   29ad8: e5c432b0     	strb	r3, [r4, #0x2b0]
   29adc: eaffff65     	b	0x29878
   29ae0: e3a02001     	mov	r2, #1
   29ae4: e5d612a1     	ldrb	r1, [r6, #0x2a1]
   29ae8: e1a00007     	mov	r0, r7
   29aec: eb010ba8     	bl	0x6c994
   29af0: e3a02000     	mov	r2, #0
   29af4: e3a03001     	mov	r3, #1
   29af8: e5c42287     	strb	r2, [r4, #0x287]
   29afc: e5c43285     	strb	r3, [r4, #0x285]
   29b00: eaffffa8     	b	0x299a8
   29b04: e1a01000     	mov	r1, r0
   29b08: e1a00004     	mov	r0, r4
   29b0c: eb0037bf     	bl	0x37a10
   29b10: e59430e8     	ldr	r3, [r4, #0xe8]
   29b14: e3500000     	cmp	r0, #0
   29b18: 1affff87     	bne	0x2993c
   29b1c: e5932014     	ldr	r2, [r3, #0x14]
   29b20: e593105c     	ldr	r1, [r3, #0x5c]
   29b24: e0821001     	add	r1, r2, r1
   29b28: e1510005     	cmp	r1, r5
   29b2c: b1a05001     	movlt	r5, r1
   29b30: e1550002     	cmp	r5, r2
   29b34: b1a05002     	movlt	r5, r2
   29b38: eaffff7f     	b	0x2993c
   29b3c: e020a599     	mla	r0, r9, r5, r10
   29b40: e2800a2a     	add	r0, r0, #172032
   29b44: e2800e1f     	add	r0, r0, #496
   29b48: eb003db4     	bl	0x39220
   29b4c: e3500000     	cmp	r0, #0
   29b50: 1affff52     	bne	0x298a0
   29b54: e2845a2a     	add	r5, r4, #172032
   29b58: e2855048     	add	r5, r5, #72
   29b5c: e1a00005     	mov	r0, r5
   29b60: eb003d97     	bl	0x391c4
   29b64: e250b000     	subs	r11, r0, #0
   29b68: 1a00008a     	bne	0x29d98
   29b6c: e5d43289     	ldrb	r3, [r4, #0x289]
   29b70: e3530000     	cmp	r3, #0
   29b74: 0a0000ae     	beq	0x29e34
   29b78: e59622e8     	ldr	r2, [r6, #0x2e8]
   29b7c: e1a00007     	mov	r0, r7
   29b80: e5d612a2     	ldrb	r1, [r6, #0x2a2]
   29b84: e3520000     	cmp	r2, #0
   29b88: 03a02016     	moveq	r2, #22
   29b8c: e6ef2072     	uxtb	r2, r2
   29b90: eb010b7f     	bl	0x6c994
   29b94: e5c4b289     	strb	r11, [r4, #0x289]
   29b98: eaffff45     	b	0x298b4
   29b9c: e5d622e0     	ldrb	r2, [r6, #0x2e0]
   29ba0: e1a00007     	mov	r0, r7
   29ba4: e5d612a3     	ldrb	r1, [r6, #0x2a3]
   29ba8: eb010b79     	bl	0x6c994
   29bac: e3a03000     	mov	r3, #0
   29bb0: e5c43288     	strb	r3, [r4, #0x288]
   29bb4: eaffff21     	b	0x29840
   29bb8: e5d612a1     	ldrb	r1, [r6, #0x2a1]
   29bbc: e3a02002     	mov	r2, #2
   29bc0: e1a00007     	mov	r0, r7
   29bc4: eb010b72     	bl	0x6c994
   29bc8: e3a03000     	mov	r3, #0
   29bcc: e5c43286     	strb	r3, [r4, #0x286]
   29bd0: eaffff17     	b	0x29834
   29bd4: e5d612a7     	ldrb	r1, [r6, #0x2a7]
   29bd8: e3a02000     	mov	r2, #0
   29bdc: e1a00007     	mov	r0, r7
   29be0: eb010b6b     	bl	0x6c994
   29be4: e2873915     	add	r3, r7, #344064
   29be8: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29bec: e3530000     	cmp	r3, #0
   29bf0: 1a000005     	bne	0x29c0c
   29bf4: e1a00006     	mov	r0, r6
   29bf8: eb003d90     	bl	0x39240
   29bfc: eaffff09     	b	0x29828
   29c00: 00 00 c8 42  	.word	0x42c80000
   29c04: bd 37 86 35  	.word	0x358637bd
   29c08: 0a d7 a3 3c  	.word	0x3ca3d70a
   29c0c: e0237599     	mla	r3, r9, r5, r7
   29c10: e3a02000     	mov	r2, #0
   29c14: e1a00007     	mov	r0, r7
   29c18: e2833ba9     	add	r3, r3, #173056
   29c1c: e5d310df     	ldrb	r1, [r3, #0xdf]
   29c20: eb010b5b     	bl	0x6c994
   29c24: e1a00006     	mov	r0, r6
   29c28: eb003d84     	bl	0x39240
   29c2c: eafffefd     	b	0x29828
   29c30: e59210b0     	ldr	r1, [r2, #0xb0]
   29c34: ed5f6a0d     	vldr	s13, [pc, #-52]         @ 0x29c08 ; float 0.019999999553
   29c38: e59230b4     	ldr	r3, [r2, #0xb4]
   29c3c: e5911040     	ldr	r1, [r1, #0x40]
   29c40: e59220b8     	ldr	r2, [r2, #0xb8]
   29c44: e3510000     	cmp	r1, #0
   29c48: 0ef06a68     	vmoveq.f32	s13, s17
   29c4c: e1530002     	cmp	r3, r2
   29c50: 0a000007     	beq	0x29c74
   29c54: ecb38a01     	vldmia	r3!, {s16}
   29c58: ee777a48     	vsub.f32	s15, s14, s16
   29c5c: eef07ae7     	vabs.f32	s15, s15
   29c60: eef46ae7     	vcmpe.f32	s13, s15
   29c64: eef1fa10     	vmrs	APSR_nzcv, fpscr
   29c68: caffff8d     	bgt	0x29aa4
   29c6c: e1520003     	cmp	r2, r3
   29c70: 1afffff7     	bne	0x29c54
   29c74: e5d612a0     	ldrb	r1, [r6, #0x2a0]
   29c78: e3a02000     	mov	r2, #0
   29c7c: e1a00007     	mov	r0, r7
   29c80: eb010b43     	bl	0x6c994
   29c84: e3a03000     	mov	r3, #0
   29c88: eaffff92     	b	0x29ad8
   29c8c: e1a00006     	mov	r0, r6
   29c90: eb003d5b     	bl	0x39204
   29c94: e3500000     	cmp	r0, #0
   29c98: 1a000003     	bne	0x29cac
   29c9c: e5d6129f     	ldrb	r1, [r6, #0x29f]
   29ca0: e3a02001     	mov	r2, #1
   29ca4: e1a00007     	mov	r0, r7
   29ca8: eb010b39     	bl	0x6c994
   29cac: e59430e8     	ldr	r3, [r4, #0xe8]
   29cb0: e5932084     	ldr	r2, [r3, #0x84]
   29cb4: e3520000     	cmp	r2, #0
   29cb8: 0a00006c     	beq	0x29e70
   29cbc: e3520002     	cmp	r2, #2
   29cc0: 0a000054     	beq	0x29e18
   29cc4: e3520001     	cmp	r2, #1
   29cc8: 1a00000c     	bne	0x29d00
   29ccc: e5d4227c     	ldrb	r2, [r4, #0x27c]
   29cd0: e3520000     	cmp	r2, #0
   29cd4: 1a00006b     	bne	0x29e88
   29cd8: e5932098     	ldr	r2, [r3, #0x98]
   29cdc: e3520000     	cmp	r2, #0
   29ce0: 0a000086     	beq	0x29f00
   29ce4: e5933098     	ldr	r3, [r3, #0x98]
   29ce8: e3530001     	cmp	r3, #1
   29cec: 1a000003     	bne	0x29d00
   29cf0: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29cf4: e3a02006     	mov	r2, #6
   29cf8: e1a00007     	mov	r0, r7
   29cfc: eb010b24     	bl	0x6c994
   29d00: e3a03000     	mov	r3, #0
   29d04: e5c43285     	strb	r3, [r4, #0x285]
   29d08: eafffee0     	b	0x29890
   29d0c: e5d42298     	ldrb	r2, [r4, #0x298]
   29d10: e1a00007     	mov	r0, r7
   29d14: e2222001     	eor	r2, r2, #1
   29d18: e5c42298     	strb	r2, [r4, #0x298]
   29d1c: e5d612a0     	ldrb	r1, [r6, #0x2a0]
   29d20: eb010b1b     	bl	0x6c994
   29d24: eafffed3     	b	0x29878
   29d28: e1a00006     	mov	r0, r6
   29d2c: e5d652a2     	ldrb	r5, [r6, #0x2a2]
   29d30: eb003d47     	bl	0x39254
   29d34: e2802001     	add	r2, r0, #1
   29d38: e6ef2072     	uxtb	r2, r2
   29d3c: e1a01005     	mov	r1, r5
   29d40: e1a00007     	mov	r0, r7
   29d44: eb010b12     	bl	0x6c994
   29d48: e59420e8     	ldr	r2, [r4, #0xe8]
   29d4c: eafffed9     	b	0x298b8
   29d50: e025a599     	mla	r5, r9, r5, r10
   29d54: e2855a2a     	add	r5, r5, #172032
   29d58: e2855e1f     	add	r5, r5, #496
   29d5c: e1a00005     	mov	r0, r5
   29d60: eb003d39     	bl	0x3924c
   29d64: e3500000     	cmp	r0, #0
   29d68: da000004     	ble	0x29d80
   29d6c: e1a00005     	mov	r0, r5
   29d70: e5d652a2     	ldrb	r5, [r6, #0x2a2]
   29d74: eb003d34     	bl	0x3924c
   29d78: e1a02000     	mov	r2, r0
   29d7c: eaffffed     	b	0x29d38
   29d80: e3a02016     	mov	r2, #22
   29d84: e5d612a2     	ldrb	r1, [r6, #0x2a2]
   29d88: e1a00007     	mov	r0, r7
   29d8c: eb010b00     	bl	0x6c994
   29d90: e59420e8     	ldr	r2, [r4, #0xe8]
   29d94: eafffec7     	b	0x298b8
   29d98: e59632e0     	ldr	r3, [r6, #0x2e0]
   29d9c: e3530005     	cmp	r3, #5
   29da0: 1afffec3     	bne	0x298b4
   29da4: e1a00005     	mov	r0, r5
   29da8: e5d652a2     	ldrb	r5, [r6, #0x2a2]
   29dac: eb003d0f     	bl	0x391f0
   29db0: eafffff0     	b	0x29d78
   29db4: e1a00006     	mov	r0, r6
   29db8: eb003d11     	bl	0x39204
   29dbc: e3500000     	cmp	r0, #0
   29dc0: 0a000049     	beq	0x29eec
   29dc4: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29dc8: e3a02002     	mov	r2, #2
   29dcc: e1a00007     	mov	r0, r7
   29dd0: eb010aef     	bl	0x6c994
   29dd4: e3a03000     	mov	r3, #0
   29dd8: e5c43285     	strb	r3, [r4, #0x285]
   29ddc: eafffeab     	b	0x29890
   29de0: e1a00006     	mov	r0, r6
   29de4: eb003d06     	bl	0x39204
   29de8: e3500000     	cmp	r0, #0
   29dec: 1afffff4     	bne	0x29dc4
   29df0: e5d6129f     	ldrb	r1, [r6, #0x29f]
   29df4: e3a02003     	mov	r2, #3
   29df8: e1a00007     	mov	r0, r7
   29dfc: eb010ae4     	bl	0x6c994
   29e00: eaffffef     	b	0x29dc4
   29e04: eeb00a49     	vmov.f32	s0, s18
   29e08: e1a00004     	mov	r0, r4
   29e0c: eef20a04     	vmov.f32	s1, #1.000000e+01
   29e10: eb0036d9     	bl	0x3797c
   29e14: eaffff2d     	b	0x29ad0
   29e18: e5932098     	ldr	r2, [r3, #0x98]
   29e1c: e3520000     	cmp	r2, #0
   29e20: 0a000024     	beq	0x29eb8
   29e24: e3520001     	cmp	r2, #1
   29e28: 0a000029     	beq	0x29ed4
   29e2c: e5932084     	ldr	r2, [r3, #0x84]
   29e30: eaffffa3     	b	0x29cc4
   29e34: e59420e8     	ldr	r2, [r4, #0xe8]
   29e38: e5d2307c     	ldrb	r3, [r2, #0x7c]
   29e3c: e3530000     	cmp	r3, #0
   29e40: 0afffe9c     	beq	0x298b8
   29e44: e5d2306c     	ldrb	r3, [r2, #0x6c]
   29e48: e1a00007     	mov	r0, r7
   29e4c: e5d612a2     	ldrb	r1, [r6, #0x2a2]
   29e50: e3530000     	cmp	r3, #0
   29e54: 03a02016     	moveq	r2, #22
   29e58: 15d22080     	ldrbne	r2, [r2, #0x80]
   29e5c: eb010acc     	bl	0x6c994
   29e60: e59420e8     	ldr	r2, [r4, #0xe8]
   29e64: e3a03000     	mov	r3, #0
   29e68: e5c2307c     	strb	r3, [r2, #0x7c]
   29e6c: eafffe91     	b	0x298b8
   29e70: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29e74: e1a00007     	mov	r0, r7
   29e78: eb010ac5     	bl	0x6c994
   29e7c: e59430e8     	ldr	r3, [r4, #0xe8]
   29e80: e5932084     	ldr	r2, [r3, #0x84]
   29e84: eaffff8c     	b	0x29cbc
   29e88: e3a02005     	mov	r2, #5
   29e8c: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29e90: e1a00007     	mov	r0, r7
   29e94: eb010abe     	bl	0x6c994
   29e98: e59430e8     	ldr	r3, [r4, #0xe8]
   29e9c: e5932084     	ldr	r2, [r3, #0x84]
   29ea0: e3520001     	cmp	r2, #1
   29ea4: 1affff95     	bne	0x29d00
   29ea8: e5d4227c     	ldrb	r2, [r4, #0x27c]
   29eac: e3520000     	cmp	r2, #0
   29eb0: 1affff92     	bne	0x29d00
   29eb4: eaffff87     	b	0x29cd8
   29eb8: e3a02001     	mov	r2, #1
   29ebc: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29ec0: e1a00007     	mov	r0, r7
   29ec4: eb010ab2     	bl	0x6c994
   29ec8: e59430e8     	ldr	r3, [r4, #0xe8]
   29ecc: e5932098     	ldr	r2, [r3, #0x98]
   29ed0: eaffffd3     	b	0x29e24
   29ed4: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29ed8: e3a02003     	mov	r2, #3
   29edc: e1a00007     	mov	r0, r7
   29ee0: eb010aab     	bl	0x6c994
   29ee4: e59430e8     	ldr	r3, [r4, #0xe8]
   29ee8: eaffffcf     	b	0x29e2c
   29eec: e5d6129f     	ldrb	r1, [r6, #0x29f]
   29ef0: e3a02002     	mov	r2, #2
   29ef4: e1a00007     	mov	r0, r7
   29ef8: eb010aa5     	bl	0x6c994
   29efc: eaffffb0     	b	0x29dc4
   29f00: e5d612a4     	ldrb	r1, [r6, #0x2a4]
   29f04: e3a02004     	mov	r2, #4
   29f08: e1a00007     	mov	r0, r7
   29f0c: eb010aa0     	bl	0x6c994
   29f10: e59430e8     	ldr	r3, [r4, #0xe8]
   29f14: eaffff72     	b	0x29ce4
