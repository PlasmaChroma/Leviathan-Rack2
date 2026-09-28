; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802af74  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
0802af78  90b0      sub	sp, #64
0802af7a  0191      str	r1, [sp, #4]
0802af7c  0028      cmp	r0, #0
0802af7e  36d0      beq	#108 ; -> 0x0802afee ; branch_target=0x0802afee
0802af80  02f03f07  and	r7, r2, #63
0802af84  1646      mov	r6, r2
0802af86  0446      mov	r4, r0
0802af88  02a9      add	r1, sp, #8
0802af8a  01a8      add	r0, sp, #4
0802af8c  3a46      mov	r2, r7
0802af8e  fef753ff  bl	#-4442 ; -> 0x08029e38 ; branch_target=0x08029e38
0802af92  0546      mov	r5, r0
0802af94  28b1      cbz	r0, #10 ; -> 0x0802afa2 ; branch_target=0x0802afa2
0802af96  0023      movs	r3, #0
0802af98  2360      str	r3, [r4]
0802af9a  2846      mov	r0, r5
0802af9c  10b0      add	sp, #64
0802af9e  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0802afa2  ddf80880  ldr.w	r8, [sp, #8]
0802afa6  03a8      add	r0, sp, #12
0802afa8  0199      ldr	r1, [sp, #4]
0802afaa  cdf80c80  str.w	r8, [sp, #12]
0802afae  fff785fb  bl	#-2294 ; -> 0x0802a6bc ; branch_target=0x0802a6bc
0802afb2  a8b9      cbnz	r0, #42 ; -> 0x0802afe0 ; branch_target=0x0802afe0
0802afb4  9df93b30  ldrsb.w	r3, [sp, #59]
0802afb8  002b      cmp	r3, #0
0802afba  1ddb      blt	#58 ; -> 0x0802aff8 ; branch_target=0x0802aff8
0802afbc  a84b      ldr	r3, [pc, #672] ; [0x0802b260] = 0x2000206c
0802afbe  06f03e0c  and	r12, r6, #62
0802afc2  039a      ldr	r2, [sp, #12]
0802afc4  1868      ldr	r0, [r3]
0802afc6  0028      cmp	r0, #0
0802afc8  00f00981  beq.w	#530 ; -> 0x0802b1de ; branch_target=0x0802b1de
0802afcc  9042      cmp	r0, r2
0802afce  60d0      beq	#192 ; -> 0x0802b092 ; branch_target=0x0802b092
0802afd0  1969      ldr	r1, [r3, #16]
0802afd2  0029      cmp	r1, #0
0802afd4  6dd0      beq	#218 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802afd6  9142      cmp	r1, r2
0802afd8  00f01681  beq.w	#556 ; -> 0x0802b208 ; branch_target=0x0802b208
0802afdc  1225      movs	r5, #18
0802afde  dae7      b	#-76 ; -> 0x0802af96 ; branch_target=0x0802af96
0802afe0  16f01c0f  tst.w	r6, #28
0802afe4  01d0      beq	#2 ; -> 0x0802afea ; branch_target=0x0802afea
0802afe6  0428      cmp	r0, #4
0802afe8  08d0      beq	#16 ; -> 0x0802affc ; branch_target=0x0802affc
0802afea  0546      mov	r5, r0
0802afec  d3e7      b	#-90 ; -> 0x0802af96 ; branch_target=0x0802af96
0802afee  0925      movs	r5, #9
0802aff0  2846      mov	r0, r5
0802aff2  10b0      add	sp, #64
0802aff4  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0802aff8  0625      movs	r5, #6
0802affa  cce7      b	#-104 ; -> 0x0802af96 ; branch_target=0x0802af96
0802affc  984b      ldr	r3, [pc, #608] ; [0x0802b260] = 0x2000206c
0802affe  1a68      ldr	r2, [r3]
0802b000  002a      cmp	r2, #0
0802b002  00f0f980  beq.w	#498 ; -> 0x0802b1f8 ; branch_target=0x0802b1f8
0802b006  1b69      ldr	r3, [r3, #16]
0802b008  002b      cmp	r3, #0
0802b00a  e7d1      bne	#-50 ; -> 0x0802afdc ; branch_target=0x0802afdc
0802b00c  03a8      add	r0, sp, #12
0802b00e  fff7fdfd  bl	#-1030 ; -> 0x0802ac0c ; branch_target=0x0802ac0c
0802b012  0346      mov	r3, r0
0802b014  0028      cmp	r0, #0
0802b016  40f01c81  bne.w	#568 ; -> 0x0802b252 ; branch_target=0x0802b252
0802b01a  47f00807  orr	r7, r7, #8
0802b01e  07f0bdfd  bl	#31610 ; -> 0x08032b9c ; branch_target=0x08032b9c
0802b022  ddf82c90  ldr.w	r9, [sp, #44]
0802b026  2023      movs	r3, #32
0802b028  c9f80e00  str.w	r0, [r9, #14]
0802b02c  c9f81600  str.w	r0, [r9, #22]
0802b030  89f80b30  strb.w	r3, [r9, #11]
0802b034  98f80030  ldrb.w	r3, [r8]
0802b038  b9f81a60  ldrh.w	r6, [r9, #26]
0802b03c  032b      cmp	r3, #3
0802b03e  03d1      bne	#6 ; -> 0x0802b048 ; branch_target=0x0802b048
0802b040  b9f81430  ldrh.w	r3, [r9, #20]
0802b044  46ea0346  orr.w	r6, r6, r3, lsl #16
0802b048  0023      movs	r3, #0
0802b04a  a9f81a30  strh.w	r3, [r9, #26]
0802b04e  98f80020  ldrb.w	r2, [r8]
0802b052  032a      cmp	r2, #3
0802b054  4ff00002  mov.w	r2, #0
0802b058  08bf      it	eq
0802b05a  a9f81430  strheq.w	r3, [r9, #20]
0802b05e  0123      movs	r3, #1
0802b060  c9f81c20  str.w	r2, [r9, #28]
0802b064  88f80330  strb.w	r3, [r8, #3]
0802b068  002e      cmp	r6, #0
0802b06a  40f0de80  bne.w	#444 ; -> 0x0802b22a ; branch_target=0x0802b22a
0802b06e  47f04007  orr	r7, r7, #64
0802b072  012f      cmp	r7, #1
0802b074  d8f82c30  ldr.w	r3, [r8, #44]
0802b078  03a8      add	r0, sp, #12
0802b07a  94bf      ite	ls
0802b07c  0021      movls	r1, #0
0802b07e  0121      movhi	r1, #1
0802b080  c4e90939  strd	r3, r9, [r4, #36]
0802b084  fef760fc  bl	#-5952 ; -> 0x08029948 ; branch_target=0x08029948
0802b088  2061      str	r0, [r4, #16]
0802b08a  0028      cmp	r0, #0
0802b08c  43d1      bne	#134 ; -> 0x0802b116 ; branch_target=0x0802b116
0802b08e  0225      movs	r5, #2
0802b090  81e7      b	#-254 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b092  5a68      ldr	r2, [r3, #4]
0802b094  ddf814e0  ldr.w	lr, [sp, #20]
0802b098  7245      cmp	r2, lr
0802b09a  00f0b980  beq.w	#370 ; -> 0x0802b210 ; branch_target=0x0802b210
0802b09e  1a69      ldr	r2, [r3, #16]
0802b0a0  3ab1      cbz	r2, #14 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802b0a2  8242      cmp	r2, r0
0802b0a4  9ad1      bne	#-204 ; -> 0x0802afdc ; branch_target=0x0802afdc
0802b0a6  2846      mov	r0, r5
0802b0a8  5a69      ldr	r2, [r3, #20]
0802b0aa  7245      cmp	r2, lr
0802b0ac  0dd0      beq	#26 ; -> 0x0802b0ca ; branch_target=0x0802b0ca
0802b0ae  0028      cmp	r0, #0
0802b0b0  94d0      beq	#-216 ; -> 0x0802afdc ; branch_target=0x0802afdc
0802b0b2  16f01c0f  tst.w	r6, #28
0802b0b6  18d0      beq	#48 ; -> 0x0802b0ea ; branch_target=0x0802b0ea
0802b0b8  9df81230  ldrb.w	r3, [sp, #18]
0802b0bc  13f0110f  tst.w	r3, #17
0802b0c0  20d1      bne	#64 ; -> 0x0802b104 ; branch_target=0x0802b104
0802b0c2  7307      lsls	r3, r6, #29
0802b0c4  20d5      bpl	#64 ; -> 0x0802b108 ; branch_target=0x0802b108
0802b0c6  0825      movs	r5, #8
0802b0c8  65e7      b	#-310 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b0ca  9969      ldr	r1, [r3, #24]
0802b0cc  089a      ldr	r2, [sp, #32]
0802b0ce  9142      cmp	r1, r2
0802b0d0  edd1      bne	#-38 ; -> 0x0802b0ae ; branch_target=0x0802b0ae
0802b0d2  0122      movs	r2, #1
0802b0d4  bcf1000f  cmp.w	r12, #0
0802b0d8  05d1      bne	#10 ; -> 0x0802b0e6 ; branch_target=0x0802b0e6
0802b0da  03eb0213  add.w	r3, r3, r2, lsl #4
0802b0de  9b89      ldrh	r3, [r3, #12]
0802b0e0  b3f5807f  cmp.w	r3, #256
0802b0e4  e5d1      bne	#-54 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802b0e6  1025      movs	r5, #16
0802b0e8  55e7      b	#-342 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b0ea  9df81230  ldrb.w	r3, [sp, #18]
0802b0ee  d806      lsls	r0, r3, #27
0802b0f0  0fd4      bmi	#30 ; -> 0x0802b112 ; branch_target=0x0802b112
0802b0f2  b207      lsls	r2, r6, #30
0802b0f4  01d5      bpl	#2 ; -> 0x0802b0fa ; branch_target=0x0802b0fa
0802b0f6  db07      lsls	r3, r3, #31
0802b0f8  04d4      bmi	#8 ; -> 0x0802b104 ; branch_target=0x0802b104
0802b0fa  3007      lsls	r0, r6, #28
0802b0fc  ddf82c90  ldr.w	r9, [sp, #44]
0802b100  b7d5      bpl	#-146 ; -> 0x0802b072 ; branch_target=0x0802b072
0802b102  b4e7      b	#-152 ; -> 0x0802b06e ; branch_target=0x0802b06e
0802b104  0725      movs	r5, #7
0802b106  46e7      b	#-372 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b108  3607      lsls	r6, r6, #28
0802b10a  ddf82c90  ldr.w	r9, [sp, #44]
0802b10e  b0d5      bpl	#-160 ; -> 0x0802b072 ; branch_target=0x0802b072
0802b110  85e7      b	#-246 ; -> 0x0802b01e ; branch_target=0x0802b01e
0802b112  0425      movs	r5, #4
0802b114  3fe7      b	#-386 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b116  98f80030  ldrb.w	r3, [r8]
0802b11a  b9f81a60  ldrh.w	r6, [r9, #26]
0802b11e  032b      cmp	r3, #3
0802b120  03d1      bne	#6 ; -> 0x0802b12a ; branch_target=0x0802b12a
0802b122  b9f81430  ldrh.w	r3, [r9, #20]
0802b126  46ea0346  orr.w	r6, r6, r3, lsl #16
0802b12a  0021      movs	r1, #0
0802b12c  a660      str	r6, [r4, #8]
0802b12e  d9f81c90  ldr.w	r9, [r9, #28]
0802b132  04f1300a  add.w	r10, r4, #48
0802b136  e162      str	r1, [r4, #44]
0802b138  4ff40072  mov.w	r2, #512
0802b13c  c4f80080  str.w	r8, [r4]
0802b140  5046      mov	r0, r10
0802b142  c4f80c90  str.w	r9, [r4, #12]
0802b146  b8f80630  ldrh.w	r3, [r8, #6]
0802b14a  6175      strb	r1, [r4, #21]
0802b14c  2162      str	r1, [r4, #32]
0802b14e  a161      str	r1, [r4, #24]
0802b150  a380      strh	r3, [r4, #4]
0802b152  2775      strb	r7, [r4, #20]
0802b154  0bf095f9  bl	#45866 ; -> 0x08036482 ; branch_target=0x08036482
0802b158  b906      lsls	r1, r7, #26
0802b15a  7ff51eaf  bpl.w	#-452 ; -> 0x0802af9a ; branch_target=0x0802af9a
0802b15e  b9f1000f  cmp.w	r9, #0
0802b162  3ff41aaf  beq.w	#-460 ; -> 0x0802af9a ; branch_target=0x0802af9a
0802b166  c4f81890  str.w	r9, [r4, #24]
0802b16a  b8f80a30  ldrh.w	r3, [r8, #10]
0802b16e  b9eb432f  cmp.w	r9, r3, lsl #9
0802b172  4fea4327  lsl.w	r7, r3, #9
0802b176  6ed9      bls	#220 ; -> 0x0802b256 ; branch_target=0x0802b256
0802b178  3146      mov	r1, r6
0802b17a  2068      ldr	r0, [r4]
0802b17c  fff746f8  bl	#-3956 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802b180  0128      cmp	r0, #1
0802b182  a9eb0709  sub.w	r9, r9, r7
0802b186  0646      mov	r6, r0
0802b188  34d9      bls	#104 ; -> 0x0802b1f4 ; branch_target=0x0802b1f4
0802b18a  421c      adds	r2, r0, #1
0802b18c  01d0      beq	#2 ; -> 0x0802b192 ; branch_target=0x0802b192
0802b18e  4f45      cmp	r7, r9
0802b190  f2d3      blo	#-28 ; -> 0x0802b178 ; branch_target=0x0802b178
0802b192  731c      adds	r3, r6, #1
0802b194  e661      str	r6, [r4, #28]
0802b196  20d0      beq	#64 ; -> 0x0802b1da ; branch_target=0x0802b1da
0802b198  c9f30803  ubfx	r3, r9, #0, #9
0802b19c  002b      cmp	r3, #0
0802b19e  3ff4fcae  beq.w	#-520 ; -> 0x0802af9a ; branch_target=0x0802af9a
0802b1a2  d8f81430  ldr.w	r3, [r8, #20]
0802b1a6  023e      subs	r6, #2
0802b1a8  023b      subs	r3, #2
0802b1aa  9e42      cmp	r6, r3
0802b1ac  bff46faf  bhs.w	#-290 ; -> 0x0802b08e ; branch_target=0x0802b08e
0802b1b0  b8f80a30  ldrh.w	r3, [r8, #10]
0802b1b4  d8f82820  ldr.w	r2, [r8, #40]
0802b1b8  06fb0322  mla	r2, r6, r3, r2
0802b1bc  002a      cmp	r2, #0
0802b1be  3ff466af  beq.w	#-308 ; -> 0x0802b08e ; branch_target=0x0802b08e
0802b1c2  02eb5922  add.w	r2, r2, r9, lsr #9
0802b1c6  5146      mov	r1, r10
0802b1c8  0123      movs	r3, #1
0802b1ca  2262      str	r2, [r4, #32]
0802b1cc  98f80100  ldrb.w	r0, [r8, #1]
0802b1d0  fef792fb  bl	#-6364 ; -> 0x080298f8 ; branch_target=0x080298f8
0802b1d4  0028      cmp	r0, #0
0802b1d6  3ff4e0ae  beq.w	#-576 ; -> 0x0802af9a ; branch_target=0x0802af9a
0802b1da  0125      movs	r5, #1
0802b1dc  dbe6      b	#-586 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b1de  1969      ldr	r1, [r3, #16]
0802b1e0  0029      cmp	r1, #0
0802b1e2  3ff466af  beq.w	#-308 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802b1e6  9142      cmp	r1, r2
0802b1e8  7ff463af  bne.w	#-314 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802b1ec  0120      movs	r0, #1
0802b1ee  ddf814e0  ldr.w	lr, [sp, #20]
0802b1f2  59e7      b	#-334 ; -> 0x0802b0a8 ; branch_target=0x0802b0a8
0802b1f4  e061      str	r0, [r4, #28]
0802b1f6  4ae7      b	#-364 ; -> 0x0802b08e ; branch_target=0x0802b08e
0802b1f8  03a8      add	r0, sp, #12
0802b1fa  fff707fd  bl	#-1522 ; -> 0x0802ac0c ; branch_target=0x0802ac0c
0802b1fe  0028      cmp	r0, #0
0802b200  3ff40baf  beq.w	#-490 ; -> 0x0802b01a ; branch_target=0x0802b01a
0802b204  0546      mov	r5, r0
0802b206  c6e6      b	#-628 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b208  2846      mov	r0, r5
0802b20a  ddf814e0  ldr.w	lr, [sp, #20]
0802b20e  4be7      b	#-362 ; -> 0x0802b0a8 ; branch_target=0x0802b0a8
0802b210  089a      ldr	r2, [sp, #32]
0802b212  9968      ldr	r1, [r3, #8]
0802b214  9142      cmp	r1, r2
0802b216  20d0      beq	#64 ; -> 0x0802b25a ; branch_target=0x0802b25a
0802b218  1a69      ldr	r2, [r3, #16]
0802b21a  002a      cmp	r2, #0
0802b21c  3ff449af  beq.w	#-366 ; -> 0x0802b0b2 ; branch_target=0x0802b0b2
0802b220  9042      cmp	r0, r2
0802b222  7ff4dbae  bne.w	#-586 ; -> 0x0802afdc ; branch_target=0x0802afdc
0802b226  0020      movs	r0, #0
0802b228  3ee7      b	#-388 ; -> 0x0802b0a8 ; branch_target=0x0802b0a8
0802b22a  3146      mov	r1, r6
0802b22c  03a8      add	r0, sp, #12
0802b22e  d8f82ca0  ldr.w	r10, [r8, #44]
0802b232  fff7d9fb  bl	#-2126 ; -> 0x0802a9e8 ; branch_target=0x0802a9e8
0802b236  0028      cmp	r0, #0
0802b238  7ff4d7ae  bne.w	#-594 ; -> 0x0802afea ; branch_target=0x0802afea
0802b23c  013e      subs	r6, #1
0802b23e  5146      mov	r1, r10
0802b240  4046      mov	r0, r8
0802b242  fef7bbfd  bl	#-5258 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802b246  c8f80c60  str.w	r6, [r8, #12]
0802b24a  0346      mov	r3, r0
0802b24c  0028      cmp	r0, #0
0802b24e  3ff40eaf  beq.w	#-484 ; -> 0x0802b06e ; branch_target=0x0802b06e
0802b252  1d46      mov	r5, r3
0802b254  9fe6      b	#-706 ; -> 0x0802af96 ; branch_target=0x0802af96
0802b256  e661      str	r6, [r4, #28]
0802b258  9ee7      b	#-196 ; -> 0x0802b198 ; branch_target=0x0802b198
0802b25a  2a46      mov	r2, r5
0802b25c  3ae7      b	#-396 ; -> 0x0802b0d4 ; branch_target=0x0802b0d4
