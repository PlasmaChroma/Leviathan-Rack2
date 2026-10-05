000022a0 <comport_baud>:
    22a0: e92d4070     	push	{r4, r5, r6, lr}
    22a4: e2805a01     	add	r5, r0, #4096
    22a8: edd57a29     	vldr	s15, [r5, #164]
    22ac: eef47a40     	vcmp.f32	s15, s0
    22b0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    22b4: 0a000017     	beq	0x2318 <comport_baud+0x78> @ imm = #0x5c
    22b8: e1a04000     	mov	r4, r0
    22bc: ebfffc0b     	bl	0x12f0 <set_baudrate>   @ imm = #-0xfd4
    22c0: e5940020     	ldr	r0, [r4, #0x20]
    22c4: e3700001     	cmn	r0, #1
    22c8: ed850a29     	vstr	s0, [r5, #164]
    22cc: 08bd8070     	popeq	{r4, r5, r6, pc}
    22d0: e2842060     	add	r2, r4, #96
    22d4: e3a01002     	mov	r1, #2
    22d8: ebfff9f3     	bl	0xaac <.plt+0xa4>       @ imm = #-0x1834  // CALL tcsetattr
    22dc: e3700001     	cmn	r0, #1
    22e0: 0a000012     	beq	0x2330 <comport_baud+0x90> @ imm = #0x48
    22e4: e59530e0     	ldr	r3, [r5, #0xe0]
    22e8: e3530000     	cmp	r3, #0
    22ec: da000008     	ble	0x2314 <comport_baud+0x74> @ imm = #0x20
    22f0: ed950a29     	vldr	s0, [r5, #164]
    22f4: e594109c     	ldr	r1, [r4, #0x9c]
    22f8: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x234c <comport_baud+0xac>  // u32=0x24f4; f32?=1.32562835e-41
    22fc: e8bd4070     	pop	{r4, r5, r6, lr}
    2300: eeb77ac0     	vcvt.f64.f32	d7, s0
    2304: e08f0000     	add	r0, pc, r0
    2308: e5911000     	ldr	r1, [r1]
    230c: ec532b17     	vmov	r2, r3, d7
    2310: eafffa09     	b	0xb3c <.plt+0x134>      @ imm = #-0x17dc  // CALL post
    2314: e8bd8070     	pop	{r4, r5, r6, pc}
    2318: eeb71ae7     	vcvt.f64.f32	d1, s15
    231c: e59f502c     	ldr	r5, [pc, #0x2c]         @ 0x2350 <comport_baud+0xb0>  // u32=0x24c0; f32?=1.3183416e-41
    2320: e08f0005     	add	r0, pc, r5
    2324: e8bd4070     	pop	{r4, r5, r6, lr}
    2328: ec532b11     	vmov	r2, r3, d1
    232c: eafffa02     	b	0xb3c <.plt+0x134>      @ imm = #-0x17f8  // CALL post
    2330: e594209c     	ldr	r2, [r4, #0x9c]
    2334: e1a00004     	mov	r0, r4
    2338: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x2354 <comport_baud+0xb4>  // u32=0x24d4; f32?=1.32114419e-41
    233c: e5922000     	ldr	r2, [r2]
    2340: e08f1004     	add	r1, pc, r4
    2344: e8bd4070     	pop	{r4, r5, r6, lr}
    2348: eafffa25     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x176c  // CALL pd_error
    234c: f4 24 00 00  	.word	0x000024f4
    2350: c0 24 00 00  	.word	0x000024c0
    2354: d4 24 00 00  	.word	0x000024d4

