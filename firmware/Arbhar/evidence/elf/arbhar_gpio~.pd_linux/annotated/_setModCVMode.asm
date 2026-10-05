00008630 <_setModCVMode>:
    8630: e92d4030     	push	{r4, r5, lr}
    8634: e3a02b02     	mov	r2, #2048
    8638: ed2d8b02     	vpush	{d8}
    863c: e1a04000     	mov	r4, r0
    8640: e3a01032     	mov	r1, #50
    8644: eeb08a40     	vmov.f32	s16, s0
    8648: e24dd00c     	sub	sp, sp, #12
    864c: ebffed25     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4b6c  // CALL writeToSharedMem
    8650: e3a02b02     	mov	r2, #2048
    8654: e3a01033     	mov	r1, #51
    8658: e1a00004     	mov	r0, r4
    865c: ebffed21     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4b7c  // CALL writeToSharedMem
    8660: e3a02b02     	mov	r2, #2048
    8664: e3a01034     	mov	r1, #52
    8668: e1a00004     	mov	r0, r4
    866c: ebffed1d     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4b8c  // CALL writeToSharedMem
    8670: e3a02b02     	mov	r2, #2048
    8674: e3a01035     	mov	r1, #53
    8678: e1a00004     	mov	r0, r4
    867c: ebffed19     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4b9c  // CALL writeToSharedMem
    8680: eefd7ac8     	vcvt.s32.f32	s15, s16
    8684: e59f00d4     	ldr	r0, [pc, #0xd4]         @ 0x8760 <_setModCVMode+0x130>  // u32=0xc940; f32?=7.21948969e-41
    8688: e08f0000     	add	r0, pc, r0
    868c: ee175a90     	vmov	r5, s15
    8690: eef70ac8     	vcvt.f64.f32	d16, s16
    8694: ee171a90     	vmov	r1, s15
    8698: ec532b30     	vmov	r2, r3, d16
    869c: ebffed35     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x4b2c  // CALL post
    86a0: e2453001     	sub	r3, r5, #1
    86a4: e3530003     	cmp	r3, #3
    86a8: 908ff103     	addls	pc, pc, r3, lsl #2
    86ac: ea000025     	b	0x8748 <_setModCVMode+0x118> @ imm = #0x94
    86b0: ea00001e     	b	0x8730 <_setModCVMode+0x100> @ imm = #0x78
    86b4: ea000016     	b	0x8714 <_setModCVMode+0xe4> @ imm = #0x58
    86b8: ea00000e     	b	0x86f8 <_setModCVMode+0xc8> @ imm = #0x38
    86bc: eaffffff     	b	0x86c0 <_setModCVMode+0x90> @ imm = #-0x4
    86c0: e3a03000     	mov	r3, #0
    86c4: e3a00001     	mov	r0, #1
    86c8: e1a02003     	mov	r2, r3
    86cc: e1a05003     	mov	r5, r3
    86d0: e3a01401     	mov	r1, #16777216
    86d4: e58410bc     	str	r1, [r4, #0xbc]
    86d8: e59f4084     	ldr	r4, [pc, #0x84]         @ 0x8764 <_setModCVMode+0x134>  // u32=0xc8f4; f32?=7.20883982e-41
    86dc: e1a01005     	mov	r1, r5
    86e0: e58d0000     	str	r0, [sp]
    86e4: e08f0004     	add	r0, pc, r4
    86e8: ebffed22     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x4b78  // CALL post
    86ec: e28dd00c     	add	sp, sp, #12
    86f0: ecbd8b02     	vpop	{d8}
    86f4: e8bd8030     	pop	{r4, r5, pc}
    86f8: e3a00000     	mov	r0, #0
    86fc: e3a0c801     	mov	r12, #65536
    8700: e1a02000     	mov	r2, r0
    8704: e1a05000     	mov	r5, r0
    8708: e584c0bc     	str	r12, [r4, #0xbc]
    870c: e3a03001     	mov	r3, #1
    8710: eafffff0     	b	0x86d8 <_setModCVMode+0xa8> @ imm = #-0x40
    8714: e3a00000     	mov	r0, #0
    8718: e3a02c01     	mov	r2, #256
    871c: e1a03000     	mov	r3, r0
    8720: e58420bc     	str	r2, [r4, #0xbc]
    8724: e1a05000     	mov	r5, r0
    8728: e3a02001     	mov	r2, #1
    872c: eaffffe9     	b	0x86d8 <_setModCVMode+0xa8> @ imm = #-0x5c
    8730: e3a00000     	mov	r0, #0
    8734: e3a0e001     	mov	lr, #1
    8738: e1a03000     	mov	r3, r0
    873c: e1a02000     	mov	r2, r0
    8740: e584e0bc     	str	lr, [r4, #0xbc]
    8744: eaffffe3     	b	0x86d8 <_setModCVMode+0xa8> @ imm = #-0x74
    8748: e3a00000     	mov	r0, #0
    874c: e58400bc     	str	r0, [r4, #0xbc]
    8750: e1a03000     	mov	r3, r0
    8754: e1a02000     	mov	r2, r0
    8758: e1a05000     	mov	r5, r0
    875c: eaffffdd     	b	0x86d8 <_setModCVMode+0xa8> @ imm = #-0x8c
    8760: 40 c9 00 00  	.word	0x0000c940
    8764: f4 c8 00 00  	.word	0x0000c8f4

