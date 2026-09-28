; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
080356d4  10b5      push	{r4, lr}
080356d6  0123      movs	r3, #1
080356d8  054c      ldr	r4, [pc, #20] ; [0x080356f0] = 0x2000000f
080356da  2370      strb	r3, [r4]
080356dc  fdf72cf8  bl	#-12200 ; -> 0x08032738 ; branch_target=0x08032738
080356e0  18b9      cbnz	r0, #6 ; -> 0x080356ea ; branch_target=0x080356ea
080356e2  2378      ldrb	r3, [r4]
080356e4  03f0fe03  and	r3, r3, #254
080356e8  2370      strb	r3, [r4]
080356ea  2078      ldrb	r0, [r4]
080356ec  10bd      pop	{r4, pc}
