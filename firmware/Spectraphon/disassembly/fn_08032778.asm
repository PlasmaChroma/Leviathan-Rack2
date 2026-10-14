; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032778  08b5      push	{r3, lr}
0803277a  fff7edff  bl	#-38 ; -> 0x08032758 ; branch_target=0x08032758
0803277e  0128      cmp	r0, #1
08032780  01d0      beq	#2 ; -> 0x08032786 ; branch_target=0x08032786
08032782  0220      movs	r0, #2
08032784  08bd      pop	{r3, pc}
08032786  0748      ldr	r0, [pc, #28] ; [0x080327a4] = 0x20014a48
08032788  f4f7a4f9  bl	#-48312 ; -> 0x08026ad4 ; branch_target=0x08026ad4
0803278c  0028      cmp	r0, #0
0803278e  f9d1      bne	#-14 ; -> 0x08032784 ; branch_target=0x08032784
08032790  4ff48041  mov.w	r1, #16384
08032794  0348      ldr	r0, [pc, #12] ; [0x080327a4] = 0x20014a48
08032796  f4f7d7f8  bl	#-48722 ; -> 0x08026948 ; branch_target=0x08026948
0803279a  0038      subs	r0, #0
0803279c  18bf      it	ne
0803279e  0120      movne	r0, #1
080327a0  08bd      pop	{r3, pc}
