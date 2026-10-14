00000aec <shmem_allocate>:
     aec: eefd7ac0     	vcvt.s32.f32	s15, s0
     af0: e92d4030     	push	{r4, r5, lr}
     af4: e24dd05c     	sub	sp, sp, #92
     af8: ee175a90     	vmov	r5, s15
     afc: e3550000     	cmp	r5, #0
     b00: da000041     	ble	0xc0c <shmem_allocate+0x120> @ imm = #0x104
     b04: eefd0ae0     	vcvt.s32.f32	s1, s1
     b08: e1a04000     	mov	r4, r0
     b0c: e5900024     	ldr	r0, [r0, #0x24]
     b10: e3500000     	cmp	r0, #0
     b14: edc40a0a     	vstr	s1, [r4, #40]
     b18: 0a000002     	beq	0xb28 <shmem_allocate+0x3c> @ imm = #0x8
     b1c: ebffff3f     	bl	0x820 <.plt+0xb0>       @ imm = #-0x304
     b20: e3a03000     	mov	r3, #0
     b24: e5843024     	str	r3, [r4, #0x24]
     b28: e594001c     	ldr	r0, [r4, #0x1c]
     b2c: e3700001     	cmn	r0, #1
     b30: 1a000019     	bne	0xb9c <shmem_allocate+0xb0> @ imm = #0x64
     b34: e5941028     	ldr	r1, [r4, #0x28]
     b38: e3002386     	movw	r2, #0x386
     b3c: e1a00005     	mov	r0, r5
     b40: e1a01101     	lsl	r1, r1, #2
     b44: ebffff17     	bl	0x7a8 <.plt+0x38>       @ imm = #-0x3a4
     b48: e3700001     	cmn	r0, #1
     b4c: e584001c     	str	r0, [r4, #0x1c]
     b50: 0a000017     	beq	0xbb4 <shmem_allocate+0xc8> @ imm = #0x5c
     b54: e3a02000     	mov	r2, #0
     b58: e1a01002     	mov	r1, r2
     b5c: ebffff17     	bl	0x7c0 <.plt+0x50>       @ imm = #-0x3a4
     b60: e28d2004     	add	r2, sp, #4
     b64: e3a01002     	mov	r1, #2
     b68: e5840024     	str	r0, [r4, #0x24]
     b6c: e594001c     	ldr	r0, [r4, #0x1c]
     b70: ebffff15     	bl	0x7cc <.plt+0x5c>       @ imm = #-0x3ac
     b74: e5942028     	ldr	r2, [r4, #0x28]
     b78: e59d0028     	ldr	r0, [sp, #0x28]
     b7c: e1500102     	cmp	r0, r2, lsl #2
     b80: ba000016     	blt	0xbe0 <shmem_allocate+0xf4> @ imm = #0x58
     b84: ee005a10     	vmov	s0, r5
     b88: e5940020     	ldr	r0, [r4, #0x20]
     b8c: eeb80ac0     	vcvt.f32.s32	s0, s0
     b90: ebffff31     	bl	0x85c <.plt+0xec>       @ imm = #-0x33c
     b94: e28dd05c     	add	sp, sp, #92
     b98: e8bd8030     	pop	{r4, r5, pc}
     b9c: e3a02000     	mov	r2, #0
     ba0: e1a01002     	mov	r1, r2
     ba4: ebffff08     	bl	0x7cc <.plt+0x5c>       @ imm = #-0x3e0
     ba8: e3e00000     	mvn	r0, #0
     bac: e584001c     	str	r0, [r4, #0x1c]
     bb0: eaffffdf     	b	0xb34 <shmem_allocate+0x48> @ imm = #-0x84
     bb4: e59fc064     	ldr	r12, [pc, #0x64]        @ 0xc20 <shmem_allocate+0x134>
     bb8: e1a01005     	mov	r1, r5
     bbc: e5942028     	ldr	r2, [r4, #0x28]
     bc0: e08f000c     	add	r0, pc, r12
     bc4: ebffff03     	bl	0x7d8 <.plt+0x68>       @ imm = #-0x3f4
     bc8: e3a03000     	mov	r3, #0
     bcc: eebf0a00     	vmov.f32	s0, #-1.000000e+00
     bd0: e5843028     	str	r3, [r4, #0x28]
     bd4: e5940020     	ldr	r0, [r4, #0x20]
     bd8: ebffff1f     	bl	0x85c <.plt+0xec>       @ imm = #-0x384
     bdc: eaffffe8     	b	0xb84 <shmem_allocate+0x98> @ imm = #-0x60
     be0: e59fe03c     	ldr	lr, [pc, #0x3c]         @ 0xc24 <shmem_allocate+0x138>
     be4: e1a01005     	mov	r1, r5
     be8: e08f000e     	add	r0, pc, lr
     bec: ebfffef9     	bl	0x7d8 <.plt+0x68>       @ imm = #-0x41c
     bf0: e3a01000     	mov	r1, #0
     bf4: eebf0a00     	vmov.f32	s0, #-1.000000e+00
     bf8: e5841028     	str	r1, [r4, #0x28]
     bfc: e5841024     	str	r1, [r4, #0x24]
     c00: e5940020     	ldr	r0, [r4, #0x20]
     c04: ebffff14     	bl	0x85c <.plt+0xec>       @ imm = #-0x3b0
     c08: eaffffdd     	b	0xb84 <shmem_allocate+0x98> @ imm = #-0x8c
     c0c: e59f5014     	ldr	r5, [pc, #0x14]         @ 0xc28 <shmem_allocate+0x13c>
     c10: e08f0005     	add	r0, pc, r5
     c14: ebfffeef     	bl	0x7d8 <.plt+0x68>       @ imm = #-0x444
     c18: e28dd05c     	add	sp, sp, #92
     c1c: e8bd8030     	pop	{r4, r5, pc}
     c20: a4 0c 00 00  	.word	0x00000ca4
     c24: 7c 0c 00 00  	.word	0x00000c7c
     c28: 40 0c 00 00  	.word	0x00000c40

