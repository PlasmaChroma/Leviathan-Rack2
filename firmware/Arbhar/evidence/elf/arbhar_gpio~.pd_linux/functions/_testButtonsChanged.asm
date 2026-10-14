00006014 <_testButtonsChanged>:
    6014: e92d4070     	push	{r4, r5, r6, lr}
    6018: e2804a01     	add	r4, r0, #4096
    601c: ed2d8b02     	vpush	{d8}
    6020: e5d42d5d     	ldrb	r2, [r4, #0xd5d]
    6024: e24dd008     	sub	sp, sp, #8
    6028: e3520000     	cmp	r2, #0
    602c: 0a000015     	beq	0x6088 <_testButtonsChanged+0x74> @ imm = #0x54
    6030: e1a03001     	mov	r3, r1
    6034: e5d01030     	ldrb	r1, [r0, #0x30]
    6038: e3510062     	cmp	r1, #98
    603c: 8a000014     	bhi	0x6094 <_testButtonsChanged+0x80> @ imm = #0x50
    6040: e5d32016     	ldrb	r2, [r3, #0x16]
    6044: e3a01064     	mov	r1, #100
    6048: e5d36006     	ldrb	r6, [r3, #0x6]
    604c: e5d35026     	ldrb	r5, [r3, #0x26]
    6050: e082c102     	add	r12, r2, r2, lsl #2
    6054: e1a0208c     	lsl	r2, r12, #1
    6058: e1062681     	smlabb	r6, r1, r6, r2
    605c: e0855006     	add	r5, r5, r6
    6060: e5c35002     	strb	r5, [r3, #0x2]
    6064: e3550000     	cmp	r5, #0
    6068: 0a000004     	beq	0x6080 <_testButtonsChanged+0x6c> @ imm = #0x10
    606c: e1550001     	cmp	r5, r1
    6070: 0a00001b     	beq	0x60e4 <_testButtonsChanged+0xd0> @ imm = #0x6c
    6074: e59f2108     	ldr	r2, [pc, #0x108]        @ 0x6184 <_testButtonsChanged+0x170>
    6078: e08f3002     	add	r3, pc, r2
    607c: e5835018     	str	r5, [r3, #0x18]
    6080: e3a05000     	mov	r5, #0
    6084: e5c45d5d     	strb	r5, [r4, #0xd5d]
    6088: e28dd008     	add	sp, sp, #8
    608c: ecbd8b02     	vpop	{d8}
    6090: e8bd8070     	pop	{r4, r5, r6, pc}
    6094: e3510063     	cmp	r1, #99
    6098: 1afffffa     	bne	0x6088 <_testButtonsChanged+0x74> @ imm = #-0x18
    609c: e5d30016     	ldrb	r0, [r3, #0x16]
    60a0: e3a0e064     	mov	lr, #100
    60a4: e5d35006     	ldrb	r5, [r3, #0x6]
    60a8: e1a01002     	mov	r1, r2
    60ac: e5d3c026     	ldrb	r12, [r3, #0x26]
    60b0: e0806100     	add	r6, r0, r0, lsl #2
    60b4: e59f20cc     	ldr	r2, [pc, #0xcc]         @ 0x6188 <_testButtonsChanged+0x174>
    60b8: e1a06086     	lsl	r6, r6, #1
    60bc: e08f0002     	add	r0, pc, r2
    60c0: e10e658e     	smlabb	lr, lr, r5, r6
    60c4: e08c500e     	add	r5, r12, lr
    60c8: e5c35002     	strb	r5, [r3, #0x2]
    60cc: e5d4cd82     	ldrb	r12, [r4, #0xd82]
    60d0: e5d43d72     	ldrb	r3, [r4, #0xd72]
    60d4: e5d42d62     	ldrb	r2, [r4, #0xd62]
    60d8: e58dc000     	str	r12, [sp]
    60dc: ebfff6a5     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x256c
    60e0: eaffffe6     	b	0x6080 <_testButtonsChanged+0x6c> @ imm = #-0x68
    60e4: e59f30a0     	ldr	r3, [pc, #0xa0]         @ 0x618c <_testButtonsChanged+0x178>
    60e8: e08f6003     	add	r6, pc, r3
    60ec: e596c018     	ldr	r12, [r6, #0x18]
    60f0: e15c0001     	cmp	r12, r1
    60f4: 1affffde     	bne	0x6074 <_testButtonsChanged+0x60> @ imm = #-0x88
    60f8: e5941ddc     	ldr	r1, [r4, #0xddc]
    60fc: edd67a07     	vldr	s15, [r6, #28]
    6100: e5d00069     	ldrb	r0, [r0, #0x69]
    6104: ee071a10     	vmov	s14, r1
    6108: ed9f0a1c     	vldr	s0, [pc, #112]          @ 0x6180 <_testButtonsChanged+0x16c>
    610c: eeb88a47     	vcvt.f32.u32	s16, s14
    6110: ee780a67     	vsub.f32	s1, s16, s15
    6114: eef40ac0     	vcmpe.f32	s1, s0
    6118: eef1fa10     	vmrs	APSR_nzcv, fpscr
    611c: 4a000009     	bmi	0x6148 <_testButtonsChanged+0x134> @ imm = #0x24
    6120: e3500000     	cmp	r0, #0
    6124: 0d868a07     	vstreq	s16, [r6, #28]
    6128: 0affffd1     	beq	0x6074 <_testButtonsChanged+0x60> @ imm = #-0xbc
    612c: e59fe05c     	ldr	lr, [pc, #0x5c]         @ 0x6190 <_testButtonsChanged+0x17c>
    6130: eef70ae0     	vcvt.f64.f32	d16, s1
    6134: e08f000e     	add	r0, pc, lr
    6138: ec532b30     	vmov	r2, r3, d16
    613c: ebfff68d     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x25cc
    6140: ed868a07     	vstr	s16, [r6, #28]
    6144: eaffffca     	b	0x6074 <_testButtonsChanged+0x60> @ imm = #-0xd8
    6148: eeb71ae0     	vcvt.f64.f32	d1, s1
    614c: e3500000     	cmp	r0, #0
    6150: ec532b11     	vmov	r2, r3, d1
    6154: 0a000004     	beq	0x616c <_testButtonsChanged+0x158> @ imm = #0x10
    6158: e59fc034     	ldr	r12, [pc, #0x34]        @ 0x6194 <_testButtonsChanged+0x180>
    615c: e08f000c     	add	r0, pc, r12
    6160: ebfff684     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x25f0
    6164: ed868a07     	vstr	s16, [r6, #28]
    6168: eaffffc1     	b	0x6074 <_testButtonsChanged+0x60> @ imm = #-0xfc
    616c: e59f1024     	ldr	r1, [pc, #0x24]         @ 0x6198 <_testButtonsChanged+0x184>
    6170: e08f0001     	add	r0, pc, r1
    6174: ebfff67f     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x2604
    6178: ed868a07     	vstr	s16, [r6, #28]
    617c: eaffffbc     	b	0x6074 <_testButtonsChanged+0x60> @ imm = #-0x110
    6180: 00 00 fa 43  	.word	0x43fa0000
    6184: 38 13 02 00  	.word	0x00021338
    6188: 28 ec 00 00  	.word	0x0000ec28
    618c: c8 12 02 00  	.word	0x000212c8
    6190: 94 eb 00 00  	.word	0x0000eb94
    6194: 48 eb 00 00  	.word	0x0000eb48
    6198: 24 eb 00 00  	.word	0x0000eb24

