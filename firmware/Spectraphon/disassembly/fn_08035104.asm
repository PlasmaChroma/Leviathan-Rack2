; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08035104  08b5      push	{r3, lr}
08035106  0648      ldr	r0, [pc, #24] ; [0x08035120] = 0x200144d8
08035108  0023      movs	r3, #0
0803510a  064a      ldr	r2, [pc, #24] ; [0x08035124] = 0x48021800 / f32_bits_interpretation=133216
0803510c  c0e90023  strd	r2, r3, [r0]
08035110  f0f786fa  bl	#-64244 ; -> 0x08025620 ; branch_target=0x08025620
08035114  00b9      cbnz	r0, #0 ; -> 0x08035118 ; branch_target=0x08035118
08035116  08bd      pop	{r3, pc}
08035118  bde80840  pop.w	{r3, lr}
0803511c  fff7f0bf  b.w	#-32 ; -> 0x08035100 ; branch_target=0x08035100
