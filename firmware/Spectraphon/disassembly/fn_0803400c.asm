; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0803400c  10b5      push	{r4, lr}
0803400e  84b0      sub	sp, #16
08034010  4ff06054  mov.w	r4, #939524096
08034014  edf7c4fa  bl	#-76408 ; -> 0x080215a0 ; branch_target=0x080215a0
08034018  0123      movs	r3, #1
0803401a  4ff04051  mov.w	r1, #805306368
0803401e  0d4a      ldr	r2, [pc, #52] ; [0x08034054] = 0x03010011
08034020  6846      mov	r0, sp
08034022  adf80030  strh.w	r3, [sp]
08034026  0191      str	r1, [sp, #4]
08034028  cde90223  strd	r2, r3, [sp, #8]
0803402c  edf7d6fa  bl	#-76372 ; -> 0x080215dc ; branch_target=0x080215dc
08034030  40f20111  movw	r1, #257
08034034  084a      ldr	r2, [pc, #32] ; [0x08034058] = 0x0301000f
08034036  6846      mov	r0, sp
08034038  084b      ldr	r3, [pc, #32] ; [0x0803405c] = 0x01010100
0803403a  adf80010  strh.w	r1, [sp]
0803403e  0194      str	r4, [sp, #4]
08034040  cde90223  strd	r2, r3, [sp, #8]
08034044  edf7cafa  bl	#-76396 ; -> 0x080215dc ; branch_target=0x080215dc
08034048  0420      movs	r0, #4
0803404a  edf7b7fa  bl	#-76434 ; -> 0x080215bc ; branch_target=0x080215bc
0803404e  04b0      add	sp, #16
08034050  10bd      pop	{r4, pc}
