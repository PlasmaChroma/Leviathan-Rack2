0000bb18 <_copyScanPositionToRecordingPos>:
    bb18: e92d4010     	push	{r4, lr}
    bb1c: e1a04000     	mov	r4, r0
    bb20: edd47a0b     	vldr	s15, [r4, #44]
    bb24: e24dd008     	sub	sp, sp, #8
    bb28: e59f003c     	ldr	r0, [pc, #0x3c]         @ 0xbb6c <_copyScanPositionToRecordingPos+0x54>  // u32=0x9ab0; f32?=5.54914192e-41
    bb2c: ed947a14     	vldr	s14, [r4, #80]
    bb30: e08f0000     	add	r0, pc, r0
    bb34: eef70ae7     	vcvt.f64.f32	d16, s15
    bb38: eef71ac7     	vcvt.f64.f32	d17, s14
    bb3c: edcd0b00     	vstr	d16, [sp]
    bb40: ec532b31     	vmov	r2, r3, d17
    bb44: ebffe00b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x7fd4  // CALL post
    bb48: e1a00004     	mov	r0, r4
    bb4c: ed940a14     	vldr	s0, [r4, #80]
    bb50: e3a010f9     	mov	r1, #249
    bb54: eefc0ac0     	vcvt.u32.f32	s1, s0
    bb58: ed840a0b     	vstr	s0, [r4, #44]
    bb5c: ee102a90     	vmov	r2, s1
    bb60: e28dd008     	add	sp, sp, #8
    bb64: e8bd4010     	pop	{r4, lr}
    bb68: eaffdfde     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8088  // CALL writeToSharedMem
    bb6c: b0 9a 00 00  	.word	0x00009ab0

