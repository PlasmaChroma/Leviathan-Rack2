0000b1c8 <_toggleRecordingState>:
    b1c8: e5d03024     	ldrb	r3, [r0, #0x24]
    b1cc: e5d02027     	ldrb	r2, [r0, #0x27]
    b1d0: e2631001     	rsb	r1, r3, #1
    b1d4: e3520000     	cmp	r2, #0
    b1d8: e6efc071     	uxtb	r12, r1
    b1dc: e5c0c024     	strb	r12, [r0, #0x24]
    b1e0: 1a000004     	bne	0xb1f8 <_toggleRecordingState+0x30> @ imm = #0x10
    b1e4: e3a03000     	mov	r3, #0
    b1e8: e35c0000     	cmp	r12, #0
    b1ec: e580302c     	str	r3, [r0, #0x2c]
    b1f0: 15803058     	strne	r3, [r0, #0x58]
    b1f4: eaffe151     	b	0x3740 <.plt+0x44>      @ imm = #-0x7abc  // CALL _shareRecordingStateToShMem
    b1f8: ed907a0b     	vldr	s14, [r0, #44]
    b1fc: eddf7a04     	vldr	s15, [pc, #16]          @ 0xb214 <_toggleRecordingState+0x4c>  // f32=480000
    b200: eeb47ae7     	vcmpe.f32	s14, s15
    b204: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b208: c3a0c000     	movgt	r12, #0
    b20c: c580c02c     	strgt	r12, [r0, #0x2c]
    b210: eaffe14a     	b	0x3740 <.plt+0x44>      @ imm = #-0x7ad8  // CALL _shareRecordingStateToShMem
    b214: 00 60 ea 48  	.word	0x48ea6000

