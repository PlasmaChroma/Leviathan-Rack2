; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080203d8  034a      ldr	r2, [pc, #12] ; [0x080203e8] = 0x52004000
080203da  1368      ldr	r3, [r2]
080203dc  23f04073  bic	r3, r3, #50331648
080203e0  0343      orrs	r3, r0
080203e2  1360      str	r3, [r2]
080203e4  7047      bx	lr
