; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
08035754  08b5      push	{r3, lr}
08035756  0846      mov	r0, r1
08035758  1146      mov	r1, r2
0803575a  1a46      mov	r2, r3
0803575c  4ff0ff33  mov.w	r3, #4294967295
08035760  fcf7c6ff  bl	#-12404 ; -> 0x080326f0 ; branch_target=0x080326f0
08035764  08b1      cbz	r0, #2 ; -> 0x0803576a ; branch_target=0x0803576a
08035766  0120      movs	r0, #1
08035768  08bd      pop	{r3, pc}
0803576a  fcf7e5ff  bl	#-12342 ; -> 0x08032738 ; branch_target=0x08032738
0803576e  0028      cmp	r0, #0
08035770  fad0      beq	#-12 ; -> 0x08035768 ; branch_target=0x08035768
08035772  fcf7e1ff  bl	#-12350 ; -> 0x08032738 ; branch_target=0x08032738
08035776  0028      cmp	r0, #0
08035778  f7d1      bne	#-18 ; -> 0x0803576a ; branch_target=0x0803576a
0803577a  f5e7      b	#-22 ; -> 0x08035768 ; branch_target=0x08035768
