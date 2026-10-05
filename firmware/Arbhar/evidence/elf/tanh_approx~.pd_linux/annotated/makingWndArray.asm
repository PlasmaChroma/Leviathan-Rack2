000028fc <makingWndArray>:
    28fc: eef76a00     	vmov.f32	s13, #1.000000e+00
    2900: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x2d28 <makingWndArray+0x42c>  // u32=0x156f0; f32?=1.23022795e-40
    2904: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x2d2c <makingWndArray+0x430>  // u32=0xbc; f32?=2.63444111e-43
    2908: e08f3003     	add	r3, pc, r3
    290c: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x2d30 <makingWndArray+0x434>  // u32=0x3088; f32?=1.74097321e-41
    2910: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2914: e08f4000     	add	r4, pc, r0
    2918: e793c002     	ldr	r12, [r3, r2]
    291c: e2845b02     	add	r5, r4, #2048
    2920: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x2d08 <makingWndArray+0x40c>  // f64=0.02
    2924: e285000c     	add	r0, r5, #12
    2928: e28c6bcb     	add	r6, r12, #207872
    292c: e3e0e031     	mvn	lr, #49
    2930: e28650bc     	add	r5, r6, #188
    2934: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x2d20 <makingWndArray+0x424>  // f32=200
    2938: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x2d10 <makingWndArray+0x414>  // f64=0.90000000000000002
    293c: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x2d18 <makingWndArray+0x41c>  // f64=0.10000000000000001
    2940: ee07ea90     	vmov	s15, lr
    2944: eeb85be7     	vcvt.f64.s32	d5, s15
    2948: ee255b23     	vmul.f64	d5, d5, d19
    294c: eeb70bc5     	vcvt.f32.f64	s0, d5
    2950: eeb14a40     	vneg.f32	s8, s0
    2954: ee705a26     	vadd.f32	s11, s0, s13
    2958: ee764ac0     	vsub.f32	s9, s13, s0
    295c: eef74ac4     	vcvt.f64.f32	d20, s8
    2960: eeb50ac0     	vcmpe.f32	s0, #0
    2964: ee241ba2     	vmul.f64	d1, d20, d18
    2968: eef1fa10     	vmrs	APSR_nzcv, fpscr
    296c: ee640aa5     	vmul.f32	s1, s9, s11
    2970: ee602a06     	vmul.f32	s5, s0, s12
    2974: 9a00005b     	bls	0x2ae8 <makingWndArray+0x1ec> @ imm = #0x16c
    2978: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x2d34 <makingWndArray+0x438>  // u32=0x3014; f32?=1.72471815e-41
    297c: e1a0700c     	mov	r7, r12
    2980: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x2d24 <makingWndArray+0x428>  // f32=0
    2984: e3004202     	movw	r4, #0x202
    2988: e08f2001     	add	r2, pc, r1
    298c: e3a08001     	mov	r8, #1
    2990: e3001203     	movw	r1, #0x203
    2994: e4923004     	ldr	r3, [r2], #4
    2998: ece71a01     	vstmia	r7!, {s3}
    299c: ea000039     	b	0x2a88 <makingWndArray+0x18c> @ imm = #0xe4
    29a0: e3580f7e     	cmp	r8, #504
    29a4: def03a66     	vmovle.f32	s7, s13
    29a8: ca000048     	bgt	0x2ad0 <makingWndArray+0x1d4> @ imm = #0x120
    29ac: ee018a10     	vmov	s2, r8
    29b0: e2883001     	add	r3, r8, #1
    29b4: e1a06002     	mov	r6, r2
    29b8: ecf64a01     	vldmia	r6!, {s9}
    29bc: eeb82ac1     	vcvt.f32.s32	s4, s2
    29c0: ee327a22     	vadd.f32	s14, s4, s5
    29c4: eebd3ac7     	vcvt.s32.f32	s6, s14
    29c8: ee138a10     	vmov	r8, s6
    29cc: ee607a24     	vmul.f32	s15, s0, s9
    29d0: e1580001     	cmp	r8, r1
    29d4: a1a08001     	movge	r8, r1
    29d8: e0802108     	add	r2, r0, r8, lsl #2
    29dc: ed925a00     	vldr	s10, [r2]
    29e0: ee407a85     	vmla.f32	s15, s1, s10
    29e4: eef47ae6     	vcmpe.f32	s15, s13
    29e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    29ec: 8ef07a66     	vmovhi.f32	s15, s13
    29f0: eef57ac0     	vcmpe.f32	s15, #0
    29f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    29f8: bef07a61     	vmovlt.f32	s15, s3
    29fc: e3530009     	cmp	r3, #9
    2a00: ee274aa3     	vmul.f32	s8, s15, s7
    2a04: eca74a01     	vstmia	r7!, {s8}
    2a08: da00002b     	ble	0x2abc <makingWndArray+0x1c0> @ imm = #0xac
    2a0c: e3530f7e     	cmp	r3, #504
    2a10: c0442003     	subgt	r2, r4, r3
    2a14: def03a66     	vmovle.f32	s7, s13
    2a18: ce042a10     	vmovgt	s8, r2
    2a1c: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2a20: ce600ba1     	vmulgt.f64	d16, d16, d17
    2a24: cef73be0     	vcvtgt.f32.f64	s7, d16
    2a28: ee013a10     	vmov	s2, r3
    2a2c: e2838001     	add	r8, r3, #1
    2a30: e1a02006     	mov	r2, r6
    2a34: ecf25a01     	vldmia	r2!, {s11}
    2a38: eeb82ac1     	vcvt.f32.s32	s4, s2
    2a3c: ee724a22     	vadd.f32	s9, s4, s5
    2a40: eebd7ae4     	vcvt.s32.f32	s14, s9
    2a44: ee173a10     	vmov	r3, s14
    2a48: ee203a25     	vmul.f32	s6, s0, s11
    2a4c: e1530001     	cmp	r3, r1
    2a50: a1a03001     	movge	r3, r1
    2a54: e0803103     	add	r3, r0, r3, lsl #2
    2a58: edd37a00     	vldr	s15, [r3]
    2a5c: ee003aa7     	vmla.f32	s6, s1, s15
    2a60: eeb43ae6     	vcmpe.f32	s6, s13
    2a64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a68: 8eb03a66     	vmovhi.f32	s6, s13
    2a6c: eeb53ac0     	vcmpe.f32	s6, #0
    2a70: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a74: beb03a61     	vmovlt.f32	s6, s3
    2a78: e1580001     	cmp	r8, r1
    2a7c: ee235a23     	vmul.f32	s10, s6, s7
    2a80: eca75a01     	vstmia	r7!, {s10}
    2a84: 0a000006     	beq	0x2aa4 <makingWndArray+0x1a8> @ imm = #0x18
    2a88: e3580009     	cmp	r8, #9
    2a8c: caffffc3     	bgt	0x29a0 <makingWndArray+0xa4> @ imm = #-0xf4
    2a90: ee058a90     	vmov	s11, r8
    2a94: eef8abe5     	vcvt.f64.s32	d26, s11
    2a98: ee6a0ba1     	vmul.f64	d16, d26, d17
    2a9c: eef73be0     	vcvt.f32.f64	s7, d16
    2aa0: eaffffc1     	b	0x29ac <makingWndArray+0xb0> @ imm = #-0xfc
    2aa4: e28ccb02     	add	r12, r12, #2048
    2aa8: e28ee001     	add	lr, lr, #1
    2aac: e28cc00c     	add	r12, r12, #12
    2ab0: e155000c     	cmp	r5, r12
    2ab4: 1affffa1     	bne	0x2940 <makingWndArray+0x44> @ imm = #-0x17c
    2ab8: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    2abc: ee033a90     	vmov	s7, r3
    2ac0: eef8bbe3     	vcvt.f64.s32	d27, s7
    2ac4: ee6b0ba1     	vmul.f64	d16, d27, d17
    2ac8: eef73be0     	vcvt.f32.f64	s7, d16
    2acc: eaffffd5     	b	0x2a28 <makingWndArray+0x12c> @ imm = #-0xac
    2ad0: e0446008     	sub	r6, r4, r8
    2ad4: ee036a90     	vmov	s7, r6
    2ad8: eef89be3     	vcvt.f64.s32	d25, s7
    2adc: ee690ba1     	vmul.f64	d16, d25, d17
    2ae0: eef73be0     	vcvt.f32.f64	s7, d16
    2ae4: eaffffb0     	b	0x29ac <makingWndArray+0xb0> @ imm = #-0x140
    2ae8: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x2d38 <makingWndArray+0x43c>  // u32=0x2ea8; f32?=1.67371089e-41
    2aec: e1a0900c     	mov	r9, r12
    2af0: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x2d3c <makingWndArray+0x440>  // u32=0x4e98; f32?=2.81941251e-41
    2af4: e08f8007     	add	r8, pc, r7
    2af8: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x2d24 <makingWndArray+0x428>  // f32=0
    2afc: e08f4003     	add	r4, pc, r3
    2b00: e2882b02     	add	r2, r8, #2048
    2b04: e3a03000     	mov	r3, #0
    2b08: e282200c     	add	r2, r2, #12
    2b0c: e2446efe     	sub	r6, r4, #4064
    2b10: e3008202     	movw	r8, #0x202
    2b14: e3007203     	movw	r7, #0x203
    2b18: ea000072     	b	0x2ce8 <makingWndArray+0x3ec> @ imm = #0x1c8
    2b1c: e3530f7e     	cmp	r3, #504
    2b20: c0481003     	subgt	r1, r8, r3
    2b24: def02a66     	vmovle.f32	s5, s13
    2b28: ce041a10     	vmovgt	s8, r1
    2b2c: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2b30: ce600ba1     	vmulgt.f64	d16, d16, d17
    2b34: cef72be0     	vcvtgt.f32.f64	s5, d16
    2b38: ecb27a01     	vldmia	r2!, {s14}
    2b3c: e1a04006     	mov	r4, r6
    2b40: e2833001     	add	r3, r3, #1
    2b44: e1a01009     	mov	r1, r9
    2b48: ecb43a01     	vldmia	r4!, {s6}
    2b4c: ee657a87     	vmul.f32	s15, s11, s14
    2b50: eef75ac3     	vcvt.f64.f32	d21, s6
    2b54: eef76ae7     	vcvt.f64.f32	d22, s15
    2b58: ee456b81     	vmla.f64	d22, d21, d1
    2b5c: eeb75be6     	vcvt.f32.f64	s10, d22
    2b60: eeb55ac0     	vcmpe.f32	s10, #0
    2b64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2b68: beb05a42     	vmovlt.f32	s10, s4
    2b6c: e3530009     	cmp	r3, #9
    2b70: ee250a22     	vmul.f32	s0, s10, s5
    2b74: eca10a01     	vstmia	r1!, {s0}
    2b78: da00007f     	ble	0x2d7c <makingWndArray+0x480> @ imm = #0x1fc
    2b7c: e3530f7e     	cmp	r3, #504
    2b80: c0486003     	subgt	r6, r8, r3
    2b84: def02a66     	vmovle.f32	s5, s13
    2b88: ce006a10     	vmovgt	s0, r6
    2b8c: cef86bc0     	vcvtgt.f64.s32	d22, s0
    2b90: ce666ba1     	vmulgt.f64	d22, d22, d17
    2b94: cef72be6     	vcvtgt.f32.f64	s5, d22
    2b98: edd20a00     	vldr	s1, [r2]
    2b9c: e2839001     	add	r9, r3, #1
    2ba0: edd44a00     	vldr	s9, [r4]
    2ba4: ee254aa0     	vmul.f32	s8, s11, s1
    2ba8: eef79ae4     	vcvt.f64.f32	d25, s9
    2bac: eef7aac4     	vcvt.f64.f32	d26, s8
    2bb0: ee49ab81     	vmla.f64	d26, d25, d1
    2bb4: eeb77bea     	vcvt.f32.f64	s14, d26
    2bb8: eeb57ac0     	vcmpe.f32	s14, #0
    2bbc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2bc0: beb07a42     	vmovlt.f32	s14, s4
    2bc4: e3590009     	cmp	r9, #9
    2bc8: ee273a22     	vmul.f32	s6, s14, s5
    2bcc: ed813a00     	vstr	s6, [r1]
    2bd0: da000064     	ble	0x2d68 <makingWndArray+0x46c> @ imm = #0x190
    2bd4: e3590f7e     	cmp	r9, #504
    2bd8: c0489009     	subgt	r9, r8, r9
    2bdc: def04a66     	vmovle.f32	s9, s13
    2be0: ce039a10     	vmovgt	s6, r9
    2be4: cef8abc3     	vcvtgt.f64.s32	d26, s6
    2be8: ce6aaba1     	vmulgt.f64	d26, d26, d17
    2bec: cef74bea     	vcvtgt.f32.f64	s9, d26
    2bf0: ed925a01     	vldr	s10, [r2, #4]
    2bf4: e2836002     	add	r6, r3, #2
    2bf8: ed940a01     	vldr	s0, [r4, #4]
    2bfc: ee650a85     	vmul.f32	s1, s11, s10
    2c00: eef7dac0     	vcvt.f64.f32	d29, s0
    2c04: eef7eae0     	vcvt.f64.f32	d30, s1
    2c08: ee4deb81     	vmla.f64	d30, d29, d1
    2c0c: eeb74bee     	vcvt.f32.f64	s8, d30
    2c10: eeb54ac0     	vcmpe.f32	s8, #0
    2c14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c18: beb04a42     	vmovlt.f32	s8, s4
    2c1c: e3560009     	cmp	r6, #9
    2c20: ee247a24     	vmul.f32	s14, s8, s9
    2c24: ed817a01     	vstr	s14, [r1, #4]
    2c28: da000049     	ble	0x2d54 <makingWndArray+0x458> @ imm = #0x124
    2c2c: e3560f7e     	cmp	r6, #504
    2c30: c0486006     	subgt	r6, r8, r6
    2c34: def04a66     	vmovle.f32	s9, s13
    2c38: ce076a10     	vmovgt	s14, r6
    2c3c: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    2c40: ce6eeba1     	vmulgt.f64	d30, d30, d17
    2c44: cef74bee     	vcvtgt.f32.f64	s9, d30
    2c48: ed923a02     	vldr	s6, [r2, #8]
    2c4c: e2839003     	add	r9, r3, #3
    2c50: edd47a02     	vldr	s15, [r4, #8]
    2c54: ee255a83     	vmul.f32	s10, s11, s6
    2c58: eef75ae7     	vcvt.f64.f32	d21, s15
    2c5c: eef70ac5     	vcvt.f64.f32	d16, s10
    2c60: ee450b81     	vmla.f64	d16, d21, d1
    2c64: eeb70be0     	vcvt.f32.f64	s0, d16
    2c68: eeb50ac0     	vcmpe.f32	s0, #0
    2c6c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c70: beb00a42     	vmovlt.f32	s0, s4
    2c74: e3590009     	cmp	r9, #9
    2c78: ee600a24     	vmul.f32	s1, s0, s9
    2c7c: edc10a02     	vstr	s1, [r1, #8]
    2c80: da00002e     	ble	0x2d40 <makingWndArray+0x444> @ imm = #0xb8
    2c84: e3590f7e     	cmp	r9, #504
    2c88: c0489009     	subgt	r9, r8, r9
    2c8c: def04a66     	vmovle.f32	s9, s13
    2c90: ce009a90     	vmovgt	s1, r9
    2c94: cef80be0     	vcvtgt.f64.s32	d16, s1
    2c98: ce600ba1     	vmulgt.f64	d16, d16, d17
    2c9c: cef74be0     	vcvtgt.f32.f64	s9, d16
    2ca0: ed927a03     	vldr	s14, [r2, #12]
    2ca4: e2833004     	add	r3, r3, #4
    2ca8: e2822010     	add	r2, r2, #16
    2cac: e2846010     	add	r6, r4, #16
    2cb0: e2819010     	add	r9, r1, #16
    2cb4: ed943a03     	vldr	s6, [r4, #12]
    2cb8: ee657a87     	vmul.f32	s15, s11, s14
    2cbc: eef78ac3     	vcvt.f64.f32	d24, s6
    2cc0: eef70ae7     	vcvt.f64.f32	d16, s15
    2cc4: ee480b81     	vmla.f64	d16, d24, d1
    2cc8: eeb75be0     	vcvt.f32.f64	s10, d16
    2ccc: eeb55ac0     	vcmpe.f32	s10, #0
    2cd0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2cd4: beb05a42     	vmovlt.f32	s10, s4
    2cd8: e1530007     	cmp	r3, r7
    2cdc: ee254a24     	vmul.f32	s8, s10, s9
    2ce0: ed814a03     	vstr	s8, [r1, #12]
    2ce4: 0affff6e     	beq	0x2aa4 <makingWndArray+0x1a8> @ imm = #-0x248
    2ce8: e3530009     	cmp	r3, #9
    2cec: caffff8a     	bgt	0x2b1c <makingWndArray+0x220> @ imm = #-0x1d8
    2cf0: ee023a90     	vmov	s5, r3
    2cf4: eef80be2     	vcvt.f64.s32	d16, s5
    2cf8: ee204ba1     	vmul.f64	d4, d16, d17
    2cfc: eef72bc4     	vcvt.f32.f64	s5, d4
    2d00: eaffff8c     	b	0x2b38 <makingWndArray+0x23c> @ imm = #-0x1d0
    2d04: e320f000     	nop
    2d08: 7b 14 ae 47  	.word	0x47ae147b
    2d0c: e1 7a 94 3f  	.word	0x3f947ae1
    2d10: cd cc cc cc  	.word	0xcccccccd
    2d14: cc cc ec 3f  	.word	0x3feccccc
    2d18: 9a 99 99 99  	.word	0x9999999a
    2d1c: 99 99 b9 3f  	.word	0x3fb99999
    2d20: 00 00 48 43  	.word	0x43480000
    2d24: 00 00 00 00  	.word	0x00000000
    2d28: f0 56 01 00  	.word	0x000156f0
    2d2c: bc 00 00 00  	.word	0x000000bc
    2d30: 88 30 00 00  	.word	0x00003088
    2d34: 14 30 00 00  	.word	0x00003014
    2d38: a8 2e 00 00  	.word	0x00002ea8
    2d3c: 98 4e 00 00  	.word	0x00004e98
    2d40: ee049a10     	vmov	s8, r9
    2d44: eef86bc4     	vcvt.f64.s32	d22, s8
    2d48: ee667ba1     	vmul.f64	d23, d22, d17
    2d4c: eef74be7     	vcvt.f32.f64	s9, d23
    2d50: eaffffd2     	b	0x2ca0 <makingWndArray+0x3a4> @ imm = #-0xb8
    2d54: ee046a90     	vmov	s9, r6
    2d58: eef8fbe4     	vcvt.f64.s32	d31, s9
    2d5c: ee6f4ba1     	vmul.f64	d20, d31, d17
    2d60: eef74be4     	vcvt.f32.f64	s9, d20
    2d64: eaffffb7     	b	0x2c48 <makingWndArray+0x34c> @ imm = #-0x124
    2d68: ee079a90     	vmov	s15, r9
    2d6c: eef8bbe7     	vcvt.f64.s32	d27, s15
    2d70: ee6bcba1     	vmul.f64	d28, d27, d17
    2d74: eef74bec     	vcvt.f32.f64	s9, d28
    2d78: eaffff9c     	b	0x2bf0 <makingWndArray+0x2f4> @ imm = #-0x190
    2d7c: ee043a90     	vmov	s9, r3
    2d80: eef87be4     	vcvt.f64.s32	d23, s9
    2d84: ee678ba1     	vmul.f64	d24, d23, d17
    2d88: eef72be8     	vcvt.f32.f64	s5, d24
    2d8c: eaffff81     	b	0x2b98 <makingWndArray+0x29c> @ imm = #-0x1fc

