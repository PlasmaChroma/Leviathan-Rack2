; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08035188  10b5      push	{r4, lr}
0803518a  154c      ldr	r4, [pc, #84] ; [0x080351e0] = 0x40015804 / f32_bits_interpretation=2.020997047
0803518c  c261      str	r2, [r0, #28]
0803518e  c260      str	r2, [r0, #12]
08035190  0322      movs	r2, #3
08035192  0460      str	r4, [r0]
08035194  4161      str	r1, [r0, #20]
08035196  4163      str	r1, [r0, #52]
08035198  c0e90131  strd	r3, r1, [r0, #4]
0803519c  c0e90b11  strd	r1, r1, [r0, #44]
080351a0  f0f75cfc  bl	#-63304 ; -> 0x08025a5c ; branch_target=0x08025a5c
080351a4  90b9      cbnz	r0, #36 ; -> 0x080351cc ; branch_target=0x080351cc
080351a6  0f48      ldr	r0, [pc, #60] ; [0x080351e4] = 0x20014918
080351a8  0123      movs	r3, #1
080351aa  0021      movs	r1, #0
080351ac  0322      movs	r2, #3
080351ae  0e4c      ldr	r4, [pc, #56] ; [0x080351e8] = 0x40015824 / f32_bits_interpretation=2.021004677
080351b0  8360      str	r3, [r0, #8]
080351b2  c361      str	r3, [r0, #28]
080351b4  c360      str	r3, [r0, #12]
080351b6  0223      movs	r3, #2
080351b8  4161      str	r1, [r0, #20]
080351ba  4163      str	r1, [r0, #52]
080351bc  c0e90042  strd	r4, r2, [r0]
080351c0  c0e90b11  strd	r1, r1, [r0, #44]
080351c4  f0f74afc  bl	#-63340 ; -> 0x08025a5c ; branch_target=0x08025a5c
080351c8  18b9      cbnz	r0, #6 ; -> 0x080351d2 ; branch_target=0x080351d2
080351ca  10bd      pop	{r4, pc}
080351cc  fff798ff  bl	#-208 ; -> 0x08035100 ; branch_target=0x08035100
080351d0  e9e7      b	#-46 ; -> 0x080351a6 ; branch_target=0x080351a6
080351d2  bde81040  pop.w	{r4, lr}
080351d6  fff793bf  b.w	#-218 ; -> 0x08035100 ; branch_target=0x08035100
