; lubadh::AudioEngine::AudioEngine(lubadh::Application&, float)
; VA 0x35dd0 size 1136

   35dd0: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   35dd4: f2c02010     	vmov.i32	d18, #0x0
   35dd8: e1a04000     	mov	r4, r0
   35ddc: f2c00050     	vmov.i32	q8, #0x0
   35de0: e280503c     	add	r5, r0, #60
   35de4: e1a07000     	mov	r7, r0
   35de8: e280e04c     	add	lr, r0, #76
   35dec: e280c068     	add	r12, r0, #104
   35df0: e24dd050     	sub	sp, sp, #80
   35df4: e59f0440     	ldr	r0, [pc, #0x440]        @ 0x3623c
   35df8: e28d6038     	add	r6, sp, #56
   35dfc: e4870004     	str	r0, [r7], #4
   35e00: e3a02000     	mov	r2, #0
   35e04: e1a03001     	mov	r3, r1
   35e08: e5842010     	str	r2, [r4, #0x10]
   35e0c: e5c42014     	strb	r2, [r4, #0x14]
   35e10: e2841014     	add	r1, r4, #20
   35e14: e5842028     	str	r2, [r4, #0x28]
   35e18: e1a00006     	mov	r0, r6
   35e1c: e584100c     	str	r1, [r4, #0xc]
   35e20: e284102c     	add	r1, r4, #44
   35e24: e5c4202c     	strb	r2, [r4, #0x2c]
   35e28: e5841024     	str	r1, [r4, #0x24]
   35e2c: e28d1020     	add	r1, sp, #32
   35e30: f4450a8f     	vst1.32	{d16, d17}, [r5]
   35e34: e28d5040     	add	r5, sp, #64
   35e38: f44e0a8f     	vst1.32	{d16, d17}, [lr]
   35e3c: e584205c     	str	r2, [r4, #0x5c]
   35e40: e5842060     	str	r2, [r4, #0x60]
   35e44: f44c0a8f     	vst1.32	{d16, d17}, [r12]
   35e48: e5843084     	str	r3, [r4, #0x84]
   35e4c: e3a0302b     	mov	r3, #43
   35e50: e5842078     	str	r2, [r4, #0x78]
   35e54: e584207c     	str	r2, [r4, #0x7c]
   35e58: e5c42080     	strb	r2, [r4, #0x80]
   35e5c: e5c42088     	strb	r2, [r4, #0x88]
   35e60: ed840a24     	vstr	s0, [r4, #144]
   35e64: edc42b26     	vstr	d18, [r4, #152]
   35e68: e58d5038     	str	r5, [sp, #0x38]
   35e6c: e58d3020     	str	r3, [sp, #0x20]
   35e70: ebff8130     	bl	0x16338    @ imm = #-0x1fb40 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   35e74: e302c0b0     	movw	r12, #0x20b0
   35e78: e340c007     	movt	r12, #0x7
   35e7c: e59d3020     	ldr	r3, [sp, #0x20]
   35e80: e1a0e000     	mov	lr, r0
   35e84: e28c9020     	add	r9, r12, #32
   35e88: e58d0038     	str	r0, [sp, #0x38]
   35e8c: e58d3040     	str	r3, [sp, #0x40]
   35e90: e1a0800c     	mov	r8, r12
   35e94: e28ee010     	add	lr, lr, #16
   35e98: e28cc010     	add	r12, r12, #16
   35e9c: e8b8000f     	ldm	r8!, {r0, r1, r2, r3}
   35ea0: e50e0010     	str	r0, [lr, #-0x10]
   35ea4: e50e100c     	str	r1, [lr, #-0xc]
   35ea8: e50e2008     	str	r2, [lr, #-0x8]
   35eac: e50e3004     	str	r3, [lr, #-0x4]
   35eb0: e1580009     	cmp	r8, r9
   35eb4: 1afffff5     	bne	0x35e90
   35eb8: e8bc0003     	ldm	r12!, {r0, r1}
   35ebc: e58e1004     	str	r1, [lr, #0x4]
   35ec0: e58e0000     	str	r0, [lr]
   35ec4: e3a02000     	mov	r2, #0
   35ec8: e3090fec     	movw	r0, #0x9fec
   35ecc: e3400009     	movt	r0, #0x9
   35ed0: e1dc10b0     	ldrh	r1, [r12]
   35ed4: e5dc3002     	ldrb	r3, [r12, #0x2]
   35ed8: e1ce10b8     	strh	r1, [lr, #8]
   35edc: e1a01006     	mov	r1, r6
   35ee0: e5ce300a     	strb	r3, [lr, #0xa]
   35ee4: e59d3020     	ldr	r3, [sp, #0x20]
   35ee8: e59dc038     	ldr	r12, [sp, #0x38]
   35eec: e58d303c     	str	r3, [sp, #0x3c]
   35ef0: e7cc2003     	strb	r2, [r12, r3]
   35ef4: eb00e809     	bl	0x6ff20
   35ef8: e59d0038     	ldr	r0, [sp, #0x38]
   35efc: e1500005     	cmp	r0, r5
   35f00: 0a000000     	beq	0x35f08
   35f04: ebff7fcd     	bl	0x15e40    @ imm = #-0x200cc ; _ZdlPv
   35f08: e3a02000     	mov	r2, #0
   35f0c: e1a01006     	mov	r1, r6
   35f10: e28d0008     	add	r0, sp, #8
   35f14: e28d8010     	add	r8, sp, #16
   35f18: e3a03025     	mov	r3, #37
   35f1c: e58d8008     	str	r8, [sp, #0x8]
   35f20: e58d3038     	str	r3, [sp, #0x38]
   35f24: ebff8103     	bl	0x16338    @ imm = #-0x1fbf4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   35f28: e302e0dc     	movw	lr, #0x20dc
   35f2c: e340e007     	movt	lr, #0x7
   35f30: e59d3038     	ldr	r3, [sp, #0x38]
   35f34: e1a0c000     	mov	r12, r0
   35f38: e28ea020     	add	r10, lr, #32
   35f3c: e58d0008     	str	r0, [sp, #0x8]
   35f40: e58d3010     	str	r3, [sp, #0x10]
   35f44: e1a0900e     	mov	r9, lr
   35f48: e28cc010     	add	r12, r12, #16
   35f4c: e28ee010     	add	lr, lr, #16
   35f50: e8b9000f     	ldm	r9!, {r0, r1, r2, r3}
   35f54: e50c0010     	str	r0, [r12, #-0x10]
   35f58: e50c100c     	str	r1, [r12, #-0xc]
   35f5c: e50c2008     	str	r2, [r12, #-0x8]
   35f60: e50c3004     	str	r3, [r12, #-0x4]
   35f64: e159000a     	cmp	r9, r10
   35f68: 1afffff5     	bne	0x35f44
   35f6c: e59e0000     	ldr	r0, [lr]
   35f70: e3a01000     	mov	r1, #0
   35f74: e5de3004     	ldrb	r3, [lr, #0x4]
   35f78: e58c0000     	str	r0, [r12]
   35f7c: e5cc3004     	strb	r3, [r12, #0x4]
   35f80: e59d3038     	ldr	r3, [sp, #0x38]
   35f84: e59d2008     	ldr	r2, [sp, #0x8]
   35f88: e58d300c     	str	r3, [sp, #0xc]
   35f8c: e7c21003     	strb	r1, [r2, r3]
   35f90: e59d0008     	ldr	r0, [sp, #0x8]
   35f94: ebff8129     	bl	0x16440    @ imm = #-0x1fb5c ; system
   35f98: e3020104     	movw	r0, #0x2104
   35f9c: e3400007     	movt	r0, #0x7
   35fa0: ebff8126     	bl	0x16440    @ imm = #-0x1fb68 ; system
   35fa4: e3a02000     	mov	r2, #0
   35fa8: e28d1020     	add	r1, sp, #32
   35fac: e1a00006     	mov	r0, r6
   35fb0: e3a03013     	mov	r3, #19
   35fb4: e58d5038     	str	r5, [sp, #0x38]
   35fb8: e58d3020     	str	r3, [sp, #0x20]
   35fbc: ebff80dd     	bl	0x16338    @ imm = #-0x1fc8c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   35fc0: e302c120     	movw	r12, #0x2120
   35fc4: e340c007     	movt	r12, #0x7
   35fc8: e58d0038     	str	r0, [sp, #0x38]
   35fcc: e1a0e000     	mov	lr, r0
   35fd0: e59da020     	ldr	r10, [sp, #0x20]
   35fd4: e3a09000     	mov	r9, #0
   35fd8: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   35fdc: e58da040     	str	r10, [sp, #0x40]
   35fe0: e58e300c     	str	r3, [lr, #0xc]
   35fe4: e58e0000     	str	r0, [lr]
   35fe8: e3090fec     	movw	r0, #0x9fec
   35fec: e3400009     	movt	r0, #0x9
   35ff0: e58e1004     	str	r1, [lr, #0x4]
   35ff4: e58e2008     	str	r2, [lr, #0x8]
   35ff8: e1a01006     	mov	r1, r6
   35ffc: e5dc3002     	ldrb	r3, [r12, #0x2]
   36000: e1a02009     	mov	r2, r9
   36004: e1dca0b0     	ldrh	r10, [r12]
   36008: e1cea1b0     	strh	r10, [lr, #16]
   3600c: e5ce3012     	strb	r3, [lr, #0x12]
   36010: e59d3020     	ldr	r3, [sp, #0x20]
   36014: e59dc038     	ldr	r12, [sp, #0x38]
   36018: e58d303c     	str	r3, [sp, #0x3c]
   3601c: e7cc9003     	strb	r9, [r12, r3]
   36020: eb00e7be     	bl	0x6ff20
   36024: e59d0038     	ldr	r0, [sp, #0x38]
   36028: e1500005     	cmp	r0, r5
   3602c: 0a000000     	beq	0x36034
   36030: ebff7f82     	bl	0x15e40    @ imm = #-0x201f8 ; _ZdlPv
   36034: e3a02000     	mov	r2, #0
   36038: e28d1020     	add	r1, sp, #32
   3603c: e1a00006     	mov	r0, r6
   36040: e3a03013     	mov	r3, #19
   36044: e58d5038     	str	r5, [sp, #0x38]
   36048: e58d3020     	str	r3, [sp, #0x20]
   3604c: ebff80b9     	bl	0x16338    @ imm = #-0x1fd1c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36050: e302c134     	movw	r12, #0x2134
   36054: e340c007     	movt	r12, #0x7
   36058: e58d0038     	str	r0, [sp, #0x38]
   3605c: e1a0e000     	mov	lr, r0
   36060: e59da020     	ldr	r10, [sp, #0x20]
   36064: e3a09000     	mov	r9, #0
   36068: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3606c: e58da040     	str	r10, [sp, #0x40]
   36070: e58e300c     	str	r3, [lr, #0xc]
   36074: e58e0000     	str	r0, [lr]
   36078: e3090fec     	movw	r0, #0x9fec
   3607c: e3400009     	movt	r0, #0x9
   36080: e58e1004     	str	r1, [lr, #0x4]
   36084: e58e2008     	str	r2, [lr, #0x8]
   36088: e1a01006     	mov	r1, r6
   3608c: e5dc3002     	ldrb	r3, [r12, #0x2]
   36090: e1a02009     	mov	r2, r9
   36094: e1dca0b0     	ldrh	r10, [r12]
   36098: e1cea1b0     	strh	r10, [lr, #16]
   3609c: e5ce3012     	strb	r3, [lr, #0x12]
   360a0: e59d3020     	ldr	r3, [sp, #0x20]
   360a4: e59dc038     	ldr	r12, [sp, #0x38]
   360a8: e58d303c     	str	r3, [sp, #0x3c]
   360ac: e7cc9003     	strb	r9, [r12, r3]
   360b0: eb00e79a     	bl	0x6ff20
   360b4: e59d0038     	ldr	r0, [sp, #0x38]
   360b8: e1500005     	cmp	r0, r5
   360bc: 0a000000     	beq	0x360c4
   360c0: ebff7f5e     	bl	0x15e40    @ imm = #-0x20288 ; _ZdlPv
   360c4: e3021148     	movw	r1, #0x2148
   360c8: e3401007     	movt	r1, #0x7
   360cc: e3a02002     	mov	r2, #2
   360d0: e28d3020     	add	r3, sp, #32
   360d4: e58d6000     	str	r6, [sp]
   360d8: e3a0c006     	mov	r12, #6
   360dc: e8910003     	ldm	r1, {r0, r1}
   360e0: e58d0028     	str	r0, [sp, #0x28]
   360e4: e1cd12bc     	strh	r1, [sp, #44]
   360e8: e1a00007     	mov	r0, r7
   360ec: e1a01002     	mov	r1, r2
   360f0: e28d9028     	add	r9, sp, #40
   360f4: e58dc024     	str	r12, [sp, #0x24]
   360f8: e3a0c000     	mov	r12, #0
   360fc: e58d5038     	str	r5, [sp, #0x38]
   36100: e58d9020     	str	r9, [sp, #0x20]
   36104: e5cdc02e     	strb	r12, [sp, #0x2e]
   36108: e58dc03c     	str	r12, [sp, #0x3c]
   3610c: e5cdc040     	strb	r12, [sp, #0x40]
   36110: eb00d354     	bl	0x6ae68
   36114: e59d0038     	ldr	r0, [sp, #0x38]
   36118: e1500005     	cmp	r0, r5
   3611c: 0a000000     	beq	0x36124
   36120: ebff7f46     	bl	0x15e40    @ imm = #-0x202e8 ; _ZdlPv
   36124: e59d0020     	ldr	r0, [sp, #0x20]
   36128: e1500009     	cmp	r0, r9
   3612c: 0a000000     	beq	0x36134
   36130: ebff7f42     	bl	0x15e40    @ imm = #-0x202f8 ; _ZdlPv
   36134: e28d1020     	add	r1, sp, #32
   36138: e3a02000     	mov	r2, #0
   3613c: e1a00006     	mov	r0, r6
   36140: e3a03010     	mov	r3, #16
   36144: e58d5038     	str	r5, [sp, #0x38]
   36148: e58d3020     	str	r3, [sp, #0x20]
   3614c: ebff8079     	bl	0x16338    @ imm = #-0x1fe1c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   36150: e302c150     	movw	r12, #0x2150
   36154: e340c007     	movt	r12, #0x7
   36158: e1a0e000     	mov	lr, r0
   3615c: e58d0038     	str	r0, [sp, #0x38]
   36160: e59da020     	ldr	r10, [sp, #0x20]
   36164: e3a09000     	mov	r9, #0
   36168: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3616c: e58da040     	str	r10, [sp, #0x40]
   36170: e58e0000     	str	r0, [lr]
   36174: e3090fec     	movw	r0, #0x9fec
   36178: e3400009     	movt	r0, #0x9
   3617c: e58e1004     	str	r1, [lr, #0x4]
   36180: e58e2008     	str	r2, [lr, #0x8]
   36184: e1a01006     	mov	r1, r6
   36188: e58e300c     	str	r3, [lr, #0xc]
   3618c: e1a02009     	mov	r2, r9
   36190: e59d3020     	ldr	r3, [sp, #0x20]
   36194: e59dc038     	ldr	r12, [sp, #0x38]
   36198: e58d303c     	str	r3, [sp, #0x3c]
   3619c: e7cc9003     	strb	r9, [r12, r3]
   361a0: eb00e75e     	bl	0x6ff20
   361a4: e59d0038     	ldr	r0, [sp, #0x38]
   361a8: e1500005     	cmp	r0, r5
   361ac: 0a000000     	beq	0x361b4
   361b0: ebff7f22     	bl	0x15e40    @ imm = #-0x20378 ; _ZdlPv
   361b4: e59d0008     	ldr	r0, [sp, #0x8]
   361b8: e1500008     	cmp	r0, r8
   361bc: 0a000000     	beq	0x361c4
   361c0: ebff7f1e     	bl	0x15e40    @ imm = #-0x20388 ; _ZdlPv
   361c4: e1a00004     	mov	r0, r4
   361c8: e28dd050     	add	sp, sp, #80
   361cc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   361d0: e1a00007     	mov	r0, r7
   361d4: eb00d03f     	bl	0x6a2d8
   361d8: ebff7f60     	bl	0x15f60    @ imm = #-0x20280 ; __cxa_end_cleanup
   361dc: e59d0038     	ldr	r0, [sp, #0x38]
   361e0: e1500005     	cmp	r0, r5
   361e4: 0a000000     	beq	0x361ec
   361e8: ebff7f14     	bl	0x15e40    @ imm = #-0x203b0 ; _ZdlPv
   361ec: e59d0008     	ldr	r0, [sp, #0x8]
   361f0: e1500008     	cmp	r0, r8
   361f4: 0afffff5     	beq	0x361d0
   361f8: ebff7f10     	bl	0x15e40    @ imm = #-0x203c0 ; _ZdlPv
   361fc: eafffff3     	b	0x361d0
   36200: e59d0038     	ldr	r0, [sp, #0x38]
   36204: e1500005     	cmp	r0, r5
   36208: 0a000000     	beq	0x36210
   3620c: ebff7f0b     	bl	0x15e40    @ imm = #-0x203d4 ; _ZdlPv
   36210: e59d0020     	ldr	r0, [sp, #0x20]
   36214: e1500009     	cmp	r0, r9
   36218: 1afffff2     	bne	0x361e8
   3621c: eafffff2     	b	0x361ec
   36220: eaffffed     	b	0x361dc
   36224: eaffffec     	b	0x361dc
   36228: eaffffef     	b	0x361ec
   3622c: e59d0038     	ldr	r0, [sp, #0x38]
   36230: e1500005     	cmp	r0, r5
   36234: 1affffef     	bne	0x361f8
   36238: eaffffe4     	b	0x361d0
   3623c: 40 33 07 00  	.word	0x00073340
