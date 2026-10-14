; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08026d4c  0028      cmp	r0, #0
08026d4e  00f0a380  beq.w	#326 ; -> 0x08026e98 ; branch_target=0x08026e98
08026d52  5449      ldr	r1, [pc, #336] ; [0x08026ea4] = 0x40013000 / f32_bits_interpretation=2.018554688
08026e98  0120      movs	r0, #1
08026e9a  7047      bx	lr
