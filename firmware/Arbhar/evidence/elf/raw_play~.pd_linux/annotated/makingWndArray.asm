00002b9c <makingWndArray>:
    2b9c: eef76a00     	vmov.f32	s13, #1.000000e+00
    2ba0: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x2fc8 <makingWndArray+0x42c>  // u32=0x15450; f32?=1.22081122e-40
    2ba4: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x2fcc <makingWndArray+0x430>  // u32=0xc8; f32?=2.80259693e-43
    2ba8: e08f3003     	add	r3, pc, r3
    2bac: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x2fd0 <makingWndArray+0x434>  // u32=0x3154; f32?=1.7695597e-41
    2bb0: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2bb4: e08f4000     	add	r4, pc, r0
    2bb8: e793c002     	ldr	r12, [r3, r2]
    2bbc: e2845b02     	add	r5, r4, #2048
    2bc0: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x2fa8 <makingWndArray+0x40c>  // f64=0.02
    2bc4: e285000c     	add	r0, r5, #12
    2bc8: e28c6bcb     	add	r6, r12, #207872
    2bcc: e3e0e031     	mvn	lr, #49
    2bd0: e28650bc     	add	r5, r6, #188
    2bd4: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x2fc0 <makingWndArray+0x424>  // f32=200
    2bd8: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x2fb0 <makingWndArray+0x414>  // f64=0.90000000000000002
    2bdc: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x2fb8 <makingWndArray+0x41c>  // f64=0.10000000000000001
    2be0: ee07ea90     	vmov	s15, lr
    2be4: eeb85be7     	vcvt.f64.s32	d5, s15
    2be8: ee255b23     	vmul.f64	d5, d5, d19
    2bec: eeb70bc5     	vcvt.f32.f64	s0, d5
    2bf0: eeb14a40     	vneg.f32	s8, s0
    2bf4: ee705a26     	vadd.f32	s11, s0, s13
    2bf8: ee764ac0     	vsub.f32	s9, s13, s0
    2bfc: eef74ac4     	vcvt.f64.f32	d20, s8
    2c00: eeb50ac0     	vcmpe.f32	s0, #0
    2c04: ee241ba2     	vmul.f64	d1, d20, d18
    2c08: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c0c: ee640aa5     	vmul.f32	s1, s9, s11
    2c10: ee602a06     	vmul.f32	s5, s0, s12
    2c14: 9a00005b     	bls	0x2d88 <makingWndArray+0x1ec> @ imm = #0x16c
    2c18: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x2fd4 <makingWndArray+0x438>  // u32=0x30e0; f32?=1.75330464e-41
    2c1c: e1a0700c     	mov	r7, r12
    2c20: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x2fc4 <makingWndArray+0x428>  // f32=0
    2c24: e3004202     	movw	r4, #0x202
    2c28: e08f2001     	add	r2, pc, r1
    2c2c: e3a08001     	mov	r8, #1
    2c30: e3001203     	movw	r1, #0x203
    2c34: e4923004     	ldr	r3, [r2], #4
    2c38: ece71a01     	vstmia	r7!, {s3}
    2c3c: ea000039     	b	0x2d28 <makingWndArray+0x18c> @ imm = #0xe4
    2c40: e3580f7e     	cmp	r8, #504
    2c44: def03a66     	vmovle.f32	s7, s13
    2c48: ca000048     	bgt	0x2d70 <makingWndArray+0x1d4> @ imm = #0x120
    2c4c: ee018a10     	vmov	s2, r8
    2c50: e2883001     	add	r3, r8, #1
    2c54: e1a06002     	mov	r6, r2
    2c58: ecf64a01     	vldmia	r6!, {s9}
    2c5c: eeb82ac1     	vcvt.f32.s32	s4, s2
    2c60: ee327a22     	vadd.f32	s14, s4, s5
    2c64: eebd3ac7     	vcvt.s32.f32	s6, s14
    2c68: ee138a10     	vmov	r8, s6
    2c6c: ee607a24     	vmul.f32	s15, s0, s9
    2c70: e1580001     	cmp	r8, r1
    2c74: a1a08001     	movge	r8, r1
    2c78: e0802108     	add	r2, r0, r8, lsl #2
    2c7c: ed925a00     	vldr	s10, [r2]
    2c80: ee407a85     	vmla.f32	s15, s1, s10
    2c84: eef47ae6     	vcmpe.f32	s15, s13
    2c88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c8c: 8ef07a66     	vmovhi.f32	s15, s13
    2c90: eef57ac0     	vcmpe.f32	s15, #0
    2c94: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2c98: bef07a61     	vmovlt.f32	s15, s3
    2c9c: e3530009     	cmp	r3, #9
    2ca0: ee274aa3     	vmul.f32	s8, s15, s7
    2ca4: eca74a01     	vstmia	r7!, {s8}
    2ca8: da00002b     	ble	0x2d5c <makingWndArray+0x1c0> @ imm = #0xac
    2cac: e3530f7e     	cmp	r3, #504
    2cb0: c0442003     	subgt	r2, r4, r3
    2cb4: def03a66     	vmovle.f32	s7, s13
    2cb8: ce042a10     	vmovgt	s8, r2
    2cbc: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2cc0: ce600ba1     	vmulgt.f64	d16, d16, d17
    2cc4: cef73be0     	vcvtgt.f32.f64	s7, d16
    2cc8: ee013a10     	vmov	s2, r3
    2ccc: e2838001     	add	r8, r3, #1
    2cd0: e1a02006     	mov	r2, r6
    2cd4: ecf25a01     	vldmia	r2!, {s11}
    2cd8: eeb82ac1     	vcvt.f32.s32	s4, s2
    2cdc: ee724a22     	vadd.f32	s9, s4, s5
    2ce0: eebd7ae4     	vcvt.s32.f32	s14, s9
    2ce4: ee173a10     	vmov	r3, s14
    2ce8: ee203a25     	vmul.f32	s6, s0, s11
    2cec: e1530001     	cmp	r3, r1
    2cf0: a1a03001     	movge	r3, r1
    2cf4: e0803103     	add	r3, r0, r3, lsl #2
    2cf8: edd37a00     	vldr	s15, [r3]
    2cfc: ee003aa7     	vmla.f32	s6, s1, s15
    2d00: eeb43ae6     	vcmpe.f32	s6, s13
    2d04: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2d08: 8eb03a66     	vmovhi.f32	s6, s13
    2d0c: eeb53ac0     	vcmpe.f32	s6, #0
    2d10: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2d14: beb03a61     	vmovlt.f32	s6, s3
    2d18: e1580001     	cmp	r8, r1
    2d1c: ee235a23     	vmul.f32	s10, s6, s7
    2d20: eca75a01     	vstmia	r7!, {s10}
    2d24: 0a000006     	beq	0x2d44 <makingWndArray+0x1a8> @ imm = #0x18
    2d28: e3580009     	cmp	r8, #9
    2d2c: caffffc3     	bgt	0x2c40 <makingWndArray+0xa4> @ imm = #-0xf4
    2d30: ee058a90     	vmov	s11, r8
    2d34: eef8abe5     	vcvt.f64.s32	d26, s11
    2d38: ee6a0ba1     	vmul.f64	d16, d26, d17
    2d3c: eef73be0     	vcvt.f32.f64	s7, d16
    2d40: eaffffc1     	b	0x2c4c <makingWndArray+0xb0> @ imm = #-0xfc
    2d44: e28ccb02     	add	r12, r12, #2048
    2d48: e28ee001     	add	lr, lr, #1
    2d4c: e28cc00c     	add	r12, r12, #12
    2d50: e155000c     	cmp	r5, r12
    2d54: 1affffa1     	bne	0x2be0 <makingWndArray+0x44> @ imm = #-0x17c
    2d58: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    2d5c: ee033a90     	vmov	s7, r3
    2d60: eef8bbe3     	vcvt.f64.s32	d27, s7
    2d64: ee6b0ba1     	vmul.f64	d16, d27, d17
    2d68: eef73be0     	vcvt.f32.f64	s7, d16
    2d6c: eaffffd5     	b	0x2cc8 <makingWndArray+0x12c> @ imm = #-0xac
    2d70: e0446008     	sub	r6, r4, r8
    2d74: ee036a90     	vmov	s7, r6
    2d78: eef89be3     	vcvt.f64.s32	d25, s7
    2d7c: ee690ba1     	vmul.f64	d16, d25, d17
    2d80: eef73be0     	vcvt.f32.f64	s7, d16
    2d84: eaffffb0     	b	0x2c4c <makingWndArray+0xb0> @ imm = #-0x140
    2d88: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x2fd8 <makingWndArray+0x43c>  // u32=0x2f74; f32?=1.70229737e-41
    2d8c: e1a0900c     	mov	r9, r12
    2d90: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x2fdc <makingWndArray+0x440>  // u32=0x4f64; f32?=2.847999e-41
    2d94: e08f8007     	add	r8, pc, r7
    2d98: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x2fc4 <makingWndArray+0x428>  // f32=0
    2d9c: e08f4003     	add	r4, pc, r3
    2da0: e2882b02     	add	r2, r8, #2048
    2da4: e3a03000     	mov	r3, #0
    2da8: e282200c     	add	r2, r2, #12
    2dac: e2446efe     	sub	r6, r4, #4064
    2db0: e3008202     	movw	r8, #0x202
    2db4: e3007203     	movw	r7, #0x203
    2db8: ea000072     	b	0x2f88 <makingWndArray+0x3ec> @ imm = #0x1c8
    2dbc: e3530f7e     	cmp	r3, #504
    2dc0: c0481003     	subgt	r1, r8, r3
    2dc4: def02a66     	vmovle.f32	s5, s13
    2dc8: ce041a10     	vmovgt	s8, r1
    2dcc: cef80bc4     	vcvtgt.f64.s32	d16, s8
    2dd0: ce600ba1     	vmulgt.f64	d16, d16, d17
    2dd4: cef72be0     	vcvtgt.f32.f64	s5, d16
    2dd8: ecb27a01     	vldmia	r2!, {s14}
    2ddc: e1a04006     	mov	r4, r6
    2de0: e2833001     	add	r3, r3, #1
    2de4: e1a01009     	mov	r1, r9
    2de8: ecb43a01     	vldmia	r4!, {s6}
    2dec: ee657a87     	vmul.f32	s15, s11, s14
    2df0: eef75ac3     	vcvt.f64.f32	d21, s6
    2df4: eef76ae7     	vcvt.f64.f32	d22, s15
    2df8: ee456b81     	vmla.f64	d22, d21, d1
    2dfc: eeb75be6     	vcvt.f32.f64	s10, d22
    2e00: eeb55ac0     	vcmpe.f32	s10, #0
    2e04: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2e08: beb05a42     	vmovlt.f32	s10, s4
    2e0c: e3530009     	cmp	r3, #9
    2e10: ee250a22     	vmul.f32	s0, s10, s5
    2e14: eca10a01     	vstmia	r1!, {s0}
    2e18: da00007f     	ble	0x301c <makingWndArray+0x480> @ imm = #0x1fc
    2e1c: e3530f7e     	cmp	r3, #504
    2e20: c0486003     	subgt	r6, r8, r3
    2e24: def02a66     	vmovle.f32	s5, s13
    2e28: ce006a10     	vmovgt	s0, r6
    2e2c: cef86bc0     	vcvtgt.f64.s32	d22, s0
    2e30: ce666ba1     	vmulgt.f64	d22, d22, d17
    2e34: cef72be6     	vcvtgt.f32.f64	s5, d22
    2e38: edd20a00     	vldr	s1, [r2]
    2e3c: e2839001     	add	r9, r3, #1
    2e40: edd44a00     	vldr	s9, [r4]
    2e44: ee254aa0     	vmul.f32	s8, s11, s1
    2e48: eef79ae4     	vcvt.f64.f32	d25, s9
    2e4c: eef7aac4     	vcvt.f64.f32	d26, s8
    2e50: ee49ab81     	vmla.f64	d26, d25, d1
    2e54: eeb77bea     	vcvt.f32.f64	s14, d26
    2e58: eeb57ac0     	vcmpe.f32	s14, #0
    2e5c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2e60: beb07a42     	vmovlt.f32	s14, s4
    2e64: e3590009     	cmp	r9, #9
    2e68: ee273a22     	vmul.f32	s6, s14, s5
    2e6c: ed813a00     	vstr	s6, [r1]
    2e70: da000064     	ble	0x3008 <makingWndArray+0x46c> @ imm = #0x190
    2e74: e3590f7e     	cmp	r9, #504
    2e78: c0489009     	subgt	r9, r8, r9
    2e7c: def04a66     	vmovle.f32	s9, s13
    2e80: ce039a10     	vmovgt	s6, r9
    2e84: cef8abc3     	vcvtgt.f64.s32	d26, s6
    2e88: ce6aaba1     	vmulgt.f64	d26, d26, d17
    2e8c: cef74bea     	vcvtgt.f32.f64	s9, d26
    2e90: ed925a01     	vldr	s10, [r2, #4]
    2e94: e2836002     	add	r6, r3, #2
    2e98: ed940a01     	vldr	s0, [r4, #4]
    2e9c: ee650a85     	vmul.f32	s1, s11, s10
    2ea0: eef7dac0     	vcvt.f64.f32	d29, s0
    2ea4: eef7eae0     	vcvt.f64.f32	d30, s1
    2ea8: ee4deb81     	vmla.f64	d30, d29, d1
    2eac: eeb74bee     	vcvt.f32.f64	s8, d30
    2eb0: eeb54ac0     	vcmpe.f32	s8, #0
    2eb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2eb8: beb04a42     	vmovlt.f32	s8, s4
    2ebc: e3560009     	cmp	r6, #9
    2ec0: ee247a24     	vmul.f32	s14, s8, s9
    2ec4: ed817a01     	vstr	s14, [r1, #4]
    2ec8: da000049     	ble	0x2ff4 <makingWndArray+0x458> @ imm = #0x124
    2ecc: e3560f7e     	cmp	r6, #504
    2ed0: c0486006     	subgt	r6, r8, r6
    2ed4: def04a66     	vmovle.f32	s9, s13
    2ed8: ce076a10     	vmovgt	s14, r6
    2edc: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    2ee0: ce6eeba1     	vmulgt.f64	d30, d30, d17
    2ee4: cef74bee     	vcvtgt.f32.f64	s9, d30
    2ee8: ed923a02     	vldr	s6, [r2, #8]
    2eec: e2839003     	add	r9, r3, #3
    2ef0: edd47a02     	vldr	s15, [r4, #8]
    2ef4: ee255a83     	vmul.f32	s10, s11, s6
    2ef8: eef75ae7     	vcvt.f64.f32	d21, s15
    2efc: eef70ac5     	vcvt.f64.f32	d16, s10
    2f00: ee450b81     	vmla.f64	d16, d21, d1
    2f04: eeb70be0     	vcvt.f32.f64	s0, d16
    2f08: eeb50ac0     	vcmpe.f32	s0, #0
    2f0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2f10: beb00a42     	vmovlt.f32	s0, s4
    2f14: e3590009     	cmp	r9, #9
    2f18: ee600a24     	vmul.f32	s1, s0, s9
    2f1c: edc10a02     	vstr	s1, [r1, #8]
    2f20: da00002e     	ble	0x2fe0 <makingWndArray+0x444> @ imm = #0xb8
    2f24: e3590f7e     	cmp	r9, #504
    2f28: c0489009     	subgt	r9, r8, r9
    2f2c: def04a66     	vmovle.f32	s9, s13
    2f30: ce009a90     	vmovgt	s1, r9
    2f34: cef80be0     	vcvtgt.f64.s32	d16, s1
    2f38: ce600ba1     	vmulgt.f64	d16, d16, d17
    2f3c: cef74be0     	vcvtgt.f32.f64	s9, d16
    2f40: ed927a03     	vldr	s14, [r2, #12]
    2f44: e2833004     	add	r3, r3, #4
    2f48: e2822010     	add	r2, r2, #16
    2f4c: e2846010     	add	r6, r4, #16
    2f50: e2819010     	add	r9, r1, #16
    2f54: ed943a03     	vldr	s6, [r4, #12]
    2f58: ee657a87     	vmul.f32	s15, s11, s14
    2f5c: eef78ac3     	vcvt.f64.f32	d24, s6
    2f60: eef70ae7     	vcvt.f64.f32	d16, s15
    2f64: ee480b81     	vmla.f64	d16, d24, d1
    2f68: eeb75be0     	vcvt.f32.f64	s10, d16
    2f6c: eeb55ac0     	vcmpe.f32	s10, #0
    2f70: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2f74: beb05a42     	vmovlt.f32	s10, s4
    2f78: e1530007     	cmp	r3, r7
    2f7c: ee254a24     	vmul.f32	s8, s10, s9
    2f80: ed814a03     	vstr	s8, [r1, #12]
    2f84: 0affff6e     	beq	0x2d44 <makingWndArray+0x1a8> @ imm = #-0x248
    2f88: e3530009     	cmp	r3, #9
    2f8c: caffff8a     	bgt	0x2dbc <makingWndArray+0x220> @ imm = #-0x1d8
    2f90: ee023a90     	vmov	s5, r3
    2f94: eef80be2     	vcvt.f64.s32	d16, s5
    2f98: ee204ba1     	vmul.f64	d4, d16, d17
    2f9c: eef72bc4     	vcvt.f32.f64	s5, d4
    2fa0: eaffff8c     	b	0x2dd8 <makingWndArray+0x23c> @ imm = #-0x1d0
    2fa4: e320f000     	nop
    2fa8: 7b 14 ae 47  	.word	0x47ae147b
    2fac: e1 7a 94 3f  	.word	0x3f947ae1
    2fb0: cd cc cc cc  	.word	0xcccccccd
    2fb4: cc cc ec 3f  	.word	0x3feccccc
    2fb8: 9a 99 99 99  	.word	0x9999999a
    2fbc: 99 99 b9 3f  	.word	0x3fb99999
    2fc0: 00 00 48 43  	.word	0x43480000
    2fc4: 00 00 00 00  	.word	0x00000000
    2fc8: 50 54 01 00  	.word	0x00015450
    2fcc: c8 00 00 00  	.word	0x000000c8
    2fd0: 54 31 00 00  	.word	0x00003154
    2fd4: e0 30 00 00  	.word	0x000030e0
    2fd8: 74 2f 00 00  	.word	0x00002f74
    2fdc: 64 4f 00 00  	.word	0x00004f64
    2fe0: ee049a10     	vmov	s8, r9
    2fe4: eef86bc4     	vcvt.f64.s32	d22, s8
    2fe8: ee667ba1     	vmul.f64	d23, d22, d17
    2fec: eef74be7     	vcvt.f32.f64	s9, d23
    2ff0: eaffffd2     	b	0x2f40 <makingWndArray+0x3a4> @ imm = #-0xb8
    2ff4: ee046a90     	vmov	s9, r6
    2ff8: eef8fbe4     	vcvt.f64.s32	d31, s9
    2ffc: ee6f4ba1     	vmul.f64	d20, d31, d17
    3000: eef74be4     	vcvt.f32.f64	s9, d20
    3004: eaffffb7     	b	0x2ee8 <makingWndArray+0x34c> @ imm = #-0x124
    3008: ee079a90     	vmov	s15, r9
    300c: eef8bbe7     	vcvt.f64.s32	d27, s15
    3010: ee6bcba1     	vmul.f64	d28, d27, d17
    3014: eef74bec     	vcvt.f32.f64	s9, d28
    3018: eaffff9c     	b	0x2e90 <makingWndArray+0x2f4> @ imm = #-0x190
    301c: ee043a90     	vmov	s9, r3
    3020: eef87be4     	vcvt.f64.s32	d23, s9
    3024: ee678ba1     	vmul.f64	d24, d23, d17
    3028: eef72be8     	vcvt.f32.f64	s5, d24
    302c: eaffff81     	b	0x2e38 <makingWndArray+0x29c> @ imm = #-0x1fc

