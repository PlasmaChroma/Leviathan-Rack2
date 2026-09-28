; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029144  194a      ldr	r2, [pc, #100] ; [0x080291ac] = 0x20000014
08029146  8446      mov	r12, r0
08029148  194b      ldr	r3, [pc, #100] ; [0x080291b0] = 0x10624dd3
0802914a  1268      ldr	r2, [r2]
0802914c  ccf80810  str.w	r1, [r12, #8]
08029150  a3fb0232  umull	r3, r2, r3, r2
08029154  dcf80c10  ldr.w	r1, [r12, #12]
08029158  1648      ldr	r0, [pc, #88] ; [0x080291b4] = 0xfffee0c0
0802915a  41f22913  movw	r3, #4393
0802915e  520a      lsrs	r2, r2, #9
08029160  0840      ands	r0, r1
08029162  41f28831  movw	r1, #5000
08029166  0343      orrs	r3, r0
08029168  01fb02f2  mul	r2, r1, r2
0802916c  ccf80c30  str.w	r3, [r12, #12]
08029170  531e      subs	r3, r2, #1
08029172  4ab1      cbz	r2, #18 ; -> 0x08029188 ; branch_target=0x08029188
08029174  dcf83420  ldr.w	r2, [r12, #52]
08029178  013b      subs	r3, #1
0802917a  12f0450f  tst.w	r2, #69
0802917e  01d0      beq	#2 ; -> 0x08029184 ; branch_target=0x08029184
08029180  9204      lsls	r2, r2, #18
08029182  04d5      bpl	#8 ; -> 0x0802918e ; branch_target=0x0802918e
08029184  591c      adds	r1, r3, #1
08029186  f5d1      bne	#-22 ; -> 0x08029174 ; branch_target=0x08029174
08029188  4ff00040  mov.w	r0, #2147483648
0802918c  7047      bx	lr
0802918e  dcf83400  ldr.w	r0, [r12, #52]
08029192  10f00400  ands	r0, r0, #4
08029196  03d1      bne	#6 ; -> 0x080291a0 ; branch_target=0x080291a0
08029198  074b      ldr	r3, [pc, #28] ; [0x080291b8] = 0x002000c5
0802919a  ccf83830  str.w	r3, [r12, #56]
0802919e  7047      bx	lr
080291a0  0423      movs	r3, #4
080291a2  1846      mov	r0, r3
080291a4  ccf83830  str.w	r3, [r12, #56]
080291a8  7047      bx	lr
