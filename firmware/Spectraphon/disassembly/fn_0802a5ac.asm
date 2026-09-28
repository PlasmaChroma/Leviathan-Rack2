; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802a5ac  2de9f843  push.w	{r3, r4, r5, r6, r7, r8, r9, lr}
0802a5b0  0023      movs	r3, #0
0802a5b2  8468      ldr	r4, [r0, #8]
0802a5b4  0646      mov	r6, r0
0802a5b6  0768      ldr	r7, [r0]
0802a5b8  4361      str	r3, [r0, #20]
0802a5ba  8cb9      cbnz	r4, #34 ; -> 0x0802a5e0 ; branch_target=0x0802a5e0
0802a5bc  3b78      ldrb	r3, [r7]
0802a5be  022b      cmp	r3, #2
0802a5c0  0ad8      bhi	#20 ; -> 0x0802a5d8 ; branch_target=0x0802a5d8
0802a5c2  3b89      ldrh	r3, [r7, #8]
0802a5c4  8bb1      cbz	r3, #34 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a5c6  7b6a      ldr	r3, [r7, #36]
0802a5c8  c6e90643  strd	r4, r3, [r6, #24]
0802a5cc  6bb1      cbz	r3, #26 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a5ce  3037      adds	r7, #48
0802a5d0  0020      movs	r0, #0
0802a5d2  3762      str	r7, [r6, #32]
0802a5d4  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802a5d8  7b6a      ldr	r3, [r7, #36]
0802a5da  002b      cmp	r3, #0
0802a5dc  f1d0      beq	#-30 ; -> 0x0802a5c2 ; branch_target=0x0802a5c2
0802a5de  1c46      mov	r4, r3
0802a5e0  7b89      ldrh	r3, [r7, #10]
0802a5e2  002b      cmp	r3, #0
0802a5e4  5cd1      bne	#184 ; -> 0x0802a6a0 ; branch_target=0x0802a6a0
0802a5e6  012c      cmp	r4, #1
0802a5e8  02d1      bne	#4 ; -> 0x0802a5f0 ; branch_target=0x0802a5f0
0802a5ea  0220      movs	r0, #2
0802a5ec  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802a5f0  3d46      mov	r5, r7
0802a5f2  6b69      ldr	r3, [r5, #20]
0802a5f4  a342      cmp	r3, r4
0802a5f6  f8d9      bls	#-16 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a5f8  2b78      ldrb	r3, [r5]
0802a5fa  022b      cmp	r3, #2
0802a5fc  25d0      beq	#74 ; -> 0x0802a64a ; branch_target=0x0802a64a
0802a5fe  032b      cmp	r3, #3
0802a600  0dd0      beq	#26 ; -> 0x0802a61e ; branch_target=0x0802a61e
0802a602  012b      cmp	r3, #1
0802a604  f1d1      bne	#-30 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a606  04eb5408  add.w	r8, r4, r4, lsr #1
0802a60a  296a      ldr	r1, [r5, #32]
0802a60c  2846      mov	r0, r5
0802a60e  01eb5821  add.w	r1, r1, r8, lsr #9
0802a612  fff7d3fb  bl	#-2138 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a616  30b3      cbz	r0, #76 ; -> 0x0802a666 ; branch_target=0x0802a666
0802a618  0120      movs	r0, #1
0802a61a  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802a61e  296a      ldr	r1, [r5, #32]
0802a620  2846      mov	r0, r5
0802a622  01ebd411  add.w	r1, r1, r4, lsr #7
0802a626  fff7c9fb  bl	#-2158 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a62a  0028      cmp	r0, #0
0802a62c  f4d1      bne	#-24 ; -> 0x0802a618 ; branch_target=0x0802a618
0802a62e  a300      lsls	r3, r4, #2
0802a630  03f4fe73  and	r3, r3, #508
0802a634  2b44      add	r3, r5
0802a636  1c6b      ldr	r4, [r3, #48]
0802a638  24f07044  bic	r4, r4, #4026531840
0802a63c  012c      cmp	r4, #1
0802a63e  d4d9      bls	#-88 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a640  7b69      ldr	r3, [r7, #20]
0802a642  9c42      cmp	r4, r3
0802a644  d1d2      bhs	#-94 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
0802a646  3568      ldr	r5, [r6]
0802a648  d3e7      b	#-90 ; -> 0x0802a5f2 ; branch_target=0x0802a5f2
0802a64a  296a      ldr	r1, [r5, #32]
0802a64c  2846      mov	r0, r5
0802a64e  01eb1421  add.w	r1, r1, r4, lsr #8
0802a652  fff7b3fb  bl	#-2202 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a656  0028      cmp	r0, #0
0802a658  ded1      bne	#-68 ; -> 0x0802a618 ; branch_target=0x0802a618
0802a65a  6300      lsls	r3, r4, #1
0802a65c  03f4ff73  and	r3, r3, #510
0802a660  2b44      add	r3, r5
0802a662  1c8e      ldrh	r4, [r3, #48]
0802a664  eae7      b	#-44 ; -> 0x0802a63c ; branch_target=0x0802a63c
0802a666  c8f30803  ubfx	r3, r8, #0, #9
0802a66a  296a      ldr	r1, [r5, #32]
0802a66c  08f10108  add.w	r8, r8, #1
0802a670  2846      mov	r0, r5
0802a672  2b44      add	r3, r5
0802a674  01eb5821  add.w	r1, r1, r8, lsr #9
0802a678  93f83090  ldrb.w	r9, [r3, #48]
0802a67c  fff79efb  bl	#-2244 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a680  0028      cmp	r0, #0
0802a682  c9d1      bne	#-110 ; -> 0x0802a618 ; branch_target=0x0802a618
0802a684  c8f30808  ubfx	r8, r8, #0, #9
0802a688  a844      add	r8, r5
0802a68a  98f83030  ldrb.w	r3, [r8, #48]
0802a68e  49ea0329  orr.w	r9, r9, r3, lsl #8
0802a692  e307      lsls	r3, r4, #31
0802a694  4cbf      ite	mi
0802a696  4fea1914  lsrmi.w	r4, r9, #4
0802a69a  c9f30b04  ubfxpl	r4, r9, #0, #12
0802a69e  cde7      b	#-102 ; -> 0x0802a63c ; branch_target=0x0802a63c
0802a6a0  7a69      ldr	r2, [r7, #20]
0802a6a2  a11e      subs	r1, r4, #2
0802a6a4  023a      subs	r2, #2
0802a6a6  9142      cmp	r1, r2
0802a6a8  03d2      bhs	#6 ; -> 0x0802a6b2 ; branch_target=0x0802a6b2
0802a6aa  ba6a      ldr	r2, [r7, #40]
0802a6ac  01fb0323  mla	r3, r1, r3, r2
0802a6b0  8ae7      b	#-236 ; -> 0x0802a5c8 ; branch_target=0x0802a5c8
0802a6b2  0023      movs	r3, #0
0802a6b4  c6e90643  strd	r4, r3, [r6, #24]
0802a6b8  97e7      b	#-210 ; -> 0x0802a5ea ; branch_target=0x0802a5ea
