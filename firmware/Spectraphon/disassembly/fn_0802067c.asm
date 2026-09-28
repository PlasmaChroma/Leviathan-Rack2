; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
0802067c  10b5      push	{r4, lr}
0802067e  836b      ldr	r3, [r0, #56]
08020680  5a6d      ldr	r2, [r3, #84]
08020682  12f0500f  tst.w	r2, #80
08020686  5a6d      ldr	r2, [r3, #84]
08020688  1dd1      bne	#58 ; -> 0x080206c6 ; branch_target=0x080206c6
0802068a  1968      ldr	r1, [r3]
0802068c  42f40072  orr	r2, r2, #512
08020690  5a65      str	r2, [r3, #84]
08020692  0a68      ldr	r2, [r1]
08020694  12f0080f  tst.w	r2, #8
08020698  ca68      ldr	r2, [r1, #12]
0802069a  1bd0      beq	#54 ; -> 0x080206d4 ; branch_target=0x080206d4
0802069c  12f4406f  tst.w	r2, #3072
080206a0  0dd1      bne	#26 ; -> 0x080206be ; branch_target=0x080206be
080206a2  ca68      ldr	r2, [r1, #12]
080206a4  9404      lsls	r4, r2, #18
080206a6  0ad4      bmi	#20 ; -> 0x080206be ; branch_target=0x080206be
080206a8  5a6d      ldr	r2, [r3, #84]
080206aa  22f48072  bic	r2, r2, #256
080206ae  5a65      str	r2, [r3, #84]
080206b0  5a6d      ldr	r2, [r3, #84]
080206b2  d104      lsls	r1, r2, #19
080206b4  03d4      bmi	#6 ; -> 0x080206be ; branch_target=0x080206be
080206b6  5a6d      ldr	r2, [r3, #84]
080206b8  42f00102  orr	r2, r2, #1
080206bc  5a65      str	r2, [r3, #84]
080206be  1846      mov	r0, r3
080206c0  11f0d0ff  bl	#73632 ; -> 0x08032664 ; branch_target=0x08032664
080206c4  10bd      pop	{r4, pc}
080206c6  d206      lsls	r2, r2, #27
080206c8  0ad4      bmi	#20 ; -> 0x080206e0 ; branch_target=0x080206e0
080206ca  db6c      ldr	r3, [r3, #76]
080206cc  bde81040  pop.w	{r4, lr}
080206d0  db6c      ldr	r3, [r3, #76]
080206d2  1847      bx	r3
080206d4  9007      lsls	r0, r2, #30
080206d6  e7d0      beq	#-50 ; -> 0x080206a8 ; branch_target=0x080206a8
080206d8  1846      mov	r0, r3
080206da  11f0c3ff  bl	#73606 ; -> 0x08032664 ; branch_target=0x08032664
080206de  f1e7      b	#-30 ; -> 0x080206c4 ; branch_target=0x080206c4
080206e0  1846      mov	r0, r3
080206e2  fff78bfe  bl	#-746 ; -> 0x080203fc ; branch_target=0x080203fc
080206e6  10bd      pop	{r4, pc}
