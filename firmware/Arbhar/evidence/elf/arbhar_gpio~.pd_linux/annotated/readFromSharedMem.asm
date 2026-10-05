00005c8c <readFromSharedMem>:
    5c8c: e2800a01     	add	r0, r0, #4096
    5c90: e5902dc0     	ldr	r2, [r0, #0xdc0]
    5c94: e3520000     	cmp	r2, #0
    5c98: 0a000008     	beq	0x5cc0 <readFromSharedMem+0x34> @ imm = #0x20
    5c9c: eebd0ac0     	vcvt.s32.f32	s0, s0
    5ca0: e5901dc4     	ldr	r1, [r0, #0xdc4]
    5ca4: ee103a10     	vmov	r3, s0
    5ca8: e1c3cfc3     	bic	r12, r3, r3, asr #31
    5cac: e151000c     	cmp	r1, r12
    5cb0: d241c001     	suble	r12, r1, #1
    5cb4: e082010c     	add	r0, r2, r12, lsl #2
    5cb8: ed900a00     	vldr	s0, [r0]
    5cbc: e12fff1e     	bx	lr
    5cc0: ed9f0a00     	vldr	s0, [pc]                @ 0x5cc8 <readFromSharedMem+0x3c>
    5cc4: e12fff1e     	bx	lr
    5cc8: 00 00 00 00  	.word	0x00000000

