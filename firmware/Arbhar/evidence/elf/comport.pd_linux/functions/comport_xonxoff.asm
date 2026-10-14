00002064 <comport_xonxoff>:
    2064: eefd7ac0     	vcvt.s32.f32	s15, s0
    2068: e92d4070     	push	{r4, r5, r6, lr}
    206c: ed2d8b02     	vpush	{d8}
    2070: e5903060     	ldr	r3, [r0, #0x60]
    2074: e5902020     	ldr	r2, [r0, #0x20]
    2078: ee175a90     	vmov	r5, s15
    207c: e3550001     	cmp	r5, #1
    2080: 03833b07     	orreq	r3, r3, #7168
    2084: 13c33b07     	bicne	r3, r3, #7168
    2088: 0d9f8a21     	vldreq	s16, [pc, #132]         @ 0x2114 <comport_xonxoff+0xb0>
    208c: 1d9f8a21     	vldrne	s16, [pc, #132]         @ 0x2118 <comport_xonxoff+0xb4>
    2090: 13a05000     	movne	r5, #0
    2094: e3720001     	cmn	r2, #1
    2098: e5803060     	str	r3, [r0, #0x60]
    209c: 0a000012     	beq	0x20ec <comport_xonxoff+0x88> @ imm = #0x48
    20a0: e1a04000     	mov	r4, r0
    20a4: e3a01002     	mov	r1, #2
    20a8: e1a00002     	mov	r0, r2
    20ac: e2842060     	add	r2, r4, #96
    20b0: ebfffa7d     	bl	0xaac <.plt+0xa4>       @ imm = #-0x160c
    20b4: e3700001     	cmn	r0, #1
    20b8: 0a00000d     	beq	0x20f4 <comport_xonxoff+0x90> @ imm = #0x34
    20bc: e2846a01     	add	r6, r4, #4096
    20c0: e59600e0     	ldr	r0, [r6, #0xe0]
    20c4: e3500000     	cmp	r0, #0
    20c8: da000006     	ble	0x20e8 <comport_xonxoff+0x84> @ imm = #0x18
    20cc: eeb77ac8     	vcvt.f64.f32	d7, s16
    20d0: e594109c     	ldr	r1, [r4, #0x9c]
    20d4: e59fc040     	ldr	r12, [pc, #0x40]        @ 0x211c <comport_xonxoff+0xb8>
    20d8: e5911000     	ldr	r1, [r1]
    20dc: e08f000c     	add	r0, pc, r12
    20e0: ec532b17     	vmov	r2, r3, d7
    20e4: ebfffa94     	bl	0xb3c <.plt+0x134>      @ imm = #-0x15b0
    20e8: e58650b4     	str	r5, [r6, #0xb4]
    20ec: ecbd8b02     	vpop	{d8}
    20f0: e8bd8070     	pop	{r4, r5, r6, pc}
    20f4: ecbd8b02     	vpop	{d8}
    20f8: e1a00004     	mov	r0, r4
    20fc: e594e09c     	ldr	lr, [r4, #0x9c]
    2100: e59f5018     	ldr	r5, [pc, #0x18]         @ 0x2120 <comport_xonxoff+0xbc>
    2104: e59e2000     	ldr	r2, [lr]
    2108: e08f1005     	add	r1, pc, r5
    210c: e8bd4070     	pop	{r4, r5, r6, lr}
    2110: eafffab3     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x1534
    2114: 00 00 80 3f  	.word	0x3f800000
    2118: 00 00 00 00  	.word	0x00000000
    211c: d8 25 00 00  	.word	0x000025d8
    2120: d0 25 00 00  	.word	0x000025d0

