; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
080356a4  38b5      push	{r3, r4, r5, lr}
080356a6  0a4c      ldr	r4, [pc, #40] ; [0x080356d0] = 0x2000000f
080356a8  0125      movs	r5, #1
080356aa  2570      strb	r5, [r4]
080356ac  fdf764f8  bl	#-12088 ; -> 0x08032778 ; branch_target=0x08032778
080356b0  08b1      cbz	r0, #2 ; -> 0x080356b6 ; branch_target=0x080356b6
080356b2  2078      ldrb	r0, [r4]
080356b4  38bd      pop	{r3, r4, r5, pc}
080356b6  2570      strb	r5, [r4]
080356b8  fdf73ef8  bl	#-12164 ; -> 0x08032738 ; branch_target=0x08032738
080356bc  18b9      cbnz	r0, #6 ; -> 0x080356c6 ; branch_target=0x080356c6
080356be  2378      ldrb	r3, [r4]
080356c0  03f0fe03  and	r3, r3, #254
080356c4  2370      strb	r3, [r4]
080356c6  2378      ldrb	r3, [r4]
080356c8  dbb2      uxtb	r3, r3
080356ca  2370      strb	r3, [r4]
080356cc  2078      ldrb	r0, [r4]
080356ce  38bd      pop	{r3, r4, r5, pc}
