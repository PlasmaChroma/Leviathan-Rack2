; lubadh::AudioEngine::start()
; VA 0x364a4 size 184

   364a4: e92d40f0     	push	{r4, r5, r6, r7, lr}
   364a8: e3a02000     	mov	r2, #0
   364ac: e1a04000     	mov	r4, r0
   364b0: e24dd024     	sub	sp, sp, #36
   364b4: e3a03014     	mov	r3, #20
   364b8: e28d1004     	add	r1, sp, #4
   364bc: e28d0008     	add	r0, sp, #8
   364c0: e28d5010     	add	r5, sp, #16
   364c4: e3a06000     	mov	r6, #0
   364c8: e98d0028     	stmib	sp, {r3, r5}
   364cc: ebff7f99     	bl	0x16338    @ imm = #-0x2019c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   364d0: e302c1c8     	movw	r12, #0x21c8
   364d4: e340c007     	movt	r12, #0x7
   364d8: e1a0e000     	mov	lr, r0
   364dc: e58d0008     	str	r0, [sp, #0x8]
   364e0: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   364e4: e58e300c     	str	r3, [lr, #0xc]
   364e8: e59d3004     	ldr	r3, [sp, #0x4]
   364ec: e58d3010     	str	r3, [sp, #0x10]
   364f0: e58e0000     	str	r0, [lr]
   364f4: e58e1004     	str	r1, [lr, #0x4]
   364f8: e28d1008     	add	r1, sp, #8
   364fc: e99d0088     	ldmib	sp, {r3, r7}
   36500: e58e2008     	str	r2, [lr, #0x8]
   36504: e59c0000     	ldr	r0, [r12]
   36508: e1a02006     	mov	r2, r6
   3650c: e58e0010     	str	r0, [lr, #0x10]
   36510: e3090fec     	movw	r0, #0x9fec
   36514: e3400009     	movt	r0, #0x9
   36518: e58d300c     	str	r3, [sp, #0xc]
   3651c: e7c76003     	strb	r6, [r7, r3]
   36520: eb00e67e     	bl	0x6ff20
   36524: e59d0008     	ldr	r0, [sp, #0x8]
   36528: e1500005     	cmp	r0, r5
   3652c: 0a000000     	beq	0x36534
   36530: ebff7e42     	bl	0x15e40    @ imm = #-0x206f8 ; _ZdlPv
   36534: e1a01004     	mov	r1, r4
   36538: e2840004     	add	r0, r4, #4
   3653c: eb00d121     	bl	0x6a9c8
   36540: e28dd024     	add	sp, sp, #36
   36544: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   36548: e59d0008     	ldr	r0, [sp, #0x8]
   3654c: e1500005     	cmp	r0, r5
   36550: 0a000000     	beq	0x36558
   36554: ebff7e39     	bl	0x15e40    @ imm = #-0x2071c ; _ZdlPv
   36558: ebff7e80     	bl	0x15f60    @ imm = #-0x20600 ; __cxa_end_cleanup
