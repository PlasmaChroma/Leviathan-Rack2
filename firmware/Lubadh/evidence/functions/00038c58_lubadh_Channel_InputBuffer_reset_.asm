; lubadh::Channel::InputBuffer::reset()
; VA 0x38c58 size 264

   38c58: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   38c5c: e1a05000     	mov	r5, r0
   38c60: e5906004     	ldr	r6, [r0, #0x4]
   38c64: e590300c     	ldr	r3, [r0, #0xc]
   38c68: e5904000     	ldr	r4, [r0]
   38c6c: e0433006     	sub	r3, r3, r6
   38c70: e2844004     	add	r4, r4, #4
   38c74: e1540143     	cmp	r4, r3, asr #2
   38c78: 8a00001f     	bhi	0x38cfc
   38c7c: e5907008     	ldr	r7, [r0, #0x8]
   38c80: e0472006     	sub	r2, r7, r6
   38c84: e1540142     	cmp	r4, r2, asr #2
   38c88: e1a08142     	asr	r8, r2, #2
   38c8c: 8a00000b     	bhi	0x38cc0
   38c90: e3540000     	cmp	r4, #0
   38c94: 0a000004     	beq	0x38cac
   38c98: e1a02104     	lsl	r2, r4, #2
   38c9c: e1a00006     	mov	r0, r6
   38ca0: e3a01000     	mov	r1, #0
   38ca4: e0866002     	add	r6, r6, r2
   38ca8: ebff7431     	bl	0x15d74    @ imm = #-0x22f3c ; memset
   38cac: e1570006     	cmp	r7, r6
   38cb0: 15856008     	strne	r6, [r5, #0x8]
   38cb4: e3a03000     	mov	r3, #0
   38cb8: e5853010     	str	r3, [r5, #0x10]
   38cbc: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   38cc0: e1560007     	cmp	r6, r7
   38cc4: 0a000003     	beq	0x38cd8
   38cc8: e0472006     	sub	r2, r7, r6
   38ccc: e1a00006     	mov	r0, r6
   38cd0: e3a01000     	mov	r1, #0
   38cd4: ebff7426     	bl	0x15d74    @ imm = #-0x22f68 ; memset
   38cd8: e0444008     	sub	r4, r4, r8
   38cdc: e3a01000     	mov	r1, #0
   38ce0: e1a00007     	mov	r0, r7
   38ce4: e1a04104     	lsl	r4, r4, #2
   38ce8: e1a02004     	mov	r2, r4
   38cec: e0874004     	add	r4, r7, r4
   38cf0: ebff741f     	bl	0x15d74    @ imm = #-0x22f84 ; memset
   38cf4: e5854008     	str	r4, [r5, #0x8]
   38cf8: eaffffed     	b	0x38cb4
   38cfc: e374021e     	cmn	r4, #-536870911
   38d00: 8a000013     	bhi	0x38d54
   38d04: e3540000     	cmp	r4, #0
   38d08: e1a07104     	lsl	r7, r4, #2
   38d0c: 01a07004     	moveq	r7, r4
   38d10: 0a000007     	beq	0x38d34
   38d14: e1a00007     	mov	r0, r7
   38d18: ebff72fb     	bl	0x1590c     @ imm = #-0x23414 ; _Znwj
   38d1c: e1a02007     	mov	r2, r7
   38d20: e3a01000     	mov	r1, #0
   38d24: e1a04000     	mov	r4, r0
   38d28: e0807007     	add	r7, r0, r7
   38d2c: ebff7410     	bl	0x15d74    @ imm = #-0x22fc0 ; memset
   38d30: e5956004     	ldr	r6, [r5, #0x4]
   38d34: e3560000     	cmp	r6, #0
   38d38: e5854004     	str	r4, [r5, #0x4]
   38d3c: e5857008     	str	r7, [r5, #0x8]
   38d40: e585700c     	str	r7, [r5, #0xc]
   38d44: 0affffda     	beq	0x38cb4
   38d48: e1a00006     	mov	r0, r6
   38d4c: ebff743b     	bl	0x15e40    @ imm = #-0x22f14 ; _ZdlPv
   38d50: eaffffd7     	b	0x38cb4
   38d54: e30203b0     	movw	r0, #0x23b0
   38d58: e3400007     	movt	r0, #0x7
   38d5c: ebff7395     	bl	0x15bb8    @ imm = #-0x231ac ; _ZSt20__throw_length_errorPKc
