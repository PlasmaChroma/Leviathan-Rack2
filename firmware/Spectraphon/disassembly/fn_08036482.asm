; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08036482  0244      add	r2, r0
08036484  0346      mov	r3, r0
08036486  9342      cmp	r3, r2
08036488  00d1      bne	#0 ; -> 0x0803648c ; branch_target=0x0803648c
0803648a  7047      bx	lr
0803648c  03f8011b  strb	r1, [r3], #1
08036490  f9e7      b	#-14 ; -> 0x08036486 ; branch_target=0x08036486
