; lubadh::Application::interpretLink()
; VA 0x27d08 size 1160

   27d08: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   27d0c: e1a04000     	mov	r4, r0
   27d10: e2847915     	add	r7, r4, #344064
   27d14: e24dd01c     	sub	sp, sp, #28
   27d18: e3a01027     	mov	r1, #39
   27d1c: eb0110e8     	bl	0x6c0c4
   27d20: e1a05000     	mov	r5, r0
   27d24: e5d76abc     	ldrb	r6, [r7, #0xabc]
   27d28: e3560000     	cmp	r6, #0
   27d2c: 0a000004     	beq	0x27d44
   27d30: e3500000     	cmp	r0, #0
   27d34: 0a000027     	beq	0x27dd8
   27d38: e5c75abc     	strb	r5, [r7, #0xabc]
   27d3c: e28dd01c     	add	sp, sp, #28
   27d40: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   27d44: e3500000     	cmp	r0, #0
   27d48: 0afffffa     	beq	0x27d38
   27d4c: e1a0900d     	mov	r9, sp
   27d50: e3001ce8     	movw	r1, #0xce8
   27d54: e3401007     	movt	r1, #0x7
   27d58: e1a00009     	mov	r0, r9
   27d5c: ebffff91     	bl	0x27ba8
   27d60: e3090fec     	movw	r0, #0x9fec
   27d64: e3400009     	movt	r0, #0x9
   27d68: e1a02006     	mov	r2, r6
   27d6c: e1a01009     	mov	r1, r9
   27d70: eb01206a     	bl	0x6ff20
   27d74: e59d0000     	ldr	r0, [sp]
   27d78: e28d3008     	add	r3, sp, #8
   27d7c: e1500003     	cmp	r0, r3
   27d80: 0a000000     	beq	0x27d88
   27d84: ebffb82d     	bl	0x15e40    @ imm = #-0x11f4c ; _ZdlPv
   27d88: e2848a2a     	add	r8, r4, #172032
   27d8c: e59422c0     	ldr	r2, [r4, #0x2c0]
   27d90: e284aba9     	add	r10, r4, #173056
   27d94: e3a01001     	mov	r1, #1
   27d98: e284306c     	add	r3, r4, #108
   27d9c: e28aad06     	add	r10, r10, #384
   27da0: e59807f8     	ldr	r0, [r8, #0x7f8]
   27da4: e284b048     	add	r11, r4, #72
   27da8: e5922004     	ldr	r2, [r2, #0x4]
   27dac: e5883668     	str	r3, [r8, #0x668]
   27db0: e5906004     	ldr	r6, [r0, #0x4]
   27db4: e5c412cd     	strb	r1, [r4, #0x2cd]
   27db8: e5c81805     	strb	r1, [r8, #0x805]
   27dbc: e3520003     	cmp	r2, #3
   27dc0: 979ff102     	ldrls	pc, [pc, r2, lsl #2]
   27dc4: ea000036     	b	0x27ea4
   27dc8: 10 80 02 00  	.word	0x00028010
   27dcc: d0 7f 02 00  	.word	0x00027fd0
   27dd0: 74 7e 02 00  	.word	0x00027e74
   27dd4: 58 7e 02 00  	.word	0x00027e58
   27dd8: e1a0000d     	mov	r0, sp
   27ddc: e3001cf8     	movw	r1, #0xcf8
   27de0: e3401007     	movt	r1, #0x7
   27de4: ebffff6f     	bl	0x27ba8
   27de8: e3090fec     	movw	r0, #0x9fec
   27dec: e3400009     	movt	r0, #0x9
   27df0: e1a02005     	mov	r2, r5
   27df4: e1a0100d     	mov	r1, sp
   27df8: eb012048     	bl	0x6ff20
   27dfc: e59d0000     	ldr	r0, [sp]
   27e00: e28d3008     	add	r3, sp, #8
   27e04: e1500003     	cmp	r0, r3
   27e08: 0a000000     	beq	0x27e10
   27e0c: ebffb80b     	bl	0x15e40    @ imm = #-0x11fd4 ; _ZdlPv
   27e10: e2843a2a     	add	r3, r4, #172032
   27e14: e2842ba9     	add	r2, r4, #173056
   27e18: e2822f69     	add	r2, r2, #420
   27e1c: e3a01001     	mov	r1, #1
   27e20: e5832668     	str	r2, [r3, #0x668]
   27e24: e5932628     	ldr	r2, [r3, #0x628]
   27e28: e5c31805     	strb	r1, [r3, #0x805]
   27e2c: e2422002     	sub	r2, r2, #2
   27e30: e5c412cd     	strb	r1, [r4, #0x2cd]
   27e34: e16f2f12     	clz	r2, r2
   27e38: e5d317fc     	ldrb	r1, [r3, #0x7fc]
   27e3c: e1a022a2     	lsr	r2, r2, #5
   27e40: e5c327fc     	strb	r2, [r3, #0x7fc]
   27e44: e0222001     	eor	r2, r2, r1
   27e48: e5c327fd     	strb	r2, [r3, #0x7fd]
   27e4c: e5c75abc     	strb	r5, [r7, #0xabc]
   27e50: e28dd01c     	add	sp, sp, #28
   27e54: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   27e58: e3560003     	cmp	r6, #3
   27e5c: 979ff106     	ldrls	pc, [pc, r6, lsl #2]
   27e60: ea00000f     	b	0x27ea4
   27e64: 10 7f 02 00  	.word	0x00027f10
   27e68: 90 7e 02 00  	.word	0x00027e90
   27e6c: 90 7e 02 00  	.word	0x00027e90
   27e70: 9c 7e 02 00  	.word	0x00027e9c
   27e74: e3560003     	cmp	r6, #3
   27e78: 979ff106     	ldrls	pc, [pc, r6, lsl #2]
   27e7c: ea000008     	b	0x27ea4
   27e80: 68 7f 02 00  	.word	0x00027f68
   27e84: d8 7e 02 00  	.word	0x00027ed8
   27e88: 9c 7e 02 00  	.word	0x00027e9c
   27e8c: e4 7e 02 00  	.word	0x00027ee4
   27e90: e3a01000     	mov	r1, #0
   27e94: e1a0000a     	mov	r0, r10
   27e98: eb005cc9     	bl	0x3f1c4
   27e9c: e1a0000a     	mov	r0, r10
   27ea0: eb004103     	bl	0x382b4
   27ea4: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   27ea8: e5d422c4     	ldrb	r2, [r4, #0x2c4]
   27eac: e2883e5f     	add	r3, r8, #1520
   27eb0: e5c827fc     	strb	r2, [r8, #0x7fc]
   27eb4: e2833008     	add	r3, r3, #8
   27eb8: e0222001     	eor	r2, r2, r1
   27ebc: e5c827fd     	strb	r2, [r8, #0x7fd]
   27ec0: e1c40cd0     	ldrd	r0, r1, [r4, #192]
   27ec4: e8830003     	stm	r3, {r0, r1}
   27ec8: e8890003     	stm	r9, {r0, r1}
   27ecc: e5c75abc     	strb	r5, [r7, #0xabc]
   27ed0: e28dd01c     	add	sp, sp, #28
   27ed4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   27ed8: e3a01000     	mov	r1, #0
   27edc: e1a0000a     	mov	r0, r10
   27ee0: eb005cb7     	bl	0x3f1c4
   27ee4: e1a0000a     	mov	r0, r10
   27ee8: eb0040f1     	bl	0x382b4
   27eec: e5983668     	ldr	r3, [r8, #0x668]
   27ef0: e5933084     	ldr	r3, [r3, #0x84]
   27ef4: e3530001     	cmp	r3, #1
   27ef8: 0a000097     	beq	0x2815c
   27efc: e3a01000     	mov	r1, #0
   27f00: e1a0000a     	mov	r0, r10
   27f04: eb005cae     	bl	0x3f1c4
   27f08: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   27f0c: eaffffe5     	b	0x27ea8
   27f10: e3a01003     	mov	r1, #3
   27f14: e1a0000a     	mov	r0, r10
   27f18: eb00483e     	bl	0x3a018
   27f1c: e2842094     	add	r2, r4, #148
   27f20: e2883d17     	add	r3, r8, #1472
   27f24: e594e0a0     	ldr	lr, [r4, #0xa0]
   27f28: e283300c     	add	r3, r3, #12
   27f2c: e598c5d8     	ldr	r12, [r8, #0x5d8]
   27f30: f462078f     	vld1.32	{d16}, [r2]
   27f34: e3a01000     	mov	r1, #0
   27f38: e3a02001     	mov	r2, #1
   27f3c: e1a0000a     	mov	r0, r10
   27f40: f443078f     	vst1.32	{d16}, [r3]
   27f44: e59430cc     	ldr	r3, [r4, #0xcc]
   27f48: e5883604     	str	r3, [r8, #0x604]
   27f4c: e59e3000     	ldr	r3, [lr]
   27f50: e58c3000     	str	r3, [r12]
   27f54: eb003dd4     	bl	0x376ac
   27f58: e1a0000a     	mov	r0, r10
   27f5c: eb0040d4     	bl	0x382b4
   27f60: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   27f64: eaffffcf     	b	0x27ea8
   27f68: e2842094     	add	r2, r4, #148
   27f6c: e2883d17     	add	r3, r8, #1472
   27f70: e283300c     	add	r3, r3, #12
   27f74: e594e0a0     	ldr	lr, [r4, #0xa0]
   27f78: e598c5d8     	ldr	r12, [r8, #0x5d8]
   27f7c: e3a01000     	mov	r1, #0
   27f80: f462078f     	vld1.32	{d16}, [r2]
   27f84: e1a0000a     	mov	r0, r10
   27f88: e3a02001     	mov	r2, #1
   27f8c: f443078f     	vst1.32	{d16}, [r3]
   27f90: e59430cc     	ldr	r3, [r4, #0xcc]
   27f94: e5883604     	str	r3, [r8, #0x604]
   27f98: e59e3000     	ldr	r3, [lr]
   27f9c: e58c3000     	str	r3, [r12]
   27fa0: eb003dc1     	bl	0x376ac
   27fa4: e1a0000a     	mov	r0, r10
   27fa8: eb0040c1     	bl	0x382b4
   27fac: e3a01002     	mov	r1, #2
   27fb0: e1a0000a     	mov	r0, r10
   27fb4: eb004817     	bl	0x3a018
   27fb8: e5983668     	ldr	r3, [r8, #0x668]
   27fbc: e5933084     	ldr	r3, [r3, #0x84]
   27fc0: e3530001     	cmp	r3, #1
   27fc4: 01a01005     	moveq	r1, r5
   27fc8: 1affffb5     	bne	0x27ea4
   27fcc: eaffffb5     	b	0x27ea8
   27fd0: e3560002     	cmp	r6, #2
   27fd4: 0a000022     	beq	0x28064
   27fd8: e3560003     	cmp	r6, #3
   27fdc: 0a000019     	beq	0x28048
   27fe0: e3560000     	cmp	r6, #0
   27fe4: 1affffae     	bne	0x27ea4
   27fe8: e59430f0     	ldr	r3, [r4, #0xf0]
   27fec: e3530001     	cmp	r3, #1
   27ff0: 1affffc1     	bne	0x27efc
   27ff4: e5d837fc     	ldrb	r3, [r8, #0x7fc]
   27ff8: e3530000     	cmp	r3, #0
   27ffc: 1affffbe     	bne	0x27efc
   28000: e1a01006     	mov	r1, r6
   28004: e1a0000a     	mov	r0, r10
   28008: eb005c6d     	bl	0x3f1c4
   2800c: eaffffba     	b	0x27efc
   28010: e3560002     	cmp	r6, #2
   28014: 0a000036     	beq	0x280f4
   28018: e3560003     	cmp	r6, #3
   2801c: 0a00001e     	beq	0x2809c
   28020: e3560001     	cmp	r6, #1
   28024: 1affff9e     	bne	0x27ea4
   28028: e3a01000     	mov	r1, #0
   2802c: e1a0000a     	mov	r0, r10
   28030: eb005c63     	bl	0x3f1c4
   28034: e1a01006     	mov	r1, r6
   28038: e1a0000a     	mov	r0, r10
   2803c: eb005c60     	bl	0x3f1c4
   28040: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   28044: eaffff97     	b	0x27ea8
   28048: e3a01000     	mov	r1, #0
   2804c: e1a0000b     	mov	r0, r11
   28050: eb005c5b     	bl	0x3f1c4
   28054: e1a0000a     	mov	r0, r10
   28058: eb004095     	bl	0x382b4
   2805c: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   28060: eaffff90     	b	0x27ea8
   28064: e1a0000b     	mov	r0, r11
   28068: e3a01000     	mov	r1, #0
   2806c: eb005c54     	bl	0x3f1c4
   28070: e1a0000a     	mov	r0, r10
   28074: eb00408e     	bl	0x382b4
   28078: e5943130     	ldr	r3, [r4, #0x130]
   2807c: e5933084     	ldr	r3, [r3, #0x84]
   28080: e3530001     	cmp	r3, #1
   28084: 0a00002e     	beq	0x28144
   28088: e3a01000     	mov	r1, #0
   2808c: e1a0000b     	mov	r0, r11
   28090: eb005c4b     	bl	0x3f1c4
   28094: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   28098: eaffff82     	b	0x27ea8
   2809c: e1a01006     	mov	r1, r6
   280a0: e1a0000b     	mov	r0, r11
   280a4: eb0047db     	bl	0x3a018
   280a8: e2881d17     	add	r1, r8, #1472
   280ac: e281100c     	add	r1, r1, #12
   280b0: e2843094     	add	r3, r4, #148
   280b4: e598e5d8     	ldr	lr, [r8, #0x5d8]
   280b8: e3a02001     	mov	r2, #1
   280bc: e594c0a0     	ldr	r12, [r4, #0xa0]
   280c0: e1a0000b     	mov	r0, r11
   280c4: f461078f     	vld1.32	{d16}, [r1]
   280c8: e3a01000     	mov	r1, #0
   280cc: f443078f     	vst1.32	{d16}, [r3]
   280d0: e5983604     	ldr	r3, [r8, #0x604]
   280d4: e58430cc     	str	r3, [r4, #0xcc]
   280d8: e59e3000     	ldr	r3, [lr]
   280dc: e58c3000     	str	r3, [r12]
   280e0: eb003d71     	bl	0x376ac
   280e4: e1a0000b     	mov	r0, r11
   280e8: eb004071     	bl	0x382b4
   280ec: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   280f0: eaffff6c     	b	0x27ea8
   280f4: e3a01003     	mov	r1, #3
   280f8: e1a0000b     	mov	r0, r11
   280fc: eb0047c5     	bl	0x3a018
   28100: e2881d17     	add	r1, r8, #1472
   28104: e281100c     	add	r1, r1, #12
   28108: e2843094     	add	r3, r4, #148
   2810c: e598e5d8     	ldr	lr, [r8, #0x5d8]
   28110: e1a0000b     	mov	r0, r11
   28114: e594c0a0     	ldr	r12, [r4, #0xa0]
   28118: e3a02001     	mov	r2, #1
   2811c: f461078f     	vld1.32	{d16}, [r1]
   28120: e3a01000     	mov	r1, #0
   28124: f443078f     	vst1.32	{d16}, [r3]
   28128: e5983604     	ldr	r3, [r8, #0x604]
   2812c: e58430cc     	str	r3, [r4, #0xcc]
   28130: e59e3000     	ldr	r3, [lr]
   28134: e58c3000     	str	r3, [r12]
   28138: eb003d5b     	bl	0x376ac
   2813c: e1a0000b     	mov	r0, r11
   28140: eaffffcb     	b	0x28074
   28144: e5d412c4     	ldrb	r1, [r4, #0x2c4]
   28148: e3510000     	cmp	r1, #0
   2814c: 1affffcd     	bne	0x28088
   28150: e1a0000b     	mov	r0, r11
   28154: eb005c1a     	bl	0x3f1c4
   28158: eaffffca     	b	0x28088
   2815c: e5d817fc     	ldrb	r1, [r8, #0x7fc]
   28160: e3510000     	cmp	r1, #0
   28164: 1affff64     	bne	0x27efc
   28168: e1a0000a     	mov	r0, r10
   2816c: eb005c14     	bl	0x3f1c4
   28170: eaffff61     	b	0x27efc
   28174: e59d0000     	ldr	r0, [sp]
   28178: e28d3008     	add	r3, sp, #8
   2817c: e1500003     	cmp	r0, r3
   28180: 0a000000     	beq	0x28188
   28184: ebffb72d     	bl	0x15e40    @ imm = #-0x1234c ; _ZdlPv
   28188: ebffb774     	bl	0x15f60    @ imm = #-0x12230 ; __cxa_end_cleanup
   2818c: eafffff8     	b	0x28174
