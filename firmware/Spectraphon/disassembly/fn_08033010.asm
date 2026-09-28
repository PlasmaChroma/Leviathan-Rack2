; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033010  00b5      push	{lr}
08033012  1d4a      ldr	r2, [pc, #116] ; [0x08033088] = 0x40002400 / f32_bits_interpretation=2.002197266
08033014  89b0      sub	sp, #36
08033016  0168      ldr	r1, [r0]
08033018  0023      movs	r3, #0
0803301a  9142      cmp	r1, r2
0803301c  0693      str	r3, [sp, #24]
0803301e  cde90233  strd	r3, r3, [sp, #8]
08033022  cde90433  strd	r3, r3, [sp, #16]
08033026  02d0      beq	#4 ; -> 0x0803302e ; branch_target=0x0803302e
08033028  09b0      add	sp, #36
0803302a  5df804fb  ldr	pc, [sp], #4
0803302e  174b      ldr	r3, [pc, #92] ; [0x0803308c] = 0x58024400
08033030  0220      movs	r0, #2
08033032  02a9      add	r1, sp, #8
08033034  d3f8e820  ldr.w	r2, [r3, #232]
08033038  42f40072  orr	r2, r2, #512
0803303c  c3f8e820  str.w	r2, [r3, #232]
08033040  d3f8e820  ldr.w	r2, [r3, #232]
08033044  02f40072  and	r2, r2, #512
08033048  0092      str	r2, [sp]
0803304a  009a      ldr	r2, [sp]
0803304c  d3f8e020  ldr.w	r2, [r3, #224]
08033050  42f04002  orr	r2, r2, #64
08033054  c3f8e020  str.w	r2, [r3, #224]
08033058  0122      movs	r2, #1
0803305a  d3f8e030  ldr.w	r3, [r3, #224]
0803305e  0590      str	r0, [sp, #20]
08033060  03f04003  and	r3, r3, #64
08033064  0a48      ldr	r0, [pc, #40] ; [0x08033090] = 0x58021800
08033066  0692      str	r2, [sp, #24]
08033068  0193      str	r3, [sp, #4]
0803306a  019b      ldr	r3, [sp, #4]
0803306c  9fed047b  vldr	d7, [pc, #16] ; [0x08033080] = 0x00002000 / f64_bits_interpretation=4.2439956293163154e-314
08033070  8ded027b  vstr	d7, [sp, #8]
08033074  f0f790f8  bl	#-65248 ; -> 0x08023198 ; branch_target=0x08023198
08033078  09b0      add	sp, #36
0803307a  5df804fb  ldr	pc, [sp], #4
