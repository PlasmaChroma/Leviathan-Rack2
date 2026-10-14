0000401c <readFromSharedMem>:
    401c: eefd7ac0     	vcvt.s32.f32	s15, s0
    4020: e0801101     	add	r1, r0, r1, lsl #2
    4024: e591321c     	ldr	r3, [r1, #0x21c]
    4028: e59101e8     	ldr	r0, [r1, #0x1e8]
    402c: ee172a90     	vmov	r2, s15
    4030: e1530002     	cmp	r3, r2
    4034: d2432001     	suble	r2, r3, #1
    4038: e080c102     	add	r12, r0, r2, lsl #2
    403c: ed9c0a00     	vldr	s0, [r12]
    4040: e12fff1e     	bx	lr

