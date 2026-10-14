; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08022f78  01f00303  and	r3, r1, #3
08022f7c  032b      cmp	r3, #3
08022f7e  22d0      beq	#68 ; -> 0x08022fc6 ; branch_target=0x08022fc6
08022f80  ca07      lsls	r2, r1, #31
08022f82  0bd5      bpl	#22 ; -> 0x08022f9c ; branch_target=0x08022f9c
08022f84  1c4b      ldr	r3, [pc, #112] ; [0x08022ff8] = 0x52002000
08022f86  da68      ldr	r2, [r3, #12]
08022f88  22f03002  bic	r2, r2, #48
08022f8c  da60      str	r2, [r3, #12]
08022f8e  da68      ldr	r2, [r3, #12]
08022f90  0243      orrs	r2, r0
08022f92  da60      str	r2, [r3, #12]
08022f94  da68      ldr	r2, [r3, #12]
08022f96  42f08802  orr	r2, r2, #136
08022f9a  da60      str	r2, [r3, #12]
08022f9c  8b07      lsls	r3, r1, #30
08022f9e  11d5      bpl	#34 ; -> 0x08022fc4 ; branch_target=0x08022fc4
08022fa0  154b      ldr	r3, [pc, #84] ; [0x08022ff8] = 0x52002000
08022fa2  d3f80c21  ldr.w	r2, [r3, #268]
08022fa6  22f03002  bic	r2, r2, #48
08022faa  c3f80c21  str.w	r2, [r3, #268]
08022fae  d3f80c21  ldr.w	r2, [r3, #268]
08022fb2  1043      orrs	r0, r2
08022fb4  c3f80c01  str.w	r0, [r3, #268]
08022fb8  d3f80c21  ldr.w	r2, [r3, #268]
08022fbc  42f08802  orr	r2, r2, #136
08022fc0  c3f80c21  str.w	r2, [r3, #268]
08022fc4  7047      bx	lr
08022fc6  0c4b      ldr	r3, [pc, #48] ; [0x08022ff8] = 0x52002000
08022fc8  da68      ldr	r2, [r3, #12]
08022fca  22f03002  bic	r2, r2, #48
08022fce  da60      str	r2, [r3, #12]
08022fd0  d3f80c21  ldr.w	r2, [r3, #268]
08022fd4  22f03002  bic	r2, r2, #48
08022fd8  c3f80c21  str.w	r2, [r3, #268]
08022fdc  da68      ldr	r2, [r3, #12]
08022fde  0243      orrs	r2, r0
08022fe0  da60      str	r2, [r3, #12]
08022fe2  d3f80c21  ldr.w	r2, [r3, #268]
08022fe6  0243      orrs	r2, r0
08022fe8  c3f80c21  str.w	r2, [r3, #268]
08022fec  9a69      ldr	r2, [r3, #24]
08022fee  42f01002  orr	r2, r2, #16
08022ff2  9a61      str	r2, [r3, #24]
08022ff4  7047      bx	lr
