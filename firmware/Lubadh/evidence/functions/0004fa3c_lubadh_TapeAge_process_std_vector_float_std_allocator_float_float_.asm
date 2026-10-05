; lubadh::TapeAge::process(std::vector<float, std::allocator<float> >&, float)
; VA 0x4fa3c size 308

   4fa3c: e5912000     	ldr	r2, [r1]
   4fa40: e5913004     	ldr	r3, [r1, #0x4]
   4fa44: e92d4070     	push	{r4, r5, r6, lr}
   4fa48: e1a04001     	mov	r4, r1
   4fa4c: e0433002     	sub	r3, r3, r2
   4fa50: ed2d8b02     	vpush	{d8}
   4fa54: e1a05000     	mov	r5, r0
   4fa58: eeb08a40     	vmov.f32	s16, s0
   4fa5c: e1b0e143     	asrs	lr, r3, #2
   4fa60: 0a000026     	beq	0x4fb00
   4fa64: e5900048     	ldr	r0, [r0, #0x48]
   4fa68: e24e1001     	sub	r1, lr, #1
   4fa6c: e280c00f     	add	r12, r0, #15
   4fa70: e04cc002     	sub	r12, r12, r2
   4fa74: e35c001e     	cmp	r12, #30
   4fa78: 83510007     	cmphi	r1, #7
   4fa7c: 9a000035     	bls	0x4fb58
   4fa80: e1a0112e     	lsr	r1, lr, #2
   4fa84: e1a03002     	mov	r3, r2
   4fa88: e1a0c000     	mov	r12, r0
   4fa8c: e0821201     	add	r1, r2, r1, lsl #4
   4fa90: f4630a8d     	vld1.32	{d16, d17}, [r3]!
   4fa94: e1530001     	cmp	r3, r1
   4fa98: f44c0a8d     	vst1.32	{d16, d17}, [r12]!
   4fa9c: 1afffffb     	bne	0x4fa90
   4faa0: e31e0003     	tst	lr, #3
   4faa4: e3ce3003     	bic	r3, lr, #3
   4faa8: 0a000014     	beq	0x4fb00
   4faac: e1a0c103     	lsl	r12, r3, #2
   4fab0: e2831001     	add	r1, r3, #1
   4fab4: e082600c     	add	r6, r2, r12
   4fab8: e080c00c     	add	r12, r0, r12
   4fabc: e15e0001     	cmp	lr, r1
   4fac0: e5966000     	ldr	r6, [r6]
   4fac4: e58c6000     	str	r6, [r12]
   4fac8: 9a00000c     	bls	0x4fb00
   4facc: e1a01101     	lsl	r1, r1, #2
   4fad0: e2833002     	add	r3, r3, #2
   4fad4: e082c001     	add	r12, r2, r1
   4fad8: e0801001     	add	r1, r0, r1
   4fadc: e15e0003     	cmp	lr, r3
   4fae0: e59cc000     	ldr	r12, [r12]
   4fae4: e581c000     	str	r12, [r1]
   4fae8: 9a000004     	bls	0x4fb00
   4faec: e1a03103     	lsl	r3, r3, #2
   4faf0: e0822003     	add	r2, r2, r3
   4faf4: e0803003     	add	r3, r0, r3
   4faf8: e5922000     	ldr	r2, [r2]
   4fafc: e5832000     	str	r2, [r3]
   4fb00: e1a01004     	mov	r1, r4
   4fb04: e2850010     	add	r0, r5, #16
   4fb08: ebffed44     	bl	0x4b020
   4fb0c: e1a01004     	mov	r1, r4
   4fb10: e285003c     	add	r0, r5, #60
   4fb14: ebffed65     	bl	0x4b0b0
   4fb18: e5943000     	ldr	r3, [r4]
   4fb1c: e5941004     	ldr	r1, [r4, #0x4]
   4fb20: e0411003     	sub	r1, r1, r3
   4fb24: e1b02121     	lsrs	r2, r1, #2
   4fb28: 0a000008     	beq	0x4fb50
   4fb2c: e5952048     	ldr	r2, [r5, #0x48]
   4fb30: e0811002     	add	r1, r1, r2
   4fb34: ecf27a01     	vldmia	r2!, {s15}
   4fb38: ed937a00     	vldr	s14, [r3]
   4fb3c: e1520001     	cmp	r2, r1
   4fb40: ee377a67     	vsub.f32	s14, s14, s15
   4fb44: eee77a08     	vfma.f32	s15, s14, s16
   4fb48: ece37a01     	vstmia	r3!, {s15}
   4fb4c: 1afffff8     	bne	0x4fb34
   4fb50: ecbd8b02     	vpop	{d8}
   4fb54: e8bd8070     	pop	{r4, r5, r6, pc}
   4fb58: e0823003     	add	r3, r2, r3
   4fb5c: e4921004     	ldr	r1, [r2], #4
   4fb60: e4801004     	str	r1, [r0], #4
   4fb64: e1520003     	cmp	r2, r3
   4fb68: 1afffffb     	bne	0x4fb5c
   4fb6c: eaffffe3     	b	0x4fb00
