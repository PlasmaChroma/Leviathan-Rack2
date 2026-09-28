; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802bd64  88b1      cbz	r0, #34 ; -> 0x0802bd8a ; branch_target=0x0802bd8a
0802bd66  2de9f043  push.w	{r4, r5, r6, r7, r8, r9, lr}
0802bd6a  0546      mov	r5, r0
0802bd6c  83b0      sub	sp, #12
0802bd6e  0e46      mov	r6, r1
0802bd70  604c      ldr	r4, [pc, #384] ; [0x0802bef4] = 0x080365f4
0802bd72  2b68      ldr	r3, [r5]
0802bd74  2bb1      cbz	r3, #10 ; -> 0x0802bd82 ; branch_target=0x0802bd82
0802bd76  1a78      ldrb	r2, [r3]
0802bd78  1ab1      cbz	r2, #6 ; -> 0x0802bd82 ; branch_target=0x0802bd82
0802bd7a  a988      ldrh	r1, [r5, #4]
0802bd7c  da88      ldrh	r2, [r3, #6]
0802bd7e  9142      cmp	r1, r2
0802bd80  05d0      beq	#10 ; -> 0x0802bd8e ; branch_target=0x0802bd8e
0802bd82  0920      movs	r0, #9
0802bd84  03b0      add	sp, #12
0802bd86  bde8f083  pop.w	{r4, r5, r6, r7, r8, r9, pc}
0802bd8a  0920      movs	r0, #9
0802bd8c  7047      bx	lr
0802bd8e  5878      ldrb	r0, [r3, #1]
0802bd90  fdf796fd  bl	#-9428 ; -> 0x080298c0 ; branch_target=0x080298c0
0802bd94  10f00101  ands	r1, r0, #1
0802bd98  f3d1      bne	#-26 ; -> 0x0802bd82 ; branch_target=0x0802bd82
0802bd9a  2846      mov	r0, r5
0802bd9c  002e      cmp	r6, #0
0802bd9e  62d0      beq	#196 ; -> 0x0802be66 ; branch_target=0x0802be66
0802bda0  fef78afa  bl	#-6892 ; -> 0x0802a2b8 ; branch_target=0x0802a2b8
0802bda4  10f0fb03  ands	r3, r0, #251
0802bda8  ecd1      bne	#-40 ; -> 0x0802bd84 ; branch_target=0x0802bd84
0802bdaa  7372      strb	r3, [r6, #9]
0802bdac  eb69      ldr	r3, [r5, #28]
0802bdae  002b      cmp	r3, #0
0802bdb0  57d0      beq	#174 ; -> 0x0802be62 ; branch_target=0x0802be62
0802bdb2  3146      mov	r1, r6
0802bdb4  2846      mov	r0, r5
0802bdb6  fdf78dfe  bl	#-8934 ; -> 0x08029ad4 ; branch_target=0x08029ad4
0802bdba  6f69      ldr	r7, [r5, #20]
0802bdbc  ea69      ldr	r2, [r5, #28]
0802bdbe  2b68      ldr	r3, [r5]
0802bdc0  2037      adds	r7, #32
0802bdc2  a2b1      cbz	r2, #40 ; -> 0x0802bdee ; branch_target=0x0802bdee
0802bdc4  b7f5001f  cmp.w	r7, #2097152
0802bdc8  11d2      bhs	#34 ; -> 0x0802bdee ; branch_target=0x0802bdee
0802bdca  c7f30808  ubfx	r8, r7, #0, #9
0802bdce  b8f1000f  cmp.w	r8, #0
0802bdd2  08d1      bne	#16 ; -> 0x0802bde6 ; branch_target=0x0802bde6
0802bdd4  0132      adds	r2, #1
0802bdd6  a969      ldr	r1, [r5, #24]
0802bdd8  ea61      str	r2, [r5, #28]
0802bdda  0029      cmp	r1, #0
0802bddc  60d1      bne	#192 ; -> 0x0802bea0 ; branch_target=0x0802bea0
0802bdde  1a89      ldrh	r2, [r3, #8]
0802bde0  b2eb571f  cmp.w	r2, r7, lsr #5
0802bde4  7ad9      bls	#244 ; -> 0x0802bedc ; branch_target=0x0802bedc
0802bde6  3033      adds	r3, #48
0802bde8  6f61      str	r7, [r5, #20]
0802bdea  4344      add	r3, r8
0802bdec  2b62      str	r3, [r5, #32]
0802bdee  737a      ldrb	r3, [r6, #9]
0802bdf0  002b      cmp	r3, #0
0802bdf2  36d0      beq	#108 ; -> 0x0802be62 ; branch_target=0x0802be62
0802bdf4  286b      ldr	r0, [r5, #48]
0802bdf6  06f10901  add.w	r1, r6, #9
0802bdfa  0378      ldrb	r3, [r0]
0802bdfc  0246      mov	r2, r0
0802bdfe  0130      adds	r0, #1
0802be00  3f2b      cmp	r3, #63
0802be02  17d0      beq	#46 ; -> 0x0802be34 ; branch_target=0x0802be34
0802be04  2a2b      cmp	r3, #42
0802be06  15d0      beq	#42 ; -> 0x0802be34 ; branch_target=0x0802be34
0802be08  a3f16102  sub.w	r2, r3, #97
0802be0c  192a      cmp	r2, #25
0802be0e  2fd8      bhi	#94 ; -> 0x0802be70 ; branch_target=0x0802be70
0802be10  203b      subs	r3, #32
0802be12  9bb2      uxth	r3, r3
0802be14  11f8012b  ldrb	r2, [r1], #1
0802be18  a2f1610c  sub.w	r12, r2, #97
0802be1c  bcf1190f  cmp.w	r12, #25
0802be20  32d8      bhi	#100 ; -> 0x0802be88 ; branch_target=0x0802be88
0802be22  203a      subs	r2, #32
0802be24  92b2      uxth	r2, r2
0802be26  9a42      cmp	r2, r3
0802be28  a3d1      bne	#-186 ; -> 0x0802bd72 ; branch_target=0x0802bd72
0802be2a  0378      ldrb	r3, [r0]
0802be2c  0246      mov	r2, r0
0802be2e  0130      adds	r0, #1
0802be30  3f2b      cmp	r3, #63
0802be32  e7d1      bne	#-50 ; -> 0x0802be04 ; branch_target=0x0802be04
0802be34  0023      movs	r3, #0
0802be36  92f800e0  ldrb.w	lr, [r2]
0802be3a  8446      mov	r12, r0
0802be3c  1a46      mov	r2, r3
0802be3e  bef13f0f  cmp.w	lr, #63
0802be42  6046      mov	r0, r12
0802be44  1cf801eb  ldrb	lr, [r12], #1
0802be48  0cbf      ite	eq
0802be4a  0132      addeq	r2, #1
0802be4c  0123      movne	r3, #1
0802be4e  bef12a0f  cmp.w	lr, #42
0802be52  f4d0      beq	#-24 ; -> 0x0802be3e ; branch_target=0x0802be3e
0802be54  bef13f0f  cmp.w	lr, #63
0802be58  f1d0      beq	#-30 ; -> 0x0802be3e ; branch_target=0x0802be3e
0802be5a  fdf7c7fd  bl	#-9330 ; -> 0x080299ec ; branch_target=0x080299ec
0802be5e  0028      cmp	r0, #0
0802be60  87d0      beq	#-242 ; -> 0x0802bd72 ; branch_target=0x0802bd72
0802be62  0020      movs	r0, #0
0802be64  8ee7      b	#-228 ; -> 0x0802bd84 ; branch_target=0x0802bd84
0802be66  03b0      add	sp, #12
0802be68  bde8f043  pop.w	{r4, r5, r6, r7, r8, r9, lr}
0802be6c  fef79ebb  b.w	#-6340 ; -> 0x0802a5ac ; branch_target=0x0802a5ac
0802be70  7f2b      cmp	r3, #127
0802be72  cfd9      bls	#-98 ; -> 0x0802be14 ; branch_target=0x0802be14
0802be74  11f8012b  ldrb	r2, [r1], #1
0802be78  2344      add	r3, r4
0802be7a  a2f1610c  sub.w	r12, r2, #97
0802be7e  13f8803c  ldrb	r3, [r3, #-128]
0802be82  bcf1190f  cmp.w	r12, #25
0802be86  ccd9      bls	#-104 ; -> 0x0802be22 ; branch_target=0x0802be22
0802be88  7f2a      cmp	r2, #127
0802be8a  02d9      bls	#4 ; -> 0x0802be92 ; branch_target=0x0802be92
0802be8c  2244      add	r2, r4
0802be8e  12f8802c  ldrb	r2, [r2, #-128]
0802be92  9a42      cmp	r2, r3
0802be94  7ff46daf  bne.w	#-294 ; -> 0x0802bd72 ; branch_target=0x0802bd72
0802be98  002a      cmp	r2, #0
0802be9a  aed1      bne	#-164 ; -> 0x0802bdfa ; branch_target=0x0802bdfa
0802be9c  0020      movs	r0, #0
0802be9e  71e7      b	#-286 ; -> 0x0802bd84 ; branch_target=0x0802bd84
0802bea0  5a89      ldrh	r2, [r3, #10]
0802bea2  013a      subs	r2, #1
0802bea4  12ea5729  ands.w	r9, r2, r7, lsr #9
0802bea8  9dd1      bne	#-198 ; -> 0x0802bde6 ; branch_target=0x0802bde6
0802beaa  1846      mov	r0, r3
0802beac  0193      str	r3, [sp, #4]
0802beae  fef7adf9  bl	#-7334 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802beb2  0128      cmp	r0, #1
0802beb4  1bd9      bls	#54 ; -> 0x0802beee ; branch_target=0x0802beee
0802beb6  431c      adds	r3, r0, #1
0802beb8  17d0      beq	#46 ; -> 0x0802beea ; branch_target=0x0802beea
0802beba  019b      ldr	r3, [sp, #4]
0802bebc  5969      ldr	r1, [r3, #20]
0802bebe  8842      cmp	r0, r1
0802bec0  0fd2      bhs	#30 ; -> 0x0802bee2 ; branch_target=0x0802bee2
0802bec2  a861      str	r0, [r5, #24]
0802bec4  0238      subs	r0, #2
0802bec6  5969      ldr	r1, [r3, #20]
0802bec8  0239      subs	r1, #2
0802beca  8842      cmp	r0, r1
0802becc  03d2      bhs	#6 ; -> 0x0802bed6 ; branch_target=0x0802bed6
0802bece  5989      ldrh	r1, [r3, #10]
0802bed0  9a6a      ldr	r2, [r3, #40]
0802bed2  00fb0129  mla	r9, r0, r1, r2
0802bed6  c5f81c90  str.w	r9, [r5, #28]
0802beda  84e7      b	#-248 ; -> 0x0802bde6 ; branch_target=0x0802bde6
0802bedc  e961      str	r1, [r5, #28]
0802bede  737a      ldrb	r3, [r6, #9]
0802bee0  86e7      b	#-244 ; -> 0x0802bdf0 ; branch_target=0x0802bdf0
0802bee2  c5f81c90  str.w	r9, [r5, #28]
0802bee6  737a      ldrb	r3, [r6, #9]
0802bee8  82e7      b	#-252 ; -> 0x0802bdf0 ; branch_target=0x0802bdf0
0802beea  0120      movs	r0, #1
0802beec  4ae7      b	#-364 ; -> 0x0802bd84 ; branch_target=0x0802bd84
0802beee  0220      movs	r0, #2
0802bef0  48e7      b	#-368 ; -> 0x0802bd84 ; branch_target=0x0802bd84
