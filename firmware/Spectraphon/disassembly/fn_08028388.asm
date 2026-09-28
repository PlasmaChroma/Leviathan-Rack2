; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028388  30b4      push	{r4, r5}
0802838a  4c69      ldr	r4, [r1, #20]
0802838c  cb68      ldr	r3, [r1, #12]
0802838e  04f1ff3c  add.w	r12, r4, #4294967295
08028392  8468      ldr	r4, [r0, #8]
08028394  013b      subs	r3, #1
08028396  4fea0c5c  lsl.w	r12, r12, #20
0802839a  1b03      lsls	r3, r3, #12
0802839c  43ea0c03  orr.w	r3, r3, r12
080283a0  cab9      cbnz	r2, #50 ; -> 0x080283d6 ; branch_target=0x080283d6
080283a2  0a68      ldr	r2, [r1]
080283a4  04f07044  and	r4, r4, #4026531840
080283a8  8d68      ldr	r5, [r1, #8]
080283aa  013a      subs	r2, #1
080283ac  013d      subs	r5, #1
080283ae  1343      orrs	r3, r2
080283b0  4a68      ldr	r2, [r1, #4]
080283b2  2343      orrs	r3, r4
080283b4  013a      subs	r2, #1
080283b6  0c69      ldr	r4, [r1, #16]
080283b8  43ea0213  orr.w	r3, r3, r2, lsl #4
080283bc  8a69      ldr	r2, [r1, #24]
080283be  611e      subs	r1, r4, #1
080283c0  43ea0523  orr.w	r3, r3, r5, lsl #8
080283c4  013a      subs	r2, #1
080283c6  43ea0143  orr.w	r3, r3, r1, lsl #16
080283ca  43ea0263  orr.w	r3, r3, r2, lsl #24
080283ce  8360      str	r3, [r0, #8]
080283d0  0020      movs	r0, #0
080283d2  30bc      pop	{r4, r5}
080283d4  7047      bx	lr
080283d6  0f4a      ldr	r2, [pc, #60] ; [0x08028414] = 0xff0f0fff
080283d8  2240      ands	r2, r4
080283da  1a43      orrs	r2, r3
080283dc  8260      str	r2, [r0, #8]
080283de  c368      ldr	r3, [r0, #12]
080283e0  8d68      ldr	r5, [r1, #8]
080283e2  03f07043  and	r3, r3, #4026531840
080283e6  05f1ff3c  add.w	r12, r5, #4294967295
080283ea  d1e90024  ldrd	r2, r4, [r1]
080283ee  013c      subs	r4, #1
080283f0  013a      subs	r2, #1
080283f2  43ea0413  orr.w	r3, r3, r4, lsl #4
080283f6  0c69      ldr	r4, [r1, #16]
080283f8  1343      orrs	r3, r2
080283fa  8a69      ldr	r2, [r1, #24]
080283fc  611e      subs	r1, r4, #1
080283fe  43ea0c23  orr.w	r3, r3, r12, lsl #8
08028402  013a      subs	r2, #1
08028404  43ea0143  orr.w	r3, r3, r1, lsl #16
08028408  43ea0263  orr.w	r3, r3, r2, lsl #24
0802840c  c360      str	r3, [r0, #12]
0802840e  0020      movs	r0, #0
08028410  30bc      pop	{r4, r5}
08028412  7047      bx	lr
