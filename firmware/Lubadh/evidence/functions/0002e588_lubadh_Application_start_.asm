; lubadh::Application::start()
; VA 0x2e588 size 556

   2e588: e92d40f0     	push	{r4, r5, r6, r7, lr}
   2e58c: e2807915     	add	r7, r0, #344064
   2e590: e2804ba7     	add	r4, r0, #171008
   2e594: e24dd064     	sub	sp, sp, #100
   2e598: e1a06000     	mov	r6, r0
   2e59c: e2844ff1     	add	r4, r4, #964
   2e5a0: e3a03001     	mov	r3, #1
   2e5a4: e5c73abd     	strb	r3, [r7, #0xabd]
   2e5a8: e1a00004     	mov	r0, r4
   2e5ac: eb00290d     	bl	0x389e8
   2e5b0: e3500000     	cmp	r0, #0
   2e5b4: 1afffffb     	bne	0x2e5a8
   2e5b8: e30454fc     	movw	r5, #0x44fc
   2e5bc: e3405005     	movt	r5, #0x5
   2e5c0: e0865005     	add	r5, r6, r5
   2e5c4: e1a00005     	mov	r0, r5
   2e5c8: eb002906     	bl	0x389e8
   2e5cc: e2504000     	subs	r4, r0, #0
   2e5d0: 1afffffb     	bne	0x2e5c4
   2e5d4: e1a00006     	mov	r0, r6
   2e5d8: ebfff316     	bl	0x2b238
   2e5dc: e1a00006     	mov	r0, r6
   2e5e0: ebffe5a3     	bl	0x27c74
   2e5e4: e59f11c4     	ldr	r1, [pc, #0x1c4]        @ 0x2e7b0
   2e5e8: e1a02004     	mov	r2, r4
   2e5ec: e28d0044     	add	r0, sp, #68
   2e5f0: eb01067c     	bl	0x6ffe8
   2e5f4: e28d0044     	add	r0, sp, #68
   2e5f8: eb010888     	bl	0x70820
   2e5fc: e28d0044     	add	r0, sp, #68
   2e600: eb010886     	bl	0x70820
   2e604: e28d0044     	add	r0, sp, #68
   2e608: eb0107a0     	bl	0x70490
   2e60c: e1a00006     	mov	r0, r6
   2e610: ebffec2f     	bl	0x296d4
   2e614: e5970ab8     	ldr	r0, [r7, #0xab8]
   2e618: eb001fa1     	bl	0x364a4
   2e61c: e28d0044     	add	r0, sp, #68
   2e620: e3011ad0     	movw	r1, #0x1ad0
   2e624: e3401007     	movt	r1, #0x7
   2e628: ebffe55e     	bl	0x27ba8
   2e62c: e3090fec     	movw	r0, #0x9fec
   2e630: e3400009     	movt	r0, #0x9
   2e634: e3a02000     	mov	r2, #0
   2e638: e28d1044     	add	r1, sp, #68
   2e63c: eb010637     	bl	0x6ff20
   2e640: e59d0044     	ldr	r0, [sp, #0x44]
   2e644: e28d404c     	add	r4, sp, #76
   2e648: e1500004     	cmp	r0, r4
   2e64c: 0a000000     	beq	0x2e654
   2e650: ebff9dfa     	bl	0x15e40    @ imm = #-0x18818 ; _ZdlPv
   2e654: e28d000c     	add	r0, sp, #12
   2e658: ebff9cc9     	bl	0x15984     @ imm = #-0x18cdc ; sigemptyset
   2e65c: e3a01002     	mov	r1, #2
   2e660: e28d000c     	add	r0, sp, #12
   2e664: ebff9d7a     	bl	0x15c54    @ imm = #-0x18a18 ; sigaddset
   2e668: e28d000c     	add	r0, sp, #12
   2e66c: e28d1008     	add	r1, sp, #8
   2e670: ebff9d74     	bl	0x15c48    @ imm = #-0x18a30 ; sigwait
   2e674: e59d0008     	ldr	r0, [sp, #0x8]
   2e678: e3500002     	cmp	r0, #2
   2e67c: 0a00002f     	beq	0x2e740
   2e680: e3a02010     	mov	r2, #16
   2e684: e58d0000     	str	r0, [sp]
   2e688: e3003d28     	movw	r3, #0xd28
   2e68c: e3403007     	movt	r3, #0x7
   2e690: e28d0014     	add	r0, sp, #20
   2e694: e3061218     	movw	r1, #0x6218
   2e698: e3401001     	movt	r1, #0x1
   2e69c: ebffee35     	bl	0x29f78
   2e6a0: e3a02000     	mov	r2, #0
   2e6a4: e3a0c015     	mov	r12, #21
   2e6a8: e3013ae4     	movw	r3, #0x1ae4
   2e6ac: e3403007     	movt	r3, #0x7
   2e6b0: e28d0014     	add	r0, sp, #20
   2e6b4: e1a01002     	mov	r1, r2
   2e6b8: e58dc000     	str	r12, [sp]
   2e6bc: ebff9d0d     	bl	0x15af8    @ imm = #-0x18bcc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   2e6c0: e1a01000     	mov	r1, r0
   2e6c4: e28d002c     	add	r0, sp, #44
   2e6c8: ebff9d4c     	bl	0x15c00    @ imm = #-0x18ad0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2e6cc: e3011afc     	movw	r1, #0x1afc
   2e6d0: e3401007     	movt	r1, #0x7
   2e6d4: e28d002c     	add	r0, sp, #44
   2e6d8: ebff9f5b     	bl	0x1644c    @ imm = #-0x18294 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2e6dc: e1a01000     	mov	r1, r0
   2e6e0: e28d0044     	add	r0, sp, #68
   2e6e4: ebff9d45     	bl	0x15c00    @ imm = #-0x18aec ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2e6e8: e3090fec     	movw	r0, #0x9fec
   2e6ec: e3400009     	movt	r0, #0x9
   2e6f0: e28d1044     	add	r1, sp, #68
   2e6f4: e3a02000     	mov	r2, #0
   2e6f8: eb010608     	bl	0x6ff20
   2e6fc: e59d0044     	ldr	r0, [sp, #0x44]
   2e700: e1500004     	cmp	r0, r4
   2e704: 0a000000     	beq	0x2e70c
   2e708: ebff9dcc     	bl	0x15e40    @ imm = #-0x188d0 ; _ZdlPv
   2e70c: e59d002c     	ldr	r0, [sp, #0x2c]
   2e710: e28d3034     	add	r3, sp, #52
   2e714: e1500003     	cmp	r0, r3
   2e718: 0a000000     	beq	0x2e720
   2e71c: ebff9dc7     	bl	0x15e40    @ imm = #-0x188e4 ; _ZdlPv
   2e720: e59d0014     	ldr	r0, [sp, #0x14]
   2e724: e28d301c     	add	r3, sp, #28
   2e728: e1500003     	cmp	r0, r3
   2e72c: 0a000000     	beq	0x2e734
   2e730: ebff9dc2     	bl	0x15e40    @ imm = #-0x188f8 ; _ZdlPv
   2e734: e3a00000     	mov	r0, #0
   2e738: e28dd064     	add	sp, sp, #100
   2e73c: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   2e740: ebff9e09     	bl	0x15f6c    @ imm = #-0x187dc ; raise
   2e744: e59d0008     	ldr	r0, [sp, #0x8]
   2e748: eaffffcc     	b	0x2e680
   2e74c: e59d0044     	ldr	r0, [sp, #0x44]
   2e750: e28d304c     	add	r3, sp, #76
   2e754: e1500003     	cmp	r0, r3
   2e758: 0a000000     	beq	0x2e760
   2e75c: ebff9db7     	bl	0x15e40    @ imm = #-0x18924 ; _ZdlPv
   2e760: ebff9dfe     	bl	0x15f60    @ imm = #-0x18808 ; __cxa_end_cleanup
   2e764: e28d0044     	add	r0, sp, #68
   2e768: eb010748     	bl	0x70490
   2e76c: ebff9dfb     	bl	0x15f60    @ imm = #-0x18814 ; __cxa_end_cleanup
   2e770: e59d0044     	ldr	r0, [sp, #0x44]
   2e774: e1500004     	cmp	r0, r4
   2e778: 0a000000     	beq	0x2e780
   2e77c: ebff9daf     	bl	0x15e40    @ imm = #-0x18944 ; _ZdlPv
   2e780: e59d002c     	ldr	r0, [sp, #0x2c]
   2e784: e28d3034     	add	r3, sp, #52
   2e788: e1500003     	cmp	r0, r3
   2e78c: 0a000000     	beq	0x2e794
   2e790: ebff9daa     	bl	0x15e40    @ imm = #-0x18958 ; _ZdlPv
   2e794: e59d0014     	ldr	r0, [sp, #0x14]
   2e798: e28d301c     	add	r3, sp, #28
   2e79c: e1500003     	cmp	r0, r3
   2e7a0: 1affffed     	bne	0x2e75c
   2e7a4: eaffffed     	b	0x2e760
   2e7a8: eafffff4     	b	0x2e780
   2e7ac: eafffff8     	b	0x2e794
   2e7b0: ac fc 08 00  	.word	0x0008fcac
