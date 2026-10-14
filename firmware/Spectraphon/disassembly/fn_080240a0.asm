; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080240a0  f8b5      push	{r3, r4, r5, r6, r7, lr}
080240a2  364c      ldr	r4, [pc, #216] ; [0x0802417c] = 0x58024400
080240a4  0646      mov	r6, r0
080240a6  0f46      mov	r7, r1
080240a8  2368      ldr	r3, [r4]
080240aa  23f08063  bic	r3, r3, #67108864
080240ae  2360      str	r3, [r4]
080240b0  fcf774f9  bl	#-15640 ; -> 0x0802039c ; branch_target=0x0802039c
080240b4  0546      mov	r5, r0
080240b6  04e0      b	#8 ; -> 0x080240c2 ; branch_target=0x080240c2
080240b8  fcf770f9  bl	#-15648 ; -> 0x0802039c ; branch_target=0x0802039c
080240bc  401b      subs	r0, r0, r5
080240be  0228      cmp	r0, #2
080240c0  56d8      bhi	#172 ; -> 0x08024170 ; branch_target=0x08024170
080240c2  2368      ldr	r3, [r4]
080240c4  1a01      lsls	r2, r3, #4
080240c6  f7d4      bmi	#-18 ; -> 0x080240b8 ; branch_target=0x080240b8
080240c8  a36a      ldr	r3, [r4, #40]
080240ca  3268      ldr	r2, [r6]
080240cc  23f47c33  bic	r3, r3, #258048
080240d0  43ea0233  orr.w	r3, r3, r2, lsl #12
080240d4  a362      str	r3, [r4, #40]
080240d6  d6e90232  ldrd	r3, r2, [r6, #8]
080240da  013b      subs	r3, #1
080240dc  013a      subs	r2, #1
080240de  5b02      lsls	r3, r3, #9
080240e0  1204      lsls	r2, r2, #16
080240e2  9bb2      uxth	r3, r3
080240e4  02f4fe02  and	r2, r2, #8323072
080240e8  1343      orrs	r3, r2
080240ea  7268      ldr	r2, [r6, #4]
080240ec  013a      subs	r2, #1
080240ee  c2f30802  ubfx	r2, r2, #0, #9
080240f2  1343      orrs	r3, r2
080240f4  3269      ldr	r2, [r6, #16]
080240f6  013a      subs	r2, #1
080240f8  1206      lsls	r2, r2, #24
080240fa  02f0fe42  and	r2, r2, #2130706432
080240fe  1343      orrs	r3, r2
08024100  a363      str	r3, [r4, #56]
08024102  e36a      ldr	r3, [r4, #44]
08024104  7269      ldr	r2, [r6, #20]
08024106  23f0c003  bic	r3, r3, #192
0802410a  1343      orrs	r3, r2
0802410c  e362      str	r3, [r4, #44]
0802410e  e26a      ldr	r2, [r4, #44]
08024110  b369      ldr	r3, [r6, #24]
08024112  22f02002  bic	r2, r2, #32
08024116  1a43      orrs	r2, r3
08024118  194b      ldr	r3, [pc, #100] ; [0x08024180] = 0xffff0007
0802411a  e262      str	r2, [r4, #44]
0802411c  e26a      ldr	r2, [r4, #44]
0802411e  22f01002  bic	r2, r2, #16
08024122  e262      str	r2, [r4, #44]
08024124  e16b      ldr	r1, [r4, #60]
08024126  f269      ldr	r2, [r6, #28]
08024128  0b40      ands	r3, r1
0802412a  43eac203  orr.w	r3, r3, r2, lsl #3
0802412e  e363      str	r3, [r4, #60]
08024130  e36a      ldr	r3, [r4, #44]
08024132  43f01003  orr	r3, r3, #16
08024136  e362      str	r3, [r4, #44]
08024138  e36a      ldr	r3, [r4, #44]
0802413a  dfb1      cbz	r7, #54 ; -> 0x08024174 ; branch_target=0x08024174
0802413c  012f      cmp	r7, #1
0802413e  0cbf      ite	eq
08024140  43f48013  orreq	r3, r3, #1048576
08024144  43f40013  orrne	r3, r3, #2097152
08024148  e362      str	r3, [r4, #44]
0802414a  0c4c      ldr	r4, [pc, #48] ; [0x0802417c] = 0x58024400
0802414c  2368      ldr	r3, [r4]
0802414e  43f08063  orr	r3, r3, #67108864
08024152  2360      str	r3, [r4]
08024154  fcf722f9  bl	#-15804 ; -> 0x0802039c ; branch_target=0x0802039c
08024158  0546      mov	r5, r0
0802415a  04e0      b	#8 ; -> 0x08024166 ; branch_target=0x08024166
0802415c  fcf71ef9  bl	#-15812 ; -> 0x0802039c ; branch_target=0x0802039c
08024160  401b      subs	r0, r0, r5
08024162  0228      cmp	r0, #2
08024164  04d8      bhi	#8 ; -> 0x08024170 ; branch_target=0x08024170
08024166  2368      ldr	r3, [r4]
08024168  1b01      lsls	r3, r3, #4
0802416a  f7d5      bpl	#-18 ; -> 0x0802415c ; branch_target=0x0802415c
0802416c  0020      movs	r0, #0
0802416e  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08024170  0320      movs	r0, #3
08024172  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08024174  43f40023  orr	r3, r3, #524288
08024178  e362      str	r3, [r4, #44]
0802417a  e6e7      b	#-52 ; -> 0x0802414a ; branch_target=0x0802414a
