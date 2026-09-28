; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08026920  0346      mov	r3, r0
08026922  0020      movs	r0, #0
08026924  9a6b      ldr	r2, [r3, #56]
08026926  0a60      str	r2, [r1]
08026928  da6b      ldr	r2, [r3, #60]
0802692a  4a60      str	r2, [r1, #4]
0802692c  1a6c      ldr	r2, [r3, #64]
0802692e  8a60      str	r2, [r1, #8]
08026930  5a6c      ldr	r2, [r3, #68]
08026932  ca60      str	r2, [r1, #12]
08026934  9a6c      ldr	r2, [r3, #72]
08026936  0a61      str	r2, [r1, #16]
08026938  da6c      ldr	r2, [r3, #76]
0802693a  4a61      str	r2, [r1, #20]
0802693c  1a6d      ldr	r2, [r3, #80]
0802693e  8a61      str	r2, [r1, #24]
08026940  5b6d      ldr	r3, [r3, #84]
08026942  cb61      str	r3, [r1, #28]
08026944  7047      bx	lr
0803274c  0146      mov	r1, r0
0803274e  0148      ldr	r0, [pc, #4] ; [0x08032754] = 0x20014a48
08032750  f4f7e6b8  b.w	#-48692 ; -> 0x08026920 ; branch_target=0x08026920
