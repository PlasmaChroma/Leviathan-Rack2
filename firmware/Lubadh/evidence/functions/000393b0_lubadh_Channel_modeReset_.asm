; lubadh::Channel::modeReset()
; VA 0x393b0 size 224

   393b0: e92d4010     	push	{r4, lr}
   393b4: e1a04000     	mov	r4, r0
   393b8: e2841004     	add	r1, r4, #4
   393bc: e24dd018     	sub	sp, sp, #24
   393c0: e3022470     	movw	r2, #0x2470
   393c4: e3402007     	movt	r2, #0x7
   393c8: e1a0000d     	mov	r0, sp
   393cc: ebffd5fd     	bl	0x2ebc8
   393d0: e3090fec     	movw	r0, #0x9fec
   393d4: e3400009     	movt	r0, #0x9
   393d8: e1a0100d     	mov	r1, sp
   393dc: e3a02000     	mov	r2, #0
   393e0: eb00dace     	bl	0x6ff20
   393e4: e59d0000     	ldr	r0, [sp]
   393e8: e28d3008     	add	r3, sp, #8
   393ec: e1500003     	cmp	r0, r3
   393f0: 0a000000     	beq	0x393f8
   393f4: ebff7291     	bl	0x15e40    @ imm = #-0x235bc ; _ZdlPv
   393f8: e59400e8     	ldr	r0, [r4, #0xe8]
   393fc: e284ea29     	add	lr, r4, #167936
   39400: e5d4327c     	ldrb	r3, [r4, #0x27c]
   39404: e3a01001     	mov	r1, #1
   39408: e284ca2a     	add	r12, r4, #172032
   3940c: e5c41285     	strb	r1, [r4, #0x285]
   39410: e5c41288     	strb	r1, [r4, #0x288]
   39414: e0233001     	eor	r3, r3, r1
   39418: e3a02002     	mov	r2, #2
   3941c: e5802084     	str	r2, [r0, #0x84]
   39420: e5c4327d     	strb	r3, [r4, #0x27d]
   39424: e3a02000     	mov	r2, #0
   39428: e59e3f5c     	ldr	r3, [lr, #0xf5c]
   3942c: e5c4127c     	strb	r1, [r4, #0x27c]
   39430: e580208c     	str	r2, [r0, #0x8c]
   39434: e5802098     	str	r2, [r0, #0x98]
   39438: e58c14d0     	str	r1, [r12, #0x4d0]
   3943c: e594c0b0     	ldr	r12, [r4, #0xb0]
   39440: e5c422ac     	strb	r2, [r4, #0x2ac]
   39444: e594e0a8     	ldr	lr, [r4, #0xa8]
   39448: e5c0206c     	strb	r2, [r0, #0x6c]
   3944c: e5802080     	str	r2, [r0, #0x80]
   39450: e583c00c     	str	r12, [r3, #0xc]
   39454: e5d40090     	ldrb	r0, [r4, #0x90]
   39458: e594c0bc     	ldr	r12, [r4, #0xbc]
   3945c: e583c010     	str	r12, [r3, #0x10]
   39460: e583e008     	str	lr, [r3, #0x8]
   39464: e5831014     	str	r1, [r3, #0x14]
   39468: e5c30038     	strb	r0, [r3, #0x38]
   3946c: e583203c     	str	r2, [r3, #0x3c]
   39470: e28dd018     	add	sp, sp, #24
   39474: e8bd8010     	pop	{r4, pc}
   39478: e59d0000     	ldr	r0, [sp]
   3947c: e28d3008     	add	r3, sp, #8
   39480: e1500003     	cmp	r0, r3
   39484: 0a000000     	beq	0x3948c
   39488: ebff726c     	bl	0x15e40    @ imm = #-0x23650 ; _ZdlPv
   3948c: ebff72b3     	bl	0x15f60    @ imm = #-0x23534 ; __cxa_end_cleanup
