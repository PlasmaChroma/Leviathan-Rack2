; lubadh::Channel::PresetLoader::active() const
; VA 0x38f5c size 252

   38f5c: e92d4070     	push	{r4, r5, r6, lr}
   38f60: e1a04000     	mov	r4, r0
   38f64: e3a02010     	mov	r2, #16
   38f68: e24dd038     	sub	sp, sp, #56
   38f6c: e3003d28     	movw	r3, #0xd28
   38f70: e3403007     	movt	r3, #0x7
   38f74: e594c144     	ldr	r12, [r4, #0x144]
   38f78: e28d0008     	add	r0, sp, #8
   38f7c: e3061218     	movw	r1, #0x6218
   38f80: e3401001     	movt	r1, #0x1
   38f84: e28d5028     	add	r5, sp, #40
   38f88: e59cc000     	ldr	r12, [r12]
   38f8c: e58dc000     	str	r12, [sp]
   38f90: ebfff804     	bl	0x36fa8
   38f94: e3a02000     	mov	r2, #0
   38f98: e1a01002     	mov	r1, r2
   38f9c: e3a03016     	mov	r3, #22
   38fa0: e28d0008     	add	r0, sp, #8
   38fa4: e58d3000     	str	r3, [sp]
   38fa8: e3023420     	movw	r3, #0x2420
   38fac: e3403007     	movt	r3, #0x7
   38fb0: ebff72d0     	bl	0x15af8    @ imm = #-0x234c0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   38fb4: e1a0e000     	mov	lr, r0
   38fb8: e58d5020     	str	r5, [sp, #0x20]
   38fbc: e1a0c000     	mov	r12, r0
   38fc0: e49e3008     	ldr	r3, [lr], #8
   38fc4: e153000e     	cmp	r3, lr
   38fc8: 158d3020     	strne	r3, [sp, #0x20]
   38fcc: 01a06005     	moveq	r6, r5
   38fd0: 059e0000     	ldreq	r0, [lr]
   38fd4: 059e1004     	ldreq	r1, [lr, #0x4]
   38fd8: 059e2008     	ldreq	r2, [lr, #0x8]
   38fdc: 059e300c     	ldreq	r3, [lr, #0xc]
   38fe0: 159c2008     	ldrne	r2, [r12, #0x8]
   38fe4: 08a6000f     	stmeq	r6!, {r0, r1, r2, r3}
   38fe8: e28d1020     	add	r1, sp, #32
   38fec: e3090fec     	movw	r0, #0x9fec
   38ff0: e3400009     	movt	r0, #0x9
   38ff4: 158d2028     	strne	r2, [sp, #0x28]
   38ff8: e3a02000     	mov	r2, #0
   38ffc: e5cc2008     	strb	r2, [r12, #0x8]
   39000: e59c3004     	ldr	r3, [r12, #0x4]
   39004: e58d3024     	str	r3, [sp, #0x24]
   39008: e58ce000     	str	lr, [r12]
   3900c: e58c2004     	str	r2, [r12, #0x4]
   39010: eb00dbc2     	bl	0x6ff20
   39014: e59d0020     	ldr	r0, [sp, #0x20]
   39018: e1500005     	cmp	r0, r5
   3901c: 0a000000     	beq	0x39024
   39020: ebff7386     	bl	0x15e40    @ imm = #-0x231e8 ; _ZdlPv
   39024: e59d0008     	ldr	r0, [sp, #0x8]
   39028: e28d3010     	add	r3, sp, #16
   3902c: e1500003     	cmp	r0, r3
   39030: 0a000000     	beq	0x39038
   39034: ebff7381     	bl	0x15e40    @ imm = #-0x231fc ; _ZdlPv
   39038: e5943144     	ldr	r3, [r4, #0x144]
   3903c: e5930000     	ldr	r0, [r3]
   39040: e3500000     	cmp	r0, #0
   39044: 0a000001     	beq	0x39050
   39048: e2500006     	subs	r0, r0, #6
   3904c: 13a00001     	movne	r0, #1
   39050: e28dd038     	add	sp, sp, #56
   39054: e8bd8070     	pop	{r4, r5, r6, pc}
