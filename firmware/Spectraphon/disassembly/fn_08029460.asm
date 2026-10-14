; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029460  10b4      push	{r4}
08029462  0146      mov	r1, r0
08029464  0024      movs	r4, #0
08029466  194a      ldr	r2, [pc, #100] ; [0x080294cc] = 0x20000014
08029468  194b      ldr	r3, [pc, #100] ; [0x080294d0] = 0x10624dd3
0802946a  8c60      str	r4, [r1, #8]
0802946c  1268      ldr	r2, [r2]
0802946e  cc68      ldr	r4, [r1, #12]
08029470  1848      ldr	r0, [pc, #96] ; [0x080294d4] = 0xfffee0c0
08029472  a3fb0232  umull	r3, r2, r3, r2
08029476  41f20233  movw	r3, #4866
0802947a  2040      ands	r0, r4
0802947c  520a      lsrs	r2, r2, #9
0802947e  0343      orrs	r3, r0
08029480  41f28830  movw	r0, #5000
08029484  00fb02f2  mul	r2, r0, r2
08029488  cb60      str	r3, [r1, #12]
0802948a  4ab1      cbz	r2, #18 ; -> 0x080294a0 ; branch_target=0x080294a0
0802948c  531e      subs	r3, r2, #1
0802948e  4a6b      ldr	r2, [r1, #52]
08029490  013b      subs	r3, #1
08029492  12f0450f  tst.w	r2, #69
08029496  01d0      beq	#2 ; -> 0x0802949c ; branch_target=0x0802949c
08029498  9204      lsls	r2, r2, #18
0802949a  06d5      bpl	#12 ; -> 0x080294aa ; branch_target=0x080294aa
0802949c  581c      adds	r0, r3, #1
0802949e  f6d1      bne	#-20 ; -> 0x0802948e ; branch_target=0x0802948e
080294a0  4ff00040  mov.w	r0, #2147483648
080294a4  5df8044b  ldr	r4, [sp], #4
080294a8  7047      bx	lr
080294aa  4b6b      ldr	r3, [r1, #52]
080294ac  5b07      lsls	r3, r3, #29
080294ae  09d4      bmi	#18 ; -> 0x080294c4 ; branch_target=0x080294c4
080294b0  486b      ldr	r0, [r1, #52]
080294b2  10f00100  ands	r0, r0, #1
080294b6  14bf      ite	ne
080294b8  0123      movne	r3, #1
080294ba  074b      ldreq	r3, [pc, #28] ; [0x080294d8] = 0x002000c5
080294bc  8b63      str	r3, [r1, #56]
080294be  5df8044b  ldr	r4, [sp], #4
080294c2  7047      bx	lr
080294c4  0423      movs	r3, #4
080294c6  1846      mov	r0, r3
080294c8  8b63      str	r3, [r1, #56]
080294ca  ebe7      b	#-42 ; -> 0x080294a4 ; branch_target=0x080294a4
