; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08021358  00b5      push	{lr}
0802135a  134a      ldr	r2, [pc, #76] ; [0x080213a8] = 0x40022000 / f32_bits_interpretation=2.033203125
0802135c  87b0      sub	sp, #28
0802135e  0368      ldr	r3, [r0]
08021360  9342      cmp	r3, r2
08021362  1ed0      beq	#60 ; -> 0x080213a2 ; branch_target=0x080213a2
08021364  02f58072  add.w	r2, r2, #256
08021368  9342      cmp	r3, r2
0802136a  14bf      ite	ne
0802136c  1022      movne	r2, #16
0802136e  0822      moveq	r2, #8
08021370  0e4b      ldr	r3, [pc, #56] ; [0x080213ac] = 0x07fc7b01
08021372  6946      mov	r1, sp
08021374  1344      add	r3, r2
08021376  9b00      lsls	r3, r3, #2
08021378  13ed017b  vldr	d7, [r3, #-4]
0802137c  043b      subs	r3, #4
0802137e  8ded007b  vstr	d7, [sp]
08021382  93ed027b  vldr	d7, [r3, #8]
08021386  d3e90423  ldrd	r2, r3, [r3, #16]
0802138a  8ded027b  vstr	d7, [sp, #8]
0802138e  cde90423  strd	r2, r3, [sp, #16]
08021392  fff72dff  bl	#-422 ; -> 0x080211f0 ; branch_target=0x080211f0
08021396  0038      subs	r0, #0
08021398  18bf      it	ne
0802139a  0120      movne	r0, #1
0802139c  07b0      add	sp, #28
0802139e  5df804fb  ldr	pc, [sp], #4
080213a2  0022      movs	r2, #0
080213a4  e4e7      b	#-56 ; -> 0x08021370 ; branch_target=0x08021370
