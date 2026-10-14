; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08036544  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08036548  dff874a0  ldr.w	r10, [pc, #116] ; [0x080365c0] = 0x20000018
0803654c  0646      mov	r6, r0
0803654e  daf80000  ldr.w	r0, [r10]
08036552  9846      mov	r8, r3
08036554  0f46      mov	r7, r1
08036556  9146      mov	r9, r2
08036558  fff7c0ff  bl	#-128 ; -> 0x080364dc ; branch_target=0x080364dc
0803655c  164b      ldr	r3, [pc, #88] ; [0x080365b8] = 0x20014e10
0803655e  1c68      ldr	r4, [r3]
08036560  0cb9      cbnz	r4, #2 ; -> 0x08036566 ; branch_target=0x08036566
08036562  164c      ldr	r4, [pc, #88] ; [0x080365bc] = 0x20014e14
08036564  1c60      str	r4, [r3]
08036566  6568      ldr	r5, [r4, #4]
08036568  daf80000  ldr.w	r0, [r10]
0803656c  1f2d      cmp	r5, #31
0803656e  05dd      ble	#10 ; -> 0x0803657c ; branch_target=0x0803657c
08036570  fff7b5ff  bl	#-150 ; -> 0x080364de ; branch_target=0x080364de
08036574  4ff0ff30  mov.w	r0, #4294967295
08036578  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0803657c  9eb1      cbz	r6, #38 ; -> 0x080365a6 ; branch_target=0x080365a6
0803657e  04eb8501  add.w	r1, r4, r5, lsl #2
08036582  0122      movs	r2, #1
08036584  c1f88890  str.w	r9, [r1, #136]
08036588  d4f88831  ldr.w	r3, [r4, #392]
0803658c  aa40      lsls	r2, r5
0803658e  1343      orrs	r3, r2
08036590  c4f88831  str.w	r3, [r4, #392]
08036594  022e      cmp	r6, #2
08036596  c1f80881  str.w	r8, [r1, #264]
0803659a  02bf      ittt	eq
0803659c  d4f88c31  ldreq.w	r3, [r4, #396]
080365a0  1343      orreq	r3, r2
080365a2  c4f88c31  streq.w	r3, [r4, #396]
080365a6  6b1c      adds	r3, r5, #1
080365a8  0235      adds	r5, #2
080365aa  6360      str	r3, [r4, #4]
080365ac  44f82570  str.w	r7, [r4, r5, lsl #2]
080365b0  fff795ff  bl	#-214 ; -> 0x080364de ; branch_target=0x080364de
080365b4  0020      movs	r0, #0
080365b6  dfe7      b	#-66 ; -> 0x08036578 ; branch_target=0x08036578
