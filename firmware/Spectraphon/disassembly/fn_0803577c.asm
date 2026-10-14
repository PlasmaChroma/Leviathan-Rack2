; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
0803577c  08b5      push	{r3, lr}
0803577e  0846      mov	r0, r1
08035780  1146      mov	r1, r2
08035782  1a46      mov	r2, r3
08035784  4ff0ff33  mov.w	r3, #4294967295
08035788  fcf7c4ff  bl	#-12408 ; -> 0x08032714 ; branch_target=0x08032714
0803578c  08b1      cbz	r0, #2 ; -> 0x08035792 ; branch_target=0x08035792
0803578e  0120      movs	r0, #1
08035790  08bd      pop	{r3, pc}
08035792  fcf7d1ff  bl	#-12382 ; -> 0x08032738 ; branch_target=0x08032738
08035796  0028      cmp	r0, #0
08035798  fad0      beq	#-12 ; -> 0x08035790 ; branch_target=0x08035790
0803579a  fcf7cdff  bl	#-12390 ; -> 0x08032738 ; branch_target=0x08032738
0803579e  0028      cmp	r0, #0
080357a0  f7d1      bne	#-18 ; -> 0x08035792 ; branch_target=0x08035792
080357a2  f5e7      b	#-22 ; -> 0x08035790 ; branch_target=0x08035790
