; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08035b40  10b4      push	{r4}
08035b42  194c      ldr	r4, [pc, #100] ; [0x08035ba8] = 0xe000ed00
08035b44  0022      movs	r2, #0
08035b46  194b      ldr	r3, [pc, #100] ; [0x08035bac] = 0x58024400
08035b48  d4f88810  ldr.w	r1, [r4, #136]
08035b4c  1848      ldr	r0, [pc, #96] ; [0x08035bb0] = 0xeaf6ed7f
08035b4e  41f47001  orr	r1, r1, #15728640
08035b52  c4f88810  str.w	r1, [r4, #136]
08035b56  1968      ldr	r1, [r3]
08035b58  164c      ldr	r4, [pc, #88] ; [0x08035bb4] = 0x5c001000
08035b5a  41f00101  orr	r1, r1, #1
08035b5e  1960      str	r1, [r3]
08035b60  1a61      str	r2, [r3, #16]
08035b62  1968      ldr	r1, [r3]
08035b64  0840      ands	r0, r1
08035b66  1449      ldr	r1, [pc, #80] ; [0x08035bb8] = 0xffff0000
08035b68  1860      str	r0, [r3]
08035b6a  9a61      str	r2, [r3, #24]
08035b6c  da61      str	r2, [r3, #28]
08035b6e  1a62      str	r2, [r3, #32]
08035b70  9a62      str	r2, [r3, #40]
08035b72  da62      str	r2, [r3, #44]
08035b74  1a63      str	r2, [r3, #48]
08035b76  5a63      str	r2, [r3, #52]
08035b78  9a63      str	r2, [r3, #56]
08035b7a  da63      str	r2, [r3, #60]
08035b7c  1a64      str	r2, [r3, #64]
08035b7e  5a64      str	r2, [r3, #68]
08035b80  1868      ldr	r0, [r3]
08035b82  20f48020  bic	r0, r0, #262144
08035b86  1860      str	r0, [r3]
08035b88  1a66      str	r2, [r3, #96]
08035b8a  2368      ldr	r3, [r4]
08035b8c  1940      ands	r1, r3
08035b8e  b1f1005f  cmp.w	r1, #536870912
08035b92  03d2      bhs	#6 ; -> 0x08035b9c ; branch_target=0x08035b9c
08035b94  094b      ldr	r3, [pc, #36] ; [0x08035bbc] = 0x51008000
08035b96  0122      movs	r2, #1
08035b98  c3f80821  str.w	r2, [r3, #264]
08035b9c  024b      ldr	r3, [pc, #8] ; [0x08035ba8] = 0xe000ed00
08035b9e  084a      ldr	r2, [pc, #32] ; [0x08035bc0] = 0x08020000
08035ba0  9a60      str	r2, [r3, #8]
08035ba2  5df8044b  ldr	r4, [sp], #4
08035ba6  7047      bx	lr
