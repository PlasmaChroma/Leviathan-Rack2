000021e4 <comport_stopbit>:
    21e4: eebd0ac0     	vcvt.s32.f32	s0, s0
    21e8: e5901068     	ldr	r1, [r0, #0x68]
    21ec: e5902020     	ldr	r2, [r0, #0x20]
    21f0: e92d4070     	push	{r4, r5, r6, lr}
    21f4: ed2d8b02     	vpush	{d8}
    21f8: ee103a10     	vmov	r3, s0
    21fc: e3530001     	cmp	r3, #1
    2200: 03811040     	orreq	r1, r1, #64
    2204: 13c11040     	bicne	r1, r1, #64
    2208: 0d9f8a20     	vldreq	s16, [pc, #128]         @ 0x2290 <comport_stopbit+0xac>
    220c: 1d9f8a20     	vldrne	s16, [pc, #128]         @ 0x2294 <comport_stopbit+0xb0>
    2210: e3720001     	cmn	r2, #1
    2214: e5801068     	str	r1, [r0, #0x68]
    2218: 0a000012     	beq	0x2268 <comport_stopbit+0x84> @ imm = #0x48
    221c: e1a04000     	mov	r4, r0
    2220: e3a01002     	mov	r1, #2
    2224: e1a00002     	mov	r0, r2
    2228: e2842060     	add	r2, r4, #96
    222c: ebfffa1e     	bl	0xaac <.plt+0xa4>       @ imm = #-0x1788
    2230: e3700001     	cmn	r0, #1
    2234: 0a00000d     	beq	0x2270 <comport_stopbit+0x8c> @ imm = #0x34
    2238: e2845a01     	add	r5, r4, #4096
    223c: e59500e0     	ldr	r0, [r5, #0xe0]
    2240: e3500000     	cmp	r0, #0
    2244: da000006     	ble	0x2264 <comport_stopbit+0x80> @ imm = #0x18
    2248: eeb77ac8     	vcvt.f64.f32	d7, s16
    224c: e594609c     	ldr	r6, [r4, #0x9c]
    2250: e59fc040     	ldr	r12, [pc, #0x40]        @ 0x2298 <comport_stopbit+0xb4>
    2254: e5961000     	ldr	r1, [r6]
    2258: e08f000c     	add	r0, pc, r12
    225c: ec532b17     	vmov	r2, r3, d7
    2260: ebfffa35     	bl	0xb3c <.plt+0x134>      @ imm = #-0x172c
    2264: ed858a2c     	vstr	s16, [r5, #176]
    2268: ecbd8b02     	vpop	{d8}
    226c: e8bd8070     	pop	{r4, r5, r6, pc}
    2270: ecbd8b02     	vpop	{d8}
    2274: e1a00004     	mov	r0, r4
    2278: e594e09c     	ldr	lr, [r4, #0x9c]
    227c: e59f3018     	ldr	r3, [pc, #0x18]         @ 0x229c <comport_stopbit+0xb8>
    2280: e59e2000     	ldr	r2, [lr]
    2284: e08f1003     	add	r1, pc, r3
    2288: e8bd4070     	pop	{r4, r5, r6, lr}
    228c: eafffa54     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x16b0
    2290: 00 00 80 3f  	.word	0x3f800000
    2294: 00 00 00 00  	.word	0x00000000
    2298: 1c 25 00 00  	.word	0x0000251c
    229c: 1c 25 00 00  	.word	0x0000251c

