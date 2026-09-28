; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029930  044b      ldr	r3, [pc, #16] ; [0x08029944] = 0x20002094
08029932  03eb000c  add.w	r12, r3, r0
08029936  03eb8003  add.w	r3, r3, r0, lsl #2
0802993a  5b68      ldr	r3, [r3, #4]
0802993c  9cf80800  ldrb.w	r0, [r12, #8]
08029940  1b69      ldr	r3, [r3, #16]
08029942  1847      bx	r3
