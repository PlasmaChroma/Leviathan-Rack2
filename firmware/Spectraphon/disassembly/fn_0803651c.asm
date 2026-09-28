; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
0803651c  38b5      push	{r3, r4, r5, lr}
0803651e  074d      ldr	r5, [pc, #28] ; [0x0803653c] = 0x08049ac4
08036520  074c      ldr	r4, [pc, #28] ; [0x08036540] = 0x08049ac8
08036522  641b      subs	r4, r4, r5
08036524  a410      asrs	r4, r4, #2
08036526  1cb9      cbnz	r4, #6 ; -> 0x08036530 ; branch_target=0x08036530
08036528  bde83840  pop.w	{r3, r4, r5, lr}
0803652c  00f050b8  b.w	#160 ; -> 0x080365d0 ; branch_target=0x080365d0
08036530  013c      subs	r4, #1
08036532  55f82430  ldr.w	r3, [r5, r4, lsl #2]
08036536  9847      blx	r3
08036538  f5e7      b	#-22 ; -> 0x08036526 ; branch_target=0x08036526
