00004650 <led_PlayPosOff>:
    4650: e92d4010     	push	{r4, lr}
    4654: e24dd010     	sub	sp, sp, #16
    4658: e5d0c030     	ldrb	r12, [r0, #0x30]
    465c: e3a03001     	mov	r3, #1
    4660: e3a01000     	mov	r1, #0
    4664: e3a02000     	mov	r2, #0
    4668: e35c0062     	cmp	r12, #98
    466c: e344230c     	movt	r2, #0x430c
    4670: e58d3000     	str	r3, [sp]
    4674: e58d1004     	str	r1, [sp, #0x4]
    4678: e58d3008     	str	r3, [sp, #0x8]
    467c: e58d200c     	str	r2, [sp, #0xc]
    4680: 9a000001     	bls	0x468c <led_PlayPosOff+0x3c> @ imm = #0x4
    4684: e28dd010     	add	sp, sp, #16
    4688: e8bd8010     	pop	{r4, pc}
    468c: e2804a01     	add	r4, r0, #4096
    4690: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x46bc <led_PlayPosOff+0x6c>
    4694: e08f0000     	add	r0, pc, r0
    4698: e5944dac     	ldr	r4, [r4, #0xdac]
    469c: ebfffc21     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xf7c
    46a0: e1a0300d     	mov	r3, sp
    46a4: e3a02002     	mov	r2, #2
    46a8: e1a01000     	mov	r1, r0
    46ac: e1a00004     	mov	r0, r4
    46b0: ebfffd6f     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa44
    46b4: e28dd010     	add	sp, sp, #16
    46b8: e8bd8010     	pop	{r4, pc}
    46bc: 20 04 01 00  	.word	0x00010420

