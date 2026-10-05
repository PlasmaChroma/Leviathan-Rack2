; lubadh::Application::readADCs(int)
; VA 0x28190 size 380

   28190: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   28194: e2808a2a     	add	r8, r0, #172032
   28198: e1a04000     	mov	r4, r0
   2819c: e1a05001     	mov	r5, r1
   281a0: e2807915     	add	r7, r0, #344064
   281a4: e5d814c8     	ldrb	r1, [r8, #0x4c8]
   281a8: e2806ba9     	add	r6, r0, #173056
   281ac: eb010fac     	bl	0x6c064
   281b0: e1a01000     	mov	r1, r0
   281b4: e2840ba9     	add	r0, r4, #173056
   281b8: e3009fff     	movw	r9, #0xfff
   281bc: e2800d05     	add	r0, r0, #320
   281c0: e0491001     	sub	r1, r9, r1
   281c4: eb010fce     	bl	0x6c104
   281c8: e58804e0     	str	r0, [r8, #0x4e0]
   281cc: e2866d06     	add	r6, r6, #384
   281d0: e1a00004     	mov	r0, r4
   281d4: e5d71a00     	ldrb	r1, [r7, #0xa00]
   281d8: eb010fa1     	bl	0x6c064
   281dc: e1a01000     	mov	r1, r0
   281e0: e2860ba9     	add	r0, r6, #173056
   281e4: e0491001     	sub	r1, r9, r1
   281e8: e28000f8     	add	r0, r0, #248
   281ec: e2849048     	add	r9, r4, #72
   281f0: eb010fc3     	bl	0x6c104
   281f4: e5870a18     	str	r0, [r7, #0xa18]
   281f8: e3550005     	cmp	r5, #5
   281fc: 979ff105     	ldrls	pc, [pc, r5, lsl #2]
   28200: ea00000e     	b	0x28240
   28204: 44 82 02 00  	.word	0x00028244
   28208: 6c 82 02 00  	.word	0x0002826c
   2820c: 94 82 02 00  	.word	0x00028294
   28210: bc 82 02 00  	.word	0x000282bc
   28214: e4 82 02 00  	.word	0x000282e4
   28218: 1c 82 02 00  	.word	0x0002821c
   2821c: e5d71a02     	ldrb	r1, [r7, #0xa02]
   28220: e1a00004     	mov	r0, r4
   28224: eb010f8e     	bl	0x6c064
   28228: e2601eff     	rsb	r1, r0, #4080
   2822c: e2860ba9     	add	r0, r6, #173056
   28230: e281100f     	add	r1, r1, #15
   28234: e2800f4a     	add	r0, r0, #296
   28238: eb010fb1     	bl	0x6c104
   2823c: e5870a28     	str	r0, [r7, #0xa28]
   28240: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   28244: e5d814c9     	ldrb	r1, [r8, #0x4c9]
   28248: e1a00004     	mov	r0, r4
   2824c: eb010f84     	bl	0x6c064
   28250: e2601eff     	rsb	r1, r0, #4080
   28254: e2890ba9     	add	r0, r9, #173056
   28258: e281100f     	add	r1, r1, #15
   2825c: e2800f42     	add	r0, r0, #264
   28260: eb010fa7     	bl	0x6c104
   28264: e58804e8     	str	r0, [r8, #0x4e8]
   28268: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   2826c: e5d71a01     	ldrb	r1, [r7, #0xa01]
   28270: e1a00004     	mov	r0, r4
   28274: eb010f7a     	bl	0x6c064
   28278: e2601eff     	rsb	r1, r0, #4080
   2827c: e2860ba9     	add	r0, r6, #173056
   28280: e281100f     	add	r1, r1, #15
   28284: e2800f42     	add	r0, r0, #264
   28288: eb010f9d     	bl	0x6c104
   2828c: e5870a20     	str	r0, [r7, #0xa20]
   28290: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   28294: e5d814cb     	ldrb	r1, [r8, #0x4cb]
   28298: e1a00004     	mov	r0, r4
   2829c: eb010f70     	bl	0x6c064
   282a0: e2601eff     	rsb	r1, r0, #4080
   282a4: e2890ba9     	add	r0, r9, #173056
   282a8: e281100f     	add	r1, r1, #15
   282ac: e2800f46     	add	r0, r0, #280
   282b0: eb010f93     	bl	0x6c104
   282b4: e58804ec     	str	r0, [r8, #0x4ec]
   282b8: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   282bc: e5d71a03     	ldrb	r1, [r7, #0xa03]
   282c0: e1a00004     	mov	r0, r4
   282c4: eb010f66     	bl	0x6c064
   282c8: e2601eff     	rsb	r1, r0, #4080
   282cc: e2860ba9     	add	r0, r6, #173056
   282d0: e281100f     	add	r1, r1, #15
   282d4: e2800f46     	add	r0, r0, #280
   282d8: eb010f89     	bl	0x6c104
   282dc: e5870a24     	str	r0, [r7, #0xa24]
   282e0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   282e4: e5d814ca     	ldrb	r1, [r8, #0x4ca]
   282e8: e1a00004     	mov	r0, r4
   282ec: eb010f5c     	bl	0x6c064
   282f0: e2601eff     	rsb	r1, r0, #4080
   282f4: e2890ba9     	add	r0, r9, #173056
   282f8: e281100f     	add	r1, r1, #15
   282fc: e2800f4a     	add	r0, r0, #296
   28300: eb010f7f     	bl	0x6c104
   28304: e58804f0     	str	r0, [r8, #0x4f0]
   28308: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
