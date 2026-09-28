; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
080351f4  10b5      push	{r4, lr}
080351f6  144c      ldr	r4, [pc, #80] ; [0x08035248] = 0x40015c04 / f32_bits_interpretation=2.021241188
080351f8  c261      str	r2, [r0, #28]
080351fa  0322      movs	r2, #3
080351fc  4161      str	r1, [r0, #20]
080351fe  8360      str	r3, [r0, #8]
08035200  4163      str	r1, [r0, #52]
08035202  c0e90043  strd	r4, r3, [r0]
08035206  c0e90b11  strd	r1, r1, [r0, #44]
0803520a  f0f727fc  bl	#-63410 ; -> 0x08025a5c ; branch_target=0x08025a5c
0803520e  88b9      cbnz	r0, #34 ; -> 0x08035234 ; branch_target=0x08035234
08035210  0e48      ldr	r0, [pc, #56] ; [0x0803524c] = 0x200147e8
08035212  0122      movs	r2, #1
08035214  0021      movs	r1, #0
08035216  0223      movs	r3, #2
08035218  0d4c      ldr	r4, [pc, #52] ; [0x08035250] = 0x40015c24 / f32_bits_interpretation=2.021248817
0803521a  c261      str	r2, [r0, #28]
0803521c  0322      movs	r2, #3
0803521e  4161      str	r1, [r0, #20]
08035220  8360      str	r3, [r0, #8]
08035222  4163      str	r1, [r0, #52]
08035224  c0e90043  strd	r4, r3, [r0]
08035228  c0e90b11  strd	r1, r1, [r0, #44]
0803522c  f0f716fc  bl	#-63444 ; -> 0x08025a5c ; branch_target=0x08025a5c
08035230  18b9      cbnz	r0, #6 ; -> 0x0803523a ; branch_target=0x0803523a
08035232  10bd      pop	{r4, pc}
08035234  fff764ff  bl	#-312 ; -> 0x08035100 ; branch_target=0x08035100
08035238  eae7      b	#-44 ; -> 0x08035210 ; branch_target=0x08035210
0803523a  bde81040  pop.w	{r4, lr}
0803523e  fff75fbf  b.w	#-322 ; -> 0x08035100 ; branch_target=0x08035100
