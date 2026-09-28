; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08035898  1448      ldr	r0, [pc, #80] ; [0x080358ec] = 0x20014bb8
0803589a  0023      movs	r3, #0
0803589c  1449      ldr	r1, [pc, #80] ; [0x080358f0] = 0x40015000 / f32_bits_interpretation=2.020507812
0803589e  4ff48002  mov.w	r2, #4194304
080358a2  10b5      push	{r4, lr}
080358a4  80e80e00  stm.w	r0, {r1, r2, r3}
080358a8  4ff00052  mov.w	r2, #536870912
080358ac  0724      movs	r4, #7
080358ae  4ff0a041  mov.w	r1, #1342177280
080358b2  c362      str	r3, [r0, #44]
080358b4  c0e90532  strd	r3, r2, [r0, #20]
080358b8  4ff08042  mov.w	r2, #1073741824
080358bc  c0e90343  strd	r4, r3, [r0, #12]
080358c0  c0e90713  strd	r1, r3, [r0, #28]
080358c4  c0e90933  strd	r3, r3, [r0, #36]
080358c8  c0e90d23  strd	r2, r3, [r0, #52]
080358cc  c0e90f33  strd	r3, r3, [r0, #60]
080358d0  c0e91133  strd	r3, r3, [r0, #68]
080358d4  c0e91333  strd	r3, r3, [r0, #76]
080358d8  c0e91533  strd	r3, r3, [r0, #84]
080358dc  f1f736fa  bl	#-60308 ; -> 0x08026d4c ; branch_target=0x08026d4c
080358e0  00b9      cbnz	r0, #0 ; -> 0x080358e4 ; branch_target=0x080358e4
080358e2  10bd      pop	{r4, pc}
080358e4  bde81040  pop.w	{r4, lr}
080358e8  fff70abc  b.w	#-2028 ; -> 0x08035100 ; branch_target=0x08035100
