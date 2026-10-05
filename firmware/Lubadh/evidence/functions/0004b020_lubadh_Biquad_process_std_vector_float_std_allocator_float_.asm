; lubadh::Biquad::process(std::vector<float, std::allocator<float> >&)
; VA 0x4b020 size 124

   4b020: e52de004     	str	lr, [sp, #-0x4]!
   4b024: e8914004     	ldm	r1, {r2, lr}
   4b028: e04ee002     	sub	lr, lr, r2
   4b02c: e1b0312e     	lsrs	r3, lr, #2
   4b030: 049df004     	ldreq	pc, [sp], #4
   4b034: e590301c     	ldr	r3, [r0, #0x1c]
   4b038: e082e00e     	add	lr, r2, lr
   4b03c: e590c004     	ldr	r12, [r0, #0x4]
   4b040: e5901010     	ldr	r1, [r0, #0x10]
   4b044: edd37a01     	vldr	s15, [r3, #4]
   4b048: edd36a02     	vldr	s13, [r3, #8]
   4b04c: eddc5a01     	vldr	s11, [r12, #4]
   4b050: ed927a00     	vldr	s14, [r2]
   4b054: ed9c6a02     	vldr	s12, [r12, #8]
   4b058: eea57ae7     	vfms.f32	s14, s11, s15
   4b05c: eea67a66     	vfms.f32	s14, s12, s13
   4b060: ed837a00     	vstr	s14, [r3]
   4b064: ed915a01     	vldr	s10, [r1, #4]
   4b068: edd15a00     	vldr	s11, [r1]
   4b06c: ed916a02     	vldr	s12, [r1, #8]
   4b070: ee677a85     	vmul.f32	s15, s15, s10
   4b074: eee77a25     	vfma.f32	s15, s14, s11
   4b078: eee67a26     	vfma.f32	s15, s12, s13
   4b07c: ece27a01     	vstmia	r2!, {s15}
   4b080: e15e0002     	cmp	lr, r2
   4b084: edd36a01     	vldr	s13, [r3, #4]
   4b088: edd37a00     	vldr	s15, [r3]
   4b08c: edc36a02     	vstr	s13, [r3, #8]
   4b090: edc37a01     	vstr	s15, [r3, #4]
   4b094: 1affffec     	bne	0x4b04c
   4b098: e49df004     	ldr	pc, [sp], #4
