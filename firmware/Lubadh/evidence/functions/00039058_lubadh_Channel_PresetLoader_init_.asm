; lubadh::Channel::PresetLoader::init()
; VA 0x39058 size 364

   39058: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   3905c: e1a04000     	mov	r4, r0
   39060: e24dd020     	sub	sp, sp, #32
   39064: ebffffbc     	bl	0x38f5c
   39068: e2503000     	subs	r3, r0, #0
   3906c: 1a000031     	bne	0x39138
   39070: e302244c     	movw	r2, #0x244c
   39074: e3402007     	movt	r2, #0x7
   39078: e28d5010     	add	r5, sp, #16
   3907c: e58d5008     	str	r5, [sp, #0x8]
   39080: e1a0c005     	mov	r12, r5
   39084: e3a0e00b     	mov	lr, #11
   39088: e8920007     	ldm	r2, {r0, r1, r2}
   3908c: e8ac0003     	stm	r12!, {r0, r1}
   39090: e28d1008     	add	r1, sp, #8
   39094: e3090fec     	movw	r0, #0x9fec
   39098: e3400009     	movt	r0, #0x9
   3909c: e0cc20b2     	strh	r2, [r12], #2
   390a0: e1a02822     	lsr	r2, r2, #16
   390a4: e5cc2000     	strb	r2, [r12]
   390a8: e1a02003     	mov	r2, r3
   390ac: e5cd301b     	strb	r3, [sp, #0x1b]
   390b0: e58de00c     	str	lr, [sp, #0xc]
   390b4: eb00db99     	bl	0x6ff20
   390b8: e59d0008     	ldr	r0, [sp, #0x8]
   390bc: e1500005     	cmp	r0, r5
   390c0: 0a000000     	beq	0x390c8
   390c4: ebff735d     	bl	0x15e40    @ imm = #-0x2328c ; _ZdlPv
   390c8: e5943000     	ldr	r3, [r4]
   390cc: e3a00000     	mov	r0, #0
   390d0: e5947008     	ldr	r7, [r4, #0x8]
   390d4: e3a05001     	mov	r5, #1
   390d8: e5942098     	ldr	r2, [r4, #0x98]
   390dc: e593c0e8     	ldr	r12, [r3, #0xe8]
   390e0: e5931020     	ldr	r1, [r3, #0x20]
   390e4: e593e000     	ldr	lr, [r3]
   390e8: e59c80b0     	ldr	r8, [r12, #0xb0]
   390ec: e281ca2a     	add	r12, r1, #172032
   390f0: e5916048     	ldr	r6, [r1, #0x48]
   390f4: e2831a2a     	add	r1, r3, #172032
   390f8: e5983000     	ldr	r3, [r8]
   390fc: e59cc580     	ldr	r12, [r12, #0x580]
   39100: e16f3f13     	clz	r3, r3
   39104: e1a032a3     	lsr	r3, r3, #5
   39108: e5873000     	str	r3, [r7]
   3910c: e7c20006     	strb	r0, [r2, r6]
   39110: e3a03005     	mov	r3, #5
   39114: e7c2000c     	strb	r0, [r2, r12]
   39118: e28400e0     	add	r0, r4, #224
   3911c: e591c4d0     	ldr	r12, [r1, #0x4d0]
   39120: e7c2500e     	strb	r5, [r2, lr]
   39124: e584c168     	str	r12, [r4, #0x168]
   39128: e58134d0     	str	r3, [r1, #0x4d0]
   3912c: eb00ddbb     	bl	0x70820
   39130: e28dd020     	add	sp, sp, #32
   39134: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   39138: e3a02000     	mov	r2, #0
   3913c: e28d1004     	add	r1, sp, #4
   39140: e28d0008     	add	r0, sp, #8
   39144: e3a03012     	mov	r3, #18
   39148: e28d4010     	add	r4, sp, #16
   3914c: e98d0018     	stmib	sp, {r3, r4}
   39150: ebff7478     	bl	0x16338    @ imm = #-0x22e20 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   39154: e302c438     	movw	r12, #0x2438
   39158: e340c007     	movt	r12, #0x7
   3915c: e58d0008     	str	r0, [sp, #0x8]
   39160: e1a0e000     	mov	lr, r0
   39164: e59d6004     	ldr	r6, [sp, #0x4]
   39168: e58d6010     	str	r6, [sp, #0x10]
   3916c: e3a05000     	mov	r5, #0
   39170: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   39174: e58e300c     	str	r3, [lr, #0xc]
   39178: e58e0000     	str	r0, [lr]
   3917c: e3090fec     	movw	r0, #0x9fec
   39180: e3400009     	movt	r0, #0x9
   39184: e58e1004     	str	r1, [lr, #0x4]
   39188: e58e2008     	str	r2, [lr, #0x8]
   3918c: e28d1008     	add	r1, sp, #8
   39190: e1a02005     	mov	r2, r5
   39194: e1dc30b0     	ldrh	r3, [r12]
   39198: e1ce31b0     	strh	r3, [lr, #16]
   3919c: e99d1008     	ldmib	sp, {r3, r12}
   391a0: e58d300c     	str	r3, [sp, #0xc]
   391a4: e7cc5003     	strb	r5, [r12, r3]
   391a8: eb00db5c     	bl	0x6ff20
   391ac: e59d0008     	ldr	r0, [sp, #0x8]
   391b0: e1500004     	cmp	r0, r4
   391b4: 0affffdd     	beq	0x39130
   391b8: ebff7320     	bl	0x15e40    @ imm = #-0x23380 ; _ZdlPv
   391bc: e28dd020     	add	sp, sp, #32
   391c0: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
