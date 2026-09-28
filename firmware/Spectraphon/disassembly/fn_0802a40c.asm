; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802a40c  2de9f84f  push.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802a410  0746      mov	r7, r0
0802a412  8946      mov	r9, r1
0802a414  d0f80080  ldr.w	r8, [r0]
0802a418  0029      cmp	r1, #0
0802a41a  40f0a780  bne.w	#334 ; -> 0x0802a56c ; branch_target=0x0802a56c
0802a41e  d8f80c60  ldr.w	r6, [r8, #12]
0802a422  d8f81430  ldr.w	r3, [r8, #20]
0802a426  c6b1      cbz	r6, #48 ; -> 0x0802a45a ; branch_target=0x0802a45a
0802a428  9e42      cmp	r6, r3
0802a42a  16d2      bhs	#44 ; -> 0x0802a45a ; branch_target=0x0802a45a
0802a42c  741c      adds	r4, r6, #1
0802a42e  a342      cmp	r3, r4
0802a430  00f2b480  bhi.w	#360 ; -> 0x0802a59c ; branch_target=0x0802a59c
0802a434  012e      cmp	r6, #1
0802a436  13d0      beq	#38 ; -> 0x0802a460 ; branch_target=0x0802a460
0802a438  3d68      ldr	r5, [r7]
0802a43a  0224      movs	r4, #2
0802a43c  6b69      ldr	r3, [r5, #20]
0802a43e  a342      cmp	r3, r4
0802a440  07d9      bls	#14 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a442  2b78      ldrb	r3, [r5]
0802a444  022b      cmp	r3, #2
0802a446  00f08380  beq.w	#262 ; -> 0x0802a550 ; branch_target=0x0802a550
0802a44a  032b      cmp	r3, #3
0802a44c  42d0      beq	#132 ; -> 0x0802a4d4 ; branch_target=0x0802a4d4
0802a44e  012b      cmp	r3, #1
0802a450  0ad0      beq	#20 ; -> 0x0802a468 ; branch_target=0x0802a468
0802a452  0124      movs	r4, #1
0802a454  2046      mov	r0, r4
0802a456  bde8f88f  pop.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802a45a  022b      cmp	r3, #2
0802a45c  00f29a80  bhi.w	#308 ; -> 0x0802a594 ; branch_target=0x0802a594
0802a460  0024      movs	r4, #0
0802a462  2046      mov	r0, r4
0802a464  bde8f88f  pop.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802a468  296a      ldr	r1, [r5, #32]
0802a46a  04eb540b  add.w	r11, r4, r4, lsr #1
0802a46e  2846      mov	r0, r5
0802a470  01eb5b21  add.w	r1, r1, r11, lsr #9
0802a474  fff7a2fc  bl	#-1724 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a478  38bb      cbnz	r0, #78 ; -> 0x0802a4ca ; branch_target=0x0802a4ca
0802a47a  cbf30803  ubfx	r3, r11, #0, #9
0802a47e  296a      ldr	r1, [r5, #32]
0802a480  0bf1010b  add.w	r11, r11, #1
0802a484  2846      mov	r0, r5
0802a486  2b44      add	r3, r5
0802a488  01eb5b21  add.w	r1, r1, r11, lsr #9
0802a48c  93f830a0  ldrb.w	r10, [r3, #48]
0802a490  fff794fc  bl	#-1752 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a494  c8b9      cbnz	r0, #50 ; -> 0x0802a4ca ; branch_target=0x0802a4ca
0802a496  cbf3080b  ubfx	r11, r11, #0, #9
0802a49a  e207      lsls	r2, r4, #31
0802a49c  ab44      add	r11, r5
0802a49e  9bf83030  ldrb.w	r3, [r11, #48]
0802a4a2  4aea0323  orr.w	r3, r10, r3, lsl #8
0802a4a6  4cbf      ite	mi
0802a4a8  1b09      lsrmi	r3, r3, #4
0802a4aa  c3f30b03  ubfxpl	r3, r3, #0, #12
0802a4ae  13b3      cbz	r3, #68 ; -> 0x0802a4f6 ; branch_target=0x0802a4f6
0802a4b0  012b      cmp	r3, #1
0802a4b2  ced0      beq	#-100 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a4b4  a642      cmp	r6, r4
0802a4b6  d3d0      beq	#-90 ; -> 0x0802a460 ; branch_target=0x0802a460
0802a4b8  0134      adds	r4, #1
0802a4ba  d8f81430  ldr.w	r3, [r8, #20]
0802a4be  9c42      cmp	r4, r3
0802a4c0  b8d2      bhs	#-144 ; -> 0x0802a434 ; branch_target=0x0802a434
0802a4c2  3d68      ldr	r5, [r7]
0802a4c4  bae7      b	#-140 ; -> 0x0802a43c ; branch_target=0x0802a43c
0802a4c6  0128      cmp	r0, #1
0802a4c8  c3d1      bne	#-122 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a4ca  4ff0ff34  mov.w	r4, #4294967295
0802a4ce  2046      mov	r0, r4
0802a4d0  bde8f88f  pop.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802a4d4  296a      ldr	r1, [r5, #32]
0802a4d6  2846      mov	r0, r5
0802a4d8  01ebd411  add.w	r1, r1, r4, lsr #7
0802a4dc  fff76efc  bl	#-1828 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a4e0  0028      cmp	r0, #0
0802a4e2  f2d1      bne	#-28 ; -> 0x0802a4ca ; branch_target=0x0802a4ca
0802a4e4  a300      lsls	r3, r4, #2
0802a4e6  03f4fe73  and	r3, r3, #508
0802a4ea  2b44      add	r3, r5
0802a4ec  1b6b      ldr	r3, [r3, #48]
0802a4ee  23f07043  bic	r3, r3, #4026531840
0802a4f2  002b      cmp	r3, #0
0802a4f4  dcd1      bne	#-72 ; -> 0x0802a4b0 ; branch_target=0x0802a4b0
0802a4f6  d8f81430  ldr.w	r3, [r8, #20]
0802a4fa  a342      cmp	r3, r4
0802a4fc  a9d9      bls	#-174 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a4fe  4ff0ff32  mov.w	r2, #4294967295
0802a502  2146      mov	r1, r4
0802a504  4046      mov	r0, r8
0802a506  fff701fe  bl	#-1022 ; -> 0x0802a10c ; branch_target=0x0802a10c
0802a50a  70b9      cbnz	r0, #28 ; -> 0x0802a52a ; branch_target=0x0802a52a
0802a50c  b9f1000f  cmp.w	r9, #0
0802a510  0bd0      beq	#22 ; -> 0x0802a52a ; branch_target=0x0802a52a
0802a512  b9f1010f  cmp.w	r9, #1
0802a516  9cd0      beq	#-200 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a518  d8f81430  ldr.w	r3, [r8, #20]
0802a51c  9945      cmp	r9, r3
0802a51e  98d2      bhs	#-208 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a520  2246      mov	r2, r4
0802a522  4946      mov	r1, r9
0802a524  4046      mov	r0, r8
0802a526  fff7f1fd  bl	#-1054 ; -> 0x0802a10c ; branch_target=0x0802a10c
0802a52a  0028      cmp	r0, #0
0802a52c  cbd1      bne	#-106 ; -> 0x0802a4c6 ; branch_target=0x0802a4c6
0802a52e  c8f80c40  str.w	r4, [r8, #12]
0802a532  d8e90423  ldrd	r2, r3, [r8, #16]
0802a536  023b      subs	r3, #2
0802a538  9a42      cmp	r2, r3
0802a53a  02d8      bhi	#4 ; -> 0x0802a542 ; branch_target=0x0802a542
0802a53c  013a      subs	r2, #1
0802a53e  c8f81020  str.w	r2, [r8, #16]
0802a542  98f80430  ldrb.w	r3, [r8, #4]
0802a546  43f00103  orr	r3, r3, #1
0802a54a  88f80430  strb.w	r3, [r8, #4]
0802a54e  81e7      b	#-254 ; -> 0x0802a454 ; branch_target=0x0802a454
0802a550  296a      ldr	r1, [r5, #32]
0802a552  2846      mov	r0, r5
0802a554  01eb1421  add.w	r1, r1, r4, lsr #8
0802a558  fff730fc  bl	#-1952 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a55c  0028      cmp	r0, #0
0802a55e  b4d1      bne	#-152 ; -> 0x0802a4ca ; branch_target=0x0802a4ca
0802a560  6300      lsls	r3, r4, #1
0802a562  03f4ff73  and	r3, r3, #510
0802a566  2b44      add	r3, r5
0802a568  1b8e      ldrh	r3, [r3, #48]
0802a56a  a0e7      b	#-192 ; -> 0x0802a4ae ; branch_target=0x0802a4ae
0802a56c  4046      mov	r0, r8
0802a56e  fff74dfe  bl	#-870 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802a572  0128      cmp	r0, #1
0802a574  0446      mov	r4, r0
0802a576  7ff66caf  bls.w	#-296 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a57a  411c      adds	r1, r0, #1
0802a57c  a5d0      beq	#-182 ; -> 0x0802a4ca ; branch_target=0x0802a4ca
0802a57e  d8f81430  ldr.w	r3, [r8, #20]
0802a582  8342      cmp	r3, r0
0802a584  3ff666af  bhi.w	#-308 ; -> 0x0802a454 ; branch_target=0x0802a454
0802a588  09f10104  add.w	r4, r9, #1
0802a58c  a342      cmp	r3, r4
0802a58e  07d8      bhi	#14 ; -> 0x0802a5a0 ; branch_target=0x0802a5a0
0802a590  4e46      mov	r6, r9
0802a592  4fe7      b	#-354 ; -> 0x0802a434 ; branch_target=0x0802a434
0802a594  3d68      ldr	r5, [r7]
0802a596  0126      movs	r6, #1
0802a598  0224      movs	r4, #2
0802a59a  4fe7      b	#-354 ; -> 0x0802a43c ; branch_target=0x0802a43c
0802a59c  4546      mov	r5, r8
0802a59e  4de7      b	#-358 ; -> 0x0802a43c ; branch_target=0x0802a43c
0802a5a0  012c      cmp	r4, #1
0802a5a2  3d68      ldr	r5, [r7]
0802a5a4  7ff655af  bls.w	#-342 ; -> 0x0802a452 ; branch_target=0x0802a452
0802a5a8  4e46      mov	r6, r9
0802a5aa  47e7      b	#-370 ; -> 0x0802a43c ; branch_target=0x0802a43c
