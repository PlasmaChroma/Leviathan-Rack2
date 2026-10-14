00002b78 <setOutputStream>:
    2b78: e92d4070     	push	{r4, r5, r6, lr}
    2b7c: e0805101     	add	r5, r0, r1, lsl #2
    2b80: e5953368     	ldr	r3, [r5, #0x368]
    2b84: e3530000     	cmp	r3, #0
    2b88: 0a000002     	beq	0x2b98 <setOutputStream+0x20> @ imm = #0x8
    2b8c: e2930000     	adds	r0, r3, #0
    2b90: 13a00001     	movne	r0, #1
    2b94: e8bd8070     	pop	{r4, r5, r6, pc}
    2b98: e59032a4     	ldr	r3, [r0, #0x2a4]
    2b9c: e1a04000     	mov	r4, r0
    2ba0: e5930000     	ldr	r0, [r3]
    2ba4: ebfffeb4     	bl	0x267c <.plt+0x17c>     @ imm = #-0x530  // CALL opendir
    2ba8: e3500000     	cmp	r0, #0
    2bac: 1a000011     	bne	0x2bf8 <setOutputStream+0x80> @ imm = #0x44
    2bb0: ebfffedb     	bl	0x2724 <.plt+0x224>     @ imm = #-0x494  // CALL __errno_location
    2bb4: e5902000     	ldr	r2, [r0]
    2bb8: e3520002     	cmp	r2, #2
    2bbc: 0a00003a     	beq	0x2cac <setOutputStream+0x134> @ imm = #0xe8
    2bc0: e59f10f8     	ldr	r1, [pc, #0xf8]         @ 0x2cc0 <setOutputStream+0x148>  // u32=0x6820; f32?=3.73530119e-41
    2bc4: e1a00004     	mov	r0, r4
    2bc8: e08f1001     	add	r1, pc, r1
    2bcc: ebffff22     	bl	0x285c <.plt+0x35c>     @ imm = #-0x378  // CALL pd_error
    2bd0: e3a02000     	mov	r2, #0
    2bd4: e5852368     	str	r2, [r5, #0x368]
    2bd8: e594c2a4     	ldr	r12, [r4, #0x2a4]
    2bdc: e1a00004     	mov	r0, r4
    2be0: e59fe0dc     	ldr	lr, [pc, #0xdc]         @ 0x2cc4 <setOutputStream+0x14c>  // u32=0x6838; f32?=3.7386643e-41
    2be4: e59c2000     	ldr	r2, [r12]
    2be8: e08f100e     	add	r1, pc, lr
    2bec: ebffff1a     	bl	0x285c <.plt+0x35c>     @ imm = #-0x398  // CALL pd_error
    2bf0: e5953368     	ldr	r3, [r5, #0x368]
    2bf4: eaffffe4     	b	0x2b8c <setOutputStream+0x14> @ imm = #-0x70
    2bf8: ebffff1a     	bl	0x2868 <.plt+0x368>     @ imm = #-0x398  // CALL closedir
    2bfc: e59462a4     	ldr	r6, [r4, #0x2a4]
    2c00: e5960000     	ldr	r0, [r6]
    2c04: ebfffeb4     	bl	0x26dc <.plt+0x1dc>     @ imm = #-0x530  // CALL strlen
    2c08: e5951154     	ldr	r1, [r5, #0x154]
    2c0c: e1a06000     	mov	r6, r0
    2c10: e5910000     	ldr	r0, [r1]
    2c14: ebfffeb0     	bl	0x26dc <.plt+0x1dc>     @ imm = #-0x540  // CALL strlen
    2c18: e0860000     	add	r0, r6, r0
    2c1c: e2806007     	add	r6, r0, #7
    2c20: e1a00006     	mov	r0, r6
    2c24: ebfffe67     	bl	0x25c8 <.plt+0xc8>      @ imm = #-0x664  // CALL getbytes
    2c28: e5952154     	ldr	r2, [r5, #0x154]
    2c2c: e5856308     	str	r6, [r5, #0x308]
    2c30: e59fc090     	ldr	r12, [pc, #0x90]        @ 0x2cc8 <setOutputStream+0x150>  // u32=0x678c; f32?=3.71456197e-41
    2c34: e5923000     	ldr	r3, [r2]
    2c38: e08f100c     	add	r1, pc, r12
    2c3c: e58502a8     	str	r0, [r5, #0x2a8]
    2c40: e59422a4     	ldr	r2, [r4, #0x2a4]
    2c44: e5922000     	ldr	r2, [r2]
    2c48: ebfffee8     	bl	0x27f0 <.plt+0x2f0>     @ imm = #-0x460  // CALL sprintf
    2c4c: e59f3078     	ldr	r3, [pc, #0x78]         @ 0x2ccc <setOutputStream+0x154>  // u32=0x6780; f32?=3.71288041e-41
    2c50: e59502a8     	ldr	r0, [r5, #0x2a8]
    2c54: e08f1003     	add	r1, pc, r3
    2c58: ebfffe42     	bl	0x2568 <.plt+0x68>      @ imm = #-0x6f8  // CALL fopen
    2c5c: e2503000     	subs	r3, r0, #0
    2c60: 15853368     	strne	r3, [r5, #0x368]
    2c64: 1affffc8     	bne	0x2b8c <setOutputStream+0x14> @ imm = #-0xe0
    2c68: e59fe060     	ldr	lr, [pc, #0x60]         @ 0x2cd0 <setOutputStream+0x158>  // u32=0x6790; f32?=3.71512249e-41
    2c6c: e59502a8     	ldr	r0, [r5, #0x2a8]
    2c70: e08f100e     	add	r1, pc, lr
    2c74: ebfffe3b     	bl	0x2568 <.plt+0x68>      @ imm = #-0x714  // CALL fopen
    2c78: e3500000     	cmp	r0, #0
    2c7c: e1a03000     	mov	r3, r0
    2c80: e5850368     	str	r0, [r5, #0x368]
    2c84: 1affffc0     	bne	0x2b8c <setOutputStream+0x14> @ imm = #-0x100
    2c88: ebfffea5     	bl	0x2724 <.plt+0x224>     @ imm = #-0x56c  // CALL __errno_location
    2c8c: e59f1040     	ldr	r1, [pc, #0x40]         @ 0x2cd4 <setOutputStream+0x15c>  // u32=0x6770; f32?=3.71063833e-41
    2c90: e59522a8     	ldr	r2, [r5, #0x2a8]
    2c94: e08f1001     	add	r1, pc, r1
    2c98: e5903000     	ldr	r3, [r0]
    2c9c: e1a00004     	mov	r0, r4
    2ca0: ebfffeed     	bl	0x285c <.plt+0x35c>     @ imm = #-0x44c  // CALL pd_error
    2ca4: e5953368     	ldr	r3, [r5, #0x368]
    2ca8: eaffffb7     	b	0x2b8c <setOutputStream+0x14> @ imm = #-0x124
    2cac: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x2cd8 <setOutputStream+0x160>  // u32=0x6728; f32?=3.70054898e-41
    2cb0: e08f1000     	add	r1, pc, r0
    2cb4: e1a00004     	mov	r0, r4
    2cb8: ebfffee7     	bl	0x285c <.plt+0x35c>     @ imm = #-0x464  // CALL pd_error
    2cbc: eaffffc3     	b	0x2bd0 <setOutputStream+0x58> @ imm = #-0xf4
    2cc0: 20 68 00 00  	.word	0x00006820
    2cc4: 38 68 00 00  	.word	0x00006838
    2cc8: 8c 67 00 00  	.word	0x0000678c
    2ccc: 80 67 00 00  	.word	0x00006780
    2cd0: 90 67 00 00  	.word	0x00006790
    2cd4: 70 67 00 00  	.word	0x00006770
    2cd8: 28 67 00 00  	.word	0x00006728

