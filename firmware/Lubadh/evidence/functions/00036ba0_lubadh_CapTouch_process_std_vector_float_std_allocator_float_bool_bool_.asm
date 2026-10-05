; lubadh::CapTouch::process(std::vector<float, std::allocator<float> >&, bool, bool)
; VA 0x36ba0 size 384

   36ba0: e590c000     	ldr	r12, [r0]
   36ba4: e59cc0e8     	ldr	r12, [r12, #0xe8]
   36ba8: e59cc0b0     	ldr	r12, [r12, #0xb0]
   36bac: e59cc03c     	ldr	r12, [r12, #0x3c]
   36bb0: e35c0000     	cmp	r12, #0
   36bb4: 012fff1e     	bxeq	lr
   36bb8: e35c0001     	cmp	r12, #1
   36bbc: 0a00001f     	beq	0x36c40
   36bc0: e35c0002     	cmp	r12, #2
   36bc4: 112fff1e     	bxne	lr
   36bc8: e5902008     	ldr	r2, [r0, #0x8]
   36bcc: e3520001     	cmp	r2, #1
   36bd0: 0a00003d     	beq	0x36ccc
   36bd4: e3520002     	cmp	r2, #2
   36bd8: 0a000047     	beq	0x36cfc
   36bdc: e3520000     	cmp	r2, #0
   36be0: 0a000040     	beq	0x36ce8
   36be4: e5913000     	ldr	r3, [r1]
   36be8: e5912004     	ldr	r2, [r1, #0x4]
   36bec: e0422003     	sub	r2, r2, r3
   36bf0: e1b01122     	lsrs	r1, r2, #2
   36bf4: 012fff1e     	bxeq	lr
   36bf8: e0832002     	add	r2, r3, r2
   36bfc: eef76a00     	vmov.f32	s13, #1.000000e+00
   36c00: edd07a01     	vldr	s15, [r0, #4]
   36c04: ed907a07     	vldr	s14, [r0, #28]
   36c08: ee767a27     	vadd.f32	s15, s12, s15
   36c0c: eef47ae6     	vcmpe.f32	s15, s13
   36c10: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36c14: 5ef07a66     	vmovpl.f32	s15, s13
   36c18: eeb47ae7     	vcmpe.f32	s14, s15
   36c1c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36c20: 5a000022     	bpl	0x36cb0
   36c24: edc07a01     	vstr	s15, [r0, #4]
   36c28: ed937a00     	vldr	s14, [r3]
   36c2c: ee677a27     	vmul.f32	s15, s14, s15
   36c30: ece37a01     	vstmia	r3!, {s15}
   36c34: e1520003     	cmp	r2, r3
   36c38: 1afffff0     	bne	0x36c00
   36c3c: e12fff1e     	bx	lr
   36c40: e3520000     	cmp	r2, #0
   36c44: e5913000     	ldr	r3, [r1]
   36c48: e5912004     	ldr	r2, [r1, #0x4]
   36c4c: edd06a03     	vldr	s13, [r0, #12]
   36c50: edd07a04     	vldr	s15, [r0, #16]
   36c54: e0422003     	sub	r2, r2, r3
   36c58: 0ef06a67     	vmoveq.f32	s13, s15
   36c5c: e1b01122     	lsrs	r1, r2, #2
   36c60: 012fff1e     	bxeq	lr
   36c64: eddf5a2c     	vldr	s11, [pc, #176]         @ 0x36d1c>&, bool, bool)+0x17c> ; float 0
   36c68: e0832002     	add	r2, r3, r2
   36c6c: eeb76a00     	vmov.f32	s12, #1.000000e+00
   36c70: edd07a01     	vldr	s15, [r0, #4]
   36c74: ee767aa7     	vadd.f32	s15, s13, s15
   36c78: eef47ac6     	vcmpe.f32	s15, s12
   36c7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36c80: 5ef77a00     	vmovpl.f32	s15, #1.000000e+00
   36c84: 5a000002     	bpl	0x36c94
   36c88: eef57ac0     	vcmpe.f32	s15, #0
   36c8c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36c90: def07a65     	vmovle.f32	s15, s11
   36c94: edc07a01     	vstr	s15, [r0, #4]
   36c98: ed937a00     	vldr	s14, [r3]
   36c9c: ee677a87     	vmul.f32	s15, s15, s14
   36ca0: ece37a01     	vstmia	r3!, {s15}
   36ca4: e1520003     	cmp	r2, r3
   36ca8: 1afffff0     	bne	0x36c70
   36cac: e12fff1e     	bx	lr
   36cb0: ed807a01     	vstr	s14, [r0, #4]
   36cb4: edd37a00     	vldr	s15, [r3]
   36cb8: ee277a27     	vmul.f32	s14, s14, s15
   36cbc: eca37a01     	vstmia	r3!, {s14}
   36cc0: e1520003     	cmp	r2, r3
   36cc4: 1affffcd     	bne	0x36c00
   36cc8: e12fff1e     	bx	lr
   36ccc: ed907a01     	vldr	s14, [r0, #4]
   36cd0: edd07a07     	vldr	s15, [r0, #28]
   36cd4: ed906a05     	vldr	s12, [r0, #20]
   36cd8: eeb47ae7     	vcmpe.f32	s14, s15
   36cdc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36ce0: 9580c008     	strls	r12, [r0, #0x8]
   36ce4: eaffffbe     	b	0x36be4
   36ce8: e3530000     	cmp	r3, #0
   36cec: 13a03001     	movne	r3, #1
   36cf0: 1d9f6a09     	vldrne	s12, [pc, #36]          @ 0x36d1c>&, bool, bool)+0x17c> ; float 0
   36cf4: 15803008     	strne	r3, [r0, #0x8]
   36cf8: eaffffb9     	b	0x36be4
   36cfc: ed907a01     	vldr	s14, [r0, #4]
   36d00: eef77a00     	vmov.f32	s15, #1.000000e+00
   36d04: ed906a06     	vldr	s12, [r0, #24]
   36d08: eeb47ae7     	vcmpe.f32	s14, s15
   36d0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36d10: a3a03000     	movge	r3, #0
   36d14: a5803008     	strge	r3, [r0, #0x8]
   36d18: eaffffb1     	b	0x36be4
   36d1c: 00 00 00 00  	.word	0x00000000
