00001d4c <arbhar_shmem_set_tab>:
    1d4c: e59fc1ec     	ldr	r12, [pc, #0x1ec]       @ 0x1f40 <arbhar_shmem_set_tab+0x1f4>  // u32=0x1129c; f32?=9.8511282e-41
    1d50: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    1d54: e1a07002     	mov	r7, r2
    1d58: e59f21e4     	ldr	r2, [pc, #0x1e4]        @ 0x1f44 <arbhar_shmem_set_tab+0x1f8>  // u32=0xd0; f32?=2.91470081e-43
    1d5c: e08f400c     	add	r4, pc, r12
    1d60: e1a05003     	mov	r5, r3
    1d64: e24dd008     	sub	sp, sp, #8
    1d68: e1a03004     	mov	r3, r4
    1d6c: e1a06000     	mov	r6, r0
    1d70: e7943002     	ldr	r3, [r4, r2]
    1d74: e1a08001     	mov	r8, r1
    1d78: e1a00007     	mov	r0, r7
    1d7c: e5931000     	ldr	r1, [r3]
    1d80: ebfffb56     	bl	0xae0 <.plt+0xec>       @ imm = #-0x12a8  // CALL pd_findbyclass
    1d84: e2504000     	subs	r4, r0, #0
    1d88: 0a000062     	beq	0x1f18 <arbhar_shmem_set_tab+0x1cc> @ imm = #0x188
    1d8c: e28d2004     	add	r2, sp, #4
    1d90: e1a0100d     	mov	r1, sp
    1d94: ebfffb48     	bl	0xabc <.plt+0xc8>       @ imm = #-0x12e0  // CALL garray_getfloatwords
    1d98: e2504000     	subs	r4, r0, #0
    1d9c: 0a000055     	beq	0x1ef8 <arbhar_shmem_set_tab+0x1ac> @ imm = #0x154
    1da0: e086e108     	add	lr, r6, r8, lsl #2
    1da4: e59d0000     	ldr	r0, [sp]
    1da8: e59d1020     	ldr	r1, [sp, #0x20]
    1dac: e59ec08c     	ldr	r12, [lr, #0x8c]
    1db0: e0404005     	sub	r4, r0, r5
    1db4: eddd7a09     	vldr	s15, [sp, #36]
    1db8: e04c7001     	sub	r7, r12, r1
    1dbc: ee004a10     	vmov	s0, r4
    1dc0: ee007a90     	vmov	s1, r7
    1dc4: eeb87ac0     	vcvt.f32.s32	s14, s0
    1dc8: eeb81ae0     	vcvt.f32.s32	s2, s1
    1dcc: eeb41ac7     	vcmpe.f32	s2, s14
    1dd0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1dd4: 8eb01a47     	vmovhi.f32	s2, s14
    1dd8: eefd1ac1     	vcvt.s32.f32	s3, s2
    1ddc: eef86ae7     	vcvt.f32.s32	s13, s15
    1de0: eeb82ae1     	vcvt.f32.s32	s4, s3
    1de4: eeb42ae6     	vcmpe.f32	s4, s13
    1de8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1dec: 8eb02a66     	vmovhi.f32	s4, s13
    1df0: eefd2ac2     	vcvt.s32.f32	s5, s4
    1df4: ee124a90     	vmov	r4, s5
    1df8: e3540000     	cmp	r4, #0
    1dfc: da00004d     	ble	0x1f38 <arbhar_shmem_set_tab+0x1ec> @ imm = #0x134
    1e00: e59d8004     	ldr	r8, [sp, #0x4]
    1e04: e0840005     	add	r0, r4, r5
    1e08: e59e2058     	ldr	r2, [lr, #0x58]
    1e0c: e0883105     	add	r3, r8, r5, lsl #2
    1e10: e0885100     	add	r5, r8, r0, lsl #2
    1e14: e045e003     	sub	lr, r5, r3
    1e18: e0822101     	add	r2, r2, r1, lsl #2
    1e1c: e24ec004     	sub	r12, lr, #4
    1e20: e1a0112c     	lsr	r1, r12, #2
    1e24: e2817001     	add	r7, r1, #1
    1e28: e2176007     	ands	r6, r7, #7
    1e2c: 0a00001a     	beq	0x1e9c <arbhar_shmem_set_tab+0x150> @ imm = #0x68
    1e30: e3560001     	cmp	r6, #1
    1e34: 0a000014     	beq	0x1e8c <arbhar_shmem_set_tab+0x140> @ imm = #0x50
    1e38: e3560002     	cmp	r6, #2
    1e3c: 0a000010     	beq	0x1e84 <arbhar_shmem_set_tab+0x138> @ imm = #0x40
    1e40: e3560003     	cmp	r6, #3
    1e44: 0a00000c     	beq	0x1e7c <arbhar_shmem_set_tab+0x130> @ imm = #0x30
    1e48: e3560004     	cmp	r6, #4
    1e4c: 0a000008     	beq	0x1e74 <arbhar_shmem_set_tab+0x128> @ imm = #0x20
    1e50: e3560005     	cmp	r6, #5
    1e54: 0a000004     	beq	0x1e6c <arbhar_shmem_set_tab+0x120> @ imm = #0x10
    1e58: e3560006     	cmp	r6, #6
    1e5c: 14938004     	ldrne	r8, [r3], #4
    1e60: 14828004     	strne	r8, [r2], #4
    1e64: e4930004     	ldr	r0, [r3], #4
    1e68: e4820004     	str	r0, [r2], #4
    1e6c: e493e004     	ldr	lr, [r3], #4
    1e70: e482e004     	str	lr, [r2], #4
    1e74: e493c004     	ldr	r12, [r3], #4
    1e78: e482c004     	str	r12, [r2], #4
    1e7c: e4931004     	ldr	r1, [r3], #4
    1e80: e4821004     	str	r1, [r2], #4
    1e84: e4937004     	ldr	r7, [r3], #4
    1e88: e4827004     	str	r7, [r2], #4
    1e8c: e4936004     	ldr	r6, [r3], #4
    1e90: e1550003     	cmp	r5, r3
    1e94: e4826004     	str	r6, [r2], #4
    1e98: 0a00001b     	beq	0x1f0c <arbhar_shmem_set_tab+0x1c0> @ imm = #0x6c
    1e9c: e1a08003     	mov	r8, r3
    1ea0: e1a00002     	mov	r0, r2
    1ea4: e498e004     	ldr	lr, [r8], #4
    1ea8: e2833020     	add	r3, r3, #32
    1eac: e2822020     	add	r2, r2, #32
    1eb0: e480e004     	str	lr, [r0], #4
    1eb4: e513e01c     	ldr	lr, [r3, #-0x1c]
    1eb8: e502e01c     	str	lr, [r2, #-0x1c]
    1ebc: e598c004     	ldr	r12, [r8, #0x4]
    1ec0: e580c004     	str	r12, [r0, #0x4]
    1ec4: e5131014     	ldr	r1, [r3, #-0x14]
    1ec8: e5021014     	str	r1, [r2, #-0x14]
    1ecc: e5137010     	ldr	r7, [r3, #-0x10]
    1ed0: e5027010     	str	r7, [r2, #-0x10]
    1ed4: e513600c     	ldr	r6, [r3, #-0xc]
    1ed8: e502600c     	str	r6, [r2, #-0xc]
    1edc: e5138008     	ldr	r8, [r3, #-0x8]
    1ee0: e5028008     	str	r8, [r2, #-0x8]
    1ee4: e5130004     	ldr	r0, [r3, #-0x4]
    1ee8: e1550003     	cmp	r5, r3
    1eec: e5020004     	str	r0, [r2, #-0x4]
    1ef0: 1affffe9     	bne	0x1e9c <arbhar_shmem_set_tab+0x150> @ imm = #-0x5c
    1ef4: ea000004     	b	0x1f0c <arbhar_shmem_set_tab+0x1c0> @ imm = #0x10
    1ef8: e59f5048     	ldr	r5, [pc, #0x48]         @ 0x1f48 <arbhar_shmem_set_tab+0x1fc>  // u32=0xdf0; f32?=4.99983292e-42
    1efc: e1a00006     	mov	r0, r6
    1f00: e5972000     	ldr	r2, [r7]
    1f04: e08f1005     	add	r1, pc, r5
    1f08: ebfffb24     	bl	0xba0 <.plt+0x1ac>      @ imm = #-0x1370  // CALL pd_error
    1f0c: e1a00004     	mov	r0, r4
    1f10: e28dd008     	add	sp, sp, #8
    1f14: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    1f18: e59fe02c     	ldr	lr, [pc, #0x2c]         @ 0x1f4c <arbhar_shmem_set_tab+0x200>  // u32=0xdbc; f32?=4.9269654e-42
    1f1c: e1a00006     	mov	r0, r6
    1f20: e5972000     	ldr	r2, [r7]
    1f24: e08f100e     	add	r1, pc, lr
    1f28: ebfffb1c     	bl	0xba0 <.plt+0x1ac>      @ imm = #-0x1390  // CALL pd_error
    1f2c: e1a00004     	mov	r0, r4
    1f30: e28dd008     	add	sp, sp, #8
    1f34: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    1f38: e3a04000     	mov	r4, #0
    1f3c: eafffff2     	b	0x1f0c <arbhar_shmem_set_tab+0x1c0> @ imm = #-0x38
    1f40: 9c 12 01 00  	.word	0x0001129c
    1f44: d0 00 00 00  	.word	0x000000d0
    1f48: f0 0d 00 00  	.word	0x00000df0
    1f4c: bc 0d 00 00  	.word	0x00000dbc

