; lubadh::Channel::InputBuffer::InputBuffer()
; VA 0x389f4 size 132

   389f4: e1a03000     	mov	r3, r0
   389f8: f2c00010     	vmov.i32	d16, #0x0
   389fc: e92d4010     	push	{r4, lr}
   38a00: e1a04000     	mov	r4, r0
   38a04: e3a01001     	mov	r1, #1
   38a08: e4831004     	str	r1, [r3], #4
   38a0c: e3a02000     	mov	r2, #0
   38a10: e3a00014     	mov	r0, #20
   38a14: f443078f     	vst1.32	{d16}, [r3]
   38a18: e584200c     	str	r2, [r4, #0xc]
   38a1c: ebff73ba     	bl	0x1590c     @ imm = #-0x23118 ; _Znwj
   38a20: f2c00050     	vmov.i32	q8, #0x0
   38a24: e280100c     	add	r1, r0, #12
   38a28: e5942004     	ldr	r2, [r4, #0x4]
   38a2c: e2803014     	add	r3, r0, #20
   38a30: e5840004     	str	r0, [r4, #0x4]
   38a34: e3520000     	cmp	r2, #0
   38a38: e5843008     	str	r3, [r4, #0x8]
   38a3c: f4400a0f     	vst1.8	{d16, d17}, [r0]
   38a40: e584300c     	str	r3, [r4, #0xc]
   38a44: f441070f     	vst1.8	{d16}, [r1]
   38a48: 0a000001     	beq	0x38a54
   38a4c: e1a00002     	mov	r0, r2
   38a50: ebff74fa     	bl	0x15e40    @ imm = #-0x22c18 ; _ZdlPv
   38a54: e3a03000     	mov	r3, #0
   38a58: e1a00004     	mov	r0, r4
   38a5c: e5843010     	str	r3, [r4, #0x10]
   38a60: e8bd8010     	pop	{r4, pc}
   38a64: e5940004     	ldr	r0, [r4, #0x4]
   38a68: e3500000     	cmp	r0, #0
   38a6c: 0a000000     	beq	0x38a74
   38a70: ebff74f2     	bl	0x15e40    @ imm = #-0x22c38 ; _ZdlPv
   38a74: ebff7539     	bl	0x15f60    @ imm = #-0x22b1c ; __cxa_end_cleanup
