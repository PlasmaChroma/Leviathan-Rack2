; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028464  84b0      sub	sp, #16
08028466  10b4      push	{r4}
08028468  0df1080c  add.w	r12, sp, #8
0802846c  1446      mov	r4, r2
0802846e  8ce80e00  stm.w	r12, {r1, r2, r3}
08028472  0b46      mov	r3, r1
08028474  0499      ldr	r1, [sp, #16]
08028476  0246      mov	r2, r0
08028478  0598      ldr	r0, [sp, #20]
0802847a  2343      orrs	r3, r4
0802847c  5468      ldr	r4, [r2, #4]
0802847e  0b43      orrs	r3, r1
08028480  0699      ldr	r1, [sp, #24]
08028482  0343      orrs	r3, r0
08028484  0020      movs	r0, #0
08028486  0b43      orrs	r3, r1
08028488  0349      ldr	r1, [pc, #12] ; [0x08028498] = 0xffc02c00
0802848a  2140      ands	r1, r4
0802848c  0b43      orrs	r3, r1
0802848e  5360      str	r3, [r2, #4]
08028490  5df8044b  ldr	r4, [sp], #4
08028494  04b0      add	sp, #16
08028496  7047      bx	lr
