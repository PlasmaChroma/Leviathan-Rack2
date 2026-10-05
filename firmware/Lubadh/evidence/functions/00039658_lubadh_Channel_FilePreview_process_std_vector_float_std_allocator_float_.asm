; lubadh::Channel::FilePreview::process(std::vector<float, std::allocator<float> >&)
; VA 0x39658 size 432

   39658: e92d4030     	push	{r4, r5, lr}
   3965c: e1a04000     	mov	r4, r0
   39660: e5d03030     	ldrb	r3, [r0, #0x30]
   39664: e24dd01c     	sub	sp, sp, #28
   39668: e1a05001     	mov	r5, r1
   3966c: e3530000     	cmp	r3, #0
   39670: 1a000005     	bne	0x3968c
   39674: e5903000     	ldr	r3, [r0]
   39678: e593301c     	ldr	r3, [r3, #0x1c]
   3967c: e2833a2a     	add	r3, r3, #172032
   39680: e5d33328     	ldrb	r3, [r3, #0x328]
   39684: e3530000     	cmp	r3, #0
   39688: 0a000007     	beq	0x396ac
   3968c: e5943078     	ldr	r3, [r4, #0x78]
   39690: e5933000     	ldr	r3, [r3]
   39694: e3530003     	cmp	r3, #3
   39698: 0a000005     	beq	0x396b4
   3969c: e3530004     	cmp	r3, #4
   396a0: 0a00001a     	beq	0x39710
   396a4: e3530000     	cmp	r3, #0
   396a8: 05c43030     	strbeq	r3, [r4, #0x30]
   396ac: e28dd01c     	add	sp, sp, #28
   396b0: e8bd8030     	pop	{r4, r5, pc}
   396b4: e5941000     	ldr	r1, [r4]
   396b8: e1a0000d     	mov	r0, sp
   396bc: e30224b8     	movw	r2, #0x24b8
   396c0: e3402007     	movt	r2, #0x7
   396c4: e2811004     	add	r1, r1, #4
   396c8: ebffd53e     	bl	0x2ebc8
   396cc: e3090fec     	movw	r0, #0x9fec
   396d0: e3400009     	movt	r0, #0x9
   396d4: e1a0100d     	mov	r1, sp
   396d8: e3a02000     	mov	r2, #0
   396dc: eb00da0f     	bl	0x6ff20
   396e0: e59d0000     	ldr	r0, [sp]
   396e4: e28d3008     	add	r3, sp, #8
   396e8: e1500003     	cmp	r0, r3
   396ec: 0a000000     	beq	0x396f4
   396f0: ebff71d2     	bl	0x15e40    @ imm = #-0x238b8 ; _ZdlPv
   396f4: e1a00004     	mov	r0, r4
   396f8: ebfffed7     	bl	0x3925c
   396fc: e5943000     	ldr	r3, [r4]
   39700: eddf0b3e     	vldr	d16, [pc, #248]         @ 0x39800>&)+0x1a8> ; float 6.36598738204e-313
   39704: edc30ba4     	vstr	d16, [r3, #656]
   39708: e28dd01c     	add	sp, sp, #28
   3970c: e8bd8030     	pop	{r4, r5, pc}
   39710: e5d42031     	ldrb	r2, [r4, #0x31]
   39714: e3520000     	cmp	r2, #0
   39718: 0a00002a     	beq	0x397c8
   3971c: e594c028     	ldr	r12, [r4, #0x28]
   39720: e5953000     	ldr	r3, [r5]
   39724: e5950004     	ldr	r0, [r5, #0x4]
   39728: e26c190f     	rsb	r1, r12, #245760
   3972c: e281105a     	add	r1, r1, #90
   39730: e0402003     	sub	r2, r0, r3
   39734: e1510142     	cmp	r1, r2, asr #2
   39738: 9a000010     	bls	0x39780
   3973c: e1530000     	cmp	r3, r0
   39740: 0affffd9     	beq	0x396ac
   39744: e5941008     	ldr	r1, [r4, #0x8]
   39748: e1a02003     	mov	r2, r3
   3974c: e081110c     	add	r1, r1, r12, lsl #2
   39750: ecb17a01     	vldmia	r1!, {s14}
   39754: edd27a00     	vldr	s15, [r2]
   39758: ee777a87     	vadd.f32	s15, s15, s14
   3975c: ece27a01     	vstmia	r2!, {s15}
   39760: e1500002     	cmp	r0, r2
   39764: 1afffff9     	bne	0x39750
   39768: e2400004     	sub	r0, r0, #4
   3976c: e28c2001     	add	r2, r12, #1
   39770: e0403003     	sub	r3, r0, r3
   39774: e0823123     	add	r3, r2, r3, lsr #2
   39778: e5843028     	str	r3, [r4, #0x28]
   3977c: eaffffca     	b	0x396ac
   39780: e3510000     	cmp	r1, #0
   39784: 0a00000b     	beq	0x397b8
   39788: e5942008     	ldr	r2, [r4, #0x8]
   3978c: e0831101     	add	r1, r3, r1, lsl #2
   39790: e082210c     	add	r2, r2, r12, lsl #2
   39794: ecb27a01     	vldmia	r2!, {s14}
   39798: edd37a00     	vldr	s15, [r3]
   3979c: ee777a87     	vadd.f32	s15, s15, s14
   397a0: ece37a01     	vstmia	r3!, {s15}
   397a4: e1510003     	cmp	r1, r3
   397a8: 1afffff9     	bne	0x39794
   397ac: e30c305a     	movw	r3, #0xc05a
   397b0: e3403003     	movt	r3, #0x3
   397b4: e5843028     	str	r3, [r4, #0x28]
   397b8: e1a00004     	mov	r0, r4
   397bc: ebfffea6     	bl	0x3925c
   397c0: e28dd01c     	add	sp, sp, #28
   397c4: e8bd8030     	pop	{r4, r5, pc}
   397c8: e5943000     	ldr	r3, [r4]
   397cc: e2831a2a     	add	r1, r3, #172032
   397d0: e5930020     	ldr	r0, [r3, #0x20]
   397d4: e5d11496     	ldrb	r1, [r1, #0x496]
   397d8: eb00cc6d     	bl	0x6c994
   397dc: e3a03001     	mov	r3, #1
   397e0: e5c43031     	strb	r3, [r4, #0x31]
   397e4: eaffffcc     	b	0x3971c
   397e8: e59d0000     	ldr	r0, [sp]
   397ec: e28d3008     	add	r3, sp, #8
   397f0: e1500003     	cmp	r0, r3
   397f4: 0a000000     	beq	0x397fc
   397f8: ebff7190     	bl	0x15e40    @ imm = #-0x239c0 ; _ZdlPv
   397fc: ebff71d7     	bl	0x15f60    @ imm = #-0x238a4 ; __cxa_end_cleanup
   39800: b9 00 00 00  	.word	0x000000b9
   39804: 1e 00 00 00  	.word	0x0000001e
