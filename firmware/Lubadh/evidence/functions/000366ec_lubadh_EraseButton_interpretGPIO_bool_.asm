; lubadh::EraseButton::interpretGPIO(bool)
; VA 0x366ec size 780

   366ec: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   366f0: e1a04000     	mov	r4, r0
   366f4: e5906008     	ldr	r6, [r0, #0x8]
   366f8: e24dd040     	sub	sp, sp, #64
   366fc: e2515000     	subs	r5, r1, #0
   36700: 0a00000b     	beq	0x36734
   36704: e3560000     	cmp	r6, #0
   36708: 15902004     	ldrne	r2, [r0, #0x4]
   3670c: 0a000013     	beq	0x36760
   36710: e3560001     	cmp	r6, #1
   36714: e3a03000     	mov	r3, #0
   36718: e5c4300d     	strb	r3, [r4, #0xd]
   3671c: 016f3f12     	clzeq	r3, r2
   36720: 01a032a3     	lsreq	r3, r3, #5
   36724: e5c4300e     	strb	r3, [r4, #0xe]
   36728: e5842008     	str	r2, [r4, #0x8]
   3672c: e28dd040     	add	sp, sp, #64
   36730: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   36734: e3560000     	cmp	r6, #0
   36738: 1a000057     	bne	0x3689c
   3673c: e5902004     	ldr	r2, [r0, #0x4]
   36740: e3520001     	cmp	r2, #1
   36744: 0a000060     	beq	0x368cc
   36748: e3a03000     	mov	r3, #0
   3674c: e5842008     	str	r2, [r4, #0x8]
   36750: e5c4300d     	strb	r3, [r4, #0xd]
   36754: e5c4300e     	strb	r3, [r4, #0xe]
   36758: e28dd040     	add	sp, sp, #64
   3675c: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   36760: e5d02068     	ldrb	r2, [r0, #0x68]
   36764: e3a05001     	mov	r5, #1
   36768: e58d0008     	str	r0, [sp, #0x8]
   3676c: e3063694     	movw	r3, #0x6694
   36770: e3403003     	movt	r3, #0x3
   36774: e28d9008     	add	r9, sp, #8
   36778: e28f1e27     	add	r1, pc, #624
   3677c: e1c100d0     	ldrd	r0, r1, [r1]
   36780: e58d3014     	str	r3, [sp, #0x14]
   36784: e3520001     	cmp	r2, #1
   36788: e30636a4     	movw	r3, #0x66a4
   3678c: e3403003     	movt	r3, #0x3
   36790: e284a010     	add	r10, r4, #16
   36794: e58d3010     	str	r3, [sp, #0x10]
   36798: e1c404f8     	strd	r0, r1, [r4, #72]
   3679c: e5845004     	str	r5, [r4, #0x4]
   367a0: 0a00006d     	beq	0x3695c
   367a4: e28d7028     	add	r7, sp, #40
   367a8: e1a01009     	mov	r1, r9
   367ac: e1a00007     	mov	r0, r7
   367b0: e3a02002     	mov	r2, #2
   367b4: e58d6030     	str	r6, [sp, #0x30]
   367b8: ebffffb9     	bl	0x366a4
   367bc: eddd0b04     	vldr	d16, [sp, #16]
   367c0: e5d43068     	ldrb	r3, [r4, #0x68]
   367c4: e59de014     	ldr	lr, [sp, #0x14]
   367c8: e59d8010     	ldr	r8, [sp, #0x10]
   367cc: e3530001     	cmp	r3, #1
   367d0: e5cd5038     	strb	r5, [sp, #0x38]
   367d4: edcd0b0c     	vstr	d16, [sp, #48]
   367d8: 0a000041     	beq	0x368e4
   367dc: e35300ff     	cmp	r3, #255
   367e0: e2848058     	add	r8, r4, #88
   367e4: e28d5018     	add	r5, sp, #24
   367e8: e1a01008     	mov	r1, r8
   367ec: 13022280     	movwne	r2, #0x2280
   367f0: 13402007     	movtne	r2, #0x7
   367f4: e1a00005     	mov	r0, r5
   367f8: 17926103     	ldrne	r6, [r2, r3, lsl #2]
   367fc: e12fff36     	blx	r6
   36800: e3a03001     	mov	r3, #1
   36804: e8970003     	ldm	r7, {r0, r1}
   36808: e5c43068     	strb	r3, [r4, #0x68]
   3680c: e8850003     	stm	r5, {r0, r1}
   36810: e3a02000     	mov	r2, #0
   36814: e8980003     	ldm	r8, {r0, r1}
   36818: e8870003     	stm	r7, {r0, r1}
   3681c: e8950003     	ldm	r5, {r0, r1}
   36820: e8880003     	stm	r8, {r0, r1}
   36824: e5dd3038     	ldrb	r3, [sp, #0x38]
   36828: e594c064     	ldr	r12, [r4, #0x64]
   3682c: e59d0034     	ldr	r0, [sp, #0x34]
   36830: e59d1030     	ldr	r1, [sp, #0x30]
   36834: e58dc034     	str	r12, [sp, #0x34]
   36838: e5840064     	str	r0, [r4, #0x64]
   3683c: e5841060     	str	r1, [r4, #0x60]
   36840: e58d2030     	str	r2, [sp, #0x30]
   36844: e35300ff     	cmp	r3, #255
   36848: 03a03000     	moveq	r3, #0
   3684c: 0a000002     	beq	0x3685c
   36850: e3022280     	movw	r2, #0x2280
   36854: e3402007     	movt	r2, #0x7
   36858: e7923103     	ldr	r3, [r2, r3, lsl #2]
   3685c: e1a01007     	mov	r1, r7
   36860: e1a00005     	mov	r0, r5
   36864: e12fff33     	blx	r3
   36868: e1a0000a     	mov	r0, r10
   3686c: eb00d538     	bl	0x6bd54
   36870: e59d3010     	ldr	r3, [sp, #0x10]
   36874: e3530000     	cmp	r3, #0
   36878: 0a000003     	beq	0x3688c
   3687c: e3a02003     	mov	r2, #3
   36880: e1a01009     	mov	r1, r9
   36884: e1a00009     	mov	r0, r9
   36888: e12fff33     	blx	r3
   3688c: e9940044     	ldmib	r4, {r2, r6}
   36890: e3560000     	cmp	r6, #0
   36894: 0affffa9     	beq	0x36740
   36898: eaffff9c     	b	0x36710
   3689c: e2800010     	add	r0, r0, #16
   368a0: eb00d4dd     	bl	0x6bc1c
   368a4: e5942008     	ldr	r2, [r4, #0x8]
   368a8: e5845004     	str	r5, [r4, #0x4]
   368ac: e3520000     	cmp	r2, #0
   368b0: 0affffa4     	beq	0x36748
   368b4: e2423001     	sub	r3, r2, #1
   368b8: e5c4500d     	strb	r5, [r4, #0xd]
   368bc: e16f3f13     	clz	r3, r3
   368c0: e1a02005     	mov	r2, r5
   368c4: e1a032a3     	lsr	r3, r3, #5
   368c8: eaffff95     	b	0x36724
   368cc: e3a03000     	mov	r3, #0
   368d0: e5c4200d     	strb	r2, [r4, #0xd]
   368d4: e5c4300e     	strb	r3, [r4, #0xe]
   368d8: e5842008     	str	r2, [r4, #0x8]
   368dc: e28dd040     	add	sp, sp, #64
   368e0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   368e4: e1a0200d     	mov	r2, sp
   368e8: e28d5018     	add	r5, sp, #24
   368ec: e8970003     	ldm	r7, {r0, r1}
   368f0: e284c058     	add	r12, r4, #88
   368f4: e8820003     	stm	r2, {r0, r1}
   368f8: e8950003     	ldm	r5, {r0, r1}
   368fc: e58d6030     	str	r6, [sp, #0x30]
   36900: e8870003     	stm	r7, {r0, r1}
   36904: e8920003     	ldm	r2, {r0, r1}
   36908: e8850003     	stm	r5, {r0, r1}
   3690c: e89c0003     	ldm	r12, {r0, r1}
   36910: e59d6024     	ldr	r6, [sp, #0x24]
   36914: e8850003     	stm	r5, {r0, r1}
   36918: e8920003     	ldm	r2, {r0, r1}
   3691c: e88c0003     	stm	r12, {r0, r1}
   36920: e58d6034     	str	r6, [sp, #0x34]
   36924: e5946060     	ldr	r6, [r4, #0x60]
   36928: e5942064     	ldr	r2, [r4, #0x64]
   3692c: e3560000     	cmp	r6, #0
   36930: e58d6020     	str	r6, [sp, #0x20]
   36934: e5848060     	str	r8, [r4, #0x60]
   36938: e58d2024     	str	r2, [sp, #0x24]
   3693c: e584e064     	str	lr, [r4, #0x64]
   36940: 0affffc2     	beq	0x36850
   36944: e3a02003     	mov	r2, #3
   36948: e1a01005     	mov	r1, r5
   3694c: e1a00005     	mov	r0, r5
   36950: e12fff36     	blx	r6
   36954: e5dd3038     	ldrb	r3, [sp, #0x38]
   36958: eaffffb9     	b	0x36844
   3695c: e28d5028     	add	r5, sp, #40
   36960: e1a01009     	mov	r1, r9
   36964: e3a02002     	mov	r2, #2
   36968: e1a00005     	mov	r0, r5
   3696c: e58d6030     	str	r6, [sp, #0x30]
   36970: ebffff4b     	bl	0x366a4
   36974: e28d2018     	add	r2, sp, #24
   36978: e2843058     	add	r3, r4, #88
   3697c: e8950003     	ldm	r5, {r0, r1}
   36980: e8820003     	stm	r2, {r0, r1}
   36984: e8930003     	ldm	r3, {r0, r1}
   36988: e8850003     	stm	r5, {r0, r1}
   3698c: e8920003     	ldm	r2, {r0, r1}
   36990: e8830003     	stm	r3, {r0, r1}
   36994: e59dc014     	ldr	r12, [sp, #0x14]
   36998: e5943060     	ldr	r3, [r4, #0x60]
   3699c: e5941064     	ldr	r1, [r4, #0x64]
   369a0: e59d2010     	ldr	r2, [sp, #0x10]
   369a4: e3530000     	cmp	r3, #0
   369a8: e58d3030     	str	r3, [sp, #0x30]
   369ac: e58d1034     	str	r1, [sp, #0x34]
   369b0: e584c064     	str	r12, [r4, #0x64]
   369b4: e5842060     	str	r2, [r4, #0x60]
   369b8: 0affffaa     	beq	0x36868
   369bc: e3a02003     	mov	r2, #3
   369c0: e1a01005     	mov	r1, r5
   369c4: e1a00005     	mov	r0, r5
   369c8: e12fff33     	blx	r3
   369cc: eaffffa5     	b	0x36868
   369d0: e59d3010     	ldr	r3, [sp, #0x10]
   369d4: e3530000     	cmp	r3, #0
   369d8: 0a000003     	beq	0x369ec
   369dc: e3a02003     	mov	r2, #3
   369e0: e1a01009     	mov	r1, r9
   369e4: e1a00009     	mov	r0, r9
   369e8: e12fff33     	blx	r3
   369ec: ebff7d5b     	bl	0x15f60    @ imm = #-0x20a94 ; __cxa_end_cleanup
   369f0: 00 65 cd 1d  	.word	0x1dcd6500
   369f4: 00 00 00 00  	.word	0x00000000
