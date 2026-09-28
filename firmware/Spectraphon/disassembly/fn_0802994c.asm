; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0802994c  70b5      push	{r4, r5, r6, lr}
0802994e  1a68      ldr	r2, [r3]
08029950  0468      ldr	r4, [r0]
08029952  9442      cmp	r4, r2
08029954  20d0      beq	#64 ; -> 0x08029998 ; branch_target=0x08029998
08029956  1869      ldr	r0, [r3, #16]
08029958  a042      cmp	r0, r4
0802995a  04d0      beq	#8 ; -> 0x08029966 ; branch_target=0x08029966
0802995c  5ab3      cbz	r2, #86 ; -> 0x080299b6 ; branch_target=0x080299b6
0802995e  0028      cmp	r0, #0
08029960  3fd0      beq	#126 ; -> 0x080299e2 ; branch_target=0x080299e2
08029962  0020      movs	r0, #0
08029964  70bd      pop	{r4, r5, r6, pc}
08029966  5d69      ldr	r5, [r3, #20]
08029968  dcf80860  ldr.w	r6, [r12, #8]
0802996c  b542      cmp	r5, r6
0802996e  f5d1      bne	#-22 ; -> 0x0802995c ; branch_target=0x0802995c
08029970  9d69      ldr	r5, [r3, #24]
08029972  dcf81460  ldr.w	r6, [r12, #20]
08029976  b542      cmp	r5, r6
08029978  f0d1      bne	#-32 ; -> 0x0802995c ; branch_target=0x0802995c
0802997a  0122      movs	r2, #1
0802997c  03eb0210  add.w	r0, r3, r2, lsl #4
08029980  4fea021e  lsl.w	lr, r2, #4
08029984  8089      ldrh	r0, [r0, #12]
08029986  91b1      cbz	r1, #36 ; -> 0x080299ae ; branch_target=0x080299ae
08029988  0028      cmp	r0, #0
0802998a  ead1      bne	#-44 ; -> 0x08029962 ; branch_target=0x08029962
0802998c  501c      adds	r0, r2, #1
0802998e  4ff48072  mov.w	r2, #256
08029992  7344      add	r3, lr
08029994  9a81      strh	r2, [r3, #12]
08029996  70bd      pop	{r4, r5, r6, pc}
08029998  8068      ldr	r0, [r0, #8]
0802999a  5d68      ldr	r5, [r3, #4]
0802999c  8542      cmp	r5, r0
0802999e  dad1      bne	#-76 ; -> 0x08029956 ; branch_target=0x08029956
080299a0  dcf81400  ldr.w	r0, [r12, #20]
080299a4  9d68      ldr	r5, [r3, #8]
080299a6  8542      cmp	r5, r0
080299a8  d5d1      bne	#-86 ; -> 0x08029956 ; branch_target=0x08029956
080299aa  0022      movs	r2, #0
080299ac  e6e7      b	#-52 ; -> 0x0802997c ; branch_target=0x0802997c
080299ae  411c      adds	r1, r0, #1
080299b0  501c      adds	r0, r2, #1
080299b2  8ab2      uxth	r2, r1
080299b4  ede7      b	#-38 ; -> 0x08029992 ; branch_target=0x08029992
080299b6  0120      movs	r0, #1
080299b8  4fea021e  lsl.w	lr, r2, #4
080299bc  0029      cmp	r1, #0
080299be  03eb0212  add.w	r2, r3, r2, lsl #4
080299c2  43f80e40  str.w	r4, [r3, lr]
080299c6  dcf81410  ldr.w	r1, [r12, #20]
080299ca  dcf80840  ldr.w	r4, [r12, #8]
080299ce  c2e90141  strd	r4, r1, [r2, #4]
080299d2  4ff00001  mov.w	r1, #0
080299d6  9181      strh	r1, [r2, #12]
080299d8  14bf      ite	ne
080299da  4ff48072  movne.w	r2, #256
080299de  0122      moveq	r2, #1
080299e0  d7e7      b	#-82 ; -> 0x08029992 ; branch_target=0x08029992
080299e2  0220      movs	r0, #2
080299e4  0122      movs	r2, #1
080299e6  e7e7      b	#-50 ; -> 0x080299b8 ; branch_target=0x080299b8
