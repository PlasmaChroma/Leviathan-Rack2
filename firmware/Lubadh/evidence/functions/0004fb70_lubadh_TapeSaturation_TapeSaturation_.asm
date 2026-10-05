; lubadh::TapeSaturation::TapeSaturation()
; VA 0x4fb70 size 24

   4fb70: eddf0b02     	vldr	d16, [pc, #8]           @ 0x4fb80 ; float 3.81469874133e-07
   4fb74: f440078f     	vst1.32	{d16}, [r0]
   4fb78: e12fff1e     	bx	lr
   4fb7c: e320f000     	nop
   4fb80: 00 00 c0 3f  	.word	0x3fc00000
   4fb84: 9a 99 99 3e  	.word	0x3e99999a
