; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08021574  431e      subs	r3, r0, #1
08021576  b3f1807f  cmp.w	r3, #16777216
0802157a  01d3      blo	#2 ; -> 0x08021580 ; branch_target=0x08021580
0802157c  0120      movs	r0, #1
0802157e  7047      bx	lr
08021580  4ff0e022  mov.w	r2, #3758153728
08021584  0020      movs	r0, #0
08021586  0549      ldr	r1, [pc, #20] ; [0x0802159c] = 0xe000ed00
08021588  4ff0f00c  mov.w	r12, #240
0802158c  5361      str	r3, [r2, #20]
0802158e  0723      movs	r3, #7
08021590  81f823c0  strb.w	r12, [r1, #35]
08021594  9061      str	r0, [r2, #24]
08021596  1361      str	r3, [r2, #16]
08021598  7047      bx	lr
