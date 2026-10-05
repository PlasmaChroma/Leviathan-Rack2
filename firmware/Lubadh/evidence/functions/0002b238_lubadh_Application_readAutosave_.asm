; lubadh::Application::readAutosave()
; VA 0x2b238 size 13136

   2b238: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   2b23c: e2802ba9     	add	r2, r0, #173056
   2b240: e3043ab8     	movw	r3, #0x4ab8
   2b244: e3403005     	movt	r3, #0x5
   2b248: e24ddb11     	sub	sp, sp, #17408
   2b24c: e24dd0a4     	sub	sp, sp, #164
   2b250: e2807048     	add	r7, r0, #72
   2b254: e0803003     	add	r3, r0, r3
   2b258: e2822f43     	add	r2, r2, #268
   2b25c: e58d3018     	str	r3, [sp, #0x18]
   2b260: e3093fec     	movw	r3, #0x9fec
   2b264: e3403009     	movt	r3, #0x9
   2b268: e58d200c     	str	r2, [sp, #0xc]
   2b26c: e58d3008     	str	r3, [sp, #0x8]
   2b270: e30a38c0     	movw	r3, #0xa8c0
   2b274: e3403708     	movt	r3, #0x708
   2b278: e58d3014     	str	r3, [sp, #0x14]
   2b27c: e59f2c0c     	ldr	r2, [pc, #0xc0c]        @ 0x2be90
   2b280: e28d3e4a     	add	r3, sp, #1184
   2b284: e2430fff     	sub	r0, r3, #1020
   2b288: e2421018     	sub	r1, r2, #24
   2b28c: eb000e3a     	bl	0x2eb7c
   2b290: e28d9e1a     	add	r9, sp, #416
   2b294: e5972008     	ldr	r2, [r7, #0x8]
   2b298: e24940fc     	sub	r4, r9, #252
   2b29c: e5971004     	ldr	r1, [r7, #0x4]
   2b2a0: e1a00004     	mov	r0, r4
   2b2a4: ebffaaf7     	bl	0x15e88    @ imm = #-0x15424 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b2a8: e28dae2a     	add	r10, sp, #672
   2b2ac: e1a01000     	mov	r1, r0
   2b2b0: e24a50e0     	sub	r5, r10, #224
   2b2b4: e1a00005     	mov	r0, r5
   2b2b8: ebffaa50     	bl	0x15c00    @ imm = #-0x156c0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b2bc: e3a0202f     	mov	r2, #47
   2b2c0: e59d11c4     	ldr	r1, [sp, #0x1c4]
   2b2c4: e1a00005     	mov	r0, r5
   2b2c8: e58d2000     	str	r2, [sp]
   2b2cc: e3a03001     	mov	r3, #1
   2b2d0: e3a02000     	mov	r2, #0
   2b2d4: ebffaa2e     	bl	0x15b94    @ imm = #-0x15748 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE14_M_replace_auxEjjjc
   2b2d8: e28d80a0     	add	r8, sp, #160
   2b2dc: e1a01000     	mov	r1, r0
   2b2e0: e248b02c     	sub	r11, r8, #44
   2b2e4: e1a0000b     	mov	r0, r11
   2b2e8: ebffaa44     	bl	0x15c00    @ imm = #-0x156f0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b2ec: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2b2f0: e24a30d8     	sub	r3, r10, #216
   2b2f4: e1500003     	cmp	r0, r3
   2b2f8: 0a000000     	beq	0x2b300
   2b2fc: ebffaacf     	bl	0x15e40    @ imm = #-0x154c4 ; _ZdlPv
   2b300: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b304: e24930f4     	sub	r3, r9, #244
   2b308: e1500003     	cmp	r0, r3
   2b30c: 0a000000     	beq	0x2b314
   2b310: ebffaaca     	bl	0x15e40    @ imm = #-0x154d8 ; _ZdlPv
   2b314: e28d3e4a     	add	r3, sp, #1184
   2b318: e59f2b74     	ldr	r2, [pc, #0xb74]        @ 0x2be94
   2b31c: e1a0100b     	mov	r1, r11
   2b320: e2430fff     	sub	r0, r3, #1020
   2b324: eb000e14     	bl	0x2eb7c
   2b328: e3a0200c     	mov	r2, #12
   2b32c: e1a01004     	mov	r1, r4
   2b330: e1a00005     	mov	r0, r5
   2b334: ebffabdb     	bl	0x162a8    @ imm = #-0x15094 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEEC1ERKNSt7__cxx1112basic_stringIcS1_SaIcEEESt13_Ios_Openmode
   2b338: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b33c: e24930f4     	sub	r3, r9, #244
   2b340: e1500003     	cmp	r0, r3
   2b344: 0a000000     	beq	0x2b34c
   2b348: ebffaabc     	bl	0x15e40    @ imm = #-0x15510 ; _ZdlPv
   2b34c: e248107d     	sub	r1, r8, #125
   2b350: e1a00005     	mov	r0, r5
   2b354: ebffab61     	bl	0x160e0    @ imm = #-0x1527c ; _ZNSi3getERc
   2b358: e5dd3023     	ldrb	r3, [sp, #0x23]
   2b35c: e2433031     	sub	r3, r3, #49
   2b360: e3530001     	cmp	r3, #1
   2b364: 8a0008ac     	bhi	0x2d61c
   2b368: e1a00005     	mov	r0, r5
   2b36c: ebffa98d     	bl	0x159a8     @ imm = #-0x159cc ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2b370: e59d1074     	ldr	r1, [sp, #0x74]
   2b374: e59d2078     	ldr	r2, [sp, #0x78]
   2b378: e1a00005     	mov	r0, r5
   2b37c: e3a03000     	mov	r3, #0
   2b380: e24ac0d8     	sub	r12, r10, #216
   2b384: e0812002     	add	r2, r1, r2
   2b388: e5dd6023     	ldrb	r6, [sp, #0x23]
   2b38c: e58dc1c0     	str	r12, [sp, #0x1c0]
   2b390: eb000dcd     	bl	0x2eacc
   2b394: e59d11c4     	ldr	r1, [sp, #0x1c4]
   2b398: e3a03001     	mov	r3, #1
   2b39c: e3a02000     	mov	r2, #0
   2b3a0: e1a00005     	mov	r0, r5
   2b3a4: e58d6000     	str	r6, [sp]
   2b3a8: ebffa9f9     	bl	0x15b94    @ imm = #-0x1581c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE14_M_replace_auxEjjjc
   2b3ac: e30f3810     	movw	r3, #0xf810
   2b3b0: e3403008     	movt	r3, #0x8
   2b3b4: e1a00005     	mov	r0, r5
   2b3b8: e5932458     	ldr	r2, [r3, #0x458]
   2b3bc: e5931454     	ldr	r1, [r3, #0x454]
   2b3c0: ebffaab0     	bl	0x15e88    @ imm = #-0x15540 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b3c4: e2486014     	sub	r6, r8, #20
   2b3c8: e1a01000     	mov	r1, r0
   2b3cc: e1a00006     	mov	r0, r6
   2b3d0: ebffaa0a     	bl	0x15c00    @ imm = #-0x157d8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b3d4: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2b3d8: e24a30d8     	sub	r3, r10, #216
   2b3dc: e1500003     	cmp	r0, r3
   2b3e0: 0a000000     	beq	0x2b3e8
   2b3e4: ebffaa95     	bl	0x15e40    @ imm = #-0x155ac ; _ZdlPv
   2b3e8: e3a02002     	mov	r2, #2
   2b3ec: e1a01006     	mov	r1, r6
   2b3f0: e1a00005     	mov	r0, r5
   2b3f4: eb000d76     	bl	0x2e9d4
   2b3f8: e28d3e4a     	add	r3, sp, #1184
   2b3fc: e1a01005     	mov	r1, r5
   2b400: e2430e47     	sub	r0, r3, #1136
   2b404: e240000c     	sub	r0, r0, #12
   2b408: ebffa9b4     	bl	0x15ae0    @ imm = #-0x15930 ; _ZNSt10filesystem6statusERKNS_7__cxx114pathE
   2b40c: e5ddb024     	ldrb	r11, [sp, #0x24]
   2b410: e59d11d8     	ldr	r1, [sp, #0x1d8]
   2b414: e28bb001     	add	r11, r11, #1
   2b418: e3510000     	cmp	r1, #0
   2b41c: e6efb07b     	uxtb	r11, r11
   2b420: 0a000001     	beq	0x2b42c
   2b424: e24a00c8     	sub	r0, r10, #200
   2b428: ebffa976     	bl	0x15a08    @ imm = #-0x15a28 ; _ZNKSt10filesystem7__cxx114path5_List13_Impl_deleterclEPNS2_5_ImplE
   2b42c: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2b430: e24a30d8     	sub	r3, r10, #216
   2b434: e1500003     	cmp	r0, r3
   2b438: 0a000000     	beq	0x2b440
   2b43c: ebffaa7f     	bl	0x15e40    @ imm = #-0x15604 ; _ZdlPv
   2b440: e35b0001     	cmp	r11, #1
   2b444: e2871004     	add	r1, r7, #4
   2b448: 9a0008a0     	bls	0x2d6d0
   2b44c: e28d3e4a     	add	r3, sp, #1184
   2b450: e30121f4     	movw	r2, #0x11f4
   2b454: e3402007     	movt	r2, #0x7
   2b458: e2430fff     	sub	r0, r3, #1020
   2b45c: eb000dd9     	bl	0x2ebc8
   2b460: e59d2090     	ldr	r2, [sp, #0x90]
   2b464: e1a00004     	mov	r0, r4
   2b468: e59d108c     	ldr	r1, [sp, #0x8c]
   2b46c: ebffaa85     	bl	0x15e88    @ imm = #-0x155ec ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b470: e1a01000     	mov	r1, r0
   2b474: e1a00005     	mov	r0, r5
   2b478: ebffa9e0     	bl	0x15c00    @ imm = #-0x15880 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b47c: e59d0008     	ldr	r0, [sp, #0x8]
   2b480: e3a02000     	mov	r2, #0
   2b484: e1a01005     	mov	r1, r5
   2b488: eb0112a4     	bl	0x6ff20
   2b48c: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2b490: e24a30d8     	sub	r3, r10, #216
   2b494: e1500003     	cmp	r0, r3
   2b498: 0a000000     	beq	0x2b4a0
   2b49c: ebffaa67     	bl	0x15e40    @ imm = #-0x15664 ; _ZdlPv
   2b4a0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b4a4: e24990f4     	sub	r9, r9, #244
   2b4a8: e1500009     	cmp	r0, r9
   2b4ac: 0a000000     	beq	0x2b4b4
   2b4b0: ebffaa62     	bl	0x15e40    @ imm = #-0x15678 ; _ZdlPv
   2b4b4: e1a01006     	mov	r1, r6
   2b4b8: e3a0200c     	mov	r2, #12
   2b4bc: e1a00005     	mov	r0, r5
   2b4c0: ebffab78     	bl	0x162a8    @ imm = #-0x15220 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEEC1ERKNSt7__cxx1112basic_stringIcS1_SaIcEEESt13_Ios_Openmode
   2b4c4: e59d300c     	ldr	r3, [sp, #0xc]
   2b4c8: e1a00005     	mov	r0, r5
   2b4cc: e59d2014     	ldr	r2, [sp, #0x14]
   2b4d0: e5131500     	ldr	r1, [r3, #-0x500]
   2b4d4: ebffab3d     	bl	0x161d0    @ imm = #-0x1530c ; _ZNSi4readEPci
   2b4d8: e1a00005     	mov	r0, r5
   2b4dc: ebffa931     	bl	0x159a8     @ imm = #-0x15b3c ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2b4e0: e59d008c     	ldr	r0, [sp, #0x8c]
   2b4e4: e248300c     	sub	r3, r8, #12
   2b4e8: e1500003     	cmp	r0, r3
   2b4ec: 0a000000     	beq	0x2b4f4
   2b4f0: ebffaa52     	bl	0x15e40    @ imm = #-0x156b8 ; _ZdlPv
   2b4f4: e59d0074     	ldr	r0, [sp, #0x74]
   2b4f8: e2488024     	sub	r8, r8, #36
   2b4fc: e1500008     	cmp	r0, r8
   2b500: 0a000000     	beq	0x2b508
   2b504: ebffaa4d     	bl	0x15e40    @ imm = #-0x156cc ; _ZdlPv
   2b508: f2c00010     	vmov.i32	d16, #0x0
   2b50c: f2c71f10     	vmov.f32	d17, #1.000000e+00
   2b510: e28d3e4a     	add	r3, sp, #1184
   2b514: e28d2f73     	add	r2, sp, #460
   2b518: e2430fad     	sub	r0, r3, #692
   2b51c: e2433faf     	sub	r3, r3, #700
   2b520: e30f1cf8     	movw	r1, #0xfcf8
   2b524: e3401008     	movt	r1, #0x8
   2b528: f442078f     	vst1.32	{d16}, [r2]
   2b52c: e304229c     	movw	r2, #0x429c
   2b530: f440078f     	vst1.32	{d16}, [r0]
   2b534: e28d0c02     	add	r0, sp, #512
   2b538: f443178f     	vst1.32	{d17}, [r3]
   2b53c: e3a0c5fe     	mov	r12, #1065353216
   2b540: e3a03000     	mov	r3, #0
   2b544: e58dc1dc     	str	r12, [sp, #0x1dc]
   2b548: e58d31c4     	str	r3, [sp, #0x1c4]
   2b54c: e3a0c001     	mov	r12, #1
   2b550: e5cd31f8     	strb	r3, [sp, #0x1f8]
   2b554: e5cdc1c0     	strb	r12, [sp, #0x1c0]
   2b558: e58dc1d4     	str	r12, [sp, #0x1d4]
   2b55c: e58dc1d8     	str	r12, [sp, #0x1d8]
   2b560: e3a0c002     	mov	r12, #2
   2b564: e58d31fc     	str	r3, [sp, #0x1fc]
   2b568: e3003333     	movw	r3, #0x333
   2b56c: e58dc1c8     	str	r12, [sp, #0x1c8]
   2b570: e58d31e0     	str	r3, [sp, #0x1e0]
   2b574: ebffaab2     	bl	0x16044    @ imm = #-0x15538 ; memcpy
   2b578: e59f2918     	ldr	r2, [pc, #0x918]        @ 0x2be98
   2b57c: e28d3e4a     	add	r3, sp, #1184
   2b580: e2430e41     	sub	r0, r3, #1040
   2b584: e2400004     	sub	r0, r0, #4
   2b588: e2421030     	sub	r1, r2, #48
   2b58c: eb000d7a     	bl	0x2eb7c
   2b590: e28d80a0     	add	r8, sp, #160
   2b594: e5972008     	ldr	r2, [r7, #0x8]
   2b598: e2486014     	sub	r6, r8, #20
   2b59c: e5971004     	ldr	r1, [r7, #0x4]
   2b5a0: e1a00006     	mov	r0, r6
   2b5a4: ebffaa37     	bl	0x15e88    @ imm = #-0x15724 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b5a8: e28d9e1a     	add	r9, sp, #416
   2b5ac: e1a01000     	mov	r1, r0
   2b5b0: e24940fc     	sub	r4, r9, #252
   2b5b4: e1a00004     	mov	r0, r4
   2b5b8: ebffa990     	bl	0x15c00    @ imm = #-0x159c0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b5bc: e3a0202f     	mov	r2, #47
   2b5c0: e59d10a8     	ldr	r1, [sp, #0xa8]
   2b5c4: e1a00004     	mov	r0, r4
   2b5c8: e58d2000     	str	r2, [sp]
   2b5cc: e3a03001     	mov	r3, #1
   2b5d0: e3a02000     	mov	r2, #0
   2b5d4: ebffa96e     	bl	0x15b94    @ imm = #-0x15a48 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE14_M_replace_auxEjjjc
   2b5d8: e1a01000     	mov	r1, r0
   2b5dc: e28d005c     	add	r0, sp, #92
   2b5e0: ebffa986     	bl	0x15c00    @ imm = #-0x159e8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b5e4: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b5e8: e24930f4     	sub	r3, r9, #244
   2b5ec: e1500003     	cmp	r0, r3
   2b5f0: 0a000000     	beq	0x2b5f8
   2b5f4: ebffaa11     	bl	0x15e40    @ imm = #-0x157bc ; _ZdlPv
   2b5f8: e59d008c     	ldr	r0, [sp, #0x8c]
   2b5fc: e248300c     	sub	r3, r8, #12
   2b600: e1500003     	cmp	r0, r3
   2b604: 0a000000     	beq	0x2b60c
   2b608: ebffaa0c     	bl	0x15e40    @ imm = #-0x157d0 ; _ZdlPv
   2b60c: e28d3e4a     	add	r3, sp, #1184
   2b610: e59f287c     	ldr	r2, [pc, #0x87c]        @ 0x2be94
   2b614: e2430e41     	sub	r0, r3, #1040
   2b618: e28d105c     	add	r1, sp, #92
   2b61c: e2400004     	sub	r0, r0, #4
   2b620: eb000d55     	bl	0x2eb7c
   2b624: e3a0200c     	mov	r2, #12
   2b628: e1a01006     	mov	r1, r6
   2b62c: e1a00004     	mov	r0, r4
   2b630: ebffab1c     	bl	0x162a8    @ imm = #-0x15390 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEEC1ERKNSt7__cxx1112basic_stringIcS1_SaIcEEESt13_Ios_Openmode
   2b634: e59d008c     	ldr	r0, [sp, #0x8c]
   2b638: e248300c     	sub	r3, r8, #12
   2b63c: e1500003     	cmp	r0, r3
   2b640: 0a000000     	beq	0x2b648
   2b644: ebffa9fd     	bl	0x15e40    @ imm = #-0x1580c ; _ZdlPv
   2b648: e248107d     	sub	r1, r8, #125
   2b64c: e1a00004     	mov	r0, r4
   2b650: ebffaaa2     	bl	0x160e0    @ imm = #-0x15578 ; _ZNSi3getERc
   2b654: e5dd3023     	ldrb	r3, [sp, #0x23]
   2b658: e2433031     	sub	r3, r3, #49
   2b65c: e3530001     	cmp	r3, #1
   2b660: 8a0008f8     	bhi	0x2da48
   2b664: e1a00004     	mov	r0, r4
   2b668: ebffa8ce     	bl	0x159a8     @ imm = #-0x15cc8 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2b66c: e59d105c     	ldr	r1, [sp, #0x5c]
   2b670: e59d2060     	ldr	r2, [sp, #0x60]
   2b674: e1a00004     	mov	r0, r4
   2b678: e3a03000     	mov	r3, #0
   2b67c: e249c0f4     	sub	r12, r9, #244
   2b680: e0812002     	add	r2, r1, r2
   2b684: e5dd5023     	ldrb	r5, [sp, #0x23]
   2b688: e58dc0a4     	str	r12, [sp, #0xa4]
   2b68c: eb000d0e     	bl	0x2eacc
   2b690: e59d10a8     	ldr	r1, [sp, #0xa8]
   2b694: e3a03001     	mov	r3, #1
   2b698: e3a02000     	mov	r2, #0
   2b69c: e1a00004     	mov	r0, r4
   2b6a0: e58d5000     	str	r5, [sp]
   2b6a4: ebffa93a     	bl	0x15b94    @ imm = #-0x15b18 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE14_M_replace_auxEjjjc
   2b6a8: e30f3810     	movw	r3, #0xf810
   2b6ac: e3403008     	movt	r3, #0x8
   2b6b0: e1a00004     	mov	r0, r4
   2b6b4: e5932470     	ldr	r2, [r3, #0x470]
   2b6b8: e593146c     	ldr	r1, [r3, #0x46c]
   2b6bc: ebffa9f1     	bl	0x15e88    @ imm = #-0x1583c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b6c0: e248b02c     	sub	r11, r8, #44
   2b6c4: e1a01000     	mov	r1, r0
   2b6c8: e1a0000b     	mov	r0, r11
   2b6cc: ebffa94b     	bl	0x15c00    @ imm = #-0x15ad4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b6d0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b6d4: e24930f4     	sub	r3, r9, #244
   2b6d8: e1500003     	cmp	r0, r3
   2b6dc: 0a000000     	beq	0x2b6e4
   2b6e0: ebffa9d6     	bl	0x15e40    @ imm = #-0x158a8 ; _ZdlPv
   2b6e4: e3a02002     	mov	r2, #2
   2b6e8: e1a0100b     	mov	r1, r11
   2b6ec: e1a00004     	mov	r0, r4
   2b6f0: eb000cb7     	bl	0x2e9d4
   2b6f4: e28d3e4a     	add	r3, sp, #1184
   2b6f8: e1a01004     	mov	r1, r4
   2b6fc: e2430e47     	sub	r0, r3, #1136
   2b700: e2400004     	sub	r0, r0, #4
   2b704: ebffa8f5     	bl	0x15ae0    @ imm = #-0x15c2c ; _ZNSt10filesystem6statusERKNS_7__cxx114pathE
   2b708: e5dd502c     	ldrb	r5, [sp, #0x2c]
   2b70c: e59d10bc     	ldr	r1, [sp, #0xbc]
   2b710: e2855001     	add	r5, r5, #1
   2b714: e3510000     	cmp	r1, #0
   2b718: e6ef5075     	uxtb	r5, r5
   2b71c: 0a000001     	beq	0x2b728
   2b720: e24900e4     	sub	r0, r9, #228
   2b724: ebffa8b7     	bl	0x15a08    @ imm = #-0x15d24 ; _ZNKSt10filesystem7__cxx114path5_List13_Impl_deleterclEPNS2_5_ImplE
   2b728: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b72c: e24930f4     	sub	r3, r9, #244
   2b730: e1500003     	cmp	r0, r3
   2b734: 0a000000     	beq	0x2b73c
   2b738: ebffa9c0     	bl	0x15e40    @ imm = #-0x15900 ; _ZdlPv
   2b73c: e2873004     	add	r3, r7, #4
   2b740: e3550001     	cmp	r5, #1
   2b744: e58d3010     	str	r3, [sp, #0x10]
   2b748: 9a000885     	bls	0x2d964
   2b74c: e28d3e4a     	add	r3, sp, #1184
   2b750: e3012254     	movw	r2, #0x1254
   2b754: e3402007     	movt	r2, #0x7
   2b758: e2430e41     	sub	r0, r3, #1040
   2b75c: e2400004     	sub	r0, r0, #4
   2b760: e2871004     	add	r1, r7, #4
   2b764: eb000d17     	bl	0x2ebc8
   2b768: e59d2078     	ldr	r2, [sp, #0x78]
   2b76c: e1a00006     	mov	r0, r6
   2b770: e59d1074     	ldr	r1, [sp, #0x74]
   2b774: ebffa9c3     	bl	0x15e88    @ imm = #-0x158f4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   2b778: e1a01000     	mov	r1, r0
   2b77c: e1a00004     	mov	r0, r4
   2b780: ebffa91e     	bl	0x15c00    @ imm = #-0x15b88 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2b784: e59d0008     	ldr	r0, [sp, #0x8]
   2b788: e3a02000     	mov	r2, #0
   2b78c: e1a01004     	mov	r1, r4
   2b790: eb0111e2     	bl	0x6ff20
   2b794: e59d00a4     	ldr	r0, [sp, #0xa4]
   2b798: e24930f4     	sub	r3, r9, #244
   2b79c: e1500003     	cmp	r0, r3
   2b7a0: 0a000000     	beq	0x2b7a8
   2b7a4: ebffa9a5     	bl	0x15e40    @ imm = #-0x1596c ; _ZdlPv
   2b7a8: e59d008c     	ldr	r0, [sp, #0x8c]
   2b7ac: e248300c     	sub	r3, r8, #12
   2b7b0: e1500003     	cmp	r0, r3
   2b7b4: 0a000000     	beq	0x2b7bc
   2b7b8: ebffa9a0     	bl	0x15e40    @ imm = #-0x15980 ; _ZdlPv
   2b7bc: e1a0100b     	mov	r1, r11
   2b7c0: e28d0034     	add	r0, sp, #52
   2b7c4: e1a02004     	mov	r2, r4
   2b7c8: e3a0c001     	mov	r12, #1
   2b7cc: e3a03000     	mov	r3, #0
   2b7d0: e1cdcab4     	strh	r12, [sp, #164]
   2b7d4: e5cd30a6     	strb	r3, [sp, #0xa6]
   2b7d8: eb009ce7     	bl	0x52b7c
   2b7dc: e3011278     	movw	r1, #0x1278
   2b7e0: e3401007     	movt	r1, #0x7
   2b7e4: e28d0034     	add	r0, sp, #52
   2b7e8: eb00d473     	bl	0x609bc
   2b7ec: eb00d56a     	bl	0x60d9c
   2b7f0: e1903001     	orrs	r3, r0, r1
   2b7f4: 13a03001     	movne	r3, #1
   2b7f8: 03a03000     	moveq	r3, #0
   2b7fc: e5cd31c0     	strb	r3, [sp, #0x1c0]
   2b800: e3011280     	movw	r1, #0x1280
   2b804: e3401007     	movt	r1, #0x7
   2b808: e28d0034     	add	r0, sp, #52
   2b80c: eb00d46a     	bl	0x609bc
   2b810: eb00d561     	bl	0x60d9c
   2b814: e58d01c4     	str	r0, [sp, #0x1c4]
   2b818: e3011288     	movw	r1, #0x1288
   2b81c: e3401007     	movt	r1, #0x7
   2b820: e28d0034     	add	r0, sp, #52
   2b824: eb00d464     	bl	0x609bc
   2b828: e2485058     	sub	r5, r8, #88
   2b82c: e1a01000     	mov	r1, r0
   2b830: e1a00005     	mov	r0, r5
   2b834: eb00eb9e     	bl	0x666b4
   2b838: e3011290     	movw	r1, #0x1290
   2b83c: e3401007     	movt	r1, #0x7
   2b840: e1a00005     	mov	r0, r5
   2b844: eb00d45c     	bl	0x609bc
   2b848: eb00d553     	bl	0x60d9c
   2b84c: e58d01c8     	str	r0, [sp, #0x1c8]
   2b850: e301129c     	movw	r1, #0x129c
   2b854: e3401007     	movt	r1, #0x7
   2b858: e1a00005     	mov	r0, r5
   2b85c: eb00d456     	bl	0x609bc
   2b860: eb00d54d     	bl	0x60d9c
   2b864: e58d01cc     	str	r0, [sp, #0x1cc]
   2b868: e30112a8     	movw	r1, #0x12a8
   2b86c: e3401007     	movt	r1, #0x7
   2b870: e1a00005     	mov	r0, r5
   2b874: eb00d450     	bl	0x609bc
   2b878: eb00d547     	bl	0x60d9c
   2b87c: e58d01d0     	str	r0, [sp, #0x1d0]
   2b880: e1a00005     	mov	r0, r5
   2b884: eb00d662     	bl	0x61214
   2b888: e30112b0     	movw	r1, #0x12b0
   2b88c: e3401007     	movt	r1, #0x7
   2b890: e28d0034     	add	r0, sp, #52
   2b894: eb00d448     	bl	0x609bc
   2b898: e2485058     	sub	r5, r8, #88
   2b89c: e1a01000     	mov	r1, r0
   2b8a0: e1a00005     	mov	r0, r5
   2b8a4: eb00eb82     	bl	0x666b4
   2b8a8: e30112b8     	movw	r1, #0x12b8
   2b8ac: e3401007     	movt	r1, #0x7
   2b8b0: e1a00005     	mov	r0, r5
   2b8b4: eb00d440     	bl	0x609bc
   2b8b8: eb00d537     	bl	0x60d9c
   2b8bc: e58d01d4     	str	r0, [sp, #0x1d4]
   2b8c0: e30112c0     	movw	r1, #0x12c0
   2b8c4: e3401007     	movt	r1, #0x7
   2b8c8: e1a00005     	mov	r0, r5
   2b8cc: eb00d43a     	bl	0x609bc
   2b8d0: eb00d531     	bl	0x60d9c
   2b8d4: e58d01d8     	str	r0, [sp, #0x1d8]
   2b8d8: e30112c8     	movw	r1, #0x12c8
   2b8dc: e3401007     	movt	r1, #0x7
   2b8e0: e1a00005     	mov	r0, r5
   2b8e4: eb00d434     	bl	0x609bc
   2b8e8: eb00d4e2     	bl	0x60c78
   2b8ec: eeb70bc0     	vcvt.f32.f64	s0, d0
   2b8f0: ed8d0a77     	vstr	s0, [sp, #476]
   2b8f4: e30112d4     	movw	r1, #0x12d4
   2b8f8: e3401007     	movt	r1, #0x7
   2b8fc: e1a00005     	mov	r0, r5
   2b900: eb00d42d     	bl	0x609bc
   2b904: eb00d524     	bl	0x60d9c
   2b908: e58d01e0     	str	r0, [sp, #0x1e0]
   2b90c: e30112dc     	movw	r1, #0x12dc
   2b910: e3401007     	movt	r1, #0x7
   2b914: e1a00005     	mov	r0, r5
   2b918: eb00d427     	bl	0x609bc
   2b91c: eb00d4d5     	bl	0x60c78
   2b920: eeb70bc0     	vcvt.f32.f64	s0, d0
   2b924: ed8d0a79     	vstr	s0, [sp, #484]
   2b928: e30112e4     	movw	r1, #0x12e4
   2b92c: e3401007     	movt	r1, #0x7
   2b930: e1a00005     	mov	r0, r5
   2b934: eb00d420     	bl	0x609bc
   2b938: eb00d4ce     	bl	0x60c78
   2b93c: eeb70bc0     	vcvt.f32.f64	s0, d0
   2b940: ed8d0a7a     	vstr	s0, [sp, #488]
   2b944: e30112ec     	movw	r1, #0x12ec
   2b948: e3401007     	movt	r1, #0x7
   2b94c: e1a00005     	mov	r0, r5
   2b950: eb00d419     	bl	0x609bc
   2b954: eb00d510     	bl	0x60d9c
   2b958: e58d01ec     	str	r0, [sp, #0x1ec]
   2b95c: e30112f4     	movw	r1, #0x12f4
   2b960: e3401007     	movt	r1, #0x7
   2b964: e1a00005     	mov	r0, r5
   2b968: eb00d413     	bl	0x609bc
   2b96c: eb00d50a     	bl	0x60d9c
   2b970: e58d01f0     	str	r0, [sp, #0x1f0]
   2b974: e30112fc     	movw	r1, #0x12fc
   2b978: e3401007     	movt	r1, #0x7
   2b97c: e1a00005     	mov	r0, r5
   2b980: eb00d40d     	bl	0x609bc
   2b984: eb00d504     	bl	0x60d9c
   2b988: e1903001     	orrs	r3, r0, r1
   2b98c: 13a03001     	movne	r3, #1
   2b990: 03a03000     	moveq	r3, #0
   2b994: e5cd31f8     	strb	r3, [sp, #0x1f8]
   2b998: e301130c     	movw	r1, #0x130c
   2b99c: e3401007     	movt	r1, #0x7
   2b9a0: e1a00005     	mov	r0, r5
   2b9a4: eb00d404     	bl	0x609bc
   2b9a8: eb00d4fb     	bl	0x60d9c
   2b9ac: e58d01fc     	str	r0, [sp, #0x1fc]
   2b9b0: e1a00005     	mov	r0, r5
   2b9b4: eb00d616     	bl	0x61214
   2b9b8: e3011338     	movw	r1, #0x1338
   2b9bc: e3401007     	movt	r1, #0x7
   2b9c0: e28d0034     	add	r0, sp, #52
   2b9c4: eb00d3fc     	bl	0x609bc
   2b9c8: e1a01000     	mov	r1, r0
   2b9cc: e1a00006     	mov	r0, r6
   2b9d0: eb00eb37     	bl	0x666b4
   2b9d4: e301131c     	movw	r1, #0x131c
   2b9d8: e3401007     	movt	r1, #0x7
   2b9dc: e1a00006     	mov	r0, r6
   2b9e0: eb00d3f5     	bl	0x609bc
   2b9e4: eb00d4ec     	bl	0x60d9c
   2b9e8: e1a03000     	mov	r3, r0
   2b9ec: e3011324     	movw	r1, #0x1324
   2b9f0: e3401007     	movt	r1, #0x7
   2b9f4: e1a00004     	mov	r0, r4
   2b9f8: e58d3200     	str	r3, [sp, #0x200]
   2b9fc: ebfff069     	bl	0x27ba8
   2ba00: e59d0008     	ldr	r0, [sp, #0x8]
   2ba04: e3a02000     	mov	r2, #0
   2ba08: e1a01004     	mov	r1, r4
   2ba0c: eb011143     	bl	0x6ff20
   2ba10: e59d00a4     	ldr	r0, [sp, #0xa4]
   2ba14: e24930f4     	sub	r3, r9, #244
   2ba18: e1500003     	cmp	r0, r3
   2ba1c: 0a000000     	beq	0x2ba24
   2ba20: ebffa906     	bl	0x15e40    @ imm = #-0x15be8 ; _ZdlPv
   2ba24: e3011340     	movw	r1, #0x1340
   2ba28: e3401007     	movt	r1, #0x7
   2ba2c: e1a00006     	mov	r0, r6
   2ba30: eb00d3e1     	bl	0x609bc
   2ba34: eb00d4d8     	bl	0x60d9c
   2ba38: e58d0204     	str	r0, [sp, #0x204]
   2ba3c: e301134c     	movw	r1, #0x134c
   2ba40: e3401007     	movt	r1, #0x7
   2ba44: e1a00006     	mov	r0, r6
   2ba48: eb00d3db     	bl	0x609bc
   2ba4c: e301135c     	movw	r1, #0x135c
   2ba50: e3401007     	movt	r1, #0x7
   2ba54: eb00d3d8     	bl	0x609bc
   2ba58: eb00d4cf     	bl	0x60d9c
   2ba5c: e58d020c     	str	r0, [sp, #0x20c]
   2ba60: e301134c     	movw	r1, #0x134c
   2ba64: e3401007     	movt	r1, #0x7
   2ba68: e1a00006     	mov	r0, r6
   2ba6c: eb00d3d2     	bl	0x609bc
   2ba70: e3011364     	movw	r1, #0x1364
   2ba74: e3401007     	movt	r1, #0x7
   2ba78: eb00d3cf     	bl	0x609bc
   2ba7c: eb00d4c6     	bl	0x60d9c
   2ba80: e58d0210     	str	r0, [sp, #0x210]
   2ba84: e301136c     	movw	r1, #0x136c
   2ba88: e3401007     	movt	r1, #0x7
   2ba8c: e1a00006     	mov	r0, r6
   2ba90: eb00d3c9     	bl	0x609bc
   2ba94: eb00d4c0     	bl	0x60d9c
   2ba98: e58d0208     	str	r0, [sp, #0x208]
   2ba9c: e3011378     	movw	r1, #0x1378
   2baa0: e3401007     	movt	r1, #0x7
   2baa4: e1a00006     	mov	r0, r6
   2baa8: eb00d3c3     	bl	0x609bc
   2baac: eb00d4ba     	bl	0x60d9c
   2bab0: e58d0214     	str	r0, [sp, #0x214]
   2bab4: e3011388     	movw	r1, #0x1388
   2bab8: e3401007     	movt	r1, #0x7
   2babc: e1a00006     	mov	r0, r6
   2bac0: eb00d3bd     	bl	0x609bc
   2bac4: eb00d4b4     	bl	0x60d9c
   2bac8: e58d0218     	str	r0, [sp, #0x218]
   2bacc: e3011394     	movw	r1, #0x1394
   2bad0: e3401007     	movt	r1, #0x7
   2bad4: e1a00006     	mov	r0, r6
   2bad8: eb00d3b7     	bl	0x609bc
   2badc: e30113a4     	movw	r1, #0x13a4
   2bae0: e3401007     	movt	r1, #0x7
   2bae4: eb00d3b4     	bl	0x609bc
   2bae8: eb00d4ab     	bl	0x60d9c
   2baec: e58d021c     	str	r0, [sp, #0x21c]
   2baf0: e3011394     	movw	r1, #0x1394
   2baf4: e3401007     	movt	r1, #0x7
   2baf8: e1a00006     	mov	r0, r6
   2bafc: eb00d3ae     	bl	0x609bc
   2bb00: e30113ac     	movw	r1, #0x13ac
   2bb04: e3401007     	movt	r1, #0x7
   2bb08: eb00d3ab     	bl	0x609bc
   2bb0c: eb00d4a2     	bl	0x60d9c
   2bb10: e58d0220     	str	r0, [sp, #0x220]
   2bb14: e3011394     	movw	r1, #0x1394
   2bb18: e3401007     	movt	r1, #0x7
   2bb1c: e1a00006     	mov	r0, r6
   2bb20: eb00d3a5     	bl	0x609bc
   2bb24: e30113b8     	movw	r1, #0x13b8
   2bb28: e3401007     	movt	r1, #0x7
   2bb2c: eb00d3a2     	bl	0x609bc
   2bb30: eb00d499     	bl	0x60d9c
   2bb34: e58d0224     	str	r0, [sp, #0x224]
   2bb38: e30113c0     	movw	r1, #0x13c0
   2bb3c: e3401007     	movt	r1, #0x7
   2bb40: e1a00006     	mov	r0, r6
   2bb44: eb00d39c     	bl	0x609bc
   2bb48: eb00d493     	bl	0x60d9c
   2bb4c: e58d0228     	str	r0, [sp, #0x228]
   2bb50: e30113cc     	movw	r1, #0x13cc
   2bb54: e3401007     	movt	r1, #0x7
   2bb58: e1a00006     	mov	r0, r6
   2bb5c: eb00d396     	bl	0x609bc
   2bb60: eb00d48d     	bl	0x60d9c
   2bb64: e58d022c     	str	r0, [sp, #0x22c]
   2bb68: e30113d4     	movw	r1, #0x13d4
   2bb6c: e3401007     	movt	r1, #0x7
   2bb70: e1a00006     	mov	r0, r6
   2bb74: eb00d390     	bl	0x609bc
   2bb78: eb00d487     	bl	0x60d9c
   2bb7c: e58d0230     	str	r0, [sp, #0x230]
   2bb80: e30113e0     	movw	r1, #0x13e0
   2bb84: e3401007     	movt	r1, #0x7
   2bb88: e1a00006     	mov	r0, r6
   2bb8c: eb00d38a     	bl	0x609bc
   2bb90: eb00d481     	bl	0x60d9c
   2bb94: ebffa8c4     	bl	0x15eac    @ imm = #-0x15cf0 ; __aeabi_l2f
   2bb98: ed9f7abb     	vldr	s14, [pc, #748]         @ 0x2be8c ; float 1000
   2bb9c: ee060a90     	vmov	s13, r0
   2bba0: eec67a87     	vdiv.f32	s15, s13, s14
   2bba4: edcd7a8d     	vstr	s15, [sp, #564]
   2bba8: e30113f4     	movw	r1, #0x13f4
   2bbac: e3401007     	movt	r1, #0x7
   2bbb0: e1a00006     	mov	r0, r6
   2bbb4: eb00d380     	bl	0x609bc
   2bbb8: eb00d477     	bl	0x60d9c
   2bbbc: e58d0238     	str	r0, [sp, #0x238]
   2bbc0: e3011400     	movw	r1, #0x1400
   2bbc4: e3401007     	movt	r1, #0x7
   2bbc8: e1a00006     	mov	r0, r6
   2bbcc: eb00d37a     	bl	0x609bc
   2bbd0: eb00d428     	bl	0x60c78
   2bbd4: e28d3b11     	add	r3, sp, #17408
   2bbd8: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bbdc: e283306c     	add	r3, r3, #108
   2bbe0: ed830a00     	vstr	s0, [r3]
   2bbe4: e301140c     	movw	r1, #0x140c
   2bbe8: e3401007     	movt	r1, #0x7
   2bbec: e1a00006     	mov	r0, r6
   2bbf0: eb00d371     	bl	0x609bc
   2bbf4: eb00d468     	bl	0x60d9c
   2bbf8: e58d023c     	str	r0, [sp, #0x23c]
   2bbfc: e3011420     	movw	r1, #0x1420
   2bc00: e3401007     	movt	r1, #0x7
   2bc04: e1a00006     	mov	r0, r6
   2bc08: eb00d36b     	bl	0x609bc
   2bc0c: eb00d462     	bl	0x60d9c
   2bc10: e58d0240     	str	r0, [sp, #0x240]
   2bc14: e3011430     	movw	r1, #0x1430
   2bc18: e3401007     	movt	r1, #0x7
   2bc1c: e1a00006     	mov	r0, r6
   2bc20: eb00d365     	bl	0x609bc
   2bc24: e1a01000     	mov	r1, r0
   2bc28: e1a00004     	mov	r0, r4
   2bc2c: eb00eaa0     	bl	0x666b4
   2bc30: e1a00004     	mov	r0, r4
   2bc34: eb00d063     	bl	0x5fdc8
   2bc38: e3500006     	cmp	r0, #6
   2bc3c: 1a0007a8     	bne	0x2dae4
   2bc40: e3a05000     	mov	r5, #0
   2bc44: e24a005c     	sub	r0, r10, #92
   2bc48: e3a02e1e     	mov	r2, #480
   2bc4c: e1a01005     	mov	r1, r5
   2bc50: e1a0b000     	mov	r11, r0
   2bc54: ebffa846     	bl	0x15d74    @ imm = #-0x15ee8 ; memset
   2bc58: ea000007     	b	0x2bc7c
   2bc5c: e1a01005     	mov	r1, r5
   2bc60: e1a00004     	mov	r0, r4
   2bc64: eb00e929     	bl	0x66110
   2bc68: eb00cfa5     	bl	0x5fb04
   2bc6c: e3550078     	cmp	r5, #120
   2bc70: 0a000143     	beq	0x2c184
   2bc74: e2855001     	add	r5, r5, #1
   2bc78: ecab0a01     	vstmia	r11!, {s0}
   2bc7c: e1a00004     	mov	r0, r4
   2bc80: eb00d061     	bl	0x5fe0c
   2bc84: e1550000     	cmp	r5, r0
   2bc88: 3afffff3     	blo	0x2bc5c
   2bc8c: e1a00004     	mov	r0, r4
   2bc90: eb00d55f     	bl	0x61214
   2bc94: e3011494     	movw	r1, #0x1494
   2bc98: e3401007     	movt	r1, #0x7
   2bc9c: e1a00006     	mov	r0, r6
   2bca0: eb00d345     	bl	0x609bc
   2bca4: eb00d43c     	bl	0x60d9c
   2bca8: e28d5dd2     	add	r5, sp, #13440
   2bcac: e2855020     	add	r5, r5, #32
   2bcb0: e5850f84     	str	r0, [r5, #0xf84]
   2bcb4: e30114a4     	movw	r1, #0x14a4
   2bcb8: e3401007     	movt	r1, #0x7
   2bcbc: e1a00006     	mov	r0, r6
   2bcc0: eb00d33d     	bl	0x609bc
   2bcc4: eb00d434     	bl	0x60d9c
   2bcc8: e5850fc8     	str	r0, [r5, #0xfc8]
   2bccc: e30114b4     	movw	r1, #0x14b4
   2bcd0: e3401007     	movt	r1, #0x7
   2bcd4: e1a00006     	mov	r0, r6
   2bcd8: eb00d337     	bl	0x609bc
   2bcdc: eb00d3e5     	bl	0x60c78
   2bce0: e28d3b11     	add	r3, sp, #17408
   2bce4: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bce8: e2833070     	add	r3, r3, #112
   2bcec: ed830a00     	vstr	s0, [r3]
   2bcf0: e30114c4     	movw	r1, #0x14c4
   2bcf4: e3401007     	movt	r1, #0x7
   2bcf8: e1a00006     	mov	r0, r6
   2bcfc: eb00d32e     	bl	0x609bc
   2bd00: eb00d3dc     	bl	0x60c78
   2bd04: e28d3b11     	add	r3, sp, #17408
   2bd08: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bd0c: e2833074     	add	r3, r3, #116
   2bd10: ed830a00     	vstr	s0, [r3]
   2bd14: e30114d4     	movw	r1, #0x14d4
   2bd18: e3401007     	movt	r1, #0x7
   2bd1c: e1a00006     	mov	r0, r6
   2bd20: eb00d325     	bl	0x609bc
   2bd24: eb00d3d3     	bl	0x60c78
   2bd28: e28d3b11     	add	r3, sp, #17408
   2bd2c: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bd30: e2833078     	add	r3, r3, #120
   2bd34: ed830a00     	vstr	s0, [r3]
   2bd38: e30114dc     	movw	r1, #0x14dc
   2bd3c: e3401007     	movt	r1, #0x7
   2bd40: e1a00006     	mov	r0, r6
   2bd44: eb00d31c     	bl	0x609bc
   2bd48: eb00d3ca     	bl	0x60c78
   2bd4c: e28d3b11     	add	r3, sp, #17408
   2bd50: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bd54: e283307c     	add	r3, r3, #124
   2bd58: ed830a00     	vstr	s0, [r3]
   2bd5c: e30114e4     	movw	r1, #0x14e4
   2bd60: e3401007     	movt	r1, #0x7
   2bd64: e1a00006     	mov	r0, r6
   2bd68: eb00d313     	bl	0x609bc
   2bd6c: eb00d3c1     	bl	0x60c78
   2bd70: e28d3b11     	add	r3, sp, #17408
   2bd74: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bd78: e2833080     	add	r3, r3, #128
   2bd7c: ed830a00     	vstr	s0, [r3]
   2bd80: e30114f0     	movw	r1, #0x14f0
   2bd84: e3401007     	movt	r1, #0x7
   2bd88: e1a00006     	mov	r0, r6
   2bd8c: eb00d30a     	bl	0x609bc
   2bd90: eb00d3b8     	bl	0x60c78
   2bd94: e28d3b11     	add	r3, sp, #17408
   2bd98: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bd9c: e2833084     	add	r3, r3, #132
   2bda0: ed830a00     	vstr	s0, [r3]
   2bda4: e30114f8     	movw	r1, #0x14f8
   2bda8: e3401007     	movt	r1, #0x7
   2bdac: e1a00006     	mov	r0, r6
   2bdb0: eb00d301     	bl	0x609bc
   2bdb4: eb00d3af     	bl	0x60c78
   2bdb8: e28d3b11     	add	r3, sp, #17408
   2bdbc: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bdc0: e2833088     	add	r3, r3, #136
   2bdc4: ed830a00     	vstr	s0, [r3]
   2bdc8: e3011508     	movw	r1, #0x1508
   2bdcc: e3401007     	movt	r1, #0x7
   2bdd0: e1a00006     	mov	r0, r6
   2bdd4: eb00d2f8     	bl	0x609bc
   2bdd8: eb00d3a6     	bl	0x60c78
   2bddc: e28d3b11     	add	r3, sp, #17408
   2bde0: eeb70bc0     	vcvt.f32.f64	s0, d0
   2bde4: e283308c     	add	r3, r3, #140
   2bde8: ed830a00     	vstr	s0, [r3]
   2bdec: e3011514     	movw	r1, #0x1514
   2bdf0: e3401007     	movt	r1, #0x7
   2bdf4: e1a00006     	mov	r0, r6
   2bdf8: eb00d2ef     	bl	0x609bc
   2bdfc: eb00d39d     	bl	0x60c78
   2be00: e28d3b11     	add	r3, sp, #17408
   2be04: eeb70bc0     	vcvt.f32.f64	s0, d0
   2be08: e2833090     	add	r3, r3, #144
   2be0c: ed830a00     	vstr	s0, [r3]
   2be10: e301151c     	movw	r1, #0x151c
   2be14: e3401007     	movt	r1, #0x7
   2be18: e1a00006     	mov	r0, r6
   2be1c: eb00d2e6     	bl	0x609bc
   2be20: eb00d394     	bl	0x60c78
   2be24: e28d3b11     	add	r3, sp, #17408
   2be28: eeb70bc0     	vcvt.f32.f64	s0, d0
   2be2c: e2833094     	add	r3, r3, #148
   2be30: ed830a00     	vstr	s0, [r3]
   2be34: e3011528     	movw	r1, #0x1528
   2be38: e3401007     	movt	r1, #0x7
   2be3c: e1a00006     	mov	r0, r6
   2be40: eb00d2dd     	bl	0x609bc
   2be44: eb00d38b     	bl	0x60c78
   2be48: e28d3b11     	add	r3, sp, #17408
   2be4c: eeb70bc0     	vcvt.f32.f64	s0, d0
   2be50: e2833098     	add	r3, r3, #152
   2be54: ed830a00     	vstr	s0, [r3]
   2be58: e5951f84     	ldr	r1, [r5, #0xf84]
   2be5c: e1a00007     	mov	r0, r7
   2be60: eb003d2e     	bl	0x3b320
   2be64: e30118f4     	movw	r1, #0x18f4
   2be68: e3401007     	movt	r1, #0x7
   2be6c: e1a00006     	mov	r0, r6
   2be70: eb00d2d1     	bl	0x609bc
   2be74: e1a01000     	mov	r1, r0
   2be78: e1a00004     	mov	r0, r4
   2be7c: eb00ea0c     	bl	0x666b4
   2be80: e28abf61     	add	r11, r10, #388
   2be84: e3a05000     	mov	r5, #0
   2be88: ea00000c     	b	0x2bec0
   2be8c: 00 00 7a 44  	.word	0x447a0000
   2be90: 34 fc 08 00  	.word	0x0008fc34
   2be94: 94 fc 08 00  	.word	0x0008fc94
   2be98: 4c fc 08 00  	.word	0x0008fc4c
   2be9c: f8 3c 09 00  	.word	0x00093cf8
   2bea0: e1a01005     	mov	r1, r5
   2bea4: e1a00004     	mov	r0, r4
   2bea8: eb00e898     	bl	0x66110
   2beac: eb00cf14     	bl	0x5fb04
   2beb0: e3550a01     	cmp	r5, #4096
   2beb4: 0a0000ad     	beq	0x2c170
   2beb8: e2855001     	add	r5, r5, #1
   2bebc: ecab0a01     	vstmia	r11!, {s0}
   2bec0: e1a00004     	mov	r0, r4
   2bec4: eb00cfd0     	bl	0x5fe0c
   2bec8: e1500005     	cmp	r0, r5
   2becc: 8afffff3     	bhi	0x2bea0
   2bed0: e1a00004     	mov	r0, r4
   2bed4: eb00d4ce     	bl	0x61214
   2bed8: e1a00006     	mov	r0, r6
   2bedc: eb00d4cc     	bl	0x61214
   2bee0: e28d0034     	add	r0, sp, #52
   2bee4: eb00d4ca     	bl	0x61214
   2bee8: e59d0074     	ldr	r0, [sp, #0x74]
   2beec: e2483024     	sub	r3, r8, #36
   2bef0: e1500003     	cmp	r0, r3
   2bef4: 0a000000     	beq	0x2befc
   2bef8: ebffa7d0     	bl	0x15e40    @ imm = #-0x160c0 ; _ZdlPv
   2befc: e59d005c     	ldr	r0, [sp, #0x5c]
   2bf00: e248803c     	sub	r8, r8, #60
   2bf04: e1500008     	cmp	r0, r8
   2bf08: 0a000000     	beq	0x2bf10
   2bf0c: ebffa7cb     	bl	0x15e40    @ imm = #-0x160d4 ; _ZdlPv
   2bf10: e5dd31c0     	ldrb	r3, [sp, #0x1c0]
   2bf14: e3a05001     	mov	r5, #1
   2bf18: e5c75285     	strb	r5, [r7, #0x285]
   2bf1c: e3530000     	cmp	r3, #0
   2bf20: 0a00004f     	beq	0x2c064
   2bf24: e59d1010     	ldr	r1, [sp, #0x10]
   2bf28: e28d00a4     	add	r0, sp, #164
   2bf2c: e3012900     	movw	r2, #0x1900
   2bf30: e3402007     	movt	r2, #0x7
   2bf34: eb000b23     	bl	0x2ebc8
   2bf38: e59d0008     	ldr	r0, [sp, #0x8]
   2bf3c: e1a01004     	mov	r1, r4
   2bf40: e3a02000     	mov	r2, #0
   2bf44: eb010ff5     	bl	0x6ff20
   2bf48: e59d00a4     	ldr	r0, [sp, #0xa4]
   2bf4c: e24990f4     	sub	r9, r9, #244
   2bf50: e1500009     	cmp	r0, r9
   2bf54: 0a000000     	beq	0x2bf5c
   2bf58: ebffa7b8     	bl	0x15e40    @ imm = #-0x16120 ; _ZdlPv
   2bf5c: e59d31c8     	ldr	r3, [sp, #0x1c8]
   2bf60: e1a00007     	mov	r0, r7
   2bf64: e58730a8     	str	r3, [r7, #0xa8]
   2bf68: e2433002     	sub	r3, r3, #2
   2bf6c: e5d7227c     	ldrb	r2, [r7, #0x27c]
   2bf70: e16f3f13     	clz	r3, r3
   2bf74: e59d11cc     	ldr	r1, [sp, #0x1cc]
   2bf78: e1a032a3     	lsr	r3, r3, #5
   2bf7c: e5c7327c     	strb	r3, [r7, #0x27c]
   2bf80: e0222003     	eor	r2, r2, r3
   2bf84: e5c7227d     	strb	r2, [r7, #0x27d]
   2bf88: eb0036d6     	bl	0x39ae8
   2bf8c: e59d11d0     	ldr	r1, [sp, #0x1d0]
   2bf90: e1a00007     	mov	r0, r7
   2bf94: eb003af1     	bl	0x3ab60
   2bf98: e5dd31c0     	ldrb	r3, [sp, #0x1c0]
   2bf9c: e3530000     	cmp	r3, #0
   2bfa0: 1a000002     	bne	0x2bfb0
   2bfa4: e59730b0     	ldr	r3, [r7, #0xb0]
   2bfa8: e3530000     	cmp	r3, #0
   2bfac: 0a00006c     	beq	0x2c164
   2bfb0: e59d400c     	ldr	r4, [sp, #0xc]
   2bfb4: e28d2f7b     	add	r2, sp, #492
   2bfb8: e59730e8     	ldr	r3, [r7, #0xe8]
   2bfbc: e2870a2a     	add	r0, r7, #172032
   2bfc0: e59de1d4     	ldr	lr, [sp, #0x1d4]
   2bfc4: e24a10a0     	sub	r1, r10, #160
   2bfc8: f462078f     	vld1.32	{d16}, [r2]
   2bfcc: e3a02001     	mov	r2, #1
   2bfd0: e584e00c     	str	lr, [r4, #0xc]
   2bfd4: e2800048     	add	r0, r0, #72
   2bfd8: e5c72288     	strb	r2, [r7, #0x288]
   2bfdc: e59dc1dc     	ldr	r12, [sp, #0x1dc]
   2bfe0: e59d21d8     	ldr	r2, [sp, #0x1d8]
   2bfe4: e5842014     	str	r2, [r4, #0x14]
   2bfe8: e59d21e4     	ldr	r2, [sp, #0x1e4]
   2bfec: e583c0a4     	str	r12, [r3, #0xa4]
   2bff0: e59dc1e0     	ldr	r12, [sp, #0x1e0]
   2bff4: e504c010     	str	r12, [r4, #-0x10]
   2bff8: e59dc1e8     	ldr	r12, [sp, #0x1e8]
   2bffc: e58320ac     	str	r2, [r3, #0xac]
   2c000: e5dd21f8     	ldrb	r2, [sp, #0x1f8]
   2c004: e583c0a8     	str	r12, [r3, #0xa8]
   2c008: f444078f     	vst1.32	{d16}, [r4]
   2c00c: e5c3206c     	strb	r2, [r3, #0x6c]
   2c010: e59d21fc     	ldr	r2, [sp, #0x1fc]
   2c014: e5832080     	str	r2, [r3, #0x80]
   2c018: eb004ac8     	bl	0x3eb40
   2c01c: e3a03000     	mov	r3, #0
   2c020: e587329c     	str	r3, [r7, #0x29c]
   2c024: e2843ba9     	add	r3, r4, #173056
   2c028: e5140568     	ldr	r0, [r4, #-0x568]
   2c02c: e28d1d07     	add	r1, sp, #448
   2c030: e1a04003     	mov	r4, r3
   2c034: e30422dc     	movw	r2, #0x42dc
   2c038: ebffa801     	bl	0x16044    @ imm = #-0x15ffc ; memcpy
   2c03c: e2877ba9     	add	r7, r7, #173056
   2c040: e2843f4e     	add	r3, r4, #312
   2c044: e58d300c     	str	r3, [sp, #0xc]
   2c048: e59d3018     	ldr	r3, [sp, #0x18]
   2c04c: e2877f4e     	add	r7, r7, #312
   2c050: e1570003     	cmp	r7, r3
   2c054: 1afffc88     	bne	0x2b27c
   2c058: e28ddb11     	add	sp, sp, #17408
   2c05c: e28dd0a4     	add	sp, sp, #164
   2c060: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   2c064: e3a01003     	mov	r1, #3
   2c068: e1a00007     	mov	r0, r7
   2c06c: eb0037e9     	bl	0x3a018
   2c070: e59d31c4     	ldr	r3, [sp, #0x1c4]
   2c074: e597e058     	ldr	lr, [r7, #0x58]
   2c078: e303c004     	movw	r12, #0x3004
   2c07c: e5971104     	ldr	r1, [r7, #0x104]
   2c080: e2430a03     	sub	r0, r3, #12288
   2c084: e1a02005     	mov	r2, r5
   2c088: e58e3000     	str	r3, [lr]
   2c08c: e240e005     	sub	lr, r0, #5
   2c090: e2411001     	sub	r1, r1, #1
   2c094: e15e000c     	cmp	lr, r12
   2c098: b1a0c00e     	movlt	r12, lr
   2c09c: e2433d99     	sub	r3, r3, #9792
   2c0a0: e3510001     	cmp	r1, #1
   2c0a4: e243302a     	sub	r3, r3, #42
   2c0a8: 83a01000     	movhi	r1, #0
   2c0ac: 93a01001     	movls	r1, #1
   2c0b0: e5873050     	str	r3, [r7, #0x50]
   2c0b4: e2400004     	sub	r0, r0, #4
   2c0b8: e587c084     	str	r12, [r7, #0x84]
   2c0bc: e587004c     	str	r0, [r7, #0x4c]
   2c0c0: e1a00007     	mov	r0, r7
   2c0c4: eb002d78     	bl	0x376ac
   2c0c8: e59d1010     	ldr	r1, [sp, #0x10]
   2c0cc: e28d0074     	add	r0, sp, #116
   2c0d0: e301291c     	movw	r2, #0x191c
   2c0d4: e3402007     	movt	r2, #0x7
   2c0d8: eb000aba     	bl	0x2ebc8
   2c0dc: e597304c     	ldr	r3, [r7, #0x4c]
   2c0e0: e58d3000     	str	r3, [sp]
   2c0e4: e3061218     	movw	r1, #0x6218
   2c0e8: e3401001     	movt	r1, #0x1
   2c0ec: e3003d28     	movw	r3, #0xd28
   2c0f0: e3403007     	movt	r3, #0x7
   2c0f4: e28d008c     	add	r0, sp, #140
   2c0f8: e3a02010     	mov	r2, #16
   2c0fc: ebfff79d     	bl	0x29f78
   2c100: e28d80a0     	add	r8, sp, #160
   2c104: e28d3e4a     	add	r3, sp, #1184
   2c108: e2430fff     	sub	r0, r3, #1020
   2c10c: e2482014     	sub	r2, r8, #20
   2c110: e248102c     	sub	r1, r8, #44
   2c114: eb0009f4     	bl	0x2e8ec
   2c118: e59d0008     	ldr	r0, [sp, #0x8]
   2c11c: e1a01004     	mov	r1, r4
   2c120: e3a02000     	mov	r2, #0
   2c124: eb010f7d     	bl	0x6ff20
   2c128: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c12c: e24990f4     	sub	r9, r9, #244
   2c130: e1500009     	cmp	r0, r9
   2c134: 0a000000     	beq	0x2c13c
   2c138: ebffa740     	bl	0x15e40    @ imm = #-0x16300 ; _ZdlPv
   2c13c: e59d008c     	ldr	r0, [sp, #0x8c]
   2c140: e248300c     	sub	r3, r8, #12
   2c144: e1500003     	cmp	r0, r3
   2c148: 0a000000     	beq	0x2c150
   2c14c: ebffa73b     	bl	0x15e40    @ imm = #-0x16314 ; _ZdlPv
   2c150: e59d0074     	ldr	r0, [sp, #0x74]
   2c154: e2488024     	sub	r8, r8, #36
   2c158: e1500008     	cmp	r0, r8
   2c15c: 1affff7d     	bne	0x2bf58
   2c160: eaffff7d     	b	0x2bf5c
   2c164: e1a00007     	mov	r0, r7
   2c168: eb002fcc     	bl	0x380a0
   2c16c: eaffff8f     	b	0x2bfb0
   2c170: e3010440     	movw	r0, #0x1440
   2c174: e3400007     	movt	r0, #0x7
   2c178: e1a02005     	mov	r2, r5
   2c17c: e1a01005     	mov	r1, r5
   2c180: ebffa803     	bl	0x16194    @ imm = #-0x15ff4 ; _ZSt24__throw_out_of_range_fmtPKcz
   2c184: e3010440     	movw	r0, #0x1440
   2c188: e3400007     	movt	r0, #0x7
   2c18c: e1a02005     	mov	r2, r5
   2c190: e1a01005     	mov	r1, r5
   2c194: ebffa7fe     	bl	0x16194    @ imm = #-0x16008 ; _ZSt24__throw_out_of_range_fmtPKcz
   2c198: e1a0b000     	mov	r11, r0
   2c19c: e1a05001     	mov	r5, r1
   2c1a0: e3510002     	cmp	r1, #2
   2c1a4: 1a0000a2     	bne	0x2c434
   2c1a8: ebffa655     	bl	0x15b04    @ imm = #-0x166ac ; __cxa_begin_catch
   2c1ac: e3011780     	movw	r1, #0x1780
   2c1b0: e3401007     	movt	r1, #0x7
   2c1b4: e1a05000     	mov	r5, r0
   2c1b8: e3a0201e     	mov	r2, #30
   2c1bc: e30f0764     	movw	r0, #0xf764
   2c1c0: e3400008     	movt	r0, #0x8
   2c1c4: ebffa7a7     	bl	0x16068    @ imm = #-0x16164 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c1c8: e5953000     	ldr	r3, [r5]
   2c1cc: e1a00005     	mov	r0, r5
   2c1d0: e5933008     	ldr	r3, [r3, #0x8]
   2c1d4: e12fff33     	blx	r3
   2c1d8: e1a01000     	mov	r1, r0
   2c1dc: e30f0764     	movw	r0, #0xf764
   2c1e0: e3400008     	movt	r0, #0x8
   2c1e4: ebffa72a     	bl	0x15e94    @ imm = #-0x16358 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c1e8: e3a02001     	mov	r2, #1
   2c1ec: e1a01004     	mov	r1, r4
   2c1f0: e3a0300a     	mov	r3, #10
   2c1f4: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c1f8: ebffa79a     	bl	0x16068    @ imm = #-0x16198 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c1fc: e51f3368     	ldr	r3, [pc, #-0x368]       @ 0x2be9c
   2c200: e28d5dd2     	add	r5, sp, #13440
   2c204: e2855020     	add	r5, r5, #32
   2c208: e5933224     	ldr	r3, [r3, #0x224]
   2c20c: e5853f84     	str	r3, [r5, #0xf84]
   2c210: ebffa815     	bl	0x1626c    @ imm = #-0x15fac ; __cxa_end_catch
   2c214: eafffea6     	b	0x2bcb4
   2c218: e1a0b000     	mov	r11, r0
   2c21c: e1a03001     	mov	r3, r1
   2c220: e3510002     	cmp	r1, #2
   2c224: 1a00003d     	bne	0x2c320
   2c228: ebffa635     	bl	0x15b04    @ imm = #-0x1672c ; __cxa_begin_catch
   2c22c: e30117c0     	movw	r1, #0x17c0
   2c230: e3401007     	movt	r1, #0x7
   2c234: e1a0b000     	mov	r11, r0
   2c238: e3a02024     	mov	r2, #36
   2c23c: e30f0764     	movw	r0, #0xf764
   2c240: e3400008     	movt	r0, #0x8
   2c244: ebffa787     	bl	0x16068    @ imm = #-0x161e4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c248: e59b3000     	ldr	r3, [r11]
   2c24c: e1a0000b     	mov	r0, r11
   2c250: e5933008     	ldr	r3, [r3, #0x8]
   2c254: e12fff33     	blx	r3
   2c258: e1a01000     	mov	r1, r0
   2c25c: e30f0764     	movw	r0, #0xf764
   2c260: e3400008     	movt	r0, #0x8
   2c264: ebffa70a     	bl	0x15e94    @ imm = #-0x163d8 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c268: e3a02001     	mov	r2, #1
   2c26c: e1a01004     	mov	r1, r4
   2c270: e3a0300a     	mov	r3, #10
   2c274: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c278: ebffa77a     	bl	0x16068    @ imm = #-0x16218 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c27c: e51f33e8     	ldr	r3, [pc, #-0x3e8]       @ 0x2be9c
   2c280: e28d2b11     	add	r2, sp, #17408
   2c284: e2822070     	add	r2, r2, #112
   2c288: e5933270     	ldr	r3, [r3, #0x270]
   2c28c: e5823000     	str	r3, [r2]
   2c290: ebffa7f5     	bl	0x1626c    @ imm = #-0x1602c ; __cxa_end_catch
   2c294: eafffe95     	b	0x2bcf0
   2c298: e1a0b000     	mov	r11, r0
   2c29c: e1a03001     	mov	r3, r1
   2c2a0: e3510002     	cmp	r1, #2
   2c2a4: 1a00001d     	bne	0x2c320
   2c2a8: ebffa615     	bl	0x15b04    @ imm = #-0x167ac ; __cxa_begin_catch
   2c2ac: e30117a0     	movw	r1, #0x17a0
   2c2b0: e3401007     	movt	r1, #0x7
   2c2b4: e1a0b000     	mov	r11, r0
   2c2b8: e3a0201c     	mov	r2, #28
   2c2bc: e30f0764     	movw	r0, #0xf764
   2c2c0: e3400008     	movt	r0, #0x8
   2c2c4: ebffa767     	bl	0x16068    @ imm = #-0x16264 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c2c8: e59b3000     	ldr	r3, [r11]
   2c2cc: e1a0000b     	mov	r0, r11
   2c2d0: e5933008     	ldr	r3, [r3, #0x8]
   2c2d4: e12fff33     	blx	r3
   2c2d8: e1a01000     	mov	r1, r0
   2c2dc: e30f0764     	movw	r0, #0xf764
   2c2e0: e3400008     	movt	r0, #0x8
   2c2e4: ebffa6ea     	bl	0x15e94    @ imm = #-0x16458 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c2e8: e3a02001     	mov	r2, #1
   2c2ec: e1a01004     	mov	r1, r4
   2c2f0: e3a0300a     	mov	r3, #10
   2c2f4: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c2f8: ebffa75a     	bl	0x16068    @ imm = #-0x16298 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c2fc: e51f3468     	ldr	r3, [pc, #-0x468]       @ 0x2be9c
   2c300: e5933268     	ldr	r3, [r3, #0x268]
   2c304: e5853fc8     	str	r3, [r5, #0xfc8]
   2c308: ebffa7d7     	bl	0x1626c    @ imm = #-0x160a4 ; __cxa_end_catch
   2c30c: eafffe6e     	b	0x2bccc
   2c310: e1a0b000     	mov	r11, r0
   2c314: e1a03001     	mov	r3, r1
   2c318: e3510002     	cmp	r1, #2
   2c31c: 0a0007f4     	beq	0x2e2f4
   2c320: e1a05003     	mov	r5, r3
   2c324: ea000042     	b	0x2c434
   2c328: e1a0b000     	mov	r11, r0
   2c32c: e1a03001     	mov	r3, r1
   2c330: e3510002     	cmp	r1, #2
   2c334: 1afffff9     	bne	0x2c320
   2c338: ebffa5f1     	bl	0x15b04    @ imm = #-0x1683c ; __cxa_begin_catch
   2c33c: e3011838     	movw	r1, #0x1838
   2c340: e3401007     	movt	r1, #0x7
   2c344: e1a0b000     	mov	r11, r0
   2c348: e3a0201a     	mov	r2, #26
   2c34c: e30f0764     	movw	r0, #0xf764
   2c350: e3400008     	movt	r0, #0x8
   2c354: ebffa743     	bl	0x16068    @ imm = #-0x162f4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c358: e59b3000     	ldr	r3, [r11]
   2c35c: e1a0000b     	mov	r0, r11
   2c360: e5933008     	ldr	r3, [r3, #0x8]
   2c364: e12fff33     	blx	r3
   2c368: e1a01000     	mov	r1, r0
   2c36c: e30f0764     	movw	r0, #0xf764
   2c370: e3400008     	movt	r0, #0x8
   2c374: ebffa6c6     	bl	0x15e94    @ imm = #-0x164e8 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c378: e3a02001     	mov	r2, #1
   2c37c: e1a01004     	mov	r1, r4
   2c380: e3a0300a     	mov	r3, #10
   2c384: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c388: ebffa736     	bl	0x16068    @ imm = #-0x16328 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c38c: e51f34f8     	ldr	r3, [pc, #-0x4f8]       @ 0x2be9c
   2c390: e28d2b11     	add	r2, sp, #17408
   2c394: e2822080     	add	r2, r2, #128
   2c398: e5933280     	ldr	r3, [r3, #0x280]
   2c39c: e5823000     	str	r3, [r2]
   2c3a0: ebffa7b1     	bl	0x1626c    @ imm = #-0x1613c ; __cxa_end_catch
   2c3a4: eafffe75     	b	0x2bd80
   2c3a8: e1a0b000     	mov	r11, r0
   2c3ac: e1a03001     	mov	r3, r1
   2c3b0: e3510002     	cmp	r1, #2
   2c3b4: 1affffd9     	bne	0x2c320
   2c3b8: ebffa5d1     	bl	0x15b04    @ imm = #-0x168bc ; __cxa_begin_catch
   2c3bc: e3011820     	movw	r1, #0x1820
   2c3c0: e3401007     	movt	r1, #0x7
   2c3c4: e1a0b000     	mov	r11, r0
   2c3c8: e3a02017     	mov	r2, #23
   2c3cc: e30f0764     	movw	r0, #0xf764
   2c3d0: e3400008     	movt	r0, #0x8
   2c3d4: ebffa723     	bl	0x16068    @ imm = #-0x16374 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c3d8: e59b3000     	ldr	r3, [r11]
   2c3dc: e1a0000b     	mov	r0, r11
   2c3e0: e5933008     	ldr	r3, [r3, #0x8]
   2c3e4: e12fff33     	blx	r3
   2c3e8: e1a01000     	mov	r1, r0
   2c3ec: e30f0764     	movw	r0, #0xf764
   2c3f0: e3400008     	movt	r0, #0x8
   2c3f4: ebffa6a6     	bl	0x15e94    @ imm = #-0x16568 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c3f8: e3a02001     	mov	r2, #1
   2c3fc: e1a01004     	mov	r1, r4
   2c400: e3a0300a     	mov	r3, #10
   2c404: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c408: ebffa716     	bl	0x16068    @ imm = #-0x163a8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c40c: e51f3578     	ldr	r3, [pc, #-0x578]       @ 0x2be9c
   2c410: e28d2b11     	add	r2, sp, #17408
   2c414: e282207c     	add	r2, r2, #124
   2c418: e593327c     	ldr	r3, [r3, #0x27c]
   2c41c: e5823000     	str	r3, [r2]
   2c420: ebffa791     	bl	0x1626c    @ imm = #-0x161bc ; __cxa_end_catch
   2c424: eafffe4c     	b	0x2bd5c
   2c428: e1a0b000     	mov	r11, r0
   2c42c: e1a05001     	mov	r5, r1
   2c430: ebffa78d     	bl	0x1626c    @ imm = #-0x161cc ; __cxa_end_catch
   2c434: e1a00006     	mov	r0, r6
   2c438: eb00d375     	bl	0x61214
   2c43c: e1a0000b     	mov	r0, r11
   2c440: e3550001     	cmp	r5, #1
   2c444: 1a000027     	bne	0x2c4e8
   2c448: ebffa5ad     	bl	0x15b04    @ imm = #-0x1694c ; __cxa_begin_catch
   2c44c: e28d3e4a     	add	r3, sp, #1184
   2c450: e1a05000     	mov	r5, r0
   2c454: e2430e41     	sub	r0, r3, #1040
   2c458: e3012abc     	movw	r2, #0x1abc
   2c45c: e3402007     	movt	r2, #0x7
   2c460: e59d1010     	ldr	r1, [sp, #0x10]
   2c464: e2400004     	sub	r0, r0, #4
   2c468: eb0009d6     	bl	0x2ebc8
   2c46c: e5953000     	ldr	r3, [r5]
   2c470: e1a00005     	mov	r0, r5
   2c474: e5933008     	ldr	r3, [r3, #0x8]
   2c478: e12fff33     	blx	r3
   2c47c: e1a01000     	mov	r1, r0
   2c480: e1a00006     	mov	r0, r6
   2c484: ebffa7f0     	bl	0x1644c    @ imm = #-0x16040 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c488: e1a01000     	mov	r1, r0
   2c48c: e1a00004     	mov	r0, r4
   2c490: ebffa5da     	bl	0x15c00    @ imm = #-0x16898 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2c494: e59d0008     	ldr	r0, [sp, #0x8]
   2c498: e3a02002     	mov	r2, #2
   2c49c: e1a01004     	mov	r1, r4
   2c4a0: eb010e9e     	bl	0x6ff20
   2c4a4: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c4a8: e24930f4     	sub	r3, r9, #244
   2c4ac: e1500003     	cmp	r0, r3
   2c4b0: 0a000000     	beq	0x2c4b8
   2c4b4: ebffa661     	bl	0x15e40    @ imm = #-0x1667c ; _ZdlPv
   2c4b8: e59d008c     	ldr	r0, [sp, #0x8c]
   2c4bc: e248300c     	sub	r3, r8, #12
   2c4c0: e1500003     	cmp	r0, r3
   2c4c4: 0a000000     	beq	0x2c4cc
   2c4c8: ebffa65c     	bl	0x15e40    @ imm = #-0x16690 ; _ZdlPv
   2c4cc: ebffa766     	bl	0x1626c    @ imm = #-0x16268 ; __cxa_end_catch
   2c4d0: eafffe82     	b	0x2bee0
   2c4d4: eaffffd3     	b	0x2c428
   2c4d8: eaffffd2     	b	0x2c428
   2c4dc: e1a05001     	mov	r5, r1
   2c4e0: e3550001     	cmp	r5, #1
   2c4e4: 0a000330     	beq	0x2d1ac
   2c4e8: e1a04000     	mov	r4, r0
   2c4ec: e28d0034     	add	r0, sp, #52
   2c4f0: eb00d347     	bl	0x61214
   2c4f4: e59d0074     	ldr	r0, [sp, #0x74]
   2c4f8: e2483024     	sub	r3, r8, #36
   2c4fc: e1500003     	cmp	r0, r3
   2c500: 0a000000     	beq	0x2c508
   2c504: ebffa64d     	bl	0x15e40    @ imm = #-0x166cc ; _ZdlPv
   2c508: e59d005c     	ldr	r0, [sp, #0x5c]
   2c50c: e248803c     	sub	r8, r8, #60
   2c510: e1500008     	cmp	r0, r8
   2c514: 0a000000     	beq	0x2c51c
   2c518: ebffa648     	bl	0x15e40    @ imm = #-0x166e0 ; _ZdlPv
   2c51c: e1a00004     	mov	r0, r4
   2c520: e3550001     	cmp	r5, #1
   2c524: 1a000016     	bne	0x2c584
   2c528: ebffa575     	bl	0x15b04    @ imm = #-0x16a2c ; __cxa_begin_catch
   2c52c: e5903000     	ldr	r3, [r0]
   2c530: e28d9e1a     	add	r9, sp, #416
   2c534: e24940fc     	sub	r4, r9, #252
   2c538: e5933008     	ldr	r3, [r3, #0x8]
   2c53c: e12fff33     	blx	r3
   2c540: e1a01000     	mov	r1, r0
   2c544: e1a00004     	mov	r0, r4
   2c548: ebffed96     	bl	0x27ba8
   2c54c: e59d0008     	ldr	r0, [sp, #0x8]
   2c550: e3a02002     	mov	r2, #2
   2c554: e1a01004     	mov	r1, r4
   2c558: eb010e70     	bl	0x6ff20
   2c55c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c560: e24930f4     	sub	r3, r9, #244
   2c564: e1500003     	cmp	r0, r3
   2c568: 0a000000     	beq	0x2c570
   2c56c: ebffa633     	bl	0x15e40    @ imm = #-0x16734 ; _ZdlPv
   2c570: ebffa73d     	bl	0x1626c    @ imm = #-0x1630c ; __cxa_end_catch
   2c574: e2873004     	add	r3, r7, #4
   2c578: e58d3010     	str	r3, [sp, #0x10]
   2c57c: eafffe63     	b	0x2bf10
   2c580: ebffa739     	bl	0x1626c    @ imm = #-0x1631c ; __cxa_end_catch
   2c584: ebffa675     	bl	0x15f60    @ imm = #-0x1662c ; __cxa_end_cleanup
   2c588: e59d30a4     	ldr	r3, [sp, #0xa4]
   2c58c: e24990f4     	sub	r9, r9, #244
   2c590: e1a04000     	mov	r4, r0
   2c594: e1a05001     	mov	r5, r1
   2c598: e1530009     	cmp	r3, r9
   2c59c: 0a000001     	beq	0x2c5a8
   2c5a0: e1a00003     	mov	r0, r3
   2c5a4: ebffa625     	bl	0x15e40    @ imm = #-0x1676c ; _ZdlPv
   2c5a8: e59d008c     	ldr	r0, [sp, #0x8c]
   2c5ac: e248300c     	sub	r3, r8, #12
   2c5b0: e1500003     	cmp	r0, r3
   2c5b4: 0a000000     	beq	0x2c5bc
   2c5b8: ebffa620     	bl	0x15e40    @ imm = #-0x16780 ; _ZdlPv
   2c5bc: ebffa72a     	bl	0x1626c    @ imm = #-0x16358 ; __cxa_end_catch
   2c5c0: eaffffc9     	b	0x2c4ec
   2c5c4: e1a04000     	mov	r4, r0
   2c5c8: e1a05001     	mov	r5, r1
   2c5cc: eafffff5     	b	0x2c5a8
   2c5d0: e1a04000     	mov	r4, r0
   2c5d4: e1a05001     	mov	r5, r1
   2c5d8: eafffff7     	b	0x2c5bc
   2c5dc: e1a0b000     	mov	r11, r0
   2c5e0: e1a03001     	mov	r3, r1
   2c5e4: e3510002     	cmp	r1, #2
   2c5e8: 1affff4c     	bne	0x2c320
   2c5ec: ebffa544     	bl	0x15b04    @ imm = #-0x16af0 ; __cxa_begin_catch
   2c5f0: e3011808     	movw	r1, #0x1808
   2c5f4: e3401007     	movt	r1, #0x7
   2c5f8: e1a0b000     	mov	r11, r0
   2c5fc: e3a02014     	mov	r2, #20
   2c600: e30f0764     	movw	r0, #0xf764
   2c604: e3400008     	movt	r0, #0x8
   2c608: ebffa696     	bl	0x16068    @ imm = #-0x165a8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c60c: e59b3000     	ldr	r3, [r11]
   2c610: e1a0000b     	mov	r0, r11
   2c614: e5933008     	ldr	r3, [r3, #0x8]
   2c618: e12fff33     	blx	r3
   2c61c: e1a01000     	mov	r1, r0
   2c620: e30f0764     	movw	r0, #0xf764
   2c624: e3400008     	movt	r0, #0x8
   2c628: ebffa619     	bl	0x15e94    @ imm = #-0x1679c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c62c: e3a02001     	mov	r2, #1
   2c630: e1a01004     	mov	r1, r4
   2c634: e3a0300a     	mov	r3, #10
   2c638: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c63c: ebffa689     	bl	0x16068    @ imm = #-0x165dc ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c640: e51f37ac     	ldr	r3, [pc, #-0x7ac]       @ 0x2be9c
   2c644: e28d2b11     	add	r2, sp, #17408
   2c648: e2822078     	add	r2, r2, #120
   2c64c: e5933278     	ldr	r3, [r3, #0x278]
   2c650: e5823000     	str	r3, [r2]
   2c654: ebffa704     	bl	0x1626c    @ imm = #-0x163f0 ; __cxa_end_catch
   2c658: eafffdb6     	b	0x2bd38
   2c65c: e1a0b000     	mov	r11, r0
   2c660: e1a03001     	mov	r3, r1
   2c664: e3510002     	cmp	r1, #2
   2c668: 1affff2c     	bne	0x2c320
   2c66c: ebffa524     	bl	0x15b04    @ imm = #-0x16b70 ; __cxa_begin_catch
   2c670: e30117e8     	movw	r1, #0x17e8
   2c674: e3401007     	movt	r1, #0x7
   2c678: e1a0b000     	mov	r11, r0
   2c67c: e3a0201c     	mov	r2, #28
   2c680: e30f0764     	movw	r0, #0xf764
   2c684: e3400008     	movt	r0, #0x8
   2c688: ebffa676     	bl	0x16068    @ imm = #-0x16628 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c68c: e59b3000     	ldr	r3, [r11]
   2c690: e1a0000b     	mov	r0, r11
   2c694: e5933008     	ldr	r3, [r3, #0x8]
   2c698: e12fff33     	blx	r3
   2c69c: e1a01000     	mov	r1, r0
   2c6a0: e30f0764     	movw	r0, #0xf764
   2c6a4: e3400008     	movt	r0, #0x8
   2c6a8: ebffa5f9     	bl	0x15e94    @ imm = #-0x1681c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2c6ac: e3a02001     	mov	r2, #1
   2c6b0: e1a01004     	mov	r1, r4
   2c6b4: e3a0300a     	mov	r3, #10
   2c6b8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2c6bc: ebffa669     	bl	0x16068    @ imm = #-0x1665c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2c6c0: e51f382c     	ldr	r3, [pc, #-0x82c]       @ 0x2be9c
   2c6c4: e28d2b11     	add	r2, sp, #17408
   2c6c8: e2822074     	add	r2, r2, #116
   2c6cc: e5933274     	ldr	r3, [r3, #0x274]
   2c6d0: e5823000     	str	r3, [r2]
   2c6d4: ebffa6e4     	bl	0x1626c    @ imm = #-0x16470 ; __cxa_end_catch
   2c6d8: eafffd8d     	b	0x2bd14
   2c6dc: eaffff51     	b	0x2c428
   2c6e0: eaffff50     	b	0x2c428
   2c6e4: eaffff4f     	b	0x2c428
   2c6e8: eaffff4e     	b	0x2c428
   2c6ec: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c6f0: e24990f4     	sub	r9, r9, #244
   2c6f4: e1500009     	cmp	r0, r9
   2c6f8: 0affffa0     	beq	0x2c580
   2c6fc: ebffa5cf     	bl	0x15e40    @ imm = #-0x168c4 ; _ZdlPv
   2c700: eaffff9e     	b	0x2c580
   2c704: eaffff9d     	b	0x2c580
   2c708: e1a03000     	mov	r3, r0
   2c70c: e1a0b001     	mov	r11, r1
   2c710: e3510001     	cmp	r1, #1
   2c714: 1a00004e     	bne	0x2c854
   2c718: ebffa4f9     	bl	0x15b04    @ imm = #-0x16c1c ; __cxa_begin_catch
   2c71c: e28d3e4a     	add	r3, sp, #1184
   2c720: e1a0b000     	mov	r11, r0
   2c724: e2430e41     	sub	r0, r3, #1040
   2c728: e3012a70     	movw	r2, #0x1a70
   2c72c: e3402007     	movt	r2, #0x7
   2c730: e2400004     	sub	r0, r0, #4
   2c734: e2871004     	add	r1, r7, #4
   2c738: eb000922     	bl	0x2ebc8
   2c73c: e59b3000     	ldr	r3, [r11]
   2c740: e1a0000b     	mov	r0, r11
   2c744: e5933008     	ldr	r3, [r3, #0x8]
   2c748: e12fff33     	blx	r3
   2c74c: e1a01000     	mov	r1, r0
   2c750: e1a00006     	mov	r0, r6
   2c754: ebffa73c     	bl	0x1644c    @ imm = #-0x16310 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c758: e1a01000     	mov	r1, r0
   2c75c: e1a00004     	mov	r0, r4
   2c760: ebffa526     	bl	0x15c00    @ imm = #-0x16b68 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2c764: e59d0008     	ldr	r0, [sp, #0x8]
   2c768: e3a02002     	mov	r2, #2
   2c76c: e1a01004     	mov	r1, r4
   2c770: eb010dea     	bl	0x6ff20
   2c774: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c778: e24930f4     	sub	r3, r9, #244
   2c77c: e1500003     	cmp	r0, r3
   2c780: 0a000000     	beq	0x2c788
   2c784: ebffa5ad     	bl	0x15e40    @ imm = #-0x1694c ; _ZdlPv
   2c788: e59d008c     	ldr	r0, [sp, #0x8c]
   2c78c: e248300c     	sub	r3, r8, #12
   2c790: e1500003     	cmp	r0, r3
   2c794: 0a000000     	beq	0x2c79c
   2c798: ebffa5a8     	bl	0x15e40    @ imm = #-0x16960 ; _ZdlPv
   2c79c: ebffa6b2     	bl	0x1626c    @ imm = #-0x16538 ; __cxa_end_catch
   2c7a0: eafffc7c     	b	0x2b998
   2c7a4: e1a03000     	mov	r3, r0
   2c7a8: e1a0b001     	mov	r11, r1
   2c7ac: e3510001     	cmp	r1, #1
   2c7b0: 1a000027     	bne	0x2c854
   2c7b4: ebffa4d2     	bl	0x15b04    @ imm = #-0x16cb8 ; __cxa_begin_catch
   2c7b8: e28d3e4a     	add	r3, sp, #1184
   2c7bc: e1a0b000     	mov	r11, r0
   2c7c0: e2430e41     	sub	r0, r3, #1040
   2c7c4: e3012a5c     	movw	r2, #0x1a5c
   2c7c8: e3402007     	movt	r2, #0x7
   2c7cc: e2400004     	sub	r0, r0, #4
   2c7d0: e2871004     	add	r1, r7, #4
   2c7d4: eb0008fb     	bl	0x2ebc8
   2c7d8: e59b3000     	ldr	r3, [r11]
   2c7dc: e1a0000b     	mov	r0, r11
   2c7e0: e5933008     	ldr	r3, [r3, #0x8]
   2c7e4: e12fff33     	blx	r3
   2c7e8: e1a01000     	mov	r1, r0
   2c7ec: e1a00006     	mov	r0, r6
   2c7f0: ebffa715     	bl	0x1644c    @ imm = #-0x163ac ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c7f4: e1a01000     	mov	r1, r0
   2c7f8: e1a00004     	mov	r0, r4
   2c7fc: ebffa4ff     	bl	0x15c00    @ imm = #-0x16c04 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2c800: e59d0008     	ldr	r0, [sp, #0x8]
   2c804: e3a02002     	mov	r2, #2
   2c808: e1a01004     	mov	r1, r4
   2c80c: eb010dc3     	bl	0x6ff20
   2c810: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c814: e24930f4     	sub	r3, r9, #244
   2c818: e1500003     	cmp	r0, r3
   2c81c: 0a000000     	beq	0x2c824
   2c820: ebffa586     	bl	0x15e40    @ imm = #-0x169e8 ; _ZdlPv
   2c824: e59d008c     	ldr	r0, [sp, #0x8c]
   2c828: e248300c     	sub	r3, r8, #12
   2c82c: e1500003     	cmp	r0, r3
   2c830: 0a000000     	beq	0x2c838
   2c834: ebffa581     	bl	0x15e40    @ imm = #-0x169fc ; _ZdlPv
   2c838: ebffa68b     	bl	0x1626c    @ imm = #-0x165d4 ; __cxa_end_catch
   2c83c: eafffc4c     	b	0x2b974
   2c840: e1a03000     	mov	r3, r0
   2c844: e1a0b001     	mov	r11, r1
   2c848: e58d301c     	str	r3, [sp, #0x1c]
   2c84c: ebffa686     	bl	0x1626c    @ imm = #-0x165e8 ; __cxa_end_catch
   2c850: e59d301c     	ldr	r3, [sp, #0x1c]
   2c854: e1a00005     	mov	r0, r5
   2c858: e58d301c     	str	r3, [sp, #0x1c]
   2c85c: eb00d26c     	bl	0x61214
   2c860: e59d301c     	ldr	r3, [sp, #0x1c]
   2c864: e1a0500b     	mov	r5, r11
   2c868: e1a00003     	mov	r0, r3
   2c86c: eaffff1b     	b	0x2c4e0
   2c870: e1a03000     	mov	r3, r0
   2c874: e1a0b001     	mov	r11, r1
   2c878: e3510001     	cmp	r1, #1
   2c87c: 1afffff4     	bne	0x2c854
   2c880: ebffa49f     	bl	0x15b04    @ imm = #-0x16d84 ; __cxa_begin_catch
   2c884: e28d3e4a     	add	r3, sp, #1184
   2c888: e1a0b000     	mov	r11, r0
   2c88c: e2430e41     	sub	r0, r3, #1040
   2c890: e30129c8     	movw	r2, #0x19c8
   2c894: e3402007     	movt	r2, #0x7
   2c898: e2400004     	sub	r0, r0, #4
   2c89c: e2871004     	add	r1, r7, #4
   2c8a0: eb0008c8     	bl	0x2ebc8
   2c8a4: e59b3000     	ldr	r3, [r11]
   2c8a8: e1a0000b     	mov	r0, r11
   2c8ac: e5933008     	ldr	r3, [r3, #0x8]
   2c8b0: e12fff33     	blx	r3
   2c8b4: e1a01000     	mov	r1, r0
   2c8b8: e1a00006     	mov	r0, r6
   2c8bc: ebffa6e2     	bl	0x1644c    @ imm = #-0x16478 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c8c0: e1a01000     	mov	r1, r0
   2c8c4: e1a00004     	mov	r0, r4
   2c8c8: ebffa4cc     	bl	0x15c00    @ imm = #-0x16cd0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2c8cc: e59d0008     	ldr	r0, [sp, #0x8]
   2c8d0: e3a02002     	mov	r2, #2
   2c8d4: e1a01004     	mov	r1, r4
   2c8d8: eb010d90     	bl	0x6ff20
   2c8dc: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c8e0: e24930f4     	sub	r3, r9, #244
   2c8e4: e1500003     	cmp	r0, r3
   2c8e8: 0a000000     	beq	0x2c8f0
   2c8ec: ebffa553     	bl	0x15e40    @ imm = #-0x16ab4 ; _ZdlPv
   2c8f0: e59d008c     	ldr	r0, [sp, #0x8c]
   2c8f4: e248300c     	sub	r3, r8, #12
   2c8f8: e1500003     	cmp	r0, r3
   2c8fc: 0a000000     	beq	0x2c904
   2c900: ebffa54e     	bl	0x15e40    @ imm = #-0x16ac8 ; _ZdlPv
   2c904: ebffa658     	bl	0x1626c    @ imm = #-0x166a0 ; __cxa_end_catch
   2c908: eafffbec     	b	0x2b8c0
   2c90c: e1a03000     	mov	r3, r0
   2c910: e1a0b001     	mov	r11, r1
   2c914: e3510001     	cmp	r1, #1
   2c918: 1affffcd     	bne	0x2c854
   2c91c: ebffa478     	bl	0x15b04    @ imm = #-0x16e20 ; __cxa_begin_catch
   2c920: e28d3e4a     	add	r3, sp, #1184
   2c924: e1a0b000     	mov	r11, r0
   2c928: e2430e41     	sub	r0, r3, #1040
   2c92c: e3012a20     	movw	r2, #0x1a20
   2c930: e3402007     	movt	r2, #0x7
   2c934: e2400004     	sub	r0, r0, #4
   2c938: e2871004     	add	r1, r7, #4
   2c93c: eb0008a1     	bl	0x2ebc8
   2c940: e59b3000     	ldr	r3, [r11]
   2c944: e1a0000b     	mov	r0, r11
   2c948: e5933008     	ldr	r3, [r3, #0x8]
   2c94c: e12fff33     	blx	r3
   2c950: e1a01000     	mov	r1, r0
   2c954: e1a00006     	mov	r0, r6
   2c958: ebffa6bb     	bl	0x1644c    @ imm = #-0x16514 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c95c: e1a01000     	mov	r1, r0
   2c960: e1a00004     	mov	r0, r4
   2c964: ebffa4a5     	bl	0x15c00    @ imm = #-0x16d6c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2c968: e59d0008     	ldr	r0, [sp, #0x8]
   2c96c: e3a02002     	mov	r2, #2
   2c970: e1a01004     	mov	r1, r4
   2c974: eb010d69     	bl	0x6ff20
   2c978: e59d00a4     	ldr	r0, [sp, #0xa4]
   2c97c: e24930f4     	sub	r3, r9, #244
   2c980: e1500003     	cmp	r0, r3
   2c984: 0a000000     	beq	0x2c98c
   2c988: ebffa52c     	bl	0x15e40    @ imm = #-0x16b50 ; _ZdlPv
   2c98c: e59d008c     	ldr	r0, [sp, #0x8c]
   2c990: e248300c     	sub	r3, r8, #12
   2c994: e1500003     	cmp	r0, r3
   2c998: 0a000000     	beq	0x2c9a0
   2c99c: ebffa527     	bl	0x15e40    @ imm = #-0x16b64 ; _ZdlPv
   2c9a0: ebffa631     	bl	0x1626c    @ imm = #-0x1673c ; __cxa_end_catch
   2c9a4: eafffbdf     	b	0x2b928
   2c9a8: e1a03000     	mov	r3, r0
   2c9ac: e1a0b001     	mov	r11, r1
   2c9b0: e3510001     	cmp	r1, #1
   2c9b4: 1a0000e3     	bne	0x2cd48
   2c9b8: ebffa451     	bl	0x15b04    @ imm = #-0x16ebc ; __cxa_begin_catch
   2c9bc: e28d3e4a     	add	r3, sp, #1184
   2c9c0: e1a0b000     	mov	r11, r0
   2c9c4: e2430e41     	sub	r0, r3, #1040
   2c9c8: e301296c     	movw	r2, #0x196c
   2c9cc: e3402007     	movt	r2, #0x7
   2c9d0: e2400004     	sub	r0, r0, #4
   2c9d4: e2871004     	add	r1, r7, #4
   2c9d8: eb00087a     	bl	0x2ebc8
   2c9dc: e59b3000     	ldr	r3, [r11]
   2c9e0: e1a0000b     	mov	r0, r11
   2c9e4: e5933008     	ldr	r3, [r3, #0x8]
   2c9e8: e12fff33     	blx	r3
   2c9ec: e1a01000     	mov	r1, r0
   2c9f0: e1a00006     	mov	r0, r6
   2c9f4: ebffa694     	bl	0x1644c    @ imm = #-0x165b0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2c9f8: e1a01000     	mov	r1, r0
   2c9fc: e1a00004     	mov	r0, r4
   2ca00: ebffa47e     	bl	0x15c00    @ imm = #-0x16e08 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2ca04: e59d0008     	ldr	r0, [sp, #0x8]
   2ca08: e3a02002     	mov	r2, #2
   2ca0c: e1a01004     	mov	r1, r4
   2ca10: eb010d42     	bl	0x6ff20
   2ca14: e59d00a4     	ldr	r0, [sp, #0xa4]
   2ca18: e24930f4     	sub	r3, r9, #244
   2ca1c: e1500003     	cmp	r0, r3
   2ca20: 0a000000     	beq	0x2ca28
   2ca24: ebffa505     	bl	0x15e40    @ imm = #-0x16bec ; _ZdlPv
   2ca28: e59d008c     	ldr	r0, [sp, #0x8c]
   2ca2c: e248300c     	sub	r3, r8, #12
   2ca30: e1500003     	cmp	r0, r3
   2ca34: 0a000000     	beq	0x2ca3c
   2ca38: ebffa500     	bl	0x15e40    @ imm = #-0x16c00 ; _ZdlPv
   2ca3c: ebffa60a     	bl	0x1626c    @ imm = #-0x167d8 ; __cxa_end_catch
   2ca40: eafffb82     	b	0x2b850
   2ca44: e1a03000     	mov	r3, r0
   2ca48: e1a0b001     	mov	r11, r1
   2ca4c: e3510001     	cmp	r1, #1
   2ca50: 1affff7f     	bne	0x2c854
   2ca54: ebffa42a     	bl	0x15b04    @ imm = #-0x16f58 ; __cxa_begin_catch
   2ca58: e28d3e4a     	add	r3, sp, #1184
   2ca5c: e1a0b000     	mov	r11, r0
   2ca60: e2430e41     	sub	r0, r3, #1040
   2ca64: e3012a48     	movw	r2, #0x1a48
   2ca68: e3402007     	movt	r2, #0x7
   2ca6c: e2400004     	sub	r0, r0, #4
   2ca70: e2871004     	add	r1, r7, #4
   2ca74: eb000853     	bl	0x2ebc8
   2ca78: e59b3000     	ldr	r3, [r11]
   2ca7c: e1a0000b     	mov	r0, r11
   2ca80: e5933008     	ldr	r3, [r3, #0x8]
   2ca84: e12fff33     	blx	r3
   2ca88: e1a01000     	mov	r1, r0
   2ca8c: e1a00006     	mov	r0, r6
   2ca90: ebffa66d     	bl	0x1644c    @ imm = #-0x1664c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2ca94: e1a01000     	mov	r1, r0
   2ca98: e1a00004     	mov	r0, r4
   2ca9c: ebffa457     	bl	0x15c00    @ imm = #-0x16ea4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2caa0: e59d0008     	ldr	r0, [sp, #0x8]
   2caa4: e3a02002     	mov	r2, #2
   2caa8: e1a01004     	mov	r1, r4
   2caac: eb010d1b     	bl	0x6ff20
   2cab0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cab4: e24930f4     	sub	r3, r9, #244
   2cab8: e1500003     	cmp	r0, r3
   2cabc: 0a000000     	beq	0x2cac4
   2cac0: ebffa4de     	bl	0x15e40    @ imm = #-0x16c88 ; _ZdlPv
   2cac4: e59d008c     	ldr	r0, [sp, #0x8c]
   2cac8: e248300c     	sub	r3, r8, #12
   2cacc: e1500003     	cmp	r0, r3
   2cad0: 0a000000     	beq	0x2cad8
   2cad4: ebffa4d9     	bl	0x15e40    @ imm = #-0x16c9c ; _ZdlPv
   2cad8: ebffa5e3     	bl	0x1626c    @ imm = #-0x16874 ; __cxa_end_catch
   2cadc: eafffb9e     	b	0x2b95c
   2cae0: e1a03000     	mov	r3, r0
   2cae4: e1a0b001     	mov	r11, r1
   2cae8: e3510001     	cmp	r1, #1
   2caec: 1affff58     	bne	0x2c854
   2caf0: ebffa403     	bl	0x15b04    @ imm = #-0x16ff4 ; __cxa_begin_catch
   2caf4: e28d3e4a     	add	r3, sp, #1184
   2caf8: e1a0b000     	mov	r11, r0
   2cafc: e2430e41     	sub	r0, r3, #1040
   2cb00: e3012a34     	movw	r2, #0x1a34
   2cb04: e3402007     	movt	r2, #0x7
   2cb08: e2400004     	sub	r0, r0, #4
   2cb0c: e2871004     	add	r1, r7, #4
   2cb10: eb00082c     	bl	0x2ebc8
   2cb14: e59b3000     	ldr	r3, [r11]
   2cb18: e1a0000b     	mov	r0, r11
   2cb1c: e5933008     	ldr	r3, [r3, #0x8]
   2cb20: e12fff33     	blx	r3
   2cb24: e1a01000     	mov	r1, r0
   2cb28: e1a00006     	mov	r0, r6
   2cb2c: ebffa646     	bl	0x1644c    @ imm = #-0x166e8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cb30: e1a01000     	mov	r1, r0
   2cb34: e1a00004     	mov	r0, r4
   2cb38: ebffa430     	bl	0x15c00    @ imm = #-0x16f40 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2cb3c: e59d0008     	ldr	r0, [sp, #0x8]
   2cb40: e3a02002     	mov	r2, #2
   2cb44: e1a01004     	mov	r1, r4
   2cb48: eb010cf4     	bl	0x6ff20
   2cb4c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cb50: e24930f4     	sub	r3, r9, #244
   2cb54: e1500003     	cmp	r0, r3
   2cb58: 0a000000     	beq	0x2cb60
   2cb5c: ebffa4b7     	bl	0x15e40    @ imm = #-0x16d24 ; _ZdlPv
   2cb60: e59d008c     	ldr	r0, [sp, #0x8c]
   2cb64: e248300c     	sub	r3, r8, #12
   2cb68: e1500003     	cmp	r0, r3
   2cb6c: 0a000000     	beq	0x2cb74
   2cb70: ebffa4b2     	bl	0x15e40    @ imm = #-0x16d38 ; _ZdlPv
   2cb74: ebffa5bc     	bl	0x1626c    @ imm = #-0x16910 ; __cxa_end_catch
   2cb78: eafffb71     	b	0x2b944
   2cb7c: e59dc0a4     	ldr	r12, [sp, #0xa4]
   2cb80: e24920f4     	sub	r2, r9, #244
   2cb84: e1a03000     	mov	r3, r0
   2cb88: e1a0b001     	mov	r11, r1
   2cb8c: e15c0002     	cmp	r12, r2
   2cb90: 0a000003     	beq	0x2cba4
   2cb94: e58d001c     	str	r0, [sp, #0x1c]
   2cb98: e1a0000c     	mov	r0, r12
   2cb9c: ebffa4a7     	bl	0x15e40    @ imm = #-0x16d64 ; _ZdlPv
   2cba0: e59d301c     	ldr	r3, [sp, #0x1c]
   2cba4: e59d008c     	ldr	r0, [sp, #0x8c]
   2cba8: e248200c     	sub	r2, r8, #12
   2cbac: e1500002     	cmp	r0, r2
   2cbb0: 0affff24     	beq	0x2c848
   2cbb4: e58d301c     	str	r3, [sp, #0x1c]
   2cbb8: ebffa4a0     	bl	0x15e40    @ imm = #-0x16d80 ; _ZdlPv
   2cbbc: e59d301c     	ldr	r3, [sp, #0x1c]
   2cbc0: eaffff20     	b	0x2c848
   2cbc4: e1a03000     	mov	r3, r0
   2cbc8: e1a0b001     	mov	r11, r1
   2cbcc: eafffff4     	b	0x2cba4
   2cbd0: eaffff1a     	b	0x2c840
   2cbd4: eaffffe8     	b	0x2cb7c
   2cbd8: eafffff9     	b	0x2cbc4
   2cbdc: eaffff17     	b	0x2c840
   2cbe0: e1a03000     	mov	r3, r0
   2cbe4: e1a0b001     	mov	r11, r1
   2cbe8: e3510001     	cmp	r1, #1
   2cbec: 1a000055     	bne	0x2cd48
   2cbf0: ebffa3c3     	bl	0x15b04    @ imm = #-0x170f4 ; __cxa_begin_catch
   2cbf4: e28d3e4a     	add	r3, sp, #1184
   2cbf8: e1a0b000     	mov	r11, r0
   2cbfc: e2430e41     	sub	r0, r3, #1040
   2cc00: e301299c     	movw	r2, #0x199c
   2cc04: e3402007     	movt	r2, #0x7
   2cc08: e2400004     	sub	r0, r0, #4
   2cc0c: e2871004     	add	r1, r7, #4
   2cc10: eb0007ec     	bl	0x2ebc8
   2cc14: e59b3000     	ldr	r3, [r11]
   2cc18: e1a0000b     	mov	r0, r11
   2cc1c: e5933008     	ldr	r3, [r3, #0x8]
   2cc20: e12fff33     	blx	r3
   2cc24: e1a01000     	mov	r1, r0
   2cc28: e1a00006     	mov	r0, r6
   2cc2c: ebffa606     	bl	0x1644c    @ imm = #-0x167e8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cc30: e1a01000     	mov	r1, r0
   2cc34: e1a00004     	mov	r0, r4
   2cc38: ebffa3f0     	bl	0x15c00    @ imm = #-0x17040 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2cc3c: e59d0008     	ldr	r0, [sp, #0x8]
   2cc40: e3a02002     	mov	r2, #2
   2cc44: e1a01004     	mov	r1, r4
   2cc48: eb010cb4     	bl	0x6ff20
   2cc4c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cc50: e24930f4     	sub	r3, r9, #244
   2cc54: e1500003     	cmp	r0, r3
   2cc58: 0a000000     	beq	0x2cc60
   2cc5c: ebffa477     	bl	0x15e40    @ imm = #-0x16e24 ; _ZdlPv
   2cc60: e59d008c     	ldr	r0, [sp, #0x8c]
   2cc64: e248300c     	sub	r3, r8, #12
   2cc68: e1500003     	cmp	r0, r3
   2cc6c: 0a000000     	beq	0x2cc74
   2cc70: ebffa472     	bl	0x15e40    @ imm = #-0x16e38 ; _ZdlPv
   2cc74: ebffa57c     	bl	0x1626c    @ imm = #-0x16a10 ; __cxa_end_catch
   2cc78: eafffb00     	b	0x2b880
   2cc7c: e1a03000     	mov	r3, r0
   2cc80: e1a0b001     	mov	r11, r1
   2cc84: e3510001     	cmp	r1, #1
   2cc88: 1a00002e     	bne	0x2cd48
   2cc8c: ebffa39c     	bl	0x15b04    @ imm = #-0x17190 ; __cxa_begin_catch
   2cc90: e28d3e4a     	add	r3, sp, #1184
   2cc94: e1a0b000     	mov	r11, r0
   2cc98: e2430e41     	sub	r0, r3, #1040
   2cc9c: e3012984     	movw	r2, #0x1984
   2cca0: e3402007     	movt	r2, #0x7
   2cca4: e2400004     	sub	r0, r0, #4
   2cca8: e2871004     	add	r1, r7, #4
   2ccac: eb0007c5     	bl	0x2ebc8
   2ccb0: e59b3000     	ldr	r3, [r11]
   2ccb4: e1a0000b     	mov	r0, r11
   2ccb8: e5933008     	ldr	r3, [r3, #0x8]
   2ccbc: e12fff33     	blx	r3
   2ccc0: e1a01000     	mov	r1, r0
   2ccc4: e1a00006     	mov	r0, r6
   2ccc8: ebffa5df     	bl	0x1644c    @ imm = #-0x16884 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cccc: e1a01000     	mov	r1, r0
   2ccd0: e1a00004     	mov	r0, r4
   2ccd4: ebffa3c9     	bl	0x15c00    @ imm = #-0x170dc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2ccd8: e59d0008     	ldr	r0, [sp, #0x8]
   2ccdc: e3a02002     	mov	r2, #2
   2cce0: e1a01004     	mov	r1, r4
   2cce4: eb010c8d     	bl	0x6ff20
   2cce8: e59d00a4     	ldr	r0, [sp, #0xa4]
   2ccec: e24930f4     	sub	r3, r9, #244
   2ccf0: e1500003     	cmp	r0, r3
   2ccf4: 0a000000     	beq	0x2ccfc
   2ccf8: ebffa450     	bl	0x15e40    @ imm = #-0x16ec0 ; _ZdlPv
   2ccfc: e59d008c     	ldr	r0, [sp, #0x8c]
   2cd00: e248300c     	sub	r3, r8, #12
   2cd04: e1500003     	cmp	r0, r3
   2cd08: 0a000000     	beq	0x2cd10
   2cd0c: ebffa44b     	bl	0x15e40    @ imm = #-0x16ed4 ; _ZdlPv
   2cd10: ebffa555     	bl	0x1626c    @ imm = #-0x16aac ; __cxa_end_catch
   2cd14: eafffad3     	b	0x2b868
   2cd18: e1a03000     	mov	r3, r0
   2cd1c: e1a0b001     	mov	r11, r1
   2cd20: e59d008c     	ldr	r0, [sp, #0x8c]
   2cd24: e248200c     	sub	r2, r8, #12
   2cd28: e1500002     	cmp	r0, r2
   2cd2c: 0a000002     	beq	0x2cd3c
   2cd30: e58d301c     	str	r3, [sp, #0x1c]
   2cd34: ebffa441     	bl	0x15e40    @ imm = #-0x16efc ; _ZdlPv
   2cd38: e59d301c     	ldr	r3, [sp, #0x1c]
   2cd3c: e58d301c     	str	r3, [sp, #0x1c]
   2cd40: ebffa549     	bl	0x1626c    @ imm = #-0x16adc ; __cxa_end_catch
   2cd44: e59d301c     	ldr	r3, [sp, #0x1c]
   2cd48: e1a00005     	mov	r0, r5
   2cd4c: e58d301c     	str	r3, [sp, #0x1c]
   2cd50: eb00d12f     	bl	0x61214
   2cd54: e59d301c     	ldr	r3, [sp, #0x1c]
   2cd58: e1a0500b     	mov	r5, r11
   2cd5c: e1a00003     	mov	r0, r3
   2cd60: e3550001     	cmp	r5, #1
   2cd64: 1afffddf     	bne	0x2c4e8
   2cd68: ebffa365     	bl	0x15b04    @ imm = #-0x1726c ; __cxa_begin_catch
   2cd6c: e28d3e4a     	add	r3, sp, #1184
   2cd70: e1a05000     	mov	r5, r0
   2cd74: e2430e41     	sub	r0, r3, #1040
   2cd78: e30129b4     	movw	r2, #0x19b4
   2cd7c: e3402007     	movt	r2, #0x7
   2cd80: e2400004     	sub	r0, r0, #4
   2cd84: e2871004     	add	r1, r7, #4
   2cd88: eb00078e     	bl	0x2ebc8
   2cd8c: e5953000     	ldr	r3, [r5]
   2cd90: e1a00005     	mov	r0, r5
   2cd94: e5933008     	ldr	r3, [r3, #0x8]
   2cd98: e12fff33     	blx	r3
   2cd9c: e1a01000     	mov	r1, r0
   2cda0: e1a00006     	mov	r0, r6
   2cda4: ebffa5a8     	bl	0x1644c    @ imm = #-0x16960 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cda8: e1a01000     	mov	r1, r0
   2cdac: e1a00004     	mov	r0, r4
   2cdb0: ebffa392     	bl	0x15c00    @ imm = #-0x171b8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2cdb4: e59d0008     	ldr	r0, [sp, #0x8]
   2cdb8: e3a02002     	mov	r2, #2
   2cdbc: e1a01004     	mov	r1, r4
   2cdc0: eb010c56     	bl	0x6ff20
   2cdc4: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cdc8: e24930f4     	sub	r3, r9, #244
   2cdcc: e1500003     	cmp	r0, r3
   2cdd0: 0a000000     	beq	0x2cdd8
   2cdd4: ebffa419     	bl	0x15e40    @ imm = #-0x16f9c ; _ZdlPv
   2cdd8: e59d008c     	ldr	r0, [sp, #0x8c]
   2cddc: e248300c     	sub	r3, r8, #12
   2cde0: e1500003     	cmp	r0, r3
   2cde4: 0a000000     	beq	0x2cdec
   2cde8: ebffa414     	bl	0x15e40    @ imm = #-0x16fb0 ; _ZdlPv
   2cdec: ebffa51e     	bl	0x1626c    @ imm = #-0x16b88 ; __cxa_end_catch
   2cdf0: eafffaa4     	b	0x2b888
   2cdf4: e1a03000     	mov	r3, r0
   2cdf8: e1a0b001     	mov	r11, r1
   2cdfc: eaffffce     	b	0x2cd3c
   2ce00: e59dc0a4     	ldr	r12, [sp, #0xa4]
   2ce04: e24920f4     	sub	r2, r9, #244
   2ce08: e1a03000     	mov	r3, r0
   2ce0c: e1a0b001     	mov	r11, r1
   2ce10: e15c0002     	cmp	r12, r2
   2ce14: 0affffc1     	beq	0x2cd20
   2ce18: e58d001c     	str	r0, [sp, #0x1c]
   2ce1c: e1a0000c     	mov	r0, r12
   2ce20: ebffa406     	bl	0x15e40    @ imm = #-0x16fe8 ; _ZdlPv
   2ce24: e59d301c     	ldr	r3, [sp, #0x1c]
   2ce28: eaffffbc     	b	0x2cd20
   2ce2c: eafffff3     	b	0x2ce00
   2ce30: eaffffb8     	b	0x2cd18
   2ce34: e1a03000     	mov	r3, r0
   2ce38: e1a0b001     	mov	r11, r1
   2ce3c: e3510001     	cmp	r1, #1
   2ce40: 1afffe83     	bne	0x2c854
   2ce44: ebffa32e     	bl	0x15b04    @ imm = #-0x17348 ; __cxa_begin_catch
   2ce48: e28d3e4a     	add	r3, sp, #1184
   2ce4c: e1a0b000     	mov	r11, r0
   2ce50: e2430e41     	sub	r0, r3, #1040
   2ce54: e30129f4     	movw	r2, #0x19f4
   2ce58: e3402007     	movt	r2, #0x7
   2ce5c: e2400004     	sub	r0, r0, #4
   2ce60: e2871004     	add	r1, r7, #4
   2ce64: eb000757     	bl	0x2ebc8
   2ce68: e59b3000     	ldr	r3, [r11]
   2ce6c: e1a0000b     	mov	r0, r11
   2ce70: e5933008     	ldr	r3, [r3, #0x8]
   2ce74: e12fff33     	blx	r3
   2ce78: e1a01000     	mov	r1, r0
   2ce7c: e1a00006     	mov	r0, r6
   2ce80: ebffa571     	bl	0x1644c    @ imm = #-0x16a3c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2ce84: e1a01000     	mov	r1, r0
   2ce88: e1a00004     	mov	r0, r4
   2ce8c: ebffa35b     	bl	0x15c00    @ imm = #-0x17294 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2ce90: e59d0008     	ldr	r0, [sp, #0x8]
   2ce94: e3a02002     	mov	r2, #2
   2ce98: e1a01004     	mov	r1, r4
   2ce9c: eb010c1f     	bl	0x6ff20
   2cea0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cea4: e24930f4     	sub	r3, r9, #244
   2cea8: e1500003     	cmp	r0, r3
   2ceac: 0a000000     	beq	0x2ceb4
   2ceb0: ebffa3e2     	bl	0x15e40    @ imm = #-0x17078 ; _ZdlPv
   2ceb4: e59d008c     	ldr	r0, [sp, #0x8c]
   2ceb8: e248300c     	sub	r3, r8, #12
   2cebc: e1500003     	cmp	r0, r3
   2cec0: 0a000000     	beq	0x2cec8
   2cec4: ebffa3dd     	bl	0x15e40    @ imm = #-0x1708c ; _ZdlPv
   2cec8: ebffa4e7     	bl	0x1626c    @ imm = #-0x16c64 ; __cxa_end_catch
   2cecc: eafffa88     	b	0x2b8f4
   2ced0: e1a03000     	mov	r3, r0
   2ced4: e1a0b001     	mov	r11, r1
   2ced8: e3510001     	cmp	r1, #1
   2cedc: 1afffe5c     	bne	0x2c854
   2cee0: ebffa307     	bl	0x15b04    @ imm = #-0x173e4 ; __cxa_begin_catch
   2cee4: e28d3e4a     	add	r3, sp, #1184
   2cee8: e1a0b000     	mov	r11, r0
   2ceec: e2430e41     	sub	r0, r3, #1040
   2cef0: e30129e0     	movw	r2, #0x19e0
   2cef4: e3402007     	movt	r2, #0x7
   2cef8: e2400004     	sub	r0, r0, #4
   2cefc: e2871004     	add	r1, r7, #4
   2cf00: eb000730     	bl	0x2ebc8
   2cf04: e59b3000     	ldr	r3, [r11]
   2cf08: e1a0000b     	mov	r0, r11
   2cf0c: e5933008     	ldr	r3, [r3, #0x8]
   2cf10: e12fff33     	blx	r3
   2cf14: e1a01000     	mov	r1, r0
   2cf18: e1a00006     	mov	r0, r6
   2cf1c: ebffa54a     	bl	0x1644c    @ imm = #-0x16ad8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cf20: e1a01000     	mov	r1, r0
   2cf24: e1a00004     	mov	r0, r4
   2cf28: ebffa334     	bl	0x15c00    @ imm = #-0x17330 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2cf2c: e59d0008     	ldr	r0, [sp, #0x8]
   2cf30: e3a02002     	mov	r2, #2
   2cf34: e1a01004     	mov	r1, r4
   2cf38: eb010bf8     	bl	0x6ff20
   2cf3c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cf40: e24930f4     	sub	r3, r9, #244
   2cf44: e1500003     	cmp	r0, r3
   2cf48: 0a000000     	beq	0x2cf50
   2cf4c: ebffa3bb     	bl	0x15e40    @ imm = #-0x17114 ; _ZdlPv
   2cf50: e59d008c     	ldr	r0, [sp, #0x8c]
   2cf54: e248300c     	sub	r3, r8, #12
   2cf58: e1500003     	cmp	r0, r3
   2cf5c: 0a000000     	beq	0x2cf64
   2cf60: ebffa3b6     	bl	0x15e40    @ imm = #-0x17128 ; _ZdlPv
   2cf64: ebffa4c0     	bl	0x1626c    @ imm = #-0x16d00 ; __cxa_end_catch
   2cf68: eafffa5a     	b	0x2b8d8
   2cf6c: eaffff14     	b	0x2cbc4
   2cf70: eafffe32     	b	0x2c840
   2cf74: eaffff00     	b	0x2cb7c
   2cf78: eafffeff     	b	0x2cb7c
   2cf7c: eaffff10     	b	0x2cbc4
   2cf80: eafffe2e     	b	0x2c840
   2cf84: e1a03000     	mov	r3, r0
   2cf88: e1a0b001     	mov	r11, r1
   2cf8c: e3510001     	cmp	r1, #1
   2cf90: 1afffe2f     	bne	0x2c854
   2cf94: ebffa2da     	bl	0x15b04    @ imm = #-0x17498 ; __cxa_begin_catch
   2cf98: e28d3e4a     	add	r3, sp, #1184
   2cf9c: e1a0b000     	mov	r11, r0
   2cfa0: e2430e41     	sub	r0, r3, #1040
   2cfa4: e3012a0c     	movw	r2, #0x1a0c
   2cfa8: e3402007     	movt	r2, #0x7
   2cfac: e2400004     	sub	r0, r0, #4
   2cfb0: e2871004     	add	r1, r7, #4
   2cfb4: eb000703     	bl	0x2ebc8
   2cfb8: e59b3000     	ldr	r3, [r11]
   2cfbc: e1a0000b     	mov	r0, r11
   2cfc0: e5933008     	ldr	r3, [r3, #0x8]
   2cfc4: e12fff33     	blx	r3
   2cfc8: e1a01000     	mov	r1, r0
   2cfcc: e1a00006     	mov	r0, r6
   2cfd0: ebffa51d     	bl	0x1644c    @ imm = #-0x16b8c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2cfd4: e1a01000     	mov	r1, r0
   2cfd8: e1a00004     	mov	r0, r4
   2cfdc: ebffa307     	bl	0x15c00    @ imm = #-0x173e4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2cfe0: e59d0008     	ldr	r0, [sp, #0x8]
   2cfe4: e3a02002     	mov	r2, #2
   2cfe8: e1a01004     	mov	r1, r4
   2cfec: eb010bcb     	bl	0x6ff20
   2cff0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2cff4: e24930f4     	sub	r3, r9, #244
   2cff8: e1500003     	cmp	r0, r3
   2cffc: 0a000000     	beq	0x2d004
   2d000: ebffa38e     	bl	0x15e40    @ imm = #-0x171c8 ; _ZdlPv
   2d004: e59d008c     	ldr	r0, [sp, #0x8c]
   2d008: e248300c     	sub	r3, r8, #12
   2d00c: e1500003     	cmp	r0, r3
   2d010: 0a000000     	beq	0x2d018
   2d014: ebffa389     	bl	0x15e40    @ imm = #-0x171dc ; _ZdlPv
   2d018: ebffa493     	bl	0x1626c    @ imm = #-0x16db4 ; __cxa_end_catch
   2d01c: eafffa3a     	b	0x2b90c
   2d020: eafffed5     	b	0x2cb7c
   2d024: eafffee6     	b	0x2cbc4
   2d028: eafffe04     	b	0x2c840
   2d02c: eafffed2     	b	0x2cb7c
   2d030: eafffee3     	b	0x2cbc4
   2d034: eafffe01     	b	0x2c840
   2d038: eafffecf     	b	0x2cb7c
   2d03c: eafffee0     	b	0x2cbc4
   2d040: eafffdfe     	b	0x2c840
   2d044: eafffecc     	b	0x2cb7c
   2d048: eafffedd     	b	0x2cbc4
   2d04c: eafffdfb     	b	0x2c840
   2d050: eafffd4c     	b	0x2c588
   2d054: e1a03000     	mov	r3, r0
   2d058: e1a05001     	mov	r5, r1
   2d05c: e3510001     	cmp	r1, #1
   2d060: 1a000049     	bne	0x2d18c
   2d064: ebffa2a6     	bl	0x15b04    @ imm = #-0x17568 ; __cxa_begin_catch
   2d068: e28d3e4a     	add	r3, sp, #1184
   2d06c: e1a05000     	mov	r5, r0
   2d070: e2430e41     	sub	r0, r3, #1040
   2d074: e3012958     	movw	r2, #0x1958
   2d078: e3402007     	movt	r2, #0x7
   2d07c: e2400004     	sub	r0, r0, #4
   2d080: e2871004     	add	r1, r7, #4
   2d084: eb0006cf     	bl	0x2ebc8
   2d088: e5953000     	ldr	r3, [r5]
   2d08c: e1a00005     	mov	r0, r5
   2d090: e5933008     	ldr	r3, [r3, #0x8]
   2d094: e12fff33     	blx	r3
   2d098: e1a01000     	mov	r1, r0
   2d09c: e1a00006     	mov	r0, r6
   2d0a0: ebffa4e9     	bl	0x1644c    @ imm = #-0x16c5c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2d0a4: e1a01000     	mov	r1, r0
   2d0a8: e1a00004     	mov	r0, r4
   2d0ac: ebffa2d3     	bl	0x15c00    @ imm = #-0x174b4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2d0b0: e59d0008     	ldr	r0, [sp, #0x8]
   2d0b4: e3a02002     	mov	r2, #2
   2d0b8: e1a01004     	mov	r1, r4
   2d0bc: eb010b97     	bl	0x6ff20
   2d0c0: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d0c4: e24930f4     	sub	r3, r9, #244
   2d0c8: e1500003     	cmp	r0, r3
   2d0cc: 0a000000     	beq	0x2d0d4
   2d0d0: ebffa35a     	bl	0x15e40    @ imm = #-0x17298 ; _ZdlPv
   2d0d4: e59d008c     	ldr	r0, [sp, #0x8c]
   2d0d8: e248300c     	sub	r3, r8, #12
   2d0dc: e1500003     	cmp	r0, r3
   2d0e0: 0a000000     	beq	0x2d0e8
   2d0e4: ebffa355     	bl	0x15e40    @ imm = #-0x172ac ; _ZdlPv
   2d0e8: ebffa45f     	bl	0x1626c    @ imm = #-0x16e84 ; __cxa_end_catch
   2d0ec: eafff9c9     	b	0x2b818
   2d0f0: e1a03000     	mov	r3, r0
   2d0f4: e1a05001     	mov	r5, r1
   2d0f8: e3510001     	cmp	r1, #1
   2d0fc: 1a000022     	bne	0x2d18c
   2d100: ebffa27f     	bl	0x15b04    @ imm = #-0x17604 ; __cxa_begin_catch
   2d104: e28d3e4a     	add	r3, sp, #1184
   2d108: e1a05000     	mov	r5, r0
   2d10c: e2430e41     	sub	r0, r3, #1040
   2d110: e3012944     	movw	r2, #0x1944
   2d114: e3402007     	movt	r2, #0x7
   2d118: e2400004     	sub	r0, r0, #4
   2d11c: e2871004     	add	r1, r7, #4
   2d120: eb0006a8     	bl	0x2ebc8
   2d124: e5953000     	ldr	r3, [r5]
   2d128: e1a00005     	mov	r0, r5
   2d12c: e5933008     	ldr	r3, [r3, #0x8]
   2d130: e12fff33     	blx	r3
   2d134: e1a01000     	mov	r1, r0
   2d138: e1a00006     	mov	r0, r6
   2d13c: ebffa4c2     	bl	0x1644c    @ imm = #-0x16cf8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2d140: e1a01000     	mov	r1, r0
   2d144: e1a00004     	mov	r0, r4
   2d148: ebffa2ac     	bl	0x15c00    @ imm = #-0x17550 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2d14c: e59d0008     	ldr	r0, [sp, #0x8]
   2d150: e3a02002     	mov	r2, #2
   2d154: e1a01004     	mov	r1, r4
   2d158: eb010b70     	bl	0x6ff20
   2d15c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d160: e24930f4     	sub	r3, r9, #244
   2d164: e1500003     	cmp	r0, r3
   2d168: 0a000000     	beq	0x2d170
   2d16c: ebffa333     	bl	0x15e40    @ imm = #-0x17334 ; _ZdlPv
   2d170: e59d008c     	ldr	r0, [sp, #0x8c]
   2d174: e248300c     	sub	r3, r8, #12
   2d178: e1500003     	cmp	r0, r3
   2d17c: 0a000000     	beq	0x2d184
   2d180: ebffa32e     	bl	0x15e40    @ imm = #-0x17348 ; _ZdlPv
   2d184: ebffa438     	bl	0x1626c    @ imm = #-0x16f20 ; __cxa_end_catch
   2d188: eafff99c     	b	0x2b800
   2d18c: e1a04003     	mov	r4, r3
   2d190: eafffcd5     	b	0x2c4ec
   2d194: eaffff19     	b	0x2ce00
   2d198: eafffede     	b	0x2cd18
   2d19c: eaffff14     	b	0x2cdf4
   2d1a0: eafffd07     	b	0x2c5c4
   2d1a4: eafffd09     	b	0x2c5d0
   2d1a8: eaffff11     	b	0x2cdf4
   2d1ac: ebffa254     	bl	0x15b04    @ imm = #-0x176b0 ; __cxa_begin_catch
   2d1b0: e28d3e4a     	add	r3, sp, #1184
   2d1b4: e1a05000     	mov	r5, r0
   2d1b8: e2430e41     	sub	r0, r3, #1040
   2d1bc: e3012aa8     	movw	r2, #0x1aa8
   2d1c0: e3402007     	movt	r2, #0x7
   2d1c4: e2400004     	sub	r0, r0, #4
   2d1c8: e2871004     	add	r1, r7, #4
   2d1cc: eb00067d     	bl	0x2ebc8
   2d1d0: e5953000     	ldr	r3, [r5]
   2d1d4: e1a00005     	mov	r0, r5
   2d1d8: e5933008     	ldr	r3, [r3, #0x8]
   2d1dc: e12fff33     	blx	r3
   2d1e0: e1a01000     	mov	r1, r0
   2d1e4: e1a00006     	mov	r0, r6
   2d1e8: ebffa497     	bl	0x1644c    @ imm = #-0x16da4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2d1ec: e1a01000     	mov	r1, r0
   2d1f0: e1a00004     	mov	r0, r4
   2d1f4: ebffa281     	bl	0x15c00    @ imm = #-0x175fc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2d1f8: e59d0008     	ldr	r0, [sp, #0x8]
   2d1fc: e3a02002     	mov	r2, #2
   2d200: e1a01004     	mov	r1, r4
   2d204: eb010b45     	bl	0x6ff20
   2d208: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d20c: e24930f4     	sub	r3, r9, #244
   2d210: e1500003     	cmp	r0, r3
   2d214: 0a000000     	beq	0x2d21c
   2d218: ebffa308     	bl	0x15e40    @ imm = #-0x173e0 ; _ZdlPv
   2d21c: e59d008c     	ldr	r0, [sp, #0x8c]
   2d220: e248300c     	sub	r3, r8, #12
   2d224: e1500003     	cmp	r0, r3
   2d228: 0a000000     	beq	0x2d230
   2d22c: ebffa303     	bl	0x15e40    @ imm = #-0x173f4 ; _ZdlPv
   2d230: ebffa40d     	bl	0x1626c    @ imm = #-0x16fcc ; __cxa_end_catch
   2d234: eafff9df     	b	0x2b9b8
   2d238: e1a05001     	mov	r5, r1
   2d23c: eafffec7     	b	0x2cd60
   2d240: eafffcdf     	b	0x2c5c4
   2d244: eafffce1     	b	0x2c5d0
   2d248: eafffe4b     	b	0x2cb7c
   2d24c: eafffe5c     	b	0x2cbc4
   2d250: eafffd7a     	b	0x2c840
   2d254: eafffccb     	b	0x2c588
   2d258: eafffcd9     	b	0x2c5c4
   2d25c: eafffcdb     	b	0x2c5d0
   2d260: eafffcd7     	b	0x2c5c4
   2d264: eafffcd9     	b	0x2c5d0
   2d268: eafffcc6     	b	0x2c588
   2d26c: eafffcc5     	b	0x2c588
   2d270: e1a0b000     	mov	r11, r0
   2d274: e1a05001     	mov	r5, r1
   2d278: e3510002     	cmp	r1, #2
   2d27c: 1afffc6c     	bne	0x2c434
   2d280: ebffa21f     	bl	0x15b04    @ imm = #-0x17784 ; __cxa_begin_catch
   2d284: e301159c     	movw	r1, #0x159c
   2d288: e3401007     	movt	r1, #0x7
   2d28c: e1a05000     	mov	r5, r0
   2d290: e3a02018     	mov	r2, #24
   2d294: e30f0764     	movw	r0, #0xf764
   2d298: e3400008     	movt	r0, #0x8
   2d29c: ebffa371     	bl	0x16068    @ imm = #-0x1723c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d2a0: e5953000     	ldr	r3, [r5]
   2d2a4: e1a00005     	mov	r0, r5
   2d2a8: e5933008     	ldr	r3, [r3, #0x8]
   2d2ac: e12fff33     	blx	r3
   2d2b0: e1a01000     	mov	r1, r0
   2d2b4: e30f0764     	movw	r0, #0xf764
   2d2b8: e3400008     	movt	r0, #0x8
   2d2bc: ebffa2f4     	bl	0x15e94    @ imm = #-0x17430 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2d2c0: e3a02001     	mov	r2, #1
   2d2c4: e1a01004     	mov	r1, r4
   2d2c8: e3a0300a     	mov	r3, #10
   2d2cc: e5cd30a4     	strb	r3, [sp, #0xa4]
   2d2d0: ebffa364     	bl	0x16068    @ imm = #-0x17270 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d2d4: e30f3cf8     	movw	r3, #0xfcf8
   2d2d8: e3403008     	movt	r3, #0x8
   2d2dc: e5933008     	ldr	r3, [r3, #0x8]
   2d2e0: e58d3208     	str	r3, [sp, #0x208]
   2d2e4: ebffa3e0     	bl	0x1626c    @ imm = #-0x17080 ; __cxa_end_catch
   2d2e8: eafff9eb     	b	0x2ba9c
   2d2ec: e1a0b000     	mov	r11, r0
   2d2f0: e1a05001     	mov	r5, r1
   2d2f4: e3510002     	cmp	r1, #2
   2d2f8: 1afffc4d     	bne	0x2c434
   2d2fc: ebffa200     	bl	0x15b04    @ imm = #-0x17800 ; __cxa_begin_catch
   2d300: e3011574     	movw	r1, #0x1574
   2d304: e3401007     	movt	r1, #0x7
   2d308: e1a05000     	mov	r5, r0
   2d30c: e3a02025     	mov	r2, #37
   2d310: e30f0764     	movw	r0, #0xf764
   2d314: e3400008     	movt	r0, #0x8
   2d318: ebffa352     	bl	0x16068    @ imm = #-0x172b8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d31c: e5953000     	ldr	r3, [r5]
   2d320: e1a00005     	mov	r0, r5
   2d324: e5933008     	ldr	r3, [r3, #0x8]
   2d328: e12fff33     	blx	r3
   2d32c: e1a01000     	mov	r1, r0
   2d330: e30f0764     	movw	r0, #0xf764
   2d334: e3400008     	movt	r0, #0x8
   2d338: ebffa2d5     	bl	0x15e94    @ imm = #-0x174ac ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2d33c: e3a02001     	mov	r2, #1
   2d340: e1a01004     	mov	r1, r4
   2d344: e3a0300a     	mov	r3, #10
   2d348: e5cd30a4     	strb	r3, [sp, #0xa4]
   2d34c: ebffa345     	bl	0x16068    @ imm = #-0x172ec ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d350: e30f3cf8     	movw	r3, #0xfcf8
   2d354: e3403008     	movt	r3, #0x8
   2d358: e5933010     	ldr	r3, [r3, #0x10]
   2d35c: e58d3210     	str	r3, [sp, #0x210]
   2d360: ebffa3c1     	bl	0x1626c    @ imm = #-0x170fc ; __cxa_end_catch
   2d364: eafff9c6     	b	0x2ba84
   2d368: eafffc2e     	b	0x2c428
   2d36c: eafffc2d     	b	0x2c428
   2d370: e1a0b000     	mov	r11, r0
   2d374: e1a05001     	mov	r5, r1
   2d378: e3510002     	cmp	r1, #2
   2d37c: 1afffc2c     	bne	0x2c434
   2d380: ebffa1df     	bl	0x15b04    @ imm = #-0x17884 ; __cxa_begin_catch
   2d384: e301154c     	movw	r1, #0x154c
   2d388: e3401007     	movt	r1, #0x7
   2d38c: e1a05000     	mov	r5, r0
   2d390: e3a02025     	mov	r2, #37
   2d394: e30f0764     	movw	r0, #0xf764
   2d398: e3400008     	movt	r0, #0x8
   2d39c: ebffa331     	bl	0x16068    @ imm = #-0x1733c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d3a0: e5953000     	ldr	r3, [r5]
   2d3a4: e1a00005     	mov	r0, r5
   2d3a8: e5933008     	ldr	r3, [r3, #0x8]
   2d3ac: e12fff33     	blx	r3
   2d3b0: e1a01000     	mov	r1, r0
   2d3b4: e30f0764     	movw	r0, #0xf764
   2d3b8: e3400008     	movt	r0, #0x8
   2d3bc: ebffa2b4     	bl	0x15e94    @ imm = #-0x17530 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2d3c0: e3a02001     	mov	r2, #1
   2d3c4: e1a01004     	mov	r1, r4
   2d3c8: e3a0300a     	mov	r3, #10
   2d3cc: e5cd30a4     	strb	r3, [sp, #0xa4]
   2d3d0: ebffa324     	bl	0x16068    @ imm = #-0x17370 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d3d4: e30f3cf8     	movw	r3, #0xfcf8
   2d3d8: e3403008     	movt	r3, #0x8
   2d3dc: e593300c     	ldr	r3, [r3, #0xc]
   2d3e0: e58d320c     	str	r3, [sp, #0x20c]
   2d3e4: ebffa3a0     	bl	0x1626c    @ imm = #-0x17180 ; __cxa_end_catch
   2d3e8: eafff99c     	b	0x2ba60
   2d3ec: e1a0b000     	mov	r11, r0
   2d3f0: e1a05001     	mov	r5, r1
   2d3f4: e3510002     	cmp	r1, #2
   2d3f8: 1afffc0d     	bne	0x2c434
   2d3fc: ebffa1c0     	bl	0x15b04    @ imm = #-0x17900 ; __cxa_begin_catch
   2d400: e3011530     	movw	r1, #0x1530
   2d404: e3401007     	movt	r1, #0x7
   2d408: e1a05000     	mov	r5, r0
   2d40c: e3a0201b     	mov	r2, #27
   2d410: e30f0764     	movw	r0, #0xf764
   2d414: e3400008     	movt	r0, #0x8
   2d418: ebffa312     	bl	0x16068    @ imm = #-0x173b8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d41c: e5953000     	ldr	r3, [r5]
   2d420: e1a00005     	mov	r0, r5
   2d424: e5933008     	ldr	r3, [r3, #0x8]
   2d428: e12fff33     	blx	r3
   2d42c: e1a01000     	mov	r1, r0
   2d430: e30f0764     	movw	r0, #0xf764
   2d434: e3400008     	movt	r0, #0x8
   2d438: ebffa295     	bl	0x15e94    @ imm = #-0x175ac ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2d43c: e3a02001     	mov	r2, #1
   2d440: e1a01004     	mov	r1, r4
   2d444: e3a0300a     	mov	r3, #10
   2d448: e5cd30a4     	strb	r3, [sp, #0xa4]
   2d44c: ebffa305     	bl	0x16068    @ imm = #-0x173ec ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2d450: e30f3cf8     	movw	r3, #0xfcf8
   2d454: e3403008     	movt	r3, #0x8
   2d458: e5933004     	ldr	r3, [r3, #0x4]
   2d45c: e58d3204     	str	r3, [sp, #0x204]
   2d460: ebffa381     	bl	0x1626c    @ imm = #-0x171fc ; __cxa_end_catch
   2d464: eafff974     	b	0x2ba3c
   2d468: eafffbee     	b	0x2c428
   2d46c: eafffbed     	b	0x2c428
   2d470: e59d20a4     	ldr	r2, [sp, #0xa4]
   2d474: e24930f4     	sub	r3, r9, #244
   2d478: e1a0b000     	mov	r11, r0
   2d47c: e1a05001     	mov	r5, r1
   2d480: e1520003     	cmp	r2, r3
   2d484: 0afffbea     	beq	0x2c434
   2d488: e1a00002     	mov	r0, r2
   2d48c: ebffa26b     	bl	0x15e40    @ imm = #-0x17654 ; _ZdlPv
   2d490: eafffbe7     	b	0x2c434
   2d494: e1a0b000     	mov	r11, r0
   2d498: e1a05001     	mov	r5, r1
   2d49c: eafffbe4     	b	0x2c434
   2d4a0: e1a05001     	mov	r5, r1
   2d4a4: eafffbe5     	b	0x2c440
   2d4a8: e1a03000     	mov	r3, r0
   2d4ac: e1a0b001     	mov	r11, r1
   2d4b0: e3510001     	cmp	r1, #1
   2d4b4: 1afffce6     	bne	0x2c854
   2d4b8: ebffa191     	bl	0x15b04    @ imm = #-0x179bc ; __cxa_begin_catch
   2d4bc: e28d3e4a     	add	r3, sp, #1184
   2d4c0: e1a0b000     	mov	r11, r0
   2d4c4: e2430e41     	sub	r0, r3, #1040
   2d4c8: e3012a8c     	movw	r2, #0x1a8c
   2d4cc: e3402007     	movt	r2, #0x7
   2d4d0: e2400004     	sub	r0, r0, #4
   2d4d4: e2871004     	add	r1, r7, #4
   2d4d8: eb0005ba     	bl	0x2ebc8
   2d4dc: e59b3000     	ldr	r3, [r11]
   2d4e0: e1a0000b     	mov	r0, r11
   2d4e4: e5933008     	ldr	r3, [r3, #0x8]
   2d4e8: e12fff33     	blx	r3
   2d4ec: e1a01000     	mov	r1, r0
   2d4f0: e1a00006     	mov	r0, r6
   2d4f4: ebffa3d4     	bl	0x1644c    @ imm = #-0x170b0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   2d4f8: e1a01000     	mov	r1, r0
   2d4fc: e1a00004     	mov	r0, r4
   2d500: ebffa1be     	bl	0x15c00    @ imm = #-0x17908 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   2d504: e59d0008     	ldr	r0, [sp, #0x8]
   2d508: e3a02002     	mov	r2, #2
   2d50c: e1a01004     	mov	r1, r4
   2d510: eb010a82     	bl	0x6ff20
   2d514: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d518: e24930f4     	sub	r3, r9, #244
   2d51c: e1500003     	cmp	r0, r3
   2d520: 0a000000     	beq	0x2d528
   2d524: ebffa245     	bl	0x15e40    @ imm = #-0x176ec ; _ZdlPv
   2d528: e59d008c     	ldr	r0, [sp, #0x8c]
   2d52c: e248300c     	sub	r3, r8, #12
   2d530: e1500003     	cmp	r0, r3
   2d534: 0a000000     	beq	0x2d53c
   2d538: ebffa240     	bl	0x15e40    @ imm = #-0x17700 ; _ZdlPv
   2d53c: ebffa34a     	bl	0x1626c    @ imm = #-0x172d8 ; __cxa_end_catch
   2d540: eafff91a     	b	0x2b9b0
   2d544: e1a04000     	mov	r4, r0
   2d548: e1a05001     	mov	r5, r1
   2d54c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d550: e24990f4     	sub	r9, r9, #244
   2d554: e1500009     	cmp	r0, r9
   2d558: 0a000029     	beq	0x2d604
   2d55c: ebffa237     	bl	0x15e40    @ imm = #-0x17724 ; _ZdlPv
   2d560: ea000027     	b	0x2d604
   2d564: e59d31c0     	ldr	r3, [sp, #0x1c0]
   2d568: e24aa0d8     	sub	r10, r10, #216
   2d56c: e1a04000     	mov	r4, r0
   2d570: e1a05001     	mov	r5, r1
   2d574: e153000a     	cmp	r3, r10
   2d578: 0a000001     	beq	0x2d584
   2d57c: e1a00003     	mov	r0, r3
   2d580: ebffa22e     	bl	0x15e40    @ imm = #-0x17748 ; _ZdlPv
   2d584: e59d0074     	ldr	r0, [sp, #0x74]
   2d588: e2488024     	sub	r8, r8, #36
   2d58c: e1500008     	cmp	r0, r8
   2d590: 0a000000     	beq	0x2d598
   2d594: ebffa229     	bl	0x15e40    @ imm = #-0x1775c ; _ZdlPv
   2d598: e1a00004     	mov	r0, r4
   2d59c: e3550001     	cmp	r5, #1
   2d5a0: 1afffbf7     	bne	0x2c584
   2d5a4: ebffa156     	bl	0x15b04    @ imm = #-0x17aa8 ; __cxa_begin_catch
   2d5a8: e5903000     	ldr	r3, [r0]
   2d5ac: e28dae2a     	add	r10, sp, #672
   2d5b0: e24a50e0     	sub	r5, r10, #224
   2d5b4: e5933008     	ldr	r3, [r3, #0x8]
   2d5b8: e12fff33     	blx	r3
   2d5bc: e1a01000     	mov	r1, r0
   2d5c0: e1a00005     	mov	r0, r5
   2d5c4: ebffe977     	bl	0x27ba8
   2d5c8: e59d0008     	ldr	r0, [sp, #0x8]
   2d5cc: e1a01005     	mov	r1, r5
   2d5d0: e3a02002     	mov	r2, #2
   2d5d4: eb010a51     	bl	0x6ff20
   2d5d8: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2d5dc: e24a30d8     	sub	r3, r10, #216
   2d5e0: e1500003     	cmp	r0, r3
   2d5e4: 0a000000     	beq	0x2d5ec
   2d5e8: ebffa214     	bl	0x15e40    @ imm = #-0x177b0 ; _ZdlPv
   2d5ec: ebffa31e     	bl	0x1626c    @ imm = #-0x17388 ; __cxa_end_catch
   2d5f0: eafff7c4     	b	0x2b508
   2d5f4: e1a04000     	mov	r4, r0
   2d5f8: e1a00005     	mov	r0, r5
   2d5fc: e1a05001     	mov	r5, r1
   2d600: eb0004ab     	bl	0x2e8b4
   2d604: e59d008c     	ldr	r0, [sp, #0x8c]
   2d608: e248300c     	sub	r3, r8, #12
   2d60c: e1500003     	cmp	r0, r3
   2d610: 0affffdb     	beq	0x2d584
   2d614: ebffa209     	bl	0x15e40    @ imm = #-0x177dc ; _ZdlPv
   2d618: eaffffd9     	b	0x2d584
   2d61c: e3a00008     	mov	r0, #8
   2d620: ebffa155     	bl	0x15b7c    @ imm = #-0x17aac ; __cxa_allocate_exception
   2d624: e28d3e4a     	add	r3, sp, #1184
   2d628: e1a06000     	mov	r6, r0
   2d62c: e30121b0     	movw	r2, #0x11b0
   2d630: e3402007     	movt	r2, #0x7
   2d634: e2871004     	add	r1, r7, #4
   2d638: e2430fff     	sub	r0, r3, #1020
   2d63c: eb000561     	bl	0x2ebc8
   2d640: e1a01004     	mov	r1, r4
   2d644: e1a00006     	mov	r0, r6
   2d648: ebffa343     	bl	0x1635c    @ imm = #-0x172f4 ; _ZNSt16invalid_argumentC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   2d64c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d650: e24990f4     	sub	r9, r9, #244
   2d654: e1500009     	cmp	r0, r9
   2d658: 0a000000     	beq	0x2d660
   2d65c: ebffa1f7     	bl	0x15e40    @ imm = #-0x17824 ; _ZdlPv
   2d660: e3062290     	movw	r2, #0x6290
   2d664: e3402001     	movt	r2, #0x1
   2d668: e30e1e48     	movw	r1, #0xee48
   2d66c: e3401008     	movt	r1, #0x8
   2d670: e1a00006     	mov	r0, r6
   2d674: ebffa329     	bl	0x16320    @ imm = #-0x1735c ; __cxa_throw
   2d678: e1a09000     	mov	r9, r0
   2d67c: e1a06001     	mov	r6, r1
   2d680: e1a00005     	mov	r0, r5
   2d684: e1a04009     	mov	r4, r9
   2d688: e1a05006     	mov	r5, r6
   2d68c: ebffa0c5     	bl	0x159a8     @ imm = #-0x17cec ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2d690: eaffffbb     	b	0x2d584
   2d694: e59d30a4     	ldr	r3, [sp, #0xa4]
   2d698: e24920f4     	sub	r2, r9, #244
   2d69c: e1a04001     	mov	r4, r1
   2d6a0: e1a09000     	mov	r9, r0
   2d6a4: e1530002     	cmp	r3, r2
   2d6a8: 0a000001     	beq	0x2d6b4
   2d6ac: e1a00003     	mov	r0, r3
   2d6b0: ebffa1e2     	bl	0x15e40    @ imm = #-0x17878 ; _ZdlPv
   2d6b4: e1a00006     	mov	r0, r6
   2d6b8: e1a06004     	mov	r6, r4
   2d6bc: ebffa18b     	bl	0x15cf0    @ imm = #-0x179d4 ; __cxa_free_exception
   2d6c0: eaffffee     	b	0x2d680
   2d6c4: e1a09000     	mov	r9, r0
   2d6c8: e1a04001     	mov	r4, r1
   2d6cc: eafffff8     	b	0x2d6b4
   2d6d0: e3a00008     	mov	r0, #8
   2d6d4: e58d1010     	str	r1, [sp, #0x10]
   2d6d8: ebffa127     	bl	0x15b7c    @ imm = #-0x17b64 ; __cxa_allocate_exception
   2d6dc: e28d3e4a     	add	r3, sp, #1184
   2d6e0: e1a04000     	mov	r4, r0
   2d6e4: e30121d4     	movw	r2, #0x11d4
   2d6e8: e3402007     	movt	r2, #0x7
   2d6ec: e59d1010     	ldr	r1, [sp, #0x10]
   2d6f0: e2430e2e     	sub	r0, r3, #736
   2d6f4: eb000533     	bl	0x2ebc8
   2d6f8: e1a01005     	mov	r1, r5
   2d6fc: e1a00004     	mov	r0, r4
   2d700: ebffa2cd     	bl	0x1623c    @ imm = #-0x174cc ; _ZNSt13runtime_errorC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   2d704: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2d708: e24aa0d8     	sub	r10, r10, #216
   2d70c: e150000a     	cmp	r0, r10
   2d710: 0a000000     	beq	0x2d718
   2d714: ebffa1c9     	bl	0x15e40    @ imm = #-0x178dc ; _ZdlPv
   2d718: e3052ec4     	movw	r2, #0x5ec4
   2d71c: e3402001     	movt	r2, #0x1
   2d720: e30e1e5c     	movw	r1, #0xee5c
   2d724: e3401008     	movt	r1, #0x8
   2d728: e1a00004     	mov	r0, r4
   2d72c: ebffa2fb     	bl	0x16320    @ imm = #-0x17414 ; __cxa_throw
   2d730: eafffd11     	b	0x2cb7c
   2d734: eafffd22     	b	0x2cbc4
   2d738: e59d21c0     	ldr	r2, [sp, #0x1c0]
   2d73c: e24aa0d8     	sub	r10, r10, #216
   2d740: e1a06000     	mov	r6, r0
   2d744: e1a05001     	mov	r5, r1
   2d748: e152000a     	cmp	r2, r10
   2d74c: 0a000001     	beq	0x2d758
   2d750: e1a00002     	mov	r0, r2
   2d754: ebffa1b9     	bl	0x15e40    @ imm = #-0x1791c ; _ZdlPv
   2d758: e1a03006     	mov	r3, r6
   2d75c: e1a00004     	mov	r0, r4
   2d760: e1a04003     	mov	r4, r3
   2d764: ebffa161     	bl	0x15cf0    @ imm = #-0x17a7c ; __cxa_free_exception
   2d768: eaffffa5     	b	0x2d604
   2d76c: e1a03000     	mov	r3, r0
   2d770: e1a05001     	mov	r5, r1
   2d774: eafffff8     	b	0x2d75c
   2d778: eaffff79     	b	0x2d564
   2d77c: e1a04000     	mov	r4, r0
   2d780: e1a05001     	mov	r5, r1
   2d784: eaffff9e     	b	0x2d604
   2d788: e59d01c0     	ldr	r0, [sp, #0x1c0]
   2d78c: e24a30d8     	sub	r3, r10, #216
   2d790: e1500003     	cmp	r0, r3
   2d794: 1afffbd8     	bne	0x2c6fc
   2d798: eafffb78     	b	0x2c580
   2d79c: e59d30a4     	ldr	r3, [sp, #0xa4]
   2d7a0: e24990f4     	sub	r9, r9, #244
   2d7a4: e1a04000     	mov	r4, r0
   2d7a8: e1a05001     	mov	r5, r1
   2d7ac: e1530009     	cmp	r3, r9
   2d7b0: 1affff71     	bne	0x2d57c
   2d7b4: eaffff72     	b	0x2d584
   2d7b8: e1a04000     	mov	r4, r0
   2d7bc: e1a05001     	mov	r5, r1
   2d7c0: eaffff6f     	b	0x2d584
   2d7c4: e59d31c0     	ldr	r3, [sp, #0x1c0]
   2d7c8: e24aa0d8     	sub	r10, r10, #216
   2d7cc: e1a04000     	mov	r4, r0
   2d7d0: e1a05001     	mov	r5, r1
   2d7d4: e153000a     	cmp	r3, r10
   2d7d8: 0a000001     	beq	0x2d7e4
   2d7dc: e1a00003     	mov	r0, r3
   2d7e0: ebffa196     	bl	0x15e40    @ imm = #-0x179a8 ; _ZdlPv
   2d7e4: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d7e8: e24990f4     	sub	r9, r9, #244
   2d7ec: e1500009     	cmp	r0, r9
   2d7f0: 1affff67     	bne	0x2d594
   2d7f4: eaffff67     	b	0x2d598
   2d7f8: e1a04000     	mov	r4, r0
   2d7fc: e1a05001     	mov	r5, r1
   2d800: eafffff7     	b	0x2d7e4
   2d804: e1a05001     	mov	r5, r1
   2d808: eaffff63     	b	0x2d59c
   2d80c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d810: e24990f4     	sub	r9, r9, #244
   2d814: e1500009     	cmp	r0, r9
   2d818: 0a000000     	beq	0x2d820
   2d81c: ebffa187     	bl	0x15e40    @ imm = #-0x179e4 ; _ZdlPv
   2d820: e59d008c     	ldr	r0, [sp, #0x8c]
   2d824: e248300c     	sub	r3, r8, #12
   2d828: e1500003     	cmp	r0, r3
   2d82c: 0a000000     	beq	0x2d834
   2d830: ebffa182     	bl	0x15e40    @ imm = #-0x179f8 ; _ZdlPv
   2d834: e59d0074     	ldr	r0, [sp, #0x74]
   2d838: e2488024     	sub	r8, r8, #36
   2d83c: e1500008     	cmp	r0, r8
   2d840: 0afffb4f     	beq	0x2c584
   2d844: ebffa17d     	bl	0x15e40    @ imm = #-0x17a0c ; _ZdlPv
   2d848: eafffb4d     	b	0x2c584
   2d84c: eafffff3     	b	0x2d820
   2d850: e28d80a0     	add	r8, sp, #160
   2d854: eafffff6     	b	0x2d834
   2d858: e1a09000     	mov	r9, r0
   2d85c: e1a05001     	mov	r5, r1
   2d860: e1a00004     	mov	r0, r4
   2d864: e1a04009     	mov	r4, r9
   2d868: ebffa04e     	bl	0x159a8     @ imm = #-0x17ec8 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2d86c: eafffb25     	b	0x2c508
   2d870: e59d208c     	ldr	r2, [sp, #0x8c]
   2d874: e248300c     	sub	r3, r8, #12
   2d878: e1a04000     	mov	r4, r0
   2d87c: e1a05001     	mov	r5, r1
   2d880: e1520003     	cmp	r2, r3
   2d884: 0afffb1f     	beq	0x2c508
   2d888: e1a00002     	mov	r0, r2
   2d88c: ebffa16b     	bl	0x15e40    @ imm = #-0x17a54 ; _ZdlPv
   2d890: eafffb1c     	b	0x2c508
   2d894: e1a04000     	mov	r4, r0
   2d898: e1a05001     	mov	r5, r1
   2d89c: eafffb19     	b	0x2c508
   2d8a0: e59d30a4     	ldr	r3, [sp, #0xa4]
   2d8a4: e24990f4     	sub	r9, r9, #244
   2d8a8: e1a04000     	mov	r4, r0
   2d8ac: e1a05001     	mov	r5, r1
   2d8b0: e1530009     	cmp	r3, r9
   2d8b4: 0a000001     	beq	0x2d8c0
   2d8b8: e1a00003     	mov	r0, r3
   2d8bc: ebffa15f     	bl	0x15e40    @ imm = #-0x17a84 ; _ZdlPv
   2d8c0: e59d008c     	ldr	r0, [sp, #0x8c]
   2d8c4: e248800c     	sub	r8, r8, #12
   2d8c8: e1500008     	cmp	r0, r8
   2d8cc: 1afffb11     	bne	0x2c518
   2d8d0: eafffb11     	b	0x2c51c
   2d8d4: e1a04000     	mov	r4, r0
   2d8d8: e1a05001     	mov	r5, r1
   2d8dc: eafffff7     	b	0x2d8c0
   2d8e0: e1a05001     	mov	r5, r1
   2d8e4: eafffb0d     	b	0x2c520
   2d8e8: e1a04000     	mov	r4, r0
   2d8ec: e1a00005     	mov	r0, r5
   2d8f0: e1a05001     	mov	r5, r1
   2d8f4: ebffa02b     	bl	0x159a8     @ imm = #-0x17f54 ; _ZNSt13basic_fstreamIcSt11char_traitsIcEED1Ev
   2d8f8: eaffff41     	b	0x2d604
   2d8fc: e59d31c0     	ldr	r3, [sp, #0x1c0]
   2d900: e24aa0d8     	sub	r10, r10, #216
   2d904: e1a04000     	mov	r4, r0
   2d908: e1a05001     	mov	r5, r1
   2d90c: e153000a     	cmp	r3, r10
   2d910: 0affff0d     	beq	0x2d54c
   2d914: e1a00003     	mov	r0, r3
   2d918: ebffa148     	bl	0x15e40    @ imm = #-0x17ae0 ; _ZdlPv
   2d91c: eaffff0a     	b	0x2d54c
   2d920: e59d30a4     	ldr	r3, [sp, #0xa4]
   2d924: e24990f4     	sub	r9, r9, #244
   2d928: e1a04000     	mov	r4, r0
   2d92c: e1a05001     	mov	r5, r1
   2d930: e1530009     	cmp	r3, r9
   2d934: 0a000001     	beq	0x2d940
   2d938: e1a00003     	mov	r0, r3
   2d93c: ebffa13f     	bl	0x15e40    @ imm = #-0x17b04 ; _ZdlPv
   2d940: e59d008c     	ldr	r0, [sp, #0x8c]
   2d944: e248300c     	sub	r3, r8, #12
   2d948: e1500003     	cmp	r0, r3
   2d94c: 0afffae8     	beq	0x2c4f4
   2d950: ebffa13a     	bl	0x15e40    @ imm = #-0x17b18 ; _ZdlPv
   2d954: eafffae6     	b	0x2c4f4
   2d958: e1a04000     	mov	r4, r0
   2d95c: e1a05001     	mov	r5, r1
   2d960: eafffff6     	b	0x2d940
   2d964: e3a00008     	mov	r0, #8
   2d968: ebffa083     	bl	0x15b7c    @ imm = #-0x17df4 ; __cxa_allocate_exception
   2d96c: e28d3e4a     	add	r3, sp, #1184
   2d970: e1a05000     	mov	r5, r0
   2d974: e3012234     	movw	r2, #0x1234
   2d978: e3402007     	movt	r2, #0x7
   2d97c: e2871004     	add	r1, r7, #4
   2d980: e2430fff     	sub	r0, r3, #1020
   2d984: eb00048f     	bl	0x2ebc8
   2d988: e1a01004     	mov	r1, r4
   2d98c: e1a00005     	mov	r0, r5
   2d990: ebffa229     	bl	0x1623c    @ imm = #-0x1775c ; _ZNSt13runtime_errorC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   2d994: e59d00a4     	ldr	r0, [sp, #0xa4]
   2d998: e24990f4     	sub	r9, r9, #244
   2d99c: e1500009     	cmp	r0, r9
   2d9a0: 0a000000     	beq	0x2d9a8
   2d9a4: ebffa125     	bl	0x15e40    @ imm = #-0x17b6c ; _ZdlPv
   2d9a8: e3052ec4     	movw	r2, #0x5ec4
   2d9ac: e3402001     	movt	r2, #0x1
   2d9b0: e30e1e5c     	movw	r1, #0xee5c
   2d9b4: e3401008     	movt	r1, #0x8
   2d9b8: e1a00005     	mov	r0, r5
   2d9bc: ebffa257     	bl	0x16320    @ imm = #-0x176a4 ; __cxa_throw
   2d9c0: e1a03000     	mov	r3, r0
   2d9c4: e1a00004     	mov	r0, r4
   2d9c8: e1a05001     	mov	r5, r1
   2d9cc: e1a04003     	mov	r4, r3
   2d9d0: eb0003b7     	bl	0x2e8b4
   2d9d4: eafffac6     	b	0x2c4f4
   2d9d8: e59d30a4     	ldr	r3, [sp, #0xa4]
   2d9dc: e24990f4     	sub	r9, r9, #244
   2d9e0: e1a04000     	mov	r4, r0
   2d9e4: e1a06001     	mov	r6, r1
   2d9e8: e1530009     	cmp	r3, r9
   2d9ec: 0a000001     	beq	0x2d9f8
   2d9f0: e1a00003     	mov	r0, r3
   2d9f4: ebffa111     	bl	0x15e40    @ imm = #-0x17bbc ; _ZdlPv
   2d9f8: e1a00005     	mov	r0, r5
   2d9fc: e1a05006     	mov	r5, r6
   2da00: ebffa0ba     	bl	0x15cf0    @ imm = #-0x17d18 ; __cxa_free_exception
   2da04: eafffaba     	b	0x2c4f4
   2da08: e1a04000     	mov	r4, r0
   2da0c: e1a06001     	mov	r6, r1
   2da10: eafffff8     	b	0x2d9f8
   2da14: e1a04000     	mov	r4, r0
   2da18: e1a05001     	mov	r5, r1
   2da1c: eafffab4     	b	0x2c4f4
   2da20: e59d30a4     	ldr	r3, [sp, #0xa4]
   2da24: e24990f4     	sub	r9, r9, #244
   2da28: e1a04000     	mov	r4, r0
   2da2c: e1a05001     	mov	r5, r1
   2da30: e1530009     	cmp	r3, r9
   2da34: 0afffab3     	beq	0x2c508
   2da38: e1a00003     	mov	r0, r3
   2da3c: ebffa0ff     	bl	0x15e40    @ imm = #-0x17c04 ; _ZdlPv
   2da40: eafffab0     	b	0x2c508
   2da44: eafffff5     	b	0x2da20
   2da48: e3a00008     	mov	r0, #8
   2da4c: ebffa04a     	bl	0x15b7c    @ imm = #-0x17ed8 ; __cxa_allocate_exception
   2da50: e28d3e4a     	add	r3, sp, #1184
   2da54: e1a05000     	mov	r5, r0
   2da58: e2430e41     	sub	r0, r3, #1040
   2da5c: e3012214     	movw	r2, #0x1214
   2da60: e3402007     	movt	r2, #0x7
   2da64: e2400004     	sub	r0, r0, #4
   2da68: e2871004     	add	r1, r7, #4
   2da6c: eb000455     	bl	0x2ebc8
   2da70: e1a01006     	mov	r1, r6
   2da74: e1a00005     	mov	r0, r5
   2da78: ebffa237     	bl	0x1635c    @ imm = #-0x17724 ; _ZNSt16invalid_argumentC1ERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
   2da7c: e59d008c     	ldr	r0, [sp, #0x8c]
   2da80: e248300c     	sub	r3, r8, #12
   2da84: e1500003     	cmp	r0, r3
   2da88: 0a000000     	beq	0x2da90
   2da8c: ebffa0eb     	bl	0x15e40    @ imm = #-0x17c54 ; _ZdlPv
   2da90: e3062290     	movw	r2, #0x6290
   2da94: e3402001     	movt	r2, #0x1
   2da98: e30e1e48     	movw	r1, #0xee48
   2da9c: e3401008     	movt	r1, #0x8
   2daa0: e1a00005     	mov	r0, r5
   2daa4: ebffa21d     	bl	0x16320    @ imm = #-0x1778c ; __cxa_throw
   2daa8: e59d208c     	ldr	r2, [sp, #0x8c]
   2daac: e248300c     	sub	r3, r8, #12
   2dab0: e1a09000     	mov	r9, r0
   2dab4: e1a06001     	mov	r6, r1
   2dab8: e1520003     	cmp	r2, r3
   2dabc: 0a000001     	beq	0x2dac8
   2dac0: e1a00002     	mov	r0, r2
   2dac4: ebffa0dd     	bl	0x15e40    @ imm = #-0x17c8c ; _ZdlPv
   2dac8: e1a00005     	mov	r0, r5
   2dacc: e1a05006     	mov	r5, r6
   2dad0: ebffa086     	bl	0x15cf0    @ imm = #-0x17de8 ; __cxa_free_exception
   2dad4: eaffff61     	b	0x2d860
   2dad8: e1a09000     	mov	r9, r0
   2dadc: e1a06001     	mov	r6, r1
   2dae0: eafffff8     	b	0x2dac8
   2dae4: e3a00008     	mov	r0, #8
   2dae8: ebffa023     	bl	0x15b7c    @ imm = #-0x17f74 ; __cxa_allocate_exception
   2daec: e3011474     	movw	r1, #0x1474
   2daf0: e3401007     	movt	r1, #0x7
   2daf4: e1a05000     	mov	r5, r0
   2daf8: ebffa12d     	bl	0x15fb4    @ imm = #-0x17b4c ; _ZNSt11logic_errorC2EPKc
   2dafc: e59f3a78     	ldr	r3, [pc, #0xa78]        @ 0x2e57c
   2db00: e30e2830     	movw	r2, #0xe830
   2db04: e3402002     	movt	r2, #0x2
   2db08: e3011e98     	movw	r1, #0x1e98
   2db0c: e3401007     	movt	r1, #0x7
   2db10: e1a00005     	mov	r0, r5
   2db14: e5853000     	str	r3, [r5]
   2db18: ebffa200     	bl	0x16320    @ imm = #-0x17800 ; __cxa_throw
   2db1c: e1a0b000     	mov	r11, r0
   2db20: e1a05001     	mov	r5, r1
   2db24: ea000003     	b	0x2db38
   2db28: e1a0b000     	mov	r11, r0
   2db2c: e1a00005     	mov	r0, r5
   2db30: e1a05001     	mov	r5, r1
   2db34: ebffa06d     	bl	0x15cf0    @ imm = #-0x17e4c ; __cxa_free_exception
   2db38: e1a00004     	mov	r0, r4
   2db3c: eb00cdb4     	bl	0x61214
   2db40: e1a0000b     	mov	r0, r11
   2db44: e3550002     	cmp	r5, #2
   2db48: 11a0b000     	movne	r11, r0
   2db4c: 1afffa38     	bne	0x2c434
   2db50: ebff9feb     	bl	0x15b04    @ imm = #-0x18054 ; __cxa_begin_catch
   2db54: e3011760     	movw	r1, #0x1760
   2db58: e3401007     	movt	r1, #0x7
   2db5c: e1a05000     	mov	r5, r0
   2db60: e3a0201c     	mov	r2, #28
   2db64: e30f0764     	movw	r0, #0xf764
   2db68: e3400008     	movt	r0, #0x8
   2db6c: ebffa13d     	bl	0x16068    @ imm = #-0x17b0c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2db70: e5953000     	ldr	r3, [r5]
   2db74: e1a00005     	mov	r0, r5
   2db78: e5933008     	ldr	r3, [r3, #0x8]
   2db7c: e12fff33     	blx	r3
   2db80: e1a01000     	mov	r1, r0
   2db84: e30f0764     	movw	r0, #0xf764
   2db88: e3400008     	movt	r0, #0x8
   2db8c: ebffa0c0     	bl	0x15e94    @ imm = #-0x17d00 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2db90: e3a02001     	mov	r2, #1
   2db94: e1a01004     	mov	r1, r4
   2db98: e3a0300a     	mov	r3, #10
   2db9c: e5cd30a4     	strb	r3, [sp, #0xa4]
   2dba0: ebffa130     	bl	0x16068    @ imm = #-0x17b40 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dba4: e59f19d4     	ldr	r1, [pc, #0x9d4]        @ 0x2e580
   2dba8: e3a02e1e     	mov	r2, #480
   2dbac: e28d0f91     	add	r0, sp, #580
   2dbb0: ebffa123     	bl	0x16044    @ imm = #-0x17b74 ; memcpy
   2dbb4: ebffa1ac     	bl	0x1626c    @ imm = #-0x17950 ; __cxa_end_catch
   2dbb8: eafff835     	b	0x2bc94
   2dbbc: e1a05001     	mov	r5, r1
   2dbc0: eaffffdf     	b	0x2db44
   2dbc4: e1a0b000     	mov	r11, r0
   2dbc8: e1a05001     	mov	r5, r1
   2dbcc: e3510002     	cmp	r1, #2
   2dbd0: 1afffa17     	bne	0x2c434
   2dbd4: ebff9fca     	bl	0x15b04    @ imm = #-0x180d8 ; __cxa_begin_catch
   2dbd8: e3011740     	movw	r1, #0x1740
   2dbdc: e3401007     	movt	r1, #0x7
   2dbe0: e1a05000     	mov	r5, r0
   2dbe4: e3a0201c     	mov	r2, #28
   2dbe8: e30f0764     	movw	r0, #0xf764
   2dbec: e3400008     	movt	r0, #0x8
   2dbf0: ebffa11c     	bl	0x16068    @ imm = #-0x17b90 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dbf4: e5953000     	ldr	r3, [r5]
   2dbf8: e1a00005     	mov	r0, r5
   2dbfc: e5933008     	ldr	r3, [r3, #0x8]
   2dc00: e12fff33     	blx	r3
   2dc04: e1a01000     	mov	r1, r0
   2dc08: e30f0764     	movw	r0, #0xf764
   2dc0c: e3400008     	movt	r0, #0x8
   2dc10: ebffa09f     	bl	0x15e94    @ imm = #-0x17d84 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2dc14: e3a02001     	mov	r2, #1
   2dc18: e1a01004     	mov	r1, r4
   2dc1c: e3a0300a     	mov	r3, #10
   2dc20: e5cd30a4     	strb	r3, [sp, #0xa4]
   2dc24: ebffa10f     	bl	0x16068    @ imm = #-0x17bc4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dc28: e30f3cf8     	movw	r3, #0xfcf8
   2dc2c: e3403008     	movt	r3, #0x8
   2dc30: e5933040     	ldr	r3, [r3, #0x40]
   2dc34: e58d3240     	str	r3, [sp, #0x240]
   2dc38: ebffa18b     	bl	0x1626c    @ imm = #-0x179d4 ; __cxa_end_catch
   2dc3c: eafff7f4     	b	0x2bc14
   2dc40: eafff9f8     	b	0x2c428
   2dc44: eafff9f7     	b	0x2c428
   2dc48: e1a0b000     	mov	r11, r0
   2dc4c: e1a05001     	mov	r5, r1
   2dc50: e3510002     	cmp	r1, #2
   2dc54: 1afff9f6     	bne	0x2c434
   2dc58: ebff9fa9     	bl	0x15b04    @ imm = #-0x1815c ; __cxa_begin_catch
   2dc5c: e301171c     	movw	r1, #0x171c
   2dc60: e3401007     	movt	r1, #0x7
   2dc64: e1a05000     	mov	r5, r0
   2dc68: e3a02023     	mov	r2, #35
   2dc6c: e30f0764     	movw	r0, #0xf764
   2dc70: e3400008     	movt	r0, #0x8
   2dc74: ebffa0fb     	bl	0x16068    @ imm = #-0x17c14 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dc78: e5953000     	ldr	r3, [r5]
   2dc7c: e1a00005     	mov	r0, r5
   2dc80: e5933008     	ldr	r3, [r3, #0x8]
   2dc84: e12fff33     	blx	r3
   2dc88: e1a01000     	mov	r1, r0
   2dc8c: e30f0764     	movw	r0, #0xf764
   2dc90: e3400008     	movt	r0, #0x8
   2dc94: ebffa07e     	bl	0x15e94    @ imm = #-0x17e08 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2dc98: e3a02001     	mov	r2, #1
   2dc9c: e1a01004     	mov	r1, r4
   2dca0: e3a0300a     	mov	r3, #10
   2dca4: e5cd30a4     	strb	r3, [sp, #0xa4]
   2dca8: ebffa0ee     	bl	0x16068    @ imm = #-0x17c48 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dcac: e30f3cf8     	movw	r3, #0xfcf8
   2dcb0: e3403008     	movt	r3, #0x8
   2dcb4: e593303c     	ldr	r3, [r3, #0x3c]
   2dcb8: e58d323c     	str	r3, [sp, #0x23c]
   2dcbc: ebffa16a     	bl	0x1626c    @ imm = #-0x17a58 ; __cxa_end_catch
   2dcc0: eafff7cd     	b	0x2bbfc
   2dcc4: e1a0b000     	mov	r11, r0
   2dcc8: e1a05001     	mov	r5, r1
   2dccc: e3510002     	cmp	r1, #2
   2dcd0: 1afff9d7     	bne	0x2c434
   2dcd4: ebff9f8a     	bl	0x15b04    @ imm = #-0x181d8 ; __cxa_begin_catch
   2dcd8: e3011700     	movw	r1, #0x1700
   2dcdc: e3401007     	movt	r1, #0x7
   2dce0: e1a05000     	mov	r5, r0
   2dce4: e3a0201b     	mov	r2, #27
   2dce8: e30f0764     	movw	r0, #0xf764
   2dcec: e3400008     	movt	r0, #0x8
   2dcf0: ebffa0dc     	bl	0x16068    @ imm = #-0x17c90 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dcf4: e5953000     	ldr	r3, [r5]
   2dcf8: e1a00005     	mov	r0, r5
   2dcfc: e5933008     	ldr	r3, [r3, #0x8]
   2dd00: e12fff33     	blx	r3
   2dd04: e1a01000     	mov	r1, r0
   2dd08: e30f0764     	movw	r0, #0xf764
   2dd0c: e3400008     	movt	r0, #0x8
   2dd10: ebffa05f     	bl	0x15e94    @ imm = #-0x17e84 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2dd14: e3a02001     	mov	r2, #1
   2dd18: e1a01004     	mov	r1, r4
   2dd1c: e3a0300a     	mov	r3, #10
   2dd20: e5cd30a4     	strb	r3, [sp, #0xa4]
   2dd24: ebffa0cf     	bl	0x16068    @ imm = #-0x17cc4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dd28: e59f3854     	ldr	r3, [pc, #0x854]        @ 0x2e584
   2dd2c: e28d2b11     	add	r2, sp, #17408
   2dd30: e282206c     	add	r2, r2, #108
   2dd34: e593326c     	ldr	r3, [r3, #0x26c]
   2dd38: e5823000     	str	r3, [r2]
   2dd3c: ebffa14a     	bl	0x1626c    @ imm = #-0x17ad8 ; __cxa_end_catch
   2dd40: eafff7a7     	b	0x2bbe4
   2dd44: eafff9b7     	b	0x2c428
   2dd48: eafff9b6     	b	0x2c428
   2dd4c: e1a0b000     	mov	r11, r0
   2dd50: e1a05001     	mov	r5, r1
   2dd54: e3510002     	cmp	r1, #2
   2dd58: 1afff9b5     	bne	0x2c434
   2dd5c: ebff9f68     	bl	0x15b04    @ imm = #-0x18260 ; __cxa_begin_catch
   2dd60: e30116e4     	movw	r1, #0x16e4
   2dd64: e3401007     	movt	r1, #0x7
   2dd68: e1a05000     	mov	r5, r0
   2dd6c: e3a0201b     	mov	r2, #27
   2dd70: e30f0764     	movw	r0, #0xf764
   2dd74: e3400008     	movt	r0, #0x8
   2dd78: ebffa0ba     	bl	0x16068    @ imm = #-0x17d18 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dd7c: e5953000     	ldr	r3, [r5]
   2dd80: e1a00005     	mov	r0, r5
   2dd84: e5933008     	ldr	r3, [r3, #0x8]
   2dd88: e12fff33     	blx	r3
   2dd8c: e1a01000     	mov	r1, r0
   2dd90: e30f0764     	movw	r0, #0xf764
   2dd94: e3400008     	movt	r0, #0x8
   2dd98: ebffa03d     	bl	0x15e94    @ imm = #-0x17f0c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2dd9c: e3a02001     	mov	r2, #1
   2dda0: e1a01004     	mov	r1, r4
   2dda4: e3a0300a     	mov	r3, #10
   2dda8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2ddac: ebffa0ad     	bl	0x16068    @ imm = #-0x17d4c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2ddb0: e30f3cf8     	movw	r3, #0xfcf8
   2ddb4: e3403008     	movt	r3, #0x8
   2ddb8: e5933038     	ldr	r3, [r3, #0x38]
   2ddbc: e58d3238     	str	r3, [sp, #0x238]
   2ddc0: ebffa129     	bl	0x1626c    @ imm = #-0x17b5c ; __cxa_end_catch
   2ddc4: eafff77d     	b	0x2bbc0
   2ddc8: e1a0b000     	mov	r11, r0
   2ddcc: e1a05001     	mov	r5, r1
   2ddd0: e3510002     	cmp	r1, #2
   2ddd4: 1afff996     	bne	0x2c434
   2ddd8: ebff9f49     	bl	0x15b04    @ imm = #-0x182dc ; __cxa_begin_catch
   2dddc: e30116c0     	movw	r1, #0x16c0
   2dde0: e3401007     	movt	r1, #0x7
   2dde4: e1a05000     	mov	r5, r0
   2dde8: e3a02021     	mov	r2, #33
   2ddec: e30f0764     	movw	r0, #0xf764
   2ddf0: e3400008     	movt	r0, #0x8
   2ddf4: ebffa09b     	bl	0x16068    @ imm = #-0x17d94 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2ddf8: e5953000     	ldr	r3, [r5]
   2ddfc: e1a00005     	mov	r0, r5
   2de00: e5933008     	ldr	r3, [r3, #0x8]
   2de04: e12fff33     	blx	r3
   2de08: e1a01000     	mov	r1, r0
   2de0c: e30f0764     	movw	r0, #0xf764
   2de10: e3400008     	movt	r0, #0x8
   2de14: ebffa01e     	bl	0x15e94    @ imm = #-0x17f88 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2de18: e3a02001     	mov	r2, #1
   2de1c: e1a01004     	mov	r1, r4
   2de20: e3a0300a     	mov	r3, #10
   2de24: e5cd30a4     	strb	r3, [sp, #0xa4]
   2de28: ebffa08e     	bl	0x16068    @ imm = #-0x17dc8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2de2c: e30f3cf8     	movw	r3, #0xfcf8
   2de30: e3403008     	movt	r3, #0x8
   2de34: e5933034     	ldr	r3, [r3, #0x34]
   2de38: e58d3234     	str	r3, [sp, #0x234]
   2de3c: ebffa10a     	bl	0x1626c    @ imm = #-0x17bd8 ; __cxa_end_catch
   2de40: eafff758     	b	0x2bba8
   2de44: eafff977     	b	0x2c428
   2de48: eafff976     	b	0x2c428
   2de4c: e1a0b000     	mov	r11, r0
   2de50: e1a05001     	mov	r5, r1
   2de54: e3510002     	cmp	r1, #2
   2de58: 1afff975     	bne	0x2c434
   2de5c: ebff9f28     	bl	0x15b04    @ imm = #-0x18360 ; __cxa_begin_catch
   2de60: e30116a4     	movw	r1, #0x16a4
   2de64: e3401007     	movt	r1, #0x7
   2de68: e1a05000     	mov	r5, r0
   2de6c: e3a02019     	mov	r2, #25
   2de70: e30f0764     	movw	r0, #0xf764
   2de74: e3400008     	movt	r0, #0x8
   2de78: ebffa07a     	bl	0x16068    @ imm = #-0x17e18 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2de7c: e5953000     	ldr	r3, [r5]
   2de80: e1a00005     	mov	r0, r5
   2de84: e5933008     	ldr	r3, [r3, #0x8]
   2de88: e12fff33     	blx	r3
   2de8c: e1a01000     	mov	r1, r0
   2de90: e30f0764     	movw	r0, #0xf764
   2de94: e3400008     	movt	r0, #0x8
   2de98: ebff9ffd     	bl	0x15e94    @ imm = #-0x1800c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2de9c: e3a02001     	mov	r2, #1
   2dea0: e1a01004     	mov	r1, r4
   2dea4: e3a0300a     	mov	r3, #10
   2dea8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2deac: ebffa06d     	bl	0x16068    @ imm = #-0x17e4c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2deb0: e30f3cf8     	movw	r3, #0xfcf8
   2deb4: e3403008     	movt	r3, #0x8
   2deb8: e5933030     	ldr	r3, [r3, #0x30]
   2debc: e58d3230     	str	r3, [sp, #0x230]
   2dec0: ebffa0e9     	bl	0x1626c    @ imm = #-0x17c5c ; __cxa_end_catch
   2dec4: eafff72d     	b	0x2bb80
   2dec8: e1a0b000     	mov	r11, r0
   2decc: e1a05001     	mov	r5, r1
   2ded0: e3510002     	cmp	r1, #2
   2ded4: 1afff956     	bne	0x2c434
   2ded8: ebff9f09     	bl	0x15b04    @ imm = #-0x183dc ; __cxa_begin_catch
   2dedc: e301168c     	movw	r1, #0x168c
   2dee0: e3401007     	movt	r1, #0x7
   2dee4: e1a05000     	mov	r5, r0
   2dee8: e3a02017     	mov	r2, #23
   2deec: e30f0764     	movw	r0, #0xf764
   2def0: e3400008     	movt	r0, #0x8
   2def4: ebffa05b     	bl	0x16068    @ imm = #-0x17e94 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2def8: e5953000     	ldr	r3, [r5]
   2defc: e1a00005     	mov	r0, r5
   2df00: e5933008     	ldr	r3, [r3, #0x8]
   2df04: e12fff33     	blx	r3
   2df08: e1a01000     	mov	r1, r0
   2df0c: e30f0764     	movw	r0, #0xf764
   2df10: e3400008     	movt	r0, #0x8
   2df14: ebff9fde     	bl	0x15e94    @ imm = #-0x18088 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2df18: e3a02001     	mov	r2, #1
   2df1c: e1a01004     	mov	r1, r4
   2df20: e3a0300a     	mov	r3, #10
   2df24: e5cd30a4     	strb	r3, [sp, #0xa4]
   2df28: ebffa04e     	bl	0x16068    @ imm = #-0x17ec8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2df2c: e30f3cf8     	movw	r3, #0xfcf8
   2df30: e3403008     	movt	r3, #0x8
   2df34: e593302c     	ldr	r3, [r3, #0x2c]
   2df38: e58d322c     	str	r3, [sp, #0x22c]
   2df3c: ebffa0ca     	bl	0x1626c    @ imm = #-0x17cd8 ; __cxa_end_catch
   2df40: eafff708     	b	0x2bb68
   2df44: eafff937     	b	0x2c428
   2df48: eafff936     	b	0x2c428
   2df4c: e1a0b000     	mov	r11, r0
   2df50: e1a05001     	mov	r5, r1
   2df54: e3510002     	cmp	r1, #2
   2df58: 1afff935     	bne	0x2c434
   2df5c: ebff9ee8     	bl	0x15b04    @ imm = #-0x18460 ; __cxa_begin_catch
   2df60: e3011670     	movw	r1, #0x1670
   2df64: e3401007     	movt	r1, #0x7
   2df68: e1a05000     	mov	r5, r0
   2df6c: e3a0201b     	mov	r2, #27
   2df70: e30f0764     	movw	r0, #0xf764
   2df74: e3400008     	movt	r0, #0x8
   2df78: ebffa03a     	bl	0x16068    @ imm = #-0x17f18 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2df7c: e5953000     	ldr	r3, [r5]
   2df80: e1a00005     	mov	r0, r5
   2df84: e5933008     	ldr	r3, [r3, #0x8]
   2df88: e12fff33     	blx	r3
   2df8c: e1a01000     	mov	r1, r0
   2df90: e30f0764     	movw	r0, #0xf764
   2df94: e3400008     	movt	r0, #0x8
   2df98: ebff9fbd     	bl	0x15e94    @ imm = #-0x1810c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2df9c: e3a02001     	mov	r2, #1
   2dfa0: e1a01004     	mov	r1, r4
   2dfa4: e3a0300a     	mov	r3, #10
   2dfa8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2dfac: ebffa02d     	bl	0x16068    @ imm = #-0x17f4c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dfb0: e30f3cf8     	movw	r3, #0xfcf8
   2dfb4: e3403008     	movt	r3, #0x8
   2dfb8: e5933028     	ldr	r3, [r3, #0x28]
   2dfbc: e58d3228     	str	r3, [sp, #0x228]
   2dfc0: ebffa0a9     	bl	0x1626c    @ imm = #-0x17d5c ; __cxa_end_catch
   2dfc4: eafff6e1     	b	0x2bb50
   2dfc8: e1a0b000     	mov	r11, r0
   2dfcc: e1a05001     	mov	r5, r1
   2dfd0: e3510002     	cmp	r1, #2
   2dfd4: 1afff916     	bne	0x2c434
   2dfd8: ebff9ec9     	bl	0x15b04    @ imm = #-0x184dc ; __cxa_begin_catch
   2dfdc: e3011648     	movw	r1, #0x1648
   2dfe0: e3401007     	movt	r1, #0x7
   2dfe4: e1a05000     	mov	r5, r0
   2dfe8: e3a02025     	mov	r2, #37
   2dfec: e30f0764     	movw	r0, #0xf764
   2dff0: e3400008     	movt	r0, #0x8
   2dff4: ebffa01b     	bl	0x16068    @ imm = #-0x17f94 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2dff8: e5953000     	ldr	r3, [r5]
   2dffc: e1a00005     	mov	r0, r5
   2e000: e5933008     	ldr	r3, [r3, #0x8]
   2e004: e12fff33     	blx	r3
   2e008: e1a01000     	mov	r1, r0
   2e00c: e30f0764     	movw	r0, #0xf764
   2e010: e3400008     	movt	r0, #0x8
   2e014: ebff9f9e     	bl	0x15e94    @ imm = #-0x18188 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e018: e3a02001     	mov	r2, #1
   2e01c: e1a01004     	mov	r1, r4
   2e020: e3a0300a     	mov	r3, #10
   2e024: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e028: ebffa00e     	bl	0x16068    @ imm = #-0x17fc8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e02c: e30f3cf8     	movw	r3, #0xfcf8
   2e030: e3403008     	movt	r3, #0x8
   2e034: e5933024     	ldr	r3, [r3, #0x24]
   2e038: e58d3224     	str	r3, [sp, #0x224]
   2e03c: ebffa08a     	bl	0x1626c    @ imm = #-0x17dd8 ; __cxa_end_catch
   2e040: eafff6bc     	b	0x2bb38
   2e044: eafff8f7     	b	0x2c428
   2e048: eafff8f6     	b	0x2c428
   2e04c: e1a0b000     	mov	r11, r0
   2e050: e1a05001     	mov	r5, r1
   2e054: e3510002     	cmp	r1, #2
   2e058: 1afff8f5     	bne	0x2c434
   2e05c: ebff9ea8     	bl	0x15b04    @ imm = #-0x18560 ; __cxa_begin_catch
   2e060: e301161c     	movw	r1, #0x161c
   2e064: e3401007     	movt	r1, #0x7
   2e068: e1a05000     	mov	r5, r0
   2e06c: e3a02028     	mov	r2, #40
   2e070: e30f0764     	movw	r0, #0xf764
   2e074: e3400008     	movt	r0, #0x8
   2e078: ebff9ffa     	bl	0x16068    @ imm = #-0x18018 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e07c: e5953000     	ldr	r3, [r5]
   2e080: e1a00005     	mov	r0, r5
   2e084: e5933008     	ldr	r3, [r3, #0x8]
   2e088: e12fff33     	blx	r3
   2e08c: e1a01000     	mov	r1, r0
   2e090: e30f0764     	movw	r0, #0xf764
   2e094: e3400008     	movt	r0, #0x8
   2e098: ebff9f7d     	bl	0x15e94    @ imm = #-0x1820c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e09c: e3a02001     	mov	r2, #1
   2e0a0: e1a01004     	mov	r1, r4
   2e0a4: e3a0300a     	mov	r3, #10
   2e0a8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e0ac: ebff9fed     	bl	0x16068    @ imm = #-0x1804c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e0b0: e30f3cf8     	movw	r3, #0xfcf8
   2e0b4: e3403008     	movt	r3, #0x8
   2e0b8: e5933020     	ldr	r3, [r3, #0x20]
   2e0bc: e58d3220     	str	r3, [sp, #0x220]
   2e0c0: ebffa069     	bl	0x1626c    @ imm = #-0x17e5c ; __cxa_end_catch
   2e0c4: eafff692     	b	0x2bb14
   2e0c8: e1a0b000     	mov	r11, r0
   2e0cc: e1a05001     	mov	r5, r1
   2e0d0: e3510002     	cmp	r1, #2
   2e0d4: 1afff8d6     	bne	0x2c434
   2e0d8: ebff9e89     	bl	0x15b04    @ imm = #-0x185dc ; __cxa_begin_catch
   2e0dc: e30115f4     	movw	r1, #0x15f4
   2e0e0: e3401007     	movt	r1, #0x7
   2e0e4: e1a05000     	mov	r5, r0
   2e0e8: e3a02025     	mov	r2, #37
   2e0ec: e30f0764     	movw	r0, #0xf764
   2e0f0: e3400008     	movt	r0, #0x8
   2e0f4: ebff9fdb     	bl	0x16068    @ imm = #-0x18094 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e0f8: e5953000     	ldr	r3, [r5]
   2e0fc: e1a00005     	mov	r0, r5
   2e100: e5933008     	ldr	r3, [r3, #0x8]
   2e104: e12fff33     	blx	r3
   2e108: e1a01000     	mov	r1, r0
   2e10c: e30f0764     	movw	r0, #0xf764
   2e110: e3400008     	movt	r0, #0x8
   2e114: ebff9f5e     	bl	0x15e94    @ imm = #-0x18288 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e118: e3a02001     	mov	r2, #1
   2e11c: e1a01004     	mov	r1, r4
   2e120: e3a0300a     	mov	r3, #10
   2e124: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e128: ebff9fce     	bl	0x16068    @ imm = #-0x180c8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e12c: e30f3cf8     	movw	r3, #0xfcf8
   2e130: e3403008     	movt	r3, #0x8
   2e134: e593301c     	ldr	r3, [r3, #0x1c]
   2e138: e58d321c     	str	r3, [sp, #0x21c]
   2e13c: ebffa04a     	bl	0x1626c    @ imm = #-0x17ed8 ; __cxa_end_catch
   2e140: eafff66a     	b	0x2baf0
   2e144: eafff8b7     	b	0x2c428
   2e148: eafff8b6     	b	0x2c428
   2e14c: e1a0b000     	mov	r11, r0
   2e150: e1a05001     	mov	r5, r1
   2e154: e3510002     	cmp	r1, #2
   2e158: 1afff8b5     	bne	0x2c434
   2e15c: ebff9e68     	bl	0x15b04    @ imm = #-0x18660 ; __cxa_begin_catch
   2e160: e30115d8     	movw	r1, #0x15d8
   2e164: e3401007     	movt	r1, #0x7
   2e168: e1a05000     	mov	r5, r0
   2e16c: e3a0201a     	mov	r2, #26
   2e170: e30f0764     	movw	r0, #0xf764
   2e174: e3400008     	movt	r0, #0x8
   2e178: ebff9fba     	bl	0x16068    @ imm = #-0x18118 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e17c: e5953000     	ldr	r3, [r5]
   2e180: e1a00005     	mov	r0, r5
   2e184: e5933008     	ldr	r3, [r3, #0x8]
   2e188: e12fff33     	blx	r3
   2e18c: e1a01000     	mov	r1, r0
   2e190: e30f0764     	movw	r0, #0xf764
   2e194: e3400008     	movt	r0, #0x8
   2e198: ebff9f3d     	bl	0x15e94    @ imm = #-0x1830c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e19c: e3a02001     	mov	r2, #1
   2e1a0: e1a01004     	mov	r1, r4
   2e1a4: e3a0300a     	mov	r3, #10
   2e1a8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e1ac: ebff9fad     	bl	0x16068    @ imm = #-0x1814c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e1b0: e30f3cf8     	movw	r3, #0xfcf8
   2e1b4: e3403008     	movt	r3, #0x8
   2e1b8: e5933018     	ldr	r3, [r3, #0x18]
   2e1bc: e58d3218     	str	r3, [sp, #0x218]
   2e1c0: ebffa029     	bl	0x1626c    @ imm = #-0x17f5c ; __cxa_end_catch
   2e1c4: eafff640     	b	0x2bacc
   2e1c8: e1a0b000     	mov	r11, r0
   2e1cc: e1a05001     	mov	r5, r1
   2e1d0: e3510002     	cmp	r1, #2
   2e1d4: 1afff896     	bne	0x2c434
   2e1d8: ebff9e49     	bl	0x15b04    @ imm = #-0x186dc ; __cxa_begin_catch
   2e1dc: e30115b8     	movw	r1, #0x15b8
   2e1e0: e3401007     	movt	r1, #0x7
   2e1e4: e1a05000     	mov	r5, r0
   2e1e8: e3a0201d     	mov	r2, #29
   2e1ec: e30f0764     	movw	r0, #0xf764
   2e1f0: e3400008     	movt	r0, #0x8
   2e1f4: ebff9f9b     	bl	0x16068    @ imm = #-0x18194 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e1f8: e5953000     	ldr	r3, [r5]
   2e1fc: e1a00005     	mov	r0, r5
   2e200: e5933008     	ldr	r3, [r3, #0x8]
   2e204: e12fff33     	blx	r3
   2e208: e1a01000     	mov	r1, r0
   2e20c: e30f0764     	movw	r0, #0xf764
   2e210: e3400008     	movt	r0, #0x8
   2e214: ebff9f1e     	bl	0x15e94    @ imm = #-0x18388 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e218: e3a02001     	mov	r2, #1
   2e21c: e1a01004     	mov	r1, r4
   2e220: e3a0300a     	mov	r3, #10
   2e224: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e228: ebff9f8e     	bl	0x16068    @ imm = #-0x181c8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e22c: e30f3cf8     	movw	r3, #0xfcf8
   2e230: e3403008     	movt	r3, #0x8
   2e234: e5933014     	ldr	r3, [r3, #0x14]
   2e238: e58d3214     	str	r3, [sp, #0x214]
   2e23c: ebffa00a     	bl	0x1626c    @ imm = #-0x17fd8 ; __cxa_end_catch
   2e240: eafff61b     	b	0x2bab4
   2e244: eafff877     	b	0x2c428
   2e248: eafff876     	b	0x2c428
   2e24c: e59d00a4     	ldr	r0, [sp, #0xa4]
   2e250: e24990f4     	sub	r9, r9, #244
   2e254: e1500009     	cmp	r0, r9
   2e258: 1afffd79     	bne	0x2d844
   2e25c: eafff8c8     	b	0x2c584
   2e260: e1a0b000     	mov	r11, r0
   2e264: e1a00004     	mov	r0, r4
   2e268: e1a05001     	mov	r5, r1
   2e26c: eb00cbe8     	bl	0x61214
   2e270: eafff86f     	b	0x2c434
   2e274: e1a0b000     	mov	r11, r0
   2e278: e1a03001     	mov	r3, r1
   2e27c: e3510002     	cmp	r1, #2
   2e280: 1afff826     	bne	0x2c320
   2e284: ebff9e1e     	bl	0x15b04    @ imm = #-0x18788 ; __cxa_begin_catch
   2e288: e30118c0     	movw	r1, #0x18c0
   2e28c: e3401007     	movt	r1, #0x7
   2e290: e1a0b000     	mov	r11, r0
   2e294: e3a0201b     	mov	r2, #27
   2e298: e30f0764     	movw	r0, #0xf764
   2e29c: e3400008     	movt	r0, #0x8
   2e2a0: ebff9f70     	bl	0x16068    @ imm = #-0x18240 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e2a4: e59b3000     	ldr	r3, [r11]
   2e2a8: e1a0000b     	mov	r0, r11
   2e2ac: e5933008     	ldr	r3, [r3, #0x8]
   2e2b0: e12fff33     	blx	r3
   2e2b4: e1a01000     	mov	r1, r0
   2e2b8: e30f0764     	movw	r0, #0xf764
   2e2bc: e3400008     	movt	r0, #0x8
   2e2c0: ebff9ef3     	bl	0x15e94    @ imm = #-0x18434 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e2c4: e3a02001     	mov	r2, #1
   2e2c8: e1a01004     	mov	r1, r4
   2e2cc: e3a0300a     	mov	r3, #10
   2e2d0: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e2d4: ebff9f63     	bl	0x16068    @ imm = #-0x18274 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e2d8: e59f32a4     	ldr	r3, [pc, #0x2a4]        @ 0x2e584
   2e2dc: e28d2b11     	add	r2, sp, #17408
   2e2e0: e2822094     	add	r2, r2, #148
   2e2e4: e5933294     	ldr	r3, [r3, #0x294]
   2e2e8: e5823000     	str	r3, [r2]
   2e2ec: ebff9fde     	bl	0x1626c    @ imm = #-0x18088 ; __cxa_end_catch
   2e2f0: eafff6cf     	b	0x2be34
   2e2f4: ebff9e02     	bl	0x15b04    @ imm = #-0x187f8 ; __cxa_begin_catch
   2e2f8: e30118dc     	movw	r1, #0x18dc
   2e2fc: e3401007     	movt	r1, #0x7
   2e300: e1a0b000     	mov	r11, r0
   2e304: e3a02016     	mov	r2, #22
   2e308: e30f0764     	movw	r0, #0xf764
   2e30c: e3400008     	movt	r0, #0x8
   2e310: ebff9f54     	bl	0x16068    @ imm = #-0x182b0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e314: e59b3000     	ldr	r3, [r11]
   2e318: e1a0000b     	mov	r0, r11
   2e31c: e5933008     	ldr	r3, [r3, #0x8]
   2e320: e12fff33     	blx	r3
   2e324: e1a01000     	mov	r1, r0
   2e328: e30f0764     	movw	r0, #0xf764
   2e32c: e3400008     	movt	r0, #0x8
   2e330: ebff9ed7     	bl	0x15e94    @ imm = #-0x184a4 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e334: e3a02001     	mov	r2, #1
   2e338: e1a01004     	mov	r1, r4
   2e33c: e3a0300a     	mov	r3, #10
   2e340: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e344: ebff9f47     	bl	0x16068    @ imm = #-0x182e4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e348: e59f3234     	ldr	r3, [pc, #0x234]        @ 0x2e584
   2e34c: e28d2b11     	add	r2, sp, #17408
   2e350: e2822098     	add	r2, r2, #152
   2e354: e5933298     	ldr	r3, [r3, #0x298]
   2e358: e5823000     	str	r3, [r2]
   2e35c: ebff9fc2     	bl	0x1626c    @ imm = #-0x180f8 ; __cxa_end_catch
   2e360: eafff6bc     	b	0x2be58
   2e364: eafff82f     	b	0x2c428
   2e368: eafff82e     	b	0x2c428
   2e36c: e1a0b000     	mov	r11, r0
   2e370: e1a03001     	mov	r3, r1
   2e374: e3510002     	cmp	r1, #2
   2e378: 1afff7e8     	bne	0x2c320
   2e37c: ebff9de0     	bl	0x15b04    @ imm = #-0x18880 ; __cxa_begin_catch
   2e380: e30118a8     	movw	r1, #0x18a8
   2e384: e3401007     	movt	r1, #0x7
   2e388: e1a0b000     	mov	r11, r0
   2e38c: e3a02017     	mov	r2, #23
   2e390: e30f0764     	movw	r0, #0xf764
   2e394: e3400008     	movt	r0, #0x8
   2e398: ebff9f32     	bl	0x16068    @ imm = #-0x18338 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e39c: e59b3000     	ldr	r3, [r11]
   2e3a0: e1a0000b     	mov	r0, r11
   2e3a4: e5933008     	ldr	r3, [r3, #0x8]
   2e3a8: e12fff33     	blx	r3
   2e3ac: e1a01000     	mov	r1, r0
   2e3b0: e30f0764     	movw	r0, #0xf764
   2e3b4: e3400008     	movt	r0, #0x8
   2e3b8: ebff9eb5     	bl	0x15e94    @ imm = #-0x1852c ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e3bc: e3a02001     	mov	r2, #1
   2e3c0: e1a01004     	mov	r1, r4
   2e3c4: e3a0300a     	mov	r3, #10
   2e3c8: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e3cc: ebff9f25     	bl	0x16068    @ imm = #-0x1836c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e3d0: e59f31ac     	ldr	r3, [pc, #0x1ac]        @ 0x2e584
   2e3d4: e28d2b11     	add	r2, sp, #17408
   2e3d8: e2822090     	add	r2, r2, #144
   2e3dc: e5933290     	ldr	r3, [r3, #0x290]
   2e3e0: e5823000     	str	r3, [r2]
   2e3e4: ebff9fa0     	bl	0x1626c    @ imm = #-0x18180 ; __cxa_end_catch
   2e3e8: eafff688     	b	0x2be10
   2e3ec: e1a0b000     	mov	r11, r0
   2e3f0: e1a03001     	mov	r3, r1
   2e3f4: e3510002     	cmp	r1, #2
   2e3f8: 1afff7c8     	bne	0x2c320
   2e3fc: ebff9dc0     	bl	0x15b04    @ imm = #-0x18900 ; __cxa_begin_catch
   2e400: e301188c     	movw	r1, #0x188c
   2e404: e3401007     	movt	r1, #0x7
   2e408: e1a0b000     	mov	r11, r0
   2e40c: e3a0201a     	mov	r2, #26
   2e410: e30f0764     	movw	r0, #0xf764
   2e414: e3400008     	movt	r0, #0x8
   2e418: ebff9f12     	bl	0x16068    @ imm = #-0x183b8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e41c: e59b3000     	ldr	r3, [r11]
   2e420: e1a0000b     	mov	r0, r11
   2e424: e5933008     	ldr	r3, [r3, #0x8]
   2e428: e12fff33     	blx	r3
   2e42c: e1a01000     	mov	r1, r0
   2e430: e30f0764     	movw	r0, #0xf764
   2e434: e3400008     	movt	r0, #0x8
   2e438: ebff9e95     	bl	0x15e94    @ imm = #-0x185ac ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e43c: e3a02001     	mov	r2, #1
   2e440: e1a01004     	mov	r1, r4
   2e444: e3a0300a     	mov	r3, #10
   2e448: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e44c: ebff9f05     	bl	0x16068    @ imm = #-0x183ec ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e450: e59f312c     	ldr	r3, [pc, #0x12c]        @ 0x2e584
   2e454: e28d2b11     	add	r2, sp, #17408
   2e458: e282208c     	add	r2, r2, #140
   2e45c: e593328c     	ldr	r3, [r3, #0x28c]
   2e460: e5823000     	str	r3, [r2]
   2e464: ebff9f80     	bl	0x1626c    @ imm = #-0x18200 ; __cxa_end_catch
   2e468: eafff65f     	b	0x2bdec
   2e46c: eafff7ed     	b	0x2c428
   2e470: eafff7ec     	b	0x2c428
   2e474: e1a0b000     	mov	r11, r0
   2e478: e1a03001     	mov	r3, r1
   2e47c: e3510002     	cmp	r1, #2
   2e480: 1afff7a6     	bne	0x2c320
   2e484: ebff9d9e     	bl	0x15b04    @ imm = #-0x18988 ; __cxa_begin_catch
   2e488: e301186c     	movw	r1, #0x186c
   2e48c: e3401007     	movt	r1, #0x7
   2e490: e1a0b000     	mov	r11, r0
   2e494: e3a0201c     	mov	r2, #28
   2e498: e30f0764     	movw	r0, #0xf764
   2e49c: e3400008     	movt	r0, #0x8
   2e4a0: ebff9ef0     	bl	0x16068    @ imm = #-0x18440 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e4a4: e59b3000     	ldr	r3, [r11]
   2e4a8: e1a0000b     	mov	r0, r11
   2e4ac: e5933008     	ldr	r3, [r3, #0x8]
   2e4b0: e12fff33     	blx	r3
   2e4b4: e1a01000     	mov	r1, r0
   2e4b8: e30f0764     	movw	r0, #0xf764
   2e4bc: e3400008     	movt	r0, #0x8
   2e4c0: ebff9e73     	bl	0x15e94    @ imm = #-0x18634 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e4c4: e3a02001     	mov	r2, #1
   2e4c8: e1a01004     	mov	r1, r4
   2e4cc: e3a0300a     	mov	r3, #10
   2e4d0: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e4d4: ebff9ee3     	bl	0x16068    @ imm = #-0x18474 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e4d8: e59f30a4     	ldr	r3, [pc, #0xa4]         @ 0x2e584
   2e4dc: e28d2b11     	add	r2, sp, #17408
   2e4e0: e2822088     	add	r2, r2, #136
   2e4e4: e5933288     	ldr	r3, [r3, #0x288]
   2e4e8: e5823000     	str	r3, [r2]
   2e4ec: ebff9f5e     	bl	0x1626c    @ imm = #-0x18288 ; __cxa_end_catch
   2e4f0: eafff634     	b	0x2bdc8
   2e4f4: e1a0b000     	mov	r11, r0
   2e4f8: e1a03001     	mov	r3, r1
   2e4fc: e3510002     	cmp	r1, #2
   2e500: 1afff786     	bne	0x2c320
   2e504: ebff9d7e     	bl	0x15b04    @ imm = #-0x18a08 ; __cxa_begin_catch
   2e508: e3011854     	movw	r1, #0x1854
   2e50c: e3401007     	movt	r1, #0x7
   2e510: e1a0b000     	mov	r11, r0
   2e514: e3a02014     	mov	r2, #20
   2e518: e30f0764     	movw	r0, #0xf764
   2e51c: e3400008     	movt	r0, #0x8
   2e520: ebff9ed0     	bl	0x16068    @ imm = #-0x184c0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e524: e59b3000     	ldr	r3, [r11]
   2e528: e1a0000b     	mov	r0, r11
   2e52c: e5933008     	ldr	r3, [r3, #0x8]
   2e530: e12fff33     	blx	r3
   2e534: e1a01000     	mov	r1, r0
   2e538: e30f0764     	movw	r0, #0xf764
   2e53c: e3400008     	movt	r0, #0x8
   2e540: ebff9e53     	bl	0x15e94    @ imm = #-0x186b4 ; _ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc
   2e544: e3a02001     	mov	r2, #1
   2e548: e1a01004     	mov	r1, r4
   2e54c: e3a0300a     	mov	r3, #10
   2e550: e5cd30a4     	strb	r3, [sp, #0xa4]
   2e554: ebff9ec3     	bl	0x16068    @ imm = #-0x184f4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   2e558: e59f3024     	ldr	r3, [pc, #0x24]         @ 0x2e584
   2e55c: e28d2b11     	add	r2, sp, #17408
   2e560: e2822084     	add	r2, r2, #132
   2e564: e5933284     	ldr	r3, [r3, #0x284]
   2e568: e5823000     	str	r3, [r2]
   2e56c: ebff9f3e     	bl	0x1626c    @ imm = #-0x18308 ; __cxa_end_catch
   2e570: eafff60b     	b	0x2bda4
   2e574: eafff7ab     	b	0x2c428
   2e578: eafff7aa     	b	0x2c428
   2e57c: d0 1f 07 00  	.word	0x00071fd0
   2e580: 3c fd 08 00  	.word	0x0008fd3c
   2e584: f8 3c 09 00  	.word	0x00093cf8
