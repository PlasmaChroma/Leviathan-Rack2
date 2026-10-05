; lubadh::TapeFlutter::TapeFlutter()
; VA 0x4f1c0 size 608

   4f1c0: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   4f1c4: e2808024     	add	r8, r0, #36
   4f1c8: e2809050     	add	r9, r0, #80
   4f1cc: ed2d8b04     	vpush	{d8, d9}
   4f1d0: e24ddd27     	sub	sp, sp, #2496
   4f1d4: e24dd008     	sub	sp, sp, #8
   4f1d8: e1a04000     	mov	r4, r0
   4f1dc: e1a00008     	mov	r0, r8
   4f1e0: ebfff061     	bl	0x4b36c
   4f1e4: e1a00009     	mov	r0, r9
   4f1e8: ebfff05f     	bl	0x4b36c
   4f1ec: e30336f8     	movw	r3, #0x36f8
   4f1f0: e3403007     	movt	r3, #0x7
   4f1f4: e28d200c     	add	r2, sp, #12
   4f1f8: e2846e47     	add	r6, r4, #1136
   4f1fc: e286600c     	add	r6, r6, #12
   4f200: e8930003     	ldm	r3, {r0, r1}
   4f204: e58d000c     	str	r0, [sp, #0xc]
   4f208: e1c210b4     	strh	r1, [r2, #4]
   4f20c: e1a00006     	mov	r0, r6
   4f210: e58d2004     	str	r2, [sp, #0x4]
   4f214: e3a03007     	mov	r3, #7
   4f218: e1a01821     	lsr	r1, r1, #16
   4f21c: e5c21006     	strb	r1, [r2, #0x6]
   4f220: e28d1004     	add	r1, sp, #4
   4f224: e58d3008     	str	r3, [sp, #0x8]
   4f228: e3a03000     	mov	r3, #0
   4f22c: e5cd3013     	strb	r3, [sp, #0x13]
   4f230: ebff1c2b     	bl	0x162e4    @ imm = #-0x38f54 ; _ZNSt13random_device7_M_initERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   4f234: e59d0004     	ldr	r0, [sp, #0x4]
   4f238: e28d300c     	add	r3, sp, #12
   4f23c: e1500003     	cmp	r0, r3
   4f240: 0a000000     	beq	0x4f248
   4f244: ebff1afd     	bl	0x15e40    @ imm = #-0x3940c ; _ZdlPv
   4f248: e1a05004     	mov	r5, r4
   4f24c: e3011571     	movw	r1, #0x1571
   4f250: e3080965     	movw	r0, #0x8965
   4f254: e3460c07     	movt	r0, #0x6c07
   4f258: e1a03001     	mov	r3, r1
   4f25c: e3a02001     	mov	r2, #1
   4f260: e5a51e40     	str	r1, [r5, #0xe40]!
   4f264: e1a01005     	mov	r1, r5
   4f268: e0233f23     	eor	r3, r3, r3, lsr #30
   4f26c: e0232390     	mla	r3, r0, r3, r2
   4f270: e2822001     	add	r2, r2, #1
   4f274: e3520e27     	cmp	r2, #624
   4f278: e5a13004     	str	r3, [r1, #0x4]!
   4f27c: 1afffff9     	bne	0x4f268
   4f280: e2847a01     	add	r7, r4, #4096
   4f284: f2c02010     	vmov.i32	d18, #0x0
   4f288: e2873b02     	add	r3, r7, #2048
   4f28c: eddf3b57     	vldr	d19, [pc, #348]         @ 0x4f3f0 ; float 0.0078125
   4f290: e1a00003     	mov	r0, r3
   4f294: eddf1b57     	vldr	d17, [pc, #348]         @ 0x4f3f8 ; float 2048
   4f298: e5872800     	str	r2, [r7, #0x800]
   4f29c: e2833004     	add	r3, r3, #4
   4f2a0: e280000c     	add	r0, r0, #12
   4f2a4: eddf0b55     	vldr	d16, [pc, #340]         @ 0x4f400 ; float 5.15412883974e-315
   4f2a8: e2841008     	add	r1, r4, #8
   4f2ac: e2842014     	add	r2, r4, #20
   4f2b0: f443378f     	vst1.32	{d19}, [r3]
   4f2b4: e3a0c000     	mov	r12, #0
   4f2b8: f440278f     	vst1.32	{d18}, [r0]
   4f2bc: e3a03000     	mov	r3, #0
   4f2c0: e309099a     	movw	r0, #0x999a
   4f2c4: e3430f19     	movt	r0, #0x3f19
   4f2c8: e5873814     	str	r3, [r7, #0x814]
   4f2cc: ed9f0a4f     	vldr	s0, [pc, #316]          @ 0x4f410 ; float 8.13499937067e-05
   4f2d0: e5840000     	str	r0, [r4]
   4f2d4: e1a00008     	mov	r0, r8
   4f2d8: e5843004     	str	r3, [r4, #0x4]
   4f2dc: f441178f     	vst1.32	{d17}, [r1]
   4f2e0: e5843010     	str	r3, [r4, #0x10]
   4f2e4: f442078f     	vst1.32	{d16}, [r2]
   4f2e8: e584304c     	str	r3, [r4, #0x4c]
   4f2ec: e584c01c     	str	r12, [r4, #0x1c]
   4f2f0: ebffecb2     	bl	0x4a5c0
   4f2f4: e3a03001     	mov	r3, #1
   4f2f8: ed9f0a45     	vldr	s0, [pc, #276]          @ 0x4f414 ; float 4.06749968533e-05
   4f2fc: e1a00009     	mov	r0, r9
   4f300: e5843078     	str	r3, [r4, #0x78]
   4f304: ebffecad     	bl	0x4a5c0
   4f308: ed9f8a42     	vldr	s16, [pc, #264]         @ 0x4f418 ; float 0.00390625
   4f30c: e284a080     	add	r10, r4, #128
   4f310: ed9f9a41     	vldr	s18, [pc, #260]         @ 0x4f41c ; float 6.28318548203
   4f314: e3a03000     	mov	r3, #0
   4f318: e584307c     	str	r3, [r4, #0x7c]
   4f31c: eef08a48     	vmov.f32	s17, s16
   4f320: ee280a09     	vmul.f32	s0, s16, s18
   4f324: ee388a28     	vadd.f32	s16, s16, s17
   4f328: ebff1bbd     	bl	0x16224    @ imm = #-0x3910c ; sinf
   4f32c: ecaa0a01     	vstmia	r10!, {s0}
   4f330: e156000a     	cmp	r6, r10
   4f334: 1afffff9     	bne	0x4f320
   4f338: e1a00006     	mov	r0, r6
   4f33c: ebff1b6d     	bl	0x160f8    @ imm = #-0x3924c ; _ZNSt13random_device9_M_getvalEv
   4f340: e3081965     	movw	r1, #0x8965
   4f344: e3461c07     	movt	r1, #0x6c07
   4f348: e28d2004     	add	r2, sp, #4
   4f34c: e3a03001     	mov	r3, #1
   4f350: e58d0004     	str	r0, [sp, #0x4]
   4f354: e0200f20     	eor	r0, r0, r0, lsr #30
   4f358: e0203091     	mla	r0, r1, r0, r3
   4f35c: e2833001     	add	r3, r3, #1
   4f360: e3530e27     	cmp	r3, #624
   4f364: e5a20004     	str	r0, [r2, #0x4]!
   4f368: 1afffff9     	bne	0x4f354
   4f36c: e28d1004     	add	r1, sp, #4
   4f370: e1a00005     	mov	r0, r5
   4f374: e30029c4     	movw	r2, #0x9c4
   4f378: e2877b02     	add	r7, r7, #2048
   4f37c: e2877004     	add	r7, r7, #4
   4f380: e58d39c4     	str	r3, [sp, #0x9c4]
   4f384: ebff1b2e     	bl	0x16044    @ imm = #-0x39348 ; memcpy
   4f388: eddf0b1e     	vldr	d16, [pc, #120]         @ 0x4f408 ; float 0.00781250557338
   4f38c: e1a00004     	mov	r0, r4
   4f390: f447078f     	vst1.32	{d16}, [r7]
   4f394: e28ddd27     	add	sp, sp, #2496
   4f398: e28dd008     	add	sp, sp, #8
   4f39c: ecbd8b04     	vpop	{d8, d9}
   4f3a0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   4f3a4: e1a00008     	mov	r0, r8
   4f3a8: eb000457     	bl	0x5050c
   4f3ac: ebff1aeb     	bl	0x15f60    @ imm = #-0x39454 ; __cxa_end_cleanup
   4f3b0: e597080c     	ldr	r0, [r7, #0x80c]
   4f3b4: e3500000     	cmp	r0, #0
   4f3b8: 0a000000     	beq	0x4f3c0
   4f3bc: ebff1a9f     	bl	0x15e40    @ imm = #-0x39584 ; _ZdlPv
   4f3c0: e1a00006     	mov	r0, r6
   4f3c4: ebff1a94     	bl	0x15e1c    @ imm = #-0x395b0 ; _ZNSt13random_device7_M_finiEv
   4f3c8: e1a00009     	mov	r0, r9
   4f3cc: eb00044e     	bl	0x5050c
   4f3d0: eafffff3     	b	0x4f3a4
   4f3d4: e59d0004     	ldr	r0, [sp, #0x4]
   4f3d8: e28d300c     	add	r3, sp, #12
   4f3dc: e1500003     	cmp	r0, r3
   4f3e0: 0afffff8     	beq	0x4f3c8
   4f3e4: ebff1a95     	bl	0x15e40    @ imm = #-0x395ac ; _ZdlPv
   4f3e8: eafffff6     	b	0x4f3c8
   4f3ec: e320f000     	nop
   4f3f0: 00 00 00 00  	.word	0x00000000
   4f3f4: 00 00 80 3f  	.word	0x3f800000
   4f3f8: 00 00 00 00  	.word	0x00000000
   4f3fc: 00 00 a0 40  	.word	0x40a00000
   4f400: 7b 14 2e 3e  	.word	0x3e2e147b
   4f404: 00 00 00 00  	.word	0x00000000
   4f408: 00 00 80 bf  	.word	0xbf800000
   4f40c: 00 00 80 3f  	.word	0x3f800000
   4f410: 72 9a aa 38  	.word	0x38aa9a72
   4f414: 72 9a 2a 38  	.word	0x382a9a72
   4f418: 00 00 80 3b  	.word	0x3b800000
   4f41c: db 0f c9 40  	.word	0x40c90fdb
