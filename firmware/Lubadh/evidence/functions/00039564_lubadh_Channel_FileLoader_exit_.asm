; lubadh::Channel::FileLoader::exit()
; VA 0x39564 size 244

   39564: e92d4030     	push	{r4, r5, lr}
   39568: e1a04000     	mov	r4, r0
   3956c: e302249c     	movw	r2, #0x249c
   39570: e3402007     	movt	r2, #0x7
   39574: e24dd01c     	sub	sp, sp, #28
   39578: e5941000     	ldr	r1, [r4]
   3957c: e1a0000d     	mov	r0, sp
   39580: e2811004     	add	r1, r1, #4
   39584: ebffd58f     	bl	0x2ebc8
   39588: e3090fec     	movw	r0, #0x9fec
   3958c: e3400009     	movt	r0, #0x9
   39590: e1a0100d     	mov	r1, sp
   39594: e3a02000     	mov	r2, #0
   39598: eb00da60     	bl	0x6ff20
   3959c: e59d0000     	ldr	r0, [sp]
   395a0: e28d3008     	add	r3, sp, #8
   395a4: e1500003     	cmp	r0, r3
   395a8: 0a000000     	beq	0x395b0
   395ac: ebff7223     	bl	0x15e40    @ imm = #-0x23774 ; _ZdlPv
   395b0: e5943000     	ldr	r3, [r4]
   395b4: e3a00001     	mov	r0, #1
   395b8: e594c008     	ldr	r12, [r4, #0x8]
   395bc: e2831a2a     	add	r1, r3, #172032
   395c0: e594e0fc     	ldr	lr, [r4, #0xfc]
   395c4: e593201c     	ldr	r2, [r3, #0x1c]
   395c8: e581e4d0     	str	lr, [r1, #0x4d0]
   395cc: e2822a2a     	add	r2, r2, #172032
   395d0: e59ce000     	ldr	lr, [r12]
   395d4: e5945100     	ldr	r5, [r4, #0x100]
   395d8: e35e0003     	cmp	lr, #3
   395dc: e58254d0     	str	r5, [r2, #0x4d0]
   395e0: 13a02000     	movne	r2, #0
   395e4: e5c40104     	strb	r0, [r4, #0x104]
   395e8: 158c2000     	strne	r2, [r12]
   395ec: 0a00000c     	beq	0x39624
   395f0: e5d11496     	ldrb	r1, [r1, #0x496]
   395f4: e3a02000     	mov	r2, #0
   395f8: e5930020     	ldr	r0, [r3, #0x20]
   395fc: eb00cce4     	bl	0x6c994
   39600: e5942000     	ldr	r2, [r4]
   39604: e3a03001     	mov	r3, #1
   39608: e592101c     	ldr	r1, [r2, #0x1c]
   3960c: e5c23285     	strb	r3, [r2, #0x285]
   39610: e5c13285     	strb	r3, [r1, #0x285]
   39614: e5c23288     	strb	r3, [r2, #0x288]
   39618: e5c13288     	strb	r3, [r1, #0x288]
   3961c: e28dd01c     	add	sp, sp, #28
   39620: e8bd8030     	pop	{r4, r5, pc}
   39624: e3a03000     	mov	r3, #0
   39628: e28400bc     	add	r0, r4, #188
   3962c: e58c3000     	str	r3, [r12]
   39630: eb00dc7a     	bl	0x70820
   39634: e5943000     	ldr	r3, [r4]
   39638: e2831a2a     	add	r1, r3, #172032
   3963c: eaffffeb     	b	0x395f0
   39640: e59d0000     	ldr	r0, [sp]
   39644: e28d3008     	add	r3, sp, #8
   39648: e1500003     	cmp	r0, r3
   3964c: 0a000000     	beq	0x39654
   39650: ebff71fa     	bl	0x15e40    @ imm = #-0x23818 ; _ZdlPv
   39654: ebff7241     	bl	0x15f60    @ imm = #-0x236fc ; __cxa_end_cleanup
