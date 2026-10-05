000030dc <arbhar_rec_tilde_stop>:
    30dc: e5903048     	ldr	r3, [r0, #0x48]
    30e0: e92d4010     	push	{r4, lr}
    30e4: e3530001     	cmp	r3, #1
    30e8: e1a04000     	mov	r4, r0
    30ec: 1a00000e     	bne	0x312c <arbhar_rec_tilde_stop+0x50> @ imm = #0x38
    30f0: e5902078     	ldr	r2, [r0, #0x78]
    30f4: e5941288     	ldr	r1, [r4, #0x288]
    30f8: e590c044     	ldr	r12, [r0, #0x44]
    30fc: e1e0e002     	mvn	lr, r2
    3100: e590001c     	ldr	r0, [r0, #0x1c]
    3104: e1a03fae     	lsr	r3, lr, #31
    3108: e3510000     	cmp	r1, #0
    310c: e08c2000     	add	r2, r12, r0
    3110: e59402a0     	ldr	r0, [r4, #0x2a0]
    3114: ee003a10     	vmov	s0, r3
    3118: e5842040     	str	r2, [r4, #0x40]
    311c: d3a02000     	movle	r2, #0
    3120: e584228c     	str	r2, [r4, #0x28c]
    3124: eeb80ac0     	vcvt.f32.s32	s0, s0
    3128: ebfffdad     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x94c
    312c: e594c078     	ldr	r12, [r4, #0x78]
    3130: e35c0001     	cmp	r12, #1
    3134: 18bd8010     	popne	{r4, pc}
    3138: e1c424d8     	ldrd	r2, r3, [r4, #72]
    313c: e5941288     	ldr	r1, [r4, #0x288]
    3140: e5940074     	ldr	r0, [r4, #0x74]
    3144: e1e0e002     	mvn	lr, r2
    3148: e3510000     	cmp	r1, #0
    314c: e1a02fae     	lsr	r2, lr, #31
    3150: e0833000     	add	r3, r3, r0
    3154: e59402a0     	ldr	r0, [r4, #0x2a0]
    3158: ee002a90     	vmov	s1, r2
    315c: e5843070     	str	r3, [r4, #0x70]
    3160: d3a03000     	movle	r3, #0
    3164: e584328c     	str	r3, [r4, #0x28c]
    3168: eeb80ae0     	vcvt.f32.s32	s0, s1
    316c: e8bd4010     	pop	{r4, lr}
    3170: eafffd9b     	b	0x27e4 <.plt+0x2e4>     @ imm = #-0x994

