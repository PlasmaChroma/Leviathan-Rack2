0000810c <_setFollowModeSwitch>:
    810c: e92d4070     	push	{r4, r5, r6, lr}
    8110: e59f4148     	ldr	r4, [pc, #0x148]        @ 0x8260 <_setFollowModeSwitch+0x154>  // u32=0x1f298; f32?=1.78861736e-40
    8114: e5d0303c     	ldrb	r3, [r0, #0x3c]
    8118: e08f4004     	add	r4, pc, r4
    811c: e3530003     	cmp	r3, #3
    8120: e5d43044     	ldrb	r3, [r4, #0x44]
    8124: 13a02000     	movne	r2, #0
    8128: 15c42045     	strbne	r2, [r4, #0x45]
    812c: 0a000001     	beq	0x8138 <_setFollowModeSwitch+0x2c> @ imm = #0x4
    8130: e1a00003     	mov	r0, r3
    8134: e8bd8070     	pop	{r4, r5, r6, pc}
    8138: eddf7a46     	vldr	s15, [pc, #280]         @ 0x8258 <_setFollowModeSwitch+0x14c>  // f32=3800
    813c: ed9f7a46     	vldr	s14, [pc, #280]         @ 0x825c <_setFollowModeSwitch+0x150>  // f32=300
    8140: eeb40ae7     	vcmpe.f32	s0, s15
    8144: eef1fa10     	vmrs	APSR_nzcv, fpscr
    8148: eeb40ac7     	vcmpe.f32	s0, s14
    814c: 43a02001     	movmi	r2, #1
    8150: 53a02000     	movpl	r2, #0
    8154: eef1fa10     	vmrs	APSR_nzcv, fpscr
    8158: c2022001     	andgt	r2, r2, #1
    815c: d3a02000     	movle	r2, #0
    8160: e3520000     	cmp	r2, #0
    8164: 13a02001     	movne	r2, #1
    8168: 15c42045     	strbne	r2, [r4, #0x45]
    816c: 1affffef     	bne	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x44
    8170: eeb40ae7     	vcmpe.f32	s0, s15
    8174: e1a05000     	mov	r5, r0
    8178: eef1fa10     	vmrs	APSR_nzcv, fpscr
    817c: ba00000e     	blt	0x81bc <_setFollowModeSwitch+0xb0> @ imm = #0x38
    8180: e5d41045     	ldrb	r1, [r4, #0x45]
    8184: e3510001     	cmp	r1, #1
    8188: 1affffe8     	bne	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x60
    818c: e263e001     	rsb	lr, r3, #1
    8190: e5c42045     	strb	r2, [r4, #0x45]
    8194: e6ef107e     	uxtb	r1, lr
    8198: e5c41044     	strb	r1, [r4, #0x44]
    819c: e3510000     	cmp	r1, #0
    81a0: 1a000021     	bne	0x822c <_setFollowModeSwitch+0x120> @ imm = #0x84
    81a4: e1a00005     	mov	r0, r5
    81a8: ebffee0f     	bl	0x39ec <.plt+0x2f0>     @ imm = #-0x47c4  // CALL _setFollowMode
    81ac: e59fc0b0     	ldr	r12, [pc, #0xb0]        @ 0x8264 <_setFollowModeSwitch+0x158>  // u32=0x1f200; f32?=1.78648739e-40
    81b0: e08f000c     	add	r0, pc, r12
    81b4: e5d03044     	ldrb	r3, [r0, #0x44]
    81b8: eaffffdc     	b	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x90
    81bc: eeb40ac7     	vcmpe.f32	s0, s14
    81c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    81c4: 8affffd9     	bhi	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x9c
    81c8: e5d41045     	ldrb	r1, [r4, #0x45]
    81cc: e3510001     	cmp	r1, #1
    81d0: 1affffd6     	bne	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0xa8
    81d4: e280ca01     	add	r12, r0, #4096
    81d8: e2633001     	rsb	r3, r3, #1
    81dc: e5c42045     	strb	r2, [r4, #0x45]
    81e0: e5dcedf4     	ldrb	lr, [r12, #0xdf4]
    81e4: e6ef1073     	uxtb	r1, r3
    81e8: e5c41044     	strb	r1, [r4, #0x44]
    81ec: e35e0000     	cmp	lr, #0
    81f0: 0a000011     	beq	0x823c <_setFollowModeSwitch+0x130> @ imm = #0x44
    81f4: e5dc1df5     	ldrb	r1, [r12, #0xdf5]
    81f8: e5d0c054     	ldrb	r12, [r0, #0x54]
    81fc: e2613001     	rsb	r3, r1, #1
    8200: e35c0000     	cmp	r12, #0
    8204: e6ef1073     	uxtb	r1, r3
    8208: e5c41044     	strb	r1, [r4, #0x44]
    820c: 1a00000d     	bne	0x8248 <_setFollowModeSwitch+0x13c> @ imm = #0x34
    8210: e3a02000     	mov	r2, #0
    8214: e1a00005     	mov	r0, r5
    8218: ebffed60     	bl	0x37a0 <.plt+0xa4>      @ imm = #-0x4a80  // CALL changeInternalClockState
    821c: e59f0044     	ldr	r0, [pc, #0x44]         @ 0x8268 <_setFollowModeSwitch+0x15c>  // u32=0x1f190; f32?=1.78491793e-40
    8220: e08f2000     	add	r2, pc, r0
    8224: e5d23044     	ldrb	r3, [r2, #0x44]
    8228: eaffffc0     	b	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x100
    822c: e1a01002     	mov	r1, r2
    8230: ebffed5a     	bl	0x37a0 <.plt+0xa4>      @ imm = #-0x4a98  // CALL changeInternalClockState
    8234: e5d41044     	ldrb	r1, [r4, #0x44]
    8238: eaffffd9     	b	0x81a4 <_setFollowModeSwitch+0x98> @ imm = #-0x9c
    823c: ebffedea     	bl	0x39ec <.plt+0x2f0>     @ imm = #-0x4858  // CALL _setFollowMode
    8240: e5d43044     	ldrb	r3, [r4, #0x44]
    8244: eaffffb9     	b	0x8130 <_setFollowModeSwitch+0x24> @ imm = #-0x11c
    8248: e1a01002     	mov	r1, r2
    824c: ebffede6     	bl	0x39ec <.plt+0x2f0>     @ imm = #-0x4868  // CALL _setFollowMode
    8250: e5d41044     	ldrb	r1, [r4, #0x44]
    8254: eaffffed     	b	0x8210 <_setFollowModeSwitch+0x104> @ imm = #-0x4c
    8258: 00 80 6d 45  	.word	0x456d8000
    825c: 00 00 96 43  	.word	0x43960000
    8260: 98 f2 01 00  	.word	0x0001f298
    8264: 00 f2 01 00  	.word	0x0001f200
    8268: 90 f1 01 00  	.word	0x0001f190

