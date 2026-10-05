; lubadh::Channel::PresetLoader::exit()
; VA 0x38e4c size 260

   38e4c: e92d4030     	push	{r4, r5, lr}
   38e50: e1a04000     	mov	r4, r0
   38e54: e24dd01c     	sub	sp, sp, #28
   38e58: e5943000     	ldr	r3, [r4]
   38e5c: e1a0000d     	mov	r0, sp
   38e60: e28d5008     	add	r5, sp, #8
   38e64: e58d5000     	str	r5, [sp]
   38e68: e9930006     	ldmib	r3, {r1, r2}
   38e6c: e0812002     	add	r2, r1, r2
   38e70: ebfff820     	bl	0x36ef8
   38e74: e59d2004     	ldr	r2, [sp, #0x4]
   38e78: e3e03103     	mvn	r3, #-1073741824
   38e7c: e0433002     	sub	r3, r3, r2
   38e80: e3530019     	cmp	r3, #25
   38e84: 9a000028     	bls	0x38f2c
   38e88: e3021404     	movw	r1, #0x2404
   38e8c: e3401007     	movt	r1, #0x7
   38e90: e3a0201a     	mov	r2, #26
   38e94: e1a0000d     	mov	r0, sp
   38e98: ebff73fa     	bl	0x15e88    @ imm = #-0x23018 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   38e9c: e3090fec     	movw	r0, #0x9fec
   38ea0: e3400009     	movt	r0, #0x9
   38ea4: e1a0100d     	mov	r1, sp
   38ea8: e3a02000     	mov	r2, #0
   38eac: eb00dc1b     	bl	0x6ff20
   38eb0: e59d0000     	ldr	r0, [sp]
   38eb4: e1500005     	cmp	r0, r5
   38eb8: 0a000000     	beq	0x38ec0
   38ebc: ebff73df     	bl	0x15e40    @ imm = #-0x23084 ; _ZdlPv
   38ec0: e5941144     	ldr	r1, [r4, #0x144]
   38ec4: e5943000     	ldr	r3, [r4]
   38ec8: e594c168     	ldr	r12, [r4, #0x168]
   38ecc: e5910000     	ldr	r0, [r1]
   38ed0: e2832a2a     	add	r2, r3, #172032
   38ed4: e3500003     	cmp	r0, #3
   38ed8: 13a00000     	movne	r0, #0
   38edc: e582c4d0     	str	r12, [r2, #0x4d0]
   38ee0: 15810000     	strne	r0, [r1]
   38ee4: 0a000009     	beq	0x38f10
   38ee8: e5d21496     	ldrb	r1, [r2, #0x496]
   38eec: e3a02000     	mov	r2, #0
   38ef0: e5930020     	ldr	r0, [r3, #0x20]
   38ef4: eb00cea6     	bl	0x6c994
   38ef8: e5943000     	ldr	r3, [r4]
   38efc: e3a02001     	mov	r2, #1
   38f00: e5c32285     	strb	r2, [r3, #0x285]
   38f04: e5c32288     	strb	r2, [r3, #0x288]
   38f08: e28dd01c     	add	sp, sp, #28
   38f0c: e8bd8030     	pop	{r4, r5, pc}
   38f10: e3a03000     	mov	r3, #0
   38f14: e28400fc     	add	r0, r4, #252
   38f18: e5813000     	str	r3, [r1]
   38f1c: eb00de3f     	bl	0x70820
   38f20: e5943000     	ldr	r3, [r4]
   38f24: e2832a2a     	add	r2, r3, #172032
   38f28: eaffffee     	b	0x38ee8
   38f2c: e3010b08     	movw	r0, #0x1b08
   38f30: e3400007     	movt	r0, #0x7
   38f34: ebff731f     	bl	0x15bb8    @ imm = #-0x23384 ; _ZSt20__throw_length_errorPKc
   38f38: e59d0000     	ldr	r0, [sp]
   38f3c: e1500005     	cmp	r0, r5
   38f40: 0a000000     	beq	0x38f48
   38f44: ebff73bd     	bl	0x15e40    @ imm = #-0x2310c ; _ZdlPv
   38f48: ebff7404     	bl	0x15f60    @ imm = #-0x22ff0 ; __cxa_end_cleanup
   38f4c: eafffff9     	b	0x38f38
