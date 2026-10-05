00000ca4 <shmem_set_tab>:
     ca4: e59fc1e4     	ldr	r12, [pc, #0x1e4]       @ 0xe90 <shmem_set_tab+0x1ec>
     ca8: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
     cac: e1a08001     	mov	r8, r1
     cb0: e59f11dc     	ldr	r1, [pc, #0x1dc]        @ 0xe94 <shmem_set_tab+0x1f0>
     cb4: e08f400c     	add	r4, pc, r12
     cb8: e1a06003     	mov	r6, r3
     cbc: e24dd008     	sub	sp, sp, #8
     cc0: e1a03004     	mov	r3, r4
     cc4: e1a07000     	mov	r7, r0
     cc8: e7943001     	ldr	r3, [r4, r1]
     ccc: e1a00008     	mov	r0, r8
     cd0: e1a05002     	mov	r5, r2
     cd4: e5931000     	ldr	r1, [r3]
     cd8: ebfffeca     	bl	0x808 <.plt+0x98>       @ imm = #-0x4d8
     cdc: e2504000     	subs	r4, r0, #0
     ce0: 0a000060     	beq	0xe68 <shmem_set_tab+0x1c4> @ imm = #0x180
     ce4: e28d2004     	add	r2, sp, #4
     ce8: e1a0100d     	mov	r1, sp
     cec: ebfffebf     	bl	0x7f0 <.plt+0x80>       @ imm = #-0x504
     cf0: e2504000     	subs	r4, r0, #0
     cf4: 0a000053     	beq	0xe48 <shmem_set_tab+0x1a4> @ imm = #0x14c
     cf8: e5970028     	ldr	r0, [r7, #0x28]
     cfc: e59d2000     	ldr	r2, [sp]
     d00: e040e006     	sub	lr, r0, r6
     d04: eddd7a08     	vldr	s15, [sp, #32]
     d08: e042c005     	sub	r12, r2, r5
     d0c: ee07ea10     	vmov	s14, lr
     d10: ee00ca10     	vmov	s0, r12
     d14: eef80ac7     	vcvt.f32.s32	s1, s14
     d18: eeb81ac0     	vcvt.f32.s32	s2, s0
     d1c: eef40ac1     	vcmpe.f32	s1, s2
     d20: eef1fa10     	vmrs	APSR_nzcv, fpscr
     d24: 9eb01a60     	vmovls.f32	s2, s1
     d28: eefd1ac1     	vcvt.s32.f32	s3, s2
     d2c: eef86ae7     	vcvt.f32.s32	s13, s15
     d30: eeb82ae1     	vcvt.f32.s32	s4, s3
     d34: eeb42ae6     	vcmpe.f32	s4, s13
     d38: eef1fa10     	vmrs	APSR_nzcv, fpscr
     d3c: 8eb02a66     	vmovhi.f32	s4, s13
     d40: eefd2ac2     	vcvt.s32.f32	s5, s4
     d44: ee124a90     	vmov	r4, s5
     d48: e3540000     	cmp	r4, #0
     d4c: da00004d     	ble	0xe88 <shmem_set_tab+0x1e4> @ imm = #0x134
     d50: e59d8004     	ldr	r8, [sp, #0x4]
     d54: e0841005     	add	r1, r4, r5
     d58: e5977024     	ldr	r7, [r7, #0x24]
     d5c: e0882105     	add	r2, r8, r5, lsl #2
     d60: e0885101     	add	r5, r8, r1, lsl #2
     d64: e0450002     	sub	r0, r5, r2
     d68: e0873106     	add	r3, r7, r6, lsl #2
     d6c: e2406004     	sub	r6, r0, #4
     d70: e1a0e126     	lsr	lr, r6, #2
     d74: e28ec001     	add	r12, lr, #1
     d78: e21c8007     	ands	r8, r12, #7
     d7c: 0a00001a     	beq	0xdec <shmem_set_tab+0x148> @ imm = #0x68
     d80: e3580001     	cmp	r8, #1
     d84: 0a000014     	beq	0xddc <shmem_set_tab+0x138> @ imm = #0x50
     d88: e3580002     	cmp	r8, #2
     d8c: 0a000010     	beq	0xdd4 <shmem_set_tab+0x130> @ imm = #0x40
     d90: e3580003     	cmp	r8, #3
     d94: 0a00000c     	beq	0xdcc <shmem_set_tab+0x128> @ imm = #0x30
     d98: e3580004     	cmp	r8, #4
     d9c: 0a000008     	beq	0xdc4 <shmem_set_tab+0x120> @ imm = #0x20
     da0: e3580005     	cmp	r8, #5
     da4: 0a000004     	beq	0xdbc <shmem_set_tab+0x118> @ imm = #0x10
     da8: e3580006     	cmp	r8, #6
     dac: 14921004     	ldrne	r1, [r2], #4
     db0: 14831004     	strne	r1, [r3], #4
     db4: e4927004     	ldr	r7, [r2], #4
     db8: e4837004     	str	r7, [r3], #4
     dbc: e4920004     	ldr	r0, [r2], #4
     dc0: e4830004     	str	r0, [r3], #4
     dc4: e4926004     	ldr	r6, [r2], #4
     dc8: e4836004     	str	r6, [r3], #4
     dcc: e492e004     	ldr	lr, [r2], #4
     dd0: e483e004     	str	lr, [r3], #4
     dd4: e492c004     	ldr	r12, [r2], #4
     dd8: e483c004     	str	r12, [r3], #4
     ddc: e4928004     	ldr	r8, [r2], #4
     de0: e1550002     	cmp	r5, r2
     de4: e4838004     	str	r8, [r3], #4
     de8: 0a00001b     	beq	0xe5c <shmem_set_tab+0x1b8> @ imm = #0x6c
     dec: e1a07002     	mov	r7, r2
     df0: e1a01003     	mov	r1, r3
     df4: e4970004     	ldr	r0, [r7], #4
     df8: e2822020     	add	r2, r2, #32
     dfc: e2833020     	add	r3, r3, #32
     e00: e4810004     	str	r0, [r1], #4
     e04: e512e01c     	ldr	lr, [r2, #-0x1c]
     e08: e503e01c     	str	lr, [r3, #-0x1c]
     e0c: e5976004     	ldr	r6, [r7, #0x4]
     e10: e5816004     	str	r6, [r1, #0x4]
     e14: e512c014     	ldr	r12, [r2, #-0x14]
     e18: e503c014     	str	r12, [r3, #-0x14]
     e1c: e5128010     	ldr	r8, [r2, #-0x10]
     e20: e5038010     	str	r8, [r3, #-0x10]
     e24: e512700c     	ldr	r7, [r2, #-0xc]
     e28: e503700c     	str	r7, [r3, #-0xc]
     e2c: e5121008     	ldr	r1, [r2, #-0x8]
     e30: e5031008     	str	r1, [r3, #-0x8]
     e34: e5120004     	ldr	r0, [r2, #-0x4]
     e38: e1550002     	cmp	r5, r2
     e3c: e5030004     	str	r0, [r3, #-0x4]
     e40: 1affffe9     	bne	0xdec <shmem_set_tab+0x148> @ imm = #-0x5c
     e44: ea000004     	b	0xe5c <shmem_set_tab+0x1b8> @ imm = #0x10
     e48: e59f5048     	ldr	r5, [pc, #0x48]         @ 0xe98 <shmem_set_tab+0x1f4>
     e4c: e1a00007     	mov	r0, r7
     e50: e5982000     	ldr	r2, [r8]
     e54: e08f1005     	add	r1, pc, r5
     e58: ebfffe8b     	bl	0x88c <.plt+0x11c>      @ imm = #-0x5d4
     e5c: e1a00004     	mov	r0, r4
     e60: e28dd008     	add	sp, sp, #8
     e64: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
     e68: e59fe02c     	ldr	lr, [pc, #0x2c]         @ 0xe9c <shmem_set_tab+0x1f8>
     e6c: e1a00007     	mov	r0, r7
     e70: e5982000     	ldr	r2, [r8]
     e74: e08f100e     	add	r1, pc, lr
     e78: ebfffe83     	bl	0x88c <.plt+0x11c>      @ imm = #-0x5f4
     e7c: e1a00004     	mov	r0, r4
     e80: e28dd008     	add	sp, sp, #8
     e84: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
     e88: e3a04000     	mov	r4, #0
     e8c: eafffff2     	b	0xe5c <shmem_set_tab+0x1b8> @ imm = #-0x38
     e90: 44 13 01 00  	.word	0x00011344
     e94: 88 00 00 00  	.word	0x00000088
     e98: 58 0a 00 00  	.word	0x00000a58
     e9c: 24 0a 00 00  	.word	0x00000a24

