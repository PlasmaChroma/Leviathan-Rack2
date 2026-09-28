; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08035128  10b5      push	{r4, lr}
0803512a  b0b0      sub	sp, #192
0803512c  0446      mov	r4, r0
0803512e  bc22      movs	r2, #188
08035130  0021      movs	r1, #0
08035132  01a8      add	r0, sp, #4
08035134  01f0a5f9  bl	#4938 ; -> 0x08036482 ; branch_target=0x08036482
08035138  0f4b      ldr	r3, [pc, #60] ; [0x08035178] = 0x48021800 / f32_bits_interpretation=133216
0803513a  2268      ldr	r2, [r4]
0803513c  9a42      cmp	r2, r3
0803513e  01d0      beq	#2 ; -> 0x08035144 ; branch_target=0x08035144
08035140  30b0      add	sp, #192
08035142  10bd      pop	{r4, pc}
08035144  4ff40033  mov.w	r3, #131072
08035148  01a8      add	r0, sp, #4
0803514a  0193      str	r3, [sp, #4]
0803514c  eff78cf8  bl	#-69352 ; -> 0x08024268 ; branch_target=0x08024268
08035150  70b9      cbnz	r0, #28 ; -> 0x08035170 ; branch_target=0x08035170
08035152  0a4b      ldr	r3, [pc, #40] ; [0x0803517c] = 0x58024400
08035154  d3f8dc20  ldr.w	r2, [r3, #220]
08035158  42f04002  orr	r2, r2, #64
0803515c  c3f8dc20  str.w	r2, [r3, #220]
08035160  d3f8dc30  ldr.w	r3, [r3, #220]
08035164  03f04003  and	r3, r3, #64
08035168  0093      str	r3, [sp]
0803516a  009b      ldr	r3, [sp]
0803516c  30b0      add	sp, #192
0803516e  10bd      pop	{r4, pc}
08035170  fff7c6ff  bl	#-116 ; -> 0x08035100 ; branch_target=0x08035100
08035174  ede7      b	#-38 ; -> 0x08035152 ; branch_target=0x08035152
