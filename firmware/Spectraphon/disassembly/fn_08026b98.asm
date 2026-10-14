; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026b98  10b5      push	{r4, lr}
08026b9a  416c      ldr	r1, [r0, #68]
08026b9c  0446      mov	r4, r0
08026b9e  0068      ldr	r0, [r0]
08026ba0  0904      lsls	r1, r1, #16
08026ba2  02f03bfd  bl	#10870 ; -> 0x0802961c ; branch_target=0x0802961c
08026ba6  0146      mov	r1, r0
08026ba8  20b1      cbz	r0, #8 ; -> 0x08026bb4 ; branch_target=0x08026bb4
08026baa  636b      ldr	r3, [r4, #52]
08026bac  0020      movs	r0, #0
08026bae  0b43      orrs	r3, r1
08026bb0  6363      str	r3, [r4, #52]
08026bb2  10bd      pop	{r4, pc}
08026bb4  2068      ldr	r0, [r4]
08026bb6  01f087fc  bl	#6414 ; -> 0x080284c8 ; branch_target=0x080284c8
08026bba  c0f34320  ubfx	r0, r0, #9, #4
08026bbe  10bd      pop	{r4, pc}
