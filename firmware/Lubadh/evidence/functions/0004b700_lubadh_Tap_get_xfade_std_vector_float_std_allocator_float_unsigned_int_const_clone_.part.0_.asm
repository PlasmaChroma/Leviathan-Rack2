; lubadh::Tap::get_xfade(std::vector<float, std::allocator<float> >&, unsigned int) const [clone .part.0]
; VA 0x4b700 size 840

   4b700: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   4b704: e1a0e000     	mov	lr, r0
   4b708: e2526000     	subs	r6, r2, #0
   4b70c: 0a00001a     	beq	0x4b77c
   4b710: e2463001     	sub	r3, r6, #1
   4b714: e591c000     	ldr	r12, [r1]
   4b718: e3530002     	cmp	r3, #2
   4b71c: 9a0000c3     	bls	0x4ba30
   4b720: e1a02126     	lsr	r2, r6, #2
   4b724: e1a0300c     	mov	r3, r12
   4b728: f2c70f50     	vmov.f32	q8, #1.000000e+00
   4b72c: e08c2202     	add	r2, r12, r2, lsl #4
   4b730: f4430a8d     	vst1.32	{d16, d17}, [r3]!
   4b734: e1520003     	cmp	r2, r3
   4b738: 1afffffc     	bne	0x4b730
   4b73c: e3c63003     	bic	r3, r6, #3
   4b740: e1560003     	cmp	r6, r3
   4b744: e1a02003     	mov	r2, r3
   4b748: 0a00000b     	beq	0x4b77c
   4b74c: e08c3103     	add	r3, r12, r3, lsl #2
   4b750: e2820001     	add	r0, r2, #1
   4b754: e3a045fe     	mov	r4, #1065353216
   4b758: e1560000     	cmp	r6, r0
   4b75c: e5834000     	str	r4, [r3]
   4b760: 9a000005     	bls	0x4b77c
   4b764: e2822002     	add	r2, r2, #2
   4b768: e08c0100     	add	r0, r12, r0, lsl #2
   4b76c: e1560002     	cmp	r6, r2
   4b770: 808cc102     	addhi	r12, r12, r2, lsl #2
   4b774: e5804000     	str	r4, [r0]
   4b778: 858c4000     	strhi	r4, [r12]
   4b77c: ee076a90     	vmov	s15, r6
   4b780: e3034f98     	movw	r4, #0x3f98
   4b784: e3404009     	movt	r4, #0x9
   4b788: e28e7060     	add	r7, lr, #96
   4b78c: eeb84ae7     	vcvt.f32.s32	s8, s15
   4b790: e1a08106     	lsl	r8, r6, #2
   4b794: eddf3aa8     	vldr	s7, [pc, #672]          @ 0x4ba3c>&, unsigned int) const (.part.0)+0x33c> ; float 0.10000000149
   4b798: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4b79c: ed9f3aa7     	vldr	s6, [pc, #668]          @ 0x4ba40>&, unsigned int) const (.part.0)+0x340> ; float 254
   4b7a0: eddf4aa7     	vldr	s9, [pc, #668]          @ 0x4ba44>&, unsigned int) const (.part.0)+0x344> ; float 0
   4b7a4: e5de301c     	ldrb	r3, [lr, #0x1c]
   4b7a8: e3530000     	cmp	r3, #0
   4b7ac: 0a00005a     	beq	0x4b91c
   4b7b0: e59e2028     	ldr	r2, [lr, #0x28]
   4b7b4: e59e3020     	ldr	r3, [lr, #0x20]
   4b7b8: ee062a90     	vmov	s13, r2
   4b7bc: ee063a10     	vmov	s12, r3
   4b7c0: edde7a0b     	vldr	s15, [lr, #44]
   4b7c4: eef86ae6     	vcvt.f32.s32	s13, s13
   4b7c8: ed9e5a09     	vldr	s10, [lr, #36]
   4b7cc: eef85ac6     	vcvt.f32.s32	s11, s12
   4b7d0: ee766aa7     	vadd.f32	s13, s13, s15
   4b7d4: ee356a25     	vadd.f32	s12, s10, s11
   4b7d8: ee366a66     	vsub.f32	s12, s12, s13
   4b7dc: eef02ac6     	vabs.f32	s5, s12
   4b7e0: eef42ae3     	vcmpe.f32	s5, s7
   4b7e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4b7e8: 4a00004f     	bmi	0x4b92c
   4b7ec: ee865a04     	vdiv.f32	s10, s12, s8
   4b7f0: e3520000     	cmp	r2, #0
   4b7f4: ee822a84     	vdiv.f32	s4, s5, s8
   4b7f8: ba00005a     	blt	0x4b968
   4b7fc: e35200fe     	cmp	r2, #254
   4b800: d3a00000     	movle	r0, #0
   4b804: da00000e     	ble	0x4b844
   4b808: ee366ac3     	vsub.f32	s12, s13, s6
   4b80c: eec67a02     	vdiv.f32	s15, s12, s4
   4b810: eefd7ae7     	vcvt.s32.f32	s15, s15
   4b814: ee170a90     	vmov	r0, s15
   4b818: e2800001     	add	r0, r0, #1
   4b81c: e1500006     	cmp	r0, r6
   4b820: a1a00006     	movge	r0, r6
   4b824: e1c00fc0     	bic	r0, r0, r0, asr #31
   4b828: ee070a90     	vmov	s15, r0
   4b82c: eef87ae7     	vcvt.f32.s32	s15, s15
   4b830: eee26a67     	vfms.f32	s13, s4, s15
   4b834: eefd7ae6     	vcvt.s32.f32	s15, s13
   4b838: ee172a90     	vmov	r2, s15
   4b83c: eef87ae7     	vcvt.f32.s32	s15, s15
   4b840: ee767ae7     	vsub.f32	s15, s13, s15
   4b844: e3530000     	cmp	r3, #0
   4b848: ba000061     	blt	0x4b9d4
   4b84c: e35300fe     	cmp	r3, #254
   4b850: d1a03006     	movle	r3, r6
   4b854: da00000b     	ble	0x4b888
   4b858: ed9e6a09     	vldr	s12, [lr, #36]
   4b85c: ee356a86     	vadd.f32	s12, s11, s12
   4b860: ee366a43     	vsub.f32	s12, s12, s6
   4b864: eec66a02     	vdiv.f32	s13, s12, s4
   4b868: eefd6ae6     	vcvt.s32.f32	s13, s13
   4b86c: ee163a90     	vmov	r3, s13
   4b870: e2833001     	add	r3, r3, #1
   4b874: e1530006     	cmp	r3, r6
   4b878: a1a03006     	movge	r3, r6
   4b87c: e3530000     	cmp	r3, #0
   4b880: a0463003     	subge	r3, r6, r3
   4b884: b1a03006     	movlt	r3, r6
   4b888: e1530000     	cmp	r3, r0
   4b88c: da000022     	ble	0x4b91c
   4b890: e5919000     	ldr	r9, [r1]
   4b894: e0890100     	add	r0, r9, r0, lsl #2
   4b898: e0899103     	add	r9, r9, r3, lsl #2
   4b89c: e2823001     	add	r3, r2, #1
   4b8a0: e084c102     	add	r12, r4, r2, lsl #2
   4b8a4: edd05a00     	vldr	s11, [r0]
   4b8a8: e0845103     	add	r5, r4, r3, lsl #2
   4b8ac: eddc6a00     	vldr	s13, [r12]
   4b8b0: ed956a00     	vldr	s12, [r5]
   4b8b4: ee366a66     	vsub.f32	s12, s12, s13
   4b8b8: eee66a27     	vfma.f32	s13, s12, s15
   4b8bc: ee777a85     	vadd.f32	s15, s15, s10
   4b8c0: eef47ac7     	vcmpe.f32	s15, s14
   4b8c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4b8c8: ee666aa5     	vmul.f32	s13, s13, s11
   4b8cc: ece06a01     	vstmia	r0!, {s13}
   4b8d0: ba000005     	blt	0x4b8ec
   4b8d4: ee777ac7     	vsub.f32	s15, s15, s14
   4b8d8: e1a02003     	mov	r2, r3
   4b8dc: e2833001     	add	r3, r3, #1
   4b8e0: eef47ac7     	vcmpe.f32	s15, s14
   4b8e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4b8e8: aafffff9     	bge	0x4b8d4
   4b8ec: eef57ac0     	vcmpe.f32	s15, #0
   4b8f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4b8f4: 42423001     	submi	r3, r2, #1
   4b8f8: 5a000005     	bpl	0x4b914
   4b8fc: ee777a87     	vadd.f32	s15, s15, s14
   4b900: e1a02003     	mov	r2, r3
   4b904: e2433001     	sub	r3, r3, #1
   4b908: eef57ac0     	vcmpe.f32	s15, #0
   4b90c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4b910: 4afffff9     	bmi	0x4b8fc
   4b914: e1590000     	cmp	r9, r0
   4b918: 1affffdf     	bne	0x4b89c
   4b91c: e28ee018     	add	lr, lr, #24
   4b920: e157000e     	cmp	r7, lr
   4b924: 1affff9e     	bne	0x4b7a4
   4b928: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   4b92c: e0842103     	add	r2, r4, r3, lsl #2
   4b930: e3560000     	cmp	r6, #0
   4b934: edd26a00     	vldr	s13, [r2]
   4b938: edd27a01     	vldr	s15, [r2, #4]
   4b93c: ee777ae6     	vsub.f32	s15, s15, s13
   4b940: eee56a27     	vfma.f32	s13, s10, s15
   4b944: 0afffff4     	beq	0x4b91c
   4b948: e5913000     	ldr	r3, [r1]
   4b94c: e0832008     	add	r2, r3, r8
   4b950: edd37a00     	vldr	s15, [r3]
   4b954: ee667aa7     	vmul.f32	s15, s13, s15
   4b958: ece37a01     	vstmia	r3!, {s15}
   4b95c: e1530002     	cmp	r3, r2
   4b960: 1afffffa     	bne	0x4b950
   4b964: eaffffec     	b	0x4b91c
   4b968: ee346ae6     	vsub.f32	s12, s9, s13
   4b96c: eec67a02     	vdiv.f32	s15, s12, s4
   4b970: eefd7ae7     	vcvt.s32.f32	s15, s15
   4b974: ee170a90     	vmov	r0, s15
   4b978: e2800001     	add	r0, r0, #1
   4b97c: e1500006     	cmp	r0, r6
   4b980: a1a00006     	movge	r0, r6
   4b984: e3500000     	cmp	r0, #0
   4b988: e1c00fc0     	bic	r0, r0, r0, asr #31
   4b98c: ee070a90     	vmov	s15, r0
   4b990: eef87ae7     	vcvt.f32.s32	s15, s15
   4b994: eee26a27     	vfma.f32	s13, s4, s15
   4b998: eefd7ae6     	vcvt.s32.f32	s15, s13
   4b99c: ee172a90     	vmov	r2, s15
   4b9a0: eef87ae7     	vcvt.f32.s32	s15, s15
   4b9a4: ee767ae7     	vsub.f32	s15, s13, s15
   4b9a8: daffffa5     	ble	0x4b844
   4b9ac: e5915000     	ldr	r5, [r1]
   4b9b0: e3a0c000     	mov	r12, #0
   4b9b4: edd56a00     	vldr	s13, [r5]
   4b9b8: e28cc001     	add	r12, r12, #1
   4b9bc: e150000c     	cmp	r0, r12
   4b9c0: ee666aa4     	vmul.f32	s13, s13, s9
   4b9c4: ece56a01     	vstmia	r5!, {s13}
   4b9c8: cafffff9     	bgt	0x4b9b4
   4b9cc: e3530000     	cmp	r3, #0
   4b9d0: aaffff9d     	bge	0x4b84c
   4b9d4: ed9e6a09     	vldr	s12, [lr, #36]
   4b9d8: ee356a86     	vadd.f32	s12, s11, s12
   4b9dc: ee346ac6     	vsub.f32	s12, s9, s12
   4b9e0: eec66a02     	vdiv.f32	s13, s12, s4
   4b9e4: eefd6ae6     	vcvt.s32.f32	s13, s13
   4b9e8: ee163a90     	vmov	r3, s13
   4b9ec: e2833001     	add	r3, r3, #1
   4b9f0: e1530006     	cmp	r3, r6
   4b9f4: a1a03006     	movge	r3, r6
   4b9f8: e3530000     	cmp	r3, #0
   4b9fc: a0463003     	subge	r3, r6, r3
   4ba00: b1a03006     	movlt	r3, r6
   4ba04: e1560003     	cmp	r6, r3
   4ba08: daffff9e     	ble	0x4b888
   4ba0c: e591c000     	ldr	r12, [r1]
   4ba10: e08c5008     	add	r5, r12, r8
   4ba14: e08cc103     	add	r12, r12, r3, lsl #2
   4ba18: eddc6a00     	vldr	s13, [r12]
   4ba1c: ee666aa4     	vmul.f32	s13, s13, s9
   4ba20: ecec6a01     	vstmia	r12!, {s13}
   4ba24: e155000c     	cmp	r5, r12
   4ba28: 1afffffa     	bne	0x4ba18
   4ba2c: eaffff95     	b	0x4b888
   4ba30: e3a03000     	mov	r3, #0
   4ba34: e1a02003     	mov	r2, r3
   4ba38: eaffff43     	b	0x4b74c
   4ba3c: cd cc cc 3d  	.word	0x3dcccccd
   4ba40: 00 00 7e 43  	.word	0x437e0000
   4ba44: 00 00 00 00  	.word	0x00000000
