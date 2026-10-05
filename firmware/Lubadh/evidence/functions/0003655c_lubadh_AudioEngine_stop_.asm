; lubadh::AudioEngine::stop()
; VA 0x3655c size 180

   3655c: e92d40f0     	push	{r4, r5, r6, r7, lr}
   36560: e3a02000     	mov	r2, #0
   36564: e1a04000     	mov	r4, r0
   36568: e24dd024     	sub	sp, sp, #36
   3656c: e3a03014     	mov	r3, #20
   36570: e28d1004     	add	r1, sp, #4
   36574: e28d0008     	add	r0, sp, #8
   36578: e28d5010     	add	r5, sp, #16
   3657c: e3a06000     	mov	r6, #0
   36580: e98d0028     	stmib	sp, {r3, r5}
   36584: ebff7f6b     	bl	0x16338    @ imm = #-0x20254 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36588: e302c1e0     	movw	r12, #0x21e0
   3658c: e340c007     	movt	r12, #0x7
   36590: e1a0e000     	mov	lr, r0
   36594: e58d0008     	str	r0, [sp, #0x8]
   36598: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3659c: e58e300c     	str	r3, [lr, #0xc]
   365a0: e59d3004     	ldr	r3, [sp, #0x4]
   365a4: e58d3010     	str	r3, [sp, #0x10]
   365a8: e58e0000     	str	r0, [lr]
   365ac: e58e1004     	str	r1, [lr, #0x4]
   365b0: e28d1008     	add	r1, sp, #8
   365b4: e99d0088     	ldmib	sp, {r3, r7}
   365b8: e58e2008     	str	r2, [lr, #0x8]
   365bc: e59c0000     	ldr	r0, [r12]
   365c0: e1a02006     	mov	r2, r6
   365c4: e58e0010     	str	r0, [lr, #0x10]
   365c8: e3090fec     	movw	r0, #0x9fec
   365cc: e3400009     	movt	r0, #0x9
   365d0: e58d300c     	str	r3, [sp, #0xc]
   365d4: e7c76003     	strb	r6, [r7, r3]
   365d8: eb00e650     	bl	0x6ff20
   365dc: e59d0008     	ldr	r0, [sp, #0x8]
   365e0: e1500005     	cmp	r0, r5
   365e4: 0a000000     	beq	0x365ec
   365e8: ebff7e14     	bl	0x15e40    @ imm = #-0x207b0 ; _ZdlPv
   365ec: e2840004     	add	r0, r4, #4
   365f0: eb00cf25     	bl	0x6a28c
   365f4: e28dd024     	add	sp, sp, #36
   365f8: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   365fc: e59d0008     	ldr	r0, [sp, #0x8]
   36600: e1500005     	cmp	r0, r5
   36604: 0a000000     	beq	0x3660c
   36608: ebff7e0c     	bl	0x15e40    @ imm = #-0x207d0 ; _ZdlPv
   3660c: ebff7e53     	bl	0x15f60    @ imm = #-0x206b4 ; __cxa_end_cleanup
