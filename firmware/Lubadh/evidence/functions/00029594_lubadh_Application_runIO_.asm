; lubadh::Application::runIO()
; VA 0x29594 size 320

   29594: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   29598: e2806915     	add	r6, r0, #344064
   2959c: e2805ba9     	add	r5, r0, #173056
   295a0: e2808a7e     	add	r8, r0, #516096
   295a4: e1a04000     	mov	r4, r0
   295a8: e5963ac0     	ldr	r3, [r6, #0xac0]
   295ac: e2855d06     	add	r5, r5, #384
   295b0: e2888eff     	add	r8, r8, #4080
   295b4: e2807048     	add	r7, r0, #72
   295b8: e2833001     	add	r3, r3, #1
   295bc: e3530005     	cmp	r3, #5
   295c0: c3a03000     	movgt	r3, #0
   295c4: e5863ac0     	str	r3, [r6, #0xac0]
   295c8: ebfffb75     	bl	0x283a4
   295cc: e2459a2a     	sub	r9, r5, #172032
   295d0: e55510b3     	ldrb	r1, [r5, #-0xb3]
   295d4: e1a00004     	mov	r0, r4
   295d8: eb010ab9     	bl	0x6c0c4
   295dc: e1a01000     	mov	r1, r0
   295e0: e2490fe2     	sub	r0, r9, #904
   295e4: eb003415     	bl	0x36640
   295e8: e55510af     	ldrb	r1, [r5, #-0xaf]
   295ec: e1a00004     	mov	r0, r4
   295f0: eb010ab3     	bl	0x6c0c4
   295f4: e1a01000     	mov	r1, r0
   295f8: e2490fe6     	sub	r0, r9, #920
   295fc: eb00340f     	bl	0x36640
   29600: e55510b1     	ldrb	r1, [r5, #-0xb1]
   29604: e1a00004     	mov	r0, r4
   29608: e2855ba9     	add	r5, r5, #173056
   2960c: eb010aac     	bl	0x6c0c4
   29610: e2855f4e     	add	r5, r5, #312
   29614: e1a01000     	mov	r1, r0
   29618: e2490fea     	sub	r0, r9, #936
   2961c: eb003407     	bl	0x36640
   29620: e1550008     	cmp	r5, r8
   29624: 1affffe8     	bne	0x295cc
   29628: e2848a2a     	add	r8, r4, #172032
   2962c: e5961ac0     	ldr	r1, [r6, #0xac0]
   29630: e1a00004     	mov	r0, r4
   29634: e2875ba9     	add	r5, r7, #173056
   29638: ebfffad4     	bl	0x28190
   2963c: e2855f4e     	add	r5, r5, #312
   29640: e59814e0     	ldr	r1, [r8, #0x4e0]
   29644: e1a00007     	mov	r0, r7
   29648: eb0037b0     	bl	0x37510
   2964c: e5961a18     	ldr	r1, [r6, #0xa18]
   29650: e1a00005     	mov	r0, r5
   29654: eb0037ad     	bl	0x37510
   29658: e594114c     	ldr	r1, [r4, #0x14c]
   2965c: e1a00007     	mov	r0, r7
   29660: e3a02000     	mov	r2, #0
   29664: e2411001     	sub	r1, r1, #1
   29668: e3510001     	cmp	r1, #1
   2966c: 83a01000     	movhi	r1, #0
   29670: 93a01001     	movls	r1, #1
   29674: eb00380c     	bl	0x376ac
   29678: e5981684     	ldr	r1, [r8, #0x684]
   2967c: e1a00005     	mov	r0, r5
   29680: e3a02000     	mov	r2, #0
   29684: e2411001     	sub	r1, r1, #1
   29688: e3510001     	cmp	r1, #1
   2968c: 83a01000     	movhi	r1, #0
   29690: 93a01001     	movls	r1, #1
   29694: eb003804     	bl	0x376ac
   29698: e59814f0     	ldr	r1, [r8, #0x4f0]
   2969c: e1a00007     	mov	r0, r7
   296a0: e3a02000     	mov	r2, #0
   296a4: eb0038e6     	bl	0x37a44
   296a8: e5961a28     	ldr	r1, [r6, #0xa28]
   296ac: e1a00005     	mov	r0, r5
   296b0: e3a02000     	mov	r2, #0
   296b4: eb0038e2     	bl	0x37a44
   296b8: e1a00004     	mov	r0, r4
   296bc: ebfffb7b     	bl	0x284b0
   296c0: e1a00004     	mov	r0, r4
   296c4: ebfffc1a     	bl	0x28734
   296c8: e1a00004     	mov	r0, r4
   296cc: e8bd47f0     	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
   296d0: eafff98c     	b	0x27d08
