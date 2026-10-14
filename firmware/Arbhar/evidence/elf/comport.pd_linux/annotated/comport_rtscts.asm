00002124 <comport_rtscts>:
    2124: eefd7ac0     	vcvt.s32.f32	s15, s0
    2128: e92d4070     	push	{r4, r5, r6, lr}
    212c: ed2d8b02     	vpush	{d8}
    2130: e5903068     	ldr	r3, [r0, #0x68]
    2134: e5902020     	ldr	r2, [r0, #0x20]
    2138: ee175a90     	vmov	r5, s15
    213c: e3550001     	cmp	r5, #1
    2140: 03833102     	orreq	r3, r3, #-2147483648
    2144: 13c33102     	bicne	r3, r3, #-2147483648
    2148: 0d9f8a21     	vldreq	s16, [pc, #132]         @ 0x21d4 <comport_rtscts+0xb0>
    214c: 1d9f8a21     	vldrne	s16, [pc, #132]         @ 0x21d8 <comport_rtscts+0xb4>
    2150: 13a05000     	movne	r5, #0
    2154: e3720001     	cmn	r2, #1
    2158: e5803068     	str	r3, [r0, #0x68]
    215c: 0a000012     	beq	0x21ac <comport_rtscts+0x88> @ imm = #0x48
    2160: e1a04000     	mov	r4, r0
    2164: e3a01002     	mov	r1, #2
    2168: e1a00002     	mov	r0, r2
    216c: e2842060     	add	r2, r4, #96
    2170: ebfffa4d     	bl	0xaac <.plt+0xa4>       @ imm = #-0x16cc  // CALL tcsetattr
    2174: e3700001     	cmn	r0, #1
    2178: 0a00000d     	beq	0x21b4 <comport_rtscts+0x90> @ imm = #0x34
    217c: e2846a01     	add	r6, r4, #4096
    2180: e59600e0     	ldr	r0, [r6, #0xe0]
    2184: e3500000     	cmp	r0, #0
    2188: da000006     	ble	0x21a8 <comport_rtscts+0x84> @ imm = #0x18
    218c: eeb77ac8     	vcvt.f64.f32	d7, s16
    2190: e594109c     	ldr	r1, [r4, #0x9c]
    2194: e59fc040     	ldr	r12, [pc, #0x40]        @ 0x21dc <comport_rtscts+0xb8>  // u32=0x2578; f32?=1.34412549e-41
    2198: e5911000     	ldr	r1, [r1]
    219c: e08f000c     	add	r0, pc, r12
    21a0: ec532b17     	vmov	r2, r3, d7
    21a4: ebfffa64     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1670  // CALL post
    21a8: e58650b8     	str	r5, [r6, #0xb8]
    21ac: ecbd8b02     	vpop	{d8}
    21b0: e8bd8070     	pop	{r4, r5, r6, pc}
    21b4: ecbd8b02     	vpop	{d8}
    21b8: e1a00004     	mov	r0, r4
    21bc: e594e09c     	ldr	lr, [r4, #0x9c]
    21c0: e59f5018     	ldr	r5, [pc, #0x18]         @ 0x21e0 <comport_rtscts+0xbc>  // u32=0x2570; f32?=1.34300445e-41
    21c4: e59e2000     	ldr	r2, [lr]
    21c8: e08f1005     	add	r1, pc, r5
    21cc: e8bd4070     	pop	{r4, r5, r6, lr}
    21d0: eafffa83     	b	0xbe4 <.plt+0x1dc>      @ imm = #-0x15f4  // CALL pd_error
    21d4: 00 00 80 3f  	.word	0x3f800000
    21d8: 00 00 00 00  	.word	0x00000000
    21dc: 78 25 00 00  	.word	0x00002578
    21e0: 70 25 00 00  	.word	0x00002570

