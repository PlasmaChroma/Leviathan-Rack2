; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802b4cc  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802b4d0  1f46      mov	r7, r3
0802b4d2  0023      movs	r3, #0
0802b4d4  83b0      sub	sp, #12
0802b4d6  3b60      str	r3, [r7]
0802b4d8  50b1      cbz	r0, #20 ; -> 0x0802b4f0 ; branch_target=0x0802b4f0
0802b4da  0368      ldr	r3, [r0]
0802b4dc  0446      mov	r4, r0
0802b4de  3bb1      cbz	r3, #14 ; -> 0x0802b4f0 ; branch_target=0x0802b4f0
0802b4e0  1646      mov	r6, r2
0802b4e2  1a78      ldrb	r2, [r3]
0802b4e4  22b1      cbz	r2, #8 ; -> 0x0802b4f0 ; branch_target=0x0802b4f0
0802b4e6  0d46      mov	r5, r1
0802b4e8  da88      ldrh	r2, [r3, #6]
0802b4ea  8188      ldrh	r1, [r0, #4]
0802b4ec  9142      cmp	r1, r2
0802b4ee  05d0      beq	#10 ; -> 0x0802b4fc ; branch_target=0x0802b4fc
0802b4f0  4ff00908  mov.w	r8, #9
0802b4f4  4046      mov	r0, r8
0802b4f6  03b0      add	sp, #12
0802b4f8  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b4fc  5878      ldrb	r0, [r3, #1]
0802b4fe  fef7dff9  bl	#-7234 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b502  c107      lsls	r1, r0, #31
0802b504  f4d4      bmi	#-24 ; -> 0x0802b4f0 ; branch_target=0x0802b4f0
0802b506  94f81580  ldrb.w	r8, [r4, #21]
0802b50a  b8f1000f  cmp.w	r8, #0
0802b50e  f1d1      bne	#-30 ; -> 0x0802b4f4 ; branch_target=0x0802b4f4
0802b510  237d      ldrb	r3, [r4, #20]
0802b512  9a07      lsls	r2, r3, #30
0802b514  40f1fe80  bpl.w	#508 ; -> 0x0802b714 ; branch_target=0x0802b714
0802b518  a269      ldr	r2, [r4, #24]
0802b51a  d4f80090  ldr.w	r9, [r4]
0802b51e  f242      cmn	r2, r6
0802b520  28bf      it	hs
0802b522  d643      mvnhs	r6, r2
0802b524  002e      cmp	r6, #0
0802b526  00f0dc80  beq.w	#440 ; -> 0x0802b6e2 ; branch_target=0x0802b6e2
0802b52a  c2f30800  ubfx	r0, r2, #0, #9
0802b52e  0028      cmp	r0, #0
0802b530  61d1      bne	#194 ; -> 0x0802b5f6 ; branch_target=0x0802b5f6
0802b532  b9f80aa0  ldrh.w	r10, [r9, #10]
0802b536  530a      lsrs	r3, r2, #9
0802b538  0af1ff3a  add.w	r10, r10, #4294967295
0802b53c  1aea522a  ands.w	r10, r10, r2, lsr #9
0802b540  10d1      bne	#32 ; -> 0x0802b564 ; branch_target=0x0802b564
0802b542  002a      cmp	r2, #0
0802b544  40f0b780  bne.w	#366 ; -> 0x0802b6b6 ; branch_target=0x0802b6b6
0802b548  a068      ldr	r0, [r4, #8]
0802b54a  0028      cmp	r0, #0
0802b54c  00f0ed80  beq.w	#474 ; -> 0x0802b72a ; branch_target=0x0802b72a
0802b550  0128      cmp	r0, #1
0802b552  00f09c80  beq.w	#312 ; -> 0x0802b68e ; branch_target=0x0802b68e
0802b556  431c      adds	r3, r0, #1
0802b558  00f0d580  beq.w	#426 ; -> 0x0802b706 ; branch_target=0x0802b706
0802b55c  a368      ldr	r3, [r4, #8]
0802b55e  e061      str	r0, [r4, #28]
0802b560  03b9      cbnz	r3, #0 ; -> 0x0802b564 ; branch_target=0x0802b564
0802b562  a060      str	r0, [r4, #8]
0802b564  94f91430  ldrsb.w	r3, [r4, #20]
0802b568  002b      cmp	r3, #0
0802b56a  c0f2be80  blt.w	#380 ; -> 0x0802b6ea ; branch_target=0x0802b6ea
0802b56e  e369      ldr	r3, [r4, #28]
0802b570  d9f81420  ldr.w	r2, [r9, #20]
0802b574  023b      subs	r3, #2
0802b576  023a      subs	r2, #2
0802b578  9342      cmp	r3, r2
0802b57a  80f08880  bhs.w	#272 ; -> 0x0802b68e ; branch_target=0x0802b68e
0802b57e  b9f80a10  ldrh.w	r1, [r9, #10]
0802b582  d9f82820  ldr.w	r2, [r9, #40]
0802b586  01fb0322  mla	r2, r1, r3, r2
0802b58a  002a      cmp	r2, #0
0802b58c  7fd0      beq	#254 ; -> 0x0802b68e ; branch_target=0x0802b68e
0802b58e  b6f5007f  cmp.w	r6, #512
0802b592  5244      add	r2, r10
0802b594  c0f08280  blo.w	#260 ; -> 0x0802b69c ; branch_target=0x0802b69c
0802b598  0aeb5623  add.w	r3, r10, r6, lsr #9
0802b59c  99f80100  ldrb.w	r0, [r9, #1]
0802b5a0  4fea562b  lsr.w	r11, r6, #9
0802b5a4  0192      str	r2, [sp, #4]
0802b5a6  8b42      cmp	r3, r1
0802b5a8  88bf      it	hi
0802b5aa  a1eb0a0b  subhi.w	r11, r1, r10
0802b5ae  2946      mov	r1, r5
0802b5b0  5b46      mov	r3, r11
0802b5b2  fef7aff9  bl	#-7330 ; -> 0x08029914 ; branch_target=0x08029914
0802b5b6  0028      cmp	r0, #0
0802b5b8  40f0a580  bne.w	#330 ; -> 0x0802b706 ; branch_target=0x0802b706
0802b5bc  236a      ldr	r3, [r4, #32]
0802b5be  019a      ldr	r2, [sp, #4]
0802b5c0  9b1a      subs	r3, r3, r2
0802b5c2  5b45      cmp	r3, r11
0802b5c4  c0f0c580  blo.w	#394 ; -> 0x0802b752 ; branch_target=0x0802b752
0802b5c8  05eb4b2c  add.w	r12, r5, r11, lsl #9
0802b5cc  4fea4b20  lsl.w	r0, r11, #9
0802b5d0  a369      ldr	r3, [r4, #24]
0802b5d2  6546      mov	r5, r12
0802b5d4  e268      ldr	r2, [r4, #12]
0802b5d6  0344      add	r3, r0
0802b5d8  9a42      cmp	r2, r3
0802b5da  a361      str	r3, [r4, #24]
0802b5dc  38bf      it	lo
0802b5de  1a46      movlo	r2, r3
0802b5e0  361a      subs	r6, r6, r0
0802b5e2  e260      str	r2, [r4, #12]
0802b5e4  3b68      ldr	r3, [r7]
0802b5e6  0344      add	r3, r0
0802b5e8  3b60      str	r3, [r7]
0802b5ea  79d0      beq	#242 ; -> 0x0802b6e0 ; branch_target=0x0802b6e0
0802b5ec  a269      ldr	r2, [r4, #24]
0802b5ee  c2f30800  ubfx	r0, r2, #0, #9
0802b5f2  0028      cmp	r0, #0
0802b5f4  9dd0      beq	#-198 ; -> 0x0802b532 ; branch_target=0x0802b532
0802b5f6  04f1300a  add.w	r10, r4, #48
0802b5fa  0aeb0003  add.w	r3, r10, r0
0802b5fe  c0f50070  rsb.w	r0, r0, #512
0802b602  b042      cmp	r0, r6
0802b604  28bf      it	hs
0802b606  3046      movhs	r0, r6
0802b608  421e      subs	r2, r0, #1
0802b60a  052a      cmp	r2, #5
0802b60c  03d9      bls	#6 ; -> 0x0802b616 ; branch_target=0x0802b616
0802b60e  6a1c      adds	r2, r5, #1
0802b610  9a1a      subs	r2, r3, r2
0802b612  022a      cmp	r2, #2
0802b614  0ed8      bhi	#28 ; -> 0x0802b634 ; branch_target=0x0802b634
0802b616  013b      subs	r3, #1
0802b618  05eb000c  add.w	r12, r5, r0
0802b61c  2946      mov	r1, r5
0802b61e  11f8012b  ldrb	r2, [r1], #1
0802b622  6145      cmp	r1, r12
0802b624  03f8012f  strb	r2, [r3, #1]!
0802b628  f9d1      bne	#-14 ; -> 0x0802b61e ; branch_target=0x0802b61e
0802b62a  237d      ldrb	r3, [r4, #20]
0802b62c  63f07f03  orn	r3, r3, #127
0802b630  2375      strb	r3, [r4, #20]
0802b632  cde7      b	#-102 ; -> 0x0802b5d0 ; branch_target=0x0802b5d0
0802b634  20f0030e  bic	lr, r0, #3
0802b638  1a46      mov	r2, r3
0802b63a  2946      mov	r1, r5
0802b63c  9e44      add	lr, r3
0802b63e  51f804cb  ldr	r12, [r1], #4
0802b642  42f804cb  str	r12, [r2], #4
0802b646  7245      cmp	r2, lr
0802b648  f9d1      bne	#-14 ; -> 0x0802b63e ; branch_target=0x0802b63e
0802b64a  20f00302  bic	r2, r0, #3
0802b64e  10f0030f  tst.w	r0, #3
0802b652  00f00301  and	r1, r0, #3
0802b656  03eb020a  add.w	r10, r3, r2
0802b65a  05eb020c  add.w	r12, r5, r2
0802b65e  0fd0      beq	#30 ; -> 0x0802b680 ; branch_target=0x0802b680
0802b660  15f802e0  ldrb.w	lr, [r5, r2]
0802b664  0129      cmp	r1, #1
0802b666  03f802e0  strb.w	lr, [r3, r2]
0802b66a  09d0      beq	#18 ; -> 0x0802b680 ; branch_target=0x0802b680
0802b66c  9cf80130  ldrb.w	r3, [r12, #1]
0802b670  0229      cmp	r1, #2
0802b672  8af80130  strb.w	r3, [r10, #1]
0802b676  1cbf      itt	ne
0802b678  9cf80230  ldrbne.w	r3, [r12, #2]
0802b67c  8af80230  strbne.w	r3, [r10, #2]
0802b680  237d      ldrb	r3, [r4, #20]
0802b682  05eb000c  add.w	r12, r5, r0
0802b686  63f07f03  orn	r3, r3, #127
0802b68a  2375      strb	r3, [r4, #20]
0802b68c  a0e7      b	#-192 ; -> 0x0802b5d0 ; branch_target=0x0802b5d0
0802b68e  0223      movs	r3, #2
0802b690  9846      mov	r8, r3
0802b692  6375      strb	r3, [r4, #21]
0802b694  4046      mov	r0, r8
0802b696  03b0      add	sp, #12
0802b698  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b69c  236a      ldr	r3, [r4, #32]
0802b69e  04f1300a  add.w	r10, r4, #48
0802b6a2  a069      ldr	r0, [r4, #24]
0802b6a4  9342      cmp	r3, r2
0802b6a6  02d0      beq	#4 ; -> 0x0802b6ae ; branch_target=0x0802b6ae
0802b6a8  e368      ldr	r3, [r4, #12]
0802b6aa  8342      cmp	r3, r0
0802b6ac  45d8      bhi	#138 ; -> 0x0802b73a ; branch_target=0x0802b73a
0802b6ae  c0f30800  ubfx	r0, r0, #0, #9
0802b6b2  2262      str	r2, [r4, #32]
0802b6b4  a1e7      b	#-190 ; -> 0x0802b5fa ; branch_target=0x0802b5fa
0802b6b6  e16a      ldr	r1, [r4, #44]
0802b6b8  79b3      cbz	r1, #94 ; -> 0x0802b71a ; branch_target=0x0802b71a
0802b6ba  2268      ldr	r2, [r4]
0802b6bc  0431      adds	r1, #4
0802b6be  5289      ldrh	r2, [r2, #10]
0802b6c0  b3fbf2f3  udiv	r3, r3, r2
0802b6c4  0a68      ldr	r2, [r1]
0802b6c6  22b9      cbnz	r2, #8 ; -> 0x0802b6d2 ; branch_target=0x0802b6d2
0802b6c8  0ae0      b	#20 ; -> 0x0802b6e0 ; branch_target=0x0802b6e0
0802b6ca  9b1a      subs	r3, r3, r2
0802b6cc  51f8082f  ldr	r2, [r1, #8]!
0802b6d0  32b1      cbz	r2, #12 ; -> 0x0802b6e0 ; branch_target=0x0802b6e0
0802b6d2  9342      cmp	r3, r2
0802b6d4  f9d2      bhs	#-14 ; -> 0x0802b6ca ; branch_target=0x0802b6ca
0802b6d6  4868      ldr	r0, [r1, #4]
0802b6d8  1844      add	r0, r3
0802b6da  0028      cmp	r0, #0
0802b6dc  7ff438af  bne.w	#-400 ; -> 0x0802b550 ; branch_target=0x0802b550
0802b6e0  237d      ldrb	r3, [r4, #20]
0802b6e2  43f04003  orr	r3, r3, #64
0802b6e6  2375      strb	r3, [r4, #20]
0802b6e8  04e7      b	#-504 ; -> 0x0802b4f4 ; branch_target=0x0802b4f4
0802b6ea  0123      movs	r3, #1
0802b6ec  226a      ldr	r2, [r4, #32]
0802b6ee  04f13001  add.w	r1, r4, #48
0802b6f2  99f80100  ldrb.w	r0, [r9, #1]
0802b6f6  fef70df9  bl	#-7654 ; -> 0x08029914 ; branch_target=0x08029914
0802b6fa  20b9      cbnz	r0, #8 ; -> 0x0802b706 ; branch_target=0x0802b706
0802b6fc  237d      ldrb	r3, [r4, #20]
0802b6fe  03f07f03  and	r3, r3, #127
0802b702  2375      strb	r3, [r4, #20]
0802b704  33e7      b	#-410 ; -> 0x0802b56e ; branch_target=0x0802b56e
0802b706  0123      movs	r3, #1
0802b708  9846      mov	r8, r3
0802b70a  6375      strb	r3, [r4, #21]
0802b70c  4046      mov	r0, r8
0802b70e  03b0      add	sp, #12
0802b710  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b714  4ff00708  mov.w	r8, #7
0802b718  ece6      b	#-552 ; -> 0x0802b4f4 ; branch_target=0x0802b4f4
0802b71a  e169      ldr	r1, [r4, #28]
0802b71c  2046      mov	r0, r4
0802b71e  fef775fe  bl	#-4886 ; -> 0x0802a40c ; branch_target=0x0802a40c
0802b722  0028      cmp	r0, #0
0802b724  7ff414af  bne.w	#-472 ; -> 0x0802b550 ; branch_target=0x0802b550
0802b728  dae7      b	#-76 ; -> 0x0802b6e0 ; branch_target=0x0802b6e0
0802b72a  5146      mov	r1, r10
0802b72c  2046      mov	r0, r4
0802b72e  fef76dfe  bl	#-4902 ; -> 0x0802a40c ; branch_target=0x0802a40c
0802b732  0028      cmp	r0, #0
0802b734  7ff40caf  bne.w	#-488 ; -> 0x0802b550 ; branch_target=0x0802b550
0802b738  d2e7      b	#-92 ; -> 0x0802b6e0 ; branch_target=0x0802b6e0
0802b73a  0123      movs	r3, #1
0802b73c  5146      mov	r1, r10
0802b73e  99f80100  ldrb.w	r0, [r9, #1]
0802b742  0192      str	r2, [sp, #4]
0802b744  fef7d8f8  bl	#-7760 ; -> 0x080298f8 ; branch_target=0x080298f8
0802b748  0028      cmp	r0, #0
0802b74a  dcd1      bne	#-72 ; -> 0x0802b706 ; branch_target=0x0802b706
0802b74c  a069      ldr	r0, [r4, #24]
0802b74e  019a      ldr	r2, [sp, #4]
0802b750  ade7      b	#-166 ; -> 0x0802b6ae ; branch_target=0x0802b6ae
0802b752  05eb4321  add.w	r1, r5, r3, lsl #9
0802b756  04f13003  add.w	r3, r4, #48
0802b75a  0a46      mov	r2, r1
0802b75c  0131      adds	r1, #1
0802b75e  591a      subs	r1, r3, r1
0802b760  0229      cmp	r1, #2
0802b762  0cd9      bls	#24 ; -> 0x0802b77e ; branch_target=0x0802b77e
0802b764  04f50c70  add.w	r0, r4, #560
0802b768  52f8041b  ldr	r1, [r2], #4
0802b76c  43f8041b  str	r1, [r3], #4
0802b770  8342      cmp	r3, r0
0802b772  f9d1      bne	#-14 ; -> 0x0802b768 ; branch_target=0x0802b768
0802b774  237d      ldrb	r3, [r4, #20]
0802b776  03f07f03  and	r3, r3, #127
0802b77a  2375      strb	r3, [r4, #20]
0802b77c  24e7      b	#-440 ; -> 0x0802b5c8 ; branch_target=0x0802b5c8
0802b77e  04f12f03  add.w	r3, r4, #47
0802b782  02f50070  add.w	r0, r2, #512
0802b786  12f8011b  ldrb	r1, [r2], #1
0802b78a  9042      cmp	r0, r2
0802b78c  03f8011f  strb	r1, [r3, #1]!
0802b790  f9d1      bne	#-14 ; -> 0x0802b786 ; branch_target=0x0802b786
0802b792  efe7      b	#-34 ; -> 0x0802b774 ; branch_target=0x0802b774
