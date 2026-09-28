; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020d3c  38b5      push	{r3, r4, r5, lr}
08020d3e  0368      ldr	r3, [r0]
08020d40  9a68      ldr	r2, [r3, #8]
08020d42  9507      lsls	r5, r2, #30
08020d44  02d5      bpl	#4 ; -> 0x08020d4c ; branch_target=0x08020d4c
08020d46  9b68      ldr	r3, [r3, #8]
08020d48  0020      movs	r0, #0
08020d4a  38bd      pop	{r3, r4, r5, pc}
08020d4c  9a68      ldr	r2, [r3, #8]
08020d4e  d407      lsls	r4, r2, #31
08020d50  fad5      bpl	#-12 ; -> 0x08020d48 ; branch_target=0x08020d48
08020d52  9a68      ldr	r2, [r3, #8]
08020d54  0446      mov	r4, r0
08020d56  02f00d02  and	r2, r2, #13
08020d5a  012a      cmp	r2, #1
08020d5c  09d0      beq	#18 ; -> 0x08020d72 ; branch_target=0x08020d72
08020d5e  636d      ldr	r3, [r4, #84]
08020d60  0120      movs	r0, #1
08020d62  43f01003  orr	r3, r3, #16
08020d66  6365      str	r3, [r4, #84]
08020d68  a36d      ldr	r3, [r4, #88]
08020d6a  43f00103  orr	r3, r3, #1
08020d6e  a365      str	r3, [r4, #88]
08020d70  38bd      pop	{r3, r4, r5, pc}
08020d72  9868      ldr	r0, [r3, #8]
08020d74  0321      movs	r1, #3
08020d76  0e4a      ldr	r2, [pc, #56] ; [0x08020db0] = 0x7fffffc0
08020d78  0240      ands	r2, r0
08020d7a  42f00202  orr	r2, r2, #2
08020d7e  9a60      str	r2, [r3, #8]
08020d80  2368      ldr	r3, [r4]
08020d82  1960      str	r1, [r3]
08020d84  fff70afb  bl	#-2540 ; -> 0x0802039c ; branch_target=0x0802039c
08020d88  2368      ldr	r3, [r4]
08020d8a  0546      mov	r5, r0
08020d8c  9b68      ldr	r3, [r3, #8]
08020d8e  d907      lsls	r1, r3, #31
08020d90  03d4      bmi	#6 ; -> 0x08020d9a ; branch_target=0x08020d9a
08020d92  d9e7      b	#-78 ; -> 0x08020d48 ; branch_target=0x08020d48
08020d94  9b68      ldr	r3, [r3, #8]
08020d96  db07      lsls	r3, r3, #31
08020d98  d6d5      bpl	#-84 ; -> 0x08020d48 ; branch_target=0x08020d48
08020d9a  fff7fffa  bl	#-2562 ; -> 0x0802039c ; branch_target=0x0802039c
08020d9e  401b      subs	r0, r0, r5
08020da0  2368      ldr	r3, [r4]
08020da2  0228      cmp	r0, #2
08020da4  f6d9      bls	#-20 ; -> 0x08020d94 ; branch_target=0x08020d94
08020da6  9a68      ldr	r2, [r3, #8]
08020da8  d207      lsls	r2, r2, #31
08020daa  f3d5      bpl	#-26 ; -> 0x08020d94 ; branch_target=0x08020d94
08020dac  d7e7      b	#-82 ; -> 0x08020d5e ; branch_target=0x08020d5e
