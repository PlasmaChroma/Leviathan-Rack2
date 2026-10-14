; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802831c  d1e90732  ldrd	r3, r2, [r1, #28]
08028320  30b4      push	{r4, r5}
08028322  1343      orrs	r3, r2
08028324  4c6a      ldr	r4, [r1, #36]
08028326  0d68      ldr	r5, [r1]
08028328  2343      orrs	r3, r4
0802832a  164a      ldr	r2, [pc, #88] ; [0x08028384] = 0xffff8000
0802832c  0468      ldr	r4, [r0]
0802832e  8db9      cbnz	r5, #34 ; -> 0x08028354 ; branch_target=0x08028354
08028330  2240      ands	r2, r4
08028332  4c68      ldr	r4, [r1, #4]
08028334  1343      orrs	r3, r2
08028336  2343      orrs	r3, r4
08028338  d1e90242  ldrd	r4, r2, [r1, #8]
0802833c  2343      orrs	r3, r4
0802833e  d1e90454  ldrd	r5, r4, [r1, #16]
08028342  1343      orrs	r3, r2
08028344  8a69      ldr	r2, [r1, #24]
08028346  2b43      orrs	r3, r5
08028348  2343      orrs	r3, r4
0802834a  1343      orrs	r3, r2
0802834c  0360      str	r3, [r0]
0802834e  0020      movs	r0, #0
08028350  30bc      pop	{r4, r5}
08028352  7047      bx	lr
08028354  24f4f84c  bic	r12, r4, #31744
08028358  43ea0c03  orr.w	r3, r3, r12
0802835c  0360      str	r3, [r0]
0802835e  4468      ldr	r4, [r0, #4]
08028360  4b68      ldr	r3, [r1, #4]
08028362  2240      ands	r2, r4
08028364  1343      orrs	r3, r2
08028366  d1e90242  ldrd	r4, r2, [r1, #8]
0802836a  2343      orrs	r3, r4
0802836c  d1e90454  ldrd	r5, r4, [r1, #16]
08028370  1343      orrs	r3, r2
08028372  8a69      ldr	r2, [r1, #24]
08028374  2b43      orrs	r3, r5
08028376  2343      orrs	r3, r4
08028378  1343      orrs	r3, r2
0802837a  4360      str	r3, [r0, #4]
0802837c  0020      movs	r0, #0
0802837e  30bc      pop	{r4, r5}
08028380  7047      bx	lr
