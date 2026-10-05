; lubadh::AudioEngine::~AudioEngine()
; VA 0x36240 size 612

   36240: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   36244: e1a05000     	mov	r5, r0
   36248: e59f3250     	ldr	r3, [pc, #0x250]        @ 0x364a0
   3624c: e24dd020     	sub	sp, sp, #32
   36250: e3a02000     	mov	r2, #0
   36254: e28d1004     	add	r1, sp, #4
   36258: e5853000     	str	r3, [r5]
   3625c: e28d0008     	add	r0, sp, #8
   36260: e3a03013     	mov	r3, #19
   36264: e28d4010     	add	r4, sp, #16
   36268: e98d0018     	stmib	sp, {r3, r4}
   3626c: ebff8031     	bl	0x16338    @ imm = #-0x1ff3c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36270: e302c164     	movw	r12, #0x2164
   36274: e340c007     	movt	r12, #0x7
   36278: e58d0008     	str	r0, [sp, #0x8]
   3627c: e1a0e000     	mov	lr, r0
   36280: e59d7004     	ldr	r7, [sp, #0x4]
   36284: e58d7010     	str	r7, [sp, #0x10]
   36288: e3a06000     	mov	r6, #0
   3628c: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   36290: e58e300c     	str	r3, [lr, #0xc]
   36294: e58e0000     	str	r0, [lr]
   36298: e3090fec     	movw	r0, #0x9fec
   3629c: e3400009     	movt	r0, #0x9
   362a0: e58e1004     	str	r1, [lr, #0x4]
   362a4: e58e2008     	str	r2, [lr, #0x8]
   362a8: e28d1008     	add	r1, sp, #8
   362ac: e1a02006     	mov	r2, r6
   362b0: e1dc70b0     	ldrh	r7, [r12]
   362b4: e5dc3002     	ldrb	r3, [r12, #0x2]
   362b8: e1ce71b0     	strh	r7, [lr, #16]
   362bc: e5ce3012     	strb	r3, [lr, #0x12]
   362c0: e99d1008     	ldmib	sp, {r3, r12}
   362c4: e58d300c     	str	r3, [sp, #0xc]
   362c8: e7cc6003     	strb	r6, [r12, r3]
   362cc: eb00e713     	bl	0x6ff20
   362d0: e59d0008     	ldr	r0, [sp, #0x8]
   362d4: e1500004     	cmp	r0, r4
   362d8: 0a000000     	beq	0x362e0
   362dc: ebff7ed7     	bl	0x15e40    @ imm = #-0x204a4 ; _ZdlPv
   362e0: e2856004     	add	r6, r5, #4
   362e4: e3a07000     	mov	r7, #0
   362e8: e1a00006     	mov	r0, r6
   362ec: eb00cfee     	bl	0x6a2ac
   362f0: e3a02000     	mov	r2, #0
   362f4: e28d1004     	add	r1, sp, #4
   362f8: e28d0008     	add	r0, sp, #8
   362fc: e3a03012     	mov	r3, #18
   36300: e58d4008     	str	r4, [sp, #0x8]
   36304: e58d3004     	str	r3, [sp, #0x4]
   36308: ebff800a     	bl	0x16338    @ imm = #-0x1ffd8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   3630c: e302c178     	movw	r12, #0x2178
   36310: e340c007     	movt	r12, #0x7
   36314: e58d0008     	str	r0, [sp, #0x8]
   36318: e1a0e000     	mov	lr, r0
   3631c: e59d8004     	ldr	r8, [sp, #0x4]
   36320: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   36324: e58d8010     	str	r8, [sp, #0x10]
   36328: e58e300c     	str	r3, [lr, #0xc]
   3632c: e58e0000     	str	r0, [lr]
   36330: e3090fec     	movw	r0, #0x9fec
   36334: e3400009     	movt	r0, #0x9
   36338: e58e1004     	str	r1, [lr, #0x4]
   3633c: e58e2008     	str	r2, [lr, #0x8]
   36340: e28d1008     	add	r1, sp, #8
   36344: e1dc30b0     	ldrh	r3, [r12]
   36348: e1a02007     	mov	r2, r7
   3634c: e1ce31b0     	strh	r3, [lr, #16]
   36350: e99d1008     	ldmib	sp, {r3, r12}
   36354: e58d300c     	str	r3, [sp, #0xc]
   36358: e7cc7003     	strb	r7, [r12, r3]
   3635c: eb00e6ef     	bl	0x6ff20
   36360: e59d0008     	ldr	r0, [sp, #0x8]
   36364: e1500004     	cmp	r0, r4
   36368: 0a000000     	beq	0x36370
   3636c: ebff7eb3     	bl	0x15e40    @ imm = #-0x20534 ; _ZdlPv
   36370: e3a02000     	mov	r2, #0
   36374: e28d1004     	add	r1, sp, #4
   36378: e28d0008     	add	r0, sp, #8
   3637c: e3a03013     	mov	r3, #19
   36380: e58d4008     	str	r4, [sp, #0x8]
   36384: e3a07000     	mov	r7, #0
   36388: e58d3004     	str	r3, [sp, #0x4]
   3638c: ebff7fe9     	bl	0x16338    @ imm = #-0x2005c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36390: e302c18c     	movw	r12, #0x218c
   36394: e340c007     	movt	r12, #0x7
   36398: e58d0008     	str	r0, [sp, #0x8]
   3639c: e1a0e000     	mov	lr, r0
   363a0: e59d8004     	ldr	r8, [sp, #0x4]
   363a4: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   363a8: e58d8010     	str	r8, [sp, #0x10]
   363ac: e58e300c     	str	r3, [lr, #0xc]
   363b0: e58e0000     	str	r0, [lr]
   363b4: e3090fec     	movw	r0, #0x9fec
   363b8: e3400009     	movt	r0, #0x9
   363bc: e58e1004     	str	r1, [lr, #0x4]
   363c0: e58e2008     	str	r2, [lr, #0x8]
   363c4: e28d1008     	add	r1, sp, #8
   363c8: e1dc80b0     	ldrh	r8, [r12]
   363cc: e1a02007     	mov	r2, r7
   363d0: e5dc3002     	ldrb	r3, [r12, #0x2]
   363d4: e1ce81b0     	strh	r8, [lr, #16]
   363d8: e5ce3012     	strb	r3, [lr, #0x12]
   363dc: e99d1008     	ldmib	sp, {r3, r12}
   363e0: e58d300c     	str	r3, [sp, #0xc]
   363e4: e7cc7003     	strb	r7, [r12, r3]
   363e8: eb00e6cc     	bl	0x6ff20
   363ec: e59d0008     	ldr	r0, [sp, #0x8]
   363f0: e1500004     	cmp	r0, r4
   363f4: 0a000000     	beq	0x363fc
   363f8: ebff7e90     	bl	0x15e40    @ imm = #-0x205c0 ; _ZdlPv
   363fc: e30201a0     	movw	r0, #0x21a0
   36400: e3400007     	movt	r0, #0x7
   36404: ebff800d     	bl	0x16440    @ imm = #-0x1ffcc ; system
   36408: e28d1004     	add	r1, sp, #4
   3640c: e3a02000     	mov	r2, #0
   36410: e28d0008     	add	r0, sp, #8
   36414: e3a03012     	mov	r3, #18
   36418: e58d4008     	str	r4, [sp, #0x8]
   3641c: e58d3004     	str	r3, [sp, #0x4]
   36420: ebff7fc4     	bl	0x16338    @ imm = #-0x200f0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36424: e302c1b4     	movw	r12, #0x21b4
   36428: e340c007     	movt	r12, #0x7
   3642c: e58d0008     	str	r0, [sp, #0x8]
   36430: e1a0e000     	mov	lr, r0
   36434: e59d8004     	ldr	r8, [sp, #0x4]
   36438: e3a07000     	mov	r7, #0
   3643c: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   36440: e58d8010     	str	r8, [sp, #0x10]
   36444: e58e300c     	str	r3, [lr, #0xc]
   36448: e58e0000     	str	r0, [lr]
   3644c: e3090fec     	movw	r0, #0x9fec
   36450: e3400009     	movt	r0, #0x9
   36454: e58e1004     	str	r1, [lr, #0x4]
   36458: e58e2008     	str	r2, [lr, #0x8]
   3645c: e28d1008     	add	r1, sp, #8
   36460: e1dc30b0     	ldrh	r3, [r12]
   36464: e1a02007     	mov	r2, r7
   36468: e1ce31b0     	strh	r3, [lr, #16]
   3646c: e99d1008     	ldmib	sp, {r3, r12}
   36470: e58d300c     	str	r3, [sp, #0xc]
   36474: e7cc7003     	strb	r7, [r12, r3]
   36478: eb00e6a8     	bl	0x6ff20
   3647c: e59d0008     	ldr	r0, [sp, #0x8]
   36480: e1500004     	cmp	r0, r4
   36484: 0a000000     	beq	0x3648c
   36488: ebff7e6c     	bl	0x15e40    @ imm = #-0x20650 ; _ZdlPv
   3648c: e1a00006     	mov	r0, r6
   36490: eb00cf90     	bl	0x6a2d8
   36494: e1a00005     	mov	r0, r5
   36498: e28dd020     	add	sp, sp, #32
   3649c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   364a0: 40 33 07 00  	.word	0x00073340
