; lubadh::Application::initDisplay()
; VA 0x296d4 size 268

   296d4: e2803a2a     	add	r3, r0, #172032
   296d8: e92d4010     	push	{r4, lr}
   296dc: e3a02000     	mov	r2, #0
   296e0: e24dd008     	sub	sp, sp, #8
   296e4: e1a04000     	mov	r4, r0
   296e8: e5d314de     	ldrb	r1, [r3, #0x4de]
   296ec: eb010ca8     	bl	0x6c994
   296f0: e2843915     	add	r3, r4, #344064
   296f4: e3a02000     	mov	r2, #0
   296f8: e1a00004     	mov	r0, r4
   296fc: e5d31a16     	ldrb	r1, [r3, #0xa16]
   29700: eb010ca3     	bl	0x6c994
   29704: e3a02000     	mov	r2, #0
   29708: e3a01098     	mov	r1, #152
   2970c: e1a00004     	mov	r0, r4
   29710: eb010c9f     	bl	0x6c994
   29714: e3a02000     	mov	r2, #0
   29718: e3a01099     	mov	r1, #153
   2971c: e1a00004     	mov	r0, r4
   29720: eb010c9b     	bl	0x6c994
   29724: e3a02000     	mov	r2, #0
   29728: e3a01088     	mov	r1, #136
   2972c: e1a00004     	mov	r0, r4
   29730: eb010c97     	bl	0x6c994
   29734: e3a02000     	mov	r2, #0
   29738: e3a01089     	mov	r1, #137
   2973c: e1a00004     	mov	r0, r4
   29740: eb010c93     	bl	0x6c994
   29744: e3a00008     	mov	r0, #8
   29748: e3a03000     	mov	r3, #0
   2974c: e58d3000     	str	r3, [sp]
   29750: ebffb06d     	bl	0x1590c     @ imm = #-0x13e4c ; _Znwj
   29754: e1a03000     	mov	r3, r0
   29758: e59fc07c     	ldr	r12, [pc, #0x7c]        @ 0x297dc
   2975c: e3052aa4     	movw	r2, #0x5aa4
   29760: e3402001     	movt	r2, #0x1
   29764: e1a0000d     	mov	r0, sp
   29768: e28d1004     	add	r1, sp, #4
   2976c: e5834004     	str	r4, [r3, #0x4]
   29770: e583c000     	str	r12, [r3]
   29774: e58d3004     	str	r3, [sp, #0x4]
   29778: ebffb19e     	bl	0x15df8    @ imm = #-0x13988 ; _ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE
   2977c: e59d0004     	ldr	r0, [sp, #0x4]
   29780: e3500000     	cmp	r0, #0
   29784: 0a000002     	beq	0x29794
   29788: e5903000     	ldr	r3, [r0]
   2978c: e5933004     	ldr	r3, [r3, #0x4]
   29790: e12fff33     	blx	r3
   29794: e1a0000d     	mov	r0, sp
   29798: ebffb2f5     	bl	0x16374    @ imm = #-0x1342c ; _ZNSt6thread6detachEv
   2979c: e59d3000     	ldr	r3, [sp]
   297a0: e3530000     	cmp	r3, #0
   297a4: 1a000004     	bne	0x297bc
   297a8: e28dd008     	add	sp, sp, #8
   297ac: e8bd8010     	pop	{r4, pc}
   297b0: e59d3000     	ldr	r3, [sp]
   297b4: e3530000     	cmp	r3, #0
   297b8: 0a000006     	beq	0x297d8
   297bc: ebffb09d     	bl	0x15a38    @ imm = #-0x13d8c ; _ZSt9terminatev
   297c0: e59d0004     	ldr	r0, [sp, #0x4]
   297c4: e3500000     	cmp	r0, #0
   297c8: 0a000002     	beq	0x297d8
   297cc: e5903000     	ldr	r3, [r0]
   297d0: e5933004     	ldr	r3, [r3, #0x4]
   297d4: e12fff33     	blx	r3
   297d8: ebffb1e0     	bl	0x15f60    @ imm = #-0x13880 ; __cxa_end_cleanup
   297dc: e4 1f 07 00  	.word	0x00071fe4
