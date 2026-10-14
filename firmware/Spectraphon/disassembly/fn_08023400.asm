; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08023400  0369      ldr	r3, [r0, #16]
08023402  1942      tst	r1, r3
08023404  14bf      ite	ne
08023406  0120      movne	r0, #1
08023408  0020      moveq	r0, #0
0802340a  7047      bx	lr
