; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080357a4  10b4      push	{r4}
080357a6  074b      ldr	r3, [pc, #28] ; [0x080357c4] = 0x20014a48
080357a8  0022      movs	r2, #0
080357aa  074c      ldr	r4, [pc, #28] ; [0x080357c8] = 0x52007000
080357ac  4ff48040  mov.w	r0, #16384
080357b0  0821      movs	r1, #8
080357b2  c3e90042  strd	r4, r2, [r3]
080357b6  c3e90220  strd	r2, r0, [r3, #8]
080357ba  c3e90421  strd	r2, r1, [r3, #16]
080357be  5df8044b  ldr	r4, [sp], #4
080357c2  7047      bx	lr
