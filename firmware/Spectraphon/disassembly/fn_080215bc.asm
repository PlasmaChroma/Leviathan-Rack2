; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080215bc  064b      ldr	r3, [pc, #24] ; [0x080215d8] = 0xe000ed00
080215be  40f00100  orr	r0, r0, #1
080215c2  c3f89400  str.w	r0, [r3, #148]
080215c6  5a6a      ldr	r2, [r3, #36]
080215c8  42f48032  orr	r2, r2, #65536
080215cc  5a62      str	r2, [r3, #36]
080215ce  bff34f8f  dsb	sy
080215d2  bff36f8f  isb	sy
080215d6  7047      bx	lr
