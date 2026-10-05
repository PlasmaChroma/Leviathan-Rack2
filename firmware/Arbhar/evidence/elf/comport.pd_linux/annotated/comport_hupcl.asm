00001a84 <comport_hupcl>:
    1a84: eefd7ac0     	vcvt.s32.f32	s15, s0
    1a88: e92d4070     	push	{r4, r5, r6, lr}
    1a8c: e24dd040     	sub	sp, sp, #64
    1a90: e28d5004     	add	r5, sp, #4
    1a94: e1a04000     	mov	r4, r0
    1a98: e1a01005     	mov	r1, r5
    1a9c: e5900020     	ldr	r0, [r0, #0x20]
    1aa0: ee176a90     	vmov	r6, s15
    1aa4: ebfffc54     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0xeb0  // CALL tcgetattr
    1aa8: e3500000     	cmp	r0, #0
    1aac: ba00000e     	blt	0x1aec <comport_hupcl+0x68> @ imm = #0x38
    1ab0: e59d3004     	ldr	r3, [sp, #0x4]
    1ab4: e3560000     	cmp	r6, #0
    1ab8: e3c30b01     	bic	r0, r3, #1024
    1abc: 13800b01     	orrne	r0, r0, #1024
    1ac0: e58d0004     	str	r0, [sp, #0x4]
    1ac4: e1a02005     	mov	r2, r5
    1ac8: e5940020     	ldr	r0, [r4, #0x20]
    1acc: e3a01000     	mov	r1, #0
    1ad0: ebfffbf5     	bl	0xaac <.plt+0xa4>       @ imm = #-0x102c  // CALL tcsetattr
    1ad4: e3500000     	cmp	r0, #0
    1ad8: a2844a01     	addge	r4, r4, #4096
    1adc: a58460bc     	strge	r6, [r4, #0xbc]
    1ae0: ba000005     	blt	0x1afc <comport_hupcl+0x78> @ imm = #0x14
    1ae4: e28dd040     	add	sp, sp, #64
    1ae8: e8bd8070     	pop	{r4, r5, r6, pc}
    1aec: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x1b10 <comport_hupcl+0x8c>  // u32=0x2aa4; f32?=1.5296574e-41
    1af0: e08f0002     	add	r0, pc, r2
    1af4: ebfffbdd     	bl	0xa70 <.plt+0x68>       @ imm = #-0x108c  // CALL perror
    1af8: eafffff9     	b	0x1ae4 <comport_hupcl+0x60> @ imm = #-0x1c
    1afc: e59f1010     	ldr	r1, [pc, #0x10]         @ 0x1b14 <comport_hupcl+0x90>  // u32=0x2aa4; f32?=1.5296574e-41
    1b00: e1a00004     	mov	r0, r4
    1b04: e08f1001     	add	r1, pc, r1
    1b08: ebfffc35     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xf2c  // CALL pd_error
    1b0c: eafffff4     	b	0x1ae4 <comport_hupcl+0x60> @ imm = #-0x30
    1b10: a4 2a 00 00  	.word	0x00002aa4
    1b14: a4 2a 00 00  	.word	0x00002aa4

