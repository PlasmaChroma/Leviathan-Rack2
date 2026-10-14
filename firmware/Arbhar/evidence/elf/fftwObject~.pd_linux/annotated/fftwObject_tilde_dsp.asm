000008f8 <fftwObject_tilde_dsp>:
     8f8: e1a02000     	mov	r2, r0
     8fc: e5910000     	ldr	r0, [r1]
     900: e1a03001     	mov	r3, r1
     904: e3a01005     	mov	r1, #5
     908: e92d4010     	push	{r4, lr}
     90c: e24dd010     	sub	sp, sp, #16
     910: e9935000     	ldmib	r3, {r12, lr}
     914: e5904000     	ldr	r4, [r0]
     918: e5903004     	ldr	r3, [r0, #0x4]
     91c: e59f0020     	ldr	r0, [pc, #0x20]         @ 0x944 <fftwObject_tilde_dsp+0x4c>  // u32=0x17f4; f32?=8.59276218e-42
     920: e58d4008     	str	r4, [sp, #0x8]
     924: e59ee004     	ldr	lr, [lr, #0x4]
     928: e08f0000     	add	r0, pc, r0
     92c: e58de004     	str	lr, [sp, #0x4]
     930: e59cc004     	ldr	r12, [r12, #0x4]
     934: e58dc000     	str	r12, [sp]
     938: ebffff6b     	bl	0x6ec <.plt+0xd4>       @ imm = #-0x254  // CALL dsp_add
     93c: e28dd010     	add	sp, sp, #16
     940: e8bd8010     	pop	{r4, pc}
     944: f4 17 00 00  	.word	0x000017f4

