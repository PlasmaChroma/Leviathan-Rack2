; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
080206e8  08b5      push	{r3, lr}
080206ea  806b      ldr	r0, [r0, #56]
080206ec  436d      ldr	r3, [r0, #84]
080206ee  43f04003  orr	r3, r3, #64
080206f2  4365      str	r3, [r0, #84]
080206f4  836d      ldr	r3, [r0, #88]
080206f6  43f00403  orr	r3, r3, #4
080206fa  8365      str	r3, [r0, #88]
080206fc  fff77efe  bl	#-772 ; -> 0x080203fc ; branch_target=0x080203fc
08020700  08bd      pop	{r3, pc}
