00002970 <comport_tick>:
    2970: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2974: ed2d8b02     	vpush	{d8}
    2978: e5907020     	ldr	r7, [r0, #0x20]
    297c: e2804a01     	add	r4, r0, #4096
    2980: e59f5460     	ldr	r5, [pc, #0x460]        @ 0x2de8 <comport_tick+0x478>
    2984: e3a06000     	mov	r6, #0
    2988: e3770001     	cmn	r7, #1
    298c: e24dd0b4     	sub	sp, sp, #180
    2990: e58460c8     	str	r6, [r4, #0xc8]
    2994: e08f5005     	add	r5, pc, r5
    2998: 0a0000fc     	beq	0x2d90 <comport_tick+0x420> @ imm = #0x3f0
    299c: e28d8030     	add	r8, sp, #48
    29a0: e1a01006     	mov	r1, r6
    29a4: e58d0014     	str	r0, [sp, #0x14]
    29a8: e3a02080     	mov	r2, #128
    29ac: e1a00008     	mov	r0, r8
    29b0: e58d6024     	str	r6, [sp, #0x24]
    29b4: ebfff85d     	bl	0xb30 <.plt+0x128>      @ imm = #-0x1e8c
    29b8: e1570006     	cmp	r7, r6
    29bc: e287301f     	add	r3, r7, #31
    29c0: a1a03007     	movge	r3, r7
    29c4: e1a02fc7     	asr	r2, r7, #31
    29c8: e1a002c3     	asr	r0, r3, #5
    29cc: e1a09da2     	lsr	r9, r2, #27
    29d0: e28d10b0     	add	r1, sp, #176
    29d4: e081a100     	add	r10, r1, r0, lsl #2
    29d8: e087b009     	add	r11, r7, r9
    29dc: e20bc01f     	and	r12, r11, #31
    29e0: e51a3080     	ldr	r3, [r10, #-0x80]
    29e4: e04c2009     	sub	r2, r12, r9
    29e8: e3a00001     	mov	r0, #1
    29ec: e59f13f8     	ldr	r1, [pc, #0x3f8]        @ 0x2dec <comport_tick+0x47c>
    29f0: e1839210     	orr	r9, r3, r0, lsl r2
    29f4: ed9f8afa     	vldr	s16, [pc, #1000]        @ 0x2de4 <comport_tick+0x474>
    29f8: e50a9080     	str	r9, [r10, #-0x80]
    29fc: e59f33ec     	ldr	r3, [pc, #0x3ec]        @ 0x2df0 <comport_tick+0x480>
    2a00: e59fa3ec     	ldr	r10, [pc, #0x3ec]       @ 0x2df4 <comport_tick+0x484>
    2a04: e58d6010     	str	r6, [sp, #0x10]
    2a08: e1a09006     	mov	r9, r6
    2a0c: e795b001     	ldr	r11, [r5, r1]
    2a10: e0876000     	add	r6, r7, r0
    2a14: e28d5024     	add	r5, sp, #36
    2a18: e08fc00a     	add	r12, pc, r10
    2a1c: e08f2003     	add	r2, pc, r3
    2a20: e58d6008     	str	r6, [sp, #0x8]
    2a24: e58d500c     	str	r5, [sp, #0xc]
    2a28: e58dc018     	str	r12, [sp, #0x18]
    2a2c: e58d201c     	str	r2, [sp, #0x1c]
    2a30: e58db000     	str	r11, [sp]
    2a34: e1a03009     	mov	r3, r9
    2a38: e3a02000     	mov	r2, #0
    2a3c: e1a01008     	mov	r1, r8
    2a40: e59d0008     	ldr	r0, [sp, #0x8]
    2a44: ebfff806     	bl	0xa64 <.plt+0x5c>       @ imm = #-0x1fe8
    2a48: e3500000     	cmp	r0, #0
    2a4c: da0000c3     	ble	0x2d60 <comport_tick+0x3f0> @ imm = #0x30c
    2a50: e59d200c     	ldr	r2, [sp, #0xc]
    2a54: e59f139c     	ldr	r1, [pc, #0x39c]        @ 0x2df8 <comport_tick+0x488>
    2a58: e1a00007     	mov	r0, r7
    2a5c: ebfff80c     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x1fd0
    2a60: e59400f4     	ldr	r0, [r4, #0xf4]
    2a64: e59d2024     	ldr	r2, [sp, #0x24]
    2a68: e59410ec     	ldr	r1, [r4, #0xec]
    2a6c: e1500002     	cmp	r0, r2
    2a70: b1a02000     	movlt	r2, r0
    2a74: b58d0024     	strlt	r0, [sp, #0x24]
    2a78: e1a00007     	mov	r0, r7
    2a7c: ebfff7ef     	bl	0xa40 <.plt+0x38>       @ imm = #-0x2044
    2a80: e2505000     	subs	r5, r0, #0
    2a84: da000086     	ble	0x2ca4 <comport_tick+0x334> @ imm = #0x218
    2a88: e594a0ec     	ldr	r10, [r4, #0xec]
    2a8c: e59400e4     	ldr	r0, [r4, #0xe4]
    2a90: e2456001     	sub	r6, r5, #1
    2a94: e5dae000     	ldrb	lr, [r10]
    2a98: e3a0a001     	mov	r10, #1
    2a9c: e2066007     	and	r6, r6, #7
    2aa0: ee00ea90     	vmov	s1, lr
    2aa4: eeb80a60     	vcvt.f32.u32	s0, s1
    2aa8: ebfff832     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1f38
    2aac: e155000a     	cmp	r5, r10
    2ab0: 0affffde     	beq	0x2a30 <comport_tick+0xc0> @ imm = #-0x88
    2ab4: e3560000     	cmp	r6, #0
    2ab8: 0a00003e     	beq	0x2bb8 <comport_tick+0x248> @ imm = #0xf8
    2abc: e3560001     	cmp	r6, #1
    2ac0: 0a000033     	beq	0x2b94 <comport_tick+0x224> @ imm = #0xcc
    2ac4: e3560002     	cmp	r6, #2
    2ac8: 0a00002a     	beq	0x2b78 <comport_tick+0x208> @ imm = #0xa8
    2acc: e3560003     	cmp	r6, #3
    2ad0: 0a000021     	beq	0x2b5c <comport_tick+0x1ec> @ imm = #0x84
    2ad4: e3560004     	cmp	r6, #4
    2ad8: 0a000018     	beq	0x2b40 <comport_tick+0x1d0> @ imm = #0x60
    2adc: e3560005     	cmp	r6, #5
    2ae0: 0a00000f     	beq	0x2b24 <comport_tick+0x1b4> @ imm = #0x3c
    2ae4: e3560006     	cmp	r6, #6
    2ae8: 0a000006     	beq	0x2b08 <comport_tick+0x198> @ imm = #0x18
    2aec: e594c0ec     	ldr	r12, [r4, #0xec]
    2af0: e59400e4     	ldr	r0, [r4, #0xe4]
    2af4: e3a0a002     	mov	r10, #2
    2af8: e5dc3001     	ldrb	r3, [r12, #0x1]
    2afc: ee013a10     	vmov	s2, r3
    2b00: eeb80a41     	vcvt.f32.u32	s0, s2
    2b04: ebfff81b     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1f94
    2b08: e59420ec     	ldr	r2, [r4, #0xec]
    2b0c: e59400e4     	ldr	r0, [r4, #0xe4]
    2b10: e7d2100a     	ldrb	r1, [r2, r10]
    2b14: e28aa001     	add	r10, r10, #1
    2b18: ee011a90     	vmov	s3, r1
    2b1c: eeb80a61     	vcvt.f32.u32	s0, s3
    2b20: ebfff814     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1fb0
    2b24: e59460ec     	ldr	r6, [r4, #0xec]
    2b28: e59400e4     	ldr	r0, [r4, #0xe4]
    2b2c: e7d6e00a     	ldrb	lr, [r6, r10]
    2b30: e28aa001     	add	r10, r10, #1
    2b34: ee02ea10     	vmov	s4, lr
    2b38: eeb80a42     	vcvt.f32.u32	s0, s4
    2b3c: ebfff80d     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1fcc
    2b40: e594c0ec     	ldr	r12, [r4, #0xec]
    2b44: e59400e4     	ldr	r0, [r4, #0xe4]
    2b48: e7dc300a     	ldrb	r3, [r12, r10]
    2b4c: e28aa001     	add	r10, r10, #1
    2b50: ee023a90     	vmov	s5, r3
    2b54: eeb80a62     	vcvt.f32.u32	s0, s5
    2b58: ebfff806     	bl	0xb78 <.plt+0x170>      @ imm = #-0x1fe8
    2b5c: e59420ec     	ldr	r2, [r4, #0xec]
    2b60: e59400e4     	ldr	r0, [r4, #0xe4]
    2b64: e7d2100a     	ldrb	r1, [r2, r10]
    2b68: e28aa001     	add	r10, r10, #1
    2b6c: ee031a10     	vmov	s6, r1
    2b70: eeb80a43     	vcvt.f32.u32	s0, s6
    2b74: ebfff7ff     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2004
    2b78: e59460ec     	ldr	r6, [r4, #0xec]
    2b7c: e59400e4     	ldr	r0, [r4, #0xe4]
    2b80: e7d6e00a     	ldrb	lr, [r6, r10]
    2b84: e28aa001     	add	r10, r10, #1
    2b88: ee03ea90     	vmov	s7, lr
    2b8c: eeb80a63     	vcvt.f32.u32	s0, s7
    2b90: ebfff7f8     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2020
    2b94: e594c0ec     	ldr	r12, [r4, #0xec]
    2b98: e59400e4     	ldr	r0, [r4, #0xe4]
    2b9c: e7dc300a     	ldrb	r3, [r12, r10]
    2ba0: e28aa001     	add	r10, r10, #1
    2ba4: ee043a10     	vmov	s8, r3
    2ba8: eeb80a44     	vcvt.f32.u32	s0, s8
    2bac: ebfff7f1     	bl	0xb78 <.plt+0x170>      @ imm = #-0x203c
    2bb0: e155000a     	cmp	r5, r10
    2bb4: 0affff9d     	beq	0x2a30 <comport_tick+0xc0> @ imm = #-0x18c
    2bb8: e59420ec     	ldr	r2, [r4, #0xec]
    2bbc: e59400e4     	ldr	r0, [r4, #0xe4]
    2bc0: e28a6001     	add	r6, r10, #1
    2bc4: e7d2100a     	ldrb	r1, [r2, r10]
    2bc8: ee041a90     	vmov	s9, r1
    2bcc: eeb80a64     	vcvt.f32.u32	s0, s9
    2bd0: ebfff7e8     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2060
    2bd4: e594c0ec     	ldr	r12, [r4, #0xec]
    2bd8: e59400e4     	ldr	r0, [r4, #0xe4]
    2bdc: e7dc3006     	ldrb	r3, [r12, r6]
    2be0: e28a6002     	add	r6, r10, #2
    2be4: ee053a10     	vmov	s10, r3
    2be8: eeb80a45     	vcvt.f32.u32	s0, s10
    2bec: ebfff7e1     	bl	0xb78 <.plt+0x170>      @ imm = #-0x207c
    2bf0: e59420ec     	ldr	r2, [r4, #0xec]
    2bf4: e59400e4     	ldr	r0, [r4, #0xe4]
    2bf8: e7d21006     	ldrb	r1, [r2, r6]
    2bfc: ee051a90     	vmov	s11, r1
    2c00: eeb80a65     	vcvt.f32.u32	s0, s11
    2c04: ebfff7db     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2094
    2c08: e594c0ec     	ldr	r12, [r4, #0xec]
    2c0c: e28a3003     	add	r3, r10, #3
    2c10: e59400e4     	ldr	r0, [r4, #0xe4]
    2c14: e7dc2003     	ldrb	r2, [r12, r3]
    2c18: ee062a10     	vmov	s12, r2
    2c1c: eeb80a46     	vcvt.f32.u32	s0, s12
    2c20: ebfff7d4     	bl	0xb78 <.plt+0x170>      @ imm = #-0x20b0
    2c24: e59460ec     	ldr	r6, [r4, #0xec]
    2c28: e28a1004     	add	r1, r10, #4
    2c2c: e59400e4     	ldr	r0, [r4, #0xe4]
    2c30: e7d6c001     	ldrb	r12, [r6, r1]
    2c34: ee06ca90     	vmov	s13, r12
    2c38: eeb80a66     	vcvt.f32.u32	s0, s13
    2c3c: ebfff7cd     	bl	0xb78 <.plt+0x170>      @ imm = #-0x20cc
    2c40: e59420ec     	ldr	r2, [r4, #0xec]
    2c44: e28a3005     	add	r3, r10, #5
    2c48: e59400e4     	ldr	r0, [r4, #0xe4]
    2c4c: e7d26003     	ldrb	r6, [r2, r3]
    2c50: ee076a10     	vmov	s14, r6
    2c54: eeb80a47     	vcvt.f32.u32	s0, s14
    2c58: ebfff7c6     	bl	0xb78 <.plt+0x170>      @ imm = #-0x20e8
    2c5c: e59410ec     	ldr	r1, [r4, #0xec]
    2c60: e28ac006     	add	r12, r10, #6
    2c64: e59400e4     	ldr	r0, [r4, #0xe4]
    2c68: e7d1200c     	ldrb	r2, [r1, r12]
    2c6c: ee082a90     	vmov	s17, r2
    2c70: eeb80a68     	vcvt.f32.u32	s0, s17
    2c74: ebfff7bf     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2104
    2c78: e59400ec     	ldr	r0, [r4, #0xec]
    2c7c: e28a3007     	add	r3, r10, #7
    2c80: e28aa008     	add	r10, r10, #8
    2c84: e7d06003     	ldrb	r6, [r0, r3]
    2c88: e59400e4     	ldr	r0, [r4, #0xe4]
    2c8c: ee076a90     	vmov	s15, r6
    2c90: eeb80a67     	vcvt.f32.u32	s0, s15
    2c94: ebfff7b7     	bl	0xb78 <.plt+0x170>      @ imm = #-0x2124
    2c98: e155000a     	cmp	r5, r10
    2c9c: 1affffc5     	bne	0x2bb8 <comport_tick+0x248> @ imm = #-0xec
    2ca0: eaffff62     	b	0x2a30 <comport_tick+0xc0> @ imm = #-0x278
    2ca4: 1a000029     	bne	0x2d50 <comport_tick+0x3e0> @ imm = #0xa4
    2ca8: e59d2024     	ldr	r2, [sp, #0x24]
    2cac: e3520000     	cmp	r2, #0
    2cb0: 1a000026     	bne	0x2d50 <comport_tick+0x3e0> @ imm = #0x98
    2cb4: e594e0d0     	ldr	lr, [r4, #0xd0]
    2cb8: e59460cc     	ldr	r6, [r4, #0xcc]
    2cbc: e15e0006     	cmp	lr, r6
    2cc0: ba00005d     	blt	0x2e3c <comport_tick+0x4cc> @ imm = #0x174
    2cc4: e59da014     	ldr	r10, [sp, #0x14]
    2cc8: e1d42af0     	ldrsh	r2, [r4, #160]
    2ccc: e59d1018     	ldr	r1, [sp, #0x18]
    2cd0: e59a309c     	ldr	r3, [r10, #0x9c]
    2cd4: e1a0000a     	mov	r0, r10
    2cd8: e5933000     	ldr	r3, [r3]
    2cdc: ebfff7c0     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x2100
    2ce0: e59400c4     	ldr	r0, [r4, #0xc4]
    2ce4: ebfff782     	bl	0xaf4 <.plt+0xec>       @ imm = #-0x21f8
    2ce8: e59a5020     	ldr	r5, [r10, #0x20]
    2cec: e3a02001     	mov	r2, #1
    2cf0: e3750001     	cmn	r5, #1
    2cf4: e58420c8     	str	r2, [r4, #0xc8]
    2cf8: 0a00000a     	beq	0x2d28 <comport_tick+0x3b8> @ imm = #0x28
    2cfc: e28a2060     	add	r2, r10, #96
    2d00: e1a01009     	mov	r1, r9
    2d04: e1a00005     	mov	r0, r5
    2d08: ebfff767     	bl	0xaac <.plt+0xa4>       @ imm = #-0x2264
    2d0c: e1a00005     	mov	r0, r5
    2d10: ebfff7b0     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x2140
    2d14: e59aa09c     	ldr	r10, [r10, #0x9c]
    2d18: e1d41af0     	ldrsh	r1, [r4, #160]
    2d1c: e59d001c     	ldr	r0, [sp, #0x1c]
    2d20: e59a2000     	ldr	r2, [r10]
    2d24: ebfff784     	bl	0xb3c <.plt+0x134>      @ imm = #-0x21f0
    2d28: e59400e8     	ldr	r0, [r4, #0xe8]
    2d2c: e59dc014     	ldr	r12, [sp, #0x14]
    2d30: e3e0e000     	mvn	lr, #0
    2d34: e3500000     	cmp	r0, #0
    2d38: e58ce020     	str	lr, [r12, #0x20]
    2d3c: e1c4eab0     	strh	lr, [r4, #160]
    2d40: 0affff3a     	beq	0x2a30 <comport_tick+0xc0> @ imm = #-0x318
    2d44: eeb00a48     	vmov.f32	s0, s16
    2d48: ebfff78a     	bl	0xb78 <.plt+0x170>      @ imm = #-0x21d8
    2d4c: eaffff37     	b	0x2a30 <comport_tick+0xc0> @ imm = #-0x324
    2d50: ebfff770     	bl	0xb18 <.plt+0x110>      @ imm = #-0x2240
    2d54: e5901000     	ldr	r1, [r0]
    2d58: e58d1010     	str	r1, [sp, #0x10]
    2d5c: eaffff33     	b	0x2a30 <comport_tick+0xc0> @ imm = #-0x334
    2d60: 1a000029     	bne	0x2e0c <comport_tick+0x49c> @ imm = #0xa4
    2d64: e59420fc     	ldr	r2, [r4, #0xfc]
    2d68: e3520000     	cmp	r2, #0
    2d6c: 1a00000a     	bne	0x2d9c <comport_tick+0x42c> @ imm = #0x28
    2d70: e594a0c8     	ldr	r10, [r4, #0xc8]
    2d74: e35a0000     	cmp	r10, #0
    2d78: 1a000004     	bne	0x2d90 <comport_tick+0x420> @ imm = #0x10
    2d7c: e59dc014     	ldr	r12, [sp, #0x14]
    2d80: e59400c4     	ldr	r0, [r4, #0xc4]
    2d84: e28c3d43     	add	r3, r12, #4288
    2d88: ed930b06     	vldr	d0, [r3, #24]
    2d8c: ebfff73d     	bl	0xa88 <.plt+0x80>       @ imm = #-0x230c
    2d90: e28dd0b4     	add	sp, sp, #180
    2d94: ecbd8b02     	vpop	{d8}
    2d98: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2d9c: e59db014     	ldr	r11, [sp, #0x14]
    2da0: e59410f0     	ldr	r1, [r4, #0xf0]
    2da4: e59b0020     	ldr	r0, [r11, #0x20]
    2da8: ebfff766     	bl	0xb48 <.plt+0x140>      @ imm = #-0x2268
    2dac: e59410fc     	ldr	r1, [r4, #0xfc]
    2db0: e1500001     	cmp	r0, r1
    2db4: e1a09000     	mov	r9, r0
    2db8: 0a000006     	beq	0x2dd8 <comport_tick+0x468> @ imm = #0x18
    2dbc: ebfff755     	bl	0xb18 <.plt+0x110>      @ imm = #-0x22ac
    2dc0: e59f6034     	ldr	r6, [pc, #0x34]         @ 0x2dfc <comport_tick+0x48c>
    2dc4: e1a02009     	mov	r2, r9
    2dc8: e08f1006     	add	r1, pc, r6
    2dcc: e5903000     	ldr	r3, [r0]
    2dd0: e1a0000b     	mov	r0, r11
    2dd4: ebfff782     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x21f8
    2dd8: e3a05000     	mov	r5, #0
    2ddc: e58450fc     	str	r5, [r4, #0xfc]
    2de0: eaffffe2     	b	0x2d70 <comport_tick+0x400> @ imm = #-0x78
    2de4: 00 00 80 bf  	.word	0xbf800000
    2de8: 64 26 01 00  	.word	0x00012664
    2dec: c8 00 00 00  	.word	0x000000c8
    2df0: 38 1c 00 00  	.word	0x00001c38
    2df4: 40 1f 00 00  	.word	0x00001f40
    2df8: 1b 54 00 00  	.word	0x0000541b
    2dfc: e4 1b 00 00  	.word	0x00001be4
    2e00: 54 1b 00 00  	.word	0x00001b54
    2e04: cc 1a 00 00  	.word	0x00001acc
    2e08: a0 1a 00 00  	.word	0x00001aa0
    2e0c: e1d48cf0     	ldrsh	r8, [r4, #192]
    2e10: e3580009     	cmp	r8, #9
    2e14: da000002     	ble	0x2e24 <comport_tick+0x4b4> @ imm = #0x8
    2e18: e288e001     	add	lr, r8, #1
    2e1c: e1c4ecb0     	strh	lr, [r4, #192]
    2e20: eaffffcf     	b	0x2d64 <comport_tick+0x3f4> @ imm = #-0xc4
    2e24: e51f702c     	ldr	r7, [pc, #-0x2c]        @ 0x2e00 <comport_tick+0x490>
    2e28: e59d1010     	ldr	r1, [sp, #0x10]
    2e2c: e08f0007     	add	r0, pc, r7
    2e30: ebfff741     	bl	0xb3c <.plt+0x134>      @ imm = #-0x22fc
    2e34: e1d48cf0     	ldrsh	r8, [r4, #192]
    2e38: eafffff6     	b	0x2e18 <comport_tick+0x4a8> @ imm = #-0x28
    2e3c: ee07ea90     	vmov	s15, lr
    2e40: e51f8044     	ldr	r8, [pc, #-0x44]        @ 0x2e04 <comport_tick+0x494>
    2e44: e3a07001     	mov	r7, #1
    2e48: e08f0008     	add	r0, pc, r8
    2e4c: eeb80ae7     	vcvt.f32.s32	s0, s15
    2e50: e594b0e8     	ldr	r11, [r4, #0xe8]
    2e54: e58d7028     	str	r7, [sp, #0x28]
    2e58: e51f5058     	ldr	r5, [pc, #-0x58]        @ 0x2e08 <comport_tick+0x498>
    2e5c: ed8d0a0b     	vstr	s0, [sp, #44]
    2e60: ebfff6ed     	bl	0xa1c <.plt+0x14>       @ imm = #-0x244c
    2e64: e1a02007     	mov	r2, r7
    2e68: e28d3028     	add	r3, sp, #40
    2e6c: e1a01000     	mov	r1, r0
    2e70: e1a0000b     	mov	r0, r11
    2e74: ebfff748     	bl	0xb9c <.plt+0x194>      @ imm = #-0x22e0
    2e78: e59d0014     	ldr	r0, [sp, #0x14]
    2e7c: e1d42af0     	ldrsh	r2, [r4, #160]
    2e80: e08f1005     	add	r1, pc, r5
    2e84: e590309c     	ldr	r3, [r0, #0x9c]
    2e88: e5933000     	ldr	r3, [r3]
    2e8c: ebfff754     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x22b0
    2e90: e59400c8     	ldr	r0, [r4, #0xc8]
    2e94: e3500000     	cmp	r0, #0
    2e98: 0a000003     	beq	0x2eac <comport_tick+0x53c> @ imm = #0xc
    2e9c: e59420d0     	ldr	r2, [r4, #0xd0]
    2ea0: e2821001     	add	r1, r2, #1
    2ea4: e58410d0     	str	r1, [r4, #0xd0]
    2ea8: eaffffb8     	b	0x2d90 <comport_tick+0x420> @ imm = #-0x120
    2eac: ed9f0b03     	vldr	d0, [pc, #12]           @ 0x2ec0 <comport_tick+0x550>
    2eb0: e59400c4     	ldr	r0, [r4, #0xc4]
    2eb4: ebfff6f3     	bl	0xa88 <.plt+0x80>       @ imm = #-0x2434
    2eb8: eafffff7     	b	0x2e9c <comport_tick+0x52c> @ imm = #-0x24
    2ebc: e1a00000     	mov	r0, r0
    2ec0: 00 00 00 00  	.word	0x00000000
    2ec4: 00 40 8f 40  	.word	0x408f4000

