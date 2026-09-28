; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080211f0  f0b5      push	{r4, r5, r6, r7, lr}
080211f2  0023      movs	r3, #0
080211f4  83b0      sub	sp, #12
080211f6  0f46      mov	r7, r1
080211f8  0168      ldr	r1, [r0]
080211fa  0193      str	r3, [sp, #4]
080211fc  0446      mov	r4, r0
080211fe  8b68      ldr	r3, [r1, #8]
08021200  9800      lsls	r0, r3, #2
08021202  04d5      bpl	#8 ; -> 0x0802120e ; branch_target=0x0802120e
08021204  8b68      ldr	r3, [r1, #8]
08021206  23f00053  bic	r3, r3, #536870912
0802120a  8b60      str	r3, [r1, #8]
0802120c  2168      ldr	r1, [r4]
0802120e  8b68      ldr	r3, [r1, #8]
08021210  da00      lsls	r2, r3, #3
08021212  15d4      bmi	#42 ; -> 0x08021240 ; branch_target=0x08021240
08021214  4c4b      ldr	r3, [pc, #304] ; [0x08021348] = 0x20000014
08021216  4d48      ldr	r0, [pc, #308] ; [0x0802134c] = 0x053e2d63
08021218  1b68      ldr	r3, [r3]
0802121a  8a68      ldr	r2, [r1, #8]
0802121c  9b09      lsrs	r3, r3, #6
0802121e  42f08052  orr	r2, r2, #268435456
08021222  a0fb0303  umull	r0, r3, r0, r3
08021226  8a60      str	r2, [r1, #8]
08021228  9b09      lsrs	r3, r3, #6
0802122a  0133      adds	r3, #1
0802122c  0193      str	r3, [sp, #4]
0802122e  019b      ldr	r3, [sp, #4]
08021230  2bb1      cbz	r3, #10 ; -> 0x0802123e ; branch_target=0x0802123e
08021232  019b      ldr	r3, [sp, #4]
08021234  013b      subs	r3, #1
08021236  0193      str	r3, [sp, #4]
08021238  019b      ldr	r3, [sp, #4]
0802123a  002b      cmp	r3, #0
0802123c  f9d1      bne	#-14 ; -> 0x08021232 ; branch_target=0x08021232
0802123e  2168      ldr	r1, [r4]
08021240  8b68      ldr	r3, [r1, #8]
08021242  db00      lsls	r3, r3, #3
08021244  61d5      bpl	#194 ; -> 0x0802130a ; branch_target=0x0802130a
08021246  8d68      ldr	r5, [r1, #8]
08021248  15f00105  ands	r5, r5, #1
0802124c  2fd1      bne	#94 ; -> 0x080212ae ; branch_target=0x080212ae
0802124e  2046      mov	r0, r4
08021250  fff78cfc  bl	#-1768 ; -> 0x08020b6c ; branch_target=0x08020b6c
08021254  0028      cmp	r0, #0
08021256  60d1      bne	#192 ; -> 0x0802131a ; branch_target=0x0802131a
08021258  07f11800  add.w	r0, r7, #24
0802125c  4ff00066  mov.w	r6, #134217728
08021260  2168      ldr	r1, [r4]
08021262  26fa05fc  lsr.w	r12, r6, r5
08021266  50f8042d  ldr	r2, [r0, #-4]!
0802126a  d1f8c830  ldr.w	r3, [r1, #200]
0802126e  03f04043  and	r3, r3, #3221225472
08021272  1343      orrs	r3, r2
08021274  c1f8c830  str.w	r3, [r1, #200]
08021278  8b68      ldr	r3, [r1, #8]
0802127a  23f48033  bic	r3, r3, #65536
0802127e  43ea0c03  orr.w	r3, r3, r12
08021282  8b60      str	r3, [r1, #8]
08021284  8b68      ldr	r3, [r1, #8]
08021286  1cea030f  tst.w	r12, r3
0802128a  07d1      bne	#14 ; -> 0x0802129c ; branch_target=0x0802129c
0802128c  304b      ldr	r3, [pc, #192] ; [0x08021350] = 0x00080070
0802128e  00e0      b	#0 ; -> 0x08021292 ; branch_target=0x08021292
08021290  23b1      cbz	r3, #8 ; -> 0x0802129c ; branch_target=0x0802129c
08021292  8a68      ldr	r2, [r1, #8]
08021294  013b      subs	r3, #1
08021296  1cea020f  tst.w	r12, r2
0802129a  f9d0      beq	#-14 ; -> 0x08021290 ; branch_target=0x08021290
0802129c  0135      adds	r5, #1
0802129e  062d      cmp	r5, #6
080212a0  ded1      bne	#-68 ; -> 0x08021260 ; branch_target=0x08021260
080212a2  2046      mov	r0, r4
080212a4  fff74afd  bl	#-1388 ; -> 0x08020d3c ; branch_target=0x08020d3c
080212a8  0020      movs	r0, #0
080212aa  03b0      add	sp, #12
080212ac  f0bd      pop	{r4, r5, r6, r7, pc}
080212ae  8e68      ldr	r6, [r1, #8]
080212b0  16f00406  ands	r6, r6, #4
080212b4  3ed1      bne	#124 ; -> 0x08021334 ; branch_target=0x08021334
080212b6  07f11800  add.w	r0, r7, #24
080212ba  4ff0000e  mov.w	lr, #0
080212be  4ff00065  mov.w	r5, #134217728
080212c2  d1f8c830  ldr.w	r3, [r1, #200]
080212c6  25fa0efc  lsr.w	r12, r5, lr
080212ca  50f8042d  ldr	r2, [r0, #-4]!
080212ce  03f04043  and	r3, r3, #3221225472
080212d2  1343      orrs	r3, r2
080212d4  c1f8c830  str.w	r3, [r1, #200]
080212d8  8b68      ldr	r3, [r1, #8]
080212da  23f48033  bic	r3, r3, #65536
080212de  43ea0c03  orr.w	r3, r3, r12
080212e2  8b60      str	r3, [r1, #8]
080212e4  8b68      ldr	r3, [r1, #8]
080212e6  1cea030f  tst.w	r12, r3
080212ea  07d1      bne	#14 ; -> 0x080212fc ; branch_target=0x080212fc
080212ec  184b      ldr	r3, [pc, #96] ; [0x08021350] = 0x00080070
080212ee  00e0      b	#0 ; -> 0x080212f2 ; branch_target=0x080212f2
080212f0  23b1      cbz	r3, #8 ; -> 0x080212fc ; branch_target=0x080212fc
080212f2  8a68      ldr	r2, [r1, #8]
080212f4  013b      subs	r3, #1
080212f6  1cea020f  tst.w	r12, r2
080212fa  f9d0      beq	#-14 ; -> 0x080212f0 ; branch_target=0x080212f0
080212fc  bef1050f  cmp.w	lr, #5
08021300  0ed0      beq	#28 ; -> 0x08021320 ; branch_target=0x08021320
08021302  2168      ldr	r1, [r4]
08021304  0ef1010e  add.w	lr, lr, #1
08021308  dbe7      b	#-74 ; -> 0x080212c2 ; branch_target=0x080212c2
0802130a  636d      ldr	r3, [r4, #84]
0802130c  43f01003  orr	r3, r3, #16
08021310  6365      str	r3, [r4, #84]
08021312  a36d      ldr	r3, [r4, #88]
08021314  43f00103  orr	r3, r3, #1
08021318  a365      str	r3, [r4, #88]
0802131a  0120      movs	r0, #1
0802131c  03b0      add	sp, #12
0802131e  f0bd      pop	{r4, r5, r6, r7, pc}
08021320  002e      cmp	r6, #0
08021322  c1d0      beq	#-126 ; -> 0x080212a8 ; branch_target=0x080212a8
08021324  2268      ldr	r2, [r4]
08021326  0b4b      ldr	r3, [pc, #44] ; [0x08021354] = 0x7fffffc0
08021328  9168      ldr	r1, [r2, #8]
0802132a  0b40      ands	r3, r1
0802132c  43f00403  orr	r3, r3, #4
08021330  9360      str	r3, [r2, #8]
08021332  b9e7      b	#-142 ; -> 0x080212a8 ; branch_target=0x080212a8
08021334  8a68      ldr	r2, [r1, #8]
08021336  2e46      mov	r6, r5
08021338  064b      ldr	r3, [pc, #24] ; [0x08021354] = 0x7fffffc0
0802133a  1340      ands	r3, r2
0802133c  43f01003  orr	r3, r3, #16
08021340  8b60      str	r3, [r1, #8]
08021342  2168      ldr	r1, [r4]
08021344  b7e7      b	#-146 ; -> 0x080212b6 ; branch_target=0x080212b6
