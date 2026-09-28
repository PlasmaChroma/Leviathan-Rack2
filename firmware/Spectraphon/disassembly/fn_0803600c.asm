; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0803600c  134a      ldr	r2, [pc, #76] ; [0x0803605c] = 0x40000400 / f32_bits_interpretation=2.000244141
0803600e  82b0      sub	sp, #8
08036010  0368      ldr	r3, [r0]
08036012  9342      cmp	r3, r2
08036014  13d0      beq	#38 ; -> 0x0803603e ; branch_target=0x0803603e
08036016  124a      ldr	r2, [pc, #72] ; [0x08036060] = 0x40000800 / f32_bits_interpretation=2.000488281
08036018  9342      cmp	r3, r2
0803601a  01d0      beq	#2 ; -> 0x08036020 ; branch_target=0x08036020
0803601c  02b0      add	sp, #8
0803601e  7047      bx	lr
08036020  104b      ldr	r3, [pc, #64] ; [0x08036064] = 0x58024400
08036022  d3f8e820  ldr.w	r2, [r3, #232]
08036026  42f00402  orr	r2, r2, #4
0803602a  c3f8e820  str.w	r2, [r3, #232]
0803602e  d3f8e830  ldr.w	r3, [r3, #232]
08036032  03f00403  and	r3, r3, #4
08036036  0193      str	r3, [sp, #4]
08036038  019b      ldr	r3, [sp, #4]
0803603a  02b0      add	sp, #8
0803603c  7047      bx	lr
0803603e  094b      ldr	r3, [pc, #36] ; [0x08036064] = 0x58024400
08036040  d3f8e820  ldr.w	r2, [r3, #232]
08036044  42f00202  orr	r2, r2, #2
08036048  c3f8e820  str.w	r2, [r3, #232]
0803604c  d3f8e830  ldr.w	r3, [r3, #232]
08036050  03f00203  and	r3, r3, #2
08036054  0093      str	r3, [sp]
08036056  009b      ldr	r3, [sp]
08036058  02b0      add	sp, #8
0803605a  7047      bx	lr
