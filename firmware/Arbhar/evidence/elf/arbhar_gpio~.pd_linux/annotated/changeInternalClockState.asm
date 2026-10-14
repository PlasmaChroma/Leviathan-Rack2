00007ff0 <changeInternalClockState>:
    7ff0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    7ff4: e6ef5071     	uxtb	r5, r1
    7ff8: ed2d8b02     	vpush	{d8}
    7ffc: e1a04000     	mov	r4, r0
    8000: e59f30f4     	ldr	r3, [pc, #0xf4]         @ 0x80fc <changeInternalClockState+0x10c>  // u32=0x1f2e0; f32?=1.78962629e-40
    8004: e08f6003     	add	r6, pc, r3
    8008: e24dd018     	sub	sp, sp, #24
    800c: e5d61011     	ldrb	r1, [r6, #0x11]
    8010: e58d2004     	str	r2, [sp, #0x4]
    8014: e1510005     	cmp	r1, r5
    8018: 1a000014     	bne	0x8070 <changeInternalClockState+0x80> @ imm = #0x50
    801c: e2800a01     	add	r0, r0, #4096
    8020: e5d03df5     	ldrb	r3, [r0, #0xdf5]
    8024: e3530000     	cmp	r3, #0
    8028: e59400fc     	ldr	r0, [r4, #0xfc]
    802c: 0a000009     	beq	0x8058 <changeInternalClockState+0x68> @ imm = #0x24
    8030: eddd0a01     	vldr	s1, [sp, #4]
    8034: eeb88be0     	vcvt.f64.s32	d8, s1
    8038: eeb00b48     	vmov.f64	d0, d8
    803c: ebffee3a     	bl	0x392c <.plt+0x230>     @ imm = #-0x4718  // CALL clock_delay
    8040: e5940100     	ldr	r0, [r4, #0x100]
    8044: eeb00b48     	vmov.f64	d0, d8
    8048: ebffee37     	bl	0x392c <.plt+0x230>     @ imm = #-0x4724  // CALL clock_delay
    804c: e28dd018     	add	sp, sp, #24
    8050: ecbd8b02     	vpop	{d8}
    8054: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    8058: ebffee90     	bl	0x3aa0 <.plt+0x3a4>     @ imm = #-0x45c0  // CALL clock_unset
    805c: e5940100     	ldr	r0, [r4, #0x100]
    8060: ebffee8e     	bl	0x3aa0 <.plt+0x3a4>     @ imm = #-0x45c8  // CALL clock_unset
    8064: e28dd018     	add	sp, sp, #24
    8068: ecbd8b02     	vpop	{d8}
    806c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    8070: e2806a01     	add	r6, r0, #4096
    8074: e1a02005     	mov	r2, r5
    8078: e3a0108c     	mov	r1, #140
    807c: e3a07002     	mov	r7, #2
    8080: e5c65df5     	strb	r5, [r6, #0xdf5]
    8084: e3a08001     	mov	r8, #1
    8088: ebffee96     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x45a8  // CALL writeToSharedMem
    808c: e59f206c     	ldr	r2, [pc, #0x6c]         @ 0x8100 <changeInternalClockState+0x110>  // u32=0xce50; f32?=7.40109797e-41
    8090: e58d7008     	str	r7, [sp, #0x8]
    8094: e08f0002     	add	r0, pc, r2
    8098: ebffeda2     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x4978  // CALL gensym
    809c: e5d63df5     	ldrb	r3, [r6, #0xdf5]
    80a0: e5d4c030     	ldrb	r12, [r4, #0x30]
    80a4: e58d8010     	str	r8, [sp, #0x10]
    80a8: ee073a90     	vmov	s15, r3
    80ac: e35c0062     	cmp	r12, #98
    80b0: eeb80a67     	vcvt.f32.u32	s0, s15
    80b4: ed8d0a05     	vstr	s0, [sp, #20]
    80b8: e58d000c     	str	r0, [sp, #0xc]
    80bc: 9a000003     	bls	0x80d0 <changeInternalClockState+0xe0> @ imm = #0xc
    80c0: e59f103c     	ldr	r1, [pc, #0x3c]         @ 0x8104 <changeInternalClockState+0x114>  // u32=0x1f220; f32?=1.7869358e-40
    80c4: e08f0001     	add	r0, pc, r1
    80c8: e5c05011     	strb	r5, [r0, #0x11]
    80cc: eaffffd4     	b	0x8024 <changeInternalClockState+0x34> @ imm = #-0xb0
    80d0: e59fe030     	ldr	lr, [pc, #0x30]         @ 0x8108 <changeInternalClockState+0x118>  // u32=0xc9dc; f32?=7.24134994e-41
    80d4: e5968db4     	ldr	r8, [r6, #0xdb4]
    80d8: e08f000e     	add	r0, pc, lr
    80dc: ebffed91     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x49bc  // CALL gensym
    80e0: e28d3008     	add	r3, sp, #8
    80e4: e1a02007     	mov	r2, r7
    80e8: e1a01000     	mov	r1, r0
    80ec: e1a00008     	mov	r0, r8
    80f0: ebffeedf     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x4484  // CALL outlet_list
    80f4: e5d63df5     	ldrb	r3, [r6, #0xdf5]
    80f8: eafffff0     	b	0x80c0 <changeInternalClockState+0xd0> @ imm = #-0x40
    80fc: e0 f2 01 00  	.word	0x0001f2e0
    8100: 50 ce 00 00  	.word	0x0000ce50
    8104: 20 f2 01 00  	.word	0x0001f220
    8108: dc c9 00 00  	.word	0x0000c9dc

