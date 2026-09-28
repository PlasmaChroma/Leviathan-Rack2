; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026790  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08026794  0446      mov	r4, r0
08026796  96b0      sub	sp, #88
08026798  0d46      mov	r5, r1
0802679a  f9f7fffd  bl	#-25602 ; -> 0x0802039c ; branch_target=0x0802039c
0802679e  0021      movs	r1, #0
080267a0  0746      mov	r7, r0
080267a2  2068      ldr	r0, [r4]
080267a4  01f090fe  bl	#7456 ; -> 0x080284c8 ; branch_target=0x080284c8
080267a8  8201      lsls	r2, r0, #6
080267aa  5cd4      bmi	#184 ; -> 0x08026866 ; branch_target=0x08026866
080267ac  4021      movs	r1, #64
080267ae  2068      ldr	r0, [r4]
080267b0  01f0a4fe  bl	#7496 ; -> 0x080284fc ; branch_target=0x080284fc
080267b4  0346      mov	r3, r0
080267b6  e8b1      cbz	r0, #58 ; -> 0x080267f4 ; branch_target=0x080267f4
080267b8  626b      ldr	r2, [r4, #52]
080267ba  2068      ldr	r0, [r4]
080267bc  6263      str	r2, [r4, #52]
080267be  564a      ldr	r2, [pc, #344] ; [0x08026918] = 0x1fe00fff
080267c0  0121      movs	r1, #1
080267c2  8263      str	r2, [r0, #56]
080267c4  0e46      mov	r6, r1
080267c6  626b      ldr	r2, [r4, #52]
080267c8  1343      orrs	r3, r2
080267ca  6363      str	r3, [r4, #52]
080267cc  84f83010  strb.w	r1, [r4, #48]
080267d0  4ff40071  mov.w	r1, #512
080267d4  2068      ldr	r0, [r4]
080267d6  01f091fe  bl	#7458 ; -> 0x080284fc ; branch_target=0x080284fc
080267da  38b1      cbz	r0, #14 ; -> 0x080267ec ; branch_target=0x080267ec
080267dc  0123      movs	r3, #1
080267de  2268      ldr	r2, [r4]
080267e0  4d49      ldr	r1, [pc, #308] ; [0x08026918] = 0x1fe00fff
080267e2  1e46      mov	r6, r3
080267e4  9163      str	r1, [r2, #56]
080267e6  6063      str	r0, [r4, #52]
080267e8  84f83030  strb.w	r3, [r4, #48]
080267ec  3046      mov	r0, r6
080267ee  16b0      add	sp, #88
080267f0  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
080267f4  616c      ldr	r1, [r4, #68]
080267f6  2068      ldr	r0, [r4]
080267f8  0904      lsls	r1, r1, #16
080267fa  02f0f7fb  bl	#10222 ; -> 0x08028fec ; branch_target=0x08028fec
080267fe  0346      mov	r3, r0
08026800  0028      cmp	r0, #0
08026802  d9d1      bne	#-78 ; -> 0x080267b8 ; branch_target=0x080267b8
08026804  4ff0ff32  mov.w	r2, #4294967295
08026808  4023      movs	r3, #64
0802680a  6946      mov	r1, sp
0802680c  2068      ldr	r0, [r4]
0802680e  cde90023  strd	r2, r3, [sp]
08026812  6022      movs	r2, #96
08026814  0223      movs	r3, #2
08026816  cde90223  strd	r2, r3, [sp, #8]
0802681a  0123      movs	r3, #1
0802681c  0022      movs	r2, #0
0802681e  cde90423  strd	r2, r3, [sp, #16]
08026822  01f055fe  bl	#7338 ; -> 0x080284d0 ; branch_target=0x080284d0
08026826  2068      ldr	r0, [r4]
08026828  02f0a4ff  bl	#12104 ; -> 0x08029774 ; branch_target=0x08029774
0802682c  0346      mov	r3, r0
0802682e  0028      cmp	r0, #0
08026830  c2d1      bne	#-124 ; -> 0x080267b8 ; branch_target=0x080267b8
08026832  06ae      add	r6, sp, #24
08026834  04e0      b	#8 ; -> 0x08026840 ; branch_target=0x08026840
08026836  f9f7b1fd  bl	#-25758 ; -> 0x0802039c ; branch_target=0x0802039c
0802683a  c01b      subs	r0, r0, r7
0802683c  0130      adds	r0, #1
0802683e  16d0      beq	#44 ; -> 0x0802686e ; branch_target=0x0802686e
08026840  2068      ldr	r0, [r4]
08026842  436b      ldr	r3, [r0, #52]
08026844  13f4957f  tst.w	r3, #298
08026848  436b      ldr	r3, [r0, #52]
0802684a  14d1      bne	#40 ; -> 0x08026876 ; branch_target=0x08026876
0802684c  1b04      lsls	r3, r3, #16
0802684e  f2d5      bpl	#-28 ; -> 0x08026836 ; branch_target=0x08026836
08026850  06f12008  add.w	r8, r6, #32
08026854  00e0      b	#0 ; -> 0x08026858 ; branch_target=0x08026858
08026856  2068      ldr	r0, [r4]
08026858  01f020fe  bl	#7232 ; -> 0x0802849c ; branch_target=0x0802849c
0802685c  46f8040b  str	r0, [r6], #4
08026860  4645      cmp	r6, r8
08026862  f8d1      bne	#-16 ; -> 0x08026856 ; branch_target=0x08026856
08026864  e7e7      b	#-50 ; -> 0x08026836 ; branch_target=0x08026836
08026866  2068      ldr	r0, [r4]
08026868  4ff40063  mov.w	r3, #2048
0802686c  a7e7      b	#-178 ; -> 0x080267be ; branch_target=0x080267be
0802686e  2068      ldr	r0, [r4]
08026870  4ff00043  mov.w	r3, #2147483648
08026874  a3e7      b	#-186 ; -> 0x080267be ; branch_target=0x080267be
08026876  1907      lsls	r1, r3, #28
08026878  48d4      bmi	#144 ; -> 0x0802690c ; branch_target=0x0802690c
0802687a  436b      ldr	r3, [r0, #52]
0802687c  9a07      lsls	r2, r3, #30
0802687e  47d4      bmi	#142 ; -> 0x08026910 ; branch_target=0x08026910
08026880  436b      ldr	r3, [r0, #52]
08026882  9b06      lsls	r3, r3, #26
08026884  0ad5      bpl	#20 ; -> 0x0802689c ; branch_target=0x0802689c
08026886  45e0      b	#138 ; -> 0x08026914 ; branch_target=0x08026914
08026888  01f008fe  bl	#7184 ; -> 0x0802849c ; branch_target=0x0802849c
0802688c  46f8040b  str	r0, [r6], #4
08026890  f9f784fd  bl	#-25848 ; -> 0x0802039c ; branch_target=0x0802039c
08026894  c01b      subs	r0, r0, r7
08026896  0130      adds	r0, #1
08026898  e9d0      beq	#-46 ; -> 0x0802686e ; branch_target=0x0802686e
0802689a  2068      ldr	r0, [r4]
0802689c  436b      ldr	r3, [r0, #52]
0802689e  13f48053  ands	r3, r3, #4096
080268a2  f1d1      bne	#-30 ; -> 0x08026888 ; branch_target=0x08026888
080268a4  1d4a      ldr	r2, [pc, #116] ; [0x0802691c] = 0x18000f3a
080268a6  1e46      mov	r6, r3
080268a8  069b      ldr	r3, [sp, #24]
080268aa  8263      str	r2, [r0, #56]
080268ac  c3f38112  ubfx	r2, r3, #6, #2
080268b0  2a70      strb	r2, [r5]
080268b2  c3f34012  ubfx	r2, r3, #5, #1
080268b6  6a70      strb	r2, [r5, #1]
080268b8  1a0a      lsrs	r2, r3, #8
080268ba  22f0ff02  bic	r2, r2, #255
080268be  42ea1362  orr.w	r2, r2, r3, lsr #24
080268c2  dde90713  ldrd	r1, r3, [sp, #28]
080268c6  92b2      uxth	r2, r2
080268c8  09ba      rev	r1, r1
080268ca  6a80      strh	r2, [r5, #2]
080268cc  dab2      uxtb	r2, r3
080268ce  6960      str	r1, [r5, #4]
080268d0  2a72      strb	r2, [r5, #8]
080268d2  c3f30722  ubfx	r2, r3, #8, #8
080268d6  9df82b10  ldrb.w	r1, [sp, #43]
080268da  6a72      strb	r2, [r5, #9]
080268dc  c3f30352  ubfx	r2, r3, #20, #4
080268e0  1b0c      lsrs	r3, r3, #16
080268e2  aa72      strb	r2, [r5, #10]
080268e4  099a      ldr	r2, [sp, #36]
080268e6  23f0ff03  bic	r3, r3, #255
080268ea  d0b2      uxtb	r0, r2
080268ec  0343      orrs	r3, r0
080268ee  ab81      strh	r3, [r5, #12]
080268f0  c2f38523  ubfx	r3, r2, #10, #6
080268f4  ab73      strb	r3, [r5, #14]
080268f6  c2f30123  ubfx	r3, r2, #8, #2
080268fa  eb73      strb	r3, [r5, #15]
080268fc  c2f30313  ubfx	r3, r2, #4, #4
08026900  02f00f02  and	r2, r2, #15
08026904  2b74      strb	r3, [r5, #16]
08026906  6a74      strb	r2, [r5, #17]
08026908  a974      strb	r1, [r5, #18]
0802690a  61e7      b	#-318 ; -> 0x080267d0 ; branch_target=0x080267d0
0802690c  0823      movs	r3, #8
0802690e  56e7      b	#-340 ; -> 0x080267be ; branch_target=0x080267be
08026910  0223      movs	r3, #2
08026912  54e7      b	#-344 ; -> 0x080267be ; branch_target=0x080267be
08026914  2023      movs	r3, #32
08026916  52e7      b	#-348 ; -> 0x080267be ; branch_target=0x080267be
