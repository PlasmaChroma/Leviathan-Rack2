; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022c38  10b5      push	{r4, lr}
08022c3a  d0e91923  ldrd	r2, r3, [r0, #100]
08022c3e  0446      mov	r4, r0
08022c40  1268      ldr	r2, [r2]
08022c42  1a42      tst	r2, r3
08022c44  0ed0      beq	#28 ; -> 0x08022c64 ; branch_target=0x08022c64
08022c46  026e      ldr	r2, [r0, #96]
08022c48  1368      ldr	r3, [r2]
08022c4a  23f48073  bic	r3, r3, #256
08022c4e  1360      str	r3, [r2]
08022c50  d0e91932  ldrd	r3, r2, [r0, #100]
08022c54  5a60      str	r2, [r3, #4]
08022c56  436d      ldr	r3, [r0, #84]
08022c58  c26c      ldr	r2, [r0, #76]
08022c5a  43f40073  orr	r3, r3, #512
08022c5e  4365      str	r3, [r0, #84]
08022c60  02b1      cbz	r2, #0 ; -> 0x08022c64 ; branch_target=0x08022c64
08022c62  9047      blx	r2
08022c64  e36e      ldr	r3, [r4, #108]
08022c66  abb1      cbz	r3, #42 ; -> 0x08022c94 ; branch_target=0x08022c94
08022c68  d4e91c12  ldrd	r1, r2, [r4, #112]
08022c6c  0968      ldr	r1, [r1]
08022c6e  1142      tst	r1, r2
08022c70  10d0      beq	#32 ; -> 0x08022c94 ; branch_target=0x08022c94
08022c72  1a68      ldr	r2, [r3]
08022c74  22f48072  bic	r2, r2, #256
08022c78  1a60      str	r2, [r3]
08022c7a  d4e91c32  ldrd	r3, r2, [r4, #112]
08022c7e  5a60      str	r2, [r3, #4]
08022c80  636d      ldr	r3, [r4, #84]
08022c82  e26c      ldr	r2, [r4, #76]
08022c84  43f48063  orr	r3, r3, #1024
08022c88  6365      str	r3, [r4, #84]
08022c8a  1ab1      cbz	r2, #6 ; -> 0x08022c94 ; branch_target=0x08022c94
08022c8c  2046      mov	r0, r4
08022c8e  bde81040  pop.w	{r4, lr}
08022c92  1047      bx	r2
08022c94  10bd      pop	{r4, pc}
