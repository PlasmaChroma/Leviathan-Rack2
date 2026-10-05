; lubadh::Channel::FileSaver::process()
; VA 0x39d4c size 716

   39d4c: e92d4070     	push	{r4, r5, r6, lr}
   39d50: e2802004     	add	r2, r0, #4
   39d54: e2803068     	add	r3, r0, #104
   39d58: e24dd038     	sub	sp, sp, #56
   39d5c: e1a04000     	mov	r4, r0
   39d60: e58d2000     	str	r2, [sp]
   39d64: e5902048     	ldr	r2, [r0, #0x48]
   39d68: e58d3004     	str	r3, [sp, #0x4]
   39d6c: e5923000     	ldr	r3, [r2]
   39d70: e3530002     	cmp	r3, #2
   39d74: 0a000052     	beq	0x39ec4
   39d78: e3530003     	cmp	r3, #3
   39d7c: 1a000004     	bne	0x39d94
   39d80: e5901000     	ldr	r1, [r0]
   39d84: e3a03000     	mov	r3, #0
   39d88: e5813290     	str	r3, [r1, #0x290]
   39d8c: e5c13240     	strb	r3, [r1, #0x240]
   39d90: e5823000     	str	r3, [r2]
   39d94: e59420ac     	ldr	r2, [r4, #0xac]
   39d98: e5923000     	ldr	r3, [r2]
   39d9c: e3530002     	cmp	r3, #2
   39da0: 0a00000a     	beq	0x39dd0
   39da4: e3530003     	cmp	r3, #3
   39da8: 0a000001     	beq	0x39db4
   39dac: e28dd038     	add	sp, sp, #56
   39db0: e8bd8070     	pop	{r4, r5, r6, pc}
   39db4: e5941000     	ldr	r1, [r4]
   39db8: e3a03000     	mov	r3, #0
   39dbc: e5813290     	str	r3, [r1, #0x290]
   39dc0: e5c13240     	strb	r3, [r1, #0x240]
   39dc4: e5823000     	str	r3, [r2]
   39dc8: e28dd038     	add	sp, sp, #56
   39dcc: e8bd8070     	pop	{r4, r5, r6, pc}
   39dd0: e5941000     	ldr	r1, [r4]
   39dd4: e28d5008     	add	r5, sp, #8
   39dd8: e1a00005     	mov	r0, r5
   39ddc: e30225a8     	movw	r2, #0x25a8
   39de0: e3402007     	movt	r2, #0x7
   39de4: e2811004     	add	r1, r1, #4
   39de8: ebffd376     	bl	0x2ebc8
   39dec: e5943000     	ldr	r3, [r4]
   39df0: e5933020     	ldr	r3, [r3, #0x20]
   39df4: e2833915     	add	r3, r3, #344064
   39df8: e5d32abc     	ldrb	r2, [r3, #0xabc]
   39dfc: e3520000     	cmp	r2, #0
   39e00: 1a00006f     	bne	0x39fc4
   39e04: e3051064     	movw	r1, #0x5064
   39e08: e3401007     	movt	r1, #0x7
   39e0c: e1a00005     	mov	r0, r5
   39e10: e28d5028     	add	r5, sp, #40
   39e14: ebff701b     	bl	0x15e88    @ imm = #-0x23f94 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   39e18: e1a0e000     	mov	lr, r0
   39e1c: e58d5020     	str	r5, [sp, #0x20]
   39e20: e1a0c000     	mov	r12, r0
   39e24: e49e3008     	ldr	r3, [lr], #8
   39e28: e153000e     	cmp	r3, lr
   39e2c: 158d3020     	strne	r3, [sp, #0x20]
   39e30: 01a06005     	moveq	r6, r5
   39e34: 059e0000     	ldreq	r0, [lr]
   39e38: 059e1004     	ldreq	r1, [lr, #0x4]
   39e3c: 059e2008     	ldreq	r2, [lr, #0x8]
   39e40: 059e300c     	ldreq	r3, [lr, #0xc]
   39e44: 159c2008     	ldrne	r2, [r12, #0x8]
   39e48: 08a6000f     	stmeq	r6!, {r0, r1, r2, r3}
   39e4c: e3a03000     	mov	r3, #0
   39e50: e28d1020     	add	r1, sp, #32
   39e54: 158d2028     	strne	r2, [sp, #0x28]
   39e58: e3090fec     	movw	r0, #0x9fec
   39e5c: e3400009     	movt	r0, #0x9
   39e60: e5cc3008     	strb	r3, [r12, #0x8]
   39e64: e59c2004     	ldr	r2, [r12, #0x4]
   39e68: e58d2024     	str	r2, [sp, #0x24]
   39e6c: e3a02002     	mov	r2, #2
   39e70: e58ce000     	str	lr, [r12]
   39e74: e58c3004     	str	r3, [r12, #0x4]
   39e78: eb00d828     	bl	0x6ff20
   39e7c: e59d0020     	ldr	r0, [sp, #0x20]
   39e80: e1500005     	cmp	r0, r5
   39e84: 0a000000     	beq	0x39e8c
   39e88: ebff6fec     	bl	0x15e40    @ imm = #-0x24050 ; _ZdlPv
   39e8c: e59d0008     	ldr	r0, [sp, #0x8]
   39e90: e28d3010     	add	r3, sp, #16
   39e94: e1500003     	cmp	r0, r3
   39e98: 0a000000     	beq	0x39ea0
   39e9c: ebff6fe7     	bl	0x15e40    @ imm = #-0x24064 ; _ZdlPv
   39ea0: e5942000     	ldr	r2, [r4]
   39ea4: e3a03000     	mov	r3, #0
   39ea8: e59410ac     	ldr	r1, [r4, #0xac]
   39eac: eddf0b57     	vldr	d16, [pc, #348]         @ 0x3a010 ; float 6.36598738204e-313
   39eb0: e5c23240     	strb	r3, [r2, #0x240]
   39eb4: edc20ba4     	vstr	d16, [r2, #656]
   39eb8: e5813000     	str	r3, [r1]
   39ebc: e28dd038     	add	sp, sp, #56
   39ec0: e8bd8070     	pop	{r4, r5, r6, pc}
   39ec4: e5901000     	ldr	r1, [r0]
   39ec8: e28d5008     	add	r5, sp, #8
   39ecc: e1a00005     	mov	r0, r5
   39ed0: e30225a8     	movw	r2, #0x25a8
   39ed4: e3402007     	movt	r2, #0x7
   39ed8: e2811004     	add	r1, r1, #4
   39edc: ebffd339     	bl	0x2ebc8
   39ee0: e5943000     	ldr	r3, [r4]
   39ee4: e5933020     	ldr	r3, [r3, #0x20]
   39ee8: e2833915     	add	r3, r3, #344064
   39eec: e5d32abc     	ldrb	r2, [r3, #0xabc]
   39ef0: e3520000     	cmp	r2, #0
   39ef4: 1a000029     	bne	0x39fa0
   39ef8: e3051064     	movw	r1, #0x5064
   39efc: e3401007     	movt	r1, #0x7
   39f00: e1a00005     	mov	r0, r5
   39f04: e28d5028     	add	r5, sp, #40
   39f08: ebff6fde     	bl	0x15e88    @ imm = #-0x24088 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   39f0c: e1a0e000     	mov	lr, r0
   39f10: e58d5020     	str	r5, [sp, #0x20]
   39f14: e1a0c000     	mov	r12, r0
   39f18: e49e3008     	ldr	r3, [lr], #8
   39f1c: e15e0003     	cmp	lr, r3
   39f20: 0a000032     	beq	0x39ff0
   39f24: e5902008     	ldr	r2, [r0, #0x8]
   39f28: e58d3020     	str	r3, [sp, #0x20]
   39f2c: e58d2028     	str	r2, [sp, #0x28]
   39f30: e59c2004     	ldr	r2, [r12, #0x4]
   39f34: e3a03000     	mov	r3, #0
   39f38: e58d2024     	str	r2, [sp, #0x24]
   39f3c: e28d1020     	add	r1, sp, #32
   39f40: e3a02002     	mov	r2, #2
   39f44: e3090fec     	movw	r0, #0x9fec
   39f48: e3400009     	movt	r0, #0x9
   39f4c: e58ce000     	str	lr, [r12]
   39f50: e58c3004     	str	r3, [r12, #0x4]
   39f54: e5cc3008     	strb	r3, [r12, #0x8]
   39f58: eb00d7f0     	bl	0x6ff20
   39f5c: e59d0020     	ldr	r0, [sp, #0x20]
   39f60: e1500005     	cmp	r0, r5
   39f64: 0a000000     	beq	0x39f6c
   39f68: ebff6fb4     	bl	0x15e40    @ imm = #-0x24130 ; _ZdlPv
   39f6c: e59d0008     	ldr	r0, [sp, #0x8]
   39f70: e28d3010     	add	r3, sp, #16
   39f74: e1500003     	cmp	r0, r3
   39f78: 0a000000     	beq	0x39f80
   39f7c: ebff6faf     	bl	0x15e40    @ imm = #-0x24144 ; _ZdlPv
   39f80: e5942000     	ldr	r2, [r4]
   39f84: e3a03000     	mov	r3, #0
   39f88: e5941048     	ldr	r1, [r4, #0x48]
   39f8c: eddf0b1f     	vldr	d16, [pc, #124]         @ 0x3a010 ; float 6.36598738204e-313
   39f90: e5c23240     	strb	r3, [r2, #0x240]
   39f94: edc20ba4     	vstr	d16, [r2, #656]
   39f98: e5813000     	str	r3, [r1]
   39f9c: eaffff7c     	b	0x39d94
   39fa0: e59d200c     	ldr	r2, [sp, #0xc]
   39fa4: e3e03103     	mvn	r3, #-1073741824
   39fa8: e0433002     	sub	r3, r3, r2
   39fac: e3530008     	cmp	r3, #8
   39fb0: 9a00000b     	bls	0x39fe4
   39fb4: e302159c     	movw	r1, #0x259c
   39fb8: e3401007     	movt	r1, #0x7
   39fbc: e3a02009     	mov	r2, #9
   39fc0: eaffffce     	b	0x39f00
   39fc4: e59d200c     	ldr	r2, [sp, #0xc]
   39fc8: e3e03103     	mvn	r3, #-1073741824
   39fcc: e0433002     	sub	r3, r3, r2
   39fd0: e3530008     	cmp	r3, #8
   39fd4: 83a02009     	movhi	r2, #9
   39fd8: 8302159c     	movwhi	r1, #0x259c
   39fdc: 83401007     	movthi	r1, #0x7
   39fe0: 8affff89     	bhi	0x39e0c
   39fe4: e3010b08     	movw	r0, #0x1b08
   39fe8: e3400007     	movt	r0, #0x7
   39fec: ebff6ef1     	bl	0x15bb8    @ imm = #-0x2443c ; _ZSt20__throw_length_errorPKc
   39ff0: e1a06005     	mov	r6, r5
   39ff4: e59e0000     	ldr	r0, [lr]
   39ff8: e59e1004     	ldr	r1, [lr, #0x4]
   39ffc: e59e2008     	ldr	r2, [lr, #0x8]
   3a000: e59e300c     	ldr	r3, [lr, #0xc]
   3a004: e8a6000f     	stm	r6!, {r0, r1, r2, r3}
   3a008: eaffffc8     	b	0x39f30
   3a00c: e320f000     	nop
   3a010: b9 00 00 00  	.word	0x000000b9
   3a014: 1e 00 00 00  	.word	0x0000001e
