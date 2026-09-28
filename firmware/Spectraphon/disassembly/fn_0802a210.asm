; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0802a210  f8b5      push	{r3, r4, r5, r6, r7, lr}
0802a212  4369      ldr	r3, [r0, #20]
0802a214  0c46      mov	r4, r1
0802a216  0546      mov	r5, r0
0802a218  9942      cmp	r1, r3
0802a21a  06d2      bhs	#12 ; -> 0x0802a22a ; branch_target=0x0802a22a
0802a21c  0378      ldrb	r3, [r0]
0802a21e  022b      cmp	r3, #2
0802a220  21d0      beq	#66 ; -> 0x0802a266 ; branch_target=0x0802a266
0802a222  032b      cmp	r3, #3
0802a224  10d0      beq	#32 ; -> 0x0802a248 ; branch_target=0x0802a248
0802a226  012b      cmp	r3, #1
0802a228  03d0      beq	#6 ; -> 0x0802a232 ; branch_target=0x0802a232
0802a22a  0120      movs	r0, #1
0802a22c  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802a232  016a      ldr	r1, [r0, #32]
0802a234  04eb5407  add.w	r7, r4, r4, lsr #1
0802a238  01eb5721  add.w	r1, r1, r7, lsr #9
0802a23c  fff7befd  bl	#-1156 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a240  f0b1      cbz	r0, #60 ; -> 0x0802a280 ; branch_target=0x0802a280
0802a242  4ff0ff30  mov.w	r0, #4294967295
0802a246  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802a248  016a      ldr	r1, [r0, #32]
0802a24a  01ebd411  add.w	r1, r1, r4, lsr #7
0802a24e  fff7b5fd  bl	#-1174 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a252  0028      cmp	r0, #0
0802a254  f5d1      bne	#-22 ; -> 0x0802a242 ; branch_target=0x0802a242
0802a256  a400      lsls	r4, r4, #2
0802a258  04f4fe74  and	r4, r4, #508
0802a25c  2544      add	r5, r4
0802a25e  286b      ldr	r0, [r5, #48]
0802a260  20f07040  bic	r0, r0, #4026531840
0802a264  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802a266  016a      ldr	r1, [r0, #32]
0802a268  01eb1421  add.w	r1, r1, r4, lsr #8
0802a26c  fff7a6fd  bl	#-1204 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a270  0028      cmp	r0, #0
0802a272  e6d1      bne	#-52 ; -> 0x0802a242 ; branch_target=0x0802a242
0802a274  6300      lsls	r3, r4, #1
0802a276  03f4ff73  and	r3, r3, #510
0802a27a  2b44      add	r3, r5
0802a27c  188e      ldrh	r0, [r3, #48]
0802a27e  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802a280  c7f30803  ubfx	r3, r7, #0, #9
0802a284  296a      ldr	r1, [r5, #32]
0802a286  0137      adds	r7, #1
0802a288  2846      mov	r0, r5
0802a28a  2b44      add	r3, r5
0802a28c  01eb5721  add.w	r1, r1, r7, lsr #9
0802a290  93f83060  ldrb.w	r6, [r3, #48]
0802a294  fff792fd  bl	#-1244 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a298  0028      cmp	r0, #0
0802a29a  d2d1      bne	#-92 ; -> 0x0802a242 ; branch_target=0x0802a242
0802a29c  c7f30807  ubfx	r7, r7, #0, #9
0802a2a0  2f44      add	r7, r5
0802a2a2  97f83030  ldrb.w	r3, [r7, #48]
0802a2a6  46ea0320  orr.w	r0, r6, r3, lsl #8
0802a2aa  e307      lsls	r3, r4, #31
0802a2ac  4cbf      ite	mi
0802a2ae  0009      lsrmi	r0, r0, #4
0802a2b0  c0f30b00  ubfxpl	r0, r0, #0, #12
0802a2b4  f8bd      pop	{r3, r4, r5, r6, r7, pc}
