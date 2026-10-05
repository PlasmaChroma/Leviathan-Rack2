; lubadh::Application::interpretJacks()
; VA 0x284b0 size 644

   284b0: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
   284b4: e2808048     	add	r8, r0, #72
   284b8: e30a9538     	movw	r9, #0xa538
   284bc: e3409002     	movt	r9, #0x2
   284c0: e1a06000     	mov	r6, r0
   284c4: e3a07000     	mov	r7, #0
   284c8: e1a04008     	mov	r4, r8
   284cc: e2273001     	eor	r3, r7, #1
   284d0: e5d4219d     	ldrb	r2, [r4, #0x19d]
   284d4: e3520000     	cmp	r2, #0
   284d8: e0258399     	mla	r5, r9, r3, r8
   284dc: 0a000008     	beq	0x28504
   284e0: e59430e8     	ldr	r3, [r4, #0xe8]
   284e4: e59330b0     	ldr	r3, [r3, #0xb0]
   284e8: e593a018     	ldr	r10, [r3, #0x18]
   284ec: e35a0001     	cmp	r10, #1
   284f0: 0a00005c     	beq	0x28668
   284f4: e35a0002     	cmp	r10, #2
   284f8: 0a000065     	beq	0x28694
   284fc: e35a0000     	cmp	r10, #0
   28500: 0a000046     	beq	0x28620
   28504: e5d4319e     	ldrb	r3, [r4, #0x19e]
   28508: e3530000     	cmp	r3, #0
   2850c: 0a000009     	beq	0x28538
   28510: e286b915     	add	r11, r6, #344064
   28514: e5db3abc     	ldrb	r3, [r11, #0xabc]
   28518: e3530000     	cmp	r3, #0
   2851c: 0a000049     	beq	0x28648
   28520: e5943000     	ldr	r3, [r4]
   28524: e3530000     	cmp	r3, #0
   28528: 0a000026     	beq	0x285c8
   2852c: e5d4328b     	ldrb	r3, [r4, #0x28b]
   28530: e3530000     	cmp	r3, #0
   28534: 1a00002f     	bne	0x285f8
   28538: e5d431ad     	ldrb	r3, [r4, #0x1ad]
   2853c: e3530000     	cmp	r3, #0
   28540: 0a00000c     	beq	0x28578
   28544: e59430e8     	ldr	r3, [r4, #0xe8]
   28548: e1a00004     	mov	r0, r4
   2854c: e59330b0     	ldr	r3, [r3, #0xb0]
   28550: e5933008     	ldr	r3, [r3, #0x8]
   28554: e3530001     	cmp	r3, #1
   28558: 03a0a017     	moveq	r10, #23
   2855c: 13a0a001     	movne	r10, #1
   28560: e1a0100a     	mov	r1, r10
   28564: eb005b16     	bl	0x3f1c4
   28568: e2863915     	add	r3, r6, #344064
   2856c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   28570: e3530000     	cmp	r3, #0
   28574: 1a000037     	bne	0x28658
   28578: e5d431bd     	ldrb	r3, [r4, #0x1bd]
   2857c: e3530000     	cmp	r3, #0
   28580: 1a000005     	bne	0x2859c
   28584: e3570001     	cmp	r7, #1
   28588: e2844ba9     	add	r4, r4, #173056
   2858c: e2844f4e     	add	r4, r4, #312
   28590: 08bd8ff8     	popeq	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
   28594: e3a07001     	mov	r7, #1
   28598: eaffffcb     	b	0x284cc
   2859c: e3a01002     	mov	r1, #2
   285a0: e1a00004     	mov	r0, r4
   285a4: eb005b06     	bl	0x3f1c4
   285a8: e2863915     	add	r3, r6, #344064
   285ac: e5d33abc     	ldrb	r3, [r3, #0xabc]
   285b0: e3530000     	cmp	r3, #0
   285b4: 0afffff2     	beq	0x28584
   285b8: e1a00005     	mov	r0, r5
   285bc: e3a01002     	mov	r1, #2
   285c0: eb005aff     	bl	0x3f1c4
   285c4: eaffffee     	b	0x28584
   285c8: e3a0100c     	mov	r1, #12
   285cc: e1a00005     	mov	r0, r5
   285d0: eb005afb     	bl	0x3f1c4
   285d4: e3a0100c     	mov	r1, #12
   285d8: e1a00004     	mov	r0, r4
   285dc: eb005af8     	bl	0x3f1c4
   285e0: e5d4319e     	ldrb	r3, [r4, #0x19e]
   285e4: e3530000     	cmp	r3, #0
   285e8: 0affffd2     	beq	0x28538
   285ec: e5d4328b     	ldrb	r3, [r4, #0x28b]
   285f0: e3530000     	cmp	r3, #0
   285f4: 0affffcf     	beq	0x28538
   285f8: e3a01005     	mov	r1, #5
   285fc: e1a00004     	mov	r0, r4
   28600: eb005aef     	bl	0x3f1c4
   28604: e5db3abc     	ldrb	r3, [r11, #0xabc]
   28608: e3530000     	cmp	r3, #0
   2860c: 0affffc9     	beq	0x28538
   28610: e3a01005     	mov	r1, #5
   28614: e1a00005     	mov	r0, r5
   28618: eb005ae9     	bl	0x3f1c4
   2861c: eaffffc5     	b	0x28538
   28620: e286b915     	add	r11, r6, #344064
   28624: e1a0100a     	mov	r1, r10
   28628: e1a00004     	mov	r0, r4
   2862c: eb005ae4     	bl	0x3f1c4
   28630: e5db3abc     	ldrb	r3, [r11, #0xabc]
   28634: e3530000     	cmp	r3, #0
   28638: 1a000021     	bne	0x286c4
   2863c: e5d4319e     	ldrb	r3, [r4, #0x19e]
   28640: e3530000     	cmp	r3, #0
   28644: 0affffbb     	beq	0x28538
   28648: e3a0100c     	mov	r1, #12
   2864c: e1a00004     	mov	r0, r4
   28650: eb005adb     	bl	0x3f1c4
   28654: eaffffe1     	b	0x285e0
   28658: e1a0100a     	mov	r1, r10
   2865c: e1a00005     	mov	r0, r5
   28660: eb005ad7     	bl	0x3f1c4
   28664: eaffffc3     	b	0x28578
   28668: e286b915     	add	r11, r6, #344064
   2866c: e3a01004     	mov	r1, #4
   28670: e1a00004     	mov	r0, r4
   28674: eb005ad2     	bl	0x3f1c4
   28678: e5db3abc     	ldrb	r3, [r11, #0xabc]
   2867c: e3530000     	cmp	r3, #0
   28680: 0affffed     	beq	0x2863c
   28684: e3a01004     	mov	r1, #4
   28688: e1a00005     	mov	r0, r5
   2868c: eb005acc     	bl	0x3f1c4
   28690: eaffff9b     	b	0x28504
   28694: e286b915     	add	r11, r6, #344064
   28698: e5dbaabc     	ldrb	r10, [r11, #0xabc]
   2869c: e35a0000     	cmp	r10, #0
   286a0: 0a00000b     	beq	0x286d4
   286a4: e594a000     	ldr	r10, [r4]
   286a8: e35a0000     	cmp	r10, #0
   286ac: 0a000013     	beq	0x28700
   286b0: e35a0001     	cmp	r10, #1
   286b4: 1affffc9     	bne	0x285e0
   286b8: e3a01000     	mov	r1, #0
   286bc: e1a00004     	mov	r0, r4
   286c0: eb005abf     	bl	0x3f1c4
   286c4: e3a01000     	mov	r1, #0
   286c8: e1a00005     	mov	r0, r5
   286cc: eb005abc     	bl	0x3f1c4
   286d0: eaffff8b     	b	0x28504
   286d4: e3a0100b     	mov	r1, #11
   286d8: e1a00004     	mov	r0, r4
   286dc: eb005ab8     	bl	0x3f1c4
   286e0: e5d430ac     	ldrb	r3, [r4, #0xac]
   286e4: e3530000     	cmp	r3, #0
   286e8: 0affff85     	beq	0x28504
   286ec: e1a0100a     	mov	r1, r10
   286f0: e1a00004     	mov	r0, r4
   286f4: e5c4a0ac     	strb	r10, [r4, #0xac]
   286f8: eb005ab1     	bl	0x3f1c4
   286fc: eaffff80     	b	0x28504
   28700: e3a0100b     	mov	r1, #11
   28704: e1a00005     	mov	r0, r5
   28708: eb005aad     	bl	0x3f1c4
   2870c: e3a0100b     	mov	r1, #11
   28710: e1a00004     	mov	r0, r4
   28714: eb005aaa     	bl	0x3f1c4
   28718: e5d430ac     	ldrb	r3, [r4, #0xac]
   2871c: e3530000     	cmp	r3, #0
   28720: 15c4a0ac     	strbne	r10, [r4, #0xac]
   28724: 11a0100a     	movne	r1, r10
   28728: 15c5a0ac     	strbne	r10, [r5, #0xac]
   2872c: 0affff74     	beq	0x28504
   28730: eaffffe1     	b	0x286bc
