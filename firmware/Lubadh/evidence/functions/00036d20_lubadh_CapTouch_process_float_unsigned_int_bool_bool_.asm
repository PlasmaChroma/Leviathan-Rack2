; lubadh::CapTouch::process(float, unsigned int, bool, bool)
; VA 0x36d20 size 296

   36d20: e24dd008     	sub	sp, sp, #8
   36d24: e58d1004     	str	r1, [sp, #0x4]
   36d28: e5901000     	ldr	r1, [r0]
   36d2c: e59110e8     	ldr	r1, [r1, #0xe8]
   36d30: e59110b0     	ldr	r1, [r1, #0xb0]
   36d34: e591103c     	ldr	r1, [r1, #0x3c]
   36d38: e3510001     	cmp	r1, #1
   36d3c: 0a000018     	beq	0x36da4
   36d40: e3510002     	cmp	r1, #2
   36d44: 1a000014     	bne	0x36d9c
   36d48: eddd7a01     	vldr	s15, [sp, #4]
   36d4c: e5902008     	ldr	r2, [r0, #0x8]
   36d50: ed907a07     	vldr	s14, [r0, #28]
   36d54: eef87a67     	vcvt.f32.u32	s15, s15
   36d58: e3520001     	cmp	r2, #1
   36d5c: edd06a01     	vldr	s13, [r0, #4]
   36d60: 0a000023     	beq	0x36df4
   36d64: e3520002     	cmp	r2, #2
   36d68: 0a00002d     	beq	0x36e24
   36d6c: e3520000     	cmp	r2, #0
   36d70: 0a000025     	beq	0x36e0c
   36d74: ee777aa6     	vadd.f32	s15, s15, s13
   36d78: eef76a00     	vmov.f32	s13, #1.000000e+00
   36d7c: eef47ae6     	vcmpe.f32	s15, s13
   36d80: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36d84: 5ef07a66     	vmovpl.f32	s15, s13
   36d88: eef47ac7     	vcmpe.f32	s15, s14
   36d8c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36d90: def07a47     	vmovle.f32	s15, s14
   36d94: ee200a27     	vmul.f32	s0, s0, s15
   36d98: edc07a01     	vstr	s15, [r0, #4]
   36d9c: e28dd008     	add	sp, sp, #8
   36da0: e12fff1e     	bx	lr
   36da4: e3520000     	cmp	r2, #0
   36da8: eddd7a01     	vldr	s15, [sp, #4]
   36dac: ed907a03     	vldr	s14, [r0, #12]
   36db0: eef76a00     	vmov.f32	s13, #1.000000e+00
   36db4: edd05a04     	vldr	s11, [r0, #16]
   36db8: eeb86a67     	vcvt.f32.u32	s12, s15
   36dbc: edd07a01     	vldr	s15, [r0, #4]
   36dc0: 0eb07a65     	vmoveq.f32	s14, s11
   36dc4: eee67a07     	vfma.f32	s15, s12, s14
   36dc8: eef47ae6     	vcmpe.f32	s15, s13
   36dcc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36dd0: 5ef07a66     	vmovpl.f32	s15, s13
   36dd4: 5affffef     	bpl	0x36d98
   36dd8: eef57ac0     	vcmpe.f32	s15, #0
   36ddc: ed9f7a18     	vldr	s14, [pc, #96]          @ 0x36e44 ; float 0
   36de0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36de4: de200a07     	vmulle.f32	s0, s0, s14
   36de8: def07a47     	vmovle.f32	s15, s14
   36dec: daffffe9     	ble	0x36d98
   36df0: eaffffe7     	b	0x36d94
   36df4: eef46ac7     	vcmpe.f32	s13, s14
   36df8: ed906a05     	vldr	s12, [r0, #20]
   36dfc: ee677a86     	vmul.f32	s15, s15, s12
   36e00: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36e04: 95801008     	strls	r1, [r0, #0x8]
   36e08: eaffffd9     	b	0x36d74
   36e0c: e3530000     	cmp	r3, #0
   36e10: 1d9f6a0b     	vldrne	s12, [pc, #44]          @ 0x36e44 ; float 0
   36e14: 13a03001     	movne	r3, #1
   36e18: 15803008     	strne	r3, [r0, #0x8]
   36e1c: 1e677a86     	vmulne.f32	s15, s15, s12
   36e20: eaffffd3     	b	0x36d74
   36e24: eef75a00     	vmov.f32	s11, #1.000000e+00
   36e28: ed906a06     	vldr	s12, [r0, #24]
   36e2c: eef46ae5     	vcmpe.f32	s13, s11
   36e30: ee677a86     	vmul.f32	s15, s15, s12
   36e34: eef1fa10     	vmrs	APSR_nzcv, fpscr
   36e38: a3a03000     	movge	r3, #0
   36e3c: a5803008     	strge	r3, [r0, #0x8]
   36e40: eaffffcb     	b	0x36d74
   36e44: 00 00 00 00  	.word	0x00000000
