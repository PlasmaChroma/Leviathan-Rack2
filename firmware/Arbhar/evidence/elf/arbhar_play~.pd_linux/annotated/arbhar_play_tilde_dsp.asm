000035b4 <arbhar_play_tilde_dsp>:
    35b4: e92d4030     	push	{r4, r5, lr}
    35b8: e1a04001     	mov	r4, r1
    35bc: e5943000     	ldr	r3, [r4]
    35c0: e1a05000     	mov	r5, r0
    35c4: e59f1070     	ldr	r1, [pc, #0x70]         @ 0x363c <arbhar_play_tilde_dsp+0x88>  // u32=0x1aa20; f32?=1.52864847e-40
    35c8: e24dd014     	sub	sp, sp, #20
    35cc: e594c00c     	ldr	r12, [r4, #0xc]
    35d0: e1a02005     	mov	r2, r5
    35d4: e593e000     	ldr	lr, [r3]
    35d8: e08f1001     	add	r1, pc, r1
    35dc: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x3640 <arbhar_play_tilde_dsp+0x8c>  // u32=0x148; f32?=4.59625896e-43
    35e0: e5933004     	ldr	r3, [r3, #0x4]
    35e4: e7910000     	ldr	r0, [r1, r0]
    35e8: e3a01006     	mov	r1, #6
    35ec: e58de00c     	str	lr, [sp, #0xc]
    35f0: e59ce004     	ldr	lr, [r12, #0x4]
    35f4: e594c008     	ldr	r12, [r4, #0x8]
    35f8: e58de008     	str	lr, [sp, #0x8]
    35fc: e59ce004     	ldr	lr, [r12, #0x4]
    3600: e594c004     	ldr	r12, [r4, #0x4]
    3604: e58de004     	str	lr, [sp, #0x4]
    3608: e59ce004     	ldr	lr, [r12, #0x4]
    360c: e58de000     	str	lr, [sp]
    3610: ebfffc1c     	bl	0x2688 <.plt+0x2a8>     @ imm = #-0xf90  // CALL dsp_add
    3614: e5942000     	ldr	r2, [r4]
    3618: e59f4024     	ldr	r4, [pc, #0x24]         @ 0x3644 <arbhar_play_tilde_dsp+0x90>  // u32=0x9538; f32?=5.35296013e-41
    361c: edd27a02     	vldr	s15, [r2, #8]
    3620: e08f0004     	add	r0, pc, r4
    3624: eef70ae7     	vcvt.f64.f32	d16, s15
    3628: edc57a0e     	vstr	s15, [r5, #56]
    362c: ec532b30     	vmov	r2, r3, d16
    3630: e28dd014     	add	sp, sp, #20
    3634: e8bd4030     	pop	{r4, r5, lr}
    3638: eafffbf7     	b	0x261c <.plt+0x23c>     @ imm = #-0x1024  // CALL post
    363c: 20 aa 01 00  	.word	0x0001aa20
    3640: 48 01 00 00  	.word	0x00000148
    3644: 38 95 00 00  	.word	0x00009538

