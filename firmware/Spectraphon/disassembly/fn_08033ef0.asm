; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033ef0  10b5      push	{r4, lr}
08033ef2  b0b0      sub	sp, #192
08033ef4  a422      movs	r2, #164
08033ef6  0021      movs	r1, #0
08033ef8  0124      movs	r4, #1
08033efa  06a8      add	r0, sp, #24
08033efc  02f0c1fa  bl	#9602 ; -> 0x08036482 ; branch_target=0x08036482
08033f00  1148      ldr	r0, [pc, #68] ; [0x08033f48] = 0x00090720
08033f02  1923      movs	r3, #25
08033f04  4ff4ac71  mov.w	r1, #344
08033f08  0722      movs	r2, #7
08033f0a  cde90003  strd	r0, r3, [sp]
08033f0e  4ff48033  mov.w	r3, #65536
08033f12  6846      mov	r0, sp
08033f14  9fed0a7b  vldr	d7, [pc, #40] ; [0x08033f40] = 0x00000002 / f64_bits_interpretation=4.2439915829186759e-314
08033f18  cde90212  strd	r1, r2, [sp, #8]
08033f1c  1393      str	r3, [sp, #76]
08033f1e  4021      movs	r1, #64
08033f20  4ff08052  mov.w	r2, #268435456
08033f24  4ff40013  mov.w	r3, #2097152
08033f28  8ded047b  vstr	d7, [sp, #16]
08033f2c  cde91541  strd	r4, r1, [sp, #84]
08033f30  2392      str	r2, [sp, #140]
08033f32  2993      str	r3, [sp, #164]
08033f34  f0f798f9  bl	#-64720 ; -> 0x08024268 ; branch_target=0x08024268
08033f38  08b9      cbnz	r0, #2 ; -> 0x08033f3e ; branch_target=0x08033f3e
08033f3a  30b0      add	sp, #192
08033f3c  10bd      pop	{r4, pc}
08033f3e  fee7      b	#-4 ; -> 0x08033f3e ; branch_target=0x08033f3e
