; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08022164  0028      cmp	r0, #0
08022166  00f00381  beq.w	#518 ; -> 0x08022370 ; branch_target=0x08022370
0802216a  2de9f843  push.w	{r3, r4, r5, r6, r7, r8, r9, lr}
0802216e  90f83530  ldrb.w	r3, [r0, #53]
08022172  022b      cmp	r3, #2
08022174  40f0ed80  bne.w	#474 ; -> 0x08022352 ; branch_target=0x08022352
08022178  0368      ldr	r3, [r0]
0802217a  7e4a      ldr	r2, [pc, #504] ; [0x08022374] = 0x40020010 / f32_bits_interpretation=2.031253815
0802217c  9342      cmp	r3, r2
0802217e  00f0ed80  beq.w	#474 ; -> 0x0802235c ; branch_target=0x0802235c
08022182  1832      adds	r2, #24
08022184  9342      cmp	r3, r2
08022186  00f0e980  beq.w	#466 ; -> 0x0802235c ; branch_target=0x0802235c
0802218a  1832      adds	r2, #24
0802218c  9342      cmp	r3, r2
0802218e  00f0e580  beq.w	#458 ; -> 0x0802235c ; branch_target=0x0802235c
08022192  1832      adds	r2, #24
08022194  9342      cmp	r3, r2
08022196  00f0e180  beq.w	#450 ; -> 0x0802235c ; branch_target=0x0802235c
0802219a  1832      adds	r2, #24
0802219c  9342      cmp	r3, r2
0802219e  00f0dd80  beq.w	#442 ; -> 0x0802235c ; branch_target=0x0802235c
080221a2  1832      adds	r2, #24
080221a4  9342      cmp	r3, r2
080221a6  00f0d980  beq.w	#434 ; -> 0x0802235c ; branch_target=0x0802235c
080221aa  1832      adds	r2, #24
080221ac  9342      cmp	r3, r2
080221ae  00f0d580  beq.w	#426 ; -> 0x0802235c ; branch_target=0x0802235c
080221b2  1832      adds	r2, #24
080221b4  9342      cmp	r3, r2
080221b6  00f0d180  beq.w	#418 ; -> 0x0802235c ; branch_target=0x0802235c
080221ba  02f55672  add.w	r2, r2, #856
080221be  9342      cmp	r3, r2
080221c0  00f0cc80  beq.w	#408 ; -> 0x0802235c ; branch_target=0x0802235c
080221c4  6c49      ldr	r1, [pc, #432] ; [0x08022378] = 0x40020428 / f32_bits_interpretation=2.031503677
080221c6  8b42      cmp	r3, r1
080221c8  00f0c880  beq.w	#400 ; -> 0x0802235c ; branch_target=0x0802235c
080221cc  dff8d081  ldr.w	r8, [pc, #464] ; [0x080223a0] = 0x40020440 / f32_bits_interpretation=2.031509399
080221d0  4345      cmp	r3, r8
080221d2  00f0c380  beq.w	#390 ; -> 0x0802235c ; branch_target=0x0802235c
080221d6  694f      ldr	r7, [pc, #420] ; [0x0802237c] = 0x40020458 / f32_bits_interpretation=2.031515121
080221d8  bb42      cmp	r3, r7
080221da  00f0bf80  beq.w	#382 ; -> 0x0802235c ; branch_target=0x0802235c
080221de  684e      ldr	r6, [pc, #416] ; [0x08022380] = 0x40020470 / f32_bits_interpretation=2.031520844
080221e0  b342      cmp	r3, r6
080221e2  00f0bb80  beq.w	#374 ; -> 0x0802235c ; branch_target=0x0802235c
080221e6  674d      ldr	r5, [pc, #412] ; [0x08022384] = 0x40020488 / f32_bits_interpretation=2.031526566
080221e8  ab42      cmp	r3, r5
080221ea  00f0b780  beq.w	#366 ; -> 0x0802235c ; branch_target=0x0802235c
080221ee  664c      ldr	r4, [pc, #408] ; [0x08022388] = 0x400204a0 / f32_bits_interpretation=2.031532288
080221f0  a342      cmp	r3, r4
080221f2  00f0b380  beq.w	#358 ; -> 0x0802235c ; branch_target=0x0802235c
080221f6  dff8ace1  ldr.w	lr, [pc, #428] ; [0x080223a4] = 0x400204b8 / f32_bits_interpretation=2.03153801
080221fa  7345      cmp	r3, lr
080221fc  00f0ae80  beq.w	#348 ; -> 0x0802235c ; branch_target=0x0802235c
08022200  d3f800c0  ldr.w	r12, [r3]
08022204  614c      ldr	r4, [pc, #388] ; [0x0802238c] = 0x40020028 / f32_bits_interpretation=2.031259537
08022206  2cf00e0c  bic	r12, r12, #14
0802220a  6149      ldr	r1, [pc, #388] ; [0x08022390] = 0x40020040 / f32_bits_interpretation=2.031265259
0802220c  c3f800c0  str.w	r12, [r3]
08022210  d0f800c0  ldr.w	r12, [r0]
08022214  dcf80030  ldr.w	r3, [r12]
08022218  23f00103  bic	r3, r3, #1
0802221c  ccf80030  str.w	r3, [r12]
08022220  544b      ldr	r3, [pc, #336] ; [0x08022374] = 0x40020010 / f32_bits_interpretation=2.031253815
08022222  0268      ldr	r2, [r0]
08022224  a242      cmp	r2, r4
08022226  18bf      it	ne
08022228  9a42      cmpne	r2, r3
0802222a  04f13004  add.w	r4, r4, #48
0802222e  0cbf      ite	eq
08022230  0123      moveq	r3, #1
08022232  0023      movne	r3, #0
08022234  8a42      cmp	r2, r1
08022236  08bf      it	eq
08022238  43f00103  orreq	r3, r3, #1
0802223c  3031      adds	r1, #48
0802223e  a242      cmp	r2, r4
08022240  08bf      it	eq
08022242  43f00103  orreq	r3, r3, #1
08022246  3034      adds	r4, #48
08022248  8a42      cmp	r2, r1
0802224a  08bf      it	eq
0802224c  43f00103  orreq	r3, r3, #1
08022250  3031      adds	r1, #48
08022252  a242      cmp	r2, r4
08022254  08bf      it	eq
08022256  43f00103  orreq	r3, r3, #1
0802225a  3034      adds	r4, #48
0802225c  8a42      cmp	r2, r1
0802225e  08bf      it	eq
08022260  43f00103  orreq	r3, r3, #1
08022264  01f55c71  add.w	r1, r1, #880
08022268  a242      cmp	r2, r4
0802226a  08bf      it	eq
0802226c  43f00103  orreq	r3, r3, #1
08022270  04f55c74  add.w	r4, r4, #880
08022274  8a42      cmp	r2, r1
08022276  08bf      it	eq
08022278  43f00103  orreq	r3, r3, #1
0802227c  3031      adds	r1, #48
0802227e  a242      cmp	r2, r4
08022280  08bf      it	eq
08022282  43f00103  orreq	r3, r3, #1
08022286  3034      adds	r4, #48
08022288  8a42      cmp	r2, r1
0802228a  08bf      it	eq
0802228c  43f00103  orreq	r3, r3, #1
08022290  3031      adds	r1, #48
08022292  a242      cmp	r2, r4
08022294  08bf      it	eq
08022296  43f00103  orreq	r3, r3, #1
0802229a  3034      adds	r4, #48
0802229c  8a42      cmp	r2, r1
0802229e  08bf      it	eq
080222a0  43f00103  orreq	r3, r3, #1
080222a4  3031      adds	r1, #48
080222a6  a242      cmp	r2, r4
080222a8  08bf      it	eq
080222aa  43f00103  orreq	r3, r3, #1
080222ae  3034      adds	r4, #48
080222b0  8a42      cmp	r2, r1
080222b2  08bf      it	eq
080222b4  43f00103  orreq	r3, r3, #1
080222b8  3649      ldr	r1, [pc, #216] ; [0x08022394] = 0x58025408
080222ba  a242      cmp	r2, r4
080222bc  08bf      it	eq
080222be  43f00103  orreq	r3, r3, #1
080222c2  354c      ldr	r4, [pc, #212] ; [0x08022398] = 0x5802541c
080222c4  8a42      cmp	r2, r1
080222c6  08bf      it	eq
080222c8  43f00103  orreq	r3, r3, #1
080222cc  2831      adds	r1, #40
080222ce  a242      cmp	r2, r4
080222d0  08bf      it	eq
080222d2  43f00103  orreq	r3, r3, #1
080222d6  2834      adds	r4, #40
080222d8  8a42      cmp	r2, r1
080222da  08bf      it	eq
080222dc  43f00103  orreq	r3, r3, #1
080222e0  2831      adds	r1, #40
080222e2  a242      cmp	r2, r4
080222e4  08bf      it	eq
080222e6  43f00103  orreq	r3, r3, #1
080222ea  2834      adds	r4, #40
080222ec  8a42      cmp	r2, r1
080222ee  08bf      it	eq
080222f0  43f00103  orreq	r3, r3, #1
080222f4  2831      adds	r1, #40
080222f6  a242      cmp	r2, r4
080222f8  08bf      it	eq
080222fa  43f00103  orreq	r3, r3, #1
080222fe  8a42      cmp	r2, r1
08022300  08bf      it	eq
08022302  43f00103  orreq	r3, r3, #1
08022306  13b9      cbnz	r3, #4 ; -> 0x0802230e ; branch_target=0x0802230e
08022308  244b      ldr	r3, [pc, #144] ; [0x0802239c] = 0x58025494
0802230a  9a42      cmp	r2, r3
0802230c  17d1      bne	#46 ; -> 0x0802233e ; branch_target=0x0802233e
0802230e  016e      ldr	r1, [r0, #96]
08022310  0123      movs	r3, #1
08022312  0a68      ldr	r2, [r1]
08022314  22f48072  bic	r2, r2, #256
08022318  0a60      str	r2, [r1]
0802231a  d0e91612  ldrd	r1, r2, [r0, #88]
0802231e  02f01f02  and	r2, r2, #31
08022322  9340      lsls	r3, r2
08022324  4b60      str	r3, [r1, #4]
08022326  d0e91932  ldrd	r3, r2, [r0, #100]
0802232a  5a60      str	r2, [r3, #4]
0802232c  c36e      ldr	r3, [r0, #108]
0802232e  33b1      cbz	r3, #12 ; -> 0x0802233e ; branch_target=0x0802233e
08022330  1a68      ldr	r2, [r3]
08022332  22f48072  bic	r2, r2, #256
08022336  1a60      str	r2, [r3]
08022338  d0e91c32  ldrd	r3, r2, [r0, #112]
0802233c  5a60      str	r2, [r3, #4]
0802233e  0021      movs	r1, #0
08022340  0122      movs	r2, #1
08022342  036d      ldr	r3, [r0, #80]
08022344  80f83410  strb.w	r1, [r0, #52]
08022348  80f83520  strb.w	r2, [r0, #53]
0802234c  6bb1      cbz	r3, #26 ; -> 0x0802236a ; branch_target=0x0802236a
0802234e  9847      blx	r3
08022350  0be0      b	#22 ; -> 0x0802236a ; branch_target=0x0802236a
08022352  8023      movs	r3, #128
08022354  4365      str	r3, [r0, #84]
08022356  0120      movs	r0, #1
08022358  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802235c  0422      movs	r2, #4
0802235e  80f83520  strb.w	r2, [r0, #53]
08022362  1a68      ldr	r2, [r3]
08022364  22f00102  bic	r2, r2, #1
08022368  1a60      str	r2, [r3]
0802236a  0020      movs	r0, #0
0802236c  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
08022370  0120      movs	r0, #1
08022372  7047      bx	lr
