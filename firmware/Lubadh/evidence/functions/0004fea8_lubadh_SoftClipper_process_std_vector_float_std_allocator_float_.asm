; lubadh::SoftClipper::process(std::vector<float, std::allocator<float> >&)
; VA 0x4fea8 size 312

   4fea8: e5d03008     	ldrb	r3, [r0, #0x8]
   4feac: e5912004     	ldr	r2, [r1, #0x4]
   4feb0: e3530000     	cmp	r3, #0
   4feb4: e5913000     	ldr	r3, [r1]
   4feb8: 1a00002c     	bne	0x4ff70
   4febc: e1520003     	cmp	r2, r3
   4fec0: 012fff1e     	bxeq	lr
   4fec4: eef76a00     	vmov.f32	s13, #1.000000e+00
   4fec8: eebf5a00     	vmov.f32	s10, #-1.000000e+00
   4fecc: ea00000c     	b	0x4ff04
   4fed0: ee777ac7     	vsub.f32	s15, s15, s14
   4fed4: ee765ac7     	vsub.f32	s11, s13, s14
   4fed8: ee876aa5     	vdiv.f32	s12, s15, s11
   4fedc: eef05a66     	vmov.f32	s11, s13
   4fee0: eee65a06     	vfma.f32	s11, s12, s12
   4fee4: ee876aa5     	vdiv.f32	s12, s15, s11
   4fee8: ee767a07     	vadd.f32	s15, s12, s14
   4feec: ed437a01     	vstr	s15, [r3, #-4]
   4fef0: ed907a01     	vldr	s14, [r0, #4]
   4fef4: e1530002     	cmp	r3, r2
   4fef8: ee677a27     	vmul.f32	s15, s14, s15
   4fefc: ed437a01     	vstr	s15, [r3, #-4]
   4ff00: 012fff1e     	bxeq	lr
   4ff04: ecf37a01     	vldmia	r3!, {s15}
   4ff08: eef47ae6     	vcmpe.f32	s15, s13
   4ff0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ff10: 5ef77a00     	vmovpl.f32	s15, #1.000000e+00
   4ff14: 5a000002     	bpl	0x4ff24
   4ff18: eef47ac5     	vcmpe.f32	s15, s10
   4ff1c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ff20: def07a45     	vmovle.f32	s15, s10
   4ff24: ed437a01     	vstr	s15, [r3, #-4]
   4ff28: ed907a00     	vldr	s14, [r0]
   4ff2c: eeb47ae7     	vcmpe.f32	s14, s15
   4ff30: eeb16a47     	vneg.f32	s12, s14
   4ff34: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ff38: 4affffe4     	bmi	0x4fed0
   4ff3c: eeb46ae7     	vcmpe.f32	s12, s15
   4ff40: eef15a67     	vneg.f32	s11, s15
   4ff44: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ff48: daffffe8     	ble	0x4fef0
   4ff4c: ee755ac7     	vsub.f32	s11, s11, s14
   4ff50: ee367ac7     	vsub.f32	s14, s13, s14
   4ff54: eec57a87     	vdiv.f32	s15, s11, s14
   4ff58: eeb07a66     	vmov.f32	s14, s13
   4ff5c: eea77aa7     	vfma.f32	s14, s15, s15
   4ff60: eec57a87     	vdiv.f32	s15, s11, s14
   4ff64: ee767a67     	vsub.f32	s15, s12, s15
   4ff68: ed437a01     	vstr	s15, [r3, #-4]
   4ff6c: eaffffdf     	b	0x4fef0
   4ff70: e1520003     	cmp	r2, r3
   4ff74: 012fff1e     	bxeq	lr
   4ff78: ed9f6a16     	vldr	s12, [pc, #88]          @ 0x4ffd8>&)+0x130> ; float 28.2743339539
   4ff7c: eeb75a00     	vmov.f32	s10, #1.000000e+00
   4ff80: eddf4a15     	vldr	s9, [pc, #84]           @ 0x4ffdc>&)+0x134> ; float 9.42477798462
   4ff84: eeff5a00     	vmov.f32	s11, #-1.000000e+00
   4ff88: ed907a01     	vldr	s14, [r0, #4]
   4ff8c: eeb04a46     	vmov.f32	s8, s12
   4ff90: ecf37a01     	vldmia	r3!, {s15}
   4ff94: ee677a87     	vmul.f32	s15, s15, s14
   4ff98: ee676aa7     	vmul.f32	s13, s15, s15
   4ff9c: ee367a86     	vadd.f32	s14, s13, s12
   4ffa0: eea64aa4     	vfma.f32	s8, s13, s9
   4ffa4: ee677a27     	vmul.f32	s15, s14, s15
   4ffa8: ee877a84     	vdiv.f32	s14, s15, s8
   4ffac: eeb47ac5     	vcmpe.f32	s14, s10
   4ffb0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ffb4: 5eb77a00     	vmovpl.f32	s14, #1.000000e+00
   4ffb8: 5a000002     	bpl	0x4ffc8
   4ffbc: eeb47ae5     	vcmpe.f32	s14, s11
   4ffc0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ffc4: deb07a65     	vmovle.f32	s14, s11
   4ffc8: e1530002     	cmp	r3, r2
   4ffcc: ed037a01     	vstr	s14, [r3, #-4]
   4ffd0: 1affffec     	bne	0x4ff88
   4ffd4: e12fff1e     	bx	lr
   4ffd8: d6 31 e2 41  	.word	0x41e231d6
   4ffdc: e4 cb 16 41  	.word	0x4116cbe4
