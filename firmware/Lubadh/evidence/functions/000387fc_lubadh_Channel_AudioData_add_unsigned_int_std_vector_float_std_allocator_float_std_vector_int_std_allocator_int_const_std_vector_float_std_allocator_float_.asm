; lubadh::Channel::AudioData::add(unsigned int, std::vector<float, std::allocator<float> >&, std::vector<int, std::allocator<int> > const&, std::vector<float, std::allocator<float> > const&)
; VA 0x387fc size 212

   387fc: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   38800: e1a05000     	mov	r5, r0
   38804: e5900000     	ldr	r0, [r0]
   38808: e1a08003     	mov	r8, r3
   3880c: e2517000     	subs	r7, r1, #0
   38810: e2800c62     	add	r0, r0, #25088
   38814: e59d3020     	ldr	r3, [sp, #0x20]
   38818: e1a09002     	mov	r9, r2
   3881c: e2800048     	add	r0, r0, #72
   38820: 0a000027     	beq	0x388c4
   38824: e593e000     	ldr	lr, [r3]
   38828: e1a07107     	lsl	r7, r7, #2
   3882c: e5986000     	ldr	r6, [r8]
   38830: e3023a2f     	movw	r3, #0x2a2f
   38834: e34031c2     	movt	r3, #0x1c2
   38838: e5951048     	ldr	r1, [r5, #0x48]
   3883c: e5924000     	ldr	r4, [r2]
   38840: e2466004     	sub	r6, r6, #4
   38844: e087200e     	add	r2, r7, lr
   38848: e5b6c004     	ldr	r12, [r6, #0x4]!
   3884c: ecbe7a01     	vldmia	lr!, {s14}
   38850: e15c0003     	cmp	r12, r3
   38854: edd47a00     	vldr	s15, [r4]
   38858: a1a0c003     	movge	r12, r3
   3885c: e152000e     	cmp	r2, lr
   38860: e1cccfcc     	bic	r12, r12, r12, asr #31
   38864: e081c10c     	add	r12, r1, r12, lsl #2
   38868: eddc6a00     	vldr	s13, [r12]
   3886c: eee67a87     	vfma.f32	s15, s13, s14
   38870: ece47a01     	vstmia	r4!, {s15}
   38874: 1afffff3     	bne	0x38848
   38878: e1a01009     	mov	r1, r9
   3887c: eb005d89     	bl	0x4fea8
   38880: e5992000     	ldr	r2, [r9]
   38884: e5981000     	ldr	r1, [r8]
   38888: e3020a2f     	movw	r0, #0x2a2f
   3888c: e34001c2     	movt	r0, #0x1c2
   38890: e595e048     	ldr	lr, [r5, #0x48]
   38894: e0877002     	add	r7, r7, r2
   38898: e2411004     	sub	r1, r1, #4
   3889c: e5b13004     	ldr	r3, [r1, #0x4]!
   388a0: e492c004     	ldr	r12, [r2], #4
   388a4: e1530000     	cmp	r3, r0
   388a8: a1a03000     	movge	r3, r0
   388ac: e1570002     	cmp	r7, r2
   388b0: e1c33fc3     	bic	r3, r3, r3, asr #31
   388b4: e08e3103     	add	r3, lr, r3, lsl #2
   388b8: e583c000     	str	r12, [r3]
   388bc: 1afffff6     	bne	0x3889c
   388c0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   388c4: e8bd47f0     	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
   388c8: e1a01002     	mov	r1, r2
   388cc: ea005d75     	b	0x4fea8
