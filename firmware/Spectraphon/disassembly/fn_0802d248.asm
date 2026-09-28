; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802d248  00b5      push	{lr}
0802d24a  89b0      sub	sp, #36
0802d24c  f5f7defd  bl	#-42052 ; -> 0x08022e0c ; branch_target=0x08022e0c
0802d250  0020      movs	r0, #0
0802d252  0221      movs	r1, #2
0802d254  2023      movs	r3, #32
0802d256  9fed087b  vldr	d7, [pc, #32] ; [0x0802d278] = 0x00000007 / f64_bits_interpretation=2.1219957944237318e-314
0802d25a  cde90201  strd	r0, r1, [sp, #8]
0802d25e  01a9      add	r1, sp, #4
0802d260  02a8      add	r0, sp, #8
0802d262  0693      str	r3, [sp, #24]
0802d264  8ded047b  vstr	d7, [sp, #16]
0802d268  f5f7c8fe  bl	#-41584 ; -> 0x08022ffc ; branch_target=0x08022ffc
0802d26c  f5f7f4fd  bl	#-42008 ; -> 0x08022e58 ; branch_target=0x08022e58
0802d270  09b0      add	sp, #36
0802d272  5df804fb  ldr	pc, [sp], #4
