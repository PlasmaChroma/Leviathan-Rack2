; lubadh::Biquad::Biquad()
; VA 0x4b36c size 244

   4b36c: e92d4010     	push	{r4, lr}
   4b370: e1a04000     	mov	r4, r0
   4b374: e284c014     	add	r12, r4, #20
   4b378: f2c00050     	vmov.i32	q8, #0x0
   4b37c: e2800004     	add	r0, r0, #4
   4b380: e3a035fe     	mov	r3, #1065353216
   4b384: e3a02000     	mov	r2, #0
   4b388: e3a01003     	mov	r1, #3
   4b38c: f4400a8f     	vst1.32	{d16, d17}, [r0]
   4b390: f44c0a8f     	vst1.32	{d16, d17}, [r12]
   4b394: e5842024     	str	r2, [r4, #0x24]
   4b398: e5843000     	str	r3, [r4]
   4b39c: ebffd2e5     	bl	0x3ff38
   4b3a0: e5943010     	ldr	r3, [r4, #0x10]
   4b3a4: e5942014     	ldr	r2, [r4, #0x14]
   4b3a8: e0421003     	sub	r1, r2, r3
   4b3ac: e351000b     	cmp	r1, #11
   4b3b0: e1a01141     	asr	r1, r1, #2
   4b3b4: 9a000018     	bls	0x4b41c
   4b3b8: e3510003     	cmp	r1, #3
   4b3bc: 1a00000e     	bne	0x4b3fc
   4b3c0: e594301c     	ldr	r3, [r4, #0x1c]
   4b3c4: e5942020     	ldr	r2, [r4, #0x20]
   4b3c8: e0421003     	sub	r1, r2, r3
   4b3cc: e351000b     	cmp	r1, #11
   4b3d0: e1a01141     	asr	r1, r1, #2
   4b3d4: 9a00000c     	bls	0x4b40c
   4b3d8: e3510003     	cmp	r1, #3
   4b3dc: 0a000002     	beq	0x4b3ec
   4b3e0: e283300c     	add	r3, r3, #12
   4b3e4: e1520003     	cmp	r2, r3
   4b3e8: 15843020     	strne	r3, [r4, #0x20]
   4b3ec: e3a03003     	mov	r3, #3
   4b3f0: e1a00004     	mov	r0, r4
   4b3f4: e5843028     	str	r3, [r4, #0x28]
   4b3f8: e8bd8010     	pop	{r4, pc}
   4b3fc: e283300c     	add	r3, r3, #12
   4b400: e1520003     	cmp	r2, r3
   4b404: 15843014     	strne	r3, [r4, #0x14]
   4b408: eaffffec     	b	0x4b3c0
   4b40c: e2611003     	rsb	r1, r1, #3
   4b410: e284001c     	add	r0, r4, #28
   4b414: ebffd2c7     	bl	0x3ff38
   4b418: eafffff3     	b	0x4b3ec
   4b41c: e2611003     	rsb	r1, r1, #3
   4b420: e2840010     	add	r0, r4, #16
   4b424: ebffd2c3     	bl	0x3ff38
   4b428: eaffffe4     	b	0x4b3c0
   4b42c: e594001c     	ldr	r0, [r4, #0x1c]
   4b430: e3500000     	cmp	r0, #0
   4b434: 0a000000     	beq	0x4b43c
   4b438: ebff2a80     	bl	0x15e40    @ imm = #-0x35600 ; _ZdlPv
   4b43c: e5940010     	ldr	r0, [r4, #0x10]
   4b440: e3500000     	cmp	r0, #0
   4b444: 0a000000     	beq	0x4b44c
   4b448: ebff2a7c     	bl	0x15e40    @ imm = #-0x35610 ; _ZdlPv
   4b44c: e5940004     	ldr	r0, [r4, #0x4]
   4b450: e3500000     	cmp	r0, #0
   4b454: 0a000000     	beq	0x4b45c
   4b458: ebff2a78     	bl	0x15e40    @ imm = #-0x35620 ; _ZdlPv
   4b45c: ebff2abf     	bl	0x15f60    @ imm = #-0x35504 ; __cxa_end_cleanup
