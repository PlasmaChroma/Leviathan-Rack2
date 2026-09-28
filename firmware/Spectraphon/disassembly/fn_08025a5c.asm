; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08025698  0028      cmp	r0, #0
0802569a  00f09981  beq.w	#818 ; -> 0x080259d0 ; branch_target=0x080259d0
0802569e  f8b5      push	{r3, r4, r5, r6, r7, lr}
080256a0  0446      mov	r4, r0
080256a2  faf793fe  bl	#-21210 ; -> 0x080203cc ; branch_target=0x080203cc
080256a6  94f83830  ldrb.w	r3, [r4, #56]
080256aa  2268      ldr	r2, [r4]
080256ac  012b      cmp	r3, #1
080256ae  1dd0      beq	#58 ; -> 0x080256ec ; branch_target=0x080256ec
080256b0  a24b      ldr	r3, [pc, #648] ; [0x0802593c] = 0x40015804 / f32_bits_interpretation=2.020997047
080256b2  9a42      cmp	r2, r3
080256b4  2ad0      beq	#84 ; -> 0x0802570c ; branch_target=0x0802570c
080256b6  2033      adds	r3, #32
080256b8  9a42      cmp	r2, r3
080256ba  27d0      beq	#78 ; -> 0x0802570c ; branch_target=0x0802570c
080256bc  03f57873  add.w	r3, r3, #992
080256c0  9a42      cmp	r2, r3
080256c2  66d0      beq	#204 ; -> 0x08025792 ; branch_target=0x08025792
080256c4  2033      adds	r3, #32
080256c6  9a42      cmp	r2, r3
080256c8  63d0      beq	#198 ; -> 0x08025792 ; branch_target=0x08025792
080256ca  03f57873  add.w	r3, r3, #992
080256ce  9a42      cmp	r2, r3
080256d0  00f08081  beq.w	#768 ; -> 0x080259d4 ; branch_target=0x080259d4
080256d4  2033      adds	r3, #32
080256d6  9a42      cmp	r2, r3
080256d8  00f07c81  beq.w	#760 ; -> 0x080259d4 ; branch_target=0x080259d4
080256dc  984b      ldr	r3, [pc, #608] ; [0x08025940] = 0x58005404
080256de  9a42      cmp	r2, r3
080256e0  02d0      beq	#4 ; -> 0x080256e8 ; branch_target=0x080256e8
080256e2  2033      adds	r3, #32
080256e4  9a42      cmp	r2, r3
080256e6  4dd1      bne	#154 ; -> 0x08025784 ; branch_target=0x08025784
080256e8  964d      ldr	r5, [pc, #600] ; [0x08025944] = 0x58005400
080256ea  10e0      b	#32 ; -> 0x0802570e ; branch_target=0x0802570e
080256ec  934b      ldr	r3, [pc, #588] ; [0x0802593c] = 0x40015804 / f32_bits_interpretation=2.020997047
080256ee  9a42      cmp	r2, r3
080256f0  05d0      beq	#10 ; -> 0x080256fe ; branch_target=0x080256fe
080256f2  03f1c053  add.w	r3, r3, #402653184
080256f6  a3f58233  sub.w	r3, r3, #66560
080256fa  9a42      cmp	r2, r3
080256fc  42d1      bne	#132 ; -> 0x08025784 ; branch_target=0x08025784
080256fe  6368      ldr	r3, [r4, #4]
08025700  012b      cmp	r3, #1
08025702  3fd1      bne	#126 ; -> 0x08025784 ; branch_target=0x08025784
08025704  636c      ldr	r3, [r4, #68]
08025706  002b      cmp	r3, #0
08025708  d2d0      beq	#-92 ; -> 0x080256b0 ; branch_target=0x080256b0
0802570a  3be0      b	#118 ; -> 0x08025784 ; branch_target=0x08025784
0802570c  8e4d      ldr	r5, [pc, #568] ; [0x08025948] = 0x40015800 / f32_bits_interpretation=2.020996094
0802570e  94f89130  ldrb.w	r3, [r4, #145]
08025712  03f0ff01  and	r1, r3, #255
08025716  002b      cmp	r3, #0
08025718  42d0      beq	#132 ; -> 0x080257a0 ; branch_target=0x080257a0
0802571a  8c4b      ldr	r3, [pc, #560] ; [0x0802594c] = 0x20000014
0802571c  8c49      ldr	r1, [pc, #560] ; [0x08025950] = 0x95cbec1b
0802571e  1b68      ldr	r3, [r3]
08025720  a1fb0313  umull	r1, r3, r1, r3
08025724  1168      ldr	r1, [r2]
08025726  1b0b      lsrs	r3, r3, #12
08025728  21f48031  bic	r1, r1, #65536
0802572c  9b00      lsls	r3, r3, #2
0802572e  1160      str	r1, [r2]
08025730  13b3      cbz	r3, #68 ; -> 0x08025778 ; branch_target=0x08025778
08025732  2268      ldr	r2, [r4]
08025734  013b      subs	r3, #1
08025736  1068      ldr	r0, [r2]
08025738  10f48030  ands	r0, r0, #65536
0802573c  f8d1      bne	#-16 ; -> 0x08025730 ; branch_target=0x08025730
0802573e  e368      ldr	r3, [r4, #12]
08025740  0222      movs	r2, #2
08025742  012b      cmp	r3, #1
08025744  84f89120  strb.w	r2, [r4, #145]
08025748  1ed0      beq	#60 ; -> 0x08025788 ; branch_target=0x08025788
0802574a  9342      cmp	r3, r2
0802574c  0bbf      itete	eq
0802574e  2022      moveq	r2, #32
08025750  0022      movne	r2, #0
08025752  2127      moveq	r7, #33
08025754  0127      movne	r7, #1
08025756  0bbf      itete	eq
08025758  2226      moveq	r6, #34
0802575a  0226      movne	r6, #2
0802575c  2321      moveq	r1, #35
0802575e  0321      movne	r1, #3
08025760  a368      ldr	r3, [r4, #8]
08025762  013b      subs	r3, #1
08025764  042b      cmp	r3, #4
08025766  00f27281  bhi.w	#740 ; -> 0x08025a4e ; branch_target=0x08025a4e
0802576a  dfe813f0  tbh	[pc, r3, lsl #1]
08025778  d4f89430  ldr.w	r3, [r4, #148]
0802577c  43f04003  orr	r3, r3, #64
08025780  c4f89430  str.w	r3, [r4, #148]
08025784  0120      movs	r0, #1
08025786  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08025788  1127      movs	r7, #17
0802578a  1226      movs	r6, #18
0802578c  1321      movs	r1, #19
0802578e  1022      movs	r2, #16
08025790  e6e7      b	#-52 ; -> 0x08025760 ; branch_target=0x08025760
08025792  94f89130  ldrb.w	r3, [r4, #145]
08025796  6f4d      ldr	r5, [pc, #444] ; [0x08025954] = 0x40015c00 / f32_bits_interpretation=2.021240234
08025798  03f0ff01  and	r1, r3, #255
0802579c  002b      cmp	r3, #0
0802579e  bcd1      bne	#-136 ; -> 0x0802571a ; branch_target=0x0802571a
080257a0  2046      mov	r0, r4
080257a2  84f89010  strb.w	r1, [r4, #144]
080257a6  0ff071fd  bl	#64226 ; -> 0x0803528c ; branch_target=0x0803528c
080257aa  2268      ldr	r2, [r4]
080257ac  b5e7      b	#-150 ; -> 0x0802571a ; branch_target=0x0802571a
080257ae  3246      mov	r2, r6
080257b0  4ff40066  mov.w	r6, #2048
080257b4  2a60      str	r2, [r5]
080257b6  236a      ldr	r3, [r4, #32]
080257b8  002b      cmp	r3, #0
080257ba  45d0      beq	#138 ; -> 0x08025848 ; branch_target=0x08025848
080257bc  2368      ldr	r3, [r4]
080257be  5f4a      ldr	r2, [pc, #380] ; [0x0802593c] = 0x40015804 / f32_bits_interpretation=2.020997047
080257c0  9342      cmp	r3, r2
080257c2  00f02281  beq.w	#580 ; -> 0x08025a0a ; branch_target=0x08025a0a
080257c6  2032      adds	r2, #32
080257c8  9342      cmp	r3, r2
080257ca  00f01e81  beq.w	#572 ; -> 0x08025a0a ; branch_target=0x08025a0a
080257ce  624a      ldr	r2, [pc, #392] ; [0x08025958] = 0x40015c04 / f32_bits_interpretation=2.021241188
080257d0  9342      cmp	r3, r2
080257d2  00f02681  beq.w	#588 ; -> 0x08025a22 ; branch_target=0x08025a22
080257d6  2032      adds	r2, #32
080257d8  9342      cmp	r3, r2
080257da  00f02281  beq.w	#580 ; -> 0x08025a22 ; branch_target=0x08025a22
080257de  5f4a      ldr	r2, [pc, #380] ; [0x0802595c] = 0x40016004 / f32_bits_interpretation=2.021485329
080257e0  9342      cmp	r3, r2
080257e2  00f01881  beq.w	#560 ; -> 0x08025a16 ; branch_target=0x08025a16
080257e6  2032      adds	r2, #32
080257e8  9342      cmp	r3, r2
080257ea  00f01481  beq.w	#552 ; -> 0x08025a16 ; branch_target=0x08025a16
080257ee  544a      ldr	r2, [pc, #336] ; [0x08025940] = 0x58005404
080257f0  9342      cmp	r3, r2
080257f2  00f02181  beq.w	#578 ; -> 0x08025a38 ; branch_target=0x08025a38
080257f6  5a4a      ldr	r2, [pc, #360] ; [0x08025960] = 0x58005424
080257f8  9342      cmp	r3, r2
080257fa  00f01881  beq.w	#560 ; -> 0x08025a2e ; branch_target=0x08025a2e
080257fe  a369      ldr	r3, [r4, #24]
08025800  616c      ldr	r1, [r4, #68]
08025802  b3f5002f  cmp.w	r3, #524288
08025806  00f0f280  beq.w	#484 ; -> 0x080259ee ; branch_target=0x080259ee
0802580a  a26a      ldr	r2, [r4, #40]
0802580c  00eb8000  add.w	r0, r0, r0, lsl #2
08025810  b2f1806f  cmp.w	r2, #67108864
08025814  4fea4003  lsl.w	r3, r0, #1
08025818  206a      ldr	r0, [r4, #32]
0802581a  14bf      ite	ne
0802581c  0122      movne	r2, #1
0802581e  0222      moveq	r2, #2
08025820  00fb02f2  mul	r2, r0, r2
08025824  1202      lsls	r2, r2, #8
08025826  b3fbf2f3  udiv	r3, r3, r2
0802582a  4e4a      ldr	r2, [pc, #312] ; [0x08025964] = 0xcccccccd / f32_bits_interpretation=-107374184
0802582c  a2fb0302  umull	r0, r2, r2, r3
08025830  d208      lsrs	r2, r2, #3
08025832  02eb8200  add.w	r0, r2, r2, lsl #2
08025836  a3eb4003  sub.w	r3, r3, r0, lsl #1
0802583a  092b      cmp	r3, #9
0802583c  08bf      it	eq
0802583e  0132      addeq	r2, #1
08025840  0429      cmp	r1, #4
08025842  08bf      it	eq
08025844  5208      lsreq	r2, r2, #1
08025846  6262      str	r2, [r4, #36]
08025848  6368      ldr	r3, [r4, #4]
0802584a  276d      ldr	r7, [r4, #80]
0802584c  33f00203  bics	r3, r3, #2
08025850  40f0b780  bne.w	#366 ; -> 0x080259c2 ; branch_target=0x080259c2
08025854  013f      subs	r7, #1
08025856  18bf      it	ne
08025858  0127      movne	r7, #1
0802585a  7f02      lsls	r7, r7, #9
0802585c  faf7b6fd  bl	#-21652 ; -> 0x080203cc ; branch_target=0x080203cc
08025860  b0f5005f  cmp.w	r0, #8192
08025864  2268      ldr	r2, [r4]
08025866  c0f09880  blo.w	#304 ; -> 0x0802599a ; branch_target=0x0802599a
0802586a  1168      ldr	r1, [r2]
0802586c  3e4b      ldr	r3, [pc, #248] ; [0x08025968] = 0xf005c010
0802586e  0b40      ands	r3, r1
08025870  1360      str	r3, [r2]
08025872  616c      ldr	r1, [r4, #68]
08025874  d4e90023  ldrd	r2, r3, [r4]
08025878  0b43      orrs	r3, r1
0802587a  a16c      ldr	r1, [r4, #72]
0802587c  0b43      orrs	r3, r1
0802587e  e16c      ldr	r1, [r4, #76]
08025880  0b43      orrs	r3, r1
08025882  e16a      ldr	r1, [r4, #44]
08025884  0b43      orrs	r3, r1
08025886  6169      ldr	r1, [r4, #20]
08025888  0b43      orrs	r3, r1
0802588a  a169      ldr	r1, [r4, #24]
0802588c  0b43      orrs	r3, r1
0802588e  a16a      ldr	r1, [r4, #40]
08025890  0b43      orrs	r3, r1
08025892  2169      ldr	r1, [r4, #16]
08025894  0b43      orrs	r3, r1
08025896  1168      ldr	r1, [r2]
08025898  0b43      orrs	r3, r1
0802589a  616a      ldr	r1, [r4, #36]
0802589c  43ea0153  orr.w	r3, r3, r1, lsl #20
080258a0  3343      orrs	r3, r6
080258a2  3b43      orrs	r3, r7
080258a4  1360      str	r3, [r2]
080258a6  2268      ldr	r2, [r4]
080258a8  304b      ldr	r3, [pc, #192] ; [0x0802596c] = 0xffff1ff0
080258aa  5168      ldr	r1, [r2, #4]
080258ac  0b40      ands	r3, r1
080258ae  5360      str	r3, [r2, #4]
080258b0  216b      ldr	r1, [r4, #48]
080258b2  e369      ldr	r3, [r4, #28]
080258b4  2268      ldr	r2, [r4]
080258b6  0b43      orrs	r3, r1
080258b8  616b      ldr	r1, [r4, #52]
080258ba  0b43      orrs	r3, r1
080258bc  5168      ldr	r1, [r2, #4]
080258be  0b43      orrs	r3, r1
080258c0  5360      str	r3, [r2, #4]
080258c2  2268      ldr	r2, [r4]
080258c4  2a4b      ldr	r3, [pc, #168] ; [0x08025970] = 0xfff88000
080258c6  9168      ldr	r1, [r2, #8]
080258c8  0b40      ands	r3, r1
080258ca  9360      str	r3, [r2, #8]
080258cc  e26d      ldr	r2, [r4, #92]
080258ce  636e      ldr	r3, [r4, #100]
080258d0  2168      ldr	r1, [r4]
080258d2  1343      orrs	r3, r2
080258d4  226e      ldr	r2, [r4, #96]
080258d6  8868      ldr	r0, [r1, #8]
080258d8  1343      orrs	r3, r2
080258da  626d      ldr	r2, [r4, #84]
080258dc  0343      orrs	r3, r0
080258de  013a      subs	r2, #1
080258e0  4ff22000  movw	r0, #61472
080258e4  1343      orrs	r3, r2
080258e6  a26d      ldr	r2, [r4, #88]
080258e8  013a      subs	r2, #1
080258ea  43ea0223  orr.w	r3, r3, r2, lsl #8
080258ee  8b60      str	r3, [r1, #8]
080258f0  2268      ldr	r2, [r4]
080258f2  d368      ldr	r3, [r2, #12]
080258f4  0340      ands	r3, r0
080258f6  d360      str	r3, [r2, #12]
080258f8  2168      ldr	r1, [r4]
080258fa  d4e91a30  ldrd	r3, r0, [r4, #104]
080258fe  ca68      ldr	r2, [r1, #12]
08025900  0343      orrs	r3, r0
08025902  1343      orrs	r3, r2
08025904  626f      ldr	r2, [r4, #116]
08025906  43ea0243  orr.w	r3, r3, r2, lsl #16
0802590a  226f      ldr	r2, [r4, #112]
0802590c  013a      subs	r2, #1
0802590e  43ea0223  orr.w	r3, r3, r2, lsl #8
08025912  0a4a      ldr	r2, [pc, #40] ; [0x0802593c] = 0x40015804 / f32_bits_interpretation=2.020997047
08025914  cb60      str	r3, [r1, #12]
08025916  2368      ldr	r3, [r4]
08025918  9342      cmp	r3, r2
0802591a  2bd0      beq	#86 ; -> 0x08025974 ; branch_target=0x08025974
0802591c  02f1c052  add.w	r2, r2, #402653184
08025920  a2f58232  sub.w	r2, r2, #66560
08025924  9342      cmp	r3, r2
08025926  25d0      beq	#74 ; -> 0x08025974 ; branch_target=0x08025974
08025928  0023      movs	r3, #0
0802592a  0122      movs	r2, #1
0802592c  c4f89430  str.w	r3, [r4, #148]
08025930  1846      mov	r0, r3
08025932  84f89030  strb.w	r3, [r4, #144]
08025936  84f89120  strb.w	r2, [r4, #145]
0802593a  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08025974  6b6c      ldr	r3, [r5, #68]
08025976  23f00103  bic	r3, r3, #1
0802597a  6b64      str	r3, [r5, #68]
0802597c  94f83830  ldrb.w	r3, [r4, #56]
08025980  012b      cmp	r3, #1
08025982  d1d1      bne	#-94 ; -> 0x08025928 ; branch_target=0x08025928
08025984  d4e90f23  ldrd	r2, r3, [r4, #60]
08025988  013a      subs	r2, #1
0802598a  43ea0213  orr.w	r3, r3, r2, lsl #4
0802598e  6b64      str	r3, [r5, #68]
08025990  6b6c      ldr	r3, [r5, #68]
08025992  43f00103  orr	r3, r3, #1
08025996  6b64      str	r3, [r5, #68]
08025998  c6e7      b	#-116 ; -> 0x08025928 ; branch_target=0x08025928
0802599a  1168      ldr	r1, [r2]
0802599c  2d4b      ldr	r3, [pc, #180] ; [0x08025a54] = 0xf805c010
0802599e  0b40      ands	r3, r1
080259a0  1360      str	r3, [r2]
080259a2  616c      ldr	r1, [r4, #68]
080259a4  d4e90023  ldrd	r2, r3, [r4]
080259a8  0b43      orrs	r3, r1
080259aa  a16c      ldr	r1, [r4, #72]
080259ac  0b43      orrs	r3, r1
080259ae  e16c      ldr	r1, [r4, #76]
080259b0  0b43      orrs	r3, r1
080259b2  e16a      ldr	r1, [r4, #44]
080259b4  0b43      orrs	r3, r1
080259b6  6169      ldr	r1, [r4, #20]
080259b8  0b43      orrs	r3, r1
080259ba  a169      ldr	r1, [r4, #24]
080259bc  0b43      orrs	r3, r1
080259be  a16a      ldr	r1, [r4, #40]
080259c0  68e7      b	#-304 ; -> 0x08025894 ; branch_target=0x08025894
080259c2  a7f10107  sub.w	r7, r7, #1
080259c6  b7fa87f7  clz	r7, r7
080259ca  7f09      lsrs	r7, r7, #5
080259cc  7f02      lsls	r7, r7, #9
080259ce  45e7      b	#-374 ; -> 0x0802585c ; branch_target=0x0802585c
080259d0  0120      movs	r0, #1
080259d2  7047      bx	lr
080259d4  204d      ldr	r5, [pc, #128] ; [0x08025a58] = 0x40016000 / f32_bits_interpretation=2.021484375
080259d6  9ae6      b	#-716 ; -> 0x0802570e ; branch_target=0x0802570e
080259d8  4ff48066  mov.w	r6, #1024
080259dc  eae6      b	#-556 ; -> 0x080257b4 ; branch_target=0x080257b4
080259de  0a46      mov	r2, r1
080259e0  4ff40066  mov.w	r6, #2048
080259e4  e6e6      b	#-564 ; -> 0x080257b4 ; branch_target=0x080257b4
080259e6  3a46      mov	r2, r7
080259e8  4ff40066  mov.w	r6, #2048
080259ec  e2e6      b	#-572 ; -> 0x080257b4 ; branch_target=0x080257b4
080259ee  0429      cmp	r1, #4
080259f0  28d0      beq	#80 ; -> 0x08025a44 ; branch_target=0x08025a44
080259f2  0829      cmp	r1, #8
080259f4  28d0      beq	#80 ; -> 0x08025a48 ; branch_target=0x08025a48
080259f6  626d      ldr	r2, [r4, #84]
080259f8  00eb8000  add.w	r0, r0, r0, lsl #2
080259fc  4300      lsls	r3, r0, #1
080259fe  206a      ldr	r0, [r4, #32]
08025a00  00fb02f2  mul	r2, r0, r2
08025a04  b3fbf2f3  udiv	r3, r3, r2
08025a08  0fe7      b	#-482 ; -> 0x0802582a ; branch_target=0x0802582a
08025a0a  4ff48070  mov.w	r0, #256
08025a0e  fff789fc  bl	#-1774 ; -> 0x08025324 ; branch_target=0x08025324
08025a12  2368      ldr	r3, [r4]
08025a14  dbe6      b	#-586 ; -> 0x080257ce ; branch_target=0x080257ce
08025a16  4ff40070  mov.w	r0, #512
08025a1a  fff783fc  bl	#-1786 ; -> 0x08025324 ; branch_target=0x08025324
08025a1e  2368      ldr	r3, [r4]
08025a20  e5e6      b	#-566 ; -> 0x080257ee ; branch_target=0x080257ee
08025a22  4ff40070  mov.w	r0, #512
08025a26  fff77dfc  bl	#-1798 ; -> 0x08025324 ; branch_target=0x08025324
08025a2a  2368      ldr	r3, [r4]
08025a2c  d7e6      b	#-594 ; -> 0x080257de ; branch_target=0x080257de
08025a2e  4ff40060  mov.w	r0, #2048
08025a32  fff777fc  bl	#-1810 ; -> 0x08025324 ; branch_target=0x08025324
08025a36  e2e6      b	#-572 ; -> 0x080257fe ; branch_target=0x080257fe
08025a38  4ff48060  mov.w	r0, #1024
08025a3c  fff772fc  bl	#-1820 ; -> 0x08025324 ; branch_target=0x08025324
08025a40  2368      ldr	r3, [r4]
08025a42  d8e6      b	#-592 ; -> 0x080257f6 ; branch_target=0x080257f6
08025a44  4022      movs	r2, #64
08025a46  d7e7      b	#-82 ; -> 0x080259f8 ; branch_target=0x080259f8
08025a48  4ff48072  mov.w	r2, #256
08025a4c  d4e7      b	#-88 ; -> 0x080259f8 ; branch_target=0x080259f8
08025a4e  0026      movs	r6, #0
08025a50  b0e6      b	#-672 ; -> 0x080257b4 ; branch_target=0x080257b4
08025a5c  0229      cmp	r1, #2
08025a5e  30b4      push	{r4, r5}
08025a60  28d9      bls	#80 ; -> 0x08025ab4 ; branch_target=0x08025ab4
08025a62  a1f1030c  sub.w	r12, r1, #3
08025a66  bcf1010f  cmp.w	r12, #1
08025a6a  20d8      bhi	#64 ; -> 0x08025aae ; branch_target=0x08025aae
08025a6c  0024      movs	r4, #0
08025a6e  0367      str	r3, [r0, #112]
08025a70  4464      str	r4, [r0, #68]
08025a72  c464      str	r4, [r0, #76]
08025a74  c465      str	r4, [r0, #92]
08025a76  8466      str	r4, [r0, #104]
08025a78  4ff6ff74  movw	r4, #65535
08025a7c  4467      str	r4, [r0, #116]
08025a7e  4468      ldr	r4, [r0, #4]
08025a80  34f00204  bics	r4, r4, #2
08025a84  0cbf      ite	eq
08025a86  0125      moveq	r5, #1
08025a88  0025      movne	r5, #0
08025a8a  0429      cmp	r1, #4
08025a8c  4ff40031  mov.w	r1, #131072
08025a90  0565      str	r5, [r0, #80]
08025a92  14bf      ite	ne
08025a94  0d24      movne	r4, #13
08025a96  0124      moveq	r4, #1
08025a98  0166      str	r1, [r0, #96]
08025a9a  4ff48021  mov.w	r1, #262144
08025a9e  8465      str	r4, [r0, #88]
08025aa0  4166      str	r1, [r0, #100]
08025aa2  032a      cmp	r2, #3
08025aa4  03d8      bhi	#6 ; -> 0x08025aae ; branch_target=0x08025aae
08025aa6  dfe802f0  tbb	[pc, r2]
08025aae  0120      movs	r0, #1
08025ab0  30bc      pop	{r4, r5}
08025ab2  7047      bx	lr
08025ab4  0024      movs	r4, #0
08025ab6  0367      str	r3, [r0, #112]
08025ab8  4464      str	r4, [r0, #68]
08025aba  c464      str	r4, [r0, #76]
08025abc  8466      str	r4, [r0, #104]
08025abe  4468      ldr	r4, [r0, #4]
08025ac0  34f00204  bics	r4, r4, #2
08025ac4  14bf      ite	ne
08025ac6  0124      movne	r4, #1
08025ac8  0024      moveq	r4, #0
08025aca  0465      str	r4, [r0, #80]
08025acc  4ff48034  mov.w	r4, #65536
08025ad0  c465      str	r4, [r0, #92]
08025ad2  4ff6ff74  movw	r4, #65535
08025ad6  4467      str	r4, [r0, #116]
08025ad8  13f00104  ands	r4, r3, #1
08025adc  e7d1      bne	#-50 ; -> 0x08025aae ; branch_target=0x08025aae
08025ade  49b9      cbnz	r1, #18 ; -> 0x08025af4 ; branch_target=0x08025af4
08025ae0  4ff48024  mov.w	r4, #262144
08025ae4  c0e91814  strd	r1, r4, [r0, #96]
08025ae8  032a      cmp	r2, #3
08025aea  e0d8      bhi	#-64 ; -> 0x08025aae ; branch_target=0x08025aae
08025aec  dfe802f0  tbb	[pc, r2]
08025af4  4466      str	r4, [r0, #100]
08025af6  4ff40034  mov.w	r4, #131072
08025afa  0466      str	r4, [r0, #96]
08025afc  032a      cmp	r2, #3
08025afe  d6d8      bhi	#-84 ; -> 0x08025aae ; branch_target=0x08025aae
08025b00  dfe802f0  tbb	[pc, r2]
08025b08  8021      movs	r1, #128
08025b0a  1b01      lsls	r3, r3, #4
08025b0c  4022      movs	r2, #64
08025b0e  8164      str	r1, [r0, #72]
08025b10  4365      str	r3, [r0, #84]
08025b12  c266      str	r2, [r0, #108]
08025b14  30bc      pop	{r4, r5}
08025b16  fff7bfbd  b.w	#-1154 ; -> 0x08025698 ; branch_target=0x08025698
08025b1a  8022      movs	r2, #128
08025b1c  5b01      lsls	r3, r3, #5
08025b1e  8264      str	r2, [r0, #72]
08025b20  4365      str	r3, [r0, #84]
08025b22  c266      str	r2, [r0, #108]
08025b24  f6e7      b	#-20 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b26  c021      movs	r1, #192
08025b28  5b01      lsls	r3, r3, #5
08025b2a  8022      movs	r2, #128
08025b2c  8164      str	r1, [r0, #72]
08025b2e  4365      str	r3, [r0, #84]
08025b30  c266      str	r2, [r0, #108]
08025b32  30bc      pop	{r4, r5}
08025b34  fff7b0bd  b.w	#-1184 ; -> 0x08025698 ; branch_target=0x08025698
08025b38  e021      movs	r1, #224
08025b3a  f5e7      b	#-22 ; -> 0x08025b28 ; branch_target=0x08025b28
08025b3c  5b08      lsrs	r3, r3, #1
08025b3e  e024      movs	r4, #224
08025b40  9a01      lsls	r2, r3, #6
08025b42  8021      movs	r1, #128
08025b44  5b01      lsls	r3, r3, #5
08025b46  8464      str	r4, [r0, #72]
08025b48  c166      str	r1, [r0, #108]
08025b4a  c0e91523  strd	r2, r3, [r0, #84]
08025b4e  e1e7      b	#-62 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b50  5b08      lsrs	r3, r3, #1
08025b52  8024      movs	r4, #128
08025b54  4021      movs	r1, #64
08025b56  5a01      lsls	r2, r3, #5
08025b58  8464      str	r4, [r0, #72]
08025b5a  1b01      lsls	r3, r3, #4
08025b5c  c166      str	r1, [r0, #108]
08025b5e  c0e91523  strd	r2, r3, [r0, #84]
08025b62  d7e7      b	#-82 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b64  5b08      lsrs	r3, r3, #1
08025b66  c024      movs	r4, #192
08025b68  eae7      b	#-44 ; -> 0x08025b40 ; branch_target=0x08025b40
08025b6a  5b08      lsrs	r3, r3, #1
08025b6c  8022      movs	r2, #128
08025b6e  9901      lsls	r1, r3, #6
08025b70  8264      str	r2, [r0, #72]
08025b72  5b01      lsls	r3, r3, #5
08025b74  c266      str	r2, [r0, #108]
08025b76  c0e91513  strd	r1, r3, [r0, #84]
08025b7a  cbe7      b	#-106 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b7c  5b08      lsrs	r3, r3, #1
08025b7e  c022      movs	r2, #192
08025b80  8025      movs	r5, #128
08025b82  0229      cmp	r1, #2
08025b84  4fea8314  lsl.w	r4, r3, #6
08025b88  8264      str	r2, [r0, #72]
08025b8a  4fea4313  lsl.w	r3, r3, #5
08025b8e  c566      str	r5, [r0, #108]
08025b90  4465      str	r4, [r0, #84]
08025b92  8365      str	r3, [r0, #88]
08025b94  bed1      bne	#-132 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b96  0823      movs	r3, #8
08025b98  8366      str	r3, [r0, #104]
08025b9a  bbe7      b	#-138 ; -> 0x08025b14 ; branch_target=0x08025b14
08025b9c  5b08      lsrs	r3, r3, #1
08025b9e  8022      movs	r2, #128
08025ba0  0229      cmp	r1, #2
08025ba2  4fea8314  lsl.w	r4, r3, #6
08025ba6  8264      str	r2, [r0, #72]
08025ba8  4fea4313  lsl.w	r3, r3, #5
08025bac  c266      str	r2, [r0, #108]
08025bae  c0e91543  strd	r4, r3, [r0, #84]
08025bb2  afd1      bne	#-162 ; -> 0x08025b14 ; branch_target=0x08025b14
08025bb4  1023      movs	r3, #16
08025bb6  8366      str	r3, [r0, #104]
08025bb8  ace7      b	#-168 ; -> 0x08025b14 ; branch_target=0x08025b14
