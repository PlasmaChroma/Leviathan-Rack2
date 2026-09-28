; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
08020298  10b5      push	{r4, lr}
0802029a  054c      ldr	r4, [pc, #20] ; [0x080202b0] = 0x20002030
0802029c  2378      ldrb	r3, [r4]
0802029e  33b9      cbnz	r3, #12 ; -> 0x080202ae ; branch_target=0x080202ae
080202a0  044b      ldr	r3, [pc, #16] ; [0x080202b4] = 0x00000000
080202a2  13b1      cbz	r3, #4 ; -> 0x080202aa ; branch_target=0x080202aa
080202a4  0448      ldr	r0, [pc, #16] ; [0x080202b8] = 0x080365c4
080202a6  aff30080  nop.w
080202aa  0123      movs	r3, #1
080202ac  2370      strb	r3, [r4]
080202ae  10bd      pop	{r4, pc}
