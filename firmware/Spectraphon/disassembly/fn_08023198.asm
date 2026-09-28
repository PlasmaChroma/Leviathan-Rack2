; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08023198  0a68      ldr	r2, [r1]
0802319a  002a      cmp	r2, #0
0802319c  00f00681  beq.w	#524 ; -> 0x080233ac ; branch_target=0x080233ac
080231a0  0023      movs	r3, #0
080231a2  4ff0b04c  mov.w	r12, #1476395008
080231a6  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
080231aa  0124      movs	r4, #1
080231ac  83b0      sub	sp, #12
080231ae  8a4d      ldr	r5, [pc, #552] ; [0x080233d8] = 0x58024400
080231b0  04fa03f9  lsl.w	r9, r4, r3
080231b4  19ea0208  ands.w	r8, r9, r2
080231b8  00f0cd80  beq.w	#410 ; -> 0x08023356 ; branch_target=0x08023356
080231bc  4a68      ldr	r2, [r1, #4]
080231be  4fea430e  lsl.w	lr, r3, #1
080231c2  0326      movs	r6, #3
080231c4  22f01002  bic	r2, r2, #16
080231c8  06fa0ef7  lsl.w	r7, r6, lr
080231cc  013a      subs	r2, #1
080231ce  ff43      mvns	r7, r7
080231d0  012a      cmp	r2, #1
080231d2  13d8      bhi	#38 ; -> 0x080231fc ; branch_target=0x080231fc
080231d4  8268      ldr	r2, [r0, #8]
080231d6  02ea070a  and.w	r10, r2, r7
080231da  ca68      ldr	r2, [r1, #12]
080231dc  02fa0ef2  lsl.w	r2, r2, lr
080231e0  42ea0a02  orr.w	r2, r2, r10
080231e4  8260      str	r2, [r0, #8]
080231e6  4a68      ldr	r2, [r1, #4]
080231e8  d0f804a0  ldr.w	r10, [r0, #4]
080231ec  c2f30012  ubfx	r2, r2, #4, #1
080231f0  2aea0909  bic.w	r9, r10, r9
080231f4  9a40      lsls	r2, r3
080231f6  42ea0902  orr.w	r2, r2, r9
080231fa  4260      str	r2, [r0, #4]
080231fc  c268      ldr	r2, [r0, #12]
080231fe  02ea0709  and.w	r9, r2, r7
08023202  8a68      ldr	r2, [r1, #8]
08023204  02fa0ef2  lsl.w	r2, r2, lr
08023208  42ea0902  orr.w	r2, r2, r9
0802320c  c260      str	r2, [r0, #12]
0802320e  4a68      ldr	r2, [r1, #4]
08023210  22f01009  bic	r9, r2, #16
08023214  b9f1020f  cmp.w	r9, #2
08023218  16d1      bne	#44 ; -> 0x08023248 ; branch_target=0x08023248
0802321a  4fead30a  lsr.w	r10, r3, #3
0802321e  03f00709  and	r9, r3, #7
08023222  0a69      ldr	r2, [r1, #16]
08023224  0f26      movs	r6, #15
08023226  00eb8a0a  add.w	r10, r0, r10, lsl #2
0802322a  4fea8909  lsl.w	r9, r9, #2
0802322e  daf820b0  ldr.w	r11, [r10, #32]
08023232  02fa09f2  lsl.w	r2, r2, r9
08023236  06fa09f9  lsl.w	r9, r6, r9
0802323a  2bea090b  bic.w	r11, r11, r9
0802323e  42ea0b02  orr.w	r2, r2, r11
08023242  caf82020  str.w	r2, [r10, #32]
08023246  4a68      ldr	r2, [r1, #4]
08023248  02f00302  and	r2, r2, #3
0802324c  02fa0ef2  lsl.w	r2, r2, lr
08023250  d0f800e0  ldr.w	lr, [r0]
08023254  0eea0707  and.w	r7, lr, r7
08023258  3a43      orrs	r2, r7
0802325a  0260      str	r2, [r0]
0802325c  4a68      ldr	r2, [r1, #4]
0802325e  d600      lsls	r6, r2, #3
08023260  78d5      bpl	#240 ; -> 0x08023354 ; branch_target=0x08023354
08023262  d5f8f420  ldr.w	r2, [r5, #244]
08023266  23f00307  bic	r7, r3, #3
0802326a  03f0030e  and	lr, r3, #3
0802326e  0f26      movs	r6, #15
08023270  42f00202  orr	r2, r2, #2
08023274  07f1b047  add.w	r7, r7, #1476395008
08023278  4fea8e0e  lsl.w	lr, lr, #2
0802327c  c5f8f420  str.w	r2, [r5, #244]
08023280  07f58067  add.w	r7, r7, #1024
08023284  d5f8f420  ldr.w	r2, [r5, #244]
08023288  06fa0ef9  lsl.w	r9, r6, lr
0802328c  534e      ldr	r6, [pc, #332] ; [0x080233dc] = 0x58020000
0802328e  02f00202  and	r2, r2, #2
08023292  b042      cmp	r0, r6
08023294  0192      str	r2, [sp, #4]
08023296  019a      ldr	r2, [sp, #4]
08023298  ba68      ldr	r2, [r7, #8]
0802329a  22ea0902  bic.w	r2, r2, r9
0802329e  2ad0      beq	#84 ; -> 0x080232f6 ; branch_target=0x080232f6
080232a0  06f58066  add.w	r6, r6, #1024
080232a4  b042      cmp	r0, r6
080232a6  65d0      beq	#202 ; -> 0x08023374 ; branch_target=0x08023374
080232a8  4d4e      ldr	r6, [pc, #308] ; [0x080233e0] = 0x58020800
080232aa  b042      cmp	r0, r6
080232ac  69d0      beq	#210 ; -> 0x08023382 ; branch_target=0x08023382
080232ae  dff83491  ldr.w	r9, [pc, #308] ; [0x080233e4] = 0x58020c00
080232b2  4845      cmp	r0, r9
080232b4  57d0      beq	#174 ; -> 0x08023366 ; branch_target=0x08023366
080232b6  dff83091  ldr.w	r9, [pc, #304] ; [0x080233e8] = 0x58021000
080232ba  4845      cmp	r0, r9
080232bc  6fd0      beq	#222 ; -> 0x0802339e ; branch_target=0x0802339e
080232be  dff82c91  ldr.w	r9, [pc, #300] ; [0x080233ec] = 0x58021400
080232c2  4845      cmp	r0, r9
080232c4  73d0      beq	#230 ; -> 0x080233ae ; branch_target=0x080233ae
080232c6  dff82891  ldr.w	r9, [pc, #296] ; [0x080233f0] = 0x58021800
080232ca  4845      cmp	r0, r9
080232cc  60d0      beq	#192 ; -> 0x08023390 ; branch_target=0x08023390
080232ce  dff82491  ldr.w	r9, [pc, #292] ; [0x080233f4] = 0x58021c00
080232d2  4845      cmp	r0, r9
080232d4  72d0      beq	#228 ; -> 0x080233bc ; branch_target=0x080233bc
080232d6  dff82091  ldr.w	r9, [pc, #288] ; [0x080233f8] = 0x58022000
080232da  4845      cmp	r0, r9
080232dc  75d0      beq	#234 ; -> 0x080233ca ; branch_target=0x080233ca
080232de  dff81c91  ldr.w	r9, [pc, #284] ; [0x080233fc] = 0x58022400
080232e2  4845      cmp	r0, r9
080232e4  0cbf      ite	eq
080232e6  4ff00909  moveq.w	r9, #9
080232ea  4ff00a09  movne.w	r9, #10
080232ee  09fa0efe  lsl.w	lr, r9, lr
080232f2  42ea0e02  orr.w	r2, r2, lr
080232f6  ba60      str	r2, [r7, #8]
080232f8  6fea080e  mvn.w	lr, r8
080232fc  4a68      ldr	r2, [r1, #4]
080232fe  dcf88070  ldr.w	r7, [r12, #128]
08023302  d203      lsls	r2, r2, #15
08023304  54bf      ite	pl
08023306  0eea0707  andpl.w	r7, lr, r7
0802330a  48ea0707  orrmi.w	r7, r8, r7
0802330e  ccf88070  str.w	r7, [r12, #128]
08023312  4a68      ldr	r2, [r1, #4]
08023314  dcf88470  ldr.w	r7, [r12, #132]
08023318  9603      lsls	r6, r2, #14
0802331a  54bf      ite	pl
0802331c  0eea0707  andpl.w	r7, lr, r7
08023320  48ea0707  orrmi.w	r7, r8, r7
08023324  ccf88470  str.w	r7, [r12, #132]
08023328  4a68      ldr	r2, [r1, #4]
0802332a  dcf80070  ldr.w	r7, [r12]
0802332e  d202      lsls	r2, r2, #11
08023330  54bf      ite	pl
08023332  0eea0707  andpl.w	r7, lr, r7
08023336  48ea0707  orrmi.w	r7, r8, r7
0802333a  ccf80070  str.w	r7, [r12]
0802333e  4f68      ldr	r7, [r1, #4]
08023340  dcf80420  ldr.w	r2, [r12, #4]
08023344  be02      lsls	r6, r7, #10
08023346  54bf      ite	pl
08023348  0eea0202  andpl.w	r2, lr, r2
0802334c  48ea0202  orrmi.w	r2, r8, r2
08023350  ccf80420  str.w	r2, [r12, #4]
08023354  0a68      ldr	r2, [r1]
08023356  0133      adds	r3, #1
08023358  32fa03f7  lsrs.w	r7, r2, r3
0802335c  7ff428af  bne.w	#-432 ; -> 0x080231b0 ; branch_target=0x080231b0
08023360  03b0      add	sp, #12
08023362  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
08023366  4ff00309  mov.w	r9, #3
0802336a  09fa0efe  lsl.w	lr, r9, lr
0802336e  42ea0e02  orr.w	r2, r2, lr
08023372  c0e7      b	#-128 ; -> 0x080232f6 ; branch_target=0x080232f6
08023374  4ff00109  mov.w	r9, #1
08023378  09fa0efe  lsl.w	lr, r9, lr
0802337c  42ea0e02  orr.w	r2, r2, lr
08023380  b9e7      b	#-142 ; -> 0x080232f6 ; branch_target=0x080232f6
08023382  4ff00209  mov.w	r9, #2
08023386  09fa0efe  lsl.w	lr, r9, lr
0802338a  42ea0e02  orr.w	r2, r2, lr
0802338e  b2e7      b	#-156 ; -> 0x080232f6 ; branch_target=0x080232f6
08023390  4ff00609  mov.w	r9, #6
08023394  09fa0efe  lsl.w	lr, r9, lr
08023398  42ea0e02  orr.w	r2, r2, lr
0802339c  abe7      b	#-170 ; -> 0x080232f6 ; branch_target=0x080232f6
0802339e  4ff00409  mov.w	r9, #4
080233a2  09fa0efe  lsl.w	lr, r9, lr
080233a6  42ea0e02  orr.w	r2, r2, lr
080233aa  a4e7      b	#-184 ; -> 0x080232f6 ; branch_target=0x080232f6
080233ac  7047      bx	lr
080233ae  4ff00509  mov.w	r9, #5
080233b2  09fa0efe  lsl.w	lr, r9, lr
080233b6  42ea0e02  orr.w	r2, r2, lr
080233ba  9ce7      b	#-200 ; -> 0x080232f6 ; branch_target=0x080232f6
080233bc  4ff00709  mov.w	r9, #7
080233c0  09fa0efe  lsl.w	lr, r9, lr
080233c4  42ea0e02  orr.w	r2, r2, lr
080233c8  95e7      b	#-214 ; -> 0x080232f6 ; branch_target=0x080232f6
080233ca  4ff00809  mov.w	r9, #8
080233ce  09fa0efe  lsl.w	lr, r9, lr
080233d2  42ea0e02  orr.w	r2, r2, lr
080233d6  8ee7      b	#-228 ; -> 0x080232f6 ; branch_target=0x080232f6
