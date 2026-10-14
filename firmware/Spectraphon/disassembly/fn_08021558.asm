; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08021558  0028      cmp	r0, #0
0802155a  07db      blt	#14 ; -> 0x0802156c ; branch_target=0x0802156c
0802155c  0123      movs	r3, #1
0802155e  00f01f01  and	r1, r0, #31
08021562  034a      ldr	r2, [pc, #12] ; [0x08021570] = 0xe000e100
08021564  4009      lsrs	r0, r0, #5
08021566  8b40      lsls	r3, r1
08021568  42f82030  str.w	r3, [r2, r0, lsl #2]
0802156c  7047      bx	lr
