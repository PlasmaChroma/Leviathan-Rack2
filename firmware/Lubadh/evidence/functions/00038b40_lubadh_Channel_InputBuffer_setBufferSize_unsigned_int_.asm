; lubadh::Channel::InputBuffer::setBufferSize(unsigned int)
; VA 0x38b40 size 280

   38b40: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   38b44: e2817004     	add	r7, r1, #4
   38b48: e1a04000     	mov	r4, r0
   38b4c: e9900048     	ldmib	r0, {r3, r6}
   38b50: e5801000     	str	r1, [r0]
   38b54: e0468003     	sub	r8, r6, r3
   38b58: e1a01148     	asr	r1, r8, #2
   38b5c: e1570001     	cmp	r7, r1
   38b60: 8a000004     	bhi	0x38b78
   38b64: 28bd87f0     	pophs	{r4, r5, r6, r7, r8, r9, r10, pc}
   38b68: e0833107     	add	r3, r3, r7, lsl #2
   38b6c: e1560003     	cmp	r6, r3
   38b70: 15803008     	strne	r3, [r0, #0x8]
   38b74: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   38b78: e590300c     	ldr	r3, [r0, #0xc]
   38b7c: e0479001     	sub	r9, r7, r1
   38b80: e3e0520e     	mvn	r5, #-536870912
   38b84: e0452001     	sub	r2, r5, r1
   38b88: e0433006     	sub	r3, r3, r6
   38b8c: e1590143     	cmp	r9, r3, asr #2
   38b90: 8a000007     	bhi	0x38bb4
   38b94: e1a09109     	lsl	r9, r9, #2
   38b98: e3a01000     	mov	r1, #0
   38b9c: e1a02009     	mov	r2, r9
   38ba0: e1a00006     	mov	r0, r6
   38ba4: e0869009     	add	r9, r6, r9
   38ba8: ebff7471     	bl	0x15d74    @ imm = #-0x22e3c ; memset
   38bac: e5849008     	str	r9, [r4, #0x8]
   38bb0: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   38bb4: e1590002     	cmp	r9, r2
   38bb8: 8a000023     	bhi	0x38c4c
   38bbc: e1510009     	cmp	r1, r9
   38bc0: 21a03001     	movhs	r3, r1
   38bc4: 31a03009     	movlo	r3, r9
   38bc8: e0911003     	adds	r1, r1, r3
   38bcc: 2a00001c     	bhs	0x38c44
   38bd0: e1510005     	cmp	r1, r5
   38bd4: 21a01005     	movhs	r1, r5
   38bd8: e1a05101     	lsl	r5, r1, #2
   38bdc: e1a00005     	mov	r0, r5
   38be0: ebff7349     	bl	0x1590c     @ imm = #-0x232dc ; _Znwj
   38be4: e1a02109     	lsl	r2, r9, #2
   38be8: e1a06000     	mov	r6, r0
   38bec: e3a01000     	mov	r1, #0
   38bf0: e0800008     	add	r0, r0, r8
   38bf4: ebff745e     	bl	0x15d74    @ imm = #-0x22e88 ; memset
   38bf8: e5948004     	ldr	r8, [r4, #0x4]
   38bfc: e5942008     	ldr	r2, [r4, #0x8]
   38c00: e0422008     	sub	r2, r2, r8
   38c04: e3520000     	cmp	r2, #0
   38c08: ca000007     	bgt	0x38c2c
   38c0c: e3580000     	cmp	r8, #0
   38c10: 1a000008     	bne	0x38c38
   38c14: e0863107     	add	r3, r6, r7, lsl #2
   38c18: e0865005     	add	r5, r6, r5
   38c1c: e5843008     	str	r3, [r4, #0x8]
   38c20: e584500c     	str	r5, [r4, #0xc]
   38c24: e5846004     	str	r6, [r4, #0x4]
   38c28: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   38c2c: e1a01008     	mov	r1, r8
   38c30: e1a00006     	mov	r0, r6
   38c34: ebff736a     	bl	0x159e4    @ imm = #-0x23258 ; memmove
   38c38: e1a00008     	mov	r0, r8
   38c3c: ebff747f     	bl	0x15e40    @ imm = #-0x22e04 ; _ZdlPv
   38c40: eafffff3     	b	0x38c14
   38c44: e3e0510e     	mvn	r5, #-2147483645
   38c48: eaffffe3     	b	0x38bdc
   38c4c: e3020394     	movw	r0, #0x2394
   38c50: e3400007     	movt	r0, #0x7
   38c54: ebff73d7     	bl	0x15bb8    @ imm = #-0x230a4 ; _ZSt20__throw_length_errorPKc
