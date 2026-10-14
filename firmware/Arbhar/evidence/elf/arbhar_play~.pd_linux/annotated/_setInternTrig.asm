00007a58 <_setInternTrig>:
    7a58: eeb50ac0     	vcmpe.f32	s0, #0
    7a5c: e92d4010     	push	{r4, lr}
    7a60: e1a04000     	mov	r4, r0
    7a64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7a68: c3a03001     	movgt	r3, #1
    7a6c: d3a03000     	movle	r3, #0
    7a70: e5803030     	str	r3, [r0, #0x30]
    7a74: da000007     	ble	0x7a98 <_setInternTrig+0x40> @ imm = #0x1c
    7a78: ebfffc6d     	bl	0x6c34 <play_next>      @ imm = #-0xe4c
    7a7c: e5940028     	ldr	r0, [r4, #0x28]
    7a80: e2844a02     	add	r4, r4, #8192
    7a84: ed9f0b0b     	vldr	d0, [pc, #44]           @ 0x7ab8 <_setInternTrig+0x60>  // f64=0
    7a88: ebffeafb     	bl	0x267c <.plt+0x29c>     @ imm = #-0x5414  // CALL clock_set
    7a8c: e3a03000     	mov	r3, #0
    7a90: e5843854     	str	r3, [r4, #0x854]
    7a94: e8bd8010     	pop	{r4, pc}
    7a98: e5900028     	ldr	r0, [r0, #0x28]
    7a9c: ebffeabd     	bl	0x2598 <.plt+0x1b8>     @ imm = #-0x550c  // CALL clock_unset
    7aa0: ed9f1a06     	vldr	s2, [pc, #24]           @ 0x7ac0 <_setInternTrig+0x68>  // f32=0
    7aa4: e1a00004     	mov	r0, r4
    7aa8: e8bd4010     	pop	{r4, lr}
    7aac: eddf0a04     	vldr	s1, [pc, #16]           @ 0x7ac4 <_setInternTrig+0x6c>  // f32=112
    7ab0: eeb00a41     	vmov.f32	s0, s2
    7ab4: eaffea84     	b	0x24cc <.plt+0xec>      @ imm = #-0x55f0  // CALL _memWrite
    7ab8: 00 00 00 00  	.word	0x00000000
    7abc: 00 00 00 00  	.word	0x00000000
    7ac0: 00 00 00 00  	.word	0x00000000
    7ac4: 00 00 e0 42  	.word	0x42e00000

