; lubadh::Channel::checkClkDivs()
; VA 0x3724c size 136

   3724c: e59030e8     	ldr	r3, [r0, #0xe8]
   37250: e2802a2a     	add	r2, r0, #172032
   37254: eddf4b19     	vldr	d20, [pc, #100]         @ 0x372c0 ; float 2.7
   37258: eef13b04     	vmov.f64	d19, #5.000000e+00
   3725c: eddf6a1b     	vldr	s13, [pc, #108]         @ 0x372d0 ; float 1000
   37260: eddf2b18     	vldr	d18, [pc, #96]          @ 0x372c8 ; float 49170.2539062
   37264: edd37a00     	vldr	s15, [r3]
   37268: e59234f4     	ldr	r3, [r2, #0x4f4]
   3726c: ee073a10     	vmov	s14, r3
   37270: eef07ae7     	vabs.f32	s15, s15
   37274: eeba7aef     	vcvt.f32.s32	s14, s14, #1
   37278: eef70ae7     	vcvt.f64.f32	d16, s15
   3727c: ee277a26     	vmul.f32	s14, s14, s13
   37280: ee600ba4     	vmul.f64	d16, d16, d20
   37284: eeb77ac7     	vcvt.f64.f32	d7, s14
   37288: ee600ba2     	vmul.f64	d16, d16, d18
   3728c: eec72b20     	vdiv.f64	d18, d7, d16
   37290: eef42be3     	vcmpe.f64	d18, d19
   37294: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37298: 53a03005     	movpl	r3, #5
   3729c: 5a000005     	bpl	0x372b8
   372a0: eef00b00     	vmov.f64	d16, #2.000000e+00
   372a4: eef42be0     	vcmpe.f64	d18, d16
   372a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   372ac: cefd7be2     	vcvtgt.s32.f64	s15, d18
   372b0: d3a03002     	movle	r3, #2
   372b4: ce173a90     	vmovgt	r3, s15
   372b8: e58234d4     	str	r3, [r2, #0x4d4]
   372bc: e12fff1e     	bx	lr
   372c0: 9a 99 99 99  	.word	0x9999999a
   372c4: 99 99 05 40  	.word	0x40059999
   372c8: 00 00 00 20  	.word	0x20000000
   372cc: 48 02 e8 40  	.word	0x40e80248
   372d0: 00 00 7a 44  	.word	0x447a0000
