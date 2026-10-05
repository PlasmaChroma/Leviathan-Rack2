00001f04 <comport_rts>:
    1f04: e92d40f0     	push	{r4, r5, r6, r7, lr}
    1f08: e24dd00c     	sub	sp, sp, #12
    1f0c: e5905020     	ldr	r5, [r0, #0x20]
    1f10: e3750001     	cmn	r5, #1
    1f14: 0a000021     	beq	0x1fa0 <comport_rts+0x9c> @ imm = #0x84
    1f18: eefd7ac0     	vcvt.s32.f32	s15, s0
    1f1c: e28d7004     	add	r7, sp, #4
    1f20: e1a04000     	mov	r4, r0
    1f24: e1a02007     	mov	r2, r7
    1f28: e59f1078     	ldr	r1, [pc, #0x78]         @ 0x1fa8 <comport_rts+0xa4>
    1f2c: e1a00005     	mov	r0, r5
    1f30: ee176a90     	vmov	r6, s15
    1f34: ebfffad6     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x14a8
    1f38: e59d3004     	ldr	r3, [sp, #0x4]
    1f3c: e1a02007     	mov	r2, r7
    1f40: e3560000     	cmp	r6, #0
    1f44: 03c33004     	biceq	r3, r3, #4
    1f48: 13833004     	orrne	r3, r3, #4
    1f4c: e1a00005     	mov	r0, r5
    1f50: e59f1054     	ldr	r1, [pc, #0x54]         @ 0x1fac <comport_rts+0xa8>
    1f54: e58d3004     	str	r3, [sp, #0x4]
    1f58: ebfffacd     	bl	0xa94 <.plt+0x8c>       @ imm = #-0x14cc
    1f5c: e5940020     	ldr	r0, [r4, #0x20]
    1f60: e3700001     	cmn	r0, #1
    1f64: 0a00000d     	beq	0x1fa0 <comport_rts+0x9c> @ imm = #0x34
    1f68: e2841a01     	add	r1, r4, #4096
    1f6c: e59120e0     	ldr	r2, [r1, #0xe0]
    1f70: e3520000     	cmp	r2, #0
    1f74: da000009     	ble	0x1fa0 <comport_rts+0x9c> @ imm = #0x24
    1f78: e296e000     	adds	lr, r6, #0
    1f7c: 13a0e001     	movne	lr, #1
    1f80: e594c09c     	ldr	r12, [r4, #0x9c]
    1f84: ee00ea10     	vmov	s0, lr
    1f88: e59f5020     	ldr	r5, [pc, #0x20]         @ 0x1fb0 <comport_rts+0xac>
    1f8c: e59c1000     	ldr	r1, [r12]
    1f90: e08f0005     	add	r0, pc, r5
    1f94: eeb87bc0     	vcvt.f64.s32	d7, s0
    1f98: ec532b17     	vmov	r2, r3, d7
    1f9c: ebfffae6     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1468
    1fa0: e28dd00c     	add	sp, sp, #12
    1fa4: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    1fa8: 15 54 00 00  	.word	0x00005415
    1fac: 18 54 00 00  	.word	0x00005418
    1fb0: e4 26 00 00  	.word	0x000026e4

