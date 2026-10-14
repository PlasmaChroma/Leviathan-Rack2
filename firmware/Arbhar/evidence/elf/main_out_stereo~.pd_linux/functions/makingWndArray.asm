000028a4 <makingWndArray>:
    28a4: eef76a00     	vmov.f32	s13, #1.000000e+00
    28a8: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x2cd0 <makingWndArray+0x42c>
    28ac: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x2cd4 <makingWndArray+0x430>
    28b0: e08f3003     	add	r3, pc, r3
    28b4: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x2cd8 <makingWndArray+0x434>
    28b8: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    28bc: e08f4000     	add	r4, pc, r0
    28c0: e793c002     	ldr	r12, [r3, r2]
    28c4: e2845b02     	add	r5, r4, #2048
    28c8: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x2cb0 <makingWndArray+0x40c>
    28cc: e285000c     	add	r0, r5, #12
    28d0: e28c6bcb     	add	r6, r12, #207872
    28d4: e3e0e031     	mvn	lr, #49
    28d8: e28650bc     	add	r5, r6, #188
    28dc: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x2cc8 <makingWndArray+0x424>
    28e0: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x2cb8 <makingWndArray+0x414>
    28e4: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x2cc0 <makingWndArray+0x41c>
    28e8: ee07ea90     	vmov	s15, lr
    28ec: eeb85be7     	vcvt.f64.s32	d5, s15
    28f0: ee255b23     	vmul.f64	d5, d5, d19
    28f4: eeb70bc5     	vcvt.f32.f64	s0, d5
    28f8: eeb14a40     	vneg.f32	s8, s0
    28fc: ee705a26     	vadd.f32	s11, s0, s13
    2900: ee764ac0     	vsub.f32	s9, s13, s0
    2904: eef74ac4     	vcvt.f64.f32	d20, s8
    2908: eeb50ac0     	vcmpe.f32	s0, #0
    290c: ee241ba2     	vmul.f64	d1, d20, d18
    2910: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2914: ee640aa5     	vmul.f32	s1, s9, s11
    2918: ee602a06     	vmul.f32	s5, s0, s12
    291c: 9a00005b     	bls	0x2a90 <makingWndArray+0x1ec> @ imm = #0x16c
    2920: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x2cdc <makingWndArray+0x438>
    2924: e1a0700c     	mov	r7, r12
    2928: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x2ccc <makingWndArray+0x428>
    292c: e3004202     	movw	r4, #0x202
    2930: e08f2001     	add	r2, pc, r1
    2934: e3a08001     	mov	r8, #1
    2938: e3001203     	movw	r1, #0x203
    293c: e4923004     	ldr	r3, [r2], #4
    2940: ece71a01     	vstmia	r7!, {s3}
    2944: ea000039     	b	0x2a30 <makingWndArray+0x18c> @ imm = #0xe4
    2948: e3580f7e     	cmp	r8, #504
    294c: def03a66     	vmovle.f32	s7, s13
    2950: ca000048     	bgt	0x2a78 <makingWndArray+0x1d4> @ imm = #0x120
    2954: ee018a10     	vmov	s2, r8
    2958: e2883001     	add	r3, r8, #1
    295c: e1a06002     	mov	r6, r2
    2960: ecf64a01     	vldmia	r6!, {s9}
    2964: eeb82ac1     	vcvt.f32.s32	s4, s2
    2968: ee327a22     	vadd.f32	s14, s4, s5
    296c: eebd3ac7     	vcvt.s32.f32	s6, s14
    2970: ee138a10     	vmov	r8, s6
    2974: ee607a24     	vmul.f32	s15, s0, s9
    2978: e1580001     	cmp	r8, r1
    297c: a1a08001     	movge	r8, r1
    2980: e0802108     	add	r2, r0, r8, lsl #2
    2984: ed925a00     	vldr	s10, [r2]
    2988: ee407a85     	vmla.f32	s15, s1, s10
    298c: eef47ae6     	vcmpe.f32	s15, s13
    2990: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2994: 8ef07a66     	vmovhi.f32	s15, s13
    2998: eef57ac0     	vcmpe.f32	s15, #0
    299c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    29a0: bef07a61     	vmovlt.f32	s15, s3
    29a4: e3530009     	cmp	r3, #9
    29a8: ee274aa3     	vmul.f32	s8, s15, s7
    29ac: eca74a01     	vstmia	r7!, {s8}
    29b0: da00002b     	ble	0x2a64 <makingWndArray+0x1c0> @ imm = #0xac
    29b4: e3530f7e     	cmp	r3, #504
    29b8: c0442003     	subgt	r2, r4, r3
    29bc: def03a66     	vmovle.f32	s7, s13
    29c0: ce042a10     	vmovgt	s8, r2
    29c4: cef80bc4     	vcvtgt.f64.s32	d16, s8
    29c8: ce600ba1     	vmulgt.f64	d16, d16, d17
    29cc: cef73be0     	vcvtgt.f32.f64	s7, d16
    29d0: ee013a10     	vmov	s2, r3
    29d4: e2838001     	add	r8, r3, #1
    29d8: e1a02006     	mov	r2, r6
    29dc: ecf25a01     	vldmia	r2!, {s11}
    29e0: eeb82ac1     	vcvt.f32.s32	s4, s2
    29e4: ee724a22     	vadd.f32	s9, s4, s5
    29e8: eebd7ae4     	vcvt.s32.f32	s14, s9
    29ec: ee173a10     	vmov	r3, s14
    29f0: ee203a25     	vmul.f32	s6, s0, s11
    29f4: e1530001     	cmp	r3, r1
    29f8: a1a03001     	movge	r3, r1
    29fc: e0803103     	add	r3, r0, r3, lsl #2
    2a00: edd37a00     	vldr	s15, [r3]
    2a04: ee003aa7     	vmla.f32	s6, s1, s15
    2a08: eeb43ae6     	vcmpe.f32	s6, s13
    2a0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a10: 8eb03a66     	vmovhi.f32	s6, s13
    2a14: eeb53ac0     	vcmpe.f32	s6, #0
    2a18: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a1c: beb03a61     	vmovlt.f32	s6, s3
    2a20: e1580001     	cmp	r8, r1
    2a24: ee235a23     	vmul.f32	s10, s6, s7
    2a28: eca75a01     	vstmia	r7!, {s10}
    2a2c: 0a000006     	beq	0x2a4c <makingWndArray+0x1a8> @ imm = #0x18
    2a30: e3580009     	cmp	r8, #9
    2a34: caffffc3     	bgt	0x2948 <makingWndArray+0xa4> @ imm = #-0xf4
    2a38: ee058a90     	vmov	s11, r8
    2a3c: eef8abe5     	vcvt.f64.s32	d26, s11
    2a40: ee6a0ba1     	vmul.f64	d16, d26, d17
    2a44: eef73be0     	vcvt.f32.f64	s7, d16
    2a48: eaffffc1     	b	0x2954 <makingWndArray+0xb0> @ imm = #-0xfc
    2a4c: e28ccb02     	add	r12, r12, #2048
    2a50: e28ee001     	add	lr, lr, #1
    2a54: e28cc00c     	add	r12, r12, #12
    2a58: e155000c     	cmp	r5, r12
    2a5c: 1affffa1     	bne	0x28e8 <makingWndArray+0x44> @ imm = #-0x17c
    2a60: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    2a64: ee033a90     	vmov	s7, r3
    2a68: eef8bbe3     	vcvt.f64.s32	d27, s7
    2a6c: ee6b0ba1     	vmul.f64	d16, d27, d17
    2a70: eef73be0     	vcvt.f32.f64	s7, d16
    2a74: eaffffd5     	b	0x29d0 <makingWndArray+0x12c> @ imm = #-0xac
    2a78: e0446008     	sub	r6, r4, r8
    2a7c: ee036a90     	vmov	s7, r6
    2a80: eef89be3     	vcvt.f64.s32	d25, s7
    2a84: ee690ba1     	vmul.f64	d16, d25, d17
    2a88: eef73be0     	vcvt.f32.f64	s7, d16
    2a8c: eaffffb0     	b	0x2954 <makingWndArray+0xb0> @ imm = #-0x140
    2a90: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x2ce0 <makingWndArray+0x43c>
    2a94: e1a0900c     	mov	r9, r12
    2a98: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x2ce4 <makingWndArray+0x440>
    2a9c: e08f8007     	add	r8, pc, r7
    2aa0: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x2ccc <makingWndArray+0x428>
    2aa4: e08f4003     	add	r4, pc, r3
    2aa8: e2882b02     	add	r2, r8, #2048
    2aac: e3a03000     	mov	r3, #0
    2ab0: e282200c     	add	r2, r2, #12
    2ab4: e2446efe     	sub	r6, r4, #4064
    2ab8: e3008202     	movw	r8, #0x202
    2abc: e3007203     	movw	r7, #0x203
    2ac0: ea000072     	b	0x2c90 <makingWndArray+0x3ec> @ imm = #0x1c8
    2ac4: e3530f7e     	cmp	r3, #504
    2ac8: c0481003     	subgt	r1, r8, r3
    2acc: def02a66     	vmovle.f32	s5, s13
    2ad0: ce041a10     	vmovgt	s8, r1
    2ad4: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2ad8: ce600ba1     	vmulgt.f64	d16, d16, d17
    2adc: cef72be0     	vcvtgt.f32.f64	s5, d16
    2ae0: ecb27a01     	vldmia	r2!, {s14}
    2ae4: e1a04006     	mov	r4, r6
    2ae8: e2833001     	add	r3, r3, #1
    2aec: e1a01009     	mov	r1, r9
    2af0: ecb43a01     	vldmia	r4!, {s6}
    2af4: ee657a87     	vmul.f32	s15, s11, s14
    2af8: eef75ac3     	vcvt.f64.f32	d21, s6
    2afc: eef76ae7     	vcvt.f64.f32	d22, s15
    2b00: ee456b81     	vmla.f64	d22, d21, d1
    2b04: eeb75be6     	vcvt.f32.f64	s10, d22
    2b08: eeb55ac0     	vcmpe.f32	s10, #0
    2b0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2b10: beb05a42     	vmovlt.f32	s10, s4
    2b14: e3530009     	cmp	r3, #9
    2b18: ee250a22     	vmul.f32	s0, s10, s5
    2b1c: eca10a01     	vstmia	r1!, {s0}
    2b20: da00007f     	ble	0x2d24 <makingWndArray+0x480> @ imm = #0x1fc
    2b24: e3530f7e     	cmp	r3, #504
    2b28: c0486003     	subgt	r6, r8, r3
    2b2c: def02a66     	vmovle.f32	s5, s13
    2b30: ce006a10     	vmovgt	s0, r6
    2b34: cef86bc0     	vcvtgt.f64.s32	d22, s0
    2b38: ce666ba1     	vmulgt.f64	d22, d22, d17
    2b3c: cef72be6     	vcvtgt.f32.f64	s5, d22
    2b40: edd20a00     	vldr	s1, [r2]
    2b44: e2839001     	add	r9, r3, #1
    2b48: edd44a00     	vldr	s9, [r4]
    2b4c: ee254aa0     	vmul.f32	s8, s11, s1
    2b50: eef79ae4     	vcvt.f64.f32	d25, s9
    2b54: eef7aac4     	vcvt.f64.f32	d26, s8
    2b58: ee49ab81     	vmla.f64	d26, d25, d1
    2b5c: eeb77bea     	vcvt.f32.f64	s14, d26
    2b60: eeb57ac0     	vcmpe.f32	s14, #0
    2b64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2b68: beb07a42     	vmovlt.f32	s14, s4
    2b6c: e3590009     	cmp	r9, #9
    2b70: ee273a22     	vmul.f32	s6, s14, s5
    2b74: ed813a00     	vstr	s6, [r1]
    2b78: da000064     	ble	0x2d10 <makingWndArray+0x46c> @ imm = #0x190
    2b7c: e3590f7e     	cmp	r9, #504
    2b80: c0489009     	subgt	r9, r8, r9
    2b84: def04a66     	vmovle.f32	s9, s13
    2b88: ce039a10     	vmovgt	s6, r9
    2b8c: cef8abc3     	vcvtgt.f64.s32	d26, s6
    2b90: ce6aaba1     	vmulgt.f64	d26, d26, d17
    2b94: cef74bea     	vcvtgt.f32.f64	s9, d26
    2b98: ed925a01     	vldr	s10, [r2, #4]
    2b9c: e2836002     	add	r6, r3, #2
    2ba0: ed940a01     	vldr	s0, [r4, #4]
    2ba4: ee650a85     	vmul.f32	s1, s11, s10
    2ba8: eef7dac0     	vcvt.f64.f32	d29, s0
    2bac: eef7eae0     	vcvt.f64.f32	d30, s1
    2bb0: ee4deb81     	vmla.f64	d30, d29, d1
    2bb4: eeb74bee     	vcvt.f32.f64	s8, d30
    2bb8: eeb54ac0     	vcmpe.f32	s8, #0
    2bbc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2bc0: beb04a42     	vmovlt.f32	s8, s4
    2bc4: e3560009     	cmp	r6, #9
    2bc8: ee247a24     	vmul.f32	s14, s8, s9
    2bcc: ed817a01     	vstr	s14, [r1, #4]
    2bd0: da000049     	ble	0x2cfc <makingWndArray+0x458> @ imm = #0x124
    2bd4: e3560f7e     	cmp	r6, #504
    2bd8: c0486006     	subgt	r6, r8, r6
    2bdc: def04a66     	vmovle.f32	s9, s13
    2be0: ce076a10     	vmovgt	s14, r6
    2be4: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    2be8: ce6eeba1     	vmulgt.f64	d30, d30, d17
    2bec: cef74bee     	vcvtgt.f32.f64	s9, d30
    2bf0: ed923a02     	vldr	s6, [r2, #8]
    2bf4: e2839003     	add	r9, r3, #3
    2bf8: edd47a02     	vldr	s15, [r4, #8]
    2bfc: ee255a83     	vmul.f32	s10, s11, s6
    2c00: eef75ae7     	vcvt.f64.f32	d21, s15
    2c04: eef70ac5     	vcvt.f64.f32	d16, s10
    2c08: ee450b81     	vmla.f64	d16, d21, d1
    2c0c: eeb70be0     	vcvt.f32.f64	s0, d16
    2c10: eeb50ac0     	vcmpe.f32	s0, #0
    2c14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c18: beb00a42     	vmovlt.f32	s0, s4
    2c1c: e3590009     	cmp	r9, #9
    2c20: ee600a24     	vmul.f32	s1, s0, s9
    2c24: edc10a02     	vstr	s1, [r1, #8]
    2c28: da00002e     	ble	0x2ce8 <makingWndArray+0x444> @ imm = #0xb8
    2c2c: e3590f7e     	cmp	r9, #504
    2c30: c0489009     	subgt	r9, r8, r9
    2c34: def04a66     	vmovle.f32	s9, s13
    2c38: ce009a90     	vmovgt	s1, r9
    2c3c: cef80be0     	vcvtgt.f64.s32	d16, s1
    2c40: ce600ba1     	vmulgt.f64	d16, d16, d17
    2c44: cef74be0     	vcvtgt.f32.f64	s9, d16
    2c48: ed927a03     	vldr	s14, [r2, #12]
    2c4c: e2833004     	add	r3, r3, #4
    2c50: e2822010     	add	r2, r2, #16
    2c54: e2846010     	add	r6, r4, #16
    2c58: e2819010     	add	r9, r1, #16
    2c5c: ed943a03     	vldr	s6, [r4, #12]
    2c60: ee657a87     	vmul.f32	s15, s11, s14
    2c64: eef78ac3     	vcvt.f64.f32	d24, s6
    2c68: eef70ae7     	vcvt.f64.f32	d16, s15
    2c6c: ee480b81     	vmla.f64	d16, d24, d1
    2c70: eeb75be0     	vcvt.f32.f64	s10, d16
    2c74: eeb55ac0     	vcmpe.f32	s10, #0
    2c78: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c7c: beb05a42     	vmovlt.f32	s10, s4
    2c80: e1530007     	cmp	r3, r7
    2c84: ee254a24     	vmul.f32	s8, s10, s9
    2c88: ed814a03     	vstr	s8, [r1, #12]
    2c8c: 0affff6e     	beq	0x2a4c <makingWndArray+0x1a8> @ imm = #-0x248
    2c90: e3530009     	cmp	r3, #9
    2c94: caffff8a     	bgt	0x2ac4 <makingWndArray+0x220> @ imm = #-0x1d8
    2c98: ee023a90     	vmov	s5, r3
    2c9c: eef80be2     	vcvt.f64.s32	d16, s5
    2ca0: ee204ba1     	vmul.f64	d4, d16, d17
    2ca4: eef72bc4     	vcvt.f32.f64	s5, d4
    2ca8: eaffff8c     	b	0x2ae0 <makingWndArray+0x23c> @ imm = #-0x1d0
    2cac: e320f000     	nop
    2cb0: 7b 14 ae 47  	.word	0x47ae147b
    2cb4: e1 7a 94 3f  	.word	0x3f947ae1
    2cb8: cd cc cc cc  	.word	0xcccccccd
    2cbc: cc cc ec 3f  	.word	0x3feccccc
    2cc0: 9a 99 99 99  	.word	0x9999999a
    2cc4: 99 99 b9 3f  	.word	0x3fb99999
    2cc8: 00 00 48 43  	.word	0x43480000
    2ccc: 00 00 00 00  	.word	0x00000000
    2cd0: 48 57 01 00  	.word	0x00015748
    2cd4: c0 00 00 00  	.word	0x000000c0
    2cd8: 90 30 00 00  	.word	0x00003090
    2cdc: 1c 30 00 00  	.word	0x0000301c
    2ce0: b0 2e 00 00  	.word	0x00002eb0
    2ce4: a0 4e 00 00  	.word	0x00004ea0
    2ce8: ee049a10     	vmov	s8, r9
    2cec: eef86bc4     	vcvt.f64.s32	d22, s8
    2cf0: ee667ba1     	vmul.f64	d23, d22, d17
    2cf4: eef74be7     	vcvt.f32.f64	s9, d23
    2cf8: eaffffd2     	b	0x2c48 <makingWndArray+0x3a4> @ imm = #-0xb8
    2cfc: ee046a90     	vmov	s9, r6
    2d00: eef8fbe4     	vcvt.f64.s32	d31, s9
    2d04: ee6f4ba1     	vmul.f64	d20, d31, d17
    2d08: eef74be4     	vcvt.f32.f64	s9, d20
    2d0c: eaffffb7     	b	0x2bf0 <makingWndArray+0x34c> @ imm = #-0x124
    2d10: ee079a90     	vmov	s15, r9
    2d14: eef8bbe7     	vcvt.f64.s32	d27, s15
    2d18: ee6bcba1     	vmul.f64	d28, d27, d17
    2d1c: eef74bec     	vcvt.f32.f64	s9, d28
    2d20: eaffff9c     	b	0x2b98 <makingWndArray+0x2f4> @ imm = #-0x190
    2d24: ee043a90     	vmov	s9, r3
    2d28: eef87be4     	vcvt.f64.s32	d23, s9
    2d2c: ee678ba1     	vmul.f64	d24, d23, d17
    2d30: eef72be8     	vcvt.f32.f64	s5, d24
    2d34: eaffff81     	b	0x2b40 <makingWndArray+0x29c> @ imm = #-0x1fc

