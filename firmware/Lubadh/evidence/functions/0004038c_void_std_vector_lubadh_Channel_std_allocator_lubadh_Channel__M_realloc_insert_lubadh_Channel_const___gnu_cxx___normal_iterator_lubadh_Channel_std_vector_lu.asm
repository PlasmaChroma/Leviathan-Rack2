; void std::vector<lubadh::Channel*, std::allocator<lubadh::Channel*> >::_M_realloc_insert<lubadh::Channel* const&>(__gnu_cxx::__normal_iterator<lubadh::Channel**, std::vector<lubadh::Channel*, std::allocator<lubadh::Channel*> > >, lubadh::Channel* const&)
; VA 0x4038c size 280

   4038c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   40390: e8900180     	ldm	r0, {r7, r8}
   40394: e24dd00c     	sub	sp, sp, #12
   40398: e0483007     	sub	r3, r8, r7
   4039c: e1a03143     	asr	r3, r3, #2
   403a0: e373021e     	cmn	r3, #-536870911
   403a4: 0a000036     	beq	0x40484
   403a8: e1a05000     	mov	r5, r0
   403ac: e1a06001     	mov	r6, r1
   403b0: e1a09002     	mov	r9, r2
   403b4: e3530000     	cmp	r3, #0
   403b8: e041a007     	sub	r10, r1, r7
   403bc: 0a00002e     	beq	0x4047c
   403c0: e1530083     	cmp	r3, r3, lsl #1
   403c4: e1a04083     	lsl	r4, r3, #1
   403c8: 83e0410e     	mvnhi	r4, #-2147483645
   403cc: 9a00001f     	bls	0x40450
   403d0: e1a00004     	mov	r0, r4
   403d4: ebff554c     	bl	0x1590c     @ imm = #-0x2aad0 ; _Znwj
   403d8: e1a0b000     	mov	r11, r0
   403dc: e0804004     	add	r4, r0, r4
   403e0: e28a2004     	add	r2, r10, #4
   403e4: e5991000     	ldr	r1, [r9]
   403e8: e0483006     	sub	r3, r8, r6
   403ec: e08b9002     	add	r9, r11, r2
   403f0: e35a0000     	cmp	r10, #0
   403f4: e0898003     	add	r8, r9, r3
   403f8: e78b100a     	str	r1, [r11, r10]
   403fc: ca000008     	bgt	0x40424
   40400: e3530000     	cmp	r3, #0
   40404: ca000015     	bgt	0x40460
   40408: e3570000     	cmp	r7, #0
   4040c: 1a00000c     	bne	0x40444
   40410: e585b000     	str	r11, [r5]
   40414: e5858004     	str	r8, [r5, #0x4]
   40418: e5854008     	str	r4, [r5, #0x8]
   4041c: e28dd00c     	add	sp, sp, #12
   40420: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   40424: e1a0200a     	mov	r2, r10
   40428: e1a01007     	mov	r1, r7
   4042c: e1a0000b     	mov	r0, r11
   40430: e58d3004     	str	r3, [sp, #0x4]
   40434: ebff556a     	bl	0x159e4    @ imm = #-0x2aa58 ; memmove
   40438: e59d3004     	ldr	r3, [sp, #0x4]
   4043c: e3530000     	cmp	r3, #0
   40440: ca000006     	bgt	0x40460
   40444: e1a00007     	mov	r0, r7
   40448: ebff567c     	bl	0x15e40    @ imm = #-0x2a610 ; _ZdlPv
   4044c: eaffffef     	b	0x40410
   40450: e3540000     	cmp	r4, #0
   40454: 1a00000d     	bne	0x40490
   40458: e1a0b004     	mov	r11, r4
   4045c: eaffffdf     	b	0x403e0
   40460: e1a02003     	mov	r2, r3
   40464: e1a01006     	mov	r1, r6
   40468: e1a00009     	mov	r0, r9
   4046c: ebff56f4     	bl	0x16044    @ imm = #-0x2a430 ; memcpy
   40470: e3570000     	cmp	r7, #0
   40474: 0affffe5     	beq	0x40410
   40478: eafffff1     	b	0x40444
   4047c: e3a04004     	mov	r4, #4
   40480: eaffffd2     	b	0x403d0
   40484: e3000c8c     	movw	r0, #0xc8c
   40488: e3400007     	movt	r0, #0x7
   4048c: ebff55c9     	bl	0x15bb8    @ imm = #-0x2a8dc ; _ZSt20__throw_length_errorPKc
   40490: e3e0320e     	mvn	r3, #-536870912
   40494: e1540003     	cmp	r4, r3
   40498: 21a04003     	movhs	r4, r3
   4049c: e1a04104     	lsl	r4, r4, #2
   404a0: eaffffca     	b	0x403d0
