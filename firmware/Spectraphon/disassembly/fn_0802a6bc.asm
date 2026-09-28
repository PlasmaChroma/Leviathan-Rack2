; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802a6bc  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802a6c0  0b78      ldrb	r3, [r1]
0802a6c2  85b0      sub	sp, #20
0802a6c4  0d46      mov	r5, r1
0802a6c6  0446      mov	r4, r0
0802a6c8  2f2b      cmp	r3, #47
0802a6ca  0768      ldr	r7, [r0]
0802a6cc  03d1      bne	#6 ; -> 0x0802a6d6 ; branch_target=0x0802a6d6
0802a6ce  15f8013f  ldrb	r3, [r5, #1]!
0802a6d2  2f2b      cmp	r3, #47
0802a6d4  fbd0      beq	#-10 ; -> 0x0802a6ce ; branch_target=0x0802a6ce
0802a6d6  5c2b      cmp	r3, #92
0802a6d8  f9d0      beq	#-14 ; -> 0x0802a6ce ; branch_target=0x0802a6ce
0802a6da  0023      movs	r3, #0
0802a6dc  a360      str	r3, [r4, #8]
0802a6de  2b78      ldrb	r3, [r5]
0802a6e0  1f2b      cmp	r3, #31
0802a6e2  40f26a81  bls.w	#724 ; -> 0x0802a9ba ; branch_target=0x0802a9ba
0802a6e6  bb46      mov	r11, r7
0802a6e8  04f12406  add.w	r6, r4, #36
0802a6ec  4ff0203a  mov.w	r10, #538976288
0802a6f0  2f46      mov	r7, r5
0802a6f2  2023      movs	r3, #32
0802a6f4  42f22002  movw	r2, #8224
0802a6f8  c4f824a0  str.w	r10, [r4, #36]
0802a6fc  c4f828a0  str.w	r10, [r4, #40]
0802a700  a285      strh	r2, [r4, #44]
0802a702  84f82e30  strb.w	r3, [r4, #46]
0802a706  3b78      ldrb	r3, [r7]
0802a708  202b      cmp	r3, #32
0802a70a  5fd9      bls	#190 ; -> 0x0802a7cc ; branch_target=0x0802a7cc
0802a70c  be46      mov	lr, r7
0802a70e  0025      movs	r5, #0
0802a710  4ff0080c  mov.w	r12, #8
0802a714  0120      movs	r0, #1
0802a716  2f2b      cmp	r3, #47
0802a718  66d0      beq	#204 ; -> 0x0802a7e8 ; branch_target=0x0802a7e8
0802a71a  5c2b      cmp	r3, #92
0802a71c  64d0      beq	#200 ; -> 0x0802a7e8 ; branch_target=0x0802a7e8
0802a71e  2e2b      cmp	r3, #46
0802a720  01d0      beq	#2 ; -> 0x0802a726 ; branch_target=0x0802a726
0802a722  6545      cmp	r5, r12
0802a724  45d3      blo	#138 ; -> 0x0802a7b2 ; branch_target=0x0802a7b2
0802a726  bcf10b0f  cmp.w	r12, #11
0802a72a  4fd0      beq	#158 ; -> 0x0802a7cc ; branch_target=0x0802a7cc
0802a72c  2e2b      cmp	r3, #46
0802a72e  4dd1      bne	#154 ; -> 0x0802a7cc ; branch_target=0x0802a7cc
0802a730  0825      movs	r5, #8
0802a732  4ff00b0c  mov.w	r12, #11
0802a736  1ef8013f  ldrb	r3, [lr, #1]!
0802a73a  0130      adds	r0, #1
0802a73c  202b      cmp	r3, #32
0802a73e  ead8      bhi	#-44 ; -> 0x0802a716 ; branch_target=0x0802a716
0802a740  0744      add	r7, r0
0802a742  94f82420  ldrb.w	r2, [r4, #36]
0802a746  e52a      cmp	r2, #229
0802a748  02d1      bne	#4 ; -> 0x0802a750 ; branch_target=0x0802a750
0802a74a  0522      movs	r2, #5
0802a74c  84f82420  strb.w	r2, [r4, #36]
0802a750  202b      cmp	r3, #32
0802a752  2046      mov	r0, r4
0802a754  d4f80080  ldr.w	r8, [r4]
0802a758  8cbf      ite	hi
0802a75a  0023      movhi	r3, #0
0802a75c  0123      movls	r3, #1
0802a75e  9b00      lsls	r3, r3, #2
0802a760  84f82f30  strb.w	r3, [r4, #47]
0802a764  fff722ff  bl	#-444 ; -> 0x0802a5ac ; branch_target=0x0802a5ac
0802a768  0246      mov	r2, r0
0802a76a  0028      cmp	r0, #0
0802a76c  40f0cc80  bne.w	#408 ; -> 0x0802a908 ; branch_target=0x0802a908
0802a770  e569      ldr	r5, [r4, #28]
0802a772  08f13009  add.w	r9, r8, #48
0802a776  cde900b6  strd	r11, r6, [sp]
0802a77a  bb46      mov	r11, r7
0802a77c  d8f82c70  ldr.w	r7, [r8, #44]
0802a780  af42      cmp	r7, r5
0802a782  46d0      beq	#140 ; -> 0x0802a812 ; branch_target=0x0802a812
0802a784  98f80330  ldrb.w	r3, [r8, #3]
0802a788  98f80100  ldrb.w	r0, [r8, #1]
0802a78c  002b      cmp	r3, #0
0802a78e  40f0c180  bne.w	#386 ; -> 0x0802a914 ; branch_target=0x0802a914
0802a792  0123      movs	r3, #1
0802a794  2a46      mov	r2, r5
0802a796  4946      mov	r1, r9
0802a798  fff7aef8  bl	#-3748 ; -> 0x080298f8 ; branch_target=0x080298f8
0802a79c  0028      cmp	r0, #0
0802a79e  36d0      beq	#108 ; -> 0x0802a80e ; branch_target=0x0802a80e
0802a7a0  0122      movs	r2, #1
0802a7a2  4ff0ff33  mov.w	r3, #4294967295
0802a7a6  1046      mov	r0, r2
0802a7a8  c8f82c30  str.w	r3, [r8, #44]
0802a7ac  05b0      add	sp, #20
0802a7ae  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802a7b2  1a06      lsls	r2, r3, #24
0802a7b4  02d5      bpl	#4 ; -> 0x0802a7bc ; branch_target=0x0802a7bc
0802a7b6  803b      subs	r3, #128
0802a7b8  894a      ldr	r2, [pc, #548] ; [0x0802a9e0] = 0x080365f4
0802a7ba  d35c      ldrb	r3, [r2, r3]
0802a7bc  2222      movs	r2, #34
0802a7be  8949      ldr	r1, [pc, #548] ; [0x0802a9e4] = 0x080365e4
0802a7c0  02e0      b	#4 ; -> 0x0802a7c8 ; branch_target=0x0802a7c8
0802a7c2  11f8012f  ldrb	r2, [r1, #1]!
0802a7c6  32b1      cbz	r2, #12 ; -> 0x0802a7d6 ; branch_target=0x0802a7d6
0802a7c8  9342      cmp	r3, r2
0802a7ca  fad1      bne	#-12 ; -> 0x0802a7c2 ; branch_target=0x0802a7c2
0802a7cc  0622      movs	r2, #6
0802a7ce  1046      mov	r0, r2
0802a7d0  05b0      add	sp, #20
0802a7d2  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802a7d6  a3f16102  sub.w	r2, r3, #97
0802a7da  192a      cmp	r2, #25
0802a7dc  01d8      bhi	#2 ; -> 0x0802a7e2 ; branch_target=0x0802a7e2
0802a7de  203b      subs	r3, #32
0802a7e0  dbb2      uxtb	r3, r3
0802a7e2  7355      strb	r3, [r6, r5]
0802a7e4  0135      adds	r5, #1
0802a7e6  a6e7      b	#-180 ; -> 0x0802a736 ; branch_target=0x0802a736
0802a7e8  3a5c      ldrb	r2, [r7, r0]
0802a7ea  3918      adds	r1, r7, r0
0802a7ec  2f2a      cmp	r2, #47
0802a7ee  02d0      beq	#4 ; -> 0x0802a7f6 ; branch_target=0x0802a7f6
0802a7f0  5c2a      cmp	r2, #92
0802a7f2  40f0ed80  bne.w	#474 ; -> 0x0802a9d0 ; branch_target=0x0802a9d0
0802a7f6  421c      adds	r2, r0, #1
0802a7f8  3a44      add	r2, r7
0802a7fa  1746      mov	r7, r2
0802a7fc  12f8011b  ldrb	r1, [r2], #1
0802a800  2f29      cmp	r1, #47
0802a802  fad0      beq	#-12 ; -> 0x0802a7fa ; branch_target=0x0802a7fa
0802a804  5c29      cmp	r1, #92
0802a806  f8d0      beq	#-16 ; -> 0x0802a7fa ; branch_target=0x0802a7fa
0802a808  002d      cmp	r5, #0
0802a80a  9ad1      bne	#-204 ; -> 0x0802a742 ; branch_target=0x0802a742
0802a80c  dee7      b	#-68 ; -> 0x0802a7cc ; branch_target=0x0802a7cc
0802a80e  c8f82c50  str.w	r5, [r8, #44]
0802a812  236a      ldr	r3, [r4, #32]
0802a814  1a78      ldrb	r2, [r3]
0802a816  002a      cmp	r2, #0
0802a818  75d0      beq	#234 ; -> 0x0802a906 ; branch_target=0x0802a906
0802a81a  d97a      ldrb	r1, [r3, #11]
0802a81c  01f03f02  and	r2, r1, #63
0802a820  a271      strb	r2, [r4, #6]
0802a822  da7a      ldrb	r2, [r3, #11]
0802a824  12f00802  ands	r2, r2, #8
0802a828  1bd0      beq	#54 ; -> 0x0802a862 ; branch_target=0x0802a862
0802a82a  6669      ldr	r6, [r4, #20]
0802a82c  e569      ldr	r5, [r4, #28]
0802a82e  2768      ldr	r7, [r4]
0802a830  2036      adds	r6, #32
0802a832  002d      cmp	r5, #0
0802a834  67d0      beq	#206 ; -> 0x0802a906 ; branch_target=0x0802a906
0802a836  b6f5001f  cmp.w	r6, #2097152
0802a83a  64d2      bhs	#200 ; -> 0x0802a906 ; branch_target=0x0802a906
0802a83c  c6f30803  ubfx	r3, r6, #0, #9
0802a840  53b9      cbnz	r3, #20 ; -> 0x0802a858 ; branch_target=0x0802a858
0802a842  0135      adds	r5, #1
0802a844  a169      ldr	r1, [r4, #24]
0802a846  e561      str	r5, [r4, #28]
0802a848  0029      cmp	r1, #0
0802a84a  40f08780  bne.w	#270 ; -> 0x0802a95c ; branch_target=0x0802a95c
0802a84e  3a89      ldrh	r2, [r7, #8]
0802a850  b2eb561f  cmp.w	r2, r6, lsr #5
0802a854  40f29f80  bls.w	#318 ; -> 0x0802a996 ; branch_target=0x0802a996
0802a858  3037      adds	r7, #48
0802a85a  6661      str	r6, [r4, #20]
0802a85c  1f44      add	r7, r3
0802a85e  2762      str	r7, [r4, #32]
0802a860  8ce7      b	#-232 ; -> 0x0802a77c ; branch_target=0x0802a77c
0802a862  94f82450  ldrb.w	r5, [r4, #36]
0802a866  1878      ldrb	r0, [r3]
0802a868  8542      cmp	r5, r0
0802a86a  ded1      bne	#-68 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a86c  5d78      ldrb	r5, [r3, #1]
0802a86e  94f82500  ldrb.w	r0, [r4, #37]
0802a872  8542      cmp	r5, r0
0802a874  d9d1      bne	#-78 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a876  94f82650  ldrb.w	r5, [r4, #38]
0802a87a  9878      ldrb	r0, [r3, #2]
0802a87c  8542      cmp	r5, r0
0802a87e  d4d1      bne	#-88 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a880  94f82750  ldrb.w	r5, [r4, #39]
0802a884  d878      ldrb	r0, [r3, #3]
0802a886  8542      cmp	r5, r0
0802a888  cfd1      bne	#-98 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a88a  1d79      ldrb	r5, [r3, #4]
0802a88c  94f82800  ldrb.w	r0, [r4, #40]
0802a890  8542      cmp	r5, r0
0802a892  cad1      bne	#-108 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a894  5d79      ldrb	r5, [r3, #5]
0802a896  94f82900  ldrb.w	r0, [r4, #41]
0802a89a  8542      cmp	r5, r0
0802a89c  c5d1      bne	#-118 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a89e  9d79      ldrb	r5, [r3, #6]
0802a8a0  94f82a00  ldrb.w	r0, [r4, #42]
0802a8a4  8542      cmp	r5, r0
0802a8a6  c0d1      bne	#-128 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a8a8  dd79      ldrb	r5, [r3, #7]
0802a8aa  94f82b00  ldrb.w	r0, [r4, #43]
0802a8ae  8542      cmp	r5, r0
0802a8b0  bbd1      bne	#-138 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a8b2  1d7a      ldrb	r5, [r3, #8]
0802a8b4  94f82c00  ldrb.w	r0, [r4, #44]
0802a8b8  8542      cmp	r5, r0
0802a8ba  b6d1      bne	#-148 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a8bc  5d7a      ldrb	r5, [r3, #9]
0802a8be  94f82d00  ldrb.w	r0, [r4, #45]
0802a8c2  8542      cmp	r5, r0
0802a8c4  b1d1      bne	#-158 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a8c6  987a      ldrb	r0, [r3, #10]
0802a8c8  94f82e30  ldrb.w	r3, [r4, #46]
0802a8cc  9842      cmp	r0, r3
0802a8ce  acd1      bne	#-168 ; -> 0x0802a82a ; branch_target=0x0802a82a
0802a8d0  94f82f30  ldrb.w	r3, [r4, #47]
0802a8d4  5f46      mov	r7, r11
0802a8d6  019e      ldr	r6, [sp, #4]
0802a8d8  5807      lsls	r0, r3, #29
0802a8da  ddf800b0  ldr.w	r11, [sp]
0802a8de  3ff576af  bmi.w	#-276 ; -> 0x0802a7ce ; branch_target=0x0802a7ce
0802a8e2  cb06      lsls	r3, r1, #27
0802a8e4  53d5      bpl	#166 ; -> 0x0802a98e ; branch_target=0x0802a98e
0802a8e6  6269      ldr	r2, [r4, #20]
0802a8e8  0bf13003  add.w	r3, r11, #48
0802a8ec  9bf80010  ldrb.w	r1, [r11]
0802a8f0  c2f30802  ubfx	r2, r2, #0, #9
0802a8f4  0329      cmp	r1, #3
0802a8f6  1344      add	r3, r2
0802a8f8  5a8b      ldrh	r2, [r3, #26]
0802a8fa  02d1      bne	#4 ; -> 0x0802a902 ; branch_target=0x0802a902
0802a8fc  9b8a      ldrh	r3, [r3, #20]
0802a8fe  42ea0342  orr.w	r2, r2, r3, lsl #16
0802a902  a260      str	r2, [r4, #8]
0802a904  f5e6      b	#-534 ; -> 0x0802a6f2 ; branch_target=0x0802a6f2
0802a906  0422      movs	r2, #4
0802a908  042a      cmp	r2, #4
0802a90a  94f82f30  ldrb.w	r3, [r4, #47]
0802a90e  7ff45eaf  bne.w	#-324 ; -> 0x0802a7ce ; branch_target=0x0802a7ce
0802a912  3ae0      b	#116 ; -> 0x0802a98a ; branch_target=0x0802a98a
0802a914  0123      movs	r3, #1
0802a916  3a46      mov	r2, r7
0802a918  4946      mov	r1, r9
0802a91a  fef7fbff  bl	#-4106 ; -> 0x08029914 ; branch_target=0x08029914
0802a91e  c0bb      cbnz	r0, #112 ; -> 0x0802a992 ; branch_target=0x0802a992
0802a920  d8f82020  ldr.w	r2, [r8, #32]
0802a924  d8f81830  ldr.w	r3, [r8, #24]
0802a928  ba1a      subs	r2, r7, r2
0802a92a  88f80300  strb.w	r0, [r8, #3]
0802a92e  9a42      cmp	r2, r3
0802a930  11d2      bhs	#34 ; -> 0x0802a956 ; branch_target=0x0802a956
0802a932  98f80260  ldrb.w	r6, [r8, #2]
0802a936  012e      cmp	r6, #1
0802a938  02d8      bhi	#4 ; -> 0x0802a940 ; branch_target=0x0802a940
0802a93a  0ce0      b	#24 ; -> 0x0802a956 ; branch_target=0x0802a956
0802a93c  d8f81830  ldr.w	r3, [r8, #24]
0802a940  1f44      add	r7, r3
0802a942  013e      subs	r6, #1
0802a944  0123      movs	r3, #1
0802a946  4946      mov	r1, r9
0802a948  3a46      mov	r2, r7
0802a94a  98f80100  ldrb.w	r0, [r8, #1]
0802a94e  fef7e1ff  bl	#-4158 ; -> 0x08029914 ; branch_target=0x08029914
0802a952  012e      cmp	r6, #1
0802a954  f2d1      bne	#-28 ; -> 0x0802a93c ; branch_target=0x0802a93c
0802a956  98f80100  ldrb.w	r0, [r8, #1]
0802a95a  1ae7      b	#-460 ; -> 0x0802a792 ; branch_target=0x0802a792
0802a95c  7a89      ldrh	r2, [r7, #10]
0802a95e  013a      subs	r2, #1
0802a960  12ea5622  ands.w	r2, r2, r6, lsr #9
0802a964  7ff478af  bne.w	#-272 ; -> 0x0802a858 ; branch_target=0x0802a858
0802a968  3846      mov	r0, r7
0802a96a  cde90232  strd	r3, r2, [sp, #8]
0802a96e  fff74dfc  bl	#-1894 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802a972  0128      cmp	r0, #1
0802a974  1fd9      bls	#62 ; -> 0x0802a9b6 ; branch_target=0x0802a9b6
0802a976  451c      adds	r5, r0, #1
0802a978  0bd0      beq	#22 ; -> 0x0802a992 ; branch_target=0x0802a992
0802a97a  7969      ldr	r1, [r7, #20]
0802a97c  029b      ldr	r3, [sp, #8]
0802a97e  8842      cmp	r0, r1
0802a980  0dd3      blo	#26 ; -> 0x0802a99e ; branch_target=0x0802a99e
0802a982  039a      ldr	r2, [sp, #12]
0802a984  94f82f30  ldrb.w	r3, [r4, #47]
0802a988  e261      str	r2, [r4, #28]
0802a98a  5a07      lsls	r2, r3, #29
0802a98c  25d4      bmi	#74 ; -> 0x0802a9da ; branch_target=0x0802a9da
0802a98e  0522      movs	r2, #5
0802a990  1de7      b	#-454 ; -> 0x0802a7ce ; branch_target=0x0802a7ce
0802a992  0122      movs	r2, #1
0802a994  b8e7      b	#-144 ; -> 0x0802a908 ; branch_target=0x0802a908
0802a996  94f82f30  ldrb.w	r3, [r4, #47]
0802a99a  e161      str	r1, [r4, #28]
0802a99c  f5e7      b	#-22 ; -> 0x0802a98a ; branch_target=0x0802a98a
0802a99e  a061      str	r0, [r4, #24]
0802a9a0  0238      subs	r0, #2
0802a9a2  7a69      ldr	r2, [r7, #20]
0802a9a4  023a      subs	r2, #2
0802a9a6  9042      cmp	r0, r2
0802a9a8  10d2      bhs	#32 ; -> 0x0802a9cc ; branch_target=0x0802a9cc
0802a9aa  7a89      ldrh	r2, [r7, #10]
0802a9ac  bd6a      ldr	r5, [r7, #40]
0802a9ae  00fb0255  mla	r5, r0, r2, r5
0802a9b2  e561      str	r5, [r4, #28]
0802a9b4  50e7      b	#-352 ; -> 0x0802a858 ; branch_target=0x0802a858
0802a9b6  0222      movs	r2, #2
0802a9b8  a6e7      b	#-180 ; -> 0x0802a908 ; branch_target=0x0802a908
0802a9ba  8023      movs	r3, #128
0802a9bc  2046      mov	r0, r4
0802a9be  84f82f30  strb.w	r3, [r4, #47]
0802a9c2  05b0      add	sp, #20
0802a9c4  bde8f04f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802a9c8  fff7f0bd  b.w	#-1056 ; -> 0x0802a5ac ; branch_target=0x0802a5ac
0802a9cc  1d46      mov	r5, r3
0802a9ce  f0e7      b	#-32 ; -> 0x0802a9b2 ; branch_target=0x0802a9b2
0802a9d0  0f46      mov	r7, r1
0802a9d2  002d      cmp	r5, #0
0802a9d4  7ff4b5ae  bne.w	#-662 ; -> 0x0802a742 ; branch_target=0x0802a742
0802a9d8  f8e6      b	#-528 ; -> 0x0802a7cc ; branch_target=0x0802a7cc
0802a9da  0422      movs	r2, #4
0802a9dc  f7e6      b	#-530 ; -> 0x0802a7ce ; branch_target=0x0802a7ce
