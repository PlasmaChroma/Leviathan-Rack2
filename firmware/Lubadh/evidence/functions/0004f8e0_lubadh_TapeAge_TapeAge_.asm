; lubadh::TapeAge::TapeAge()
; VA 0x4f8e0 size 184

   4f8e0: e92d4070     	push	{r4, r5, r6, lr}
   4f8e4: e2805010     	add	r5, r0, #16
   4f8e8: e1a04000     	mov	r4, r0
   4f8ec: e1a00005     	mov	r0, r5
   4f8f0: ebffee9d     	bl	0x4b36c
   4f8f4: e284003c     	add	r0, r4, #60
   4f8f8: ebffede7     	bl	0x4b09c
   4f8fc: f2c01010     	vmov.i32	d17, #0x0
   4f900: e2843048     	add	r3, r4, #72
   4f904: eddf0b1d     	vldr	d16, [pc, #116]         @ 0x4f980 ; float 1.34110481007e-08
   4f908: e3a02000     	mov	r2, #0
   4f90c: e3031127     	movw	r1, #0x3127
   4f910: e3431e9c     	movt	r1, #0x3e9c
   4f914: e1a00005     	mov	r0, r5
   4f918: eddf0a1c     	vldr	s1, [pc, #112]          @ 0x4f990 ; float 0.20000000298
   4f91c: f443178f     	vst1.32	{d17}, [r3]
   4f920: ed9f0a1b     	vldr	s0, [pc, #108]          @ 0x4f994 ; float 0.000406749983085
   4f924: e5841008     	str	r1, [r4, #0x8]
   4f928: e5842050     	str	r2, [r4, #0x50]
   4f92c: f444078f     	vst1.32	{d16}, [r4]
   4f930: ebffec0e     	bl	0x4a970
   4f934: edd47a02     	vldr	s15, [r4, #8]
   4f938: eef72b00     	vmov.f64	d18, #1.000000e+00
   4f93c: eddf1b11     	vldr	d17, [pc, #68]          @ 0x4f988 ; float 3.14159265359
   4f940: e1a00004     	mov	r0, r4
   4f944: eef70ae7     	vcvt.f64.f32	d16, s15
   4f948: ee600ba1     	vmul.f64	d16, d16, d17
   4f94c: eec21ba0     	vdiv.f64	d17, d18, d16
   4f950: eef77be1     	vcvt.f32.f64	s15, d17
   4f954: edc47a0f     	vstr	s15, [r4, #60]
   4f958: e8bd8070     	pop	{r4, r5, r6, pc}
   4f95c: e1a00005     	mov	r0, r5
   4f960: eb0002e9     	bl	0x5050c
   4f964: ebff197d     	bl	0x15f60    @ imm = #-0x39a0c ; __cxa_end_cleanup
   4f968: e5940048     	ldr	r0, [r4, #0x48]
   4f96c: e3500000     	cmp	r0, #0
   4f970: 0afffff9     	beq	0x4f95c
   4f974: ebff1931     	bl	0x15e40    @ imm = #-0x39b3c ; _ZdlPv
   4f978: eafffff7     	b	0x4f95c
   4f97c: e320f000     	nop
   4f980: 0f 41 d5 39  	.word	0x39d5410f
   4f984: cd cc 4c 3e  	.word	0x3e4ccccd
   4f988: 18 2d 44 54  	.word	0x54442d18
   4f98c: fb 21 09 40  	.word	0x400921fb
   4f990: cd cc 4c 3e  	.word	0x3e4ccccd
   4f994: 0f 41 d5 39  	.word	0x39d5410f
