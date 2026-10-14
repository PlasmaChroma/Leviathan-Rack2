; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08021148  f0b5      push	{r4, r5, r6, r7, lr}
0802114a  0023      movs	r3, #0
0802114c  83b0      sub	sp, #12
0802114e  0193      str	r3, [sp, #4]
08021150  90f85030  ldrb.w	r3, [r0, #80]
08021154  012b      cmp	r3, #1
08021156  41d0      beq	#130 ; -> 0x080211dc ; branch_target=0x080211dc
08021158  0123      movs	r3, #1
0802115a  0446      mov	r4, r0
0802115c  0e46      mov	r6, r1
0802115e  1546      mov	r5, r2
08021160  80f85030  strb.w	r3, [r0, #80]
08021164  fff7eafd  bl	#-1068 ; -> 0x08020d3c ; branch_target=0x08020d3c
08021168  f0b9      cbnz	r0, #60 ; -> 0x080211a8 ; branch_target=0x080211a8
0802116a  676d      ldr	r7, [r4, #84]
0802116c  05f08042  and	r2, r5, #1073741824
08021170  1c4b      ldr	r3, [pc, #112] ; [0x080211e4] = 0xffffeefd
08021172  06f48031  and	r1, r6, #65536
08021176  2568      ldr	r5, [r4]
08021178  3b40      ands	r3, r7
0802117a  43f00203  orr	r3, r3, #2
0802117e  6365      str	r3, [r4, #84]
08021180  194b      ldr	r3, [pc, #100] ; [0x080211e8] = 0x3ffeffc0 / f32_bits_interpretation=1.992179871
08021182  ae68      ldr	r6, [r5, #8]
08021184  3340      ands	r3, r6
08021186  1343      orrs	r3, r2
08021188  0b43      orrs	r3, r1
0802118a  43f00043  orr	r3, r3, #2147483648
0802118e  ab60      str	r3, [r5, #8]
08021190  2268      ldr	r2, [r4]
08021192  9368      ldr	r3, [r2, #8]
08021194  1549      ldr	r1, [pc, #84] ; [0x080211ec] = 0x25c3f800
08021196  002b      cmp	r3, #0
08021198  0fdb      blt	#30 ; -> 0x080211ba ; branch_target=0x080211ba
0802119a  636d      ldr	r3, [r4, #84]
0802119c  23f00303  bic	r3, r3, #3
080211a0  43f00103  orr	r3, r3, #1
080211a4  6365      str	r3, [r4, #84]
080211a6  03e0      b	#6 ; -> 0x080211b0 ; branch_target=0x080211b0
080211a8  636d      ldr	r3, [r4, #84]
080211aa  43f01003  orr	r3, r3, #16
080211ae  6365      str	r3, [r4, #84]
080211b0  0023      movs	r3, #0
080211b2  84f85030  strb.w	r3, [r4, #80]
080211b6  03b0      add	sp, #12
080211b8  f0bd      pop	{r4, r5, r6, r7, pc}
080211ba  019b      ldr	r3, [sp, #4]
080211bc  0133      adds	r3, #1
080211be  0193      str	r3, [sp, #4]
080211c0  019b      ldr	r3, [sp, #4]
080211c2  8b42      cmp	r3, r1
080211c4  e5d3      blo	#-54 ; -> 0x08021192 ; branch_target=0x08021192
080211c6  636d      ldr	r3, [r4, #84]
080211c8  0022      movs	r2, #0
080211ca  0120      movs	r0, #1
080211cc  23f01203  bic	r3, r3, #18
080211d0  84f85020  strb.w	r2, [r4, #80]
080211d4  43f01003  orr	r3, r3, #16
080211d8  6365      str	r3, [r4, #84]
080211da  ece7      b	#-40 ; -> 0x080211b6 ; branch_target=0x080211b6
080211dc  0220      movs	r0, #2
080211de  03b0      add	sp, #12
080211e0  f0bd      pop	{r4, r5, r6, r7, pc}
