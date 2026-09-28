; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08036494  70b5      push	{r4, r5, r6, lr}
08036496  0d4d      ldr	r5, [pc, #52] ; [0x080364cc] = 0x08049abc
08036498  0d4c      ldr	r4, [pc, #52] ; [0x080364d0] = 0x08049abc
0803649a  641b      subs	r4, r4, r5
0803649c  a410      asrs	r4, r4, #2
0803649e  0026      movs	r6, #0
080364a0  a642      cmp	r6, r4
080364a2  09d1      bne	#18 ; -> 0x080364b8 ; branch_target=0x080364b8
080364a4  0b4d      ldr	r5, [pc, #44] ; [0x080364d4] = 0x08049abc
080364a6  0c4c      ldr	r4, [pc, #48] ; [0x080364d8] = 0x08049ac4
080364a8  00f08cf8  bl	#280 ; -> 0x080365c4 ; branch_target=0x080365c4
080364ac  641b      subs	r4, r4, r5
080364ae  a410      asrs	r4, r4, #2
080364b0  0026      movs	r6, #0
080364b2  a642      cmp	r6, r4
080364b4  05d1      bne	#10 ; -> 0x080364c2 ; branch_target=0x080364c2
080364b6  70bd      pop	{r4, r5, r6, pc}
080364b8  55f8043b  ldr	r3, [r5], #4
080364bc  9847      blx	r3
080364be  0136      adds	r6, #1
080364c0  eee7      b	#-36 ; -> 0x080364a0 ; branch_target=0x080364a0
080364c2  55f8043b  ldr	r3, [r5], #4
080364c6  9847      blx	r3
080364c8  0136      adds	r6, #1
080364ca  f2e7      b	#-28 ; -> 0x080364b2 ; branch_target=0x080364b2
