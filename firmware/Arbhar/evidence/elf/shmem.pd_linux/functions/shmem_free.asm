00000a40 <shmem_free>:
     a40: e92d4030     	push	{r4, r5, lr}
     a44: e1a04000     	mov	r4, r0
     a48: e5900024     	ldr	r0, [r0, #0x24]
     a4c: e24dd05c     	sub	sp, sp, #92
     a50: e3500000     	cmp	r0, #0
     a54: 0a000002     	beq	0xa64 <shmem_free+0x24> @ imm = #0x8
     a58: ebffff70     	bl	0x820 <.plt+0xb0>       @ imm = #-0x240
     a5c: e3700001     	cmn	r0, #1
     a60: 0a00001a     	beq	0xad0 <shmem_free+0x90> @ imm = #0x68
     a64: e594501c     	ldr	r5, [r4, #0x1c]
     a68: e3a03000     	mov	r3, #0
     a6c: e5843024     	str	r3, [r4, #0x24]
     a70: e1550003     	cmp	r5, r3
     a74: ca000001     	bgt	0xa80 <shmem_free+0x40> @ imm = #0x4
     a78: e28dd05c     	add	sp, sp, #92
     a7c: e8bd8030     	pop	{r4, r5, pc}
     a80: e28d4004     	add	r4, sp, #4
     a84: e3a01002     	mov	r1, #2
     a88: e1a00005     	mov	r0, r5
     a8c: e1a02004     	mov	r2, r4
     a90: ebffff4d     	bl	0x7cc <.plt+0x5c>       @ imm = #-0x2cc
     a94: e3700001     	cmn	r0, #1
     a98: 0afffff6     	beq	0xa78 <shmem_free+0x38> @ imm = #-0x28
     a9c: e59d104c     	ldr	r1, [sp, #0x4c]
     aa0: e3510000     	cmp	r1, #0
     aa4: 1afffff3     	bne	0xa78 <shmem_free+0x38> @ imm = #-0x34
     aa8: e1a02004     	mov	r2, r4
     aac: e1a00005     	mov	r0, r5
     ab0: ebffff45     	bl	0x7cc <.plt+0x5c>       @ imm = #-0x2ec
     ab4: e3700001     	cmn	r0, #1
     ab8: 1affffee     	bne	0xa78 <shmem_free+0x38> @ imm = #-0x48
     abc: e59f2020     	ldr	r2, [pc, #0x20]         @ 0xae4 <shmem_free+0xa4>
     ac0: e1a01005     	mov	r1, r5
     ac4: e08f0002     	add	r0, pc, r2
     ac8: ebffff42     	bl	0x7d8 <.plt+0x68>       @ imm = #-0x2f8
     acc: eaffffe9     	b	0xa78 <shmem_free+0x38> @ imm = #-0x5c
     ad0: e59f0010     	ldr	r0, [pc, #0x10]         @ 0xae8 <shmem_free+0xa8>
     ad4: e5941024     	ldr	r1, [r4, #0x24]
     ad8: e08f0000     	add	r0, pc, r0
     adc: ebffff3d     	bl	0x7d8 <.plt+0x68>       @ imm = #-0x30c
     ae0: eaffffdf     	b	0xa64 <shmem_free+0x24> @ imm = #-0x84
     ae4: 70 0d 00 00  	.word	0x00000d70
     ae8: 48 0d 00 00  	.word	0x00000d48

