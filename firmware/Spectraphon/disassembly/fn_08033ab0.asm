; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033ab0  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08033ab4  81b0      sub	sp, #4
08033ab6  0d46      mov	r5, r1
08033ab8  0021      movs	r1, #0
08033aba  0446      mov	r4, r0
08033abc  1646      mov	r6, r2
08033abe  1f46      mov	r7, r3
08033ac0  dde90a89  ldrd	r8, r9, [sp, #40]
08033ac4  dde90cab  ldrd	r10, r11, [sp, #48]
08033ac8  f7f73cff  bl	#-33160 ; -> 0x0802b944 ; branch_target=0x0802b944
08033acc  10b1      cbz	r0, #4 ; -> 0x08033ad4 ; branch_target=0x08033ad4
08033ace  01b0      add	sp, #4
08033ad0  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
08033ad4  3b46      mov	r3, r7
08033ad6  3246      mov	r2, r6
08033ad8  2946      mov	r1, r5
08033ada  2046      mov	r0, r4
08033adc  cdf82c90  str.w	r9, [sp, #44]
08033ae0  cdf82880  str.w	r8, [sp, #40]
08033ae4  cde90cab  strd	r10, r11, [sp, #48]
08033ae8  01b0      add	sp, #4
08033aea  bde8f04f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
08033aee  fff7d1ba  b.w	#-2654 ; -> 0x08033094 ; branch_target=0x08033094
