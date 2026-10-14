0000b158 <_shareRecordingStateToShMem>:
    b158: edd07a0b     	vldr	s15, [r0, #44]
    b15c: e3a010f9     	mov	r1, #249
    b160: e92d4070     	push	{r4, r5, r6, lr}
    b164: e1a04000     	mov	r4, r0
    b168: eebc0ae7     	vcvt.u32.f32	s0, s15
    b16c: ee102a10     	vmov	r2, s0
    b170: ebffe25c     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7690
    b174: e5d42024     	ldrb	r2, [r4, #0x24]
    b178: e1a00004     	mov	r0, r4
    b17c: e3a01021     	mov	r1, #33
    b180: ebffe258     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x76a0
    b184: e5d43024     	ldrb	r3, [r4, #0x24]
    b188: e3530000     	cmp	r3, #0
    b18c: 08bd8070     	popeq	{r4, r5, r6, pc}
    b190: e5d45037     	ldrb	r5, [r4, #0x37]
    b194: e1a00004     	mov	r0, r4
    b198: e285607c     	add	r6, r5, #124
    b19c: ee006a90     	vmov	s1, r6
    b1a0: eeb80ae0     	vcvt.f32.s32	s0, s1
    b1a4: ebffe18f     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x79c4
    b1a8: e6ef1076     	uxtb	r1, r6
    b1ac: e1a00004     	mov	r0, r4
    b1b0: e8bd4070     	pop	{r4, r5, r6, lr}
    b1b4: eebd1ac0     	vcvt.s32.f32	s2, s0
    b1b8: ee112a10     	vmov	r2, s2
    b1bc: e3520001     	cmp	r2, #1
    b1c0: d2822001     	addle	r2, r2, #1
    b1c4: eaffe247     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x76e4

