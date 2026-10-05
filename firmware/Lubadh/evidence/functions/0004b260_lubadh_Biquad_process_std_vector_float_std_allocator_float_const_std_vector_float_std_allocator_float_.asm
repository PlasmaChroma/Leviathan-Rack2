; lubadh::Biquad::process(std::vector<float, std::allocator<float> > const&, std::vector<float, std::allocator<float> >&)
; VA 0x4b260 size 268

   4b260: e92d4070     	push	{r4, r5, r6, lr}
   4b264: e1a04002     	mov	r4, r2
   4b268: e591c000     	ldr	r12, [r1]
   4b26c: e5913004     	ldr	r3, [r1, #0x4]
   4b270: e1a05000     	mov	r5, r0
   4b274: e5922000     	ldr	r2, [r2]
   4b278: e1a06001     	mov	r6, r1
   4b27c: e5940004     	ldr	r0, [r4, #0x4]
   4b280: e043300c     	sub	r3, r3, r12
   4b284: e0401002     	sub	r1, r0, r2
   4b288: e1a0e143     	asr	lr, r3, #2
   4b28c: e1530001     	cmp	r3, r1
   4b290: 1a00001c     	bne	0x4b308
   4b294: e35e0000     	cmp	lr, #0
   4b298: 08bd8070     	popeq	{r4, r5, r6, pc}
   4b29c: e595301c     	ldr	r3, [r5, #0x1c]
   4b2a0: e08c110e     	add	r1, r12, lr, lsl #2
   4b2a4: e5944000     	ldr	r4, [r4]
   4b2a8: e595e004     	ldr	lr, [r5, #0x4]
   4b2ac: e5952010     	ldr	r2, [r5, #0x10]
   4b2b0: edd37a01     	vldr	s15, [r3, #4]
   4b2b4: edd36a02     	vldr	s13, [r3, #8]
   4b2b8: edde5a01     	vldr	s11, [lr, #4]
   4b2bc: ecbc7a01     	vldmia	r12!, {s14}
   4b2c0: ed9e6a02     	vldr	s12, [lr, #8]
   4b2c4: eea57ae7     	vfms.f32	s14, s11, s15
   4b2c8: e151000c     	cmp	r1, r12
   4b2cc: eea67a66     	vfms.f32	s14, s12, s13
   4b2d0: ed837a00     	vstr	s14, [r3]
   4b2d4: ed925a01     	vldr	s10, [r2, #4]
   4b2d8: edd25a00     	vldr	s11, [r2]
   4b2dc: ed926a02     	vldr	s12, [r2, #8]
   4b2e0: ee677a85     	vmul.f32	s15, s15, s10
   4b2e4: eee77a25     	vfma.f32	s15, s14, s11
   4b2e8: eee67a26     	vfma.f32	s15, s12, s13
   4b2ec: ece47a01     	vstmia	r4!, {s15}
   4b2f0: edd36a01     	vldr	s13, [r3, #4]
   4b2f4: edd37a00     	vldr	s15, [r3]
   4b2f8: edc36a02     	vstr	s13, [r3, #8]
   4b2fc: edc37a01     	vstr	s15, [r3, #4]
   4b300: 1affffec     	bne	0x4b2b8
   4b304: e8bd8070     	pop	{r4, r5, r6, pc}
   4b308: e1a01141     	asr	r1, r1, #2
   4b30c: e15e0001     	cmp	lr, r1
   4b310: 8a00000b     	bhi	0x4b344
   4b314: 2affffde     	bhs	0x4b294
   4b318: e0823003     	add	r3, r2, r3
   4b31c: e1500003     	cmp	r0, r3
   4b320: 0affffdb     	beq	0x4b294
   4b324: e5843004     	str	r3, [r4, #0x4]
   4b328: e596c000     	ldr	r12, [r6]
   4b32c: e5961004     	ldr	r1, [r6, #0x4]
   4b330: e041100c     	sub	r1, r1, r12
   4b334: e1a0e141     	asr	lr, r1, #2
   4b338: e35e0000     	cmp	lr, #0
   4b33c: 1affffd6     	bne	0x4b29c
   4b340: e8bd8070     	pop	{r4, r5, r6, pc}
   4b344: e04e1001     	sub	r1, lr, r1
   4b348: e1a00004     	mov	r0, r4
   4b34c: ebffd2f9     	bl	0x3ff38
   4b350: e596c000     	ldr	r12, [r6]
   4b354: e5961004     	ldr	r1, [r6, #0x4]
   4b358: e041100c     	sub	r1, r1, r12
   4b35c: e1a0e141     	asr	lr, r1, #2
   4b360: e35e0000     	cmp	lr, #0
   4b364: 1affffcc     	bne	0x4b29c
   4b368: e8bd8070     	pop	{r4, r5, r6, pc}
