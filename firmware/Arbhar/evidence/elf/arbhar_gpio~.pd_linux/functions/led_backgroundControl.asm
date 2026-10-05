00006cbc <led_backgroundControl>:
    6cbc: e92d4010     	push	{r4, lr}
    6cc0: e3520000     	cmp	r2, #0
    6cc4: e5d0303c     	ldrb	r3, [r0, #0x3c]
    6cc8: 10801001     	addne	r1, r0, r1
    6ccc: e1a04000     	mov	r4, r0
    6cd0: e24dd028     	sub	sp, sp, #40
    6cd4: e2432002     	sub	r2, r3, #2
    6cd8: 15d11105     	ldrbne	r1, [r1, #0x105]
    6cdc: e16f0f12     	clz	r0, r2
    6ce0: e1a0c2a0     	lsr	r12, r0, #5
    6ce4: e3530003     	cmp	r3, #3
    6ce8: 838cc001     	orrhi	r12, r12, #1
    6cec: e35c0000     	cmp	r12, #0
    6cf0: 1d9f7a32     	vldrne	s14, [pc, #200]         @ 0x6dc0 <led_backgroundControl+0x104>
    6cf4: 0a000016     	beq	0x6d54 <led_backgroundControl+0x98> @ imm = #0x58
    6cf8: e2432001     	sub	r2, r3, #1
    6cfc: e5d40030     	ldrb	r0, [r4, #0x30]
    6d00: e3520001     	cmp	r2, #1
    6d04: e3a0c001     	mov	r12, #1
    6d08: e3a03000     	mov	r3, #0
    6d0c: ed8d7a07     	vstr	s14, [sp, #28]
    6d10: e3443301     	movt	r3, #0x4301
    6d14: e58dc008     	str	r12, [sp, #0x8]
    6d18: e58d3014     	str	r3, [sp, #0x14]
    6d1c: 8e071a90     	vmovhi	s15, r1
    6d20: e58dc010     	str	r12, [sp, #0x10]
    6d24: 9ddf7a26     	vldrls	s15, [pc, #152]         @ 0x6dc4 <led_backgroundControl+0x108>
    6d28: e3a01000     	mov	r1, #0
    6d2c: e58dc018     	str	r12, [sp, #0x18]
    6d30: e3441302     	movt	r1, #0x4302
    6d34: e58dc020     	str	r12, [sp, #0x20]
    6d38: 8ef87a67     	vcvthi.f32.u32	s15, s15
    6d3c: e3500062     	cmp	r0, #98
    6d40: e58d1024     	str	r1, [sp, #0x24]
    6d44: edcd7a03     	vstr	s15, [sp, #12]
    6d48: 9a000010     	bls	0x6d90 <led_backgroundControl+0xd4> @ imm = #0x40
    6d4c: e28dd028     	add	sp, sp, #40
    6d50: e8bd8010     	pop	{r4, pc}
    6d54: e3530000     	cmp	r3, #0
    6d58: 1d9f7a1a     	vldrne	s14, [pc, #104]         @ 0x6dc8 <led_backgroundControl+0x10c>
    6d5c: 1affffe5     	bne	0x6cf8 <led_backgroundControl+0x3c> @ imm = #-0x6c
    6d60: e1a00004     	mov	r0, r4
    6d64: e58d1004     	str	r1, [sp, #0x4]
    6d68: ed9f0a17     	vldr	s0, [pc, #92]           @ 0x6dcc <led_backgroundControl+0x110>
    6d6c: ebfff29d     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x358c
    6d70: e5d4303c     	ldrb	r3, [r4, #0x3c]
    6d74: eefc7ac0     	vcvt.u32.f32	s15, s0
    6d78: edcd7a00     	vstr	s15, [sp]
    6d7c: e5dd1000     	ldrb	r1, [sp]
    6d80: ee071a10     	vmov	s14, r1
    6d84: e59d1004     	ldr	r1, [sp, #0x4]
    6d88: eeb87a47     	vcvt.f32.u32	s14, s14
    6d8c: eaffffd9     	b	0x6cf8 <led_backgroundControl+0x3c> @ imm = #-0x9c
    6d90: e284ea01     	add	lr, r4, #4096
    6d94: e59f4034     	ldr	r4, [pc, #0x34]         @ 0x6dd0 <led_backgroundControl+0x114>
    6d98: e08f0004     	add	r0, pc, r4
    6d9c: e59e4dac     	ldr	r4, [lr, #0xdac]
    6da0: ebfff260     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x3680
    6da4: e28d3008     	add	r3, sp, #8
    6da8: e3a02004     	mov	r2, #4
    6dac: e1a01000     	mov	r1, r0
    6db0: e1a00004     	mov	r0, r4
    6db4: ebfff3ae     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3148
    6db8: e28dd028     	add	sp, sp, #40
    6dbc: e8bd8010     	pop	{r4, pc}
    6dc0: 00 00 fe 42  	.word	0x42fe0000
    6dc4: 00 00 08 42  	.word	0x42080000
    6dc8: 00 00 00 00  	.word	0x00000000
    6dcc: 00 00 e8 42  	.word	0x42e80000
    6dd0: 1c dd 00 00  	.word	0x0000dd1c

