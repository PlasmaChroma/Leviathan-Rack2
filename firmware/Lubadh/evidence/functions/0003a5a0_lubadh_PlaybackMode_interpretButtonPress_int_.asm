; lubadh::PlaybackMode::interpretButtonPress(int)
; VA 0x3a5a0 size 268

   3a5a0: e92d4010     	push	{r4, lr}
   3a5a4: e3510001     	cmp	r1, #1
   3a5a8: e1a04000     	mov	r4, r0
   3a5ac: e24dd018     	sub	sp, sp, #24
   3a5b0: 0a000005     	beq	0x3a5cc
   3a5b4: e3510006     	cmp	r1, #6
   3a5b8: 0a00001d     	beq	0x3a634
   3a5bc: e3510000     	cmp	r1, #0
   3a5c0: 0a00000d     	beq	0x3a5fc
   3a5c4: e28dd018     	add	sp, sp, #24
   3a5c8: e8bd8010     	pop	{r4, pc}
   3a5cc: e5900008     	ldr	r0, [r0, #0x8]
   3a5d0: e59030e8     	ldr	r3, [r0, #0xe8]
   3a5d4: e5933084     	ldr	r3, [r3, #0x84]
   3a5d8: e3530001     	cmp	r3, #1
   3a5dc: 1a000018     	bne	0x3a644
   3a5e0: e5d0227c     	ldrb	r2, [r0, #0x27c]
   3a5e4: e3520000     	cmp	r2, #0
   3a5e8: 0a000015     	beq	0x3a644
   3a5ec: e5c0327d     	strb	r3, [r0, #0x27d]
   3a5f0: e3a03000     	mov	r3, #0
   3a5f4: e5c0327c     	strb	r3, [r0, #0x27c]
   3a5f8: eafffff1     	b	0x3a5c4
   3a5fc: e5900008     	ldr	r0, [r0, #0x8]
   3a600: e59030e8     	ldr	r3, [r0, #0xe8]
   3a604: e5933084     	ldr	r3, [r3, #0x84]
   3a608: e3530001     	cmp	r3, #1
   3a60c: 1a000004     	bne	0x3a624
   3a610: e5d0227c     	ldrb	r2, [r0, #0x27c]
   3a614: e3520000     	cmp	r2, #0
   3a618: 05c0327d     	strbeq	r3, [r0, #0x27d]
   3a61c: 05c0327c     	strbeq	r3, [r0, #0x27c]
   3a620: 0affffe7     	beq	0x3a5c4
   3a624: e3a01002     	mov	r1, #2
   3a628: ebfffe7a     	bl	0x3a018
   3a62c: e28dd018     	add	sp, sp, #24
   3a630: e8bd8010     	pop	{r4, pc}
   3a634: e5900008     	ldr	r0, [r0, #0x8]
   3a638: e3a01002     	mov	r1, #2
   3a63c: ebfffe75     	bl	0x3a018
   3a640: eafffff9     	b	0x3a62c
   3a644: ebfff7b3     	bl	0x38518
   3a648: e5943008     	ldr	r3, [r4, #0x8]
   3a64c: e3a0c001     	mov	r12, #1
   3a650: e1a0000d     	mov	r0, sp
   3a654: e2831004     	add	r1, r3, #4
   3a658: e3022660     	movw	r2, #0x2660
   3a65c: e3402007     	movt	r2, #0x7
   3a660: e5c3c284     	strb	r12, [r3, #0x284]
   3a664: ebffd157     	bl	0x2ebc8
   3a668: e3090fec     	movw	r0, #0x9fec
   3a66c: e3400009     	movt	r0, #0x9
   3a670: e1a0100d     	mov	r1, sp
   3a674: e3a02000     	mov	r2, #0
   3a678: eb00d628     	bl	0x6ff20
   3a67c: e59d0000     	ldr	r0, [sp]
   3a680: e28d3008     	add	r3, sp, #8
   3a684: e1500003     	cmp	r0, r3
   3a688: 0affffcd     	beq	0x3a5c4
   3a68c: ebff6deb     	bl	0x15e40    @ imm = #-0x24854 ; _ZdlPv
   3a690: eaffffcb     	b	0x3a5c4
   3a694: e59d0000     	ldr	r0, [sp]
   3a698: e28d3008     	add	r3, sp, #8
   3a69c: e1500003     	cmp	r0, r3
   3a6a0: 0a000000     	beq	0x3a6a8
   3a6a4: ebff6de5     	bl	0x15e40    @ imm = #-0x2486c ; _ZdlPv
   3a6a8: ebff6e2c     	bl	0x15f60    @ imm = #-0x24750 ; __cxa_end_cleanup
