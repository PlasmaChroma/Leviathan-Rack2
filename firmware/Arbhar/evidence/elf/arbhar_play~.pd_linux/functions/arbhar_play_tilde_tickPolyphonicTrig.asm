00007ac8 <arbhar_play_tilde_tickPolyphonicTrig>:
    7ac8: e2803a02     	add	r3, r0, #8192
    7acc: ed9f7a24     	vldr	s14, [pc, #144]         @ 0x7b64 <arbhar_play_tilde_tickPolyphonicTrig+0x9c>
    7ad0: e5932894     	ldr	r2, [r3, #0x894]
    7ad4: ed9f6a23     	vldr	s12, [pc, #140]         @ 0x7b68 <arbhar_play_tilde_tickPolyphonicTrig+0xa0>
    7ad8: edd26ad7     	vldr	s13, [r2, #860]
    7adc: edd27a0c     	vldr	s15, [r2, #48]
    7ae0: ee567a87     	vnmls.f32	s15, s13, s14
    7ae4: eef47ac6     	vcmpe.f32	s15, s12
    7ae8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7aec: 4a00000b     	bmi	0x7b20 <arbhar_play_tilde_tickPolyphonicTrig+0x58> @ imm = #0x2c
    7af0: e3a01001     	mov	r1, #1
    7af4: e58010ac     	str	r1, [r0, #0xac]
    7af8: e593c6e8     	ldr	r12, [r3, #0x6e8]
    7afc: e35c0000     	cmp	r12, #0
    7b00: d12fff1e     	bxle	lr
    7b04: e92d4010     	push	{r4, lr}
    7b08: e1a04000     	mov	r4, r0
    7b0c: ebfffc48     	bl	0x6c34 <play_next>      @ imm = #-0xee0
    7b10: e5940100     	ldr	r0, [r4, #0x100]
    7b14: eeb70a00     	vmov.f32	s0, #1.000000e+00
    7b18: e8bd4010     	pop	{r4, lr}
    7b1c: eaffeae2     	b	0x26ac <.plt+0x2cc>     @ imm = #-0x5478
    7b20: ed9f0a11     	vldr	s0, [pc, #68]           @ 0x7b6c <arbhar_play_tilde_tickPolyphonicTrig+0xa4>
    7b24: eddf0a11     	vldr	s1, [pc, #68]           @ 0x7b70 <arbhar_play_tilde_tickPolyphonicTrig+0xa8>
    7b28: ee371a80     	vadd.f32	s2, s15, s0
    7b2c: eeb41ac6     	vcmpe.f32	s2, s12
    7b30: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7b34: beb01a46     	vmovlt.f32	s2, s12
    7b38: eeb41ac0     	vcmpe.f32	s2, s0
    7b3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7b40: 8eb01a40     	vmovhi.f32	s2, s0
    7b44: ee611a20     	vmul.f32	s3, s2, s1
    7b48: eeb62a00     	vmov.f32	s4, #5.000000e-01
    7b4c: eef41ac2     	vcmpe.f32	s3, s4
    7b50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7b54: 93a02000     	movls	r2, #0
    7b58: 958020ac     	strls	r2, [r0, #0xac]
    7b5c: 9affffe5     	bls	0x7af8 <arbhar_play_tilde_tickPolyphonicTrig+0x30> @ imm = #-0x6c
    7b60: eaffffe2     	b	0x7af0 <arbhar_play_tilde_tickPolyphonicTrig+0x28> @ imm = #-0x78
    7b64: 00 00 00 42  	.word	0x42000000
    7b68: 00 00 00 00  	.word	0x00000000
    7b6c: 00 f0 7f 45  	.word	0x457ff000
    7b70: 01 08 80 39  	.word	0x39800801

