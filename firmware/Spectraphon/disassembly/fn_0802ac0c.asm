; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802ac0c  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802ac10  85b0      sub	sp, #20
0802ac12  0446      mov	r4, r0
0802ac14  0668      ldr	r6, [r0]
0802ac16  0090      str	r0, [sp]
0802ac18  fff7c8fc  bl	#-1648 ; -> 0x0802a5ac ; branch_target=0x0802a5ac
0802ac1c  0028      cmp	r0, #0
0802ac1e  40f0a280  bne.w	#324 ; -> 0x0802ad66 ; branch_target=0x0802ad66
0802ac22  d4f81ca0  ldr.w	r10, [r4, #28]
0802ac26  06f1300b  add.w	r11, r6, #48
0802ac2a  f56a      ldr	r5, [r6, #44]
0802ac2c  5545      cmp	r5, r10
0802ac2e  13d0      beq	#38 ; -> 0x0802ac58 ; branch_target=0x0802ac58
0802ac30  f378      ldrb	r3, [r6, #3]
0802ac32  7078      ldrb	r0, [r6, #1]
0802ac34  002b      cmp	r3, #0
0802ac36  5fd1      bne	#190 ; -> 0x0802acf8 ; branch_target=0x0802acf8
0802ac38  0123      movs	r3, #1
0802ac3a  5246      mov	r2, r10
0802ac3c  5946      mov	r1, r11
0802ac3e  fef75bfe  bl	#-4938 ; -> 0x080298f8 ; branch_target=0x080298f8
0802ac42  38b1      cbz	r0, #14 ; -> 0x0802ac54 ; branch_target=0x0802ac54
0802ac44  4ff0ff33  mov.w	r3, #4294967295
0802ac48  f362      str	r3, [r6, #44]
0802ac4a  0125      movs	r5, #1
0802ac4c  2846      mov	r0, r5
0802ac4e  05b0      add	sp, #20
0802ac50  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802ac54  c6f82ca0  str.w	r10, [r6, #44]
0802ac58  009a      ldr	r2, [sp]
0802ac5a  136a      ldr	r3, [r2, #32]
0802ac5c  1b78      ldrb	r3, [r3]
0802ac5e  e52b      cmp	r3, #229
0802ac60  00d0      beq	#0 ; -> 0x0802ac64 ; branch_target=0x0802ac64
0802ac62  3bbb      cbnz	r3, #78 ; -> 0x0802acb4 ; branch_target=0x0802acb4
0802ac64  009c      ldr	r4, [sp]
0802ac66  3046      mov	r0, r6
0802ac68  e169      ldr	r1, [r4, #28]
0802ac6a  fff7a7f8  bl	#-3762 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802ac6e  0546      mov	r5, r0
0802ac70  0028      cmp	r0, #0
0802ac72  ebd1      bne	#-42 ; -> 0x0802ac4c ; branch_target=0x0802ac4c
0802ac74  0146      mov	r1, r0
0802ac76  2022      movs	r2, #32
0802ac78  206a      ldr	r0, [r4, #32]
0802ac7a  0bf002fc  bl	#47108 ; -> 0x08036482 ; branch_target=0x08036482
0802ac7e  236a      ldr	r3, [r4, #32]
0802ac80  04f12502  add.w	r2, r4, #37
0802ac84  2146      mov	r1, r4
0802ac86  9a1a      subs	r2, r3, r2
0802ac88  022a      cmp	r2, #2
0802ac8a  40f2dc80  bls.w	#440 ; -> 0x0802ae46 ; branch_target=0x0802ae46
0802ac8e  626a      ldr	r2, [r4, #36]
0802ac90  1a60      str	r2, [r3]
0802ac92  a26a      ldr	r2, [r4, #40]
0802ac94  5a60      str	r2, [r3, #4]
0802ac96  91f82c20  ldrb.w	r2, [r1, #44]
0802ac9a  2846      mov	r0, r5
0802ac9c  1a72      strb	r2, [r3, #8]
0802ac9e  91f82d20  ldrb.w	r2, [r1, #45]
0802aca2  5a72      strb	r2, [r3, #9]
0802aca4  0122      movs	r2, #1
0802aca6  91f82e10  ldrb.w	r1, [r1, #46]
0802acaa  9972      strb	r1, [r3, #10]
0802acac  f270      strb	r2, [r6, #3]
0802acae  05b0      add	sp, #20
0802acb0  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802acb4  5569      ldr	r5, [r2, #20]
0802acb6  1768      ldr	r7, [r2]
0802acb8  2035      adds	r5, #32
0802acba  d2f81ca0  ldr.w	r10, [r2, #28]
0802acbe  b5f5001f  cmp.w	r5, #2097152
0802acc2  66d2      bhs	#204 ; -> 0x0802ad92 ; branch_target=0x0802ad92
0802acc4  baf1000f  cmp.w	r10, #0
0802acc8  63d0      beq	#198 ; -> 0x0802ad92 ; branch_target=0x0802ad92
0802acca  c5f30809  ubfx	r9, r5, #0, #9
0802acce  b9f1000f  cmp.w	r9, #0
0802acd2  09d1      bne	#18 ; -> 0x0802ace8 ; branch_target=0x0802ace8
0802acd4  0af1010a  add.w	r10, r10, #1
0802acd8  9169      ldr	r1, [r2, #24]
0802acda  c2f81ca0  str.w	r10, [r2, #28]
0802acde  e9b9      cbnz	r1, #58 ; -> 0x0802ad1c ; branch_target=0x0802ad1c
0802ace0  3b89      ldrh	r3, [r7, #8]
0802ace2  b3eb551f  cmp.w	r3, r5, lsr #5
0802ace6  52d9      bls	#164 ; -> 0x0802ad8e ; branch_target=0x0802ad8e
0802ace8  07f13008  add.w	r8, r7, #48
0802acec  009a      ldr	r2, [sp]
0802acee  08eb0903  add.w	r3, r8, r9
0802acf2  5561      str	r5, [r2, #20]
0802acf4  1362      str	r3, [r2, #32]
0802acf6  98e7      b	#-208 ; -> 0x0802ac2a ; branch_target=0x0802ac2a
0802acf8  0123      movs	r3, #1
0802acfa  2a46      mov	r2, r5
0802acfc  5946      mov	r1, r11
0802acfe  fef709fe  bl	#-5102 ; -> 0x08029914 ; branch_target=0x08029914
0802ad02  0028      cmp	r0, #0
0802ad04  a1d1      bne	#-190 ; -> 0x0802ac4a ; branch_target=0x0802ac4a
0802ad06  336a      ldr	r3, [r6, #32]
0802ad08  f070      strb	r0, [r6, #3]
0802ad0a  ea1a      subs	r2, r5, r3
0802ad0c  b369      ldr	r3, [r6, #24]
0802ad0e  9a42      cmp	r2, r3
0802ad10  02d2      bhs	#4 ; -> 0x0802ad18 ; branch_target=0x0802ad18
0802ad12  b778      ldrb	r7, [r6, #2]
0802ad14  012f      cmp	r7, #1
0802ad16  2ed8      bhi	#92 ; -> 0x0802ad76 ; branch_target=0x0802ad76
0802ad18  7078      ldrb	r0, [r6, #1]
0802ad1a  8de7      b	#-230 ; -> 0x0802ac38 ; branch_target=0x0802ac38
0802ad1c  7b89      ldrh	r3, [r7, #10]
0802ad1e  013b      subs	r3, #1
0802ad20  13ea5523  ands.w	r3, r3, r5, lsr #9
0802ad24  e0d1      bne	#-64 ; -> 0x0802ace8 ; branch_target=0x0802ace8
0802ad26  3846      mov	r0, r7
0802ad28  fff770fa  bl	#-2848 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802ad2c  0128      cmp	r0, #1
0802ad2e  8246      mov	r10, r0
0802ad30  40f28780  bls.w	#270 ; -> 0x0802ae42 ; branch_target=0x0802ae42
0802ad34  b0f1ff3f  cmp.w	r0, #4294967295
0802ad38  87d0      beq	#-242 ; -> 0x0802ac4a ; branch_target=0x0802ac4a
0802ad3a  7b69      ldr	r3, [r7, #20]
0802ad3c  9842      cmp	r0, r3
0802ad3e  2ad2      bhs	#84 ; -> 0x0802ad96 ; branch_target=0x0802ad96
0802ad40  841e      subs	r4, r0, #2
0802ad42  07f13008  add.w	r8, r7, #48
0802ad46  009b      ldr	r3, [sp]
0802ad48  c3f818a0  str.w	r10, [r3, #24]
0802ad4c  7b69      ldr	r3, [r7, #20]
0802ad4e  023b      subs	r3, #2
0802ad50  a342      cmp	r3, r4
0802ad52  40f29280  bls.w	#292 ; -> 0x0802ae7a ; branch_target=0x0802ae7a
0802ad56  7a89      ldrh	r2, [r7, #10]
0802ad58  bb6a      ldr	r3, [r7, #40]
0802ad5a  04fb023a  mla	r10, r4, r2, r3
0802ad5e  009b      ldr	r3, [sp]
0802ad60  c3f81ca0  str.w	r10, [r3, #28]
0802ad64  c2e7      b	#-124 ; -> 0x0802acec ; branch_target=0x0802acec
0802ad66  0428      cmp	r0, #4
0802ad68  0546      mov	r5, r0
0802ad6a  12d0      beq	#36 ; -> 0x0802ad92 ; branch_target=0x0802ad92
0802ad6c  2846      mov	r0, r5
0802ad6e  05b0      add	sp, #20
0802ad70  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802ad74  b369      ldr	r3, [r6, #24]
0802ad76  1d44      add	r5, r3
0802ad78  013f      subs	r7, #1
0802ad7a  0123      movs	r3, #1
0802ad7c  5946      mov	r1, r11
0802ad7e  2a46      mov	r2, r5
0802ad80  7078      ldrb	r0, [r6, #1]
0802ad82  fef7c7fd  bl	#-5234 ; -> 0x08029914 ; branch_target=0x08029914
0802ad86  012f      cmp	r7, #1
0802ad88  f4d1      bne	#-24 ; -> 0x0802ad74 ; branch_target=0x0802ad74
0802ad8a  7078      ldrb	r0, [r6, #1]
0802ad8c  54e7      b	#-344 ; -> 0x0802ac38 ; branch_target=0x0802ac38
0802ad8e  009b      ldr	r3, [sp]
0802ad90  d961      str	r1, [r3, #28]
0802ad92  0725      movs	r5, #7
0802ad94  5ae7      b	#-332 ; -> 0x0802ac4c ; branch_target=0x0802ac4c
0802ad96  009b      ldr	r3, [sp]
0802ad98  9969      ldr	r1, [r3, #24]
0802ad9a  1846      mov	r0, r3
0802ad9c  fff736fb  bl	#-2452 ; -> 0x0802a40c ; branch_target=0x0802a40c
0802ada0  8246      mov	r10, r0
0802ada2  0028      cmp	r0, #0
0802ada4  f5d0      beq	#-22 ; -> 0x0802ad92 ; branch_target=0x0802ad92
0802ada6  0128      cmp	r0, #1
0802ada8  4bd0      beq	#150 ; -> 0x0802ae42 ; branch_target=0x0802ae42
0802adaa  b0f1ff3f  cmp.w	r0, #4294967295
0802adae  3ff44caf  beq.w	#-360 ; -> 0x0802ac4a ; branch_target=0x0802ac4a
0802adb2  fb78      ldrb	r3, [r7, #3]
0802adb4  07f13008  add.w	r8, r7, #48
0802adb8  002b      cmp	r3, #0
0802adba  68d1      bne	#208 ; -> 0x0802ae8e ; branch_target=0x0802ae8e
0802adbc  0021      movs	r1, #0
0802adbe  4ff40072  mov.w	r2, #512
0802adc2  4046      mov	r0, r8
0802adc4  aaf10204  sub.w	r4, r10, #2
0802adc8  0bf05bfb  bl	#46774 ; -> 0x08036482 ; branch_target=0x08036482
0802adcc  7b69      ldr	r3, [r7, #20]
0802adce  7989      ldrh	r1, [r7, #10]
0802add0  023b      subs	r3, #2
0802add2  9c42      cmp	r4, r3
0802add4  7fd2      bhs	#254 ; -> 0x0802aed6 ; branch_target=0x0802aed6
0802add6  ba6a      ldr	r2, [r7, #40]
0802add8  04fb0122  mla	r2, r4, r1, r2
0802addc  fa62      str	r2, [r7, #44]
0802adde  0029      cmp	r1, #0
0802ade0  53d0      beq	#166 ; -> 0x0802ae8a ; branch_target=0x0802ae8a
0802ade2  0021      movs	r1, #0
0802ade4  0394      str	r4, [sp, #12]
0802ade6  1446      mov	r4, r2
0802ade8  cde90165  strd	r6, r5, [sp, #4]
0802adec  0e46      mov	r6, r1
0802adee  4ff00103  mov.w	r3, #1
0802adf2  2246      mov	r2, r4
0802adf4  4146      mov	r1, r8
0802adf6  7878      ldrb	r0, [r7, #1]
0802adf8  fb70      strb	r3, [r7, #3]
0802adfa  0123      movs	r3, #1
0802adfc  fef78afd  bl	#-5356 ; -> 0x08029914 ; branch_target=0x08029914
0802ae00  0028      cmp	r0, #0
0802ae02  7ff422af  bne.w	#-444 ; -> 0x0802ac4a ; branch_target=0x0802ac4a
0802ae06  3b6a      ldr	r3, [r7, #32]
0802ae08  f870      strb	r0, [r7, #3]
0802ae0a  e11a      subs	r1, r4, r3
0802ae0c  bb69      ldr	r3, [r7, #24]
0802ae0e  9942      cmp	r1, r3
0802ae10  0ed2      bhs	#28 ; -> 0x0802ae30 ; branch_target=0x0802ae30
0802ae12  bd78      ldrb	r5, [r7, #2]
0802ae14  012d      cmp	r5, #1
0802ae16  01d8      bhi	#2 ; -> 0x0802ae1c ; branch_target=0x0802ae1c
0802ae18  0ae0      b	#20 ; -> 0x0802ae30 ; branch_target=0x0802ae30
0802ae1a  bb69      ldr	r3, [r7, #24]
0802ae1c  1c44      add	r4, r3
0802ae1e  013d      subs	r5, #1
0802ae20  0123      movs	r3, #1
0802ae22  4146      mov	r1, r8
0802ae24  2246      mov	r2, r4
0802ae26  7878      ldrb	r0, [r7, #1]
0802ae28  fef774fd  bl	#-5400 ; -> 0x08029914 ; branch_target=0x08029914
0802ae2c  012d      cmp	r5, #1
0802ae2e  f4d1      bne	#-24 ; -> 0x0802ae1a ; branch_target=0x0802ae1a
0802ae30  f86a      ldr	r0, [r7, #44]
0802ae32  731c      adds	r3, r6, #1
0802ae34  7989      ldrh	r1, [r7, #10]
0802ae36  441c      adds	r4, r0, #1
0802ae38  8b42      cmp	r3, r1
0802ae3a  fc62      str	r4, [r7, #44]
0802ae3c  20d2      bhs	#64 ; -> 0x0802ae80 ; branch_target=0x0802ae80
0802ae3e  1e46      mov	r6, r3
0802ae40  d5e7      b	#-86 ; -> 0x0802adee ; branch_target=0x0802adee
0802ae42  0225      movs	r5, #2
0802ae44  02e7      b	#-508 ; -> 0x0802ac4c ; branch_target=0x0802ac4c
0802ae46  0099      ldr	r1, [sp]
0802ae48  91f82420  ldrb.w	r2, [r1, #36]
0802ae4c  1a70      strb	r2, [r3]
0802ae4e  91f82520  ldrb.w	r2, [r1, #37]
0802ae52  5a70      strb	r2, [r3, #1]
0802ae54  91f82620  ldrb.w	r2, [r1, #38]
0802ae58  9a70      strb	r2, [r3, #2]
0802ae5a  91f82720  ldrb.w	r2, [r1, #39]
0802ae5e  da70      strb	r2, [r3, #3]
0802ae60  91f82820  ldrb.w	r2, [r1, #40]
0802ae64  1a71      strb	r2, [r3, #4]
0802ae66  91f82920  ldrb.w	r2, [r1, #41]
0802ae6a  5a71      strb	r2, [r3, #5]
0802ae6c  91f82a20  ldrb.w	r2, [r1, #42]
0802ae70  9a71      strb	r2, [r3, #6]
0802ae72  91f82b20  ldrb.w	r2, [r1, #43]
0802ae76  da71      strb	r2, [r3, #7]
0802ae78  0de7      b	#-486 ; -> 0x0802ac96 ; branch_target=0x0802ac96
0802ae7a  4ff0000a  mov.w	r10, #0
0802ae7e  6ee7      b	#-292 ; -> 0x0802ad5e ; branch_target=0x0802ad5e
0802ae80  3146      mov	r1, r6
0802ae82  029d      ldr	r5, [sp, #8]
0802ae84  019e      ldr	r6, [sp, #4]
0802ae86  039c      ldr	r4, [sp, #12]
0802ae88  421a      subs	r2, r0, r1
0802ae8a  fa62      str	r2, [r7, #44]
0802ae8c  5be7      b	#-330 ; -> 0x0802ad46 ; branch_target=0x0802ad46
0802ae8e  fa6a      ldr	r2, [r7, #44]
0802ae90  0123      movs	r3, #1
0802ae92  4146      mov	r1, r8
0802ae94  7878      ldrb	r0, [r7, #1]
0802ae96  0192      str	r2, [sp, #4]
0802ae98  fef73cfd  bl	#-5512 ; -> 0x08029914 ; branch_target=0x08029914
0802ae9c  0028      cmp	r0, #0
0802ae9e  7ff4d4ae  bne.w	#-600 ; -> 0x0802ac4a ; branch_target=0x0802ac4a
0802aea2  3b6a      ldr	r3, [r7, #32]
0802aea4  019a      ldr	r2, [sp, #4]
0802aea6  f870      strb	r0, [r7, #3]
0802aea8  d11a      subs	r1, r2, r3
0802aeaa  bb69      ldr	r3, [r7, #24]
0802aeac  9942      cmp	r1, r3
0802aeae  85d2      bhs	#-246 ; -> 0x0802adbc ; branch_target=0x0802adbc
0802aeb0  bc78      ldrb	r4, [r7, #2]
0802aeb2  012c      cmp	r4, #1
0802aeb4  82d9      bls	#-252 ; -> 0x0802adbc ; branch_target=0x0802adbc
0802aeb6  0195      str	r5, [sp, #4]
0802aeb8  1546      mov	r5, r2
0802aeba  00e0      b	#0 ; -> 0x0802aebe ; branch_target=0x0802aebe
0802aebc  bb69      ldr	r3, [r7, #24]
0802aebe  1d44      add	r5, r3
0802aec0  013c      subs	r4, #1
0802aec2  0123      movs	r3, #1
0802aec4  4146      mov	r1, r8
0802aec6  2a46      mov	r2, r5
0802aec8  7878      ldrb	r0, [r7, #1]
0802aeca  fef723fd  bl	#-5562 ; -> 0x08029914 ; branch_target=0x08029914
0802aece  012c      cmp	r4, #1
0802aed0  f4d1      bne	#-24 ; -> 0x0802aebc ; branch_target=0x0802aebc
0802aed2  019d      ldr	r5, [sp, #4]
0802aed4  72e7      b	#-284 ; -> 0x0802adbc ; branch_target=0x0802adbc
0802aed6  0022      movs	r2, #0
0802aed8  80e7      b	#-256 ; -> 0x0802addc ; branch_target=0x0802addc
