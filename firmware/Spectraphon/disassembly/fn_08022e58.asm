; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08022e58  0a4b      ldr	r3, [pc, #40] ; [0x08022e84] = 0x52002000
08022e5a  da68      ldr	r2, [r3, #12]
08022e5c  42f00102  orr	r2, r2, #1
08022e60  da60      str	r2, [r3, #12]
08022e62  da68      ldr	r2, [r3, #12]
08022e64  d207      lsls	r2, r2, #31
08022e66  0bd5      bpl	#22 ; -> 0x08022e80 ; branch_target=0x08022e80
08022e68  d3f80c21  ldr.w	r2, [r3, #268]
08022e6c  42f00102  orr	r2, r2, #1
08022e70  c3f80c21  str.w	r2, [r3, #268]
08022e74  d3f80c01  ldr.w	r0, [r3, #268]
08022e78  c043      mvns	r0, r0
08022e7a  00f00100  and	r0, r0, #1
08022e7e  7047      bx	lr
08022e80  0120      movs	r0, #1
08022e82  7047      bx	lr
