00004848 <arbhar_rec_tilde_bang>:
    4848: e92d4070     	push	{r4, r5, r6, lr}
    484c: e3a01000     	mov	r1, #0
    4850: ed9f0a42     	vldr	s0, [pc, #264]          @ 0x4960 <arbhar_rec_tilde_bang+0x118>
    4854: e1a04000     	mov	r4, r0
    4858: ebfff745     	bl	0x2574 <.plt+0x74>      @ imm = #-0x22ec
    485c: e1a00004     	mov	r0, r4
    4860: e3a01000     	mov	r1, #0
    4864: eefd7ac0     	vcvt.s32.f32	s15, s0
    4868: ed9f0a3d     	vldr	s0, [pc, #244]          @ 0x4964 <arbhar_rec_tilde_bang+0x11c>
    486c: ee175a90     	vmov	r5, s15
    4870: ebfff73f     	bl	0x2574 <.plt+0x74>      @ imm = #-0x2304
    4874: e3a03c53     	mov	r3, #21248
    4878: e3403007     	movt	r3, #0x7
    487c: e5942048     	ldr	r2, [r4, #0x48]
    4880: e1550003     	cmp	r5, r3
    4884: c3a05000     	movgt	r5, #0
    4888: e3520001     	cmp	r2, #1
    488c: eebd0ac0     	vcvt.s32.f32	s0, s0
    4890: ee100a10     	vmov	r0, s0
    4894: ed840aa2     	vstr	s0, [r4, #648]
    4898: 0a00000e     	beq	0x48d8 <arbhar_rec_tilde_bang+0x90> @ imm = #0x38
    489c: e3500000     	cmp	r0, #0
    48a0: e3a01001     	mov	r1, #1
    48a4: e594e288     	ldr	lr, [r4, #0x288]
    48a8: 11a00005     	movne	r0, r5
    48ac: e5840080     	str	r0, [r4, #0x80]
    48b0: e5940078     	ldr	r0, [r4, #0x78]
    48b4: e5841284     	str	r1, [r4, #0x284]
    48b8: e3500001     	cmp	r0, #1
    48bc: 0a000017     	beq	0x4920 <arbhar_rec_tilde_bang+0xd8> @ imm = #0x5c
    48c0: e35e0000     	cmp	lr, #0
    48c4: e3a03001     	mov	r3, #1
    48c8: e5843284     	str	r3, [r4, #0x284]
    48cc: 11a0e005     	movne	lr, r5
    48d0: e584e080     	str	lr, [r4, #0x80]
    48d4: e8bd8070     	pop	{r4, r5, r6, pc}
    48d8: e594c078     	ldr	r12, [r4, #0x78]
    48dc: e3500000     	cmp	r0, #0
    48e0: e594e044     	ldr	lr, [r4, #0x44]
    48e4: e594301c     	ldr	r3, [r4, #0x1c]
    48e8: e1e0200c     	mvn	r2, r12
    48ec: e59402a0     	ldr	r0, [r4, #0x2a0]
    48f0: e1a01fa2     	lsr	r1, r2, #31
    48f4: e08ec003     	add	r12, lr, r3
    48f8: e584c040     	str	r12, [r4, #0x40]
    48fc: d3a0c000     	movle	r12, #0
    4900: ee001a90     	vmov	s1, r1
    4904: e584c28c     	str	r12, [r4, #0x28c]
    4908: eeb80ae0     	vcvt.f32.s32	s0, s1
    490c: ebfff7b4     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x2130
    4910: e5940078     	ldr	r0, [r4, #0x78]
    4914: e594e288     	ldr	lr, [r4, #0x288]
    4918: e3500001     	cmp	r0, #1
    491c: 1affffe7     	bne	0x48c0 <arbhar_rec_tilde_bang+0x78> @ imm = #-0x64
    4920: e5945048     	ldr	r5, [r4, #0x48]
    4924: e35e0000     	cmp	lr, #0
    4928: e594604c     	ldr	r6, [r4, #0x4c]
    492c: e5942074     	ldr	r2, [r4, #0x74]
    4930: e1e01005     	mvn	r1, r5
    4934: e59402a0     	ldr	r0, [r4, #0x2a0]
    4938: e1a0cfa1     	lsr	r12, r1, #31
    493c: e086e002     	add	lr, r6, r2
    4940: d3a06000     	movle	r6, #0
    4944: e584e070     	str	lr, [r4, #0x70]
    4948: ee01ca10     	vmov	s2, r12
    494c: c1a0600e     	movgt	r6, lr
    4950: e584628c     	str	r6, [r4, #0x28c]
    4954: eeb80ac1     	vcvt.f32.s32	s0, s2
    4958: e8bd4070     	pop	{r4, r5, r6, lr}
    495c: eafff7a0     	b	0x27e4 <.plt+0x2e4>     @ imm = #-0x2180
    4960: 00 00 79 43  	.word	0x43790000
    4964: 00 00 08 42  	.word	0x42080000

