00002934 <arbhar_wtosc_tilde_dsp>:
    2934: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    2938: e1a04000     	mov	r4, r0
    293c: e59f6280     	ldr	r6, [pc, #0x280]        @ 0x2bc4 <arbhar_wtosc_tilde_dsp+0x290>
    2940: e24dd014     	sub	sp, sp, #20
    2944: e59f327c     	ldr	r3, [pc, #0x27c]        @ 0x2bc8 <arbhar_wtosc_tilde_dsp+0x294>
    2948: e1a05001     	mov	r5, r1
    294c: e08f6006     	add	r6, pc, r6
    2950: e5908030     	ldr	r8, [r0, #0x30]
    2954: e7967003     	ldr	r7, [r6, r3]
    2958: e1a00008     	mov	r0, r8
    295c: e5971000     	ldr	r1, [r7]
    2960: ebfffed0     	bl	0x24a8 <.plt+0x1c4>     @ imm = #-0x4c0
    2964: e2509000     	subs	r9, r0, #0
    2968: 0a000055     	beq	0x2ac4 <arbhar_wtosc_tilde_dsp+0x190> @ imm = #0x154
    296c: e284202c     	add	r2, r4, #44
    2970: e2841028     	add	r1, r4, #40
    2974: ebfffeb6     	bl	0x2454 <.plt+0x170>     @ imm = #-0x528
    2978: e2508000     	subs	r8, r0, #0
    297c: 0a000048     	beq	0x2aa4 <arbhar_wtosc_tilde_dsp+0x170> @ imm = #0x120
    2980: e1a00009     	mov	r0, r9
    2984: ebfffee5     	bl	0x2520 <.plt+0x23c>     @ imm = #-0x46c
    2988: e5948084     	ldr	r8, [r4, #0x84]
    298c: e5971000     	ldr	r1, [r7]
    2990: e1a00008     	mov	r0, r8
    2994: ebfffec3     	bl	0x24a8 <.plt+0x1c4>     @ imm = #-0x4f4
    2998: e2509000     	subs	r9, r0, #0
    299c: 0a00004f     	beq	0x2ae0 <arbhar_wtosc_tilde_dsp+0x1ac> @ imm = #0x13c
    29a0: e2842080     	add	r2, r4, #128
    29a4: e28d100c     	add	r1, sp, #12
    29a8: ebfffea9     	bl	0x2454 <.plt+0x170>     @ imm = #-0x55c
    29ac: e2507000     	subs	r7, r0, #0
    29b0: 1a000051     	bne	0x2afc <arbhar_wtosc_tilde_dsp+0x1c8> @ imm = #0x144
    29b4: e5949084     	ldr	r9, [r4, #0x84]
    29b8: e1a00004     	mov	r0, r4
    29bc: e59f2208     	ldr	r2, [pc, #0x208]        @ 0x2bcc <arbhar_wtosc_tilde_dsp+0x298>
    29c0: e08f1002     	add	r1, pc, r2
    29c4: e5992000     	ldr	r2, [r9]
    29c8: ebfffefb     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x414
    29cc: e5847080     	str	r7, [r4, #0x80]
    29d0: ed941a19     	vldr	s2, [r4, #100]
    29d4: e595e000     	ldr	lr, [r5]
    29d8: e59f31f0     	ldr	r3, [pc, #0x1f0]        @ 0x2bd0 <arbhar_wtosc_tilde_dsp+0x29c>
    29dc: ed9f2b73     	vldr	d2, [pc, #460]          @ 0x2bb0 <arbhar_wtosc_tilde_dsp+0x27c>
    29e0: e08f0003     	add	r0, pc, r3
    29e4: eeb73ac1     	vcvt.f64.f32	d3, s2
    29e8: edde1a02     	vldr	s3, [lr, #8]
    29ec: eddf3b71     	vldr	d19, [pc, #452]         @ 0x2bb8 <arbhar_wtosc_tilde_dsp+0x284>
    29f0: ee234b02     	vmul.f64	d4, d3, d2
    29f4: ec532b13     	vmov	r2, r3, d3
    29f8: eeb75ae1     	vcvt.f64.f32	d5, s3
    29fc: ee846b05     	vdiv.f64	d6, d4, d5
    2a00: eeb77b00     	vmov.f64	d7, #1.000000e+00
    2a04: eeb46be3     	vcmpe.f64	d6, d19
    2a08: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a0c: beb06b63     	vmovlt.f64	d6, d19
    2a10: eeb46bc7     	vcmpe.f64	d6, d7
    2a14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a18: 8eb06b47     	vmovhi.f64	d6, d7
    2a1c: eef72bc6     	vcvt.f32.f64	s5, d6
    2a20: eef74ae2     	vcvt.f64.f32	d20, s5
    2a24: edc42a1b     	vstr	s5, [r4, #108]
    2a28: edcd4b00     	vstr	d20, [sp]
    2a2c: ebfffeb5     	bl	0x2508 <.plt+0x224>     @ imm = #-0x52c
    2a30: e5959000     	ldr	r9, [r5]
    2a34: e59f2198     	ldr	r2, [pc, #0x198]        @ 0x2bd4 <arbhar_wtosc_tilde_dsp+0x2a0>
    2a38: edd46a0a     	vldr	s13, [r4, #40]
    2a3c: e08f0002     	add	r0, pc, r2
    2a40: eddf3a5e     	vldr	s7, [pc, #376]          @ 0x2bc0 <arbhar_wtosc_tilde_dsp+0x28c>
    2a44: eef87ae6     	vcvt.f32.s32	s15, s13
    2a48: edd94a02     	vldr	s9, [r9, #8]
    2a4c: edc44a07     	vstr	s9, [r4, #28]
    2a50: eef75a00     	vmov.f32	s11, #1.000000e+00
    2a54: eec30aa7     	vdiv.f32	s1, s7, s15
    2a58: ee850aa4     	vdiv.f32	s0, s11, s9
    2a5c: eef75ae0     	vcvt.f64.f32	d21, s1
    2a60: edc40a13     	vstr	s1, [r4, #76]
    2a64: ec532b35     	vmov	r2, r3, d21
    2a68: ed840a24     	vstr	s0, [r4, #144]
    2a6c: ebfffea5     	bl	0x2508 <.plt+0x224>     @ imm = #-0x56c
    2a70: e8951008     	ldm	r5, {r3, r12}
    2a74: e1a02004     	mov	r2, r4
    2a78: e59f4158     	ldr	r4, [pc, #0x158]        @ 0x2bd8 <arbhar_wtosc_tilde_dsp+0x2a4>
    2a7c: e3a01004     	mov	r1, #4
    2a80: e5935000     	ldr	r5, [r3]
    2a84: e7960004     	ldr	r0, [r6, r4]
    2a88: e5933004     	ldr	r3, [r3, #0x4]
    2a8c: e58d5004     	str	r5, [sp, #0x4]
    2a90: e59c6004     	ldr	r6, [r12, #0x4]
    2a94: e58d6000     	str	r6, [sp]
    2a98: ebfffeaf     	bl	0x255c <.plt+0x278>     @ imm = #-0x544
    2a9c: e28dd014     	add	sp, sp, #20
    2aa0: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    2aa4: e5942030     	ldr	r2, [r4, #0x30]
    2aa8: e1a00004     	mov	r0, r4
    2aac: e59f1128     	ldr	r1, [pc, #0x128]        @ 0x2bdc <arbhar_wtosc_tilde_dsp+0x2a8>
    2ab0: e5922000     	ldr	r2, [r2]
    2ab4: e08f1001     	add	r1, pc, r1
    2ab8: ebfffebf     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x504
    2abc: e584802c     	str	r8, [r4, #0x2c]
    2ac0: eaffffb0     	b	0x2988 <arbhar_wtosc_tilde_dsp+0x54> @ imm = #-0x140
    2ac4: e5980000     	ldr	r0, [r8]
    2ac8: e5d0c000     	ldrb	r12, [r0]
    2acc: e35c0000     	cmp	r12, #0
    2ad0: 1a00002e     	bne	0x2b90 <arbhar_wtosc_tilde_dsp+0x25c> @ imm = #0xb8
    2ad4: e3a09000     	mov	r9, #0
    2ad8: e584902c     	str	r9, [r4, #0x2c]
    2adc: eaffffa9     	b	0x2988 <arbhar_wtosc_tilde_dsp+0x54> @ imm = #-0x15c
    2ae0: e5980000     	ldr	r0, [r8]
    2ae4: e5d07000     	ldrb	r7, [r0]
    2ae8: e3570000     	cmp	r7, #0
    2aec: 1a000020     	bne	0x2b74 <arbhar_wtosc_tilde_dsp+0x240> @ imm = #0x80
    2af0: e3a0c000     	mov	r12, #0
    2af4: e584c080     	str	r12, [r4, #0x80]
    2af8: eaffffb4     	b	0x29d0 <arbhar_wtosc_tilde_dsp+0x9c> @ imm = #-0x130
    2afc: e59d200c     	ldr	r2, [sp, #0xc]
    2b00: e2428003     	sub	r8, r2, #3
    2b04: e1a00008     	mov	r0, r8
    2b08: ebfffe2d     	bl	0x23c4 <.plt+0xe0>      @ imm = #-0x74c
    2b0c: e3a01001     	mov	r1, #1
    2b10: e1580011     	cmp	r8, r1, lsl r0
    2b14: 0a00000b     	beq	0x2b48 <arbhar_wtosc_tilde_dsp+0x214> @ imm = #0x2c
    2b18: e594c084     	ldr	r12, [r4, #0x84]
    2b1c: e1a00004     	mov	r0, r4
    2b20: e59fe0b8     	ldr	lr, [pc, #0xb8]         @ 0x2be0 <arbhar_wtosc_tilde_dsp+0x2ac>
    2b24: e59d300c     	ldr	r3, [sp, #0xc]
    2b28: e08f100e     	add	r1, pc, lr
    2b2c: e59c2000     	ldr	r2, [r12]
    2b30: ebfffea1     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x57c
    2b34: e3a03000     	mov	r3, #0
    2b38: e1a00009     	mov	r0, r9
    2b3c: e5843080     	str	r3, [r4, #0x80]
    2b40: ebfffe76     	bl	0x2520 <.plt+0x23c>     @ imm = #-0x628
    2b44: eaffffa1     	b	0x29d0 <arbhar_wtosc_tilde_dsp+0x9c> @ imm = #-0x17c
    2b48: eef72b00     	vmov.f64	d18, #1.000000e+00
    2b4c: e1a00009     	mov	r0, r9
    2b50: ee078a90     	vmov	s15, r8
    2b54: eef81be7     	vcvt.f64.s32	d17, s15
    2b58: eec20ba1     	vdiv.f64	d16, d18, d17
    2b5c: eeb80ae7     	vcvt.f32.s32	s0, s15
    2b60: ed840a1e     	vstr	s0, [r4, #120]
    2b64: eef70be0     	vcvt.f32.f64	s1, d16
    2b68: edc40a1f     	vstr	s1, [r4, #124]
    2b6c: ebfffe6b     	bl	0x2520 <.plt+0x23c>     @ imm = #-0x654
    2b70: eaffff96     	b	0x29d0 <arbhar_wtosc_tilde_dsp+0x9c> @ imm = #-0x1a8
    2b74: e5948084     	ldr	r8, [r4, #0x84]
    2b78: e1a00004     	mov	r0, r4
    2b7c: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2be4 <arbhar_wtosc_tilde_dsp+0x2b0>
    2b80: e5982000     	ldr	r2, [r8]
    2b84: e08f1001     	add	r1, pc, r1
    2b88: ebfffe8b     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x5d4
    2b8c: eaffffd7     	b	0x2af0 <arbhar_wtosc_tilde_dsp+0x1bc> @ imm = #-0xa4
    2b90: e594e030     	ldr	lr, [r4, #0x30]
    2b94: e1a00004     	mov	r0, r4
    2b98: e59f3048     	ldr	r3, [pc, #0x48]         @ 0x2be8 <arbhar_wtosc_tilde_dsp+0x2b4>
    2b9c: e59e2000     	ldr	r2, [lr]
    2ba0: e08f1003     	add	r1, pc, r3
    2ba4: ebfffe84     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x5f0
    2ba8: eaffffc9     	b	0x2ad4 <arbhar_wtosc_tilde_dsp+0x1a0> @ imm = #-0xdc
    2bac: e320f000     	nop
    2bb0: 18 2d 44 54  	.word	0x54442d18
    2bb4: fb 21 19 40  	.word	0x401921fb
    2bb8: 00 00 00 00  	.word	0x00000000
    2bbc: 00 00 00 00  	.word	0x00000000
    2bc0: 00 c0 00 44  	.word	0x4400c000
    2bc4: ac 66 01 00  	.word	0x000166ac
    2bc8: 10 01 00 00  	.word	0x00000110
    2bcc: c8 5a 00 00  	.word	0x00005ac8
    2bd0: e8 5a 00 00  	.word	0x00005ae8
    2bd4: e0 5a 00 00  	.word	0x00005ae0
    2bd8: 18 01 00 00  	.word	0x00000118
    2bdc: d4 59 00 00  	.word	0x000059d4
    2be0: bc 59 00 00  	.word	0x000059bc
    2be4: e0 58 00 00  	.word	0x000058e0
    2be8: c4 58 00 00  	.word	0x000058c4

