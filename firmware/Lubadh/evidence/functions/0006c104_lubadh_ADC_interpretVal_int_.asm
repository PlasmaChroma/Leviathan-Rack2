; lubadh::ADC::interpretVal(int)
; VA 0x6c104 size 56

   6c104: ed906a00     	vldr	s12, [r0]
   6c108: eef77a00     	vmov.f32	s15, #1.000000e+00
   6c10c: ee071a10     	vmov	s14, r1
   6c110: e1a03000     	mov	r3, r0
   6c114: ee776ac6     	vsub.f32	s13, s15, s12
   6c118: edd07a01     	vldr	s15, [r0, #4]
   6c11c: eeb87ac7     	vcvt.f32.s32	s14, s14
   6c120: eef87ae7     	vcvt.f32.s32	s15, s15
   6c124: ee677aa6     	vmul.f32	s15, s15, s13
   6c128: eee77a06     	vfma.f32	s15, s14, s12
   6c12c: eefd7ae7     	vcvt.s32.f32	s15, s15
   6c130: ee170a90     	vmov	r0, s15
   6c134: edc37a01     	vstr	s15, [r3, #4]
   6c138: e12fff1e     	bx	lr
