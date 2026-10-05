00006a10 <led_directionControl>:
    6a10: e92d4010     	push	{r4, lr}
    6a14: eeb70a00     	vmov.f32	s0, #1.000000e+00
    6a18: e1a04000     	mov	r4, r0
    6a1c: ebfff371     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x323c
    6a20: eddf7a2e     	vldr	s15, [pc, #184]         @ 0x6ae0 <led_directionControl+0xd0>
    6a24: eeb40ae7     	vcmpe.f32	s0, s15
    6a28: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a2c: 4a000003     	bmi	0x6a40 <led_directionControl+0x30> @ imm = #0xc
    6a30: e2843a01     	add	r3, r4, #4096
    6a34: e5d30df5     	ldrb	r0, [r3, #0xdf5]
    6a38: e3500000     	cmp	r0, #0
    6a3c: 0a000019     	beq	0x6aa8 <led_directionControl+0x98> @ imm = #0x64
    6a40: ed9f0a27     	vldr	s0, [pc, #156]          @ 0x6ae4 <led_directionControl+0xd4>
    6a44: e1a00004     	mov	r0, r4
    6a48: ebfff366     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x3268
    6a4c: eeb50a40     	vcmp.f32	s0, #0
    6a50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a54: 1a000010     	bne	0x6a9c <led_directionControl+0x8c> @ imm = #0x40
    6a58: ed9f0a22     	vldr	s0, [pc, #136]          @ 0x6ae8 <led_directionControl+0xd8>
    6a5c: e1a00004     	mov	r0, r4
    6a60: ebfff360     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x3280
    6a64: eeb50a40     	vcmp.f32	s0, #0
    6a68: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a6c: 1eb00a00     	vmovne.f32	s0, #2.000000e+00
    6a70: 0d9f0a1d     	vldreq	s0, [pc, #116]          @ 0x6aec <led_directionControl+0xdc>
    6a74: ebfff361     	bl	0x3800 <.plt+0x104>     @ imm = #-0x327c
    6a78: e1a00004     	mov	r0, r4
    6a7c: ed9f0a1b     	vldr	s0, [pc, #108]          @ 0x6af0 <led_directionControl+0xe0>
    6a80: ebfff358     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x32a0
    6a84: eeb50a40     	vcmp.f32	s0, #0
    6a88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a8c: 0a00000a     	beq	0x6abc <led_directionControl+0xac> @ imm = #0x28
    6a90: eeb70a00     	vmov.f32	s0, #1.000000e+00
    6a94: e8bd4010     	pop	{r4, lr}
    6a98: eafff382     	b	0x38a8 <.plt+0x1ac>     @ imm = #-0x31f8
    6a9c: eeb70a00     	vmov.f32	s0, #1.000000e+00
    6aa0: ebfff356     	bl	0x3800 <.plt+0x104>     @ imm = #-0x32a8
    6aa4: eafffff3     	b	0x6a78 <led_directionControl+0x68> @ imm = #-0x34
    6aa8: eeb00a00     	vmov.f32	s0, #2.000000e+00
    6aac: ebfff353     	bl	0x3800 <.plt+0x104>     @ imm = #-0x32b4
    6ab0: e8bd4010     	pop	{r4, lr}
    6ab4: eeb00a00     	vmov.f32	s0, #2.000000e+00
    6ab8: eafff37a     	b	0x38a8 <.plt+0x1ac>     @ imm = #-0x3218
    6abc: e1a00004     	mov	r0, r4
    6ac0: ed9f0a0b     	vldr	s0, [pc, #44]           @ 0x6af4 <led_directionControl+0xe4>
    6ac4: ebfff347     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x32e4
    6ac8: eeb50a40     	vcmp.f32	s0, #0
    6acc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6ad0: 1afffff6     	bne	0x6ab0 <led_directionControl+0xa0> @ imm = #-0x28
    6ad4: ed9f0a04     	vldr	s0, [pc, #16]           @ 0x6aec <led_directionControl+0xdc>
    6ad8: e8bd4010     	pop	{r4, lr}
    6adc: eafff371     	b	0x38a8 <.plt+0x1ac>     @ imm = #-0x323c
    6ae0: 00 00 7a 45  	.word	0x457a0000
    6ae4: 00 00 e0 42  	.word	0x42e00000
    6ae8: 00 00 e4 42  	.word	0x42e40000
    6aec: 00 00 00 00  	.word	0x00000000
    6af0: 00 00 e2 42  	.word	0x42e20000
    6af4: 00 00 e6 42  	.word	0x42e60000

