; lubadh::Application::interpretButtons()
; VA 0x28734 size 3680

   28734: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   28738: e2808048     	add	r8, r0, #72
   2873c: e2883a2a     	add	r3, r8, #172032
   28740: e24dd02c     	sub	sp, sp, #44
   28744: e30ab538     	movw	r11, #0xa538
   28748: e340b002     	movt	r11, #0x2
   2874c: e1a06000     	mov	r6, r0
   28750: e3a07000     	mov	r7, #0
   28754: e1a04008     	mov	r4, r8
   28758: e2833e1f     	add	r3, r3, #496
   2875c: e58d3000     	str	r3, [sp]
   28760: e227a001     	eor	r10, r7, #1
   28764: e5942104     	ldr	r2, [r4, #0x104]
   28768: e2423001     	sub	r3, r2, #1
   2876c: e0090a9b     	mul	r9, r11, r10
   28770: e3530001     	cmp	r3, #1
   28774: e0885009     	add	r5, r8, r9
   28778: 9a0001c0     	bls	0x28e80
   2877c: e3520000     	cmp	r2, #0
   28780: 1a000008     	bne	0x287a8
   28784: e5953104     	ldr	r3, [r5, #0x104]
   28788: e3530000     	cmp	r3, #0
   2878c: 1a000005     	bne	0x287a8
   28790: e2863915     	add	r3, r6, #344064
   28794: e5c4228c     	strb	r2, [r4, #0x28c]
   28798: e5d33abc     	ldrb	r3, [r3, #0xabc]
   2879c: e3530000     	cmp	r3, #0
   287a0: 10236a9b     	mlane	r3, r11, r10, r6
   287a4: 15c322d4     	strbne	r2, [r3, #0x2d4]
   287a8: e5d430fd     	ldrb	r3, [r4, #0xfd]
   287ac: e3530000     	cmp	r3, #0
   287b0: 0a000005     	beq	0x287cc
   287b4: e594311c     	ldr	r3, [r4, #0x11c]
   287b8: e3530000     	cmp	r3, #0
   287bc: 1a000002     	bne	0x287cc
   287c0: e5943104     	ldr	r3, [r4, #0x104]
   287c4: e3530000     	cmp	r3, #0
   287c8: 0a0001c5     	beq	0x28ee4
   287cc: e5d43126     	ldrb	r3, [r4, #0x126]
   287d0: e3530000     	cmp	r3, #0
   287d4: 0a000099     	beq	0x28a40
   287d8: e59430f4     	ldr	r3, [r4, #0xf4]
   287dc: e3530000     	cmp	r3, #0
   287e0: 1a000096     	bne	0x28a40
   287e4: e5943104     	ldr	r3, [r4, #0x104]
   287e8: e3530000     	cmp	r3, #0
   287ec: 1a000005     	bne	0x28808
   287f0: e59530f4     	ldr	r3, [r5, #0xf4]
   287f4: e3530000     	cmp	r3, #0
   287f8: 1a000002     	bne	0x28808
   287fc: e5953104     	ldr	r3, [r5, #0x104]
   28800: e3530000     	cmp	r3, #0
   28804: 0a000256     	beq	0x29164
   28808: e5d4310e     	ldrb	r3, [r4, #0x10e]
   2880c: e3530000     	cmp	r3, #0
   28810: 0a0000c8     	beq	0x28b38
   28814: e594311c     	ldr	r3, [r4, #0x11c]
   28818: e3530000     	cmp	r3, #0
   2881c: 1a00008d     	bne	0x28a58
   28820: e59530f4     	ldr	r3, [r5, #0xf4]
   28824: e3530000     	cmp	r3, #0
   28828: 1a000002     	bne	0x28838
   2882c: e595311c     	ldr	r3, [r5, #0x11c]
   28830: e3530000     	cmp	r3, #0
   28834: 0a0000ab     	beq	0x28ae8
   28838: e5d4310d     	ldrb	r3, [r4, #0x10d]
   2883c: e3530000     	cmp	r3, #0
   28840: 0a000084     	beq	0x28a58
   28844: e5953104     	ldr	r3, [r5, #0x104]
   28848: e3530000     	cmp	r3, #0
   2884c: 1a000005     	bne	0x28868
   28850: e59530f4     	ldr	r3, [r5, #0xf4]
   28854: e3530000     	cmp	r3, #0
   28858: 1a000002     	bne	0x28868
   2885c: e595311c     	ldr	r3, [r5, #0x11c]
   28860: e3530000     	cmp	r3, #0
   28864: 0a00024e     	beq	0x291a4
   28868: e5d430fd     	ldrb	r3, [r4, #0xfd]
   2886c: e3530000     	cmp	r3, #0
   28870: 0a000006     	beq	0x28890
   28874: e5943104     	ldr	r3, [r4, #0x104]
   28878: e2433001     	sub	r3, r3, #1
   2887c: e3530001     	cmp	r3, #1
   28880: 9a0001ca     	bls	0x28fb0
   28884: e59430f4     	ldr	r3, [r4, #0xf4]
   28888: e3530000     	cmp	r3, #0
   2888c: 1a000005     	bne	0x288a8
   28890: e5d43125     	ldrb	r3, [r4, #0x125]
   28894: e3530000     	cmp	r3, #0
   28898: 0a000002     	beq	0x288a8
   2889c: e5943104     	ldr	r3, [r4, #0x104]
   288a0: e3530000     	cmp	r3, #0
   288a4: 0a0001ad     	beq	0x28f60
   288a8: e5d430fe     	ldrb	r3, [r4, #0xfe]
   288ac: e3530000     	cmp	r3, #0
   288b0: 0a000007     	beq	0x288d4
   288b4: e5d432ac     	ldrb	r3, [r4, #0x2ac]
   288b8: e3530000     	cmp	r3, #0
   288bc: 0a000004     	beq	0x288d4
   288c0: e59430e8     	ldr	r3, [r4, #0xe8]
   288c4: e59330b0     	ldr	r3, [r3, #0xb0]
   288c8: e5933028     	ldr	r3, [r3, #0x28]
   288cc: e3530000     	cmp	r3, #0
   288d0: 0a00020b     	beq	0x29104
   288d4: e594311c     	ldr	r3, [r4, #0x11c]
   288d8: e3530000     	cmp	r3, #0
   288dc: 1a00000c     	bne	0x28914
   288e0: e5d431e0     	ldrb	r3, [r4, #0x1e0]
   288e4: e3530000     	cmp	r3, #0
   288e8: 1a000006     	bne	0x28908
   288ec: e2863915     	add	r3, r6, #344064
   288f0: e5d33abc     	ldrb	r3, [r3, #0xabc]
   288f4: e3530000     	cmp	r3, #0
   288f8: 0a00011e     	beq	0x28d78
   288fc: e5943000     	ldr	r3, [r4]
   28900: e3530000     	cmp	r3, #0
   28904: 0a00020b     	beq	0x29138
   28908: e5d430fd     	ldrb	r3, [r4, #0xfd]
   2890c: e3530000     	cmp	r3, #0
   28910: 1a000121     	bne	0x28d9c
   28914: e5943104     	ldr	r3, [r4, #0x104]
   28918: e2433001     	sub	r3, r3, #1
   2891c: e3530001     	cmp	r3, #1
   28920: 9a00012a     	bls	0x28dd0
   28924: e59430f4     	ldr	r3, [r4, #0xf4]
   28928: e3530000     	cmp	r3, #0
   2892c: 1a0000a9     	bne	0x28bd8
   28930: e5d43125     	ldrb	r3, [r4, #0x125]
   28934: e3530000     	cmp	r3, #0
   28938: 0a000003     	beq	0x2894c
   2893c: e5943104     	ldr	r3, [r4, #0x104]
   28940: e2433001     	sub	r3, r3, #1
   28944: e3530001     	cmp	r3, #1
   28948: 9a000087     	bls	0x28b6c
   2894c: e594211c     	ldr	r2, [r4, #0x11c]
   28950: e2423001     	sub	r3, r2, #1
   28954: e3530001     	cmp	r3, #1
   28958: 9a0001ce     	bls	0x29098
   2895c: e3520000     	cmp	r2, #0
   28960: 1a0000fb     	bne	0x28d54
   28964: e5943104     	ldr	r3, [r4, #0x104]
   28968: e2432001     	sub	r2, r3, #1
   2896c: e3520001     	cmp	r2, #1
   28970: 9a0000d8     	bls	0x28cd8
   28974: e2433001     	sub	r3, r3, #1
   28978: e3530001     	cmp	r3, #1
   2897c: 9a0001ab     	bls	0x29030
   28980: e2844ba9     	add	r4, r4, #173056
   28984: e3570001     	cmp	r7, #1
   28988: e2844f4e     	add	r4, r4, #312
   2898c: 1a0000cf     	bne	0x28cd0
   28990: e28dd02c     	add	sp, sp, #44
   28994: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   28998: e2893a2a     	add	r3, r9, #172032
   2899c: e58d200c     	str	r2, [sp, #0xc]
   289a0: e2833e1f     	add	r3, r3, #496
   289a4: e0883003     	add	r3, r8, r3
   289a8: e58d3004     	str	r3, [sp, #0x4]
   289ac: e1a00003     	mov	r0, r3
   289b0: eb00421a     	bl	0x39220
   289b4: e59d200c     	ldr	r2, [sp, #0xc]
   289b8: e3500000     	cmp	r0, #0
   289bc: 1a0001f1     	bne	0x29188
   289c0: e1a00002     	mov	r0, r2
   289c4: eb00420e     	bl	0x39204
   289c8: e59d3004     	ldr	r3, [sp, #0x4]
   289cc: e3500000     	cmp	r0, #0
   289d0: 1a00001a     	bne	0x28a40
   289d4: e1a00003     	mov	r0, r3
   289d8: eb004209     	bl	0x39204
   289dc: e3500000     	cmp	r0, #0
   289e0: 1a000016     	bne	0x28a40
   289e4: e59d3008     	ldr	r3, [sp, #0x8]
   289e8: e2833048     	add	r3, r3, #72
   289ec: e58d3004     	str	r3, [sp, #0x4]
   289f0: e1a00003     	mov	r0, r3
   289f4: eb0041f2     	bl	0x391c4
   289f8: e3500000     	cmp	r0, #0
   289fc: 0a0002d0     	beq	0x29544
   28a00: e28d0010     	add	r0, sp, #16
   28a04: e3001d08     	movw	r1, #0xd08
   28a08: e3401007     	movt	r1, #0x7
   28a0c: ebfffc65     	bl	0x27ba8
   28a10: e3090fec     	movw	r0, #0x9fec
   28a14: e3400009     	movt	r0, #0x9
   28a18: e28d1010     	add	r1, sp, #16
   28a1c: e3a02000     	mov	r2, #0
   28a20: eb011d3e     	bl	0x6ff20
   28a24: e59d0010     	ldr	r0, [sp, #0x10]
   28a28: e28d3018     	add	r3, sp, #24
   28a2c: e1500003     	cmp	r0, r3
   28a30: 0a000000     	beq	0x28a38
   28a34: ebffb501     	bl	0x15e40    @ imm = #-0x12bfc ; _ZdlPv
   28a38: e59d0004     	ldr	r0, [sp, #0x4]
   28a3c: eb004102     	bl	0x38e4c
   28a40: e5d4310e     	ldrb	r3, [r4, #0x10e]
   28a44: e3530000     	cmp	r3, #0
   28a48: 0a00003a     	beq	0x28b38
   28a4c: e59430f4     	ldr	r3, [r4, #0xf4]
   28a50: e3530000     	cmp	r3, #0
   28a54: 0affff6e     	beq	0x28814
   28a58: e5d430fd     	ldrb	r3, [r4, #0xfd]
   28a5c: e3530000     	cmp	r3, #0
   28a60: 0affff87     	beq	0x28884
   28a64: e594311c     	ldr	r3, [r4, #0x11c]
   28a68: e3530000     	cmp	r3, #0
   28a6c: 0affff80     	beq	0x28874
   28a70: e2433001     	sub	r3, r3, #1
   28a74: e3530001     	cmp	r3, #1
   28a78: 8affff81     	bhi	0x28884
   28a7c: e5943104     	ldr	r3, [r4, #0x104]
   28a80: e3530000     	cmp	r3, #0
   28a84: 1affff7e     	bne	0x28884
   28a88: e59530f4     	ldr	r3, [r5, #0xf4]
   28a8c: e3530000     	cmp	r3, #0
   28a90: 1affff7b     	bne	0x28884
   28a94: e595311c     	ldr	r3, [r5, #0x11c]
   28a98: e3530000     	cmp	r3, #0
   28a9c: 1affff78     	bne	0x28884
   28aa0: e5953104     	ldr	r3, [r5, #0x104]
   28aa4: e3530000     	cmp	r3, #0
   28aa8: 1affff75     	bne	0x28884
   28aac: e2863915     	add	r3, r6, #344064
   28ab0: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28ab4: e3530000     	cmp	r3, #0
   28ab8: 0a000269     	beq	0x29464
   28abc: e5943000     	ldr	r3, [r4]
   28ac0: e3530000     	cmp	r3, #0
   28ac4: 1affff6e     	bne	0x28884
   28ac8: e3a01006     	mov	r1, #6
   28acc: e1a00005     	mov	r0, r5
   28ad0: eb0059bb     	bl	0x3f1c4
   28ad4: e3a01006     	mov	r1, #6
   28ad8: e1a00004     	mov	r0, r4
   28adc: eb0059b8     	bl	0x3f1c4
   28ae0: e59430f4     	ldr	r3, [r4, #0xf4]
   28ae4: eaffff67     	b	0x28888
   28ae8: e2842a2a     	add	r2, r4, #172032
   28aec: e2822fbe     	add	r2, r2, #760
   28af0: e58d2004     	str	r2, [sp, #0x4]
   28af4: e1a00002     	mov	r0, r2
   28af8: eb004209     	bl	0x39324
   28afc: e59d2004     	ldr	r2, [sp, #0x4]
   28b00: e2863915     	add	r3, r6, #344064
   28b04: e3500000     	cmp	r0, #0
   28b08: 1a00022f     	bne	0x293cc
   28b0c: e3a01002     	mov	r1, #2
   28b10: e1a00004     	mov	r0, r4
   28b14: e58d3004     	str	r3, [sp, #0x4]
   28b18: eb0059a9     	bl	0x3f1c4
   28b1c: e59d3004     	ldr	r3, [sp, #0x4]
   28b20: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28b24: e3530000     	cmp	r3, #0
   28b28: 0a000002     	beq	0x28b38
   28b2c: e3a01002     	mov	r1, #2
   28b30: e1a00005     	mov	r0, r5
   28b34: eb0059a2     	bl	0x3f1c4
   28b38: e5d4310d     	ldrb	r3, [r4, #0x10d]
   28b3c: e3530000     	cmp	r3, #0
   28b40: 0affffc4     	beq	0x28a58
   28b44: e59430f4     	ldr	r3, [r4, #0xf4]
   28b48: e3530000     	cmp	r3, #0
   28b4c: 1affffc1     	bne	0x28a58
   28b50: e594311c     	ldr	r3, [r4, #0x11c]
   28b54: e3530000     	cmp	r3, #0
   28b58: 1affffbe     	bne	0x28a58
   28b5c: e5953104     	ldr	r3, [r5, #0x104]
   28b60: e3530000     	cmp	r3, #0
   28b64: 1affff3f     	bne	0x28868
   28b68: eaffff38     	b	0x28850
   28b6c: e59530f4     	ldr	r3, [r5, #0xf4]
   28b70: e3530000     	cmp	r3, #0
   28b74: 1affff74     	bne	0x2894c
   28b78: e595311c     	ldr	r3, [r5, #0x11c]
   28b7c: e3530000     	cmp	r3, #0
   28b80: 1affff71     	bne	0x2894c
   28b84: e5953104     	ldr	r3, [r5, #0x104]
   28b88: e2433001     	sub	r3, r3, #1
   28b8c: e3530001     	cmp	r3, #1
   28b90: 8affff6d     	bhi	0x2894c
   28b94: e2863915     	add	r3, r6, #344064
   28b98: e0216a9b     	mla	r1, r11, r10, r6
   28b9c: e5d32abc     	ldrb	r2, [r3, #0xabc]
   28ba0: e2822001     	add	r2, r2, #1
   28ba4: e5842110     	str	r2, [r4, #0x110]
   28ba8: e5d32abc     	ldrb	r2, [r3, #0xabc]
   28bac: e2822001     	add	r2, r2, #1
   28bb0: e5812158     	str	r2, [r1, #0x158]
   28bb4: e3a01016     	mov	r1, #22
   28bb8: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28bbc: e3530000     	cmp	r3, #0
   28bc0: 11a00008     	movne	r0, r8
   28bc4: 01a00004     	moveq	r0, r4
   28bc8: eb00597d     	bl	0x3f1c4
   28bcc: e59430f4     	ldr	r3, [r4, #0xf4]
   28bd0: e3530000     	cmp	r3, #0
   28bd4: 0affff5c     	beq	0x2894c
   28bd8: e594311c     	ldr	r3, [r4, #0x11c]
   28bdc: e1a02003     	mov	r2, r3
   28be0: e5d430fd     	ldrb	r3, [r4, #0xfd]
   28be4: e3530000     	cmp	r3, #0
   28be8: 02423001     	subeq	r3, r2, #1
   28bec: 0a0000ad     	beq	0x28ea8
   28bf0: e3520000     	cmp	r2, #0
   28bf4: 12423001     	subne	r3, r2, #1
   28bf8: 1a000059     	bne	0x28d64
   28bfc: e5943104     	ldr	r3, [r4, #0x104]
   28c00: e2433001     	sub	r3, r3, #1
   28c04: e3530001     	cmp	r3, #1
   28c08: 8affff5c     	bhi	0x28980
   28c0c: e59530f4     	ldr	r3, [r5, #0xf4]
   28c10: e2433001     	sub	r3, r3, #1
   28c14: e3530001     	cmp	r3, #1
   28c18: 8affff58     	bhi	0x28980
   28c1c: e595311c     	ldr	r3, [r5, #0x11c]
   28c20: e3530000     	cmp	r3, #0
   28c24: 1affff55     	bne	0x28980
   28c28: e5953104     	ldr	r3, [r5, #0x104]
   28c2c: e2433001     	sub	r3, r3, #1
   28c30: e3530001     	cmp	r3, #1
   28c34: 8affff51     	bhi	0x28980
   28c38: ed9f0afb     	vldr	s0, [pc, #1004]         @ 0x2902c ; float 500
   28c3c: eef10a08     	vmov.f32	s1, #6.000000e+00
   28c40: e1a00008     	mov	r0, r8
   28c44: e2863915     	add	r3, r6, #344064
   28c48: e58d3004     	str	r3, [sp, #0x4]
   28c4c: eb003b4a     	bl	0x3797c
   28c50: e59d3004     	ldr	r3, [sp, #0x4]
   28c54: e2869ba9     	add	r9, r6, #173056
   28c58: e2899d06     	add	r9, r9, #384
   28c5c: e1a00008     	mov	r0, r8
   28c60: e5d32abc     	ldrb	r2, [r3, #0xabc]
   28c64: e2822001     	add	r2, r2, #1
   28c68: e5862158     	str	r2, [r6, #0x158]
   28c6c: eb0041cf     	bl	0x393b0
   28c70: e1a00009     	mov	r0, r9
   28c74: ed9f0aec     	vldr	s0, [pc, #944]          @ 0x2902c ; float 500
   28c78: eef10a08     	vmov.f32	s1, #6.000000e+00
   28c7c: eb003b3e     	bl	0x3797c
   28c80: e59d3004     	ldr	r3, [sp, #0x4]
   28c84: e2862a2a     	add	r2, r6, #172032
   28c88: e1a00009     	mov	r0, r9
   28c8c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28c90: e2833001     	add	r3, r3, #1
   28c94: e5823690     	str	r3, [r2, #0x690]
   28c98: eb0041c4     	bl	0x393b0
   28c9c: e594311c     	ldr	r3, [r4, #0x11c]
   28ca0: e2433001     	sub	r3, r3, #1
   28ca4: e3530001     	cmp	r3, #1
   28ca8: 8affff34     	bhi	0x28980
   28cac: e5d430fd     	ldrb	r3, [r4, #0xfd]
   28cb0: e3530000     	cmp	r3, #0
   28cb4: 0a00007d     	beq	0x28eb0
   28cb8: e3a03002     	mov	r3, #2
   28cbc: e584311c     	str	r3, [r4, #0x11c]
   28cc0: e2844ba9     	add	r4, r4, #173056
   28cc4: e3570001     	cmp	r7, #1
   28cc8: e2844f4e     	add	r4, r4, #312
   28ccc: 0affff2f     	beq	0x28990
   28cd0: e3a07001     	mov	r7, #1
   28cd4: eafffea1     	b	0x28760
   28cd8: e59520f4     	ldr	r2, [r5, #0xf4]
   28cdc: e3520000     	cmp	r2, #0
   28ce0: 1affff23     	bne	0x28974
   28ce4: e5d52125     	ldrb	r2, [r5, #0x125]
   28ce8: e3520000     	cmp	r2, #0
   28cec: 0affff20     	beq	0x28974
   28cf0: e5952104     	ldr	r2, [r5, #0x104]
   28cf4: e3520000     	cmp	r2, #0
   28cf8: 1affff1d     	bne	0x28974
   28cfc: e2863915     	add	r3, r6, #344064
   28d00: e3a01012     	mov	r1, #18
   28d04: e58d3004     	str	r3, [sp, #0x4]
   28d08: e5d30abc     	ldrb	r0, [r3, #0xabc]
   28d0c: e3500000     	cmp	r0, #0
   28d10: 11a00008     	movne	r0, r8
   28d14: 01a00004     	moveq	r0, r4
   28d18: eb005929     	bl	0x3f1c4
   28d1c: e59d3004     	ldr	r3, [sp, #0x4]
   28d20: e3a02001     	mov	r2, #1
   28d24: e5842110     	str	r2, [r4, #0x110]
   28d28: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28d2c: e3530000     	cmp	r3, #0
   28d30: 10236a9b     	mlane	r3, r11, r10, r6
   28d34: 15832158     	strne	r2, [r3, #0x158]
   28d38: e59430f4     	ldr	r3, [r4, #0xf4]
   28d3c: e594211c     	ldr	r2, [r4, #0x11c]
   28d40: e3530000     	cmp	r3, #0
   28d44: 1affffa5     	bne	0x28be0
   28d48: e3520000     	cmp	r2, #0
   28d4c: 05943104     	ldreq	r3, [r4, #0x104]
   28d50: 0affff07     	beq	0x28974
   28d54: e2423001     	sub	r3, r2, #1
   28d58: e5d420fd     	ldrb	r2, [r4, #0xfd]
   28d5c: e3520000     	cmp	r2, #0
   28d60: 0a000050     	beq	0x28ea8
   28d64: e3530001     	cmp	r3, #1
   28d68: 8affff04     	bhi	0x28980
   28d6c: e3a03002     	mov	r3, #2
   28d70: e584311c     	str	r3, [r4, #0x11c]
   28d74: eaffffd1     	b	0x28cc0
   28d78: e3a01009     	mov	r1, #9
   28d7c: e1a00004     	mov	r0, r4
   28d80: eb00590f     	bl	0x3f1c4
   28d84: e5d430fd     	ldrb	r3, [r4, #0xfd]
   28d88: e3530000     	cmp	r3, #0
   28d8c: 0afffee0     	beq	0x28914
   28d90: e594311c     	ldr	r3, [r4, #0x11c]
   28d94: e3530000     	cmp	r3, #0
   28d98: 1afffedd     	bne	0x28914
   28d9c: e5943104     	ldr	r3, [r4, #0x104]
   28da0: e2433001     	sub	r3, r3, #1
   28da4: e3530001     	cmp	r3, #1
   28da8: 8afffedd     	bhi	0x28924
   28dac: e59530f4     	ldr	r3, [r5, #0xf4]
   28db0: e3530000     	cmp	r3, #0
   28db4: 1a000005     	bne	0x28dd0
   28db8: e595311c     	ldr	r3, [r5, #0x11c]
   28dbc: e3530000     	cmp	r3, #0
   28dc0: 1a000002     	bne	0x28dd0
   28dc4: e5953104     	ldr	r3, [r5, #0x104]
   28dc8: e3530000     	cmp	r3, #0
   28dcc: 0a00012f     	beq	0x29290
   28dd0: e5d43126     	ldrb	r3, [r4, #0x126]
   28dd4: e3530000     	cmp	r3, #0
   28dd8: 0a00000b     	beq	0x28e0c
   28ddc: e59430f4     	ldr	r3, [r4, #0xf4]
   28de0: e3530000     	cmp	r3, #0
   28de4: 1a000008     	bne	0x28e0c
   28de8: e59530f4     	ldr	r3, [r5, #0xf4]
   28dec: e3530000     	cmp	r3, #0
   28df0: 1a000005     	bne	0x28e0c
   28df4: e595311c     	ldr	r3, [r5, #0x11c]
   28df8: e3530000     	cmp	r3, #0
   28dfc: 1a000002     	bne	0x28e0c
   28e00: e5953104     	ldr	r3, [r5, #0x104]
   28e04: e3530000     	cmp	r3, #0
   28e08: 0a000100     	beq	0x29210
   28e0c: e594311c     	ldr	r3, [r4, #0x11c]
   28e10: e2433001     	sub	r3, r3, #1
   28e14: e3530001     	cmp	r3, #1
   28e18: 8afffec1     	bhi	0x28924
   28e1c: e5d430fd     	ldrb	r3, [r4, #0xfd]
   28e20: e3530000     	cmp	r3, #0
   28e24: 0afffebe     	beq	0x28924
   28e28: e59530f4     	ldr	r3, [r5, #0xf4]
   28e2c: e3530000     	cmp	r3, #0
   28e30: 1afffebb     	bne	0x28924
   28e34: e595311c     	ldr	r3, [r5, #0x11c]
   28e38: e3530000     	cmp	r3, #0
   28e3c: 1afffeb8     	bne	0x28924
   28e40: e5953104     	ldr	r3, [r5, #0x104]
   28e44: e3530000     	cmp	r3, #0
   28e48: 1afffeb5     	bne	0x28924
   28e4c: e3a09001     	mov	r9, #1
   28e50: e3a0100f     	mov	r1, #15
   28e54: e1a00004     	mov	r0, r4
   28e58: e5849110     	str	r9, [r4, #0x110]
   28e5c: e2863915     	add	r3, r6, #344064
   28e60: e58d3004     	str	r3, [sp, #0x4]
   28e64: eb0058d6     	bl	0x3f1c4
   28e68: e59d3004     	ldr	r3, [sp, #0x4]
   28e6c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28e70: e3530000     	cmp	r3, #0
   28e74: 10236a9b     	mlane	r3, r11, r10, r6
   28e78: 15839158     	strne	r9, [r3, #0x158]
   28e7c: eafffea8     	b	0x28924
   28e80: e2863915     	add	r3, r6, #344064
   28e84: e3a02001     	mov	r2, #1
   28e88: e5c4228c     	strb	r2, [r4, #0x28c]
   28e8c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28e90: e3530000     	cmp	r3, #0
   28e94: 0afffe43     	beq	0x287a8
   28e98: e0863009     	add	r3, r6, r9
   28e9c: e5c322d4     	strb	r2, [r3, #0x2d4]
   28ea0: e5942104     	ldr	r2, [r4, #0x104]
   28ea4: eafffe34     	b	0x2877c
   28ea8: e3530001     	cmp	r3, #1
   28eac: 8afffeb3     	bhi	0x28980
   28eb0: e5d4310d     	ldrb	r3, [r4, #0x10d]
   28eb4: e3530000     	cmp	r3, #0
   28eb8: 1affff7e     	bne	0x28cb8
   28ebc: e5d53125     	ldrb	r3, [r5, #0x125]
   28ec0: e3530000     	cmp	r3, #0
   28ec4: 1affff7b     	bne	0x28cb8
   28ec8: e5d5310d     	ldrb	r3, [r5, #0x10d]
   28ecc: e3530000     	cmp	r3, #0
   28ed0: 1affff78     	bne	0x28cb8
   28ed4: e5d530fd     	ldrb	r3, [r5, #0xfd]
   28ed8: e3530000     	cmp	r3, #0
   28edc: 1affff75     	bne	0x28cb8
   28ee0: eafffea6     	b	0x28980
   28ee4: e595311c     	ldr	r3, [r5, #0x11c]
   28ee8: e3530000     	cmp	r3, #0
   28eec: 1afffe36     	bne	0x287cc
   28ef0: e5953104     	ldr	r3, [r5, #0x104]
   28ef4: e3530000     	cmp	r3, #0
   28ef8: 1afffe33     	bne	0x287cc
   28efc: e2843a2a     	add	r3, r4, #172032
   28f00: e58d3008     	str	r3, [sp, #0x8]
   28f04: e2833e1f     	add	r3, r3, #496
   28f08: e58d3004     	str	r3, [sp, #0x4]
   28f0c: e1a00003     	mov	r0, r3
   28f10: eb0040c2     	bl	0x39220
   28f14: e3500000     	cmp	r0, #0
   28f18: 0a00010c     	beq	0x29350
   28f1c: e2863915     	add	r3, r6, #344064
   28f20: e58d300c     	str	r3, [sp, #0xc]
   28f24: e5d32abc     	ldrb	r2, [r3, #0xabc]
   28f28: e3520000     	cmp	r2, #0
   28f2c: 1a0000fa     	bne	0x2931c
   28f30: e59d0004     	ldr	r0, [sp, #0x4]
   28f34: eb0040b9     	bl	0x39220
   28f38: e3500000     	cmp	r0, #0
   28f3c: 0a00013d     	beq	0x29438
   28f40: e59d3008     	ldr	r3, [sp, #0x8]
   28f44: e3a02001     	mov	r2, #1
   28f48: e1a00006     	mov	r0, r6
   28f4c: e5d31497     	ldrb	r1, [r3, #0x497]
   28f50: eb010e8f     	bl	0x6c994
   28f54: e59d0004     	ldr	r0, [sp, #0x4]
   28f58: eb0047a3     	bl	0x3adec
   28f5c: eafffe1a     	b	0x287cc
   28f60: e59530f4     	ldr	r3, [r5, #0xf4]
   28f64: e3530000     	cmp	r3, #0
   28f68: 1afffe4e     	bne	0x288a8
   28f6c: e595311c     	ldr	r3, [r5, #0x11c]
   28f70: e2433001     	sub	r3, r3, #1
   28f74: e3530001     	cmp	r3, #1
   28f78: 8afffe4a     	bhi	0x288a8
   28f7c: e5953104     	ldr	r3, [r5, #0x104]
   28f80: e3530000     	cmp	r3, #0
   28f84: 1afffe47     	bne	0x288a8
   28f88: e3a03002     	mov	r3, #2
   28f8c: e1a00004     	mov	r0, r4
   28f90: eef10a00     	vmov.f32	s1, #4.000000e+00
   28f94: e584311c     	str	r3, [r4, #0x11c]
   28f98: eeb70a00     	vmov.f32	s0, #1.000000e+00
   28f9c: eb003a76     	bl	0x3797c
   28fa0: e3a01008     	mov	r1, #8
   28fa4: e1a00004     	mov	r0, r4
   28fa8: eb005885     	bl	0x3f1c4
   28fac: eafffe3d     	b	0x288a8
   28fb0: e59530f4     	ldr	r3, [r5, #0xf4]
   28fb4: e3530000     	cmp	r3, #0
   28fb8: 1afffe31     	bne	0x28884
   28fbc: e595311c     	ldr	r3, [r5, #0x11c]
   28fc0: e3530000     	cmp	r3, #0
   28fc4: 1afffe2e     	bne	0x28884
   28fc8: e5953104     	ldr	r3, [r5, #0x104]
   28fcc: e2433001     	sub	r3, r3, #1
   28fd0: e3530001     	cmp	r3, #1
   28fd4: 8afffe2a     	bhi	0x28884
   28fd8: e2863915     	add	r3, r6, #344064
   28fdc: e3a01014     	mov	r1, #20
   28fe0: e58d3004     	str	r3, [sp, #0x4]
   28fe4: e5d32abc     	ldrb	r2, [r3, #0xabc]
   28fe8: e3520000     	cmp	r2, #0
   28fec: 11a00008     	movne	r0, r8
   28ff0: 01a00004     	moveq	r0, r4
   28ff4: eb005872     	bl	0x3f1c4
   28ff8: e59d3004     	ldr	r3, [sp, #0x4]
   28ffc: e0216a9b     	mla	r1, r11, r10, r6
   29000: e5d32abc     	ldrb	r2, [r3, #0xabc]
   29004: e2822001     	add	r2, r2, #1
   29008: e5842110     	str	r2, [r4, #0x110]
   2900c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29010: e2833001     	add	r3, r3, #1
   29014: e5813158     	str	r3, [r1, #0x158]
   29018: e5d430fd     	ldrb	r3, [r4, #0xfd]
   2901c: e3530000     	cmp	r3, #0
   29020: 1594311c     	ldrne	r3, [r4, #0x11c]
   29024: 1afffe91     	bne	0x28a70
   29028: eafffe15     	b	0x28884
   2902c: 00 00 fa 43  	.word	0x43fa0000
   29030: e5d530fd     	ldrb	r3, [r5, #0xfd]
   29034: e3530000     	cmp	r3, #0
   29038: 0a000045     	beq	0x29154
   2903c: e595311c     	ldr	r3, [r5, #0x11c]
   29040: e3530000     	cmp	r3, #0
   29044: 1a000042     	bne	0x29154
   29048: e5953104     	ldr	r3, [r5, #0x104]
   2904c: e3530000     	cmp	r3, #0
   29050: 1a00003f     	bne	0x29154
   29054: e2863915     	add	r3, r6, #344064
   29058: e3a01013     	mov	r1, #19
   2905c: e58d3004     	str	r3, [sp, #0x4]
   29060: e5d30abc     	ldrb	r0, [r3, #0xabc]
   29064: e3500000     	cmp	r0, #0
   29068: 11a00008     	movne	r0, r8
   2906c: 01a00004     	moveq	r0, r4
   29070: eb005853     	bl	0x3f1c4
   29074: e59d3004     	ldr	r3, [sp, #0x4]
   29078: e3a02001     	mov	r2, #1
   2907c: e5842110     	str	r2, [r4, #0x110]
   29080: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29084: e3530000     	cmp	r3, #0
   29088: 102a6a9b     	mlane	r10, r11, r10, r6
   2908c: 158a2158     	strne	r2, [r10, #0x158]
   29090: e594211c     	ldr	r2, [r4, #0x11c]
   29094: eafffed1     	b	0x28be0
   29098: e5d4210d     	ldrb	r2, [r4, #0x10d]
   2909c: e3520000     	cmp	r2, #0
   290a0: 0affff2c     	beq	0x28d58
   290a4: e59520f4     	ldr	r2, [r5, #0xf4]
   290a8: e3520000     	cmp	r2, #0
   290ac: 1affff29     	bne	0x28d58
   290b0: e595211c     	ldr	r2, [r5, #0x11c]
   290b4: e3520000     	cmp	r2, #0
   290b8: 1affff26     	bne	0x28d58
   290bc: e5952104     	ldr	r2, [r5, #0x104]
   290c0: e3520000     	cmp	r2, #0
   290c4: 1affff23     	bne	0x28d58
   290c8: e3a01017     	mov	r1, #23
   290cc: e1a00004     	mov	r0, r4
   290d0: e2863915     	add	r3, r6, #344064
   290d4: e58d3004     	str	r3, [sp, #0x4]
   290d8: eb005839     	bl	0x3f1c4
   290dc: e59d3004     	ldr	r3, [sp, #0x4]
   290e0: e5d33abc     	ldrb	r3, [r3, #0xabc]
   290e4: e3530000     	cmp	r3, #0
   290e8: 1a0000e1     	bne	0x29474
   290ec: e594311c     	ldr	r3, [r4, #0x11c]
   290f0: e59410f4     	ldr	r1, [r4, #0xf4]
   290f4: e1a02003     	mov	r2, r3
   290f8: e3510000     	cmp	r1, #0
   290fc: 0afffe16     	beq	0x2895c
   29100: eafffeb5     	b	0x28bdc
   29104: e3a01007     	mov	r1, #7
   29108: e1a00004     	mov	r0, r4
   2910c: e2863915     	add	r3, r6, #344064
   29110: e58d3004     	str	r3, [sp, #0x4]
   29114: eb00582a     	bl	0x3f1c4
   29118: e59d3004     	ldr	r3, [sp, #0x4]
   2911c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29120: e3530000     	cmp	r3, #0
   29124: 0afffdea     	beq	0x288d4
   29128: e3a01007     	mov	r1, #7
   2912c: e1a00005     	mov	r0, r5
   29130: eb005823     	bl	0x3f1c4
   29134: eafffde6     	b	0x288d4
   29138: e1a00005     	mov	r0, r5
   2913c: e3a01009     	mov	r1, #9
   29140: eb00581f     	bl	0x3f1c4
   29144: e3a01009     	mov	r1, #9
   29148: e1a00004     	mov	r0, r4
   2914c: eb00581c     	bl	0x3f1c4
   29150: eaffff0b     	b	0x28d84
   29154: e5d430fd     	ldrb	r3, [r4, #0xfd]
   29158: e3530000     	cmp	r3, #0
   2915c: 1afffeaa     	bne	0x28c0c
   29160: eafffe06     	b	0x28980
   29164: e2843a2a     	add	r3, r4, #172032
   29168: e58d3008     	str	r3, [sp, #0x8]
   2916c: e2832e1f     	add	r2, r3, #496
   29170: e58d2004     	str	r2, [sp, #0x4]
   29174: e1a00002     	mov	r0, r2
   29178: eb004028     	bl	0x39220
   2917c: e59d2004     	ldr	r2, [sp, #0x4]
   29180: e3500000     	cmp	r0, #0
   29184: 0afffe03     	beq	0x28998
   29188: e2863915     	add	r3, r6, #344064
   2918c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29190: e3530000     	cmp	r3, #0
   29194: 0a000055     	beq	0x292f0
   29198: e59d0000     	ldr	r0, [sp]
   2919c: eb0040f0     	bl	0x39564
   291a0: eafffe26     	b	0x28a40
   291a4: e2843a2a     	add	r3, r4, #172032
   291a8: e58d3004     	str	r3, [sp, #0x4]
   291ac: e2830e1f     	add	r0, r3, #496
   291b0: eb00401a     	bl	0x39220
   291b4: e59d3004     	ldr	r3, [sp, #0x4]
   291b8: e3500000     	cmp	r0, #0
   291bc: 0afffe25     	beq	0x28a58
   291c0: e2830fbe     	add	r0, r3, #760
   291c4: e2863915     	add	r3, r6, #344064
   291c8: e58d3004     	str	r3, [sp, #0x4]
   291cc: eb004794     	bl	0x3b024
   291d0: e59420e8     	ldr	r2, [r4, #0xe8]
   291d4: e59d3004     	ldr	r3, [sp, #0x4]
   291d8: e592208c     	ldr	r2, [r2, #0x8c]
   291dc: e2522000     	subs	r2, r2, #0
   291e0: 13a02001     	movne	r2, #1
   291e4: e5842110     	str	r2, [r4, #0x110]
   291e8: e5d33abc     	ldrb	r3, [r3, #0xabc]
   291ec: e3530000     	cmp	r3, #0
   291f0: 0a000003     	beq	0x29204
   291f4: e59530e8     	ldr	r3, [r5, #0xe8]
   291f8: e593308c     	ldr	r3, [r3, #0x8c]
   291fc: e2533000     	subs	r3, r3, #0
   29200: 13a03001     	movne	r3, #1
   29204: e0226a9b     	mla	r2, r11, r10, r6
   29208: e5823158     	str	r3, [r2, #0x158]
   2920c: eafffe11     	b	0x28a58
   29210: e2843a2a     	add	r3, r4, #172032
   29214: e58d3008     	str	r3, [sp, #0x8]
   29218: e2830e1f     	add	r0, r3, #496
   2921c: eb003ff8     	bl	0x39204
   29220: e3500000     	cmp	r0, #0
   29224: 1a000075     	bne	0x29400
   29228: e2890a2a     	add	r0, r9, #172032
   2922c: e2800e1f     	add	r0, r0, #496
   29230: e0880000     	add	r0, r8, r0
   29234: eb003ff2     	bl	0x39204
   29238: e3500000     	cmp	r0, #0
   2923c: 1a00006f     	bne	0x29400
   29240: e2863915     	add	r3, r6, #344064
   29244: e58d3004     	str	r3, [sp, #0x4]
   29248: e5d32abc     	ldrb	r2, [r3, #0xabc]
   2924c: e3520000     	cmp	r2, #0
   29250: 1a00008b     	bne	0x29484
   29254: e59d3008     	ldr	r3, [sp, #0x8]
   29258: e3a02001     	mov	r2, #1
   2925c: e1a00006     	mov	r0, r6
   29260: e5d31497     	ldrb	r1, [r3, #0x497]
   29264: eb010dca     	bl	0x6c994
   29268: e3a03001     	mov	r3, #1
   2926c: e3a0100e     	mov	r1, #14
   29270: e1a00004     	mov	r0, r4
   29274: e5843110     	str	r3, [r4, #0x110]
   29278: eb0057d1     	bl	0x3f1c4
   2927c: e5943104     	ldr	r3, [r4, #0x104]
   29280: e2433001     	sub	r3, r3, #1
   29284: e3530001     	cmp	r3, #1
   29288: 9afffedf     	bls	0x28e0c
   2928c: eafffda4     	b	0x28924
   29290: e2840a2a     	add	r0, r4, #172032
   29294: e2800e1f     	add	r0, r0, #496
   29298: eb003fd9     	bl	0x39204
   2929c: e3500000     	cmp	r0, #0
   292a0: 1afffd9b     	bne	0x28914
   292a4: e2890a2a     	add	r0, r9, #172032
   292a8: e2800e1f     	add	r0, r0, #496
   292ac: e0880000     	add	r0, r8, r0
   292b0: eb003fd3     	bl	0x39204
   292b4: e3500000     	cmp	r0, #0
   292b8: 1afffd95     	bne	0x28914
   292bc: e3a0100d     	mov	r1, #13
   292c0: e1a00004     	mov	r0, r4
   292c4: e2863915     	add	r3, r6, #344064
   292c8: e58d3004     	str	r3, [sp, #0x4]
   292cc: eb0057bc     	bl	0x3f1c4
   292d0: e59d3004     	ldr	r3, [sp, #0x4]
   292d4: e3a02001     	mov	r2, #1
   292d8: e5842110     	str	r2, [r4, #0x110]
   292dc: e5d33abc     	ldrb	r3, [r3, #0xabc]
   292e0: e3530000     	cmp	r3, #0
   292e4: 10236a9b     	mlane	r3, r11, r10, r6
   292e8: 15832158     	strne	r2, [r3, #0x158]
   292ec: eafffd88     	b	0x28914
   292f0: e1a00002     	mov	r0, r2
   292f4: e58d2004     	str	r2, [sp, #0x4]
   292f8: eb003fc8     	bl	0x39220
   292fc: e3500000     	cmp	r0, #0
   29300: 02890a2a     	addeq	r0, r9, #172032
   29304: 02800e1f     	addeq	r0, r0, #496
   29308: 159d2004     	ldrne	r2, [sp, #0x4]
   2930c: 00880000     	addeq	r0, r8, r0
   29310: 11a00002     	movne	r0, r2
   29314: eb004092     	bl	0x39564
   29318: eafffdc8     	b	0x28a40
   2931c: e2861a2a     	add	r1, r6, #172032
   29320: e3a02001     	mov	r2, #1
   29324: e1a00006     	mov	r0, r6
   29328: e5d114df     	ldrb	r1, [r1, #0x4df]
   2932c: eb010d98     	bl	0x6c994
   29330: e59d300c     	ldr	r3, [sp, #0xc]
   29334: e3a02001     	mov	r2, #1
   29338: e1a00006     	mov	r0, r6
   2933c: e5d31a17     	ldrb	r1, [r3, #0xa17]
   29340: eb010d93     	bl	0x6c994
   29344: e59d0000     	ldr	r0, [sp]
   29348: eb0046a7     	bl	0x3adec
   2934c: eafffd1e     	b	0x287cc
   29350: e2890a2a     	add	r0, r9, #172032
   29354: e2800e1f     	add	r0, r0, #496
   29358: e0880000     	add	r0, r8, r0
   2935c: eb003faf     	bl	0x39220
   29360: e3500000     	cmp	r0, #0
   29364: 1afffeec     	bne	0x28f1c
   29368: e59d3008     	ldr	r3, [sp, #0x8]
   2936c: e2833048     	add	r3, r3, #72
   29370: e58d3004     	str	r3, [sp, #0x4]
   29374: e1a00003     	mov	r0, r3
   29378: eb003f91     	bl	0x391c4
   2937c: e59d3004     	ldr	r3, [sp, #0x4]
   29380: e2501000     	subs	r1, r0, #0
   29384: 1a00005c     	bne	0x294fc
   29388: e59420e8     	ldr	r2, [r4, #0xe8]
   2938c: e59230b0     	ldr	r3, [r2, #0xb0]
   29390: e5933018     	ldr	r3, [r3, #0x18]
   29394: e3530002     	cmp	r3, #2
   29398: 1a00004b     	bne	0x294cc
   2939c: e5943278     	ldr	r3, [r4, #0x278]
   293a0: e5933004     	ldr	r3, [r3, #0x4]
   293a4: e3530000     	cmp	r3, #0
   293a8: 13530003     	cmpne	r3, #3
   293ac: 1a000055     	bne	0x29508
   293b0: e5d430ac     	ldrb	r3, [r4, #0xac]
   293b4: e3530000     	cmp	r3, #0
   293b8: 03a01001     	moveq	r1, #1
   293bc: 0a000058     	beq	0x29524
   293c0: e3a03000     	mov	r3, #0
   293c4: e5c23088     	strb	r3, [r2, #0x88]
   293c8: eafffcff     	b	0x287cc
   293cc: e1a00002     	mov	r0, r2
   293d0: e58d3004     	str	r3, [sp, #0x4]
   293d4: eb003fa0     	bl	0x3925c
   293d8: e59d3004     	ldr	r3, [sp, #0x4]
   293dc: e5d32abc     	ldrb	r2, [r3, #0xabc]
   293e0: e3520000     	cmp	r2, #0
   293e4: 0afffdc8     	beq	0x28b0c
   293e8: e2890a2a     	add	r0, r9, #172032
   293ec: e2800fbe     	add	r0, r0, #760
   293f0: e0880000     	add	r0, r8, r0
   293f4: eb003f98     	bl	0x3925c
   293f8: e59d3004     	ldr	r3, [sp, #0x4]
   293fc: eafffdc2     	b	0x28b0c
   29400: ed1f0af7     	vldr	s0, [pc, #-988]         @ 0x2902c ; float 500
   29404: e1a00004     	mov	r0, r4
   29408: eef10a08     	vmov.f32	s1, #6.000000e+00
   2940c: e2863915     	add	r3, r6, #344064
   29410: e58d3004     	str	r3, [sp, #0x4]
   29414: eb003958     	bl	0x3797c
   29418: e59d3004     	ldr	r3, [sp, #0x4]
   2941c: e3a02001     	mov	r2, #1
   29420: e5842110     	str	r2, [r4, #0x110]
   29424: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29428: e3530000     	cmp	r3, #0
   2942c: 10236a9b     	mlane	r3, r11, r10, r6
   29430: 15832158     	strne	r2, [r3, #0x158]
   29434: eaffff90     	b	0x2927c
   29438: e0236a9b     	mla	r3, r11, r10, r6
   2943c: e3a02001     	mov	r2, #1
   29440: e1a00006     	mov	r0, r6
   29444: e2833ba9     	add	r3, r3, #173056
   29448: e5d310df     	ldrb	r1, [r3, #0xdf]
   2944c: eb010d50     	bl	0x6c994
   29450: e2890a2a     	add	r0, r9, #172032
   29454: e2800e1f     	add	r0, r0, #496
   29458: e0880000     	add	r0, r8, r0
   2945c: eb004662     	bl	0x3adec
   29460: eafffcd9     	b	0x287cc
   29464: e3a01006     	mov	r1, #6
   29468: e1a00004     	mov	r0, r4
   2946c: eb005754     	bl	0x3f1c4
   29470: eafffd03     	b	0x28884
   29474: e3a01017     	mov	r1, #23
   29478: e1a00005     	mov	r0, r5
   2947c: eb005750     	bl	0x3f1c4
   29480: eaffff19     	b	0x290ec
   29484: e2869a2a     	add	r9, r6, #172032
   29488: e3a02001     	mov	r2, #1
   2948c: e1a00006     	mov	r0, r6
   29490: e5d914df     	ldrb	r1, [r9, #0x4df]
   29494: eb010d3e     	bl	0x6c994
   29498: e3a03001     	mov	r3, #1
   2949c: e1a02003     	mov	r2, r3
   294a0: e5863158     	str	r3, [r6, #0x158]
   294a4: e59d3004     	ldr	r3, [sp, #0x4]
   294a8: e1a00006     	mov	r0, r6
   294ac: e5d31a17     	ldrb	r1, [r3, #0xa17]
   294b0: eb010d37     	bl	0x6c994
   294b4: e3a03001     	mov	r3, #1
   294b8: e3a0100e     	mov	r1, #14
   294bc: e1a00008     	mov	r0, r8
   294c0: e5893690     	str	r3, [r9, #0x690]
   294c4: eb00573e     	bl	0x3f1c4
   294c8: eaffff6b     	b	0x2927c
   294cc: e1a00004     	mov	r0, r4
   294d0: e2863915     	add	r3, r6, #344064
   294d4: e58d3004     	str	r3, [sp, #0x4]
   294d8: eb005739     	bl	0x3f1c4
   294dc: e59d3004     	ldr	r3, [sp, #0x4]
   294e0: e5d33abc     	ldrb	r3, [r3, #0xabc]
   294e4: e3530000     	cmp	r3, #0
   294e8: 0afffcb7     	beq	0x287cc
   294ec: e3a01000     	mov	r1, #0
   294f0: e1a00005     	mov	r0, r5
   294f4: eb005732     	bl	0x3f1c4
   294f8: eafffcb3     	b	0x287cc
   294fc: e1a00003     	mov	r0, r3
   29500: eb003e16     	bl	0x38d60
   29504: eafffcb0     	b	0x287cc
   29508: e2433001     	sub	r3, r3, #1
   2950c: e3530001     	cmp	r3, #1
   29510: 8afffcad     	bhi	0x287cc
   29514: e5d430ac     	ldrb	r3, [r4, #0xac]
   29518: e3530000     	cmp	r3, #0
   2951c: 1affffa7     	bne	0x293c0
   29520: e3a01002     	mov	r1, #2
   29524: ed9f0a19     	vldr	s0, [pc, #100]          @ 0x29590 ; float 250
   29528: e1a00004     	mov	r0, r4
   2952c: eef10a04     	vmov.f32	s1, #5.000000e+00
   29530: eb003924     	bl	0x379c8
   29534: e59430e8     	ldr	r3, [r4, #0xe8]
   29538: e3a02001     	mov	r2, #1
   2953c: e5c32088     	strb	r2, [r3, #0x88]
   29540: eafffca1     	b	0x287cc
   29544: e3a01001     	mov	r1, #1
   29548: e1a00004     	mov	r0, r4
   2954c: e2863915     	add	r3, r6, #344064
   29550: e58d3004     	str	r3, [sp, #0x4]
   29554: eb00571a     	bl	0x3f1c4
   29558: e59d3004     	ldr	r3, [sp, #0x4]
   2955c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   29560: e3530000     	cmp	r3, #0
   29564: 0afffd35     	beq	0x28a40
   29568: e3a01001     	mov	r1, #1
   2956c: e1a00005     	mov	r0, r5
   29570: eb005713     	bl	0x3f1c4
   29574: eafffd31     	b	0x28a40
   29578: e59d0010     	ldr	r0, [sp, #0x10]
   2957c: e28d3018     	add	r3, sp, #24
   29580: e1500003     	cmp	r0, r3
   29584: 0a000000     	beq	0x2958c
   29588: ebffb22c     	bl	0x15e40    @ imm = #-0x13750 ; _ZdlPv
   2958c: ebffb273     	bl	0x15f60    @ imm = #-0x13634 ; __cxa_end_cleanup
   29590: 00 00 7a 43  	.word	0x437a0000
