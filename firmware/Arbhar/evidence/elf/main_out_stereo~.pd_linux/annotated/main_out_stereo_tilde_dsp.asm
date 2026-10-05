00002424 <main_out_stereo_tilde_dsp>:
    2424: e1a0c001     	mov	r12, r1
    2428: e59f3058     	ldr	r3, [pc, #0x58]         @ 0x2488 <main_out_stereo_tilde_dsp+0x64>  // u32=0x15bc8; f32?=1.24760405e-40
    242c: e92d4070     	push	{r4, r5, r6, lr}
    2430: e08f1003     	add	r1, pc, r3
    2434: e59c4000     	ldr	r4, [r12]
    2438: e24dd010     	sub	sp, sp, #16
    243c: e59c500c     	ldr	r5, [r12, #0xc]
    2440: e1a02000     	mov	r2, r0
    2444: e59fe040     	ldr	lr, [pc, #0x40]         @ 0x248c <main_out_stereo_tilde_dsp+0x68>  // u32=0xb8; f32?=2.57838917e-43
    2448: e1a00001     	mov	r0, r1
    244c: e5946000     	ldr	r6, [r4]
    2450: e3a01006     	mov	r1, #6
    2454: e5943004     	ldr	r3, [r4, #0x4]
    2458: e790000e     	ldr	r0, [r0, lr]
    245c: e58d600c     	str	r6, [sp, #0xc]
    2460: e99c5000     	ldmib	r12, {r12, lr}
    2464: e5954004     	ldr	r4, [r5, #0x4]
    2468: e58d4008     	str	r4, [sp, #0x8]
    246c: e59ee004     	ldr	lr, [lr, #0x4]
    2470: e58de004     	str	lr, [sp, #0x4]
    2474: e59cc004     	ldr	r12, [r12, #0x4]
    2478: e58dc000     	str	r12, [sp]
    247c: ebffff1a     	bl	0x20ec <.plt+0x194>     @ imm = #-0x398  // CALL dsp_add
    2480: e28dd010     	add	sp, sp, #16
    2484: e8bd8070     	pop	{r4, r5, r6, pc}
    2488: c8 5b 01 00  	.word	0x00015bc8
    248c: b8 00 00 00  	.word	0x000000b8

