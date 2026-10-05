000008b8 <terminal_itoa>:
     8b8: e3500000     	cmp	r0, #0
     8bc: e0201fc0     	eor	r1, r0, r0, asr #31
     8c0: e92d4070     	push	{r4, r5, r6, lr}
     8c4: e0411fc0     	sub	r1, r1, r0, asr #31
     8c8: 0a000030     	beq	0x990 <terminal_itoa+0xd8> @ imm = #0xc0
     8cc: e59fc0c4     	ldr	r12, [pc, #0xc4]        @ 0x998 <terminal_itoa+0xe0>  // u32=0x1079c; f32?=9.45652256e-41
     8d0: e30c5ccd     	movw	r5, #0xcccd
     8d4: e3a04000     	mov	r4, #0
     8d8: e34c5ccc     	movt	r5, #0xcccc
     8dc: e08f200c     	add	r2, pc, r12
     8e0: e3a0600a     	mov	r6, #10
     8e4: e242c001     	sub	r12, r2, #1
     8e8: ea000000     	b	0x8f0 <terminal_itoa+0x38> @ imm = #0x0
     8ec: e1a0400e     	mov	r4, lr
     8f0: e0832195     	umull	r2, r3, r5, r1
     8f4: e284e001     	add	lr, r4, #1
     8f8: e1a031a3     	lsr	r3, r3, #3
     8fc: e0621396     	mls	r2, r6, r3, r1
     900: e2531000     	subs	r1, r3, #0
     904: e2823030     	add	r3, r2, #48
     908: e5ec3001     	strb	r3, [r12, #0x1]!
     90c: 1afffff6     	bne	0x8ec <terminal_itoa+0x34> @ imm = #-0x28
     910: e3500000     	cmp	r0, #0
     914: ba000017     	blt	0x978 <terminal_itoa+0xc0> @ imm = #0x5c
     918: e59f407c     	ldr	r4, [pc, #0x7c]         @ 0x99c <terminal_itoa+0xe4>  // u32=0x1075c; f32?=9.44755425e-41
     91c: e08f5004     	add	r5, pc, r4
     920: e3a04000     	mov	r4, #0
     924: e1a00005     	mov	r0, r5
     928: e7c5400e     	strb	r4, [r5, lr]
     92c: ebffff40     	bl	0x634 <.plt+0x74>       @ imm = #-0x300  // CALL strlen
     930: e2402001     	sub	r2, r0, #1
     934: e1520004     	cmp	r2, r4
     938: da00000b     	ble	0x96c <terminal_itoa+0xb4> @ imm = #0x2c
     93c: e245c001     	sub	r12, r5, #1
     940: e0801005     	add	r1, r0, r5
     944: e1a03004     	mov	r3, r4
     948: e2833001     	add	r3, r3, #1
     94c: e5fce001     	ldrb	lr, [r12, #0x1]!
     950: e5714001     	ldrb	r4, [r1, #-0x1]!
     954: e1e02003     	mvn	r2, r3
     958: e0822000     	add	r2, r2, r0
     95c: e1530002     	cmp	r3, r2
     960: e5cc4000     	strb	r4, [r12]
     964: e5c1e000     	strb	lr, [r1]
     968: bafffff6     	blt	0x948 <terminal_itoa+0x90> @ imm = #-0x28
     96c: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x9a0 <terminal_itoa+0xe8>  // u32=0x10708; f32?=9.43578334e-41
     970: e08f0000     	add	r0, pc, r0
     974: e8bd8070     	pop	{r4, r5, r6, pc}
     978: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x9a4 <terminal_itoa+0xec>  // u32=0x106f8; f32?=9.43354126e-41
     97c: e3a0c02d     	mov	r12, #45
     980: e08f5000     	add	r5, pc, r0
     984: e7c5c00e     	strb	r12, [r5, lr]
     988: e284e002     	add	lr, r4, #2
     98c: eaffffe1     	b	0x918 <terminal_itoa+0x60> @ imm = #-0x7c
     990: e1a0e000     	mov	lr, r0
     994: eaffffdf     	b	0x918 <terminal_itoa+0x60> @ imm = #-0x84
     998: 9c 07 01 00  	.word	0x0001079c
     99c: 5c 07 01 00  	.word	0x0001075c
     9a0: 08 07 01 00  	.word	0x00010708
     9a4: f8 06 01 00  	.word	0x000106f8

