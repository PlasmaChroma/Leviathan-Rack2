00005fe8 <timerEndPrint>:
    5fe8: e92d4070     	push	{r4, r5, r6, lr}
    5fec: e3a01000     	mov	r1, #0
    5ff0: e59f4068     	ldr	r4, [pc, #0x68]         @ 0x6060 <timerEndPrint+0x78>
    5ff4: e59f3068     	ldr	r3, [pc, #0x68]         @ 0x6064 <timerEndPrint+0x7c>
    5ff8: e08f4004     	add	r4, pc, r4
    5ffc: e7945003     	ldr	r5, [r4, r3]
    6000: e1a00005     	mov	r0, r5
    6004: ebfff145     	bl	0x2520 <.plt+0x140>     @ imm = #-0x3aec
    6008: e59f1058     	ldr	r1, [pc, #0x58]         @ 0x6068 <timerEndPrint+0x80>
    600c: e59f0058     	ldr	r0, [pc, #0x58]         @ 0x606c <timerEndPrint+0x84>
    6010: e1c520d0     	ldrd	r2, r3, [r5]
    6014: e794c001     	ldr	r12, [r4, r1]
    6018: e3041240     	movw	r1, #0x4240
    601c: e7944000     	ldr	r4, [r4, r0]
    6020: e340100f     	movt	r1, #0xf
    6024: e59c6004     	ldr	r6, [r12, #0x4]
    6028: e59c5000     	ldr	r5, [r12]
    602c: e0433006     	sub	r3, r3, r6
    6030: e59fc038     	ldr	r12, [pc, #0x38]        @ 0x6070 <timerEndPrint+0x88>
    6034: e3530000     	cmp	r3, #0
    6038: e0422005     	sub	r2, r2, r5
    603c: e1c420f0     	strd	r2, r3, [r4]
    6040: b283393d     	addlt	r3, r3, #999424
    6044: b2422001     	sublt	r2, r2, #1
    6048: b2833d09     	addlt	r3, r3, #576
    604c: e08f000c     	add	r0, pc, r12
    6050: b1c420f0     	strdlt	r2, r3, [r4]
    6054: e0213291     	mla	r1, r1, r2, r3
    6058: e8bd4070     	pop	{r4, r5, r6, lr}
    605c: eafff16e     	b	0x261c <.plt+0x23c>     @ imm = #-0x3a48
    6060: 00 80 01 00  	.word	0x00018000
    6064: 24 01 00 00  	.word	0x00000124
    6068: 2c 01 00 00  	.word	0x0000012c
    606c: 34 01 00 00  	.word	0x00000134
    6070: 4c 6c 00 00  	.word	0x00006c4c

