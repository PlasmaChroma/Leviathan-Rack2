; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032fd8  08b5      push	{r3, lr}
08032fda  0b48      ldr	r0, [pc, #44] ; [0x08033008] = 0x20003430
08032fdc  0023      movs	r3, #0
08032fde  0b49      ldr	r1, [pc, #44] ; [0x0803300c] = 0x40002400 / f32_bits_interpretation=2.002197266
08032fe0  4ff6ff72  movw	r2, #65535
08032fe4  8360      str	r3, [r0, #8]
08032fe6  4261      str	r2, [r0, #20]
08032fe8  0363      str	r3, [r0, #48]
08032fea  c0e90013  strd	r1, r3, [r0]
08032fee  c0e90833  strd	r3, r3, [r0, #32]
08032ff2  c0e90a33  strd	r3, r3, [r0, #40]
08032ff6  f0f71bfa  bl	#-64458 ; -> 0x08023430 ; branch_target=0x08023430
08032ffa  00b9      cbnz	r0, #0 ; -> 0x08032ffe ; branch_target=0x08032ffe
08032ffc  08bd      pop	{r3, pc}
08032ffe  bde80840  pop.w	{r3, lr}
08033002  02f07db8  b.w	#8442 ; -> 0x08035100 ; branch_target=0x08035100
