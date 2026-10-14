; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080215a0  bff35f8f  dmb	sy
080215a4  044b      ldr	r3, [pc, #16] ; [0x080215b8] = 0xe000ed00
080215a6  0021      movs	r1, #0
080215a8  5a6a      ldr	r2, [r3, #36]
080215aa  22f48032  bic	r2, r2, #65536
080215ae  5a62      str	r2, [r3, #36]
080215b0  c3f89410  str.w	r1, [r3, #148]
080215b4  7047      bx	lr
