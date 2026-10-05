; lubadh::Channel::flashSpeedLED(bool, float)
; VA 0x3794c size 48

   3794c: e3510000     	cmp	r1, #0
   37950: 05801290     	streq	r1, [r0, #0x290]
   37954: e5c01240     	strb	r1, [r0, #0x240]
   37958: 1ddf7a05     	vldrne	s15, [pc, #20]          @ 0x37974 ; float 1000
   3795c: 1d9f7a05     	vldrne	s14, [pc, #20]          @ 0x37978 ; float 5.40000009537
   37960: 1ec76a80     	vdivne.f32	s13, s15, s0
   37964: 1ec67a87     	vdivne.f32	s15, s13, s14
   37968: 1efd7ae7     	vcvtne.s32.f32	s15, s15
   3796c: 1dc07aa5     	vstrne	s15, [r0, #660]
   37970: e12fff1e     	bx	lr
   37974: 00 00 7a 44  	.word	0x447a0000
   37978: cd cc ac 40  	.word	0x40accccd
