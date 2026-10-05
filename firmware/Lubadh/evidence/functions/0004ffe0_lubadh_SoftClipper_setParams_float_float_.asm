; lubadh::SoftClipper::setParams(float, float)
; VA 0x4ffe0 size 96

   4ffe0: eef71ac0     	vcvt.f64.f32	d17, s0
   4ffe4: eddf0b13     	vldr	d16, [pc, #76]          @ 0x50038 ; float 0.01
   4ffe8: eeb07a60     	vmov.f32	s14, s1
   4ffec: eef41be0     	vcmpe.f64	d17, d16
   4fff0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fff4: da00000a     	ble	0x50024
   4fff8: eef70a00     	vmov.f32	s1, #1.000000e+00
   4fffc: ed800a00     	vstr	s0, [r0]
   50000: eef06a00     	vmov.f32	s13, #2.000000e+00
   50004: e3a03000     	mov	r3, #0
   50008: ee300a20     	vadd.f32	s0, s0, s1
   5000c: eec67a80     	vdiv.f32	s15, s13, s0
   50010: ee777ae0     	vsub.f32	s15, s15, s1
   50014: eee70a27     	vfma.f32	s1, s14, s15
   50018: e5c03008     	strb	r3, [r0, #0x8]
   5001c: edc00a01     	vstr	s1, [r0, #4]
   50020: e12fff1e     	bx	lr
   50024: eef77a00     	vmov.f32	s15, #1.000000e+00
   50028: e3a03001     	mov	r3, #1
   5002c: ee700aa7     	vadd.f32	s1, s1, s15
   50030: eafffff8     	b	0x50018
   50034: e320f000     	nop
   50038: 7b 14 ae 47  	.word	0x47ae147b
   5003c: e1 7a 84 3f  	.word	0x3f847ae1
