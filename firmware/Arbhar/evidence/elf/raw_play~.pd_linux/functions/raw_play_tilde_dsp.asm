000025c4 <raw_play_tilde_dsp>:
    25c4: e1a0c001     	mov	r12, r1
    25c8: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x2620 <raw_play_tilde_dsp+0x5c>
    25cc: e92d4070     	push	{r4, r5, r6, lr}
    25d0: e08f1003     	add	r1, pc, r3
    25d4: e59c4000     	ldr	r4, [r12]
    25d8: e24dd010     	sub	sp, sp, #16
    25dc: e59c5008     	ldr	r5, [r12, #0x8]
    25e0: e1a02000     	mov	r2, r0
    25e4: e59fe038     	ldr	lr, [pc, #0x38]         @ 0x2624 <raw_play_tilde_dsp+0x60>
    25e8: e1a00001     	mov	r0, r1
    25ec: e5946000     	ldr	r6, [r4]
    25f0: e3a01005     	mov	r1, #5
    25f4: e5943004     	ldr	r3, [r4, #0x4]
    25f8: e790000e     	ldr	r0, [r0, lr]
    25fc: e59cc004     	ldr	r12, [r12, #0x4]
    2600: e58d6008     	str	r6, [sp, #0x8]
    2604: e5954004     	ldr	r4, [r5, #0x4]
    2608: e58d4004     	str	r4, [sp, #0x4]
    260c: e59ce004     	ldr	lr, [r12, #0x4]
    2610: e58de000     	str	lr, [sp]
    2614: ebfffec5     	bl	0x2130 <.plt+0x1a0>     @ imm = #-0x4ec
    2618: e28dd010     	add	sp, sp, #16
    261c: e8bd8070     	pop	{r4, r5, r6, pc}
    2620: 28 5a 01 00  	.word	0x00015a28
    2624: d0 00 00 00  	.word	0x000000d0

