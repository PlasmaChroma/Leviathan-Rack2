0000b3ac <_setRecordingState>:
    b3ac: e3510000     	cmp	r1, #0
    b3b0: e5c01024     	strb	r1, [r0, #0x24]
    b3b4: 0a000005     	beq	0xb3d0 <_setRecordingState+0x24> @ imm = #0x14
    b3b8: ed907a0b     	vldr	s14, [r0, #44]
    b3bc: eddf7a04     	vldr	s15, [pc, #16]          @ 0xb3d4 <_setRecordingState+0x28>
    b3c0: eeb47ae7     	vcmpe.f32	s14, s15
    b3c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b3c8: c3a03000     	movgt	r3, #0
    b3cc: c580302c     	strgt	r3, [r0, #0x2c]
    b3d0: eaffe0da     	b	0x3740 <.plt+0x44>      @ imm = #-0x7c98
    b3d4: 00 60 ea 48  	.word	0x48ea6000

