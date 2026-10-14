; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080214bc  0649      ldr	r1, [pc, #24] ; [0x080214d8] = 0xe000ed00
080214be  4ff6ff0c  movw	r12, #63743
080214c2  0002      lsls	r0, r0, #8
080214c4  054b      ldr	r3, [pc, #20] ; [0x080214dc] = 0x05fa0000
080214c6  ca68      ldr	r2, [r1, #12]
080214c8  00f4e060  and	r0, r0, #1792
080214cc  02ea0c02  and.w	r2, r2, r12
080214d0  1043      orrs	r0, r2
080214d2  0343      orrs	r3, r0
080214d4  cb60      str	r3, [r1, #12]
080214d6  7047      bx	lr
