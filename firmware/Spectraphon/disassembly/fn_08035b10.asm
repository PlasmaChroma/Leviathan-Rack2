; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic,vector
08035b10  08b5      push	{r3, lr}
08035b12  0548      ldr	r0, [pc, #20] ; [0x08035b28] = 0x200146d8
08035b14  edf790f8  bl	#-77536 ; -> 0x08022c38 ; branch_target=0x08022c38
08035b18  0448      ldr	r0, [pc, #16] ; [0x08035b2c] = 0x200145e8
08035b1a  edf78df8  bl	#-77542 ; -> 0x08022c38 ; branch_target=0x08022c38
08035b1e  0448      ldr	r0, [pc, #16] ; [0x08035b30] = 0x20014570
08035b20  bde80840  pop.w	{r3, lr}
08035b24  edf788b8  b.w	#-77552 ; -> 0x08022c38 ; branch_target=0x08022c38
