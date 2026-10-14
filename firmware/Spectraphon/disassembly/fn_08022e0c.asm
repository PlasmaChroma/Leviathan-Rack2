; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08022e0c  0f4b      ldr	r3, [pc, #60] ; [0x08022e4c] = 0x52002000
08022e0e  da68      ldr	r2, [r3, #12]
08022e10  d207      lsls	r2, r2, #31
08022e12  06d5      bpl	#12 ; -> 0x08022e22 ; branch_target=0x08022e22
08022e14  0e49      ldr	r1, [pc, #56] ; [0x08022e50] = 0x45670123 / f32_bits_interpretation=3696.071045
08022e16  0f4a      ldr	r2, [pc, #60] ; [0x08022e54] = 0xcdef89ab / f32_bits_interpretation=-502347104
08022e18  5960      str	r1, [r3, #4]
08022e1a  5a60      str	r2, [r3, #4]
08022e1c  db68      ldr	r3, [r3, #12]
08022e1e  db07      lsls	r3, r3, #31
08022e20  11d4      bmi	#34 ; -> 0x08022e46 ; branch_target=0x08022e46
08022e22  0a4b      ldr	r3, [pc, #40] ; [0x08022e4c] = 0x52002000
08022e24  d3f80c01  ldr.w	r0, [r3, #268]
08022e28  10f00100  ands	r0, r0, #1
08022e2c  0ad0      beq	#20 ; -> 0x08022e44 ; branch_target=0x08022e44
08022e2e  0849      ldr	r1, [pc, #32] ; [0x08022e50] = 0x45670123 / f32_bits_interpretation=3696.071045
08022e30  084a      ldr	r2, [pc, #32] ; [0x08022e54] = 0xcdef89ab / f32_bits_interpretation=-502347104
08022e32  c3f80411  str.w	r1, [r3, #260]
08022e36  c3f80421  str.w	r2, [r3, #260]
08022e3a  d3f80c01  ldr.w	r0, [r3, #268]
08022e3e  00f00100  and	r0, r0, #1
08022e42  7047      bx	lr
08022e44  7047      bx	lr
08022e46  0120      movs	r0, #1
08022e48  7047      bx	lr
