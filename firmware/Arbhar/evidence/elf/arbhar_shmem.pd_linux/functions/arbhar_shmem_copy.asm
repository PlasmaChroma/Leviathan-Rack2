00000d60 <arbhar_shmem_copy>:
     d60: e3520001     	cmp	r2, #1
     d64: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
     d68: e24dd01c     	sub	sp, sp, #28
     d6c: da000070     	ble	0xf34 <arbhar_shmem_copy+0x1d4> @ imm = #0x1c0
     d70: e1a06002     	mov	r6, r2
     d74: e1a07000     	mov	r7, r0
     d78: e1a02003     	mov	r2, r3
     d7c: e1a01006     	mov	r1, r6
     d80: e3a00000     	mov	r0, #0
     d84: e1a0b003     	mov	r11, r3
     d88: ebffff81     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1fc
     d8c: e1a0200b     	mov	r2, r11
     d90: e1a01006     	mov	r1, r6
     d94: e3a00001     	mov	r0, #1
     d98: eefd7ac0     	vcvt.s32.f32	s15, s0
     d9c: ee179a90     	vmov	r9, s15
     da0: ebffff7b     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x214
     da4: eebd0ac0     	vcvt.s32.f32	s0, s0
     da8: ee108a10     	vmov	r8, s0
     dac: e3580000     	cmp	r8, #0
     db0: c3590000     	cmpgt	r9, #0
     db4: d3a05001     	movle	r5, #1
     db8: c3a05000     	movgt	r5, #0
     dbc: da000091     	ble	0x1008 <arbhar_shmem_copy+0x2a8> @ imm = #0x244
     dc0: e087a109     	add	r10, r7, r9, lsl #2
     dc4: e3560002     	cmp	r6, #2
     dc8: e59a008c     	ldr	r0, [r10, #0x8c]
     dcc: 01a06005     	moveq	r6, r5
     dd0: 01a04000     	moveq	r4, r0
     dd4: 1a00005d     	bne	0xf50 <arbhar_shmem_copy+0x1f0> @ imm = #0x174
     dd8: e59fe260     	ldr	lr, [pc, #0x260]        @ 0x1040 <arbhar_shmem_copy+0x2e0>
     ddc: e1a01009     	mov	r1, r9
     de0: e58d0008     	str	r0, [sp, #0x8]
     de4: e1a03004     	mov	r3, r4
     de8: e58d6004     	str	r6, [sp, #0x4]
     dec: e1a02005     	mov	r2, r5
     df0: e58d8000     	str	r8, [sp]
     df4: e08f000e     	add	r0, pc, lr
     df8: ebffff44     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x2f0
     dfc: e1540005     	cmp	r4, r5
     e00: da000077     	ble	0xfe4 <arbhar_shmem_copy+0x284> @ imm = #0x1dc
     e04: e59ab058     	ldr	r11, [r10, #0x58]
     e08: e2889016     	add	r9, r8, #22
     e0c: e08b3105     	add	r3, r11, r5, lsl #2
     e10: e08b8104     	add	r8, r11, r4, lsl #2
     e14: e048a003     	sub	r10, r8, r3
     e18: e7970109     	ldr	r0, [r7, r9, lsl #2]
     e1c: e24a1004     	sub	r1, r10, #4
     e20: e0802106     	add	r2, r0, r6, lsl #2
     e24: e1a0c121     	lsr	r12, r1, #2
     e28: e28ce001     	add	lr, r12, #1
     e2c: e21eb007     	ands	r11, lr, #7
     e30: 0a00001a     	beq	0xea0 <arbhar_shmem_copy+0x140> @ imm = #0x68
     e34: e35b0001     	cmp	r11, #1
     e38: 0a000014     	beq	0xe90 <arbhar_shmem_copy+0x130> @ imm = #0x50
     e3c: e35b0002     	cmp	r11, #2
     e40: 0a000010     	beq	0xe88 <arbhar_shmem_copy+0x128> @ imm = #0x40
     e44: e35b0003     	cmp	r11, #3
     e48: 0a00000c     	beq	0xe80 <arbhar_shmem_copy+0x120> @ imm = #0x30
     e4c: e35b0004     	cmp	r11, #4
     e50: 0a000008     	beq	0xe78 <arbhar_shmem_copy+0x118> @ imm = #0x20
     e54: e35b0005     	cmp	r11, #5
     e58: 0a000004     	beq	0xe70 <arbhar_shmem_copy+0x110> @ imm = #0x10
     e5c: e35b0006     	cmp	r11, #6
     e60: 14939004     	ldrne	r9, [r3], #4
     e64: 14829004     	strne	r9, [r2], #4
     e68: e493a004     	ldr	r10, [r3], #4
     e6c: e482a004     	str	r10, [r2], #4
     e70: e4930004     	ldr	r0, [r3], #4
     e74: e4820004     	str	r0, [r2], #4
     e78: e4931004     	ldr	r1, [r3], #4
     e7c: e4821004     	str	r1, [r2], #4
     e80: e493c004     	ldr	r12, [r3], #4
     e84: e482c004     	str	r12, [r2], #4
     e88: e493e004     	ldr	lr, [r3], #4
     e8c: e482e004     	str	lr, [r2], #4
     e90: e493b004     	ldr	r11, [r3], #4
     e94: e1530008     	cmp	r3, r8
     e98: e482b004     	str	r11, [r2], #4
     e9c: 0a000015     	beq	0xef8 <arbhar_shmem_copy+0x198> @ imm = #0x54
     ea0: e1a09003     	mov	r9, r3
     ea4: e1a0a002     	mov	r10, r2
     ea8: e4990004     	ldr	r0, [r9], #4
     eac: e2833020     	add	r3, r3, #32
     eb0: e2822020     	add	r2, r2, #32
     eb4: e48a0004     	str	r0, [r10], #4
     eb8: e513101c     	ldr	r1, [r3, #-0x1c]
     ebc: e502101c     	str	r1, [r2, #-0x1c]
     ec0: e599c004     	ldr	r12, [r9, #0x4]
     ec4: e58ac004     	str	r12, [r10, #0x4]
     ec8: e513e014     	ldr	lr, [r3, #-0x14]
     ecc: e502e014     	str	lr, [r2, #-0x14]
     ed0: e513b010     	ldr	r11, [r3, #-0x10]
     ed4: e502b010     	str	r11, [r2, #-0x10]
     ed8: e513900c     	ldr	r9, [r3, #-0xc]
     edc: e502900c     	str	r9, [r2, #-0xc]
     ee0: e513a008     	ldr	r10, [r3, #-0x8]
     ee4: e502a008     	str	r10, [r2, #-0x8]
     ee8: e5130004     	ldr	r0, [r3, #-0x4]
     eec: e1530008     	cmp	r3, r8
     ef0: e5020004     	str	r0, [r2, #-0x4]
     ef4: 1affffe9     	bne	0xea0 <arbhar_shmem_copy+0x140> @ imm = #-0x5c
     ef8: e1a01005     	mov	r1, r5
     efc: e0448005     	sub	r8, r4, r5
     f00: e59f513c     	ldr	r5, [pc, #0x13c]        @ 0x1044 <arbhar_shmem_copy+0x2e4>
     f04: e1a03006     	mov	r3, r6
     f08: e1a02004     	mov	r2, r4
     f0c: e0886006     	add	r6, r8, r6
     f10: e08f0005     	add	r0, pc, r5
     f14: e58d6000     	str	r6, [sp]
     f18: ebfffefc     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x410
     f1c: e5970050     	ldr	r0, [r7, #0x50]
     f20: ed9f0b44     	vldr	d0, [pc, #272]          @ 0x1038 <arbhar_shmem_copy+0x2d8>
     f24: ebffff05     	bl	0xb40 <.plt+0x14c>      @ imm = #-0x3ec
     f28: e1a00004     	mov	r0, r4
     f2c: e28dd01c     	add	sp, sp, #28
     f30: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     f34: e59f110c     	ldr	r1, [pc, #0x10c]        @ 0x1048 <arbhar_shmem_copy+0x2e8>
     f38: e3a04000     	mov	r4, #0
     f3c: e08f0001     	add	r0, pc, r1
     f40: ebfffef2     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x438
     f44: e1a00004     	mov	r0, r4
     f48: e28dd01c     	add	sp, sp, #28
     f4c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
     f50: e58d0014     	str	r0, [sp, #0x14]
     f54: e1a0200b     	mov	r2, r11
     f58: e1a01006     	mov	r1, r6
     f5c: e3a00002     	mov	r0, #2
     f60: ebffff0b     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x3d4
     f64: e3560003     	cmp	r6, #3
     f68: e59d3014     	ldr	r3, [sp, #0x14]
     f6c: eefd0ac0     	vcvt.s32.f32	s1, s0
     f70: ee105a90     	vmov	r5, s1
     f74: 0a00002a     	beq	0x1024 <arbhar_shmem_copy+0x2c4> @ imm = #0xa8
     f78: e1a0200b     	mov	r2, r11
     f7c: e1a01006     	mov	r1, r6
     f80: e3a00003     	mov	r0, #3
     f84: ebffff02     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x3f8
     f88: e3560004     	cmp	r6, #4
     f8c: eebd1ac0     	vcvt.s32.f32	s2, s0
     f90: ee114a10     	vmov	r4, s2
     f94: da000023     	ble	0x1028 <arbhar_shmem_copy+0x2c8> @ imm = #0x8c
     f98: e1a01006     	mov	r1, r6
     f9c: e1a0200b     	mov	r2, r11
     fa0: e3a00004     	mov	r0, #4
     fa4: ebfffefa     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x418
     fa8: e0872108     	add	r2, r7, r8, lsl #2
     fac: e0441005     	sub	r1, r4, r5
     fb0: ed927a23     	vldr	s14, [r2, #140]
     fb4: eef81ac7     	vcvt.f32.s32	s3, s14
     fb8: eebd2ac0     	vcvt.s32.f32	s4, s0
     fbc: ee126a10     	vmov	r6, s4
     fc0: e081c006     	add	r12, r1, r6
     fc4: ee02ca90     	vmov	s5, r12
     fc8: eeb83ae2     	vcvt.f32.s32	s6, s5
     fcc: eeb43ae1     	vcmpe.f32	s6, s3
     fd0: eef1fa10     	vmrs	APSR_nzcv, fpscr
     fd4: 8eb03a61     	vmovhi.f32	s6, s3
     fd8: eefd3ac3     	vcvt.s32.f32	s7, s6
     fdc: ee130a90     	vmov	r0, s7
     fe0: eaffff7c     	b	0xdd8 <arbhar_shmem_copy+0x78> @ imm = #-0x210
     fe4: e1a02004     	mov	r2, r4
     fe8: e59f405c     	ldr	r4, [pc, #0x5c]         @ 0x104c <arbhar_shmem_copy+0x2ec>
     fec: e1a01005     	mov	r1, r5
     ff0: e08f0004     	add	r0, pc, r4
     ff4: e3a04000     	mov	r4, #0
     ff8: ebfffec4     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x4f0
     ffc: e1a00004     	mov	r0, r4
    1000: e28dd01c     	add	sp, sp, #28
    1004: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1008: e59f7040     	ldr	r7, [pc, #0x40]         @ 0x1050 <arbhar_shmem_copy+0x2f0>
    100c: e3a04000     	mov	r4, #0
    1010: e08f0007     	add	r0, pc, r7
    1014: ebfffebd     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x50c
    1018: e1a00004     	mov	r0, r4
    101c: e28dd01c     	add	sp, sp, #28
    1020: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1024: e1a04003     	mov	r4, r3
    1028: e1a00004     	mov	r0, r4
    102c: e1a06005     	mov	r6, r5
    1030: eaffff68     	b	0xdd8 <arbhar_shmem_copy+0x78> @ imm = #-0x260
    1034: e320f000     	nop
    1038: 00 00 00 00  	.word	0x00000000
    103c: 00 00 59 40  	.word	0x40590000
    1040: 6c 1d 00 00  	.word	0x00001d6c
    1044: ac 1c 00 00  	.word	0x00001cac
    1048: 9c 1b 00 00  	.word	0x00001b9c
    104c: ac 1b 00 00  	.word	0x00001bac
    1050: 1c 1b 00 00  	.word	0x00001b1c

