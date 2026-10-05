000011f0 <comport_bits>:
    11f0: eefd7ac0     	vcvt.s32.f32	s15, s0
    11f4: e5903068     	ldr	r3, [r0, #0x68]
    11f8: e92d4070     	push	{r4, r5, r6, lr}
    11fc: e3c31030     	bic	r1, r3, #48
    1200: ed2d8b02     	vpush	{d8}
    1204: ee172a90     	vmov	r2, s15
    1208: e3520006     	cmp	r2, #6
    120c: 0a000025     	beq	0x12a8 <comport_bits+0xb8> @ imm = #0x94
    1210: e3520007     	cmp	r2, #7
    1214: 0a00001f     	beq	0x1298 <comport_bits+0xa8> @ imm = #0x7c
    1218: e3520005     	cmp	r2, #5
    121c: 0a00001a     	beq	0x128c <comport_bits+0x9c> @ imm = #0x68
    1220: ed9f8a2c     	vldr	s16, [pc, #176]         @ 0x12d8 <comport_bits+0xe8>
    1224: e3814030     	orr	r4, r1, #48
    1228: e5804068     	str	r4, [r0, #0x68]
    122c: e590c020     	ldr	r12, [r0, #0x20]
    1230: e37c0001     	cmn	r12, #1
    1234: 0a000012     	beq	0x1284 <comport_bits+0x94> @ imm = #0x48
    1238: e1a04000     	mov	r4, r0
    123c: e2842060     	add	r2, r4, #96
    1240: e1a0000c     	mov	r0, r12
    1244: e3a01002     	mov	r1, #2
    1248: ebfffe17     	bl	0xaac <.plt+0xa4>       @ imm = #-0x7a4
    124c: e3700001     	cmn	r0, #1
    1250: 0a000018     	beq	0x12b8 <comport_bits+0xc8> @ imm = #0x60
    1254: e2845a01     	add	r5, r4, #4096
    1258: e59500e0     	ldr	r0, [r5, #0xe0]
    125c: e3500000     	cmp	r0, #0
    1260: da000006     	ble	0x1280 <comport_bits+0x90> @ imm = #0x18
    1264: eeb77ac8     	vcvt.f64.f32	d7, s16
    1268: e594e09c     	ldr	lr, [r4, #0x9c]
    126c: e59f1074     	ldr	r1, [pc, #0x74]         @ 0x12e8 <comport_bits+0xf8>
    1270: e08f0001     	add	r0, pc, r1
    1274: e59e1000     	ldr	r1, [lr]
    1278: ec532b17     	vmov	r2, r3, d7
    127c: ebfffe2e     	bl	0xb3c <.plt+0x134>      @ imm = #-0x748
    1280: ed858a2a     	vstr	s16, [r5, #168]
    1284: ecbd8b02     	vpop	{d8}
    1288: e8bd8070     	pop	{r4, r5, r6, pc}
    128c: e5801068     	str	r1, [r0, #0x68]
    1290: ed9f8a11     	vldr	s16, [pc, #68]          @ 0x12dc <comport_bits+0xec>
    1294: eaffffe4     	b	0x122c <comport_bits+0x3c> @ imm = #-0x70
    1298: e3815020     	orr	r5, r1, #32
    129c: e5805068     	str	r5, [r0, #0x68]
    12a0: ed9f8a0e     	vldr	s16, [pc, #56]          @ 0x12e0 <comport_bits+0xf0>
    12a4: eaffffe0     	b	0x122c <comport_bits+0x3c> @ imm = #-0x80
    12a8: e3816010     	orr	r6, r1, #16
    12ac: e5806068     	str	r6, [r0, #0x68]
    12b0: ed9f8a0b     	vldr	s16, [pc, #44]          @ 0x12e4 <comport_bits+0xf4>
    12b4: eaffffdc     	b	0x122c <comport_bits+0x3c> @ imm = #-0x90
    12b8: ecbd8b02     	vpop	{d8}
    12bc: e1a00004     	mov	r0, r4
    12c0: e594209c     	ldr	r2, [r4, #0x9c]
    12c4: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x12ec <comport_bits+0xfc>
    12c8: e8bd4070     	pop	{r4, r5, r6, lr}
    12cc: e08f1003     	add	r1, pc, r3
    12d0: e5922000     	ldr	r2, [r2]
    12d4: eafffe42     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x6f8
    12d8: 00 00 00 41  	.word	0x41000000
    12dc: 00 00 a0 40  	.word	0x40a00000
    12e0: 00 00 e0 40  	.word	0x40e00000
    12e4: 00 00 c0 40  	.word	0x40c00000
    12e8: 68 30 00 00  	.word	0x00003068
    12ec: 24 30 00 00  	.word	0x00003024

