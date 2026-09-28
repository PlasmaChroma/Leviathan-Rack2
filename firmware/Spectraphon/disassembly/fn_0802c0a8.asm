; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802c0a8  10b5      push	{r4, lr}
0802c0aa  4648      ldr	r0, [pc, #280] ; [0x0802c1c4] = 0x200021f8
0802c0ac  4ff48022  mov.w	r2, #262144
0802c0b0  454c      ldr	r4, [pc, #276] ; [0x0802c1c8] = 0x40022000 / f32_bits_interpretation=2.033203125
0802c0b2  0023      movs	r3, #0
0802c0b4  0121      movs	r1, #1
0802c0b6  8ab0      sub	sp, #40
0802c0b8  8382      strh	r3, [r0, #20]
0802c0ba  0093      str	r3, [sp]
0802c0bc  0393      str	r3, [sp, #12]
0802c0be  0377      strb	r3, [r0, #28]
0802c0c0  4363      str	r3, [r0, #52]
0802c0c2  80f83830  strb.w	r3, [r0, #56]
0802c0c6  c0e90042  strd	r4, r2, [r0]
0802c0ca  0822      movs	r2, #8
0802c0cc  0424      movs	r4, #4
0802c0ce  0261      str	r2, [r0, #16]
0802c0d0  4ff49862  mov.w	r2, #1216
0802c0d4  8461      str	r4, [r0, #24]
0802c0d6  4262      str	r2, [r0, #36]
0802c0d8  0322      movs	r2, #3
0802c0da  c0e90231  strd	r3, r1, [r0, #8]
0802c0de  4ff48061  mov.w	r1, #1024
0802c0e2  c0e90b23  strd	r2, r3, [r0, #44]
0802c0e6  8162      str	r1, [r0, #40]
0802c0e8  cde90133  strd	r3, r3, [sp, #4]
0802c0ec  cde90433  strd	r3, r3, [sp, #16]
0802c0f0  cde90633  strd	r3, r3, [sp, #24]
0802c0f4  cde90833  strd	r3, r3, [sp, #32]
0802c0f8  f4f716ff  bl	#-45524 ; -> 0x08020f28 ; branch_target=0x08020f28
0802c0fc  0028      cmp	r0, #0
0802c0fe  46d1      bne	#140 ; -> 0x0802c18e ; branch_target=0x0802c18e
0802c100  0023      movs	r3, #0
0802c102  6946      mov	r1, sp
0802c104  2f48      ldr	r0, [pc, #188] ; [0x0802c1c4] = 0x200021f8
0802c106  0093      str	r3, [sp]
0802c108  f5f75cf9  bl	#-44360 ; -> 0x080213c4 ; branch_target=0x080213c4
0802c10c  0028      cmp	r0, #0
0802c10e  4dd1      bne	#154 ; -> 0x0802c1ac ; branch_target=0x0802c1ac
0802c110  0422      movs	r2, #4
0802c112  0620      movs	r0, #6
0802c114  2d4c      ldr	r4, [pc, #180] ; [0x0802c1cc] = 0x47520000 / f32_bits_interpretation=53760
0802c116  0023      movs	r3, #0
0802c118  0592      str	r2, [sp, #20]
0802c11a  03a9      add	r1, sp, #12
0802c11c  0792      str	r2, [sp, #28]
0802c11e  40f2ff72  movw	r2, #2047
0802c122  0893      str	r3, [sp, #32]
0802c124  8df82530  strb.w	r3, [sp, #37]
0802c128  0692      str	r2, [sp, #24]
0802c12a  cde90340  strd	r4, r0, [sp, #12]
0802c12e  2548      ldr	r0, [pc, #148] ; [0x0802c1c4] = 0x200021f8
0802c130  f4f7e8fa  bl	#-47664 ; -> 0x08020704 ; branch_target=0x08020704
0802c134  0028      cmp	r0, #0
0802c136  36d1      bne	#108 ; -> 0x0802c1a6 ; branch_target=0x0802c1a6
0802c138  2548      ldr	r0, [pc, #148] ; [0x0802c1d0] = 0x43210000 / f32_bits_interpretation=161
0802c13a  0c22      movs	r2, #12
0802c13c  0423      movs	r3, #4
0802c13e  0deb0201  add.w	r1, sp, r2
0802c142  0593      str	r3, [sp, #20]
0802c144  cde90302  strd	r0, r2, [sp, #12]
0802c148  1e48      ldr	r0, [pc, #120] ; [0x0802c1c4] = 0x200021f8
0802c14a  f4f7dbfa  bl	#-47690 ; -> 0x08020704 ; branch_target=0x08020704
0802c14e  38bb      cbnz	r0, #78 ; -> 0x0802c1a0 ; branch_target=0x0802c1a0
0802c150  2048      ldr	r0, [pc, #128] ; [0x0802c1d4] = 0x4fb80000 / f32_bits_interpretation=6174015488
0802c152  1222      movs	r2, #18
0802c154  0423      movs	r3, #4
0802c156  03a9      add	r1, sp, #12
0802c158  cde90302  strd	r0, r2, [sp, #12]
0802c15c  1948      ldr	r0, [pc, #100] ; [0x0802c1c4] = 0x200021f8
0802c15e  0593      str	r3, [sp, #20]
0802c160  f4f7d0fa  bl	#-47712 ; -> 0x08020704 ; branch_target=0x08020704
0802c164  c8b9      cbnz	r0, #50 ; -> 0x0802c19a ; branch_target=0x0802c19a
0802c166  1c4c      ldr	r4, [pc, #112] ; [0x0802c1d8] = 0x4b840000 / f32_bits_interpretation=17301504
0802c168  1822      movs	r2, #24
0802c16a  0423      movs	r3, #4
0802c16c  03a9      add	r1, sp, #12
0802c16e  1548      ldr	r0, [pc, #84] ; [0x0802c1c4] = 0x200021f8
0802c170  0593      str	r3, [sp, #20]
0802c172  cde90342  strd	r4, r2, [sp, #12]
0802c176  f4f7c5fa  bl	#-47734 ; -> 0x08020704 ; branch_target=0x08020704
0802c17a  58b9      cbnz	r0, #22 ; -> 0x0802c194 ; branch_target=0x0802c194
0802c17c  1148      ldr	r0, [pc, #68] ; [0x0802c1c4] = 0x200021f8
0802c17e  f5f7ebf8  bl	#-44586 ; -> 0x08021358 ; branch_target=0x08021358
0802c182  164b      ldr	r3, [pc, #88] ; [0x0802c1dc] = 0x200144d4
0802c184  1b68      ldr	r3, [r3]
0802c186  012b      cmp	r3, #1
0802c188  13d0      beq	#38 ; -> 0x0802c1b2 ; branch_target=0x0802c1b2
0802c18a  0ab0      add	sp, #40
0802c18c  10bd      pop	{r4, pc}
0802c18e  08f0b7ff  bl	#36718 ; -> 0x08035100 ; branch_target=0x08035100
0802c192  b5e7      b	#-150 ; -> 0x0802c100 ; branch_target=0x0802c100
0802c194  08f0b4ff  bl	#36712 ; -> 0x08035100 ; branch_target=0x08035100
0802c198  f0e7      b	#-32 ; -> 0x0802c17c ; branch_target=0x0802c17c
0802c19a  08f0b1ff  bl	#36706 ; -> 0x08035100 ; branch_target=0x08035100
0802c19e  e2e7      b	#-60 ; -> 0x0802c166 ; branch_target=0x0802c166
0802c1a0  08f0aeff  bl	#36700 ; -> 0x08035100 ; branch_target=0x08035100
0802c1a4  d4e7      b	#-88 ; -> 0x0802c150 ; branch_target=0x0802c150
0802c1a6  08f0abff  bl	#36694 ; -> 0x08035100 ; branch_target=0x08035100
0802c1aa  c5e7      b	#-118 ; -> 0x0802c138 ; branch_target=0x0802c138
0802c1ac  08f0a8ff  bl	#36688 ; -> 0x08035100 ; branch_target=0x08035100
0802c1b0  aee7      b	#-164 ; -> 0x0802c110 ; branch_target=0x0802c110
0802c1b2  40f2ff72  movw	r2, #2047
0802c1b6  0021      movs	r1, #0
0802c1b8  0248      ldr	r0, [pc, #8] ; [0x0802c1c4] = 0x200021f8
0802c1ba  f4f7c5ff  bl	#-45174 ; -> 0x08021148 ; branch_target=0x08021148
0802c1be  0ab0      add	sp, #40
0802c1c0  10bd      pop	{r4, pc}
