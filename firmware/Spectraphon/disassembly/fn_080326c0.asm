; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
080326c0  10b5      push	{r4, lr}
080326c2  d3f8e040  ldr.w	r4, [r3, #224]
080326c6  82b0      sub	sp, #8
080326c8  44f40014  orr	r4, r4, #2097152
080326cc  c3f8e040  str.w	r4, [r3, #224]
080326d0  d3f8e030  ldr.w	r3, [r3, #224]
080326d4  03f40013  and	r3, r3, #2097152
080326d8  0193      str	r3, [sp, #4]
080326da  019b      ldr	r3, [sp, #4]
080326dc  eef700ff  bl	#-70144 ; -> 0x080214e0 ; branch_target=0x080214e0
080326e0  8120      movs	r0, #129
080326e2  02b0      add	sp, #8
080326e4  bde81040  pop.w	{r4, lr}
080326e8  eef736bf  b.w	#-70036 ; -> 0x08021558 ; branch_target=0x08021558
