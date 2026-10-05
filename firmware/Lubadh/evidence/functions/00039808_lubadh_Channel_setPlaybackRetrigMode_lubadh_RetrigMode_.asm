; lubadh::Channel::setPlaybackRetrigMode(lubadh::RetrigMode)
; VA 0x39808 size 280

   39808: e59030e8     	ldr	r3, [r0, #0xe8]
   3980c: e5933090     	ldr	r3, [r3, #0x90]
   39810: e1530001     	cmp	r3, r1
   39814: 012fff1e     	bxeq	lr
   39818: e92d40f0     	push	{r4, r5, r6, r7, lr}
   3981c: e1a05000     	mov	r5, r0
   39820: e1a04001     	mov	r4, r1
   39824: e24dd034     	sub	sp, sp, #52
   39828: e2851004     	add	r1, r5, #4
   3982c: e1a0000d     	mov	r0, sp
   39830: e30224ec     	movw	r2, #0x24ec
   39834: e3402007     	movt	r2, #0x7
   39838: ebffd4e2     	bl	0x2ebc8
   3983c: e3540000     	cmp	r4, #0
   39840: e30234e0     	movw	r3, #0x24e0
   39844: e3403007     	movt	r3, #0x7
   39848: e30214d8     	movw	r1, #0x24d8
   3984c: e3401007     	movt	r1, #0x7
   39850: e1a0000d     	mov	r0, sp
   39854: 11a01003     	movne	r1, r3
   39858: ebff72fb     	bl	0x1644c    @ imm = #-0x23414 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3985c: e1a0e000     	mov	lr, r0
   39860: e28d6020     	add	r6, sp, #32
   39864: e58d6018     	str	r6, [sp, #0x18]
   39868: e1a0c000     	mov	r12, r0
   3986c: e49e3008     	ldr	r3, [lr], #8
   39870: e153000e     	cmp	r3, lr
   39874: 158d3018     	strne	r3, [sp, #0x18]
   39878: 01a07006     	moveq	r7, r6
   3987c: 059e0000     	ldreq	r0, [lr]
   39880: 059e1004     	ldreq	r1, [lr, #0x4]
   39884: 059e2008     	ldreq	r2, [lr, #0x8]
   39888: 059e300c     	ldreq	r3, [lr, #0xc]
   3988c: 159c2008     	ldrne	r2, [r12, #0x8]
   39890: 08a7000f     	stmeq	r7!, {r0, r1, r2, r3}
   39894: e3090fec     	movw	r0, #0x9fec
   39898: e3400009     	movt	r0, #0x9
   3989c: 158d2020     	strne	r2, [sp, #0x20]
   398a0: e28d1018     	add	r1, sp, #24
   398a4: e3a02000     	mov	r2, #0
   398a8: e5cc2008     	strb	r2, [r12, #0x8]
   398ac: e59c3004     	ldr	r3, [r12, #0x4]
   398b0: e58d301c     	str	r3, [sp, #0x1c]
   398b4: e58ce000     	str	lr, [r12]
   398b8: e58c2004     	str	r2, [r12, #0x4]
   398bc: eb00d997     	bl	0x6ff20
   398c0: e59d0018     	ldr	r0, [sp, #0x18]
   398c4: e1500006     	cmp	r0, r6
   398c8: 0a000000     	beq	0x398d0
   398cc: ebff715b     	bl	0x15e40    @ imm = #-0x23a94 ; _ZdlPv
   398d0: e59d0000     	ldr	r0, [sp]
   398d4: e28d3008     	add	r3, sp, #8
   398d8: e1500003     	cmp	r0, r3
   398dc: 0a000000     	beq	0x398e4
   398e0: ebff7156     	bl	0x15e40    @ imm = #-0x23aa8 ; _ZdlPv
   398e4: e59530e8     	ldr	r3, [r5, #0xe8]
   398e8: e5834090     	str	r4, [r3, #0x90]
   398ec: e28dd034     	add	sp, sp, #52
   398f0: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   398f4: e59d0000     	ldr	r0, [sp]
   398f8: e28d3008     	add	r3, sp, #8
   398fc: e1500003     	cmp	r0, r3
   39900: 0a000000     	beq	0x39908
   39904: ebff714d     	bl	0x15e40    @ imm = #-0x23acc ; _ZdlPv
   39908: ebff7194     	bl	0x15f60    @ imm = #-0x239b0 ; __cxa_end_cleanup
   3990c: e59d0018     	ldr	r0, [sp, #0x18]
   39910: e1500006     	cmp	r0, r6
   39914: 0afffff6     	beq	0x398f4
   39918: ebff7148     	bl	0x15e40    @ imm = #-0x23ae0 ; _ZdlPv
   3991c: eafffff4     	b	0x398f4
