; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08024184  f8b5      push	{r3, r4, r5, r6, r7, lr}
08024186  364c      ldr	r4, [pc, #216] ; [0x08024260] = 0x58024400
08024188  0646      mov	r6, r0
0802418a  0f46      mov	r7, r1
0802418c  2368      ldr	r3, [r4]
0802418e  23f08053  bic	r3, r3, #268435456
08024192  2360      str	r3, [r4]
08024194  fcf702f9  bl	#-15868 ; -> 0x0802039c ; branch_target=0x0802039c
08024198  0546      mov	r5, r0
0802419a  04e0      b	#8 ; -> 0x080241a6 ; branch_target=0x080241a6
0802419c  fcf7fef8  bl	#-15876 ; -> 0x0802039c ; branch_target=0x0802039c
080241a0  401b      subs	r0, r0, r5
080241a2  0228      cmp	r0, #2
080241a4  56d8      bhi	#172 ; -> 0x08024254 ; branch_target=0x08024254
080241a6  2368      ldr	r3, [r4]
080241a8  9a00      lsls	r2, r3, #2
080241aa  f7d4      bmi	#-18 ; -> 0x0802419c ; branch_target=0x0802419c
080241ac  a36a      ldr	r3, [r4, #40]
080241ae  3268      ldr	r2, [r6]
080241b0  23f07c73  bic	r3, r3, #66060288
080241b4  43ea0253  orr.w	r3, r3, r2, lsl #20
080241b8  a362      str	r3, [r4, #40]
080241ba  d6e90232  ldrd	r3, r2, [r6, #8]
080241be  013b      subs	r3, #1
080241c0  013a      subs	r2, #1
080241c2  5b02      lsls	r3, r3, #9
080241c4  1204      lsls	r2, r2, #16
080241c6  9bb2      uxth	r3, r3
080241c8  02f4fe02  and	r2, r2, #8323072
080241cc  1343      orrs	r3, r2
080241ce  7268      ldr	r2, [r6, #4]
080241d0  013a      subs	r2, #1
080241d2  c2f30802  ubfx	r2, r2, #0, #9
080241d6  1343      orrs	r3, r2
080241d8  3269      ldr	r2, [r6, #16]
080241da  013a      subs	r2, #1
080241dc  1206      lsls	r2, r2, #24
080241de  02f0fe42  and	r2, r2, #2130706432
080241e2  1343      orrs	r3, r2
080241e4  2364      str	r3, [r4, #64]
080241e6  e36a      ldr	r3, [r4, #44]
080241e8  7269      ldr	r2, [r6, #20]
080241ea  23f44063  bic	r3, r3, #3072
080241ee  1343      orrs	r3, r2
080241f0  e362      str	r3, [r4, #44]
080241f2  e26a      ldr	r2, [r4, #44]
080241f4  b369      ldr	r3, [r6, #24]
080241f6  22f40072  bic	r2, r2, #512
080241fa  1a43      orrs	r2, r3
080241fc  194b      ldr	r3, [pc, #100] ; [0x08024264] = 0xffff0007
080241fe  e262      str	r2, [r4, #44]
08024200  e26a      ldr	r2, [r4, #44]
08024202  22f48072  bic	r2, r2, #256
08024206  e262      str	r2, [r4, #44]
08024208  616c      ldr	r1, [r4, #68]
0802420a  f269      ldr	r2, [r6, #28]
0802420c  0b40      ands	r3, r1
0802420e  43eac203  orr.w	r3, r3, r2, lsl #3
08024212  6364      str	r3, [r4, #68]
08024214  e36a      ldr	r3, [r4, #44]
08024216  43f48073  orr	r3, r3, #256
0802421a  e362      str	r3, [r4, #44]
0802421c  e36a      ldr	r3, [r4, #44]
0802421e  dfb1      cbz	r7, #54 ; -> 0x08024258 ; branch_target=0x08024258
08024220  012f      cmp	r7, #1
08024222  0cbf      ite	eq
08024224  43f40003  orreq	r3, r3, #8388608
08024228  43f08073  orrne	r3, r3, #16777216
0802422c  e362      str	r3, [r4, #44]
0802422e  0c4c      ldr	r4, [pc, #48] ; [0x08024260] = 0x58024400
08024230  2368      ldr	r3, [r4]
08024232  43f08053  orr	r3, r3, #268435456
08024236  2360      str	r3, [r4]
08024238  fcf7b0f8  bl	#-16032 ; -> 0x0802039c ; branch_target=0x0802039c
0802423c  0546      mov	r5, r0
0802423e  04e0      b	#8 ; -> 0x0802424a ; branch_target=0x0802424a
08024240  fcf7acf8  bl	#-16040 ; -> 0x0802039c ; branch_target=0x0802039c
08024244  401b      subs	r0, r0, r5
08024246  0228      cmp	r0, #2
08024248  04d8      bhi	#8 ; -> 0x08024254 ; branch_target=0x08024254
0802424a  2368      ldr	r3, [r4]
0802424c  9b00      lsls	r3, r3, #2
0802424e  f7d5      bpl	#-18 ; -> 0x08024240 ; branch_target=0x08024240
08024250  0020      movs	r0, #0
08024252  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08024254  0320      movs	r0, #3
08024256  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08024258  43f48003  orr	r3, r3, #4194304
0802425c  e362      str	r3, [r4, #44]
0802425e  e6e7      b	#-52 ; -> 0x0802422e ; branch_target=0x0802422e
