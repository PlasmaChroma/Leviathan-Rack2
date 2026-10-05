; lubadh::Channel::PresetLoader::load()
; VA 0x38d60 size 236

   38d60: e92d40f0     	push	{r4, r5, r6, r7, lr}
   38d64: e3a02000     	mov	r2, #0
   38d68: e1a04000     	mov	r4, r0
   38d6c: e24dd024     	sub	sp, sp, #36
   38d70: e3a0301e     	mov	r3, #30
   38d74: e28d1004     	add	r1, sp, #4
   38d78: e28d0008     	add	r0, sp, #8
   38d7c: e28d5010     	add	r5, sp, #16
   38d80: e3a06000     	mov	r6, #0
   38d84: e98d0028     	stmib	sp, {r3, r5}
   38d88: ebff756a     	bl	0x16338    @ imm = #-0x22a58 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   38d8c: e302c3e4     	movw	r12, #0x23e4
   38d90: e340c007     	movt	r12, #0x7
   38d94: e1a0e000     	mov	lr, r0
   38d98: e58d0008     	str	r0, [sp, #0x8]
   38d9c: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   38da0: e58e300c     	str	r3, [lr, #0xc]
   38da4: e59d3004     	ldr	r3, [sp, #0x4]
   38da8: e58d3010     	str	r3, [sp, #0x10]
   38dac: e58e0000     	str	r0, [lr]
   38db0: e58e1004     	str	r1, [lr, #0x4]
   38db4: e58e2008     	str	r2, [lr, #0x8]
   38db8: e8bc0007     	ldm	r12!, {r0, r1, r2}
   38dbc: e58e0010     	str	r0, [lr, #0x10]
   38dc0: e59d3004     	ldr	r3, [sp, #0x4]
   38dc4: e3090fec     	movw	r0, #0x9fec
   38dc8: e3400009     	movt	r0, #0x9
   38dcc: e59d7008     	ldr	r7, [sp, #0x8]
   38dd0: e58e1014     	str	r1, [lr, #0x14]
   38dd4: e28d1008     	add	r1, sp, #8
   38dd8: e58e2018     	str	r2, [lr, #0x18]
   38ddc: e1a02006     	mov	r2, r6
   38de0: e1dcc0b0     	ldrh	r12, [r12]
   38de4: e1cec1bc     	strh	r12, [lr, #28]
   38de8: e58d300c     	str	r3, [sp, #0xc]
   38dec: e7c76003     	strb	r6, [r7, r3]
   38df0: eb00dc4a     	bl	0x6ff20
   38df4: e59d0008     	ldr	r0, [sp, #0x8]
   38df8: e1500005     	cmp	r0, r5
   38dfc: 0a000000     	beq	0x38e04
   38e00: ebff740e     	bl	0x15e40    @ imm = #-0x22fc8 ; _ZdlPv
   38e04: e5942000     	ldr	r2, [r4]
   38e08: e3a05001     	mov	r5, #1
   38e0c: e5941098     	ldr	r1, [r4, #0x98]
   38e10: e28400fc     	add	r0, r4, #252
   38e14: e592e000     	ldr	lr, [r2]
   38e18: e5923020     	ldr	r3, [r2, #0x20]
   38e1c: e592c01c     	ldr	r12, [r2, #0x1c]
   38e20: e2833915     	add	r3, r3, #344064
   38e24: e7c1500e     	strb	r5, [r1, lr]
   38e28: e59420bc     	ldr	r2, [r4, #0xbc]
   38e2c: e59cc000     	ldr	r12, [r12]
   38e30: e5d3eabc     	ldrb	lr, [r3, #0xabc]
   38e34: e59430dc     	ldr	r3, [r4, #0xdc]
   38e38: e7c1e00c     	strb	lr, [r1, r12]
   38e3c: e5823000     	str	r3, [r2]
   38e40: eb00de76     	bl	0x70820
   38e44: e28dd024     	add	sp, sp, #36
   38e48: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
