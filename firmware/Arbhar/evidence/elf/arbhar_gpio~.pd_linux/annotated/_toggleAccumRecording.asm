0000bb70 <_toggleAccumRecording>:
    bb70: e59f314c     	ldr	r3, [pc, #0x14c]        @ 0xbcc4 <_toggleAccumRecording+0x154>  // u32=0x1b76c; f32?=1.57634867e-40
    bb74: e92d4030     	push	{r4, r5, lr}
    bb78: e08f5003     	add	r5, pc, r3
    bb7c: e1a04000     	mov	r4, r0
    bb80: e59f3140     	ldr	r3, [pc, #0x140]        @ 0xbcc8 <_toggleAccumRecording+0x158>  // u32=0x1b744; f32?=1.57578815e-40
    bb84: e5d52018     	ldrb	r2, [r5, #0x18]
    bb88: e24dd014     	sub	sp, sp, #20
    bb8c: e35200ff     	cmp	r2, #255
    bb90: 05d02029     	ldrbeq	r2, [r0, #0x29]
    bb94: e59f0130     	ldr	r0, [pc, #0x130]        @ 0xbccc <_toggleAccumRecording+0x15c>  // u32=0x1b74c; f32?=1.57590025e-40
    bb98: e08fc000     	add	r12, pc, r0
    bb9c: 05c52018     	strbeq	r2, [r5, #0x18]
    bba0: e08f5003     	add	r5, pc, r3
    bba4: e1a00004     	mov	r0, r4
    bba8: e5dce019     	ldrb	lr, [r12, #0x19]
    bbac: e5d5201a     	ldrb	r2, [r5, #0x1a]
    bbb0: e35e00ff     	cmp	lr, #255
    bbb4: 05d4e028     	ldrbeq	lr, [r4, #0x28]
    bbb8: 05cce019     	strbeq	lr, [r12, #0x19]
    bbbc: e35200ff     	cmp	r2, #255
    bbc0: 05d42025     	ldrbeq	r2, [r4, #0x25]
    bbc4: 05c5201a     	strbeq	r2, [r5, #0x1a]
    bbc8: e3510000     	cmp	r1, #0
    bbcc: a6ef2071     	uxtbge	r2, r1
    bbd0: e3a01022     	mov	r1, #34
    bbd4: b5d42027     	ldrblt	r2, [r4, #0x27]
    bbd8: b2622001     	rsblt	r2, r2, #1
    bbdc: b6ef2072     	uxtblt	r2, r2
    bbe0: e5c42027     	strb	r2, [r4, #0x27]
    bbe4: ebffdfbf     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x8104  // CALL writeToSharedMem
    bbe8: e59f20e0     	ldr	r2, [pc, #0xe0]         @ 0xbcd0 <_toggleAccumRecording+0x160>  // u32=0x9a18; f32?=5.52784218e-41
    bbec: e5d41027     	ldrb	r1, [r4, #0x27]
    bbf0: e08f0002     	add	r0, pc, r2
    bbf4: ebffdfdf     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8084  // CALL post
    bbf8: e1a00004     	mov	r0, r4
    bbfc: ebffdecf     	bl	0x3740 <.plt+0x44>      @ imm = #-0x84c4  // CALL _shareRecordingStateToShMem
    bc00: e5d41029     	ldrb	r1, [r4, #0x29]
    bc04: e5d40027     	ldrb	r0, [r4, #0x27]
    bc08: e3510000     	cmp	r1, #0
    bc0c: 0a00001d     	beq	0xbc88 <_toggleAccumRecording+0x118> @ imm = #0x74
    bc10: e3500000     	cmp	r0, #0
    bc14: 1a000016     	bne	0xbc74 <_toggleAccumRecording+0x104> @ imm = #0x58
    bc18: e59fc0b4     	ldr	r12, [pc, #0xb4]        @ 0xbcd4 <_toggleAccumRecording+0x164>  // u32=0x1b6c8; f32?=1.57405054e-40
    bc1c: e08fe00c     	add	lr, pc, r12
    bc20: e5de5019     	ldrb	r5, [lr, #0x19]
    bc24: e5de301a     	ldrb	r3, [lr, #0x1a]
    bc28: e5c45028     	strb	r5, [r4, #0x28]
    bc2c: e5c43025     	strb	r3, [r4, #0x25]
    bc30: e59f00a0     	ldr	r0, [pc, #0xa0]         @ 0xbcd8 <_toggleAccumRecording+0x168>  // u32=0x99e4; f32?=5.52055543e-41
    bc34: e3a05002     	mov	r5, #2
    bc38: e58d5000     	str	r5, [sp]
    bc3c: e08f0000     	add	r0, pc, r0
    bc40: ebffdeb8     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8520  // CALL gensym
    bc44: e5d4c027     	ldrb	r12, [r4, #0x27]
    bc48: e5d42030     	ldrb	r2, [r4, #0x30]
    bc4c: e3a03001     	mov	r3, #1
    bc50: e58d3008     	str	r3, [sp, #0x8]
    bc54: ee07ca90     	vmov	s15, r12
    bc58: e3520062     	cmp	r2, #98
    bc5c: eeb80a67     	vcvt.f32.u32	s0, s15
    bc60: ed8d0a03     	vstr	s0, [sp, #12]
    bc64: e58d0004     	str	r0, [sp, #0x4]
    bc68: 9a000009     	bls	0xbc94 <_toggleAccumRecording+0x124> @ imm = #0x24
    bc6c: e28dd014     	add	sp, sp, #20
    bc70: e8bd8030     	pop	{r4, r5, pc}
    bc74: e3a02001     	mov	r2, #1
    bc78: e3a01000     	mov	r1, #0
    bc7c: e5c42028     	strb	r2, [r4, #0x28]
    bc80: e5c41025     	strb	r1, [r4, #0x25]
    bc84: eaffffe9     	b	0xbc30 <_toggleAccumRecording+0xc0> @ imm = #-0x5c
    bc88: e3500000     	cmp	r0, #0
    bc8c: 1affffe7     	bne	0xbc30 <_toggleAccumRecording+0xc0> @ imm = #-0x64
    bc90: eaffffe0     	b	0xbc18 <_toggleAccumRecording+0xa8> @ imm = #-0x80
    bc94: e59fe040     	ldr	lr, [pc, #0x40]         @ 0xbcdc <_toggleAccumRecording+0x16c>  // u32=0x8e18; f32?=5.09736329e-41
    bc98: e2844a01     	add	r4, r4, #4096
    bc9c: e08f000e     	add	r0, pc, lr
    bca0: e5944db4     	ldr	r4, [r4, #0xdb4]
    bca4: ebffde9f     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x8584  // CALL gensym
    bca8: e1a02005     	mov	r2, r5
    bcac: e1a0300d     	mov	r3, sp
    bcb0: e1a01000     	mov	r1, r0
    bcb4: e1a00004     	mov	r0, r4
    bcb8: ebffdfed     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x804c  // CALL outlet_list
    bcbc: e28dd014     	add	sp, sp, #20
    bcc0: e8bd8030     	pop	{r4, r5, pc}
    bcc4: 6c b7 01 00  	.word	0x0001b76c
    bcc8: 44 b7 01 00  	.word	0x0001b744
    bccc: 4c b7 01 00  	.word	0x0001b74c
    bcd0: 18 9a 00 00  	.word	0x00009a18
    bcd4: c8 b6 01 00  	.word	0x0001b6c8
    bcd8: e4 99 00 00  	.word	0x000099e4
    bcdc: 18 8e 00 00  	.word	0x00008e18

