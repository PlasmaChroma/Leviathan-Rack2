; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080294dc  1b4a      ldr	r2, [pc, #108] ; [0x0802954c] = 0x20000014
080294de  8446      mov	r12, r0
080294e0  1b4b      ldr	r3, [pc, #108] ; [0x08029550] = 0x10624dd3
080294e2  1268      ldr	r2, [r2]
080294e4  ccf80810  str.w	r1, [r12, #8]
080294e8  a3fb0232  umull	r3, r2, r3, r2
080294ec  dcf80c10  ldr.w	r1, [r12, #12]
080294f0  1848      ldr	r0, [pc, #96] ; [0x08029554] = 0xfffee0c0
080294f2  41f20933  movw	r3, #4873
080294f6  520a      lsrs	r2, r2, #9
080294f8  0840      ands	r0, r1
080294fa  41f28831  movw	r1, #5000
080294fe  0343      orrs	r3, r0
08029500  01fb02f2  mul	r2, r1, r2
08029504  ccf80c30  str.w	r3, [r12, #12]
08029508  531e      subs	r3, r2, #1
0802950a  4ab1      cbz	r2, #18 ; -> 0x08029520 ; branch_target=0x08029520
0802950c  dcf83420  ldr.w	r2, [r12, #52]
08029510  013b      subs	r3, #1
08029512  12f0450f  tst.w	r2, #69
08029516  01d0      beq	#2 ; -> 0x0802951c ; branch_target=0x0802951c
08029518  9204      lsls	r2, r2, #18
0802951a  04d5      bpl	#8 ; -> 0x08029526 ; branch_target=0x08029526
0802951c  591c      adds	r1, r3, #1
0802951e  f5d1      bne	#-22 ; -> 0x0802950c ; branch_target=0x0802950c
08029520  4ff00040  mov.w	r0, #2147483648
08029524  7047      bx	lr
08029526  dcf83430  ldr.w	r3, [r12, #52]
0802952a  5b07      lsls	r3, r3, #29
0802952c  09d4      bmi	#18 ; -> 0x08029542 ; branch_target=0x08029542
0802952e  dcf83400  ldr.w	r0, [r12, #52]
08029532  10f00100  ands	r0, r0, #1
08029536  14bf      ite	ne
08029538  0123      movne	r3, #1
0802953a  074b      ldreq	r3, [pc, #28] ; [0x08029558] = 0x002000c5
0802953c  ccf83830  str.w	r3, [r12, #56]
08029540  7047      bx	lr
08029542  0423      movs	r3, #4
08029544  1846      mov	r0, r3
08029546  ccf83830  str.w	r3, [r12, #56]
0802954a  7047      bx	lr
