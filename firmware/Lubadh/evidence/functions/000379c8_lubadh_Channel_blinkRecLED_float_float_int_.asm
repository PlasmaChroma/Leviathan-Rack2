; lubadh::Channel::blinkRecLED(float, float, int)
; VA 0x379c8 size 72

   379c8: ed9f7a0d     	vldr	s14, [pc, #52]          @ 0x37a04 ; float 1000
   379cc: e3a03001     	mov	r3, #1
   379d0: eddf7a0c     	vldr	s15, [pc, #48]          @ 0x37a08 ; float 2.70000004768
   379d4: eddf6a0c     	vldr	s13, [pc, #48]          @ 0x37a0c ; float 5.40000009537
   379d8: ee876a20     	vdiv.f32	s12, s14, s1
   379dc: e58012a8     	str	r1, [r0, #0x2a8]
   379e0: ee807a27     	vdiv.f32	s14, s0, s15
   379e4: e5c03285     	strb	r3, [r0, #0x285]
   379e8: e5c032a4     	strb	r3, [r0, #0x2a4]
   379ec: eec67a26     	vdiv.f32	s15, s12, s13
   379f0: eebd7ac7     	vcvt.s32.f32	s14, s14
   379f4: ed807aa7     	vstr	s14, [r0, #668]
   379f8: eefd7ae7     	vcvt.s32.f32	s15, s15
   379fc: edc07aa8     	vstr	s15, [r0, #672]
   37a00: e12fff1e     	bx	lr
   37a04: 00 00 7a 44  	.word	0x447a0000
   37a08: cd cc 2c 40  	.word	0x402ccccd
   37a0c: cd cc ac 40  	.word	0x40accccd
