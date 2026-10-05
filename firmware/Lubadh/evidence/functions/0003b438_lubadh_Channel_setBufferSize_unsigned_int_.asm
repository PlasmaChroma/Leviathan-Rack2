; lubadh::Channel::setBufferSize(unsigned int)
; VA 0x3b438 size 936

   3b438: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   3b43c: e2805a2a     	add	r5, r0, #172032
   3b440: e1a04001     	mov	r4, r1
   3b444: e1a06000     	mov	r6, r0
   3b448: e595300c     	ldr	r3, [r5, #0xc]
   3b44c: e5952010     	ldr	r2, [r5, #0x10]
   3b450: e0421003     	sub	r1, r2, r3
   3b454: e1a01141     	asr	r1, r1, #2
   3b458: e1540001     	cmp	r4, r1
   3b45c: 8a0000a1     	bhi	0x3b6e8
   3b460: e1a08104     	lsl	r8, r4, #2
   3b464: 3a000097     	blo	0x3b6c8
   3b468: e5953018     	ldr	r3, [r5, #0x18]
   3b46c: e595201c     	ldr	r2, [r5, #0x1c]
   3b470: e0421003     	sub	r1, r2, r3
   3b474: e1a01141     	asr	r1, r1, #2
   3b478: e1540001     	cmp	r4, r1
   3b47c: 8a000095     	bhi	0x3b6d8
   3b480: 3a00008c     	blo	0x3b6b8
   3b484: e5952024     	ldr	r2, [r5, #0x24]
   3b488: e0887004     	add	r7, r8, r4
   3b48c: e5953028     	ldr	r3, [r5, #0x28]
   3b490: e0431002     	sub	r1, r3, r2
   3b494: e1a01141     	asr	r1, r1, #2
   3b498: e1570001     	cmp	r7, r1
   3b49c: 8a00009a     	bhi	0x3b70c
   3b4a0: 3a00007f     	blo	0x3b6a4
   3b4a4: e1c523d0     	ldrd	r2, r3, [r5, #48]
   3b4a8: e0431002     	sub	r1, r3, r2
   3b4ac: e1a01141     	asr	r1, r1, #2
   3b4b0: e1570001     	cmp	r7, r1
   3b4b4: 8a000090     	bhi	0x3b6fc
   3b4b8: 3a000074     	blo	0x3b690
   3b4bc: e1c523dc     	ldrd	r2, r3, [r5, #60]
   3b4c0: e0431002     	sub	r1, r3, r2
   3b4c4: e1a01141     	asr	r1, r1, #2
   3b4c8: e1570001     	cmp	r7, r1
   3b4cc: 8a000097     	bhi	0x3b730
   3b4d0: 3a000069     	blo	0x3b67c
   3b4d4: e2867a29     	add	r7, r6, #167936
   3b4d8: e2849004     	add	r9, r4, #4
   3b4dc: e5973fe8     	ldr	r3, [r7, #0xfe8]
   3b4e0: e5972fec     	ldr	r2, [r7, #0xfec]
   3b4e4: e5874fe4     	str	r4, [r7, #0xfe4]
   3b4e8: e0421003     	sub	r1, r2, r3
   3b4ec: e1a01141     	asr	r1, r1, #2
   3b4f0: e1590001     	cmp	r9, r1
   3b4f4: 8a000088     	bhi	0x3b71c
   3b4f8: 3a00005b     	blo	0x3b66c
   3b4fc: e5952000     	ldr	r2, [r5]
   3b500: e5973ffc     	ldr	r3, [r7, #0xffc]
   3b504: e5874ff8     	str	r4, [r7, #0xff8]
   3b508: e0421003     	sub	r1, r2, r3
   3b50c: e1a01141     	asr	r1, r1, #2
   3b510: e1590001     	cmp	r9, r1
   3b514: 8a0000a2     	bhi	0x3b7a4
   3b518: 3a00004f     	blo	0x3b65c
   3b51c: e2862a06     	add	r2, r6, #24576
   3b520: e592321c     	ldr	r3, [r2, #0x21c]
   3b524: e5920220     	ldr	r0, [r2, #0x220]
   3b528: e0401003     	sub	r1, r0, r3
   3b52c: e1a01141     	asr	r1, r1, #2
   3b530: e1540001     	cmp	r4, r1
   3b534: 8a000095     	bhi	0x3b790
   3b538: 3a000043     	blo	0x3b64c
   3b53c: e2865a01     	add	r5, r6, #4096
   3b540: e5953a38     	ldr	r3, [r5, #0xa38]
   3b544: e5952a3c     	ldr	r2, [r5, #0xa3c]
   3b548: e0421003     	sub	r1, r2, r3
   3b54c: e1a01141     	asr	r1, r1, #2
   3b550: e1540001     	cmp	r4, r1
   3b554: 8a00009c     	bhi	0x3b7cc
   3b558: 3a000037     	blo	0x3b63c
   3b55c: e5953a44     	ldr	r3, [r5, #0xa44]
   3b560: e5952a48     	ldr	r2, [r5, #0xa48]
   3b564: e0421003     	sub	r1, r2, r3
   3b568: e1a01141     	asr	r1, r1, #2
   3b56c: e1540001     	cmp	r4, r1
   3b570: 8a000090     	bhi	0x3b7b8
   3b574: 3a00002c     	blo	0x3b62c
   3b578: e5953a50     	ldr	r3, [r5, #0xa50]
   3b57c: e5952a54     	ldr	r2, [r5, #0xa54]
   3b580: e0421003     	sub	r1, r2, r3
   3b584: e1a01141     	asr	r1, r1, #2
   3b588: e1540001     	cmp	r4, r1
   3b58c: 8a000070     	bhi	0x3b754
   3b590: 3a000021     	blo	0x3b61c
   3b594: e2865a03     	add	r5, r6, #12288
   3b598: e5953244     	ldr	r3, [r5, #0x244]
   3b59c: e5952248     	ldr	r2, [r5, #0x248]
   3b5a0: e0421003     	sub	r1, r2, r3
   3b5a4: e1a01141     	asr	r1, r1, #2
   3b5a8: e1540001     	cmp	r4, r1
   3b5ac: 8a000063     	bhi	0x3b740
   3b5b0: 3a000015     	blo	0x3b60c
   3b5b4: e5953250     	ldr	r3, [r5, #0x250]
   3b5b8: e5952254     	ldr	r2, [r5, #0x254]
   3b5bc: e0421003     	sub	r1, r2, r3
   3b5c0: e1a01141     	asr	r1, r1, #2
   3b5c4: e1540001     	cmp	r4, r1
   3b5c8: 8a00006b     	bhi	0x3b77c
   3b5cc: 3a00000a     	blo	0x3b5fc
   3b5d0: e595325c     	ldr	r3, [r5, #0x25c]
   3b5d4: e5952260     	ldr	r2, [r5, #0x260]
   3b5d8: e0421003     	sub	r1, r2, r3
   3b5dc: e1a01141     	asr	r1, r1, #2
   3b5e0: e1540001     	cmp	r4, r1
   3b5e4: 8a00005f     	bhi	0x3b768
   3b5e8: 28bd87f0     	pophs	{r4, r5, r6, r7, r8, r9, r10, pc}
   3b5ec: e0838008     	add	r8, r3, r8
   3b5f0: e1520008     	cmp	r2, r8
   3b5f4: 15858260     	strne	r8, [r5, #0x260]
   3b5f8: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   3b5fc: e0833008     	add	r3, r3, r8
   3b600: e1520003     	cmp	r2, r3
   3b604: 15853254     	strne	r3, [r5, #0x254]
   3b608: eafffff0     	b	0x3b5d0
   3b60c: e0833008     	add	r3, r3, r8
   3b610: e1520003     	cmp	r2, r3
   3b614: 15853248     	strne	r3, [r5, #0x248]
   3b618: eaffffe5     	b	0x3b5b4
   3b61c: e0833008     	add	r3, r3, r8
   3b620: e1520003     	cmp	r2, r3
   3b624: 15853a54     	strne	r3, [r5, #0xa54]
   3b628: eaffffd9     	b	0x3b594
   3b62c: e0833008     	add	r3, r3, r8
   3b630: e1520003     	cmp	r2, r3
   3b634: 15853a48     	strne	r3, [r5, #0xa48]
   3b638: eaffffce     	b	0x3b578
   3b63c: e0833008     	add	r3, r3, r8
   3b640: e1520003     	cmp	r2, r3
   3b644: 15853a3c     	strne	r3, [r5, #0xa3c]
   3b648: eaffffc3     	b	0x3b55c
   3b64c: e0833008     	add	r3, r3, r8
   3b650: e1500003     	cmp	r0, r3
   3b654: 15823220     	strne	r3, [r2, #0x220]
   3b658: eaffffb7     	b	0x3b53c
   3b65c: e0839109     	add	r9, r3, r9, lsl #2
   3b660: e1520009     	cmp	r2, r9
   3b664: 15859000     	strne	r9, [r5]
   3b668: eaffffab     	b	0x3b51c
   3b66c: e0833109     	add	r3, r3, r9, lsl #2
   3b670: e1520003     	cmp	r2, r3
   3b674: 15873fec     	strne	r3, [r7, #0xfec]
   3b678: eaffff9f     	b	0x3b4fc
   3b67c: e3a01014     	mov	r1, #20
   3b680: e0222491     	mla	r2, r1, r4, r2
   3b684: e1530002     	cmp	r3, r2
   3b688: 15852040     	strne	r2, [r5, #0x40]
   3b68c: eaffff90     	b	0x3b4d4
   3b690: e3a01014     	mov	r1, #20
   3b694: e0222491     	mla	r2, r1, r4, r2
   3b698: e1530002     	cmp	r3, r2
   3b69c: 15852034     	strne	r2, [r5, #0x34]
   3b6a0: eaffff85     	b	0x3b4bc
   3b6a4: e3a01014     	mov	r1, #20
   3b6a8: e0222491     	mla	r2, r1, r4, r2
   3b6ac: e1530002     	cmp	r3, r2
   3b6b0: 15852028     	strne	r2, [r5, #0x28]
   3b6b4: eaffff7a     	b	0x3b4a4
   3b6b8: e0833008     	add	r3, r3, r8
   3b6bc: e1520003     	cmp	r2, r3
   3b6c0: 1585301c     	strne	r3, [r5, #0x1c]
   3b6c4: eaffff6e     	b	0x3b484
   3b6c8: e0833008     	add	r3, r3, r8
   3b6cc: e1520003     	cmp	r2, r3
   3b6d0: 15853010     	strne	r3, [r5, #0x10]
   3b6d4: eaffff63     	b	0x3b468
   3b6d8: e0441001     	sub	r1, r4, r1
   3b6dc: e2850018     	add	r0, r5, #24
   3b6e0: eb001214     	bl	0x3ff38
   3b6e4: eaffff66     	b	0x3b484
   3b6e8: e0441001     	sub	r1, r4, r1
   3b6ec: e285000c     	add	r0, r5, #12
   3b6f0: e1a08104     	lsl	r8, r4, #2
   3b6f4: eb00120f     	bl	0x3ff38
   3b6f8: eaffff5a     	b	0x3b468
   3b6fc: e0471001     	sub	r1, r7, r1
   3b700: e2850030     	add	r0, r5, #48
   3b704: eb0012e5     	bl	0x402a0
   3b708: eaffff6b     	b	0x3b4bc
   3b70c: e0471001     	sub	r1, r7, r1
   3b710: e2850024     	add	r0, r5, #36
   3b714: eb001207     	bl	0x3ff38
   3b718: eaffff61     	b	0x3b4a4
   3b71c: e2860ba7     	add	r0, r6, #171008
   3b720: e0491001     	sub	r1, r9, r1
   3b724: e2800ffa     	add	r0, r0, #1000
   3b728: eb001202     	bl	0x3ff38
   3b72c: eaffff72     	b	0x3b4fc
   3b730: e0471001     	sub	r1, r7, r1
   3b734: e285003c     	add	r0, r5, #60
   3b738: eb0011fe     	bl	0x3ff38
   3b73c: eaffff64     	b	0x3b4d4
   3b740: e2860dc9     	add	r0, r6, #12864
   3b744: e0441001     	sub	r1, r4, r1
   3b748: e2800004     	add	r0, r0, #4
   3b74c: eb0011f9     	bl	0x3ff38
   3b750: eaffff97     	b	0x3b5b4
   3b754: e2860d69     	add	r0, r6, #6720
   3b758: e0441001     	sub	r1, r4, r1
   3b75c: e2800010     	add	r0, r0, #16
   3b760: eb0011f4     	bl	0x3ff38
   3b764: eaffff8a     	b	0x3b594
   3b768: e2860dc9     	add	r0, r6, #12864
   3b76c: e0441001     	sub	r1, r4, r1
   3b770: e280001c     	add	r0, r0, #28
   3b774: e8bd47f0     	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
   3b778: ea0011ee     	b	0x3ff38
   3b77c: e2860dc9     	add	r0, r6, #12864
   3b780: e0441001     	sub	r1, r4, r1
   3b784: e2800010     	add	r0, r0, #16
   3b788: eb0011ea     	bl	0x3ff38
   3b78c: eaffff8f     	b	0x3b5d0
   3b790: e2860c62     	add	r0, r6, #25088
   3b794: e0441001     	sub	r1, r4, r1
   3b798: e280001c     	add	r0, r0, #28
   3b79c: eb0011e5     	bl	0x3ff38
   3b7a0: eaffff65     	b	0x3b53c
   3b7a4: e2860ba7     	add	r0, r6, #171008
   3b7a8: e0491001     	sub	r1, r9, r1
   3b7ac: e2800fff     	add	r0, r0, #1020
   3b7b0: eb0011e0     	bl	0x3ff38
   3b7b4: eaffff58     	b	0x3b51c
   3b7b8: e2860d69     	add	r0, r6, #6720
   3b7bc: e0441001     	sub	r1, r4, r1
   3b7c0: e2800004     	add	r0, r0, #4
   3b7c4: eb0011db     	bl	0x3ff38
   3b7c8: eaffff6a     	b	0x3b578
   3b7cc: e2860c1a     	add	r0, r6, #6656
   3b7d0: e0441001     	sub	r1, r4, r1
   3b7d4: e2800038     	add	r0, r0, #56
   3b7d8: eb0011d6     	bl	0x3ff38
   3b7dc: eaffff5e     	b	0x3b55c
