; lubadh::TapManager::activate_new(int, unsigned int, bool)
; VA 0x4eea8 size 788

   4eea8: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4eeac: e2807eaa     	add	r7, r0, #2720
   4eeb0: e1a0b002     	mov	r11, r2
   4eeb4: e24dd00c     	sub	sp, sp, #12
   4eeb8: e1a05000     	mov	r5, r0
   4eebc: e1a08001     	mov	r8, r1
   4eec0: e1a09003     	mov	r9, r3
   4eec4: e1a04000     	mov	r4, r0
   4eec8: e1a02007     	mov	r2, r7
   4eecc: e5d43000     	ldrb	r3, [r4]
   4eed0: e3530000     	cmp	r3, #0
   4eed4: 1a00001d     	bne	0x4ef50
   4eed8: e5d43084     	ldrb	r3, [r4, #0x84]
   4eedc: e3530000     	cmp	r3, #0
   4eee0: 1a00001a     	bne	0x4ef50
   4eee4: e5d43108     	ldrb	r3, [r4, #0x108]
   4eee8: e3530000     	cmp	r3, #0
   4eeec: 1a000017     	bne	0x4ef50
   4eef0: e5d4318c     	ldrb	r3, [r4, #0x18c]
   4eef4: e3530000     	cmp	r3, #0
   4eef8: 1a000014     	bne	0x4ef50
   4eefc: e5d43210     	ldrb	r3, [r4, #0x210]
   4ef00: e3530000     	cmp	r3, #0
   4ef04: 1a000011     	bne	0x4ef50
   4ef08: e1570004     	cmp	r7, r4
   4ef0c: 0a00004e     	beq	0x4f04c
   4ef10: e1a01008     	mov	r1, r8
   4ef14: e1a00004     	mov	r0, r4
   4ef18: ebfffd0e     	bl	0x4e358
   4ef1c: e2852eaa     	add	r2, r5, #2720
   4ef20: e282200c     	add	r2, r2, #12
   4ef24: e2850eab     	add	r0, r5, #2736
   4ef28: e0422007     	sub	r2, r2, r7
   4ef2c: e1a01007     	mov	r1, r7
   4ef30: e0400002     	sub	r0, r0, r2
   4ef34: ebff1aaa     	bl	0x159e4    @ imm = #-0x39558 ; memmove
   4ef38: e5940294     	ldr	r0, [r4, #0x294]
   4ef3c: e5854aa0     	str	r4, [r5, #0xaa0]
   4ef40: e3500000     	cmp	r0, #0
   4ef44: 01a00004     	moveq	r0, r4
   4ef48: e28dd00c     	add	sp, sp, #12
   4ef4c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4ef50: e5d432a8     	ldrb	r3, [r4, #0x2a8]
   4ef54: e3530000     	cmp	r3, #0
   4ef58: 1a00000c     	bne	0x4ef90
   4ef5c: e5d4332c     	ldrb	r3, [r4, #0x32c]
   4ef60: e3530000     	cmp	r3, #0
   4ef64: 1a000009     	bne	0x4ef90
   4ef68: e5d433b0     	ldrb	r3, [r4, #0x3b0]
   4ef6c: e3530000     	cmp	r3, #0
   4ef70: 1a000006     	bne	0x4ef90
   4ef74: e5d43434     	ldrb	r3, [r4, #0x434]
   4ef78: e3530000     	cmp	r3, #0
   4ef7c: 1a000003     	bne	0x4ef90
   4ef80: e5d434b8     	ldrb	r3, [r4, #0x4b8]
   4ef84: e3530000     	cmp	r3, #0
   4ef88: 02844faa     	addeq	r4, r4, #680
   4ef8c: 0affffdd     	beq	0x4ef08
   4ef90: e5d43550     	ldrb	r3, [r4, #0x550]
   4ef94: e3530000     	cmp	r3, #0
   4ef98: 1a00000c     	bne	0x4efd0
   4ef9c: e5d435d4     	ldrb	r3, [r4, #0x5d4]
   4efa0: e3530000     	cmp	r3, #0
   4efa4: 1a000009     	bne	0x4efd0
   4efa8: e5d43658     	ldrb	r3, [r4, #0x658]
   4efac: e3530000     	cmp	r3, #0
   4efb0: 1a000006     	bne	0x4efd0
   4efb4: e5d436dc     	ldrb	r3, [r4, #0x6dc]
   4efb8: e3530000     	cmp	r3, #0
   4efbc: 1a000003     	bne	0x4efd0
   4efc0: e5d43760     	ldrb	r3, [r4, #0x760]
   4efc4: e3530000     	cmp	r3, #0
   4efc8: 02844e55     	addeq	r4, r4, #1360
   4efcc: 0affffcd     	beq	0x4ef08
   4efd0: e5d437f8     	ldrb	r3, [r4, #0x7f8]
   4efd4: e3530000     	cmp	r3, #0
   4efd8: 1a00000d     	bne	0x4f014
   4efdc: e5d4387c     	ldrb	r3, [r4, #0x87c]
   4efe0: e3530000     	cmp	r3, #0
   4efe4: 1a00000a     	bne	0x4f014
   4efe8: e5d43900     	ldrb	r3, [r4, #0x900]
   4efec: e3530000     	cmp	r3, #0
   4eff0: 1a000007     	bne	0x4f014
   4eff4: e5d43984     	ldrb	r3, [r4, #0x984]
   4eff8: e3530000     	cmp	r3, #0
   4effc: 1a000004     	bne	0x4f014
   4f000: e5d43a08     	ldrb	r3, [r4, #0xa08]
   4f004: e3530000     	cmp	r3, #0
   4f008: 02844e7f     	addeq	r4, r4, #2032
   4f00c: 02844008     	addeq	r4, r4, #8
   4f010: 0affffbc     	beq	0x4ef08
   4f014: e2844eaa     	add	r4, r4, #2720
   4f018: e1540002     	cmp	r4, r2
   4f01c: 1affffaa     	bne	0x4eecc
   4f020: e0472004     	sub	r2, r7, r4
   4f024: e30f1cfd     	movw	r1, #0xfcfd
   4f028: e34f1cfc     	movt	r1, #0xfcfc
   4f02c: e1a021c2     	asr	r2, r2, #3
   4f030: e0020291     	mul	r2, r1, r2
   4f034: e3520002     	cmp	r2, #2
   4f038: 0a000051     	beq	0x4f184
   4f03c: e3520003     	cmp	r2, #3
   4f040: 0a00003b     	beq	0x4f134
   4f044: e3520001     	cmp	r2, #1
   4f048: 0a000027     	beq	0x4f0ec
   4f04c: e3a04faa     	mov	r4, #680
   4f050: e1a00005     	mov	r0, r5
   4f054: ebfffb74     	bl	0x4de2c
   4f058: e0245094     	mla	r4, r4, r0, r5
   4f05c: e284afa5     	add	r10, r4, #660
   4f060: e1a06004     	mov	r6, r4
   4f064: e5d62000     	ldrb	r2, [r6]
   4f068: e3520000     	cmp	r2, #0
   4f06c: 0a000013     	beq	0x4f0c0
   4f070: e5d6304c     	ldrb	r3, [r6, #0x4c]
   4f074: e3530000     	cmp	r3, #0
   4f078: 1a000010     	bne	0x4f0c0
   4f07c: e5d62014     	ldrb	r2, [r6, #0x14]
   4f080: e1a03009     	mov	r3, r9
   4f084: e3520000     	cmp	r2, #0
   4f088: e5962004     	ldr	r2, [r6, #0x4]
   4f08c: 0a000004     	beq	0x4f0a4
   4f090: edd67a06     	vldr	s15, [r6, #24]
   4f094: eef57ac0     	vcmpe.f32	s15, #0
   4f098: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4f09c: a3a03001     	movge	r3, #1
   4f0a0: b3a03000     	movlt	r3, #0
   4f0a4: e58d3004     	str	r3, [sp, #0x4]
   4f0a8: e3a01002     	mov	r1, #2
   4f0ac: e3a03000     	mov	r3, #0
   4f0b0: e1a00006     	mov	r0, r6
   4f0b4: e58d3000     	str	r3, [sp]
   4f0b8: e1a0300b     	mov	r3, r11
   4f0bc: ebfffcea     	bl	0x4e46c
   4f0c0: e2866084     	add	r6, r6, #132
   4f0c4: e15a0006     	cmp	r10, r6
   4f0c8: 1affffe5     	bne	0x4f064
   4f0cc: eaffff8f     	b	0x4ef10
   4f0d0: e1500001     	cmp	r0, r1
   4f0d4: 0a000003     	beq	0x4f0e8
   4f0d8: e2813084     	add	r3, r1, #132
   4f0dc: e2811f42     	add	r1, r1, #264
   4f0e0: e1500003     	cmp	r0, r3
   4f0e4: 1afffff9     	bne	0x4f0d0
   4f0e8: e2844faa     	add	r4, r4, #680
   4f0ec: e2840fa5     	add	r0, r4, #660
   4f0f0: e2842084     	add	r2, r4, #132
   4f0f4: e5523084     	ldrb	r3, [r2, #-0x84]
   4f0f8: e282c084     	add	r12, r2, #132
   4f0fc: e1a01002     	mov	r1, r2
   4f100: e3530000     	cmp	r3, #0
   4f104: 1a000003     	bne	0x4f118
   4f108: e1500002     	cmp	r0, r2
   4f10c: 0affff7d     	beq	0x4ef08
   4f110: e1a0200c     	mov	r2, r12
   4f114: eafffff6     	b	0x4f0f4
   4f118: e1510000     	cmp	r1, r0
   4f11c: 0affffca     	beq	0x4f04c
   4f120: e2813084     	add	r3, r1, #132
   4f124: e2811f42     	add	r1, r1, #264
   4f128: e1500003     	cmp	r0, r3
   4f12c: 1afffff9     	bne	0x4f118
   4f130: eaffffc5     	b	0x4f04c
   4f134: e5d43000     	ldrb	r3, [r4]
   4f138: e2842084     	add	r2, r4, #132
   4f13c: e2841fa5     	add	r1, r4, #660
   4f140: e2820084     	add	r0, r2, #132
   4f144: e3530000     	cmp	r3, #0
   4f148: 1a000006     	bne	0x4f168
   4f14c: e1510002     	cmp	r1, r2
   4f150: 0affff6c     	beq	0x4ef08
   4f154: e1a02000     	mov	r2, r0
   4f158: e2820084     	add	r0, r2, #132
   4f15c: e5523084     	ldrb	r3, [r2, #-0x84]
   4f160: e3530000     	cmp	r3, #0
   4f164: 0afffff8     	beq	0x4f14c
   4f168: e1520001     	cmp	r2, r1
   4f16c: 0a000003     	beq	0x4f180
   4f170: e2823084     	add	r3, r2, #132
   4f174: e2822f42     	add	r2, r2, #264
   4f178: e1510003     	cmp	r1, r3
   4f17c: 1a00000b     	bne	0x4f1b0
   4f180: e2844faa     	add	r4, r4, #680
   4f184: e2840fa5     	add	r0, r4, #660
   4f188: e2842084     	add	r2, r4, #132
   4f18c: e5523084     	ldrb	r3, [r2, #-0x84]
   4f190: e282c084     	add	r12, r2, #132
   4f194: e1a01002     	mov	r1, r2
   4f198: e3530000     	cmp	r3, #0
   4f19c: 1affffcb     	bne	0x4f0d0
   4f1a0: e1500002     	cmp	r0, r2
   4f1a4: 0affff57     	beq	0x4ef08
   4f1a8: e1a0200c     	mov	r2, r12
   4f1ac: eafffff6     	b	0x4f18c
   4f1b0: e1510002     	cmp	r1, r2
   4f1b4: 1affffed     	bne	0x4f170
   4f1b8: eafffff0     	b	0x4f180
