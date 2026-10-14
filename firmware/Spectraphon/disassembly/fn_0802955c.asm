; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802955c  0023      movs	r3, #0
0802955e  2b4a      ldr	r2, [pc, #172] ; [0x0802960c] = 0x20000014
08029560  8446      mov	r12, r0
08029562  10b4      push	{r4}
08029564  8360      str	r3, [r0, #8]
08029566  1268      ldr	r2, [r2]
08029568  c368      ldr	r3, [r0, #12]
0802956a  2948      ldr	r0, [pc, #164] ; [0x08029610] = 0x10624dd3
0802956c  294c      ldr	r4, [pc, #164] ; [0x08029614] = 0xfffee0c0
0802956e  a0fb0202  umull	r0, r2, r0, r2
08029572  41f28830  movw	r0, #5000
08029576  1c40      ands	r4, r3
08029578  41f20313  movw	r3, #4355
0802957c  520a      lsrs	r2, r2, #9
0802957e  2343      orrs	r3, r4
08029580  00fb02f2  mul	r2, r0, r2
08029584  ccf80c30  str.w	r3, [r12, #12]
08029588  52b1      cbz	r2, #20 ; -> 0x080295a0 ; branch_target=0x080295a0
0802958a  531e      subs	r3, r2, #1
0802958c  dcf83420  ldr.w	r2, [r12, #52]
08029590  013b      subs	r3, #1
08029592  12f0450f  tst.w	r2, #69
08029596  01d0      beq	#2 ; -> 0x0802959c ; branch_target=0x0802959c
08029598  9204      lsls	r2, r2, #18
0802959a  06d5      bpl	#12 ; -> 0x080295aa ; branch_target=0x080295aa
0802959c  581c      adds	r0, r3, #1
0802959e  f5d1      bne	#-22 ; -> 0x0802958c ; branch_target=0x0802958c
080295a0  4ff00040  mov.w	r0, #2147483648
080295a4  5df8044b  ldr	r4, [sp], #4
080295a8  7047      bx	lr
080295aa  dcf83430  ldr.w	r3, [r12, #52]
080295ae  5c07      lsls	r4, r3, #29
080295b0  20d4      bmi	#64 ; -> 0x080295f4 ; branch_target=0x080295f4
080295b2  dcf83430  ldr.w	r3, [r12, #52]
080295b6  d807      lsls	r0, r3, #31
080295b8  06d4      bmi	#12 ; -> 0x080295c8 ; branch_target=0x080295c8
080295ba  dcf81030  ldr.w	r3, [r12, #16]
080295be  dbb2      uxtb	r3, r3
080295c0  032b      cmp	r3, #3
080295c2  05d0      beq	#10 ; -> 0x080295d0 ; branch_target=0x080295d0
080295c4  0120      movs	r0, #1
080295c6  ede7      b	#-38 ; -> 0x080295a4 ; branch_target=0x080295a4
080295c8  0123      movs	r3, #1
080295ca  ccf83830  str.w	r3, [r12, #56]
080295ce  f9e7      b	#-14 ; -> 0x080295c4 ; branch_target=0x080295c4
080295d0  114b      ldr	r3, [pc, #68] ; [0x08029618] = 0x002000c5
080295d2  ccf83830  str.w	r3, [r12, #56]
080295d6  dcf81430  ldr.w	r3, [r12, #20]
080295da  13f46040  ands	r0, r3, #57344
080295de  11d0      beq	#34 ; -> 0x08029604 ; branch_target=0x08029604
080295e0  5a04      lsls	r2, r3, #17
080295e2  0cd4      bmi	#24 ; -> 0x080295fe ; branch_target=0x080295fe
080295e4  13f4004f  tst.w	r3, #32768
080295e8  0cbf      ite	eq
080295ea  4ff48030  moveq.w	r0, #65536
080295ee  4ff48050  movne.w	r0, #4096
080295f2  d7e7      b	#-82 ; -> 0x080295a4 ; branch_target=0x080295a4
080295f4  0423      movs	r3, #4
080295f6  1846      mov	r0, r3
080295f8  ccf83830  str.w	r3, [r12, #56]
080295fc  d2e7      b	#-92 ; -> 0x080295a4 ; branch_target=0x080295a4
080295fe  4ff40050  mov.w	r0, #8192
08029602  cfe7      b	#-98 ; -> 0x080295a4 ; branch_target=0x080295a4
08029604  1b0c      lsrs	r3, r3, #16
08029606  0b80      strh	r3, [r1]
08029608  cce7      b	#-104 ; -> 0x080295a4 ; branch_target=0x080295a4
