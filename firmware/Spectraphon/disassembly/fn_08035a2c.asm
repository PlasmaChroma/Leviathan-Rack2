; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08035a2c  074b      ldr	r3, [pc, #28] ; [0x08035a4c] = 0x58024400
08035a2e  82b0      sub	sp, #8
08035a30  d3f8f420  ldr.w	r2, [r3, #244]
08035a34  42f00202  orr	r2, r2, #2
08035a38  c3f8f420  str.w	r2, [r3, #244]
08035a3c  d3f8f430  ldr.w	r3, [r3, #244]
08035a40  03f00203  and	r3, r3, #2
08035a44  0193      str	r3, [sp, #4]
08035a46  019b      ldr	r3, [sp, #4]
08035a48  02b0      add	sp, #8
08035a4a  7047      bx	lr
