00002e50 <arbhar_rec_tilde_dsp>:
    2e50: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2e54: e1a04000     	mov	r4, r0
    2e58: ed2d8b02     	vpush	{d8}
    2e5c: e59f0258     	ldr	r0, [pc, #0x258]        @ 0x30bc <arbhar_rec_tilde_dsp+0x26c>
    2e60: e08f0000     	add	r0, pc, r0
    2e64: e24dd034     	sub	sp, sp, #52
    2e68: e58d102c     	str	r1, [sp, #0x2c]
    2e6c: ebfffe38     	bl	0x2754 <.plt+0x254>     @ imm = #-0x720
    2e70: e594a250     	ldr	r10, [r4, #0x250]
    2e74: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x30c0 <arbhar_rec_tilde_dsp+0x270>
    2e78: e35a0001     	cmp	r10, #1
    2e7c: e08a1faa     	add	r1, r10, r10, lsr #31
    2e80: e08f3003     	add	r3, pc, r3
    2e84: e58d3024     	str	r3, [sp, #0x24]
    2e88: e1a020c1     	asr	r2, r1, #1
    2e8c: e58d2014     	str	r2, [sp, #0x14]
    2e90: da000075     	ble	0x306c <arbhar_rec_tilde_dsp+0x21c> @ imm = #0x1d4
    2e94: e59f7228     	ldr	r7, [pc, #0x228]        @ 0x30c4 <arbhar_rec_tilde_dsp+0x274>
    2e98: e1a06004     	mov	r6, r4
    2e9c: e59dc024     	ldr	r12, [sp, #0x24]
    2ea0: e59f5220     	ldr	r5, [pc, #0x220]        @ 0x30c8 <arbhar_rec_tilde_dsp+0x278>
    2ea4: e59f9220     	ldr	r9, [pc, #0x220]        @ 0x30cc <arbhar_rec_tilde_dsp+0x27c>
    2ea8: e59fe220     	ldr	lr, [pc, #0x220]        @ 0x30d0 <arbhar_rec_tilde_dsp+0x280>
    2eac: e08f8005     	add	r8, pc, r5
    2eb0: e08fb009     	add	r11, pc, r9
    2eb4: e58d801c     	str	r8, [sp, #0x1c]
    2eb8: e58db028     	str	r11, [sp, #0x28]
    2ebc: e08f000e     	add	r0, pc, lr
    2ec0: e79c9007     	ldr	r9, [r12, r7]
    2ec4: e3a05000     	mov	r5, #0
    2ec8: e58d0020     	str	r0, [sp, #0x20]
    2ecc: ea000038     	b	0x2fb4 <arbhar_rec_tilde_dsp+0x164> @ imm = #0xe0
    2ed0: ee181a90     	vmov	r1, s17
    2ed4: e2812025     	add	r2, r1, #37
    2ed8: e1a0c102     	lsl	r12, r2, #2
    2edc: e28ce060     	add	lr, r12, #96
    2ee0: e084100c     	add	r1, r4, r12
    2ee4: e084200e     	add	r2, r4, lr
    2ee8: ebfffde9     	bl	0x2694 <.plt+0x194>     @ imm = #-0x85c
    2eec: e250a000     	subs	r10, r0, #0
    2ef0: 1a00004d     	bne	0x302c <arbhar_rec_tilde_dsp+0x1dc> @ imm = #0x134
    2ef4: e5983154     	ldr	r3, [r8, #0x154]
    2ef8: e1a00004     	mov	r0, r4
    2efc: e59d101c     	ldr	r1, [sp, #0x1c]
    2f00: e5932000     	ldr	r2, [r3]
    2f04: ebfffe54     	bl	0x285c <.plt+0x35c>     @ imm = #-0x6b0
    2f08: e588a0f4     	str	r10, [r8, #0xf4]
    2f0c: e287a001     	add	r10, r7, #1
    2f10: e596b158     	ldr	r11, [r6, #0x158]
    2f14: e5991000     	ldr	r1, [r9]
    2f18: ee00aa10     	vmov	s0, r10
    2f1c: e1a0000b     	mov	r0, r11
    2f20: eef80ac0     	vcvt.f32.s32	s1, s0
    2f24: eebd8ae0     	vcvt.s32.f32	s16, s1
    2f28: ee188a10     	vmov	r8, s16
    2f2c: e0848108     	add	r8, r4, r8, lsl #2
    2f30: e588b154     	str	r11, [r8, #0x154]
    2f34: ebfffde5     	bl	0x26d0 <.plt+0x1d0>     @ imm = #-0x86c
    2f38: e250c000     	subs	r12, r0, #0
    2f3c: e58dc010     	str	r12, [sp, #0x10]
    2f40: 0a00002f     	beq	0x3004 <arbhar_rec_tilde_dsp+0x1b4> @ imm = #0xbc
    2f44: ee18ea10     	vmov	lr, s16
    2f48: e28e3025     	add	r3, lr, #37
    2f4c: e1a01103     	lsl	r1, r3, #2
    2f50: e2812060     	add	r2, r1, #96
    2f54: e0841001     	add	r1, r4, r1
    2f58: e0842002     	add	r2, r4, r2
    2f5c: ebfffdcc     	bl	0x2694 <.plt+0x194>     @ imm = #-0x8d0
    2f60: e3500000     	cmp	r0, #0
    2f64: e58d0018     	str	r0, [sp, #0x18]
    2f68: 1a00002c     	bne	0x3020 <arbhar_rec_tilde_dsp+0x1d0> @ imm = #0xb0
    2f6c: e598b154     	ldr	r11, [r8, #0x154]
    2f70: e1a00004     	mov	r0, r4
    2f74: e59d1020     	ldr	r1, [sp, #0x20]
    2f78: e59b2000     	ldr	r2, [r11]
    2f7c: ebfffe36     	bl	0x285c <.plt+0x35c>     @ imm = #-0x728
    2f80: e59dc018     	ldr	r12, [sp, #0x18]
    2f84: e588c0f4     	str	r12, [r8, #0xf4]
    2f88: e1a01007     	mov	r1, r7
    2f8c: e1a00004     	mov	r0, r4
    2f90: ebfffef8     	bl	0x2b78 <setOutputStream> @ imm = #-0x420
    2f94: e1a0100a     	mov	r1, r10
    2f98: e1a00004     	mov	r0, r4
    2f9c: e2855001     	add	r5, r5, #1
    2fa0: ebfffef4     	bl	0x2b78 <setOutputStream> @ imm = #-0x430
    2fa4: e59d7014     	ldr	r7, [sp, #0x14]
    2fa8: e2866008     	add	r6, r6, #8
    2fac: e1570005     	cmp	r7, r5
    2fb0: da00002d     	ble	0x306c <arbhar_rec_tilde_dsp+0x21c> @ imm = #0xb4
    2fb4: e1a07085     	lsl	r7, r5, #1
    2fb8: e596a154     	ldr	r10, [r6, #0x154]
    2fbc: e5991000     	ldr	r1, [r9]
    2fc0: ee077a90     	vmov	s15, r7
    2fc4: e1a0000a     	mov	r0, r10
    2fc8: eeb88ae7     	vcvt.f32.s32	s16, s15
    2fcc: eefd8ac8     	vcvt.s32.f32	s17, s16
    2fd0: ee183a90     	vmov	r3, s17
    2fd4: e0848103     	add	r8, r4, r3, lsl #2
    2fd8: e588a154     	str	r10, [r8, #0x154]
    2fdc: ebfffdbb     	bl	0x26d0 <.plt+0x1d0>     @ imm = #-0x914
    2fe0: e250b000     	subs	r11, r0, #0
    2fe4: 1affffb9     	bne	0x2ed0 <arbhar_rec_tilde_dsp+0x80> @ imm = #-0x11c
    2fe8: e59a0000     	ldr	r0, [r10]
    2fec: e5d0b000     	ldrb	r11, [r0]
    2ff0: e35b0000     	cmp	r11, #0
    2ff4: 1a00000f     	bne	0x3038 <arbhar_rec_tilde_dsp+0x1e8> @ imm = #0x3c
    2ff8: e3a01000     	mov	r1, #0
    2ffc: e58810f4     	str	r1, [r8, #0xf4]
    3000: eaffffc1     	b	0x2f0c <arbhar_rec_tilde_dsp+0xbc> @ imm = #-0xfc
    3004: e59be000     	ldr	lr, [r11]
    3008: e5de3000     	ldrb	r3, [lr]
    300c: e3530000     	cmp	r3, #0
    3010: 1a00000e     	bne	0x3050 <arbhar_rec_tilde_dsp+0x200> @ imm = #0x38
    3014: e3a00000     	mov	r0, #0
    3018: e58800f4     	str	r0, [r8, #0xf4]
    301c: eaffffd9     	b	0x2f88 <arbhar_rec_tilde_dsp+0x138> @ imm = #-0x9c
    3020: e59d0010     	ldr	r0, [sp, #0x10]
    3024: ebfffdd0     	bl	0x276c <.plt+0x26c>     @ imm = #-0x8c0
    3028: eaffffd6     	b	0x2f88 <arbhar_rec_tilde_dsp+0x138> @ imm = #-0xa8
    302c: e1a0000b     	mov	r0, r11
    3030: ebfffdcd     	bl	0x276c <.plt+0x26c>     @ imm = #-0x8cc
    3034: eaffffb4     	b	0x2f0c <arbhar_rec_tilde_dsp+0xbc> @ imm = #-0x130
    3038: e5982154     	ldr	r2, [r8, #0x154]
    303c: e1a00004     	mov	r0, r4
    3040: e59d1028     	ldr	r1, [sp, #0x28]
    3044: e5922000     	ldr	r2, [r2]
    3048: ebfffe03     	bl	0x285c <.plt+0x35c>     @ imm = #-0x7f4
    304c: eaffffe9     	b	0x2ff8 <arbhar_rec_tilde_dsp+0x1a8> @ imm = #-0x5c
    3050: e5982154     	ldr	r2, [r8, #0x154]
    3054: e1a00004     	mov	r0, r4
    3058: e59f1074     	ldr	r1, [pc, #0x74]         @ 0x30d4 <arbhar_rec_tilde_dsp+0x284>
    305c: e5922000     	ldr	r2, [r2]
    3060: e08f1001     	add	r1, pc, r1
    3064: ebfffdfc     	bl	0x285c <.plt+0x35c>     @ imm = #-0x810
    3068: eaffffe9     	b	0x3014 <arbhar_rec_tilde_dsp+0x1c4> @ imm = #-0x5c
    306c: e59d902c     	ldr	r9, [sp, #0x2c]
    3070: e1a02004     	mov	r2, r4
    3074: e59dc024     	ldr	r12, [sp, #0x24]
    3078: e3a01005     	mov	r1, #5
    307c: e59f8054     	ldr	r8, [pc, #0x54]         @ 0x30d8 <arbhar_rec_tilde_dsp+0x288>
    3080: e599a000     	ldr	r10, [r9]
    3084: e599b008     	ldr	r11, [r9, #0x8]
    3088: e5995004     	ldr	r5, [r9, #0x4]
    308c: e59a4000     	ldr	r4, [r10]
    3090: e59a3004     	ldr	r3, [r10, #0x4]
    3094: e79c0008     	ldr	r0, [r12, r8]
    3098: e58d4008     	str	r4, [sp, #0x8]
    309c: e59be004     	ldr	lr, [r11, #0x4]
    30a0: e58de004     	str	lr, [sp, #0x4]
    30a4: e5957004     	ldr	r7, [r5, #0x4]
    30a8: e58d7000     	str	r7, [sp]
    30ac: ebfffdc3     	bl	0x27c0 <.plt+0x2c0>     @ imm = #-0x8f4
    30b0: e28dd034     	add	sp, sp, #52
    30b4: ecbd8b02     	vpop	{d8}
    30b8: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    30bc: 60 66 00 00  	.word	0x00006660
    30c0: 78 71 01 00  	.word	0x00017178
    30c4: 54 01 00 00  	.word	0x00000154
    30c8: 48 66 00 00  	.word	0x00006648
    30cc: 24 66 00 00  	.word	0x00006624
    30d0: 38 66 00 00  	.word	0x00006638
    30d4: 74 64 00 00  	.word	0x00006474
    30d8: 60 01 00 00  	.word	0x00000160

