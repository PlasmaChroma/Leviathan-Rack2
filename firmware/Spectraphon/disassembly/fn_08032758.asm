; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032758  00b5      push	{lr}
0803275a  0123      movs	r3, #1
0803275c  83b0      sub	sp, #12
0803275e  8df80730  strb.w	r3, [sp, #7]
08032762  00f01dfa  bl	#1082 ; -> 0x08032ba0 ; branch_target=0x08032ba0
08032766  08b9      cbnz	r0, #2 ; -> 0x0803276c ; branch_target=0x0803276c
08032768  8df80700  strb.w	r0, [sp, #7]
0803276c  9df80700  ldrb.w	r0, [sp, #7]
08032770  03b0      add	sp, #12
08032772  5df804fb  ldr	pc, [sp], #4
