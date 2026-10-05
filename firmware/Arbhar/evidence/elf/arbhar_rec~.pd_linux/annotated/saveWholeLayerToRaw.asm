00002dc4 <saveWholeLayerToRaw>:
    2dc4: eef77a00     	vmov.f32	s15, #1.000000e+00
    2dc8: eeb40ae7     	vcmpe.f32	s0, s15
    2dcc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2dd0: b12fff1e     	bxlt	lr
    2dd4: eebd0ac0     	vcvt.s32.f32	s0, s0
    2dd8: e92d4070     	push	{r4, r5, r6, lr}
    2ddc: ee104a10     	vmov	r4, s0
    2de0: e0805104     	add	r5, r0, r4, lsl #2
    2de4: e5956364     	ldr	r6, [r5, #0x364]
    2de8: e595321c     	ldr	r3, [r5, #0x21c]
    2dec: e3560000     	cmp	r6, #0
    2df0: 0a000001     	beq	0x2dfc <saveWholeLayerToRaw+0x38> @ imm = #0x4
    2df4: e3530000     	cmp	r3, #0
    2df8: ca000005     	bgt	0x2e14 <saveWholeLayerToRaw+0x50> @ imm = #0x14
    2dfc: e59f0048     	ldr	r0, [pc, #0x48]         @ 0x2e4c <saveWholeLayerToRaw+0x88>  // u32=0x669c; f32?=3.68093081e-41
    2e00: e1a01004     	mov	r1, r4
    2e04: e3a02004     	mov	r2, #4
    2e08: e08f0000     	add	r0, pc, r0
    2e0c: e8bd4070     	pop	{r4, r5, r6, lr}
    2e10: eafffe4f     	b	0x2754 <.plt+0x254>     @ imm = #-0x6c4  // CALL post
    2e14: e3a02000     	mov	r2, #0
    2e18: e1a00006     	mov	r0, r6
    2e1c: e1a01002     	mov	r1, r2
    2e20: ebfffe81     	bl	0x282c <.plt+0x32c>     @ imm = #-0x5fc  // CALL fseek
    2e24: e3500000     	cmp	r0, #0
    2e28: 0a000001     	beq	0x2e34 <saveWholeLayerToRaw+0x70> @ imm = #0x4
    2e2c: e595321c     	ldr	r3, [r5, #0x21c]
    2e30: eafffff1     	b	0x2dfc <saveWholeLayerToRaw+0x38> @ imm = #-0x3c
    2e34: e1a03006     	mov	r3, r6
    2e38: e595221c     	ldr	r2, [r5, #0x21c]
    2e3c: e3a01004     	mov	r1, #4
    2e40: e59501e8     	ldr	r0, [r5, #0x1e8]
    2e44: ebfffdfa     	bl	0x2634 <.plt+0x134>     @ imm = #-0x818  // CALL fwrite
    2e48: eafffff7     	b	0x2e2c <saveWholeLayerToRaw+0x68> @ imm = #-0x24
    2e4c: 9c 66 00 00  	.word	0x0000669c

