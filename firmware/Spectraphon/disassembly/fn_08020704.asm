; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020704  70b5      push	{r4, r5, r6, lr}
08020706  0022      movs	r2, #0
08020708  82b0      sub	sp, #8
0802070a  0192      str	r2, [sp, #4]
0802070c  90f85020  ldrb.w	r2, [r0, #80]
08020710  012a      cmp	r2, #1
08020712  00f04681  beq.w	#652 ; -> 0x080209a2 ; branch_target=0x080209a2
08020716  0122      movs	r2, #1
08020718  0468      ldr	r4, [r0]
0802071a  0346      mov	r3, r0
0802071c  80f85020  strb.w	r2, [r0, #80]
08020720  a068      ldr	r0, [r4, #8]
08020722  4507      lsls	r5, r0, #29
08020724  49d4      bmi	#146 ; -> 0x080207ba ; branch_target=0x080207ba
08020726  0868      ldr	r0, [r1]
08020728  c0f31305  ubfx	r5, r0, #0, #20
0802072c  002d      cmp	r5, #0
0802072e  40f0a780  bne.w	#334 ; -> 0x08020880 ; branch_target=0x08020880
08020732  c0f38460  ubfx	r0, r0, #26, #5
08020736  8240      lsls	r2, r0
08020738  e069      ldr	r0, [r4, #28]
0802073a  4ff01f0e  mov.w	lr, #31
0802073e  0243      orrs	r2, r0
08020740  e261      str	r2, [r4, #28]
08020742  4868      ldr	r0, [r1, #4]
08020744  1a68      ldr	r2, [r3]
08020746  8409      lsrs	r4, r0, #6
08020748  00f01f00  and	r0, r0, #31
0802074c  02f1300c  add.w	r12, r2, #48
08020750  0a68      ldr	r2, [r1]
08020752  04f00c04  and	r4, r4, #12
08020756  0efa00fe  lsl.w	lr, lr, r0
0802075a  c2f38462  ubfx	r2, r2, #26, #5
0802075e  8240      lsls	r2, r0
08020760  5cf80400  ldr.w	r0, [r12, r4]
08020764  20ea0e00  bic.w	r0, r0, lr
08020768  0243      orrs	r2, r0
0802076a  4cf80420  str.w	r2, [r12, r4]
0802076e  1a68      ldr	r2, [r3]
08020770  9068      ldr	r0, [r2, #8]
08020772  10f0040f  tst.w	r0, #4
08020776  9068      ldr	r0, [r2, #8]
08020778  01d1      bne	#2 ; -> 0x0802077e ; branch_target=0x0802077e
0802077a  0407      lsls	r4, r0, #28
0802077c  27d5      bpl	#78 ; -> 0x080207ce ; branch_target=0x080207ce
0802077e  9068      ldr	r0, [r2, #8]
08020780  c007      lsls	r0, r0, #31
08020782  18d4      bmi	#48 ; -> 0x080207b6 ; branch_target=0x080207b6
08020784  c868      ldr	r0, [r1, #12]
08020786  0c68      ldr	r4, [r1]
08020788  00f0180c  and	r12, r0, #24
0802078c  b848      ldr	r0, [pc, #736] ; [0x08020a70] = 0x000fffff
0802078e  d2f8c050  ldr.w	r5, [r2, #192]
08020792  20fa0cf0  lsr.w	r0, r0, r12
08020796  2040      ands	r0, r4
08020798  c4f31304  ubfx	r4, r4, #0, #20
0802079c  25ea0404  bic.w	r4, r5, r4
080207a0  2043      orrs	r0, r4
080207a2  b44c      ldr	r4, [pc, #720] ; [0x08020a74] = 0x47ff0000 / f32_bits_interpretation=130560
080207a4  c2f8c000  str.w	r0, [r2, #192]
080207a8  ca68      ldr	r2, [r1, #12]
080207aa  a242      cmp	r2, r4
080207ac  00f0a780  beq.w	#334 ; -> 0x080208fe ; branch_target=0x080208fe
080207b0  0a68      ldr	r2, [r1]
080207b2  002a      cmp	r2, #0
080207b4  6ddb      blt	#218 ; -> 0x08020892 ; branch_target=0x08020892
080207b6  0020      movs	r0, #0
080207b8  04e0      b	#8 ; -> 0x080207c4 ; branch_target=0x080207c4
080207ba  5a6d      ldr	r2, [r3, #84]
080207bc  0120      movs	r0, #1
080207be  42f02002  orr	r2, r2, #32
080207c2  5a65      str	r2, [r3, #84]
080207c4  0022      movs	r2, #0
080207c6  83f85020  strb.w	r2, [r3, #80]
080207ca  02b0      add	sp, #8
080207cc  70bd      pop	{r4, r5, r6, pc}
080207ce  0c68      ldr	r4, [r1]
080207d0  1432      adds	r2, #20
080207d2  0725      movs	r5, #7
080207d4  4fead45c  lsr.w	r12, r4, #23
080207d8  c4f30454  ubfx	r4, r4, #20, #5
080207dc  0cf0040c  and	r12, r12, #4
080207e0  a540      lsls	r5, r4
080207e2  52f80c00  ldr.w	r0, [r2, r12]
080207e6  20ea0500  bic.w	r0, r0, r5
080207ea  8d68      ldr	r5, [r1, #8]
080207ec  a540      lsls	r5, r4
080207ee  2843      orrs	r0, r5
080207f0  42f80c00  str.w	r0, [r2, r12]
080207f4  a04a      ldr	r2, [pc, #640] ; [0x08020a78] = 0x5c001000
080207f6  4c69      ldr	r4, [r1, #20]
080207f8  1268      ldr	r2, [r2]
080207fa  02f07042  and	r2, r2, #4026531840
080207fe  b2f1805f  cmp.w	r2, #268435456
08020802  1a68      ldr	r2, [r3]
08020804  d068      ldr	r0, [r2, #12]
08020806  75d0      beq	#234 ; -> 0x080208f4 ; branch_target=0x080208f4
08020808  10f0100f  tst.w	r0, #16
0802080c  d068      ldr	r0, [r2, #12]
0802080e  71d0      beq	#226 ; -> 0x080208f4 ; branch_target=0x080208f4
08020810  4008      lsrs	r0, r0, #1
08020812  00f00800  and	r0, r0, #8
08020816  8440      lsls	r4, r0
08020818  0d69      ldr	r5, [r1, #16]
0802081a  0868      ldr	r0, [r1]
0802081c  042d      cmp	r5, #4
0802081e  00f0c380  beq.w	#390 ; -> 0x080209a8 ; branch_target=0x080209a8
08020822  6032      adds	r2, #96
08020824  00f0f840  and	r0, r0, #2080374784
08020828  2043      orrs	r0, r4
0802082a  52f82540  ldr.w	r4, [r2, r5, lsl #2]
0802082e  04f00044  and	r4, r4, #2147483648
08020832  2043      orrs	r0, r4
08020834  42f82500  str.w	r0, [r2, r5, lsl #2]
08020838  1868      ldr	r0, [r3]
0802083a  4c7e      ldrb	r4, [r1, #25]
0802083c  0d69      ldr	r5, [r1, #16]
0802083e  6030      adds	r0, #96
08020840  a4f10104  sub.w	r4, r4, #1
08020844  50f82520  ldr.w	r2, [r0, r5, lsl #2]
08020848  b4fa84f4  clz	r4, r4
0802084c  22f00042  bic	r2, r2, #2147483648
08020850  6409      lsrs	r4, r4, #5
08020852  42eac472  orr.w	r2, r2, r4, lsl #31
08020856  40f82520  str.w	r2, [r0, r5, lsl #2]
0802085a  0a7e      ldrb	r2, [r1, #24]
0802085c  1d68      ldr	r5, [r3]
0802085e  a2f10102  sub.w	r2, r2, #1
08020862  0c69      ldr	r4, [r1, #16]
08020864  2869      ldr	r0, [r5, #16]
08020866  b2fa82f2  clz	r2, r2
0802086a  04f01f04  and	r4, r4, #31
0802086e  20f4f040  bic	r0, r0, #30720
08020872  5209      lsrs	r2, r2, #5
08020874  d202      lsls	r2, r2, #11
08020876  a240      lsls	r2, r4
08020878  0243      orrs	r2, r0
0802087a  2a61      str	r2, [r5, #16]
0802087c  1a68      ldr	r2, [r3]
0802087e  7ee7      b	#-260 ; -> 0x0802077e ; branch_target=0x0802077e
08020880  90faa0f0  rbit	r0, r0
08020884  0028      cmp	r0, #0
08020886  3ff457af  beq.w	#-338 ; -> 0x08020738 ; branch_target=0x08020738
0802088a  b0fa80f0  clz	r0, r0
0802088e  8240      lsls	r2, r0
08020890  52e7      b	#-348 ; -> 0x08020738 ; branch_target=0x08020738
08020892  1968      ldr	r1, [r3]
08020894  7948      ldr	r0, [pc, #484] ; [0x08020a7c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020896  8142      cmp	r1, r0
08020898  00f0d180  beq.w	#418 ; -> 0x08020a3e ; branch_target=0x08020a3e
0802089c  00f58070  add.w	r0, r0, #256
080208a0  8142      cmp	r1, r0
080208a2  00f0cc80  beq.w	#408 ; -> 0x08020a3e ; branch_target=0x08020a3e
080208a6  00f1c050  add.w	r0, r0, #402653184
080208aa  754d      ldr	r5, [pc, #468] ; [0x08020a80] = 0x58026000
080208ac  00f58440  add.w	r0, r0, #16896
080208b0  8468      ldr	r4, [r0, #8]
080208b2  ae68      ldr	r6, [r5, #8]
080208b4  f607      lsls	r6, r6, #31
080208b6  80d4      bmi	#-256 ; -> 0x080207ba ; branch_target=0x080207ba
080208b8  724e      ldr	r6, [pc, #456] ; [0x08020a84] = 0xcb840000 / f32_bits_interpretation=-17301504
080208ba  04f0e07c  and	r12, r4, #29360128
080208be  b242      cmp	r2, r6
080208c0  00f01b81  beq.w	#566 ; -> 0x08020afa ; branch_target=0x08020afa
080208c4  704e      ldr	r6, [pc, #448] ; [0x08020a88] = 0xc7520000 / f32_bits_interpretation=-53760
080208c6  b242      cmp	r2, r6
080208c8  00f00881  beq.w	#528 ; -> 0x08020adc ; branch_target=0x08020adc
080208cc  6f4d      ldr	r5, [pc, #444] ; [0x08020a8c] = 0xcfb80000 / f32_bits_interpretation=-6174015488
080208ce  aa42      cmp	r2, r5
080208d0  7ff471af  bne.w	#-286 ; -> 0x080207b6 ; branch_target=0x080207b6
080208d4  6202      lsls	r2, r4, #9
080208d6  3ff56eaf  bmi.w	#-292 ; -> 0x080207b6 ; branch_target=0x080207b6
080208da  694a      ldr	r2, [pc, #420] ; [0x08020a80] = 0x58026000
080208dc  9142      cmp	r1, r2
080208de  7ff46aaf  bne.w	#-300 ; -> 0x080207b6 ; branch_target=0x080207b6
080208e2  8268      ldr	r2, [r0, #8]
080208e4  22f0e072  bic	r2, r2, #29360128
080208e8  42ea0c02  orr.w	r2, r2, r12
080208ec  42f48002  orr	r2, r2, #4194304
080208f0  8260      str	r2, [r0, #8]
080208f2  60e7      b	#-320 ; -> 0x080207b6 ; branch_target=0x080207b6
080208f4  c0f38200  ubfx	r0, r0, #2, #3
080208f8  4000      lsls	r0, r0, #1
080208fa  8440      lsls	r4, r0
080208fc  8ce7      b	#-232 ; -> 0x08020818 ; branch_target=0x08020818
080208fe  0a68      ldr	r2, [r1]
08020900  1c68      ldr	r4, [r3]
08020902  c2f31300  ubfx	r0, r2, #0, #20
08020906  0028      cmp	r0, #0
08020908  6bd0      beq	#214 ; -> 0x080209e2 ; branch_target=0x080209e2
0802090a  92faa2f0  rbit	r0, r2
0802090e  0028      cmp	r0, #0
08020910  00f0c080  beq.w	#384 ; -> 0x08020a94 ; branch_target=0x08020a94
08020914  b0fa80f0  clz	r0, r0
08020918  0130      adds	r0, #1
0802091a  00f01f00  and	r0, r0, #31
0802091e  0928      cmp	r0, #9
08020920  40f2b880  bls.w	#368 ; -> 0x08020a94 ; branch_target=0x08020a94
08020924  92faa2f0  rbit	r0, r2
08020928  0028      cmp	r0, #0
0802092a  00f00d81  beq.w	#538 ; -> 0x08020b48 ; branch_target=0x08020b48
0802092e  b0fa80f0  clz	r0, r0
08020932  0130      adds	r0, #1
08020934  8006      lsls	r0, r0, #26
08020936  00f0f840  and	r0, r0, #2080374784
0802093a  92faa2f5  rbit	r5, r2
0802093e  002d      cmp	r5, #0
08020940  00f00081  beq.w	#512 ; -> 0x08020b44 ; branch_target=0x08020b44
08020944  b5fa85f5  clz	r5, r5
08020948  0126      movs	r6, #1
0802094a  0135      adds	r5, #1
0802094c  05f01f05  and	r5, r5, #31
08020950  06fa05f5  lsl.w	r5, r6, r5
08020954  2843      orrs	r0, r5
08020956  92faa2f2  rbit	r2, r2
0802095a  002a      cmp	r2, #0
0802095c  00f0f080  beq.w	#480 ; -> 0x08020b40 ; branch_target=0x08020b40
08020960  b2fa82f2  clz	r2, r2
08020964  6ff01d06  mvn	r6, #29
08020968  551c      adds	r5, r2, #1
0802096a  0322      movs	r2, #3
0802096c  05f01f05  and	r5, r5, #31
08020970  12fb0562  smlabb	r2, r2, r5, r6
08020974  1205      lsls	r2, r2, #20
08020976  42f00072  orr	r2, r2, #33554432
0802097a  0243      orrs	r2, r0
0802097c  1434      adds	r4, #20
0802097e  4ff0070c  mov.w	r12, #7
08020982  8e68      ldr	r6, [r1, #8]
08020984  d50d      lsrs	r5, r2, #23
08020986  c2f30452  ubfx	r2, r2, #20, #5
0802098a  05f00405  and	r5, r5, #4
0802098e  0cfa02fc  lsl.w	r12, r12, r2
08020992  06fa02f2  lsl.w	r2, r6, r2
08020996  6059      ldr	r0, [r4, r5]
08020998  20ea0c00  bic.w	r0, r0, r12
0802099c  0243      orrs	r2, r0
0802099e  6251      str	r2, [r4, r5]
080209a0  06e7      b	#-500 ; -> 0x080207b0 ; branch_target=0x080207b0
080209a2  0220      movs	r0, #2
080209a4  02b0      add	sp, #8
080209a6  70bd      pop	{r4, r5, r6, pc}
080209a8  156e      ldr	r5, [r2, #96]
080209aa  8406      lsls	r4, r0, #26
080209ac  05f0f845  and	r5, r5, #2080374784
080209b0  b5eb806f  cmp.w	r5, r0, lsl #26
080209b4  3bd0      beq	#118 ; -> 0x08020a2e ; branch_target=0x08020a2e
080209b6  506e      ldr	r0, [r2, #100]
080209b8  00f0f840  and	r0, r0, #2080374784
080209bc  a042      cmp	r0, r4
080209be  2ed0      beq	#92 ; -> 0x08020a1e ; branch_target=0x08020a1e
080209c0  906e      ldr	r0, [r2, #104]
080209c2  00f0f840  and	r0, r0, #2080374784
080209c6  a042      cmp	r0, r4
080209c8  21d0      beq	#66 ; -> 0x08020a0e ; branch_target=0x08020a0e
080209ca  d06e      ldr	r0, [r2, #108]
080209cc  00f0f840  and	r0, r0, #2080374784
080209d0  a042      cmp	r0, r4
080209d2  7ff4d4ae  bne.w	#-600 ; -> 0x0802077e ; branch_target=0x0802077e
080209d6  d06e      ldr	r0, [r2, #108]
080209d8  20f00040  bic	r0, r0, #2147483648
080209dc  d066      str	r0, [r2, #108]
080209de  1a68      ldr	r2, [r3]
080209e0  cde6      b	#-614 ; -> 0x0802077e ; branch_target=0x0802077e
080209e2  920e      lsrs	r2, r2, #26
080209e4  0132      adds	r2, #1
080209e6  02f01f0c  and	r12, r2, #31
080209ea  9206      lsls	r2, r2, #26
080209ec  02f0f845  and	r5, r2, #2080374784
080209f0  0122      movs	r2, #1
080209f2  bcf1090f  cmp.w	r12, #9
080209f6  02fa0cf2  lsl.w	r2, r2, r12
080209fa  45ea0200  orr.w	r0, r5, r2
080209fe  0ceb4c02  add.w	r2, r12, r12, lsl #1
08020a02  69d9      bls	#210 ; -> 0x08020ad8 ; branch_target=0x08020ad8
08020a04  1e3a      subs	r2, #30
08020a06  1205      lsls	r2, r2, #20
08020a08  42f00072  orr	r2, r2, #33554432
08020a0c  b5e7      b	#-150 ; -> 0x0802097a ; branch_target=0x0802097a
08020a0e  906e      ldr	r0, [r2, #104]
08020a10  20f00040  bic	r0, r0, #2147483648
08020a14  9066      str	r0, [r2, #104]
08020a16  0c68      ldr	r4, [r1]
08020a18  1a68      ldr	r2, [r3]
08020a1a  a406      lsls	r4, r4, #26
08020a1c  d5e7      b	#-86 ; -> 0x080209ca ; branch_target=0x080209ca
08020a1e  506e      ldr	r0, [r2, #100]
08020a20  20f00040  bic	r0, r0, #2147483648
08020a24  5066      str	r0, [r2, #100]
08020a26  0c68      ldr	r4, [r1]
08020a28  1a68      ldr	r2, [r3]
08020a2a  a406      lsls	r4, r4, #26
08020a2c  c8e7      b	#-112 ; -> 0x080209c0 ; branch_target=0x080209c0
08020a2e  106e      ldr	r0, [r2, #96]
08020a30  20f00040  bic	r0, r0, #2147483648
08020a34  1066      str	r0, [r2, #96]
08020a36  0c68      ldr	r4, [r1]
08020a38  1a68      ldr	r2, [r3]
08020a3a  a406      lsls	r4, r4, #26
08020a3c  bbe7      b	#-138 ; -> 0x080209b6 ; branch_target=0x080209b6
08020a3e  0f4d      ldr	r5, [pc, #60] ; [0x08020a7c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020a40  1348      ldr	r0, [pc, #76] ; [0x08020a90] = 0x40022300 / f32_bits_interpretation=2.03338623
08020a42  05f58075  add.w	r5, r5, #256
08020a46  8468      ldr	r4, [r0, #8]
08020a48  55f8f86c  ldr	r6, [r5, #-248]
08020a4c  ad68      ldr	r5, [r5, #8]
08020a4e  ed07      lsls	r5, r5, #31
08020a50  3ff5b3ae  bmi.w	#-666 ; -> 0x080207ba ; branch_target=0x080207ba
08020a54  f507      lsls	r5, r6, #31
08020a56  3ff5b0ae  bmi.w	#-672 ; -> 0x080207ba ; branch_target=0x080207ba
08020a5a  0a4d      ldr	r5, [pc, #40] ; [0x08020a84] = 0xcb840000 / f32_bits_interpretation=-17301504
08020a5c  aa42      cmp	r2, r5
08020a5e  3ff4aaae  beq.w	#-684 ; -> 0x080207b6 ; branch_target=0x080207b6
08020a62  094d      ldr	r5, [pc, #36] ; [0x08020a88] = 0xc7520000 / f32_bits_interpretation=-53760
08020a64  aa42      cmp	r2, r5
08020a66  3ff4a6ae  beq.w	#-692 ; -> 0x080207b6 ; branch_target=0x080207b6
08020a6a  04f0e07c  and	r12, r4, #29360128
08020a6e  2de7      b	#-422 ; -> 0x080208cc ; branch_target=0x080208cc
08020a94  92faa2f0  rbit	r0, r2
08020a98  0028      cmp	r0, #0
08020a9a  5dd0      beq	#186 ; -> 0x08020b58 ; branch_target=0x08020b58
08020a9c  b0fa80f0  clz	r0, r0
08020aa0  0130      adds	r0, #1
08020aa2  8006      lsls	r0, r0, #26
08020aa4  00f0f840  and	r0, r0, #2080374784
08020aa8  92faa2f5  rbit	r5, r2
08020aac  002d      cmp	r5, #0
08020aae  51d0      beq	#162 ; -> 0x08020b54 ; branch_target=0x08020b54
08020ab0  b5fa85f5  clz	r5, r5
08020ab4  0126      movs	r6, #1
08020ab6  0135      adds	r5, #1
08020ab8  05f01f05  and	r5, r5, #31
08020abc  06fa05f5  lsl.w	r5, r6, r5
08020ac0  2843      orrs	r0, r5
08020ac2  92faa2f2  rbit	r2, r2
08020ac6  002a      cmp	r2, #0
08020ac8  41d0      beq	#130 ; -> 0x08020b4e ; branch_target=0x08020b4e
08020aca  b2fa82f2  clz	r2, r2
08020ace  0132      adds	r2, #1
08020ad0  02f01f02  and	r2, r2, #31
08020ad4  02eb4202  add.w	r2, r2, r2, lsl #1
08020ad8  1205      lsls	r2, r2, #20
08020ada  4ee7      b	#-356 ; -> 0x0802097a ; branch_target=0x0802097a
08020adc  e401      lsls	r4, r4, #7
08020ade  3ff56aae  bmi.w	#-812 ; -> 0x080207b6 ; branch_target=0x080207b6
08020ae2  a942      cmp	r1, r5
08020ae4  7ff467ae  bne.w	#-818 ; -> 0x080207b6 ; branch_target=0x080207b6
08020ae8  8268      ldr	r2, [r0, #8]
08020aea  22f0e072  bic	r2, r2, #29360128
08020aee  42ea0c02  orr.w	r2, r2, r12
08020af2  42f08072  orr	r2, r2, #16777216
08020af6  8260      str	r2, [r0, #8]
08020af8  5de6      b	#-838 ; -> 0x080207b6 ; branch_target=0x080207b6
08020afa  2602      lsls	r6, r4, #8
08020afc  3ff55bae  bmi.w	#-842 ; -> 0x080207b6 ; branch_target=0x080207b6
08020b00  a942      cmp	r1, r5
08020b02  7ff458ae  bne.w	#-848 ; -> 0x080207b6 ; branch_target=0x080207b6
08020b06  8268      ldr	r2, [r0, #8]
08020b08  1549      ldr	r1, [pc, #84] ; [0x08020b60] = 0x20000014
08020b0a  22f0e072  bic	r2, r2, #29360128
08020b0e  42ea0c02  orr.w	r2, r2, r12
08020b12  42f40002  orr	r2, r2, #8388608
08020b16  8260      str	r2, [r0, #8]
08020b18  0a68      ldr	r2, [r1]
08020b1a  1249      ldr	r1, [pc, #72] ; [0x08020b64] = 0x053e2d63
08020b1c  9209      lsrs	r2, r2, #6
08020b1e  a1fb0212  umull	r1, r2, r1, r2
08020b22  9209      lsrs	r2, r2, #6
08020b24  0132      adds	r2, #1
08020b26  5200      lsls	r2, r2, #1
08020b28  0192      str	r2, [sp, #4]
08020b2a  019a      ldr	r2, [sp, #4]
08020b2c  002a      cmp	r2, #0
08020b2e  3ff442ae  beq.w	#-892 ; -> 0x080207b6 ; branch_target=0x080207b6
08020b32  019a      ldr	r2, [sp, #4]
08020b34  013a      subs	r2, #1
08020b36  0192      str	r2, [sp, #4]
08020b38  019a      ldr	r2, [sp, #4]
08020b3a  002a      cmp	r2, #0
08020b3c  f9d1      bne	#-14 ; -> 0x08020b32 ; branch_target=0x08020b32
08020b3e  3ae6      b	#-908 ; -> 0x080207b6 ; branch_target=0x080207b6
08020b40  094a      ldr	r2, [pc, #36] ; [0x08020b68] = 0xfe500000
08020b42  1ae7      b	#-460 ; -> 0x0802097a ; branch_target=0x0802097a
08020b44  0225      movs	r5, #2
08020b46  05e7      b	#-502 ; -> 0x08020954 ; branch_target=0x08020954
08020b48  4ff08060  mov.w	r0, #67108864
08020b4c  f5e6      b	#-534 ; -> 0x0802093a ; branch_target=0x0802093a
08020b4e  4ff44012  mov.w	r2, #3145728
08020b52  12e7      b	#-476 ; -> 0x0802097a ; branch_target=0x0802097a
08020b54  0225      movs	r5, #2
08020b56  b3e7      b	#-154 ; -> 0x08020ac0 ; branch_target=0x08020ac0
08020b58  4ff08060  mov.w	r0, #67108864
08020b5c  a4e7      b	#-184 ; -> 0x08020aa8 ; branch_target=0x08020aa8
