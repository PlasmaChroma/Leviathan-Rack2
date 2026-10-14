; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
080202bc  08b5      push	{r3, lr}
080202be  034b      ldr	r3, [pc, #12] ; [0x080202cc] = 0x00000000
080202c0  1bb1      cbz	r3, #6 ; -> 0x080202ca ; branch_target=0x080202ca
080202c2  0349      ldr	r1, [pc, #12] ; [0x080202d0] = 0x20002034
080202c4  0348      ldr	r0, [pc, #12] ; [0x080202d4] = 0x080365c4
080202c6  aff30080  nop.w
080202ca  08bd      pop	{r3, pc}
