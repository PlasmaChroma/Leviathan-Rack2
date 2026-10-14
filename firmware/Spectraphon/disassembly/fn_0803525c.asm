; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0803525c  10b5      push	{r4, lr}
0803525e  0a4c      ldr	r4, [pc, #40] ; [0x08035288] = 0x58005404
08035260  c261      str	r2, [r0, #28]
08035262  0322      movs	r2, #3
08035264  4161      str	r1, [r0, #20]
08035266  8360      str	r3, [r0, #8]
08035268  4163      str	r1, [r0, #52]
0803526a  c0e90043  strd	r4, r3, [r0]
0803526e  c0e90b11  strd	r1, r1, [r0, #44]
08035272  f0f7f3fb  bl	#-63514 ; -> 0x08025a5c ; branch_target=0x08025a5c
08035276  00b9      cbnz	r0, #0 ; -> 0x0803527a ; branch_target=0x0803527a
08035278  10bd      pop	{r4, pc}
0803527a  bde81040  pop.w	{r4, lr}
0803527e  fff73fbf  b.w	#-386 ; -> 0x08035100 ; branch_target=0x08035100
