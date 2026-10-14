; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802c1e0  10b5      push	{r4, lr}
0802c1e2  4948      ldr	r0, [pc, #292] ; [0x0802c308] = 0x20002194
0802c1e4  4ff48022  mov.w	r2, #262144
0802c1e8  484c      ldr	r4, [pc, #288] ; [0x0802c30c] = 0x40022100 / f32_bits_interpretation=2.03326416
0802c1ea  0023      movs	r3, #0
0802c1ec  0121      movs	r1, #1
0802c1ee  88b0      sub	sp, #32
0802c1f0  0361      str	r3, [r0, #16]
0802c1f2  0193      str	r3, [sp, #4]
0802c1f4  0377      strb	r3, [r0, #28]
0802c1f6  4363      str	r3, [r0, #52]
0802c1f8  80f83830  strb.w	r3, [r0, #56]
0802c1fc  c0e90042  strd	r4, r2, [r0]
0802c200  4ff48072  mov.w	r2, #256
0802c204  0624      movs	r4, #6
0802c206  c0e90231  strd	r3, r1, [r0, #8]
0802c20a  8282      strh	r2, [r0, #20]
0802c20c  4ff49c61  mov.w	r1, #1248
0802c210  0322      movs	r2, #3
0802c212  8461      str	r4, [r0, #24]
0802c214  c0e90913  strd	r1, r3, [r0, #36]
0802c218  c0e90b23  strd	r2, r3, [r0, #44]
0802c21c  cde90233  strd	r3, r3, [sp, #8]
0802c220  cde90433  strd	r3, r3, [sp, #16]
0802c224  cde90633  strd	r3, r3, [sp, #24]
0802c228  f4f77efe  bl	#-45828 ; -> 0x08020f28 ; branch_target=0x08020f28
0802c22c  0028      cmp	r0, #0
0802c22e  4ed1      bne	#156 ; -> 0x0802c2ce ; branch_target=0x0802c2ce
0802c230  3749      ldr	r1, [pc, #220] ; [0x0802c310] = 0x3ac04000 / f32_bits_interpretation=0.001466751099
0802c232  0622      movs	r2, #6
0802c234  0520      movs	r0, #5
0802c236  0023      movs	r3, #0
0802c238  0191      str	r1, [sp, #4]
0802c23a  40f2ff71  movw	r1, #2047
0802c23e  0292      str	r2, [sp, #8]
0802c240  0422      movs	r2, #4
0802c242  0693      str	r3, [sp, #24]
0802c244  0592      str	r2, [sp, #20]
0802c246  8df81d30  strb.w	r3, [sp, #29]
0802c24a  cde90301  strd	r0, r1, [sp, #12]
0802c24e  0deb0201  add.w	r1, sp, r2
0802c252  2d48      ldr	r0, [pc, #180] ; [0x0802c308] = 0x20002194
0802c254  f4f756fa  bl	#-47956 ; -> 0x08020704 ; branch_target=0x08020704
0802c258  0028      cmp	r0, #0
0802c25a  4ad1      bne	#148 ; -> 0x0802c2f2 ; branch_target=0x0802c2f2
0802c25c  2d4a      ldr	r2, [pc, #180] ; [0x0802c314] = 0x3ef08000 / f32_bits_interpretation=0.4697265625
0802c25e  0c23      movs	r3, #12
0802c260  01a9      add	r1, sp, #4
0802c262  2948      ldr	r0, [pc, #164] ; [0x0802c308] = 0x20002194
0802c264  cde90123  strd	r2, r3, [sp, #4]
0802c268  f4f74cfa  bl	#-47976 ; -> 0x08020704 ; branch_target=0x08020704
0802c26c  0028      cmp	r0, #0
0802c26e  3dd1      bne	#122 ; -> 0x0802c2ec ; branch_target=0x0802c2ec
0802c270  294a      ldr	r2, [pc, #164] ; [0x0802c318] = 0x0c900008
0802c272  1223      movs	r3, #18
0802c274  01a9      add	r1, sp, #4
0802c276  2448      ldr	r0, [pc, #144] ; [0x0802c308] = 0x20002194
0802c278  cde90123  strd	r2, r3, [sp, #4]
0802c27c  f4f742fa  bl	#-47996 ; -> 0x08020704 ; branch_target=0x08020704
0802c280  88bb      cbnz	r0, #98 ; -> 0x0802c2e6 ; branch_target=0x0802c2e6
0802c282  264a      ldr	r2, [pc, #152] ; [0x0802c31c] = 0x1d500080
0802c284  1823      movs	r3, #24
0802c286  01a9      add	r1, sp, #4
0802c288  1f48      ldr	r0, [pc, #124] ; [0x0802c308] = 0x20002194
0802c28a  cde90123  strd	r2, r3, [sp, #4]
0802c28e  f4f739fa  bl	#-48014 ; -> 0x08020704 ; branch_target=0x08020704
0802c292  28bb      cbnz	r0, #74 ; -> 0x0802c2e0 ; branch_target=0x0802c2e0
0802c294  224a      ldr	r2, [pc, #136] ; [0x0802c320] = 0x10c00010
0802c296  4ff48073  mov.w	r3, #256
0802c29a  01a9      add	r1, sp, #4
0802c29c  1a48      ldr	r0, [pc, #104] ; [0x0802c308] = 0x20002194
0802c29e  cde90123  strd	r2, r3, [sp, #4]
0802c2a2  f4f72ffa  bl	#-48034 ; -> 0x08020704 ; branch_target=0x08020704
0802c2a6  c0b9      cbnz	r0, #48 ; -> 0x0802c2da ; branch_target=0x0802c2da
0802c2a8  1e4a      ldr	r2, [pc, #120] ; [0x0802c324] = 0x21800100
0802c2aa  4ff48373  mov.w	r3, #262
0802c2ae  01a9      add	r1, sp, #4
0802c2b0  1548      ldr	r0, [pc, #84] ; [0x0802c308] = 0x20002194
0802c2b2  cde90123  strd	r2, r3, [sp, #4]
0802c2b6  f4f725fa  bl	#-48054 ; -> 0x08020704 ; branch_target=0x08020704
0802c2ba  58b9      cbnz	r0, #22 ; -> 0x0802c2d4 ; branch_target=0x0802c2d4
0802c2bc  1248      ldr	r0, [pc, #72] ; [0x0802c308] = 0x20002194
0802c2be  f5f74bf8  bl	#-44906 ; -> 0x08021358 ; branch_target=0x08021358
0802c2c2  194b      ldr	r3, [pc, #100] ; [0x0802c328] = 0x200144d4
0802c2c4  1b68      ldr	r3, [r3]
0802c2c6  012b      cmp	r3, #1
0802c2c8  16d0      beq	#44 ; -> 0x0802c2f8 ; branch_target=0x0802c2f8
0802c2ca  08b0      add	sp, #32
0802c2cc  10bd      pop	{r4, pc}
0802c2ce  08f017ff  bl	#36398 ; -> 0x08035100 ; branch_target=0x08035100
0802c2d2  ade7      b	#-166 ; -> 0x0802c230 ; branch_target=0x0802c230
0802c2d4  08f014ff  bl	#36392 ; -> 0x08035100 ; branch_target=0x08035100
0802c2d8  f0e7      b	#-32 ; -> 0x0802c2bc ; branch_target=0x0802c2bc
0802c2da  08f011ff  bl	#36386 ; -> 0x08035100 ; branch_target=0x08035100
0802c2de  e3e7      b	#-58 ; -> 0x0802c2a8 ; branch_target=0x0802c2a8
0802c2e0  08f00eff  bl	#36380 ; -> 0x08035100 ; branch_target=0x08035100
0802c2e4  d6e7      b	#-84 ; -> 0x0802c294 ; branch_target=0x0802c294
0802c2e6  08f00bff  bl	#36374 ; -> 0x08035100 ; branch_target=0x08035100
0802c2ea  cae7      b	#-108 ; -> 0x0802c282 ; branch_target=0x0802c282
0802c2ec  08f008ff  bl	#36368 ; -> 0x08035100 ; branch_target=0x08035100
0802c2f0  bee7      b	#-132 ; -> 0x0802c270 ; branch_target=0x0802c270
0802c2f2  08f005ff  bl	#36362 ; -> 0x08035100 ; branch_target=0x08035100
0802c2f6  b1e7      b	#-158 ; -> 0x0802c25c ; branch_target=0x0802c25c
0802c2f8  40f2ff72  movw	r2, #2047
0802c2fc  0021      movs	r1, #0
0802c2fe  0248      ldr	r0, [pc, #8] ; [0x0802c308] = 0x20002194
0802c300  f4f722ff  bl	#-45500 ; -> 0x08021148 ; branch_target=0x08021148
0802c304  08b0      add	sp, #32
0802c306  10bd      pop	{r4, pc}
