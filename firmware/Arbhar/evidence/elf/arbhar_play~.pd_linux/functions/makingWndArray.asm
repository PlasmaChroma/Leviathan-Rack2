00005b34 <makingWndArray>:
    5b34: eef76a00     	vmov.f32	s13, #1.000000e+00
    5b38: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x5f60 <makingWndArray+0x42c>
    5b3c: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x5f64 <makingWndArray+0x430>
    5b40: e08f3003     	add	r3, pc, r3
    5b44: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x5f68 <makingWndArray+0x434>
    5b48: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    5b4c: e08f4000     	add	r4, pc, r0
    5b50: e793c002     	ldr	r12, [r3, r2]
    5b54: e2845d31     	add	r5, r4, #3136
    5b58: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x5f40 <makingWndArray+0x40c>
    5b5c: e285000c     	add	r0, r5, #12
    5b60: e28c6bcb     	add	r6, r12, #207872
    5b64: e3e0e031     	mvn	lr, #49
    5b68: e28650bc     	add	r5, r6, #188
    5b6c: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x5f58 <makingWndArray+0x424>
    5b70: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x5f48 <makingWndArray+0x414>
    5b74: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x5f50 <makingWndArray+0x41c>
    5b78: ee07ea90     	vmov	s15, lr
    5b7c: eeb85be7     	vcvt.f64.s32	d5, s15
    5b80: ee255b23     	vmul.f64	d5, d5, d19
    5b84: eeb70bc5     	vcvt.f32.f64	s0, d5
    5b88: eeb14a40     	vneg.f32	s8, s0
    5b8c: ee705a26     	vadd.f32	s11, s0, s13
    5b90: ee764ac0     	vsub.f32	s9, s13, s0
    5b94: eef74ac4     	vcvt.f64.f32	d20, s8
    5b98: eeb50ac0     	vcmpe.f32	s0, #0
    5b9c: ee241ba2     	vmul.f64	d1, d20, d18
    5ba0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5ba4: ee640aa5     	vmul.f32	s1, s9, s11
    5ba8: ee602a06     	vmul.f32	s5, s0, s12
    5bac: 9a00005c     	bls	0x5d24 <makingWndArray+0x1f0> @ imm = #0x170
    5bb0: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x5f6c <makingWndArray+0x438>
    5bb4: e1a0700c     	mov	r7, r12
    5bb8: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x5f5c <makingWndArray+0x428>
    5bbc: e3004202     	movw	r4, #0x202
    5bc0: e08f8001     	add	r8, pc, r1
    5bc4: e3001203     	movw	r1, #0x203
    5bc8: e2882d11     	add	r2, r8, #1088
    5bcc: e3a08001     	mov	r8, #1
    5bd0: e4923004     	ldr	r3, [r2], #4
    5bd4: ece71a01     	vstmia	r7!, {s3}
    5bd8: ea000039     	b	0x5cc4 <makingWndArray+0x190> @ imm = #0xe4
    5bdc: e3580f7e     	cmp	r8, #504
    5be0: def03a66     	vmovle.f32	s7, s13
    5be4: ca000048     	bgt	0x5d0c <makingWndArray+0x1d8> @ imm = #0x120
    5be8: ee018a10     	vmov	s2, r8
    5bec: e1a06002     	mov	r6, r2
    5bf0: ecf64a01     	vldmia	r6!, {s9}
    5bf4: e2883001     	add	r3, r8, #1
    5bf8: eeb82ac1     	vcvt.f32.s32	s4, s2
    5bfc: ee327a22     	vadd.f32	s14, s4, s5
    5c00: eebd3ac7     	vcvt.s32.f32	s6, s14
    5c04: ee132a10     	vmov	r2, s6
    5c08: ee607a24     	vmul.f32	s15, s0, s9
    5c0c: e1520001     	cmp	r2, r1
    5c10: a1a02001     	movge	r2, r1
    5c14: e0808102     	add	r8, r0, r2, lsl #2
    5c18: ed985a00     	vldr	s10, [r8]
    5c1c: ee407a85     	vmla.f32	s15, s1, s10
    5c20: eef47ae6     	vcmpe.f32	s15, s13
    5c24: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5c28: 8ef07a66     	vmovhi.f32	s15, s13
    5c2c: eef57ac0     	vcmpe.f32	s15, #0
    5c30: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5c34: bef07a61     	vmovlt.f32	s15, s3
    5c38: e3530009     	cmp	r3, #9
    5c3c: ee274aa3     	vmul.f32	s8, s15, s7
    5c40: eca74a01     	vstmia	r7!, {s8}
    5c44: da00002b     	ble	0x5cf8 <makingWndArray+0x1c4> @ imm = #0xac
    5c48: e3530f7e     	cmp	r3, #504
    5c4c: c0448003     	subgt	r8, r4, r3
    5c50: def03a66     	vmovle.f32	s7, s13
    5c54: ce048a10     	vmovgt	s8, r8
    5c58: cef80bc4     	vcvtgt.f64.s32	d16, s8
    5c5c: ce600ba1     	vmulgt.f64	d16, d16, d17
    5c60: cef73be0     	vcvtgt.f32.f64	s7, d16
    5c64: ee013a10     	vmov	s2, r3
    5c68: e2838001     	add	r8, r3, #1
    5c6c: e1a02006     	mov	r2, r6
    5c70: ecf25a01     	vldmia	r2!, {s11}
    5c74: eeb82ac1     	vcvt.f32.s32	s4, s2
    5c78: ee724a22     	vadd.f32	s9, s4, s5
    5c7c: eebd7ae4     	vcvt.s32.f32	s14, s9
    5c80: ee173a10     	vmov	r3, s14
    5c84: ee203a25     	vmul.f32	s6, s0, s11
    5c88: e1530001     	cmp	r3, r1
    5c8c: a1a03001     	movge	r3, r1
    5c90: e0803103     	add	r3, r0, r3, lsl #2
    5c94: edd37a00     	vldr	s15, [r3]
    5c98: ee003aa7     	vmla.f32	s6, s1, s15
    5c9c: eeb43ae6     	vcmpe.f32	s6, s13
    5ca0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5ca4: 8eb03a66     	vmovhi.f32	s6, s13
    5ca8: eeb53ac0     	vcmpe.f32	s6, #0
    5cac: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5cb0: beb03a61     	vmovlt.f32	s6, s3
    5cb4: e1580001     	cmp	r8, r1
    5cb8: ee235a23     	vmul.f32	s10, s6, s7
    5cbc: eca75a01     	vstmia	r7!, {s10}
    5cc0: 0a000006     	beq	0x5ce0 <makingWndArray+0x1ac> @ imm = #0x18
    5cc4: e3580009     	cmp	r8, #9
    5cc8: caffffc3     	bgt	0x5bdc <makingWndArray+0xa8> @ imm = #-0xf4
    5ccc: ee058a90     	vmov	s11, r8
    5cd0: eef8abe5     	vcvt.f64.s32	d26, s11
    5cd4: ee6a0ba1     	vmul.f64	d16, d26, d17
    5cd8: eef73be0     	vcvt.f32.f64	s7, d16
    5cdc: eaffffc1     	b	0x5be8 <makingWndArray+0xb4> @ imm = #-0xfc
    5ce0: e28ccb02     	add	r12, r12, #2048
    5ce4: e28ee001     	add	lr, lr, #1
    5ce8: e28cc00c     	add	r12, r12, #12
    5cec: e155000c     	cmp	r5, r12
    5cf0: 1affffa0     	bne	0x5b78 <makingWndArray+0x44> @ imm = #-0x180
    5cf4: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    5cf8: ee033a90     	vmov	s7, r3
    5cfc: eef8bbe3     	vcvt.f64.s32	d27, s7
    5d00: ee6b0ba1     	vmul.f64	d16, d27, d17
    5d04: eef73be0     	vcvt.f32.f64	s7, d16
    5d08: eaffffd5     	b	0x5c64 <makingWndArray+0x130> @ imm = #-0xac
    5d0c: e0446008     	sub	r6, r4, r8
    5d10: ee036a90     	vmov	s7, r6
    5d14: eef89be3     	vcvt.f64.s32	d25, s7
    5d18: ee690ba1     	vmul.f64	d16, d25, d17
    5d1c: eef73be0     	vcvt.f32.f64	s7, d16
    5d20: eaffffb0     	b	0x5be8 <makingWndArray+0xb4> @ imm = #-0x140
    5d24: e59f7244     	ldr	r7, [pc, #0x244]        @ 0x5f70 <makingWndArray+0x43c>
    5d28: e1a0900c     	mov	r9, r12
    5d2c: e59f3240     	ldr	r3, [pc, #0x240]        @ 0x5f74 <makingWndArray+0x440>
    5d30: e08f8007     	add	r8, pc, r7
    5d34: ed9f2a88     	vldr	s4, [pc, #544]          @ 0x5f5c <makingWndArray+0x428>
    5d38: e08f4003     	add	r4, pc, r3
    5d3c: e2882d31     	add	r2, r8, #3136
    5d40: e3a03000     	mov	r3, #0
    5d44: e282200c     	add	r2, r2, #12
    5d48: e2446eba     	sub	r6, r4, #2976
    5d4c: e3008202     	movw	r8, #0x202
    5d50: e3007203     	movw	r7, #0x203
    5d54: ea000072     	b	0x5f24 <makingWndArray+0x3f0> @ imm = #0x1c8
    5d58: e3530f7e     	cmp	r3, #504
    5d5c: c0481003     	subgt	r1, r8, r3
    5d60: def02a66     	vmovle.f32	s5, s13
    5d64: ce041a10     	vmovgt	s8, r1
    5d68: cef80bc4     	vcvtgt.f64.s32	d16, s8
    5d6c: ce600ba1     	vmulgt.f64	d16, d16, d17
    5d70: cef72be0     	vcvtgt.f32.f64	s5, d16
    5d74: ecb27a01     	vldmia	r2!, {s14}
    5d78: e1a04006     	mov	r4, r6
    5d7c: e2833001     	add	r3, r3, #1
    5d80: e1a01009     	mov	r1, r9
    5d84: ecb43a01     	vldmia	r4!, {s6}
    5d88: ee657a87     	vmul.f32	s15, s11, s14
    5d8c: eef75ac3     	vcvt.f64.f32	d21, s6
    5d90: eef76ae7     	vcvt.f64.f32	d22, s15
    5d94: ee456b81     	vmla.f64	d22, d21, d1
    5d98: eeb75be6     	vcvt.f32.f64	s10, d22
    5d9c: eeb55ac0     	vcmpe.f32	s10, #0
    5da0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5da4: beb05a42     	vmovlt.f32	s10, s4
    5da8: e3530009     	cmp	r3, #9
    5dac: ee250a22     	vmul.f32	s0, s10, s5
    5db0: eca10a01     	vstmia	r1!, {s0}
    5db4: da00007e     	ble	0x5fb4 <makingWndArray+0x480> @ imm = #0x1f8
    5db8: e3530f7e     	cmp	r3, #504
    5dbc: c0486003     	subgt	r6, r8, r3
    5dc0: def02a66     	vmovle.f32	s5, s13
    5dc4: ce006a10     	vmovgt	s0, r6
    5dc8: cef86bc0     	vcvtgt.f64.s32	d22, s0
    5dcc: ce666ba1     	vmulgt.f64	d22, d22, d17
    5dd0: cef72be6     	vcvtgt.f32.f64	s5, d22
    5dd4: edd20a00     	vldr	s1, [r2]
    5dd8: e2839001     	add	r9, r3, #1
    5ddc: edd44a00     	vldr	s9, [r4]
    5de0: ee254aa0     	vmul.f32	s8, s11, s1
    5de4: eef79ae4     	vcvt.f64.f32	d25, s9
    5de8: eef7aac4     	vcvt.f64.f32	d26, s8
    5dec: ee49ab81     	vmla.f64	d26, d25, d1
    5df0: eeb77bea     	vcvt.f32.f64	s14, d26
    5df4: eeb57ac0     	vcmpe.f32	s14, #0
    5df8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5dfc: beb07a42     	vmovlt.f32	s14, s4
    5e00: e3590009     	cmp	r9, #9
    5e04: ee273a22     	vmul.f32	s6, s14, s5
    5e08: ed813a00     	vstr	s6, [r1]
    5e0c: da000063     	ble	0x5fa0 <makingWndArray+0x46c> @ imm = #0x18c
    5e10: e3590f7e     	cmp	r9, #504
    5e14: c0489009     	subgt	r9, r8, r9
    5e18: def04a66     	vmovle.f32	s9, s13
    5e1c: ce039a10     	vmovgt	s6, r9
    5e20: cef8abc3     	vcvtgt.f64.s32	d26, s6
    5e24: ce6aaba1     	vmulgt.f64	d26, d26, d17
    5e28: cef74bea     	vcvtgt.f32.f64	s9, d26
    5e2c: ed925a01     	vldr	s10, [r2, #4]
    5e30: e2836002     	add	r6, r3, #2
    5e34: ed940a01     	vldr	s0, [r4, #4]
    5e38: ee650a85     	vmul.f32	s1, s11, s10
    5e3c: eef7dac0     	vcvt.f64.f32	d29, s0
    5e40: eef7eae0     	vcvt.f64.f32	d30, s1
    5e44: ee4deb81     	vmla.f64	d30, d29, d1
    5e48: eeb74bee     	vcvt.f32.f64	s8, d30
    5e4c: eeb54ac0     	vcmpe.f32	s8, #0
    5e50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5e54: beb04a42     	vmovlt.f32	s8, s4
    5e58: e3560009     	cmp	r6, #9
    5e5c: ee247a24     	vmul.f32	s14, s8, s9
    5e60: ed817a01     	vstr	s14, [r1, #4]
    5e64: da000048     	ble	0x5f8c <makingWndArray+0x458> @ imm = #0x120
    5e68: e3560f7e     	cmp	r6, #504
    5e6c: c0486006     	subgt	r6, r8, r6
    5e70: def04a66     	vmovle.f32	s9, s13
    5e74: ce076a10     	vmovgt	s14, r6
    5e78: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    5e7c: ce6eeba1     	vmulgt.f64	d30, d30, d17
    5e80: cef74bee     	vcvtgt.f32.f64	s9, d30
    5e84: ed923a02     	vldr	s6, [r2, #8]
    5e88: e2839003     	add	r9, r3, #3
    5e8c: edd47a02     	vldr	s15, [r4, #8]
    5e90: ee255a83     	vmul.f32	s10, s11, s6
    5e94: eef75ae7     	vcvt.f64.f32	d21, s15
    5e98: eef70ac5     	vcvt.f64.f32	d16, s10
    5e9c: ee450b81     	vmla.f64	d16, d21, d1
    5ea0: eeb70be0     	vcvt.f32.f64	s0, d16
    5ea4: eeb50ac0     	vcmpe.f32	s0, #0
    5ea8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5eac: beb00a42     	vmovlt.f32	s0, s4
    5eb0: e3590009     	cmp	r9, #9
    5eb4: ee600a24     	vmul.f32	s1, s0, s9
    5eb8: edc10a02     	vstr	s1, [r1, #8]
    5ebc: da00002d     	ble	0x5f78 <makingWndArray+0x444> @ imm = #0xb4
    5ec0: e3590f7e     	cmp	r9, #504
    5ec4: c0489009     	subgt	r9, r8, r9
    5ec8: def04a66     	vmovle.f32	s9, s13
    5ecc: ce009a90     	vmovgt	s1, r9
    5ed0: cef80be0     	vcvtgt.f64.s32	d16, s1
    5ed4: ce600ba1     	vmulgt.f64	d16, d16, d17
    5ed8: cef74be0     	vcvtgt.f32.f64	s9, d16
    5edc: ed927a03     	vldr	s14, [r2, #12]
    5ee0: e2833004     	add	r3, r3, #4
    5ee4: e2822010     	add	r2, r2, #16
    5ee8: e2846010     	add	r6, r4, #16
    5eec: e2819010     	add	r9, r1, #16
    5ef0: ed943a03     	vldr	s6, [r4, #12]
    5ef4: ee657a87     	vmul.f32	s15, s11, s14
    5ef8: eef78ac3     	vcvt.f64.f32	d24, s6
    5efc: eef70ae7     	vcvt.f64.f32	d16, s15
    5f00: ee480b81     	vmla.f64	d16, d24, d1
    5f04: eeb75be0     	vcvt.f32.f64	s10, d16
    5f08: eeb55ac0     	vcmpe.f32	s10, #0
    5f0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5f10: beb05a42     	vmovlt.f32	s10, s4
    5f14: e1530007     	cmp	r3, r7
    5f18: ee254a24     	vmul.f32	s8, s10, s9
    5f1c: ed814a03     	vstr	s8, [r1, #12]
    5f20: 0affff6e     	beq	0x5ce0 <makingWndArray+0x1ac> @ imm = #-0x248
    5f24: e3530009     	cmp	r3, #9
    5f28: caffff8a     	bgt	0x5d58 <makingWndArray+0x224> @ imm = #-0x1d8
    5f2c: ee023a90     	vmov	s5, r3
    5f30: eef80be2     	vcvt.f64.s32	d16, s5
    5f34: ee204ba1     	vmul.f64	d4, d16, d17
    5f38: eef72bc4     	vcvt.f32.f64	s5, d4
    5f3c: eaffff8c     	b	0x5d74 <makingWndArray+0x240> @ imm = #-0x1d0
    5f40: 7b 14 ae 47  	.word	0x47ae147b
    5f44: e1 7a 94 3f  	.word	0x3f947ae1
    5f48: cd cc cc cc  	.word	0xcccccccd
    5f4c: cc cc ec 3f  	.word	0x3feccccc
    5f50: 9a 99 99 99  	.word	0x9999999a
    5f54: 99 99 b9 3f  	.word	0x3fb99999
    5f58: 00 00 48 43  	.word	0x43480000
    5f5c: 00 00 00 00  	.word	0x00000000
    5f60: b8 84 01 00  	.word	0x000184b8
    5f64: 38 01 00 00  	.word	0x00000138
    5f68: 60 52 00 00  	.word	0x00005260
    5f6c: ec 51 00 00  	.word	0x000051ec
    5f70: 7c 50 00 00  	.word	0x0000507c
    5f74: 6c 70 00 00  	.word	0x0000706c
    5f78: ee049a10     	vmov	s8, r9
    5f7c: eef86bc4     	vcvt.f64.s32	d22, s8
    5f80: ee667ba1     	vmul.f64	d23, d22, d17
    5f84: eef74be7     	vcvt.f32.f64	s9, d23
    5f88: eaffffd3     	b	0x5edc <makingWndArray+0x3a8> @ imm = #-0xb4
    5f8c: ee046a90     	vmov	s9, r6
    5f90: eef8fbe4     	vcvt.f64.s32	d31, s9
    5f94: ee6f4ba1     	vmul.f64	d20, d31, d17
    5f98: eef74be4     	vcvt.f32.f64	s9, d20
    5f9c: eaffffb8     	b	0x5e84 <makingWndArray+0x350> @ imm = #-0x120
    5fa0: ee079a90     	vmov	s15, r9
    5fa4: eef8bbe7     	vcvt.f64.s32	d27, s15
    5fa8: ee6bcba1     	vmul.f64	d28, d27, d17
    5fac: eef74bec     	vcvt.f32.f64	s9, d28
    5fb0: eaffff9d     	b	0x5e2c <makingWndArray+0x2f8> @ imm = #-0x18c
    5fb4: ee043a90     	vmov	s9, r3
    5fb8: eef87be4     	vcvt.f64.s32	d23, s9
    5fbc: ee678ba1     	vmul.f64	d24, d23, d17
    5fc0: eef72be8     	vcvt.f32.f64	s5, d24
    5fc4: eaffff82     	b	0x5dd4 <makingWndArray+0x2a0> @ imm = #-0x1f8

