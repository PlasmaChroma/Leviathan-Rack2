; lubadh::TapeAllpass::process(std::vector<float, std::allocator<float> >&)
; VA 0x4fce8 size 436

   4fce8: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4fcec: e5913004     	ldr	r3, [r1, #0x4]
   4fcf0: e5916000     	ldr	r6, [r1]
   4fcf4: e24dd044     	sub	sp, sp, #68
   4fcf8: e0433006     	sub	r3, r3, r6
   4fcfc: e1b02123     	lsrs	r2, r3, #2
   4fd00: 0a000062     	beq	0x4fe90
   4fd04: e2802a01     	add	r2, r0, #4096
   4fd08: e2808d5e     	add	r8, r0, #6016
   4fd0c: e1a0e002     	mov	lr, r2
   4fd10: e58d201c     	str	r2, [sp, #0x1c]
   4fd14: e1a09008     	mov	r9, r8
   4fd18: e1a0b008     	mov	r11, r8
   4fd1c: e592c774     	ldr	r12, [r2, #0x774]
   4fd20: e0863003     	add	r3, r6, r3
   4fd24: e59e4794     	ldr	r4, [lr, #0x794]
   4fd28: e2888004     	add	r8, r8, #4
   4fd2c: e5921778     	ldr	r1, [r2, #0x778]
   4fd30: e2899008     	add	r9, r9, #8
   4fd34: e58d4004     	str	r4, [sp, #0x4]
   4fd38: e28bb00c     	add	r11, r11, #12
   4fd3c: e59e4798     	ldr	r4, [lr, #0x798]
   4fd40: e592277c     	ldr	r2, [r2, #0x77c]
   4fd44: e58d3010     	str	r3, [sp, #0x10]
   4fd48: e58d4008     	str	r4, [sp, #0x8]
   4fd4c: e59e3780     	ldr	r3, [lr, #0x780]
   4fd50: e59e479c     	ldr	r4, [lr, #0x79c]
   4fd54: eddf4a4f     	vldr	s9, [pc, #316]          @ 0x4fe98>&)+0x1b0> ; float 0.17499999702
   4fd58: e59ee7a0     	ldr	lr, [lr, #0x7a0]
   4fd5c: e58d400c     	str	r4, [sp, #0xc]
   4fd60: e58de000     	str	lr, [sp]
   4fd64: e58d9014     	str	r9, [sp, #0x14]
   4fd68: e58db018     	str	r11, [sp, #0x18]
   4fd6c: e080710c     	add	r7, r0, r12, lsl #2
   4fd70: edd66a00     	vldr	s13, [r6]
   4fd74: e0805101     	add	r5, r0, r1, lsl #2
   4fd78: e0804102     	add	r4, r0, r2, lsl #2
   4fd7c: e2855e5e     	add	r5, r5, #1504
   4fd80: e2844ebb     	add	r4, r4, #2992
   4fd84: ed975a01     	vldr	s10, [r7, #4]
   4fd88: e080e103     	add	lr, r0, r3, lsl #2
   4fd8c: e28eea01     	add	lr, lr, #4096
   4fd90: e59da004     	ldr	r10, [sp, #0x4]
   4fd94: edd55a00     	vldr	s11, [r5]
   4fd98: e28cc001     	add	r12, r12, #1
   4fd9c: ee367a85     	vadd.f32	s14, s13, s10
   4fda0: ed946a03     	vldr	s12, [r4, #12]
   4fda4: edde7a66     	vldr	s15, [lr, #408]
   4fda8: e2811001     	add	r1, r1, #1
   4fdac: ed8d5a0c     	vstr	s10, [sp, #48]
   4fdb0: e2822001     	add	r2, r2, #1
   4fdb4: edcd5a0d     	vstr	s11, [sp, #52]
   4fdb8: e284400c     	add	r4, r4, #12
   4fdbc: ee377a25     	vadd.f32	s14, s14, s11
   4fdc0: edcd7a0f     	vstr	s15, [sp, #60]
   4fdc4: ed8d6a0e     	vstr	s12, [sp, #56]
   4fdc8: e2833001     	add	r3, r3, #1
   4fdcc: eddd0b0c     	vldr	d16, [sp, #48]
   4fdd0: eddd1b0e     	vldr	d17, [sp, #56]
   4fdd4: e719fa1c     	sdiv	r9, r12, r10
   4fdd8: ee377a06     	vadd.f32	s14, s14, s12
   4fddc: f4480a8f     	vst1.32	{d16, d17}, [r8]
   4fde0: ed906a00     	vldr	s12, [r0]
   4fde4: ee377a27     	vadd.f32	s14, s14, s15
   4fde8: eef07a66     	vmov.f32	s15, s13
   4fdec: e06cc99a     	mls	r12, r10, r9, r12
   4fdf0: e59da008     	ldr	r10, [sp, #0x8]
   4fdf4: eed77a24     	vfnms.f32	s15, s14, s9
   4fdf8: eeb07a66     	vmov.f32	s14, s13
   4fdfc: e719fa11     	sdiv	r9, r1, r10
   4fe00: e061199a     	mls	r1, r10, r9, r1
   4fe04: e59da00c     	ldr	r10, [sp, #0xc]
   4fe08: e59d9000     	ldr	r9, [sp]
   4fe0c: eea67a27     	vfma.f32	s14, s12, s15
   4fe10: e71bfa12     	sdiv	r11, r2, r10
   4fe14: e719f913     	sdiv	r9, r3, r9
   4fe18: e0622b9a     	mls	r2, r10, r11, r2
   4fe1c: e59da000     	ldr	r10, [sp]
   4fe20: eca67a01     	vstmia	r6!, {s14}
   4fe24: edc76a01     	vstr	s13, [r7, #4]
   4fe28: e063399a     	mls	r3, r10, r9, r3
   4fe2c: edd87a00     	vldr	s15, [r8]
   4fe30: e59d9010     	ldr	r9, [sp, #0x10]
   4fe34: ee367ae7     	vsub.f32	s14, s13, s15
   4fe38: ee777aa6     	vadd.f32	s15, s15, s13
   4fe3c: e1560009     	cmp	r6, r9
   4fe40: ed857a00     	vstr	s14, [r5]
   4fe44: e59d5014     	ldr	r5, [sp, #0x14]
   4fe48: ed957a00     	vldr	s14, [r5]
   4fe4c: ee776ac7     	vsub.f32	s13, s15, s14
   4fe50: ee777a87     	vadd.f32	s15, s15, s14
   4fe54: edc46a00     	vstr	s13, [r4]
   4fe58: e59d4018     	ldr	r4, [sp, #0x18]
   4fe5c: ed947a00     	vldr	s14, [r4]
   4fe60: ee777ac7     	vsub.f32	s15, s15, s14
   4fe64: edce7a66     	vstr	s15, [lr, #408]
   4fe68: 1affffbf     	bne	0x4fd6c
   4fe6c: e1cd22f8     	strd	r2, r3, [sp, #40]
   4fe70: e59d001c     	ldr	r0, [sp, #0x1c]
   4fe74: e58dc020     	str	r12, [sp, #0x20]
   4fe78: e58d1024     	str	r1, [sp, #0x24]
   4fe7c: e2800e77     	add	r0, r0, #1904
   4fe80: eddd0b08     	vldr	d16, [sp, #32]
   4fe84: eddd1b0a     	vldr	d17, [sp, #40]
   4fe88: e2800004     	add	r0, r0, #4
   4fe8c: f4400a8f     	vst1.32	{d16, d17}, [r0]
   4fe90: e28dd044     	add	sp, sp, #68
   4fe94: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4fe98: 33 33 33 3e  	.word	0x3e333333
