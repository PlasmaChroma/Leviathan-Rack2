; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08020b6c  0368      ldr	r3, [r0]
08020b6e  9a68      ldr	r2, [r3, #8]
08020b70  d107      lsls	r1, r2, #31
08020b72  01d5      bpl	#2 ; -> 0x08020b78 ; branch_target=0x08020b78
08020b74  0020      movs	r0, #0
08020b76  7047      bx	lr
