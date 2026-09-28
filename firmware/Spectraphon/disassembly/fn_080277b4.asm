; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
080277b4  08b5      push	{r3, lr}
080277b6  0122      movs	r2, #1
080277b8  806b      ldr	r0, [r0, #56]
080277ba  0023      movs	r3, #0
080277bc  a0f86a30  strh.w	r3, [r0, #106]
080277c0  a0f86230  strh.w	r3, [r0, #98]
080277c4  80f88120  strb.w	r2, [r0, #129]
080277c8  fff7c6fe  bl	#-628 ; -> 0x08027558 ; branch_target=0x08027558
080277cc  08bd      pop	{r3, pc}
