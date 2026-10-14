; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08036370  00b5      push	{lr}
08036372  0023      movs	r3, #0
08036374  91b0      sub	sp, #68
08036376  2c48      ldr	r0, [pc, #176] ; [0x08036428] = 0x20014cdc
08036378  8022      movs	r2, #128
0803637a  2c49      ldr	r1, [pc, #176] ; [0x0803642c] = 0x40000800 / f32_bits_interpretation=2.000488281
0803637c  0493      str	r3, [sp, #16]
0803637e  0193      str	r3, [sp, #4]
08036380  0893      str	r3, [sp, #32]
08036382  0361      str	r3, [r0, #16]
08036384  8261      str	r2, [r0, #24]
08036386  0793      str	r3, [sp, #28]
08036388  c0e90013  strd	r1, r3, [r0]
0803638c  c0e90232  strd	r3, r2, [r0, #8]
08036390  cde90533  strd	r3, r3, [sp, #20]
08036394  cde90233  strd	r3, r3, [sp, #8]
08036398  cde90933  strd	r3, r3, [sp, #36]
0803639c  cde90b33  strd	r3, r3, [sp, #44]
080363a0  cde90d33  strd	r3, r3, [sp, #52]
080363a4  f1f78efa  bl	#-60132 ; -> 0x080278c4 ; branch_target=0x080278c4
080363a8  30bb      cbnz	r0, #76 ; -> 0x080363f8 ; branch_target=0x080363f8
080363aa  4ff48053  mov.w	r3, #4096
080363ae  04a9      add	r1, sp, #16
080363b0  1d48      ldr	r0, [pc, #116] ; [0x08036428] = 0x20014cdc
080363b2  0493      str	r3, [sp, #16]
080363b4  f1f7c8fc  bl	#-58992 ; -> 0x08027d48 ; branch_target=0x08027d48
080363b8  78bb      cbnz	r0, #94 ; -> 0x0803641a ; branch_target=0x0803641a
080363ba  1b48      ldr	r0, [pc, #108] ; [0x08036428] = 0x20014cdc
080363bc  f1f768fb  bl	#-59696 ; -> 0x08027a90 ; branch_target=0x08027a90
080363c0  40bb      cbnz	r0, #80 ; -> 0x08036414 ; branch_target=0x08036414
080363c2  0023      movs	r3, #0
080363c4  01a9      add	r1, sp, #4
080363c6  1848      ldr	r0, [pc, #96] ; [0x08036428] = 0x20014cdc
080363c8  0193      str	r3, [sp, #4]
080363ca  0393      str	r3, [sp, #12]
080363cc  f1f7fefe  bl	#-57860 ; -> 0x080281cc ; branch_target=0x080281cc
080363d0  e8b9      cbnz	r0, #58 ; -> 0x0803640e ; branch_target=0x0803640e
080363d2  0023      movs	r3, #0
080363d4  0822      movs	r2, #8
080363d6  08a9      add	r1, sp, #32
080363d8  1348      ldr	r0, [pc, #76] ; [0x08036428] = 0x20014cdc
080363da  0a93      str	r3, [sp, #40]
080363dc  0c93      str	r3, [sp, #48]
080363de  9fed107b  vldr	d7, [pc, #64] ; [0x08036420] = 0x00000060 / f64_bits_interpretation=4.7430302000759668e-322
080363e2  8ded087b  vstr	d7, [sp, #32]
080363e6  f1f7bbfd  bl	#-58506 ; -> 0x08027f60 ; branch_target=0x08027f60
080363ea  40b9      cbnz	r0, #16 ; -> 0x080363fe ; branch_target=0x080363fe
080363ec  0e48      ldr	r0, [pc, #56] ; [0x08036428] = 0x20014cdc
080363ee  fff73bfe  bl	#-906 ; -> 0x08036068 ; branch_target=0x08036068
080363f2  11b0      add	sp, #68
080363f4  5df804fb  ldr	pc, [sp], #4
080363f8  fef782fe  bl	#-4860 ; -> 0x08035100 ; branch_target=0x08035100
080363fc  d5e7      b	#-86 ; -> 0x080363aa ; branch_target=0x080363aa
080363fe  fef77ffe  bl	#-4866 ; -> 0x08035100 ; branch_target=0x08035100
08036402  0948      ldr	r0, [pc, #36] ; [0x08036428] = 0x20014cdc
08036404  fff730fe  bl	#-928 ; -> 0x08036068 ; branch_target=0x08036068
08036408  11b0      add	sp, #68
0803640a  5df804fb  ldr	pc, [sp], #4
0803640e  fef777fe  bl	#-4882 ; -> 0x08035100 ; branch_target=0x08035100
08036412  dee7      b	#-68 ; -> 0x080363d2 ; branch_target=0x080363d2
08036414  fef774fe  bl	#-4888 ; -> 0x08035100 ; branch_target=0x08035100
08036418  d3e7      b	#-90 ; -> 0x080363c2 ; branch_target=0x080363c2
0803641a  fef771fe  bl	#-4894 ; -> 0x08035100 ; branch_target=0x08035100
0803641e  cce7      b	#-104 ; -> 0x080363ba ; branch_target=0x080363ba
