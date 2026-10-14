; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080284d0  30b4      push	{r4, r5}
080284d2  0b68      ldr	r3, [r1]
080284d4  0246      mov	r2, r0
080284d6  4362      str	r3, [r0, #36]
080284d8  4b68      ldr	r3, [r1, #4]
080284da  8362      str	r3, [r0, #40]
080284dc  0d69      ldr	r5, [r1, #16]
080284de  c06a      ldr	r0, [r0, #44]
080284e0  d1e90234  ldrd	r3, r4, [r1, #8]
080284e4  2343      orrs	r3, r4
080284e6  4c69      ldr	r4, [r1, #20]
080284e8  20f0ff01  bic	r1, r0, #255
080284ec  0020      movs	r0, #0
080284ee  2b43      orrs	r3, r5
080284f0  2343      orrs	r3, r4
080284f2  0b43      orrs	r3, r1
080284f4  d362      str	r3, [r2, #44]
080284f6  30bc      pop	{r4, r5}
080284f8  7047      bx	lr
