0000287c <makingWndArray>:
    287c: eef76a00     	vmov.f32	s13, #1.000000e+00
    2880: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x2ca8 <makingWndArray+0x42c>
    2884: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x2cac <makingWndArray+0x430>
    2888: e08f3003     	add	r3, pc, r3
    288c: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x2cb0 <makingWndArray+0x434>
    2890: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2894: e08f4000     	add	r4, pc, r0
    2898: e793c002     	ldr	r12, [r3, r2]
    289c: e2845b02     	add	r5, r4, #2048
    28a0: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x2c88 <makingWndArray+0x40c>
    28a4: e285000c     	add	r0, r5, #12
    28a8: e28c6bcb     	add	r6, r12, #207872
    28ac: e3e0e031     	mvn	lr, #49
    28b0: e28650bc     	add	r5, r6, #188
    28b4: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x2ca0 <makingWndArray+0x424>
    28b8: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x2c90 <makingWndArray+0x414>
    28bc: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x2c98 <makingWndArray+0x41c>
    28c0: ee07ea90     	vmov	s15, lr
    28c4: eeb85be7     	vcvt.f64.s32	d5, s15
    28c8: ee255b23     	vmul.f64	d5, d5, d19
    28cc: eeb70bc5     	vcvt.f32.f64	s0, d5
    28d0: eeb14a40     	vneg.f32	s8, s0
    28d4: ee705a26     	vadd.f32	s11, s0, s13
    28d8: ee764ac0     	vsub.f32	s9, s13, s0
    28dc: eef74ac4     	vcvt.f64.f32	d20, s8
    28e0: eeb50ac0     	vcmpe.f32	s0, #0
    28e4: ee241ba2     	vmul.f64	d1, d20, d18
    28e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    28ec: ee640aa5     	vmul.f32	s1, s9, s11
    28f0: ee602a06     	vmul.f32	s5, s0, s12
    28f4: 9a00005b     	bls	0x2a68 <makingWndArray+0x1ec> @ imm = #0x16c
    28f8: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x2cb4 <makingWndArray+0x438>
    28fc: e1a0700c     	mov	r7, r12
    2900: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x2ca4 <makingWndArray+0x428>
    2904: e3004202     	movw	r4, #0x202
    2908: e08f2001     	add	r2, pc, r1
    290c: e3a08001     	mov	r8, #1
    2910: e3001203     	movw	r1, #0x203
    2914: e4923004     	ldr	r3, [r2], #4
    2918: ece71a01     	vstmia	r7!, {s3}
    291c: ea000039     	b	0x2a08 <makingWndArray+0x18c> @ imm = #0xe4
    2920: e3580f7e     	cmp	r8, #504
    2924: def03a66     	vmovle.f32	s7, s13
    2928: ca000048     	bgt	0x2a50 <makingWndArray+0x1d4> @ imm = #0x120
    292c: ee018a10     	vmov	s2, r8
    2930: e2883001     	add	r3, r8, #1
    2934: e1a06002     	mov	r6, r2
    2938: ecf64a01     	vldmia	r6!, {s9}
    293c: eeb82ac1     	vcvt.f32.s32	s4, s2
    2940: ee327a22     	vadd.f32	s14, s4, s5
    2944: eebd3ac7     	vcvt.s32.f32	s6, s14
    2948: ee138a10     	vmov	r8, s6
    294c: ee607a24     	vmul.f32	s15, s0, s9
    2950: e1580001     	cmp	r8, r1
    2954: a1a08001     	movge	r8, r1
    2958: e0802108     	add	r2, r0, r8, lsl #2
    295c: ed925a00     	vldr	s10, [r2]
    2960: ee407a85     	vmla.f32	s15, s1, s10
    2964: eef47ae6     	vcmpe.f32	s15, s13
    2968: eef1fa10     	vmrs	APSR_nzcv, fpscr
    296c: 8ef07a66     	vmovhi.f32	s15, s13
    2970: eef57ac0     	vcmpe.f32	s15, #0
    2974: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2978: bef07a61     	vmovlt.f32	s15, s3
    297c: e3530009     	cmp	r3, #9
    2980: ee274aa3     	vmul.f32	s8, s15, s7
    2984: eca74a01     	vstmia	r7!, {s8}
    2988: da00002b     	ble	0x2a3c <makingWndArray+0x1c0> @ imm = #0xac
    298c: e3530f7e     	cmp	r3, #504
    2990: c0442003     	subgt	r2, r4, r3
    2994: def03a66     	vmovle.f32	s7, s13
    2998: ce042a10     	vmovgt	s8, r2
    299c: cef80bc4     	vcvtgt.f64.s32	d16, s8
    29a0: ce600ba1     	vmulgt.f64	d16, d16, d17
    29a4: cef73be0     	vcvtgt.f32.f64	s7, d16
    29a8: ee013a10     	vmov	s2, r3
    29ac: e2838001     	add	r8, r3, #1
    29b0: e1a02006     	mov	r2, r6
    29b4: ecf25a01     	vldmia	r2!, {s11}
    29b8: eeb82ac1     	vcvt.f32.s32	s4, s2
    29bc: ee724a22     	vadd.f32	s9, s4, s5
    29c0: eebd7ae4     	vcvt.s32.f32	s14, s9
    29c4: ee173a10     	vmov	r3, s14
    29c8: ee203a25     	vmul.f32	s6, s0, s11
    29cc: e1530001     	cmp	r3, r1
    29d0: a1a03001     	movge	r3, r1
    29d4: e0803103     	add	r3, r0, r3, lsl #2
    29d8: edd37a00     	vldr	s15, [r3]
    29dc: ee003aa7     	vmla.f32	s6, s1, s15
    29e0: eeb43ae6     	vcmpe.f32	s6, s13
    29e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    29e8: 8eb03a66     	vmovhi.f32	s6, s13
    29ec: eeb53ac0     	vcmpe.f32	s6, #0
    29f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    29f4: beb03a61     	vmovlt.f32	s6, s3
    29f8: e1580001     	cmp	r8, r1
    29fc: ee235a23     	vmul.f32	s10, s6, s7
    2a00: eca75a01     	vstmia	r7!, {s10}
    2a04: 0a000006     	beq	0x2a24 <makingWndArray+0x1a8> @ imm = #0x18
    2a08: e3580009     	cmp	r8, #9
    2a0c: caffffc3     	bgt	0x2920 <makingWndArray+0xa4> @ imm = #-0xf4
    2a10: ee058a90     	vmov	s11, r8
    2a14: eef8abe5     	vcvt.f64.s32	d26, s11
    2a18: ee6a0ba1     	vmul.f64	d16, d26, d17
    2a1c: eef73be0     	vcvt.f32.f64	s7, d16
    2a20: eaffffc1     	b	0x292c <makingWndArray+0xb0> @ imm = #-0xfc
    2a24: e28ccb02     	add	r12, r12, #2048
    2a28: e28ee001     	add	lr, lr, #1
    2a2c: e28cc00c     	add	r12, r12, #12
    2a30: e155000c     	cmp	r5, r12
    2a34: 1affffa1     	bne	0x28c0 <makingWndArray+0x44> @ imm = #-0x17c
    2a38: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    2a3c: ee033a90     	vmov	s7, r3
    2a40: eef8bbe3     	vcvt.f64.s32	d27, s7
    2a44: ee6b0ba1     	vmul.f64	d16, d27, d17
    2a48: eef73be0     	vcvt.f32.f64	s7, d16
    2a4c: eaffffd5     	b	0x29a8 <makingWndArray+0x12c> @ imm = #-0xac
    2a50: e0446008     	sub	r6, r4, r8
    2a54: ee036a90     	vmov	s7, r6
    2a58: eef89be3     	vcvt.f64.s32	d25, s7
    2a5c: ee690ba1     	vmul.f64	d16, d25, d17
    2a60: eef73be0     	vcvt.f32.f64	s7, d16
    2a64: eaffffb0     	b	0x292c <makingWndArray+0xb0> @ imm = #-0x140
    2a68: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x2cb8 <makingWndArray+0x43c>
    2a6c: e1a0900c     	mov	r9, r12
    2a70: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x2cbc <makingWndArray+0x440>
    2a74: e08f8007     	add	r8, pc, r7
    2a78: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x2ca4 <makingWndArray+0x428>
    2a7c: e08f4003     	add	r4, pc, r3
    2a80: e2882b02     	add	r2, r8, #2048
    2a84: e3a03000     	mov	r3, #0
    2a88: e282200c     	add	r2, r2, #12
    2a8c: e2446efe     	sub	r6, r4, #4064
    2a90: e3008202     	movw	r8, #0x202
    2a94: e3007203     	movw	r7, #0x203
    2a98: ea000072     	b	0x2c68 <makingWndArray+0x3ec> @ imm = #0x1c8
    2a9c: e3530f7e     	cmp	r3, #504
    2aa0: c0481003     	subgt	r1, r8, r3
    2aa4: def02a66     	vmovle.f32	s5, s13
    2aa8: ce041a10     	vmovgt	s8, r1
    2aac: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2ab0: ce600ba1     	vmulgt.f64	d16, d16, d17
    2ab4: cef72be0     	vcvtgt.f32.f64	s5, d16
    2ab8: ecb27a01     	vldmia	r2!, {s14}
    2abc: e1a04006     	mov	r4, r6
    2ac0: e2833001     	add	r3, r3, #1
    2ac4: e1a01009     	mov	r1, r9
    2ac8: ecb43a01     	vldmia	r4!, {s6}
    2acc: ee657a87     	vmul.f32	s15, s11, s14
    2ad0: eef75ac3     	vcvt.f64.f32	d21, s6
    2ad4: eef76ae7     	vcvt.f64.f32	d22, s15
    2ad8: ee456b81     	vmla.f64	d22, d21, d1
    2adc: eeb75be6     	vcvt.f32.f64	s10, d22
    2ae0: eeb55ac0     	vcmpe.f32	s10, #0
    2ae4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2ae8: beb05a42     	vmovlt.f32	s10, s4
    2aec: e3530009     	cmp	r3, #9
    2af0: ee250a22     	vmul.f32	s0, s10, s5
    2af4: eca10a01     	vstmia	r1!, {s0}
    2af8: da00007f     	ble	0x2cfc <makingWndArray+0x480> @ imm = #0x1fc
    2afc: e3530f7e     	cmp	r3, #504
    2b00: c0486003     	subgt	r6, r8, r3
    2b04: def02a66     	vmovle.f32	s5, s13
    2b08: ce006a10     	vmovgt	s0, r6
    2b0c: cef86bc0     	vcvtgt.f64.s32	d22, s0
    2b10: ce666ba1     	vmulgt.f64	d22, d22, d17
    2b14: cef72be6     	vcvtgt.f32.f64	s5, d22
    2b18: edd20a00     	vldr	s1, [r2]
    2b1c: e2839001     	add	r9, r3, #1
    2b20: edd44a00     	vldr	s9, [r4]
    2b24: ee254aa0     	vmul.f32	s8, s11, s1
    2b28: eef79ae4     	vcvt.f64.f32	d25, s9
    2b2c: eef7aac4     	vcvt.f64.f32	d26, s8
    2b30: ee49ab81     	vmla.f64	d26, d25, d1
    2b34: eeb77bea     	vcvt.f32.f64	s14, d26
    2b38: eeb57ac0     	vcmpe.f32	s14, #0
    2b3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2b40: beb07a42     	vmovlt.f32	s14, s4
    2b44: e3590009     	cmp	r9, #9
    2b48: ee273a22     	vmul.f32	s6, s14, s5
    2b4c: ed813a00     	vstr	s6, [r1]
    2b50: da000064     	ble	0x2ce8 <makingWndArray+0x46c> @ imm = #0x190
    2b54: e3590f7e     	cmp	r9, #504
    2b58: c0489009     	subgt	r9, r8, r9
    2b5c: def04a66     	vmovle.f32	s9, s13
    2b60: ce039a10     	vmovgt	s6, r9
    2b64: cef8abc3     	vcvtgt.f64.s32	d26, s6
    2b68: ce6aaba1     	vmulgt.f64	d26, d26, d17
    2b6c: cef74bea     	vcvtgt.f32.f64	s9, d26
    2b70: ed925a01     	vldr	s10, [r2, #4]
    2b74: e2836002     	add	r6, r3, #2
    2b78: ed940a01     	vldr	s0, [r4, #4]
    2b7c: ee650a85     	vmul.f32	s1, s11, s10
    2b80: eef7dac0     	vcvt.f64.f32	d29, s0
    2b84: eef7eae0     	vcvt.f64.f32	d30, s1
    2b88: ee4deb81     	vmla.f64	d30, d29, d1
    2b8c: eeb74bee     	vcvt.f32.f64	s8, d30
    2b90: eeb54ac0     	vcmpe.f32	s8, #0
    2b94: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2b98: beb04a42     	vmovlt.f32	s8, s4
    2b9c: e3560009     	cmp	r6, #9
    2ba0: ee247a24     	vmul.f32	s14, s8, s9
    2ba4: ed817a01     	vstr	s14, [r1, #4]
    2ba8: da000049     	ble	0x2cd4 <makingWndArray+0x458> @ imm = #0x124
    2bac: e3560f7e     	cmp	r6, #504
    2bb0: c0486006     	subgt	r6, r8, r6
    2bb4: def04a66     	vmovle.f32	s9, s13
    2bb8: ce076a10     	vmovgt	s14, r6
    2bbc: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    2bc0: ce6eeba1     	vmulgt.f64	d30, d30, d17
    2bc4: cef74bee     	vcvtgt.f32.f64	s9, d30
    2bc8: ed923a02     	vldr	s6, [r2, #8]
    2bcc: e2839003     	add	r9, r3, #3
    2bd0: edd47a02     	vldr	s15, [r4, #8]
    2bd4: ee255a83     	vmul.f32	s10, s11, s6
    2bd8: eef75ae7     	vcvt.f64.f32	d21, s15
    2bdc: eef70ac5     	vcvt.f64.f32	d16, s10
    2be0: ee450b81     	vmla.f64	d16, d21, d1
    2be4: eeb70be0     	vcvt.f32.f64	s0, d16
    2be8: eeb50ac0     	vcmpe.f32	s0, #0
    2bec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2bf0: beb00a42     	vmovlt.f32	s0, s4
    2bf4: e3590009     	cmp	r9, #9
    2bf8: ee600a24     	vmul.f32	s1, s0, s9
    2bfc: edc10a02     	vstr	s1, [r1, #8]
    2c00: da00002e     	ble	0x2cc0 <makingWndArray+0x444> @ imm = #0xb8
    2c04: e3590f7e     	cmp	r9, #504
    2c08: c0489009     	subgt	r9, r8, r9
    2c0c: def04a66     	vmovle.f32	s9, s13
    2c10: ce009a90     	vmovgt	s1, r9
    2c14: cef80be0     	vcvtgt.f64.s32	d16, s1
    2c18: ce600ba1     	vmulgt.f64	d16, d16, d17
    2c1c: cef74be0     	vcvtgt.f32.f64	s9, d16
    2c20: ed927a03     	vldr	s14, [r2, #12]
    2c24: e2833004     	add	r3, r3, #4
    2c28: e2822010     	add	r2, r2, #16
    2c2c: e2846010     	add	r6, r4, #16
    2c30: e2819010     	add	r9, r1, #16
    2c34: ed943a03     	vldr	s6, [r4, #12]
    2c38: ee657a87     	vmul.f32	s15, s11, s14
    2c3c: eef78ac3     	vcvt.f64.f32	d24, s6
    2c40: eef70ae7     	vcvt.f64.f32	d16, s15
    2c44: ee480b81     	vmla.f64	d16, d24, d1
    2c48: eeb75be0     	vcvt.f32.f64	s10, d16
    2c4c: eeb55ac0     	vcmpe.f32	s10, #0
    2c50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c54: beb05a42     	vmovlt.f32	s10, s4
    2c58: e1530007     	cmp	r3, r7
    2c5c: ee254a24     	vmul.f32	s8, s10, s9
    2c60: ed814a03     	vstr	s8, [r1, #12]
    2c64: 0affff6e     	beq	0x2a24 <makingWndArray+0x1a8> @ imm = #-0x248
    2c68: e3530009     	cmp	r3, #9
    2c6c: caffff8a     	bgt	0x2a9c <makingWndArray+0x220> @ imm = #-0x1d8
    2c70: ee023a90     	vmov	s5, r3
    2c74: eef80be2     	vcvt.f64.s32	d16, s5
    2c78: ee204ba1     	vmul.f64	d4, d16, d17
    2c7c: eef72bc4     	vcvt.f32.f64	s5, d4
    2c80: eaffff8c     	b	0x2ab8 <makingWndArray+0x23c> @ imm = #-0x1d0
    2c84: e320f000     	nop
    2c88: 7b 14 ae 47  	.word	0x47ae147b
    2c8c: e1 7a 94 3f  	.word	0x3f947ae1
    2c90: cd cc cc cc  	.word	0xcccccccd
    2c94: cc cc ec 3f  	.word	0x3feccccc
    2c98: 9a 99 99 99  	.word	0x9999999a
    2c9c: 99 99 b9 3f  	.word	0x3fb99999
    2ca0: 00 00 48 43  	.word	0x43480000
    2ca4: 00 00 00 00  	.word	0x00000000
    2ca8: 70 57 01 00  	.word	0x00015770
    2cac: d4 00 00 00  	.word	0x000000d4
    2cb0: 68 34 00 00  	.word	0x00003468
    2cb4: f4 33 00 00  	.word	0x000033f4
    2cb8: 88 32 00 00  	.word	0x00003288
    2cbc: 78 52 00 00  	.word	0x00005278
    2cc0: ee049a10     	vmov	s8, r9
    2cc4: eef86bc4     	vcvt.f64.s32	d22, s8
    2cc8: ee667ba1     	vmul.f64	d23, d22, d17
    2ccc: eef74be7     	vcvt.f32.f64	s9, d23
    2cd0: eaffffd2     	b	0x2c20 <makingWndArray+0x3a4> @ imm = #-0xb8
    2cd4: ee046a90     	vmov	s9, r6
    2cd8: eef8fbe4     	vcvt.f64.s32	d31, s9
    2cdc: ee6f4ba1     	vmul.f64	d20, d31, d17
    2ce0: eef74be4     	vcvt.f32.f64	s9, d20
    2ce4: eaffffb7     	b	0x2bc8 <makingWndArray+0x34c> @ imm = #-0x124
    2ce8: ee079a90     	vmov	s15, r9
    2cec: eef8bbe7     	vcvt.f64.s32	d27, s15
    2cf0: ee6bcba1     	vmul.f64	d28, d27, d17
    2cf4: eef74bec     	vcvt.f32.f64	s9, d28
    2cf8: eaffff9c     	b	0x2b70 <makingWndArray+0x2f4> @ imm = #-0x190
    2cfc: ee043a90     	vmov	s9, r3
    2d00: eef87be4     	vcvt.f64.s32	d23, s9
    2d04: ee678ba1     	vmul.f64	d24, d23, d17
    2d08: eef72be8     	vcvt.f32.f64	s5, d24
    2d0c: eaffff81     	b	0x2b18 <makingWndArray+0x29c> @ imm = #-0x1fc

