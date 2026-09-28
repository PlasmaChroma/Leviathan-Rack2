; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028418  10b4      push	{r4}
0802841a  d1e90034  ldrd	r3, r4, [r1]
0802841e  0246      mov	r2, r0
08028420  0069      ldr	r0, [r0, #16]
08028422  2343      orrs	r3, r4
08028424  d1e90214  ldrd	r1, r4, [r1, #8]
08028428  43ea4423  orr.w	r3, r3, r4, lsl #9
0802842c  054c      ldr	r4, [pc, #20] ; [0x08028444] = 0xffc00000
0802842e  0139      subs	r1, #1
08028430  0440      ands	r4, r0
08028432  0020      movs	r0, #0
08028434  2343      orrs	r3, r4
08028436  43ea4113  orr.w	r3, r3, r1, lsl #5
0802843a  1361      str	r3, [r2, #16]
0802843c  5df8044b  ldr	r4, [sp], #4
08028440  7047      bx	lr
