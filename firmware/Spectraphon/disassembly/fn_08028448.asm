; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028448  0246      mov	r2, r0
0802844a  054b      ldr	r3, [pc, #20] ; [0x08028460] = 0xffffc001
0802844c  0020      movs	r0, #0
0802844e  10b4      push	{r4}
08028450  5469      ldr	r4, [r2, #20]
08028452  2340      ands	r3, r4
08028454  43ea4103  orr.w	r3, r3, r1, lsl #1
08028458  5361      str	r3, [r2, #20]
0802845a  5df8044b  ldr	r4, [sp], #4
0802845e  7047      bx	lr
