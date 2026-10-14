0000e96c <_setCaptureToggleGateSwitch>:
    e96c: e5d0303c     	ldrb	r3, [r0, #0x3c]
    e970: e3530003     	cmp	r3, #3
    e974: 0a000004     	beq	0xe98c <_setCaptureToggleGateSwitch+0x20> @ imm = #0x10
    e978: e59f016c     	ldr	r0, [pc, #0x16c]        @ 0xeaec <_setCaptureToggleGateSwitch+0x180>
    e97c: e3a02000     	mov	r2, #0
    e980: e08f1000     	add	r1, pc, r0
    e984: e5c121a8     	strb	r2, [r1, #0x1a8]
    e988: e12fff1e     	bx	lr
    e98c: eddf7a54     	vldr	s15, [pc, #336]         @ 0xeae4 <_setCaptureToggleGateSwitch+0x178>
    e990: ed9f7a54     	vldr	s14, [pc, #336]         @ 0xeae8 <_setCaptureToggleGateSwitch+0x17c>
    e994: eeb40ae7     	vcmpe.f32	s0, s15
    e998: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e99c: eeb40ac7     	vcmpe.f32	s0, s14
    e9a0: 43a03001     	movmi	r3, #1
    e9a4: 53a03000     	movpl	r3, #0
    e9a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e9ac: c2033001     	andgt	r3, r3, #1
    e9b0: d3a03000     	movle	r3, #0
    e9b4: e3530000     	cmp	r3, #0
    e9b8: 1a00002a     	bne	0xea68 <_setCaptureToggleGateSwitch+0xfc> @ imm = #0xa8
    e9bc: e59fc12c     	ldr	r12, [pc, #0x12c]       @ 0xeaf0 <_setCaptureToggleGateSwitch+0x184>
    e9c0: eeb40ae7     	vcmpe.f32	s0, s15
    e9c4: e08f100c     	add	r1, pc, r12
    e9c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e9cc: e5d1c1a8     	ldrb	r12, [r1, #0x1a8]
    e9d0: ba000029     	blt	0xea7c <_setCaptureToggleGateSwitch+0x110> @ imm = #0xa4
    e9d4: e35c0001     	cmp	r12, #1
    e9d8: 0a000030     	beq	0xeaa0 <_setCaptureToggleGateSwitch+0x134> @ imm = #0xc0
    e9dc: e35c0000     	cmp	r12, #0
    e9e0: 112fff1e     	bxne	lr
    e9e4: e59f3108     	ldr	r3, [pc, #0x108]        @ 0xeaf4 <_setCaptureToggleGateSwitch+0x188>
    e9e8: e08f2003     	add	r2, pc, r3
    e9ec: e5d2c1a9     	ldrb	r12, [r2, #0x1a9]
    e9f0: e59f1100     	ldr	r1, [pc, #0x100]        @ 0xeaf8 <_setCaptureToggleGateSwitch+0x18c>
    e9f4: e08f3001     	add	r3, pc, r1
    e9f8: e5d3207a     	ldrb	r2, [r3, #0x7a]
    e9fc: e152000c     	cmp	r2, r12
    ea00: 012fff1e     	bxeq	lr
    ea04: e92d4070     	push	{r4, r5, r6, lr}
    ea08: e24dd010     	sub	sp, sp, #16
    ea0c: e59f50e8     	ldr	r5, [pc, #0xe8]         @ 0xeafc <_setCaptureToggleGateSwitch+0x190>
    ea10: e1a04000     	mov	r4, r0
    ea14: e3a06002     	mov	r6, #2
    ea18: e58d6000     	str	r6, [sp]
    ea1c: e08f0005     	add	r0, pc, r5
    ea20: ebffd340     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb300
    ea24: e59fc0d4     	ldr	r12, [pc, #0xd4]        @ 0xeb00 <_setCaptureToggleGateSwitch+0x194>
    ea28: e5d42030     	ldrb	r2, [r4, #0x30]
    ea2c: e3a01001     	mov	r1, #1
    ea30: e08f500c     	add	r5, pc, r12
    ea34: e58d1008     	str	r1, [sp, #0x8]
    ea38: e3520062     	cmp	r2, #98
    ea3c: e5d531a9     	ldrb	r3, [r5, #0x1a9]
    ea40: ee003a10     	vmov	s0, r3
    ea44: eef80a40     	vcvt.f32.u32	s1, s0
    ea48: edcd0a03     	vstr	s1, [sp, #12]
    ea4c: e58d0004     	str	r0, [sp, #0x4]
    ea50: 9a000017     	bls	0xeab4 <_setCaptureToggleGateSwitch+0x148> @ imm = #0x5c
    ea54: e59f60a8     	ldr	r6, [pc, #0xa8]         @ 0xeb04 <_setCaptureToggleGateSwitch+0x198>
    ea58: e08fc006     	add	r12, pc, r6
    ea5c: e5cc307a     	strb	r3, [r12, #0x7a]
    ea60: e28dd010     	add	sp, sp, #16
    ea64: e8bd8070     	pop	{r4, r5, r6, pc}
    ea68: e59f1098     	ldr	r1, [pc, #0x98]         @ 0xeb08 <_setCaptureToggleGateSwitch+0x19c>
    ea6c: e3a02001     	mov	r2, #1
    ea70: e08f0001     	add	r0, pc, r1
    ea74: e5c021a8     	strb	r2, [r0, #0x1a8]
    ea78: e12fff1e     	bx	lr
    ea7c: eeb40ac7     	vcmpe.f32	s0, s14
    ea80: eef1fa10     	vmrs	APSR_nzcv, fpscr
    ea84: 8affffd4     	bhi	0xe9dc <_setCaptureToggleGateSwitch+0x70> @ imm = #-0xb0
    ea88: e35c0001     	cmp	r12, #1
    ea8c: 1affffd2     	bne	0xe9dc <_setCaptureToggleGateSwitch+0x70> @ imm = #-0xb8
    ea90: e5c1c1a9     	strb	r12, [r1, #0x1a9]
    ea94: e5c131a8     	strb	r3, [r1, #0x1a8]
    ea98: e5c0302a     	strb	r3, [r0, #0x2a]
    ea9c: eaffffd3     	b	0xe9f0 <_setCaptureToggleGateSwitch+0x84> @ imm = #-0xb4
    eaa0: e5c0c02a     	strb	r12, [r0, #0x2a]
    eaa4: e1a0c003     	mov	r12, r3
    eaa8: e5c131a9     	strb	r3, [r1, #0x1a9]
    eaac: e5c131a8     	strb	r3, [r1, #0x1a8]
    eab0: eaffffce     	b	0xe9f0 <_setCaptureToggleGateSwitch+0x84> @ imm = #-0xc8
    eab4: e59f0050     	ldr	r0, [pc, #0x50]         @ 0xeb0c <_setCaptureToggleGateSwitch+0x1a0>
    eab8: e284ea01     	add	lr, r4, #4096
    eabc: e08f0000     	add	r0, pc, r0
    eac0: e59e4db4     	ldr	r4, [lr, #0xdb4]
    eac4: ebffd317     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb3a4
    eac8: e1a0300d     	mov	r3, sp
    eacc: e1a02006     	mov	r2, r6
    ead0: e1a01000     	mov	r1, r0
    ead4: e1a00004     	mov	r0, r4
    ead8: ebffd465     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xae6c
    eadc: e5d531a9     	ldrb	r3, [r5, #0x1a9]
    eae0: eaffffdb     	b	0xea54 <_setCaptureToggleGateSwitch+0xe8> @ imm = #-0x94
    eae4: 00 80 6d 45  	.word	0x456d8000
    eae8: 00 00 96 43  	.word	0x43960000
    eaec: 30 8a 01 00  	.word	0x00018a30
    eaf0: ec 89 01 00  	.word	0x000189ec
    eaf4: c8 89 01 00  	.word	0x000189c8
    eaf8: f0 88 01 00  	.word	0x000188f0
    eafc: d4 6f 00 00  	.word	0x00006fd4
    eb00: 80 89 01 00  	.word	0x00018980
    eb04: 8c 88 01 00  	.word	0x0001888c
    eb08: 40 89 01 00  	.word	0x00018940
    eb0c: f8 5f 00 00  	.word	0x00005ff8

