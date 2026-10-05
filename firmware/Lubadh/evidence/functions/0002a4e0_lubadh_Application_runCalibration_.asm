; lubadh::Application::runCalibration()
; VA 0x2a4e0 size 2816

   2a4e0: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   2a4e4: e3001fb8     	movw	r1, #0xfb8
   2a4e8: e3401007     	movt	r1, #0x7
   2a4ec: ed2d8b02     	vpush	{d8}
   2a4f0: e24dd09c     	sub	sp, sp, #156
   2a4f4: e1a0a000     	mov	r10, r0
   2a4f8: e28d0078     	add	r0, sp, #120
   2a4fc: ebfff5a9     	bl	0x27ba8
   2a500: e3090fec     	movw	r0, #0x9fec
   2a504: e3400009     	movt	r0, #0x9
   2a508: e28d1078     	add	r1, sp, #120
   2a50c: e3a02000     	mov	r2, #0
   2a510: eb011682     	bl	0x6ff20
   2a514: e59d0078     	ldr	r0, [sp, #0x78]
   2a518: e28d3080     	add	r3, sp, #128
   2a51c: e1500003     	cmp	r0, r3
   2a520: 0a000000     	beq	0x2a528
   2a524: ebffae45     	bl	0x15e40    @ imm = #-0x146ec ; _ZdlPv
   2a528: e3a02001     	mov	r2, #1
   2a52c: e3a01098     	mov	r1, #152
   2a530: e1a0000a     	mov	r0, r10
   2a534: e28a4a2a     	add	r4, r10, #172032
   2a538: e58d400c     	str	r4, [sp, #0xc]
   2a53c: eb010914     	bl	0x6c994
   2a540: e3a02001     	mov	r2, #1
   2a544: e3a01099     	mov	r1, #153
   2a548: e1a0000a     	mov	r0, r10
   2a54c: e28a5915     	add	r5, r10, #344064
   2a550: eb01090f     	bl	0x6c994
   2a554: e3a02000     	mov	r2, #0
   2a558: e3a01092     	mov	r1, #146
   2a55c: e1a0000a     	mov	r0, r10
   2a560: e58d501c     	str	r5, [sp, #0x1c]
   2a564: eb01090a     	bl	0x6c994
   2a568: e3a02000     	mov	r2, #0
   2a56c: e3a01093     	mov	r1, #147
   2a570: e1a0000a     	mov	r0, r10
   2a574: ed9f8bef     	vldr	d8, [pc, #956]          @ 0x2a938 ; float 7.0086295542e-308
   2a578: eb010905     	bl	0x6c994
   2a57c: e5d414de     	ldrb	r1, [r4, #0x4de]
   2a580: e3a02000     	mov	r2, #0
   2a584: e1a0000a     	mov	r0, r10
   2a588: eb010901     	bl	0x6c994
   2a58c: e5d51a16     	ldrb	r1, [r5, #0xa16]
   2a590: e3a02000     	mov	r2, #0
   2a594: e1a0000a     	mov	r0, r10
   2a598: eb0108fd     	bl	0x6c994
   2a59c: e28a3ba9     	add	r3, r10, #173056
   2a5a0: f3c70e5f     	vmov.i8	q8, #0xff
   2a5a4: e2833d05     	add	r3, r3, #320
   2a5a8: e58da02c     	str	r10, [sp, #0x2c]
   2a5ac: e58d3010     	str	r3, [sp, #0x10]
   2a5b0: e28a3048     	add	r3, r10, #72
   2a5b4: e58d3020     	str	r3, [sp, #0x20]
   2a5b8: e3a03000     	mov	r3, #0
   2a5bc: e58d3008     	str	r3, [sp, #0x8]
   2a5c0: edcd0b1e     	vstr	d16, [sp, #120]
   2a5c4: edcd1b20     	vstr	d17, [sp, #128]
   2a5c8: edcd0b22     	vstr	d16, [sp, #136]
   2a5cc: edcd1b24     	vstr	d17, [sp, #144]
   2a5d0: e1a0000a     	mov	r0, r10
   2a5d4: ebfff772     	bl	0x283a4
   2a5d8: e59d3008     	ldr	r3, [sp, #0x8]
   2a5dc: e3530003     	cmp	r3, #3
   2a5e0: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   2a5e4: ea0000aa     	b	0x2a894
   2a5e8: ec aa 02 00  	.word	0x0002aaec
   2a5ec: 44 a9 02 00  	.word	0x0002a944
   2a5f0: 40 a7 02 00  	.word	0x0002a740
   2a5f4: f8 a5 02 00  	.word	0x0002a5f8
   2a5f8: e28a6a7e     	add	r6, r10, #516096
   2a5fc: e59d4010     	ldr	r4, [sp, #0x10]
   2a600: e3069218     	movw	r9, #0x6218
   2a604: e3409001     	movt	r9, #0x1
   2a608: e2866efb     	add	r6, r6, #4016
   2a60c: e28d5078     	add	r5, sp, #120
   2a610: e30130d4     	movw	r3, #0x10d4
   2a614: e3403007     	movt	r3, #0x7
   2a618: e3008d28     	movw	r8, #0xd28
   2a61c: e3408007     	movt	r8, #0x7
   2a620: e58d3018     	str	r3, [sp, #0x18]
   2a624: e58d8014     	str	r8, [sp, #0x14]
   2a628: e5541078     	ldrb	r1, [r4, #-0x78]
   2a62c: e1a0000a     	mov	r0, r10
   2a630: eb01068b     	bl	0x6c064
   2a634: e1a01000     	mov	r1, r0
   2a638: e3007fff     	movw	r7, #0xfff
   2a63c: e1a00004     	mov	r0, r4
   2a640: e0471001     	sub	r1, r7, r1
   2a644: eb0106ae     	bl	0x6c104
   2a648: e2443a2a     	sub	r3, r4, #172032
   2a64c: e1a0c000     	mov	r12, r0
   2a650: e55333fb     	ldrb	r3, [r3, #-0x3fb]
   2a654: e3530000     	cmp	r3, #0
   2a658: 0a000032     	beq	0x2a728
   2a65c: e2443ba9     	sub	r3, r4, #173056
   2a660: e3002fce     	movw	r2, #0xfce
   2a664: e24330f4     	sub	r3, r3, #244
   2a668: e1500002     	cmp	r0, r2
   2a66c: e58d3024     	str	r3, [sp, #0x24]
   2a670: da0001b8     	ble	0x2ad58
   2a674: e5541064     	ldrb	r1, [r4, #-0x64]
   2a678: e3a02004     	mov	r2, #4
   2a67c: e585c00c     	str	r12, [r5, #0xc]
   2a680: e1a0000a     	mov	r0, r10
   2a684: eb0108c2     	bl	0x6c994
   2a688: e59d3024     	ldr	r3, [sp, #0x24]
   2a68c: e28d8030     	add	r8, sp, #48
   2a690: e30120c0     	movw	r2, #0x10c0
   2a694: e3402007     	movt	r2, #0x7
   2a698: e1a01003     	mov	r1, r3
   2a69c: e1a00008     	mov	r0, r8
   2a6a0: e28db048     	add	r11, sp, #72
   2a6a4: eb001147     	bl	0x2ebc8
   2a6a8: e595300c     	ldr	r3, [r5, #0xc]
   2a6ac: e58d3000     	str	r3, [sp]
   2a6b0: e1a0000b     	mov	r0, r11
   2a6b4: e59d3014     	ldr	r3, [sp, #0x14]
   2a6b8: e3a02010     	mov	r2, #16
   2a6bc: e1a01009     	mov	r1, r9
   2a6c0: ebfffe2c     	bl	0x29f78
   2a6c4: e28d7060     	add	r7, sp, #96
   2a6c8: e1a0200b     	mov	r2, r11
   2a6cc: e1a01008     	mov	r1, r8
   2a6d0: e1a00007     	mov	r0, r7
   2a6d4: eb001084     	bl	0x2e8ec
   2a6d8: e3090fec     	movw	r0, #0x9fec
   2a6dc: e3400009     	movt	r0, #0x9
   2a6e0: e1a01007     	mov	r1, r7
   2a6e4: e3a02000     	mov	r2, #0
   2a6e8: eb01160c     	bl	0x6ff20
   2a6ec: e59d0060     	ldr	r0, [sp, #0x60]
   2a6f0: e28d3068     	add	r3, sp, #104
   2a6f4: e1500003     	cmp	r0, r3
   2a6f8: 0a000000     	beq	0x2a700
   2a6fc: ebffadcf     	bl	0x15e40    @ imm = #-0x148c4 ; _ZdlPv
   2a700: e59d0048     	ldr	r0, [sp, #0x48]
   2a704: e28d3050     	add	r3, sp, #80
   2a708: e1500003     	cmp	r0, r3
   2a70c: 0a000000     	beq	0x2a714
   2a710: ebffadca     	bl	0x15e40    @ imm = #-0x148d8 ; _ZdlPv
   2a714: e59d0030     	ldr	r0, [sp, #0x30]
   2a718: e28d3038     	add	r3, sp, #56
   2a71c: e1500003     	cmp	r0, r3
   2a720: 0a000000     	beq	0x2a728
   2a724: ebffadc5     	bl	0x15e40    @ imm = #-0x148ec ; _ZdlPv
   2a728: e2844ba9     	add	r4, r4, #173056
   2a72c: e2855010     	add	r5, r5, #16
   2a730: e2844f4e     	add	r4, r4, #312
   2a734: e1560004     	cmp	r6, r4
   2a738: 1affffba     	bne	0x2a628
   2a73c: ea000054     	b	0x2a894
   2a740: e28a6a7e     	add	r6, r10, #516096
   2a744: e59d4010     	ldr	r4, [sp, #0x10]
   2a748: e3069218     	movw	r9, #0x6218
   2a74c: e3409001     	movt	r9, #0x1
   2a750: e2866efb     	add	r6, r6, #4016
   2a754: e28d5078     	add	r5, sp, #120
   2a758: e3013088     	movw	r3, #0x1088
   2a75c: e3403007     	movt	r3, #0x7
   2a760: e3007d28     	movw	r7, #0xd28
   2a764: e3407007     	movt	r7, #0x7
   2a768: e58d3018     	str	r3, [sp, #0x18]
   2a76c: e58d7014     	str	r7, [sp, #0x14]
   2a770: e5541078     	ldrb	r1, [r4, #-0x78]
   2a774: e1a0000a     	mov	r0, r10
   2a778: eb010639     	bl	0x6c064
   2a77c: e2601eff     	rsb	r1, r0, #4080
   2a780: e281100f     	add	r1, r1, #15
   2a784: e1a00004     	mov	r0, r4
   2a788: eb01065d     	bl	0x6c104
   2a78c: e2443a2a     	sub	r3, r4, #172032
   2a790: e1a0c000     	mov	r12, r0
   2a794: e55333fb     	ldrb	r3, [r3, #-0x3fb]
   2a798: e3530000     	cmp	r3, #0
   2a79c: 0a000031     	beq	0x2a868
   2a7a0: e2403e96     	sub	r3, r0, #2400
   2a7a4: e2447ba9     	sub	r7, r4, #173056
   2a7a8: e2433007     	sub	r3, r3, #7
   2a7ac: e24770f4     	sub	r7, r7, #244
   2a7b0: e3530064     	cmp	r3, #100
   2a7b4: 8a0001a0     	bhi	0x2ae3c
   2a7b8: e5541064     	ldrb	r1, [r4, #-0x64]
   2a7bc: e3a02004     	mov	r2, #4
   2a7c0: e585c008     	str	r12, [r5, #0x8]
   2a7c4: e1a0000a     	mov	r0, r10
   2a7c8: e28d8030     	add	r8, sp, #48
   2a7cc: eb010870     	bl	0x6c994
   2a7d0: e1a01007     	mov	r1, r7
   2a7d4: e1a00008     	mov	r0, r8
   2a7d8: e3012074     	movw	r2, #0x1074
   2a7dc: e3402007     	movt	r2, #0x7
   2a7e0: eb0010f8     	bl	0x2ebc8
   2a7e4: e28db048     	add	r11, sp, #72
   2a7e8: e5953008     	ldr	r3, [r5, #0x8]
   2a7ec: e1a0000b     	mov	r0, r11
   2a7f0: e58d3000     	str	r3, [sp]
   2a7f4: e3a02010     	mov	r2, #16
   2a7f8: e59d3014     	ldr	r3, [sp, #0x14]
   2a7fc: e1a01009     	mov	r1, r9
   2a800: ebfffddc     	bl	0x29f78
   2a804: e28d7060     	add	r7, sp, #96
   2a808: e1a0200b     	mov	r2, r11
   2a80c: e1a01008     	mov	r1, r8
   2a810: e1a00007     	mov	r0, r7
   2a814: eb001034     	bl	0x2e8ec
   2a818: e3090fec     	movw	r0, #0x9fec
   2a81c: e3400009     	movt	r0, #0x9
   2a820: e1a01007     	mov	r1, r7
   2a824: e3a02000     	mov	r2, #0
   2a828: eb0115bc     	bl	0x6ff20
   2a82c: e59d0060     	ldr	r0, [sp, #0x60]
   2a830: e28d3068     	add	r3, sp, #104
   2a834: e1500003     	cmp	r0, r3
   2a838: 0a000000     	beq	0x2a840
   2a83c: ebffad7f     	bl	0x15e40    @ imm = #-0x14a04 ; _ZdlPv
   2a840: e59d0048     	ldr	r0, [sp, #0x48]
   2a844: e28d3050     	add	r3, sp, #80
   2a848: e1500003     	cmp	r0, r3
   2a84c: 0a000000     	beq	0x2a854
   2a850: ebffad7a     	bl	0x15e40    @ imm = #-0x14a18 ; _ZdlPv
   2a854: e59d0030     	ldr	r0, [sp, #0x30]
   2a858: e28d3038     	add	r3, sp, #56
   2a85c: e1500003     	cmp	r0, r3
   2a860: 0a000000     	beq	0x2a868
   2a864: ebffad75     	bl	0x15e40    @ imm = #-0x14a2c ; _ZdlPv
   2a868: e2844ba9     	add	r4, r4, #173056
   2a86c: e2855010     	add	r5, r5, #16
   2a870: e2844f4e     	add	r4, r4, #312
   2a874: e1540006     	cmp	r4, r6
   2a878: 1affffbc     	bne	0x2a770
   2a87c: e59d3080     	ldr	r3, [sp, #0x80]
   2a880: e3530000     	cmp	r3, #0
   2a884: ba000002     	blt	0x2a894
   2a888: e59d3090     	ldr	r3, [sp, #0x90]
   2a88c: e3530000     	cmp	r3, #0
   2a890: aa00019a     	bge	0x2af00
   2a894: e59d3084     	ldr	r3, [sp, #0x84]
   2a898: e3530000     	cmp	r3, #0
   2a89c: da000002     	ble	0x2a8ac
   2a8a0: e59d3094     	ldr	r3, [sp, #0x94]
   2a8a4: e3530000     	cmp	r3, #0
   2a8a8: ca000180     	bgt	0x2aeb0
   2a8ac: e59a32d8     	ldr	r3, [r10, #0x2d8]
   2a8b0: e3530000     	cmp	r3, #0
   2a8b4: da000006     	ble	0x2a8d4
   2a8b8: e59a12dc     	ldr	r1, [r10, #0x2dc]
   2a8bc: e2433001     	sub	r3, r3, #1
   2a8c0: e58a32d8     	str	r3, [r10, #0x2d8]
   2a8c4: e712f113     	sdiv	r2, r3, r1
   2a8c8: e0633291     	mls	r3, r1, r2, r3
   2a8cc: e3530000     	cmp	r3, #0
   2a8d0: 0a0000ea     	beq	0x2ac80
   2a8d4: e59d200c     	ldr	r2, [sp, #0xc]
   2a8d8: e5923810     	ldr	r3, [r2, #0x810]
   2a8dc: e3530000     	cmp	r3, #0
   2a8e0: da000006     	ble	0x2a900
   2a8e4: e5921814     	ldr	r1, [r2, #0x814]
   2a8e8: e2433001     	sub	r3, r3, #1
   2a8ec: e5823810     	str	r3, [r2, #0x810]
   2a8f0: e712f113     	sdiv	r2, r3, r1
   2a8f4: e0633291     	mls	r3, r1, r2, r3
   2a8f8: e3530000     	cmp	r3, #0
   2a8fc: 0a0000d6     	beq	0x2ac5c
   2a900: e28d4060     	add	r4, sp, #96
   2a904: ed8d8b18     	vstr	d8, [sp, #96]
   2a908: ea000003     	b	0x2a91c
   2a90c: ebffad3f     	bl	0x15e10    @ imm = #-0x14b04 ; __errno_location
   2a910: e5903000     	ldr	r3, [r0]
   2a914: e3530004     	cmp	r3, #4
   2a918: 1affff2c     	bne	0x2a5d0
   2a91c: e1a01004     	mov	r1, r4
   2a920: e1a00004     	mov	r0, r4
   2a924: ebffada5     	bl	0x15fc0    @ imm = #-0x1496c ; nanosleep
   2a928: e3700001     	cmn	r0, #1
   2a92c: 0afffff6     	beq	0x2a90c
   2a930: eaffff26     	b	0x2a5d0
   2a934: e320f000     	nop
   2a938: 00 00 00 00  	.word	0x00000000
   2a93c: e0 32 29 00  	.word	0x002932e0
   2a940: 00 00 7a 44  	.word	0x447a0000
   2a944: e28a6a7e     	add	r6, r10, #516096
   2a948: e59d4010     	ldr	r4, [sp, #0x10]
   2a94c: e3069218     	movw	r9, #0x6218
   2a950: e3409001     	movt	r9, #0x1
   2a954: e2866efb     	add	r6, r6, #4016
   2a958: e28d5078     	add	r5, sp, #120
   2a95c: e301303c     	movw	r3, #0x103c
   2a960: e3403007     	movt	r3, #0x7
   2a964: e3007d28     	movw	r7, #0xd28
   2a968: e3407007     	movt	r7, #0x7
   2a96c: e58d3018     	str	r3, [sp, #0x18]
   2a970: e58d7014     	str	r7, [sp, #0x14]
   2a974: e5541078     	ldrb	r1, [r4, #-0x78]
   2a978: e1a0000a     	mov	r0, r10
   2a97c: eb0105b8     	bl	0x6c064
   2a980: e2601eff     	rsb	r1, r0, #4080
   2a984: e281100f     	add	r1, r1, #15
   2a988: e1a00004     	mov	r0, r4
   2a98c: eb0105dc     	bl	0x6c104
   2a990: e2443a2a     	sub	r3, r4, #172032
   2a994: e1a0c000     	mov	r12, r0
   2a998: e55333fb     	ldrb	r3, [r3, #-0x3fb]
   2a99c: e3530000     	cmp	r3, #0
   2a9a0: 0a000031     	beq	0x2aa6c
   2a9a4: e2403c03     	sub	r3, r0, #768
   2a9a8: e2447ba9     	sub	r7, r4, #173056
   2a9ac: e2433001     	sub	r3, r3, #1
   2a9b0: e24770f4     	sub	r7, r7, #244
   2a9b4: e3530064     	cmp	r3, #100
   2a9b8: 8a000102     	bhi	0x2adc8
   2a9bc: e5541064     	ldrb	r1, [r4, #-0x64]
   2a9c0: e3a02004     	mov	r2, #4
   2a9c4: e585c004     	str	r12, [r5, #0x4]
   2a9c8: e1a0000a     	mov	r0, r10
   2a9cc: e28d8030     	add	r8, sp, #48
   2a9d0: eb0107ef     	bl	0x6c994
   2a9d4: e1a01007     	mov	r1, r7
   2a9d8: e1a00008     	mov	r0, r8
   2a9dc: e3012028     	movw	r2, #0x1028
   2a9e0: e3402007     	movt	r2, #0x7
   2a9e4: eb001077     	bl	0x2ebc8
   2a9e8: e28db048     	add	r11, sp, #72
   2a9ec: e5953004     	ldr	r3, [r5, #0x4]
   2a9f0: e1a0000b     	mov	r0, r11
   2a9f4: e58d3000     	str	r3, [sp]
   2a9f8: e3a02010     	mov	r2, #16
   2a9fc: e59d3014     	ldr	r3, [sp, #0x14]
   2aa00: e1a01009     	mov	r1, r9
   2aa04: ebfffd5b     	bl	0x29f78
   2aa08: e28d7060     	add	r7, sp, #96
   2aa0c: e1a0200b     	mov	r2, r11
   2aa10: e1a01008     	mov	r1, r8
   2aa14: e1a00007     	mov	r0, r7
   2aa18: eb000fb3     	bl	0x2e8ec
   2aa1c: e3090fec     	movw	r0, #0x9fec
   2aa20: e3400009     	movt	r0, #0x9
   2aa24: e1a01007     	mov	r1, r7
   2aa28: e3a02000     	mov	r2, #0
   2aa2c: eb01153b     	bl	0x6ff20
   2aa30: e59d0060     	ldr	r0, [sp, #0x60]
   2aa34: e28d3068     	add	r3, sp, #104
   2aa38: e1500003     	cmp	r0, r3
   2aa3c: 0a000000     	beq	0x2aa44
   2aa40: ebffacfe     	bl	0x15e40    @ imm = #-0x14c08 ; _ZdlPv
   2aa44: e59d0048     	ldr	r0, [sp, #0x48]
   2aa48: e28d3050     	add	r3, sp, #80
   2aa4c: e1500003     	cmp	r0, r3
   2aa50: 0a000000     	beq	0x2aa58
   2aa54: ebffacf9     	bl	0x15e40    @ imm = #-0x14c1c ; _ZdlPv
   2aa58: e59d0030     	ldr	r0, [sp, #0x30]
   2aa5c: e28d3038     	add	r3, sp, #56
   2aa60: e1500003     	cmp	r0, r3
   2aa64: 0a000000     	beq	0x2aa6c
   2aa68: ebffacf4     	bl	0x15e40    @ imm = #-0x14c30 ; _ZdlPv
   2aa6c: e2844ba9     	add	r4, r4, #173056
   2aa70: e2855010     	add	r5, r5, #16
   2aa74: e2844f4e     	add	r4, r4, #312
   2aa78: e1560004     	cmp	r6, r4
   2aa7c: 1affffbc     	bne	0x2a974
   2aa80: e59d307c     	ldr	r3, [sp, #0x7c]
   2aa84: e3530000     	cmp	r3, #0
   2aa88: baffff81     	blt	0x2a894
   2aa8c: e59d308c     	ldr	r3, [sp, #0x8c]
   2aa90: e3530000     	cmp	r3, #0
   2aa94: baffff7e     	blt	0x2a894
   2aa98: e59d4020     	ldr	r4, [sp, #0x20]
   2aa9c: e3a03002     	mov	r3, #2
   2aaa0: ed1f0a5a     	vldr	s0, [pc, #-360]         @ 0x2a940 ; float 1000
   2aaa4: eef10a0c     	vmov.f32	s1, #7.000000e+00
   2aaa8: e1a00004     	mov	r0, r4
   2aaac: e58d3008     	str	r3, [sp, #0x8]
   2aab0: eb0033b1     	bl	0x3797c
   2aab4: e2840ba9     	add	r0, r4, #173056
   2aab8: ed1f0a60     	vldr	s0, [pc, #-384]         @ 0x2a940 ; float 1000
   2aabc: eef10a0c     	vmov.f32	s1, #7.000000e+00
   2aac0: e2800f4e     	add	r0, r0, #312
   2aac4: eb0033ac     	bl	0x3797c
   2aac8: e3a02003     	mov	r2, #3
   2aacc: e3a01092     	mov	r1, #146
   2aad0: e1a0000a     	mov	r0, r10
   2aad4: eb0107ae     	bl	0x6c994
   2aad8: e3a02003     	mov	r2, #3
   2aadc: e3a01093     	mov	r1, #147
   2aae0: e1a0000a     	mov	r0, r10
   2aae4: eb0107aa     	bl	0x6c994
   2aae8: eaffff69     	b	0x2a894
   2aaec: e59d4010     	ldr	r4, [sp, #0x10]
   2aaf0: e3069218     	movw	r9, #0x6218
   2aaf4: e3409001     	movt	r9, #0x1
   2aaf8: e59d6008     	ldr	r6, [sp, #0x8]
   2aafc: e1a0500a     	mov	r5, r10
   2ab00: e3003ff0     	movw	r3, #0xff0
   2ab04: e3403007     	movt	r3, #0x7
   2ab08: e3007d28     	movw	r7, #0xd28
   2ab0c: e3407007     	movt	r7, #0x7
   2ab10: e58d3018     	str	r3, [sp, #0x18]
   2ab14: e58d7014     	str	r7, [sp, #0x14]
   2ab18: e5541078     	ldrb	r1, [r4, #-0x78]
   2ab1c: e1a0000a     	mov	r0, r10
   2ab20: eb01054f     	bl	0x6c064
   2ab24: e2601eff     	rsb	r1, r0, #4080
   2ab28: e281100f     	add	r1, r1, #15
   2ab2c: e1a00004     	mov	r0, r4
   2ab30: eb010573     	bl	0x6c104
   2ab34: e5d52145     	ldrb	r2, [r5, #0x145]
   2ab38: e1a03000     	mov	r3, r0
   2ab3c: e3520000     	cmp	r2, #0
   2ab40: 02855ba9     	addeq	r5, r5, #173056
   2ab44: 0a000031     	beq	0x2ac10
   2ab48: e3500013     	cmp	r0, #19
   2ab4c: e285704c     	add	r7, r5, #76
   2ab50: ca000052     	bgt	0x2aca0
   2ab54: e28dc078     	add	r12, sp, #120
   2ab58: e5541064     	ldrb	r1, [r4, #-0x64]
   2ab5c: e3a02004     	mov	r2, #4
   2ab60: e1a0000a     	mov	r0, r10
   2ab64: e28d8030     	add	r8, sp, #48
   2ab68: e28db048     	add	r11, sp, #72
   2ab6c: e78c3206     	str	r3, [r12, r6, lsl #4]
   2ab70: eb010787     	bl	0x6c994
   2ab74: e1a01007     	mov	r1, r7
   2ab78: e1a00008     	mov	r0, r8
   2ab7c: e3002fdc     	movw	r2, #0xfdc
   2ab80: e3402007     	movt	r2, #0x7
   2ab84: eb00100f     	bl	0x2ebc8
   2ab88: e28d3078     	add	r3, sp, #120
   2ab8c: e1a0000b     	mov	r0, r11
   2ab90: e3a02010     	mov	r2, #16
   2ab94: e1a01009     	mov	r1, r9
   2ab98: e7933206     	ldr	r3, [r3, r6, lsl #4]
   2ab9c: e58d3000     	str	r3, [sp]
   2aba0: e59d3014     	ldr	r3, [sp, #0x14]
   2aba4: ebfffcf3     	bl	0x29f78
   2aba8: e28d7060     	add	r7, sp, #96
   2abac: e1a0200b     	mov	r2, r11
   2abb0: e1a01008     	mov	r1, r8
   2abb4: e1a00007     	mov	r0, r7
   2abb8: eb000f4b     	bl	0x2e8ec
   2abbc: e3090fec     	movw	r0, #0x9fec
   2abc0: e3400009     	movt	r0, #0x9
   2abc4: e1a01007     	mov	r1, r7
   2abc8: e3a02000     	mov	r2, #0
   2abcc: eb0114d3     	bl	0x6ff20
   2abd0: e59d0060     	ldr	r0, [sp, #0x60]
   2abd4: e28d3068     	add	r3, sp, #104
   2abd8: e1500003     	cmp	r0, r3
   2abdc: 0a000000     	beq	0x2abe4
   2abe0: ebffac96     	bl	0x15e40    @ imm = #-0x14da8 ; _ZdlPv
   2abe4: e59d0048     	ldr	r0, [sp, #0x48]
   2abe8: e28d3050     	add	r3, sp, #80
   2abec: e1500003     	cmp	r0, r3
   2abf0: 0a000000     	beq	0x2abf8
   2abf4: ebffac91     	bl	0x15e40    @ imm = #-0x14dbc ; _ZdlPv
   2abf8: e59d0030     	ldr	r0, [sp, #0x30]
   2abfc: e28d3038     	add	r3, sp, #56
   2ac00: e2855ba9     	add	r5, r5, #173056
   2ac04: e1500003     	cmp	r0, r3
   2ac08: 0a000000     	beq	0x2ac10
   2ac0c: ebffac8b     	bl	0x15e40    @ imm = #-0x14dd4 ; _ZdlPv
   2ac10: e2844ba9     	add	r4, r4, #173056
   2ac14: e2855f4e     	add	r5, r5, #312
   2ac18: e2844f4e     	add	r4, r4, #312
   2ac1c: e3560001     	cmp	r6, #1
   2ac20: 1a00000b     	bne	0x2ac54
   2ac24: e59d3078     	ldr	r3, [sp, #0x78]
   2ac28: e3530000     	cmp	r3, #0
   2ac2c: baffff18     	blt	0x2a894
   2ac30: e59d3088     	ldr	r3, [sp, #0x88]
   2ac34: e3530000     	cmp	r3, #0
   2ac38: baffff15     	blt	0x2a894
   2ac3c: e59d4020     	ldr	r4, [sp, #0x20]
   2ac40: eef10a0c     	vmov.f32	s1, #7.000000e+00
   2ac44: ed1f0ac3     	vldr	s0, [pc, #-780]         @ 0x2a940 ; float 1000
   2ac48: e1a00004     	mov	r0, r4
   2ac4c: e58d6008     	str	r6, [sp, #0x8]
   2ac50: eaffff96     	b	0x2aab0
   2ac54: e3a06001     	mov	r6, #1
   2ac58: eaffffae     	b	0x2ab18
   2ac5c: e59d300c     	ldr	r3, [sp, #0xc]
   2ac60: e1a0000a     	mov	r0, r10
   2ac64: e5d32818     	ldrb	r2, [r3, #0x818]
   2ac68: e2222001     	eor	r2, r2, #1
   2ac6c: e5c32818     	strb	r2, [r3, #0x818]
   2ac70: e59d301c     	ldr	r3, [sp, #0x1c]
   2ac74: e5d31a10     	ldrb	r1, [r3, #0xa10]
   2ac78: eb010745     	bl	0x6c994
   2ac7c: eaffff1f     	b	0x2a900
   2ac80: e5da22e0     	ldrb	r2, [r10, #0x2e0]
   2ac84: e1a0000a     	mov	r0, r10
   2ac88: e59d300c     	ldr	r3, [sp, #0xc]
   2ac8c: e2222001     	eor	r2, r2, #1
   2ac90: e5ca22e0     	strb	r2, [r10, #0x2e0]
   2ac94: e5d314d8     	ldrb	r1, [r3, #0x4d8]
   2ac98: eb01073d     	bl	0x6c994
   2ac9c: eaffff0c     	b	0x2a8d4
   2aca0: e2855ba9     	add	r5, r5, #173056
   2aca4: e28d002c     	add	r0, sp, #44
   2aca8: e28510db     	add	r1, r5, #219
   2acac: e28d8030     	add	r8, sp, #48
   2acb0: ebfff373     	bl	0x27a84
   2acb4: e28db048     	add	r11, sp, #72
   2acb8: e59d2018     	ldr	r2, [sp, #0x18]
   2acbc: e1a01007     	mov	r1, r7
   2acc0: e1a00008     	mov	r0, r8
   2acc4: eb000fbf     	bl	0x2ebc8
   2acc8: e3a02000     	mov	r2, #0
   2accc: e59d3014     	ldr	r3, [sp, #0x14]
   2acd0: e1a0000b     	mov	r0, r11
   2acd4: e58d2000     	str	r2, [sp]
   2acd8: e1a01009     	mov	r1, r9
   2acdc: e3a02010     	mov	r2, #16
   2ace0: ebfffca4     	bl	0x29f78
   2ace4: e28d7060     	add	r7, sp, #96
   2ace8: e1a0200b     	mov	r2, r11
   2acec: e1a01008     	mov	r1, r8
   2acf0: e1a00007     	mov	r0, r7
   2acf4: eb000efc     	bl	0x2e8ec
   2acf8: e3090fec     	movw	r0, #0x9fec
   2acfc: e3400009     	movt	r0, #0x9
   2ad00: e1a01007     	mov	r1, r7
   2ad04: e3a02000     	mov	r2, #0
   2ad08: eb011484     	bl	0x6ff20
   2ad0c: e59d0060     	ldr	r0, [sp, #0x60]
   2ad10: e28d3068     	add	r3, sp, #104
   2ad14: e1500003     	cmp	r0, r3
   2ad18: 0a000000     	beq	0x2ad20
   2ad1c: ebffac47     	bl	0x15e40    @ imm = #-0x14ee4 ; _ZdlPv
   2ad20: e59d0048     	ldr	r0, [sp, #0x48]
   2ad24: e28d3050     	add	r3, sp, #80
   2ad28: e1500003     	cmp	r0, r3
   2ad2c: 0a000000     	beq	0x2ad34
   2ad30: ebffac42     	bl	0x15e40    @ imm = #-0x14ef8 ; _ZdlPv
   2ad34: e59d0030     	ldr	r0, [sp, #0x30]
   2ad38: e28d3038     	add	r3, sp, #56
   2ad3c: e1500003     	cmp	r0, r3
   2ad40: 0a000000     	beq	0x2ad48
   2ad44: ebffac3d     	bl	0x15e40    @ imm = #-0x14f0c ; _ZdlPv
   2ad48: e28d2078     	add	r2, sp, #120
   2ad4c: e3a03000     	mov	r3, #0
   2ad50: e7823206     	str	r3, [r2, r6, lsl #4]
   2ad54: eaffffad     	b	0x2ac10
   2ad58: e2441065     	sub	r1, r4, #101
   2ad5c: e28d002c     	add	r0, sp, #44
   2ad60: e585700c     	str	r7, [r5, #0xc]
   2ad64: e28d8030     	add	r8, sp, #48
   2ad68: ebfff345     	bl	0x27a84
   2ad6c: e59d3024     	ldr	r3, [sp, #0x24]
   2ad70: e59d2018     	ldr	r2, [sp, #0x18]
   2ad74: e1a00008     	mov	r0, r8
   2ad78: e1a01003     	mov	r1, r3
   2ad7c: e28db048     	add	r11, sp, #72
   2ad80: eb000f90     	bl	0x2ebc8
   2ad84: e59d3014     	ldr	r3, [sp, #0x14]
   2ad88: e1a0000b     	mov	r0, r11
   2ad8c: e3a02010     	mov	r2, #16
   2ad90: e1a01009     	mov	r1, r9
   2ad94: e58d7000     	str	r7, [sp]
   2ad98: ebfffc76     	bl	0x29f78
   2ad9c: e28d7060     	add	r7, sp, #96
   2ada0: e1a0200b     	mov	r2, r11
   2ada4: e1a01008     	mov	r1, r8
   2ada8: e1a00007     	mov	r0, r7
   2adac: eb000ece     	bl	0x2e8ec
   2adb0: e3090fec     	movw	r0, #0x9fec
   2adb4: e3400009     	movt	r0, #0x9
   2adb8: e1a01007     	mov	r1, r7
   2adbc: e3a02000     	mov	r2, #0
   2adc0: eb011456     	bl	0x6ff20
   2adc4: eafffe48     	b	0x2a6ec
   2adc8: e3003333     	movw	r3, #0x333
   2adcc: e2441065     	sub	r1, r4, #101
   2add0: e5853004     	str	r3, [r5, #0x4]
   2add4: e28d002c     	add	r0, sp, #44
   2add8: e28d8030     	add	r8, sp, #48
   2addc: ebfff328     	bl	0x27a84
   2ade0: e28db048     	add	r11, sp, #72
   2ade4: e59d2018     	ldr	r2, [sp, #0x18]
   2ade8: e1a01007     	mov	r1, r7
   2adec: e1a00008     	mov	r0, r8
   2adf0: eb000f74     	bl	0x2ebc8
   2adf4: e3003333     	movw	r3, #0x333
   2adf8: e1a0000b     	mov	r0, r11
   2adfc: e58d3000     	str	r3, [sp]
   2ae00: e3a02010     	mov	r2, #16
   2ae04: e59d3014     	ldr	r3, [sp, #0x14]
   2ae08: e1a01009     	mov	r1, r9
   2ae0c: ebfffc59     	bl	0x29f78
   2ae10: e28d7060     	add	r7, sp, #96
   2ae14: e1a0200b     	mov	r2, r11
   2ae18: e1a01008     	mov	r1, r8
   2ae1c: e1a00007     	mov	r0, r7
   2ae20: eb000eb1     	bl	0x2e8ec
   2ae24: e3090fec     	movw	r0, #0x9fec
   2ae28: e3400009     	movt	r0, #0x9
   2ae2c: e1a01007     	mov	r1, r7
   2ae30: e3a02000     	mov	r2, #0
   2ae34: eb011439     	bl	0x6ff20
   2ae38: eafffefc     	b	0x2aa30
   2ae3c: e30039a4     	movw	r3, #0x9a4
   2ae40: e2441065     	sub	r1, r4, #101
   2ae44: e5853008     	str	r3, [r5, #0x8]
   2ae48: e28d002c     	add	r0, sp, #44
   2ae4c: e28d8030     	add	r8, sp, #48
   2ae50: ebfff30b     	bl	0x27a84
   2ae54: e28db048     	add	r11, sp, #72
   2ae58: e59d2018     	ldr	r2, [sp, #0x18]
   2ae5c: e1a01007     	mov	r1, r7
   2ae60: e1a00008     	mov	r0, r8
   2ae64: eb000f57     	bl	0x2ebc8
   2ae68: e30039a4     	movw	r3, #0x9a4
   2ae6c: e1a0000b     	mov	r0, r11
   2ae70: e58d3000     	str	r3, [sp]
   2ae74: e3a02010     	mov	r2, #16
   2ae78: e59d3014     	ldr	r3, [sp, #0x14]
   2ae7c: e1a01009     	mov	r1, r9
   2ae80: ebfffc3c     	bl	0x29f78
   2ae84: e28d7060     	add	r7, sp, #96
   2ae88: e1a0200b     	mov	r2, r11
   2ae8c: e1a01008     	mov	r1, r8
   2ae90: e1a00007     	mov	r0, r7
   2ae94: eb000e94     	bl	0x2e8ec
   2ae98: e3090fec     	movw	r0, #0x9fec
   2ae9c: e3400009     	movt	r0, #0x9
   2aea0: e1a01007     	mov	r1, r7
   2aea4: e3a02000     	mov	r2, #0
   2aea8: eb01141c     	bl	0x6ff20
   2aeac: eafffe5e     	b	0x2a82c
   2aeb0: e28d0060     	add	r0, sp, #96
   2aeb4: e301110c     	movw	r1, #0x110c
   2aeb8: e3401007     	movt	r1, #0x7
   2aebc: ebfff339     	bl	0x27ba8
   2aec0: e3090fec     	movw	r0, #0x9fec
   2aec4: e3400009     	movt	r0, #0x9
   2aec8: e28d1060     	add	r1, sp, #96
   2aecc: e3a02000     	mov	r2, #0
   2aed0: eb011412     	bl	0x6ff20
   2aed4: e59d0060     	ldr	r0, [sp, #0x60]
   2aed8: e28d3068     	add	r3, sp, #104
   2aedc: e1500003     	cmp	r0, r3
   2aee0: 0a000000     	beq	0x2aee8
   2aee4: ebffabd5     	bl	0x15e40    @ imm = #-0x150ac ; _ZdlPv
   2aee8: e28d1078     	add	r1, sp, #120
   2aeec: e1a0000a     	mov	r0, r10
   2aef0: ebfffc89     	bl	0x2a11c
   2aef4: e28dd09c     	add	sp, sp, #156
   2aef8: ecbd8b02     	vpop	{d8}
   2aefc: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   2af00: e59d4020     	ldr	r4, [sp, #0x20]
   2af04: e3a03003     	mov	r3, #3
   2af08: ed9f0a33     	vldr	s0, [pc, #204]          @ 0x2afdc ; float 1000
   2af0c: eef10a0c     	vmov.f32	s1, #7.000000e+00
   2af10: e1a00004     	mov	r0, r4
   2af14: eafffee4     	b	0x2aaac
   2af18: e59d0078     	ldr	r0, [sp, #0x78]
   2af1c: e28d3080     	add	r3, sp, #128
   2af20: e1500003     	cmp	r0, r3
   2af24: 0a000000     	beq	0x2af2c
   2af28: ebffabc4     	bl	0x15e40    @ imm = #-0x150f0 ; _ZdlPv
   2af2c: ebffac0b     	bl	0x15f60    @ imm = #-0x14fd4 ; __cxa_end_cleanup
   2af30: e59d0060     	ldr	r0, [sp, #0x60]
   2af34: e28d3068     	add	r3, sp, #104
   2af38: e1500003     	cmp	r0, r3
   2af3c: 1afffff9     	bne	0x2af28
   2af40: eafffff9     	b	0x2af2c
   2af44: e59d0060     	ldr	r0, [sp, #0x60]
   2af48: e28d3068     	add	r3, sp, #104
   2af4c: e1500003     	cmp	r0, r3
   2af50: 0a000000     	beq	0x2af58
   2af54: ebffabb9     	bl	0x15e40    @ imm = #-0x1511c ; _ZdlPv
   2af58: e59d0048     	ldr	r0, [sp, #0x48]
   2af5c: e28d3050     	add	r3, sp, #80
   2af60: e1500003     	cmp	r0, r3
   2af64: 0a000000     	beq	0x2af6c
   2af68: ebffabb4     	bl	0x15e40    @ imm = #-0x15130 ; _ZdlPv
   2af6c: e59d0030     	ldr	r0, [sp, #0x30]
   2af70: e28d3038     	add	r3, sp, #56
   2af74: e1500003     	cmp	r0, r3
   2af78: 1affffea     	bne	0x2af28
   2af7c: eaffffea     	b	0x2af2c
   2af80: eafffff4     	b	0x2af58
   2af84: eafffff8     	b	0x2af6c
   2af88: eaffffed     	b	0x2af44
   2af8c: eafffff1     	b	0x2af58
   2af90: eafffff5     	b	0x2af6c
   2af94: eaffffea     	b	0x2af44
   2af98: eaffffee     	b	0x2af58
   2af9c: eafffff2     	b	0x2af6c
   2afa0: eaffffe7     	b	0x2af44
   2afa4: eaffffeb     	b	0x2af58
   2afa8: eaffffef     	b	0x2af6c
   2afac: eaffffe4     	b	0x2af44
   2afb0: eaffffe8     	b	0x2af58
   2afb4: eaffffec     	b	0x2af6c
   2afb8: eaffffe1     	b	0x2af44
   2afbc: eaffffe5     	b	0x2af58
   2afc0: eaffffe9     	b	0x2af6c
   2afc4: eaffffde     	b	0x2af44
   2afc8: eaffffe2     	b	0x2af58
   2afcc: eaffffe6     	b	0x2af6c
   2afd0: eaffffdb     	b	0x2af44
   2afd4: eaffffdf     	b	0x2af58
   2afd8: eaffffe3     	b	0x2af6c
   2afdc: 00 00 7a 44  	.word	0x447a0000
