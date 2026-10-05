; lubadh::LubadhHardware::init()
; VA 0x6c164 size 2076

   6c164: e92d4070     	push	{r4, r5, r6, lr}
   6c168: e1a04000     	mov	r4, r0
   6c16c: e3a01002     	mov	r1, #2
   6c170: e24dd030     	sub	sp, sp, #48
   6c174: e3050254     	movw	r0, #0x5254
   6c178: e3400007     	movt	r0, #0x7
   6c17c: ebfea6f9     	bl	0x15d68    @ imm = #-0x5641c ; open64
   6c180: e3500000     	cmp	r0, #0
   6c184: e584003c     	str	r0, [r4, #0x3c]
   6c188: ba00012d     	blt	0x6c644
   6c18c: e1a01004     	mov	r1, r4
   6c190: ebfea6e2     	bl	0x15d20    @ imm = #-0x56478 ; tcgetattr
   6c194: e2505000     	subs	r5, r0, #0
   6c198: 1a0000de     	bne	0x6c518
   6c19c: e5943008     	ldr	r3, [r4, #0x8]
   6c1a0: e1a00004     	mov	r0, r4
   6c1a4: e5942000     	ldr	r2, [r4]
   6c1a8: e3011002     	movw	r1, #0x1002
   6c1ac: e3c33102     	bic	r3, r3, #-2147483648
   6c1b0: e594c00c     	ldr	r12, [r4, #0xc]
   6c1b4: e3c33e17     	bic	r3, r3, #368
   6c1b8: e3c22d77     	bic	r2, r2, #7616
   6c1bc: e3833e8b     	orr	r3, r3, #2224
   6c1c0: e5843008     	str	r3, [r4, #0x8]
   6c1c4: e5943004     	ldr	r3, [r4, #0x4]
   6c1c8: e3c2202b     	bic	r2, r2, #43
   6c1cc: e3ccc05b     	bic	r12, r12, #91
   6c1d0: e5842000     	str	r2, [r4]
   6c1d4: e3c33005     	bic	r3, r3, #5
   6c1d8: e584c00c     	str	r12, [r4, #0xc]
   6c1dc: e5843004     	str	r3, [r4, #0x4]
   6c1e0: ebfea7a6     	bl	0x16080    @ imm = #-0x56168 ; cfsetispeed
   6c1e4: e1a00004     	mov	r0, r4
   6c1e8: e3011002     	movw	r1, #0x1002
   6c1ec: ebfea7b8     	bl	0x160d4    @ imm = #-0x56120 ; cfsetospeed
   6c1f0: e594003c     	ldr	r0, [r4, #0x3c]
   6c1f4: e1a02004     	mov	r2, r4
   6c1f8: e1a01005     	mov	r1, r5
   6c1fc: ebfea736     	bl	0x15edc    @ imm = #-0x56328 ; tcsetattr
   6c200: e2504000     	subs	r4, r0, #0
   6c204: 1a000170     	bne	0x6c7cc
   6c208: eb000bd5     	bl	0x6f164  @ imm = #0x2f54
   6c20c: e3500000     	cmp	r0, #0
   6c210: 0a00019e     	beq	0x6c890
   6c214: eb0004ec     	bl	0x6d5cc
   6c218: e3500000     	cmp	r0, #0
   6c21c: 0a000139     	beq	0x6c708
   6c220: e3a00001     	mov	r0, #1
   6c224: eb000534     	bl	0x6d6fc
   6c228: e1a00004     	mov	r0, r4
   6c22c: eb000546     	bl	0x6d74c
   6c230: e3a00040     	mov	r0, #64
   6c234: eb000534     	bl	0x6d70c
   6c238: e1a00004     	mov	r0, r4
   6c23c: eb000666     	bl	0x6dbdc
   6c240: e1a01004     	mov	r1, r4
   6c244: e1a00004     	mov	r0, r4
   6c248: eb000669     	bl	0x6dbf4
   6c24c: e1a01004     	mov	r1, r4
   6c250: e3a00028     	mov	r0, #40
   6c254: eb000383     	bl	0x6d068
   6c258: e1a01004     	mov	r1, r4
   6c25c: e3a00020     	mov	r0, #32
   6c260: eb000380     	bl	0x6d068
   6c264: e1a01004     	mov	r1, r4
   6c268: e3a0002a     	mov	r0, #42
   6c26c: eb00037d     	bl	0x6d068
   6c270: e1a01004     	mov	r1, r4
   6c274: e3a0001b     	mov	r0, #27
   6c278: eb00037a     	bl	0x6d068
   6c27c: e1a01004     	mov	r1, r4
   6c280: e3a00021     	mov	r0, #33
   6c284: eb000377     	bl	0x6d068
   6c288: e1a01004     	mov	r1, r4
   6c28c: e3a00019     	mov	r0, #25
   6c290: eb000374     	bl	0x6d068
   6c294: e1a01004     	mov	r1, r4
   6c298: e3a00024     	mov	r0, #36
   6c29c: eb000371     	bl	0x6d068
   6c2a0: e1a01004     	mov	r1, r4
   6c2a4: e3a00026     	mov	r0, #38
   6c2a8: eb00036e     	bl	0x6d068
   6c2ac: e1a01004     	mov	r1, r4
   6c2b0: e3a0002b     	mov	r0, #43
   6c2b4: eb00036b     	bl	0x6d068
   6c2b8: e1a01004     	mov	r1, r4
   6c2bc: e3a00022     	mov	r0, #34
   6c2c0: eb000368     	bl	0x6d068
   6c2c4: e1a01004     	mov	r1, r4
   6c2c8: e3a00017     	mov	r0, #23
   6c2cc: eb000365     	bl	0x6d068
   6c2d0: e1a01004     	mov	r1, r4
   6c2d4: e3a00027     	mov	r0, #39
   6c2d8: eb000362     	bl	0x6d068
   6c2dc: e1a01004     	mov	r1, r4
   6c2e0: e3a0001e     	mov	r0, #30
   6c2e4: eb00035f     	bl	0x6d068
   6c2e8: e1a01004     	mov	r1, r4
   6c2ec: e3a0000c     	mov	r0, #12
   6c2f0: eb00035c     	bl	0x6d068
   6c2f4: e3a01002     	mov	r1, #2
   6c2f8: e3a00028     	mov	r0, #40
   6c2fc: eb00096a     	bl	0x6e8ac
   6c300: e3a01002     	mov	r1, #2
   6c304: e3a00020     	mov	r0, #32
   6c308: eb000967     	bl	0x6e8ac
   6c30c: e3a01002     	mov	r1, #2
   6c310: e3a0002a     	mov	r0, #42
   6c314: eb000964     	bl	0x6e8ac
   6c318: e3a01002     	mov	r1, #2
   6c31c: e3a0001b     	mov	r0, #27
   6c320: eb000961     	bl	0x6e8ac
   6c324: e3a01002     	mov	r1, #2
   6c328: e3a00021     	mov	r0, #33
   6c32c: eb00095e     	bl	0x6e8ac
   6c330: e3a01002     	mov	r1, #2
   6c334: e3a00019     	mov	r0, #25
   6c338: eb00095b     	bl	0x6e8ac
   6c33c: e3a01002     	mov	r1, #2
   6c340: e3a00029     	mov	r0, #41
   6c344: eb000958     	bl	0x6e8ac
   6c348: e3a01002     	mov	r1, #2
   6c34c: e3a00024     	mov	r0, #36
   6c350: eb000955     	bl	0x6e8ac
   6c354: e3a01002     	mov	r1, #2
   6c358: e3a00026     	mov	r0, #38
   6c35c: eb000952     	bl	0x6e8ac
   6c360: e3a01002     	mov	r1, #2
   6c364: e3a0002b     	mov	r0, #43
   6c368: eb00094f     	bl	0x6e8ac
   6c36c: e3a01002     	mov	r1, #2
   6c370: e3a00022     	mov	r0, #34
   6c374: eb00094c     	bl	0x6e8ac
   6c378: e3a01002     	mov	r1, #2
   6c37c: e3a00017     	mov	r0, #23
   6c380: eb000949     	bl	0x6e8ac
   6c384: e3a01002     	mov	r1, #2
   6c388: e3a00027     	mov	r0, #39
   6c38c: eb000946     	bl	0x6e8ac
   6c390: e3a01002     	mov	r1, #2
   6c394: e3a0001e     	mov	r0, #30
   6c398: eb000943     	bl	0x6e8ac
   6c39c: e3a01001     	mov	r1, #1
   6c3a0: e3a0000c     	mov	r0, #12
   6c3a4: eb000940     	bl	0x6e8ac
   6c3a8: e3a01001     	mov	r1, #1
   6c3ac: e3a00023     	mov	r0, #35
   6c3b0: eb00032c     	bl	0x6d068
   6c3b4: e3a01001     	mov	r1, #1
   6c3b8: e3a00025     	mov	r0, #37
   6c3bc: eb000329     	bl	0x6d068
   6c3c0: e3a01002     	mov	r1, #2
   6c3c4: e3a00023     	mov	r0, #35
   6c3c8: eb000937     	bl	0x6e8ac
   6c3cc: e1a01004     	mov	r1, r4
   6c3d0: e3a00025     	mov	r0, #37
   6c3d4: eb000934     	bl	0x6e8ac
   6c3d8: e3a01001     	mov	r1, #1
   6c3dc: e3a00023     	mov	r0, #35
   6c3e0: eb00043c     	bl	0x6d4d8
   6c3e4: e1a01004     	mov	r1, r4
   6c3e8: e3a00025     	mov	r0, #37
   6c3ec: eb000439     	bl	0x6d4d8
   6c3f0: e3090fbc     	movw	r0, #0x9fbc
   6c3f4: e3400009     	movt	r0, #0x9
   6c3f8: ebfea723     	bl	0x1608c    @ imm = #-0x56374 ; localtime
   6c3fc: e3053264     	movw	r3, #0x5264
   6c400: e3403007     	movt	r3, #0x7
   6c404: e58d302c     	str	r3, [sp, #0x2c]
   6c408: e28d3030     	add	r3, sp, #48
   6c40c: e58d0028     	str	r0, [sp, #0x28]
   6c410: e30f06d0     	movw	r0, #0xf6d0
   6c414: e3400008     	movt	r0, #0x8
   6c418: e9130006     	ldmdb	r3, {r1, r2}
   6c41c: eb0001be     	bl	0x6cb1c
   6c420: e3a02008     	mov	r2, #8
   6c424: e1a05000     	mov	r5, r0
   6c428: e305136c     	movw	r1, #0x536c
   6c42c: e3401007     	movt	r1, #0x7
   6c430: ebfea70c     	bl	0x16068    @ imm = #-0x563d0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c434: e59f1540     	ldr	r1, [pc, #0x540]        @ 0x6c97c
   6c438: e1a00005     	mov	r0, r5
   6c43c: e3a0200c     	mov	r2, #12
   6c440: ebfea708     	bl	0x16068    @ imm = #-0x563e0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c444: e1a00005     	mov	r0, r5
   6c448: e3a02001     	mov	r2, #1
   6c44c: e30512c4     	movw	r1, #0x52c4
   6c450: e3401007     	movt	r1, #0x7
   6c454: ebfea703     	bl	0x16068    @ imm = #-0x563f4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c458: e1a00005     	mov	r0, r5
   6c45c: e3a02004     	mov	r2, #4
   6c460: e30512c8     	movw	r1, #0x52c8
   6c464: e3401007     	movt	r1, #0x7
   6c468: ebfea6fe     	bl	0x16068    @ imm = #-0x56408 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c46c: e3a02001     	mov	r2, #1
   6c470: e1a00005     	mov	r0, r5
   6c474: e30512d0     	movw	r1, #0x52d0
   6c478: e3401007     	movt	r1, #0x7
   6c47c: ebfea6f9     	bl	0x16068    @ imm = #-0x5641c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c480: e1a00005     	mov	r0, r5
   6c484: e3a01075     	mov	r1, #117
   6c488: ebfea7ad     	bl	0x16344    @ imm = #-0x5614c ; _ZNSolsEi
   6c48c: e1a06000     	mov	r6, r0
   6c490: e3a02003     	mov	r2, #3
   6c494: e30512d4     	movw	r1, #0x52d4
   6c498: e3401007     	movt	r1, #0x7
   6c49c: ebfea6f1     	bl	0x16068    @ imm = #-0x5643c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c4a0: e1a00006     	mov	r0, r6
   6c4a4: e3a02019     	mov	r2, #25
   6c4a8: e3051378     	movw	r1, #0x5378
   6c4ac: e3401007     	movt	r1, #0x7
   6c4b0: ebfea6ec     	bl	0x16068    @ imm = #-0x56450 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c4b4: e5963000     	ldr	r3, [r6]
   6c4b8: e513300c     	ldr	r3, [r3, #-0xc]
   6c4bc: e0863003     	add	r3, r6, r3
   6c4c0: e593507c     	ldr	r5, [r3, #0x7c]
   6c4c4: e3550000     	cmp	r5, #0
   6c4c8: 0a00012a     	beq	0x6c978
   6c4cc: e5d5301c     	ldrb	r3, [r5, #0x1c]
   6c4d0: e3530000     	cmp	r3, #0
   6c4d4: 15d51027     	ldrbne	r1, [r5, #0x27]
   6c4d8: 1a000008     	bne	0x6c500
   6c4dc: e1a00005     	mov	r0, r5
   6c4e0: ebfea6a7     	bl	0x15f84    @ imm = #-0x56564 ; _ZNKSt5ctypeIcE13_M_widen_initEv
   6c4e4: e5952000     	ldr	r2, [r5]
   6c4e8: e30c3b14     	movw	r3, #0xcb14
   6c4ec: e3403006     	movt	r3, #0x6
   6c4f0: e5922018     	ldr	r2, [r2, #0x18]
   6c4f4: e1520003     	cmp	r2, r3
   6c4f8: 03a0100a     	moveq	r1, #10
   6c4fc: 1a000118     	bne	0x6c964
   6c500: e1a00006     	mov	r0, r6
   6c504: ebfea4f7     	bl	0x158e8     @ imm = #-0x56c24 ; _ZNSo3putEc
   6c508: ebfea5e9     	bl	0x15cb4    @ imm = #-0x5685c ; _ZNSo5flushEv
   6c50c: e1a00004     	mov	r0, r4
   6c510: e28dd030     	add	sp, sp, #48
   6c514: e8bd8070     	pop	{r4, r5, r6, pc}
   6c518: e3090fbc     	movw	r0, #0x9fbc
   6c51c: e3400009     	movt	r0, #0x9
   6c520: ebfea6d9     	bl	0x1608c    @ imm = #-0x5649c ; localtime
   6c524: e3053264     	movw	r3, #0x5264
   6c528: e3403007     	movt	r3, #0x7
   6c52c: e58d0008     	str	r0, [sp, #0x8]
   6c530: e1a02003     	mov	r2, r3
   6c534: e59d1008     	ldr	r1, [sp, #0x8]
   6c538: e30f06d0     	movw	r0, #0xf6d0
   6c53c: e3400008     	movt	r0, #0x8
   6c540: e58d300c     	str	r3, [sp, #0xc]
   6c544: eb000174     	bl	0x6cb1c
   6c548: e3a02009     	mov	r2, #9
   6c54c: e1a04000     	mov	r4, r0
   6c550: e305127c     	movw	r1, #0x527c
   6c554: e3401007     	movt	r1, #0x7
   6c558: ebfea6c2     	bl	0x16068    @ imm = #-0x564f8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c55c: e59f1418     	ldr	r1, [pc, #0x418]        @ 0x6c97c
   6c560: e1a00004     	mov	r0, r4
   6c564: e3a0200c     	mov	r2, #12
   6c568: ebfea6be     	bl	0x16068    @ imm = #-0x56508 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c56c: e1a00004     	mov	r0, r4
   6c570: e3a02001     	mov	r2, #1
   6c574: e30512c4     	movw	r1, #0x52c4
   6c578: e3401007     	movt	r1, #0x7
   6c57c: ebfea6b9     	bl	0x16068    @ imm = #-0x5651c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c580: e1a00004     	mov	r0, r4
   6c584: e3a02004     	mov	r2, #4
   6c588: e30512c8     	movw	r1, #0x52c8
   6c58c: e3401007     	movt	r1, #0x7
   6c590: ebfea6b4     	bl	0x16068    @ imm = #-0x56530 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c594: e3a02001     	mov	r2, #1
   6c598: e1a00004     	mov	r0, r4
   6c59c: e30512d0     	movw	r1, #0x52d0
   6c5a0: e3401007     	movt	r1, #0x7
   6c5a4: ebfea6af     	bl	0x16068    @ imm = #-0x56544 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c5a8: e1a00004     	mov	r0, r4
   6c5ac: e3a0101a     	mov	r1, #26
   6c5b0: ebfea763     	bl	0x16344    @ imm = #-0x56274 ; _ZNSolsEi
   6c5b4: e3a02003     	mov	r2, #3
   6c5b8: e1a04000     	mov	r4, r0
   6c5bc: e30512d4     	movw	r1, #0x52d4
   6c5c0: e3401007     	movt	r1, #0x7
   6c5c4: ebfea6a7     	bl	0x16068    @ imm = #-0x56564 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c5c8: e30512f4     	movw	r1, #0x52f4
   6c5cc: e3401007     	movt	r1, #0x7
   6c5d0: e1a00004     	mov	r0, r4
   6c5d4: e3a02027     	mov	r2, #39
   6c5d8: ebfea6a2     	bl	0x16068    @ imm = #-0x56578 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c5dc: e5943000     	ldr	r3, [r4]
   6c5e0: e513300c     	ldr	r3, [r3, #-0xc]
   6c5e4: e0843003     	add	r3, r4, r3
   6c5e8: e593507c     	ldr	r5, [r3, #0x7c]
   6c5ec: e3550000     	cmp	r5, #0
   6c5f0: 0a0000e0     	beq	0x6c978
   6c5f4: e5d5301c     	ldrb	r3, [r5, #0x1c]
   6c5f8: e3530000     	cmp	r3, #0
   6c5fc: 15d51027     	ldrbne	r1, [r5, #0x27]
   6c600: 1a000008     	bne	0x6c628
   6c604: e1a00005     	mov	r0, r5
   6c608: ebfea65d     	bl	0x15f84    @ imm = #-0x5668c ; _ZNKSt5ctypeIcE13_M_widen_initEv
   6c60c: e5952000     	ldr	r2, [r5]
   6c610: e30c3b14     	movw	r3, #0xcb14
   6c614: e3403006     	movt	r3, #0x6
   6c618: e5922018     	ldr	r2, [r2, #0x18]
   6c61c: e1520003     	cmp	r2, r3
   6c620: 03a0100a     	moveq	r1, #10
   6c624: 1a0000c9     	bne	0x6c950
   6c628: e1a00004     	mov	r0, r4
   6c62c: e3a04001     	mov	r4, #1
   6c630: ebfea4ac     	bl	0x158e8     @ imm = #-0x56d50 ; _ZNSo3putEc
   6c634: ebfea59e     	bl	0x15cb4    @ imm = #-0x56988 ; _ZNSo5flushEv
   6c638: e1a00004     	mov	r0, r4
   6c63c: e28dd030     	add	sp, sp, #48
   6c640: e8bd8070     	pop	{r4, r5, r6, pc}
   6c644: e3090fbc     	movw	r0, #0x9fbc
   6c648: e3400009     	movt	r0, #0x9
   6c64c: ebfea68e     	bl	0x1608c    @ imm = #-0x565c8 ; localtime
   6c650: e3053264     	movw	r3, #0x5264
   6c654: e3403007     	movt	r3, #0x7
   6c658: e58d0000     	str	r0, [sp]
   6c65c: e1a02003     	mov	r2, r3
   6c660: e59d1000     	ldr	r1, [sp]
   6c664: e30f06d0     	movw	r0, #0xf6d0
   6c668: e3400008     	movt	r0, #0x8
   6c66c: e58d3004     	str	r3, [sp, #0x4]
   6c670: eb000129     	bl	0x6cb1c
   6c674: e3a02009     	mov	r2, #9
   6c678: e1a04000     	mov	r4, r0
   6c67c: e305127c     	movw	r1, #0x527c
   6c680: e3401007     	movt	r1, #0x7
   6c684: ebfea677     	bl	0x16068    @ imm = #-0x56624 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c688: e59f12ec     	ldr	r1, [pc, #0x2ec]        @ 0x6c97c
   6c68c: e1a00004     	mov	r0, r4
   6c690: e3a0200c     	mov	r2, #12
   6c694: ebfea673     	bl	0x16068    @ imm = #-0x56634 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c698: e1a00004     	mov	r0, r4
   6c69c: e3a02001     	mov	r2, #1
   6c6a0: e30512c4     	movw	r1, #0x52c4
   6c6a4: e3401007     	movt	r1, #0x7
   6c6a8: ebfea66e     	bl	0x16068    @ imm = #-0x56648 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c6ac: e1a00004     	mov	r0, r4
   6c6b0: e3a02004     	mov	r2, #4
   6c6b4: e30512c8     	movw	r1, #0x52c8
   6c6b8: e3401007     	movt	r1, #0x7
   6c6bc: ebfea669     	bl	0x16068    @ imm = #-0x5665c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c6c0: e3a02001     	mov	r2, #1
   6c6c4: e1a00004     	mov	r0, r4
   6c6c8: e30512d0     	movw	r1, #0x52d0
   6c6cc: e3401007     	movt	r1, #0x7
   6c6d0: ebfea664     	bl	0x16068    @ imm = #-0x56670 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c6d4: e1a00004     	mov	r0, r4
   6c6d8: e3a01014     	mov	r1, #20
   6c6dc: ebfea718     	bl	0x16344    @ imm = #-0x563a0 ; _ZNSolsEi
   6c6e0: e3a02003     	mov	r2, #3
   6c6e4: e1a04000     	mov	r4, r0
   6c6e8: e30512d4     	movw	r1, #0x52d4
   6c6ec: e3401007     	movt	r1, #0x7
   6c6f0: ebfea65c     	bl	0x16068    @ imm = #-0x56690 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c6f4: e30512d8     	movw	r1, #0x52d8
   6c6f8: e3401007     	movt	r1, #0x7
   6c6fc: e1a00004     	mov	r0, r4
   6c700: e3a0201b     	mov	r2, #27
   6c704: eaffffb3     	b	0x6c5d8
   6c708: e3090fbc     	movw	r0, #0x9fbc
   6c70c: e3400009     	movt	r0, #0x9
   6c710: ebfea65d     	bl	0x1608c    @ imm = #-0x5668c ; localtime
   6c714: e3053264     	movw	r3, #0x5264
   6c718: e3403007     	movt	r3, #0x7
   6c71c: e58d0020     	str	r0, [sp, #0x20]
   6c720: e1a02003     	mov	r2, r3
   6c724: e59d1020     	ldr	r1, [sp, #0x20]
   6c728: e30f06d0     	movw	r0, #0xf6d0
   6c72c: e3400008     	movt	r0, #0x8
   6c730: e58d3024     	str	r3, [sp, #0x24]
   6c734: eb0000f8     	bl	0x6cb1c
   6c738: e3a02009     	mov	r2, #9
   6c73c: e1a04000     	mov	r4, r0
   6c740: e305127c     	movw	r1, #0x527c
   6c744: e3401007     	movt	r1, #0x7
   6c748: ebfea646     	bl	0x16068    @ imm = #-0x566e8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c74c: e59f1228     	ldr	r1, [pc, #0x228]        @ 0x6c97c
   6c750: e1a00004     	mov	r0, r4
   6c754: e3a0200c     	mov	r2, #12
   6c758: ebfea642     	bl	0x16068    @ imm = #-0x566f8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c75c: e1a00004     	mov	r0, r4
   6c760: e3a02001     	mov	r2, #1
   6c764: e30512c4     	movw	r1, #0x52c4
   6c768: e3401007     	movt	r1, #0x7
   6c76c: ebfea63d     	bl	0x16068    @ imm = #-0x5670c ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c770: e1a00004     	mov	r0, r4
   6c774: e3a02004     	mov	r2, #4
   6c778: e30512c8     	movw	r1, #0x52c8
   6c77c: e3401007     	movt	r1, #0x7
   6c780: ebfea638     	bl	0x16068    @ imm = #-0x56720 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c784: e3a02001     	mov	r2, #1
   6c788: e1a00004     	mov	r0, r4
   6c78c: e30512d0     	movw	r1, #0x52d0
   6c790: e3401007     	movt	r1, #0x7
   6c794: ebfea633     	bl	0x16068    @ imm = #-0x56734 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c798: e1a00004     	mov	r0, r4
   6c79c: e3a01042     	mov	r1, #66
   6c7a0: ebfea6e7     	bl	0x16344    @ imm = #-0x56464 ; _ZNSolsEi
   6c7a4: e3a02003     	mov	r2, #3
   6c7a8: e1a04000     	mov	r4, r0
   6c7ac: e30512d4     	movw	r1, #0x52d4
   6c7b0: e3401007     	movt	r1, #0x7
   6c7b4: ebfea62b     	bl	0x16068    @ imm = #-0x56754 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c7b8: e3051358     	movw	r1, #0x5358
   6c7bc: e3401007     	movt	r1, #0x7
   6c7c0: e1a00004     	mov	r0, r4
   6c7c4: e3a02013     	mov	r2, #19
   6c7c8: eaffff82     	b	0x6c5d8
   6c7cc: e3090fbc     	movw	r0, #0x9fbc
   6c7d0: e3400009     	movt	r0, #0x9
   6c7d4: ebfea62c     	bl	0x1608c    @ imm = #-0x56750 ; localtime
   6c7d8: e3053264     	movw	r3, #0x5264
   6c7dc: e3403007     	movt	r3, #0x7
   6c7e0: e58d0010     	str	r0, [sp, #0x10]
   6c7e4: e1a02003     	mov	r2, r3
   6c7e8: e59d1010     	ldr	r1, [sp, #0x10]
   6c7ec: e30f06d0     	movw	r0, #0xf6d0
   6c7f0: e3400008     	movt	r0, #0x8
   6c7f4: e58d3014     	str	r3, [sp, #0x14]
   6c7f8: eb0000c7     	bl	0x6cb1c
   6c7fc: e3a02009     	mov	r2, #9
   6c800: e1a04000     	mov	r4, r0
   6c804: e305127c     	movw	r1, #0x527c
   6c808: e3401007     	movt	r1, #0x7
   6c80c: ebfea615     	bl	0x16068    @ imm = #-0x567ac ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c810: e59f1164     	ldr	r1, [pc, #0x164]        @ 0x6c97c
   6c814: e1a00004     	mov	r0, r4
   6c818: e3a0200c     	mov	r2, #12
   6c81c: ebfea611     	bl	0x16068    @ imm = #-0x567bc ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c820: e1a00004     	mov	r0, r4
   6c824: e3a02001     	mov	r2, #1
   6c828: e30512c4     	movw	r1, #0x52c4
   6c82c: e3401007     	movt	r1, #0x7
   6c830: ebfea60c     	bl	0x16068    @ imm = #-0x567d0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c834: e1a00004     	mov	r0, r4
   6c838: e3a02004     	mov	r2, #4
   6c83c: e30512c8     	movw	r1, #0x52c8
   6c840: e3401007     	movt	r1, #0x7
   6c844: ebfea607     	bl	0x16068    @ imm = #-0x567e4 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c848: e3a02001     	mov	r2, #1
   6c84c: e1a00004     	mov	r0, r4
   6c850: e30512d0     	movw	r1, #0x52d0
   6c854: e3401007     	movt	r1, #0x7
   6c858: ebfea602     	bl	0x16068    @ imm = #-0x567f8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c85c: e1a00004     	mov	r0, r4
   6c860: e3a01035     	mov	r1, #53
   6c864: ebfea6b6     	bl	0x16344    @ imm = #-0x56528 ; _ZNSolsEi
   6c868: e3a02003     	mov	r2, #3
   6c86c: e1a04000     	mov	r4, r0
   6c870: e30512d4     	movw	r1, #0x52d4
   6c874: e3401007     	movt	r1, #0x7
   6c878: ebfea5fa     	bl	0x16068    @ imm = #-0x56818 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c87c: e305131c     	movw	r1, #0x531c
   6c880: e3401007     	movt	r1, #0x7
   6c884: e1a00004     	mov	r0, r4
   6c888: e3a0201d     	mov	r2, #29
   6c88c: eaffff51     	b	0x6c5d8
   6c890: e3090fbc     	movw	r0, #0x9fbc
   6c894: e3400009     	movt	r0, #0x9
   6c898: ebfea5fb     	bl	0x1608c    @ imm = #-0x56814 ; localtime
   6c89c: e3053264     	movw	r3, #0x5264
   6c8a0: e3403007     	movt	r3, #0x7
   6c8a4: e58d0018     	str	r0, [sp, #0x18]
   6c8a8: e1a02003     	mov	r2, r3
   6c8ac: e59d1018     	ldr	r1, [sp, #0x18]
   6c8b0: e30f06d0     	movw	r0, #0xf6d0
   6c8b4: e3400008     	movt	r0, #0x8
   6c8b8: e58d301c     	str	r3, [sp, #0x1c]
   6c8bc: eb000096     	bl	0x6cb1c
   6c8c0: e3a02009     	mov	r2, #9
   6c8c4: e1a04000     	mov	r4, r0
   6c8c8: e305127c     	movw	r1, #0x527c
   6c8cc: e3401007     	movt	r1, #0x7
   6c8d0: ebfea5e4     	bl	0x16068    @ imm = #-0x56870 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c8d4: e59f10a0     	ldr	r1, [pc, #0xa0]         @ 0x6c97c
   6c8d8: e1a00004     	mov	r0, r4
   6c8dc: e3a0200c     	mov	r2, #12
   6c8e0: ebfea5e0     	bl	0x16068    @ imm = #-0x56880 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c8e4: e1a00004     	mov	r0, r4
   6c8e8: e3a02001     	mov	r2, #1
   6c8ec: e30512c4     	movw	r1, #0x52c4
   6c8f0: e3401007     	movt	r1, #0x7
   6c8f4: ebfea5db     	bl	0x16068    @ imm = #-0x56894 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c8f8: e1a00004     	mov	r0, r4
   6c8fc: e3a02004     	mov	r2, #4
   6c900: e30512c8     	movw	r1, #0x52c8
   6c904: e3401007     	movt	r1, #0x7
   6c908: ebfea5d6     	bl	0x16068    @ imm = #-0x568a8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c90c: e3a02001     	mov	r2, #1
   6c910: e1a00004     	mov	r0, r4
   6c914: e30512d0     	movw	r1, #0x52d0
   6c918: e3401007     	movt	r1, #0x7
   6c91c: ebfea5d1     	bl	0x16068    @ imm = #-0x568bc ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c920: e1a00004     	mov	r0, r4
   6c924: e3a0103c     	mov	r1, #60
   6c928: ebfea685     	bl	0x16344    @ imm = #-0x565ec ; _ZNSolsEi
   6c92c: e3a02003     	mov	r2, #3
   6c930: e1a04000     	mov	r4, r0
   6c934: e30512d4     	movw	r1, #0x52d4
   6c938: e3401007     	movt	r1, #0x7
   6c93c: ebfea5c9     	bl	0x16068    @ imm = #-0x568dc ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6c940: e305133c     	movw	r1, #0x533c
   6c944: e3401007     	movt	r1, #0x7
   6c948: e1a00004     	mov	r0, r4
   6c94c: eaffff6b     	b	0x6c700
   6c950: e3a0100a     	mov	r1, #10
   6c954: e1a00005     	mov	r0, r5
   6c958: e12fff32     	blx	r2
   6c95c: e1a01000     	mov	r1, r0
   6c960: eaffff30     	b	0x6c628
   6c964: e3a0100a     	mov	r1, #10
   6c968: e1a00005     	mov	r0, r5
   6c96c: e12fff32     	blx	r2
   6c970: e1a01000     	mov	r1, r0
   6c974: eafffee1     	b	0x6c500
   6c978: ebfea5ae     	bl	0x16038    @ imm = #-0x56948 ; _ZSt16__throw_bad_castv
   6c97c: b6 52 07 00  	.word	0x000752b6
