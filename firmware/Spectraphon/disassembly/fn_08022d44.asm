; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022d44  f8b5      push	{r3, r4, r5, r6, r7, lr}
08022d46  2f4e      ldr	r6, [pc, #188] ; [0x08022e04] = 0x20002050
08022d48  337d      ldrb	r3, [r6, #20]
08022d4a  012b      cmp	r3, #1
08022d4c  51d0      beq	#162 ; -> 0x08022df2 ; branch_target=0x08022df2
08022d4e  01f17843  add.w	r3, r1, #4160749568
08022d52  0127      movs	r7, #1
08022d54  0c46      mov	r4, r1
08022d56  1546      mov	r5, r2
08022d58  b3f5801f  cmp.w	r3, #1048576
08022d5c  3775      strb	r7, [r6, #20]
08022d5e  39d3      blo	#114 ; -> 0x08022dd4 ; branch_target=0x08022dd4
08022d60  a1f10163  sub.w	r3, r1, #135266304
08022d64  b3f5801f  cmp.w	r3, #1048576
08022d68  41d2      bhs	#130 ; -> 0x08022dee ; branch_target=0x08022dee
08022d6a  0023      movs	r3, #0
08022d6c  0220      movs	r0, #2
08022d6e  b361      str	r3, [r6, #24]
08022d70  fff792ff  bl	#-220 ; -> 0x08022c98 ; branch_target=0x08022c98
08022d74  58bb      cbnz	r0, #86 ; -> 0x08022dce ; branch_target=0x08022dce
08022d76  244a      ldr	r2, [pc, #144] ; [0x08022e08] = 0x52002000
08022d78  0227      movs	r7, #2
08022d7a  d2f80c31  ldr.w	r3, [r2, #268]
08022d7e  43f00203  orr	r3, r3, #2
08022d82  c2f80c31  str.w	r3, [r2, #268]
08022d86  bff36f8f  isb	sy
08022d8a  bff34f8f  dsb	sy
08022d8e  2b68      ldr	r3, [r5]
08022d90  2360      str	r3, [r4]
08022d92  6b68      ldr	r3, [r5, #4]
08022d94  6360      str	r3, [r4, #4]
08022d96  ab68      ldr	r3, [r5, #8]
08022d98  a360      str	r3, [r4, #8]
08022d9a  eb68      ldr	r3, [r5, #12]
08022d9c  e360      str	r3, [r4, #12]
08022d9e  2b69      ldr	r3, [r5, #16]
08022da0  2361      str	r3, [r4, #16]
08022da2  6b69      ldr	r3, [r5, #20]
08022da4  6361      str	r3, [r4, #20]
08022da6  ab69      ldr	r3, [r5, #24]
08022da8  a361      str	r3, [r4, #24]
08022daa  eb69      ldr	r3, [r5, #28]
08022dac  e361      str	r3, [r4, #28]
08022dae  bff36f8f  isb	sy
08022db2  bff34f8f  dsb	sy
08022db6  3846      mov	r0, r7
08022db8  fff76eff  bl	#-292 ; -> 0x08022c98 ; branch_target=0x08022c98
08022dbc  012f      cmp	r7, #1
08022dbe  124a      ldr	r2, [pc, #72] ; [0x08022e08] = 0x52002000
08022dc0  19d0      beq	#50 ; -> 0x08022df6 ; branch_target=0x08022df6
08022dc2  d2f80c31  ldr.w	r3, [r2, #268]
08022dc6  23f00203  bic	r3, r3, #2
08022dca  c2f80c31  str.w	r3, [r2, #268]
08022dce  0023      movs	r3, #0
08022dd0  3375      strb	r3, [r6, #20]
08022dd2  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022dd4  0023      movs	r3, #0
08022dd6  3846      mov	r0, r7
08022dd8  b361      str	r3, [r6, #24]
08022dda  fff75dff  bl	#-326 ; -> 0x08022c98 ; branch_target=0x08022c98
08022dde  0028      cmp	r0, #0
08022de0  f5d1      bne	#-22 ; -> 0x08022dce ; branch_target=0x08022dce
08022de2  094a      ldr	r2, [pc, #36] ; [0x08022e08] = 0x52002000
08022de4  d368      ldr	r3, [r2, #12]
08022de6  43f00203  orr	r3, r3, #2
08022dea  d360      str	r3, [r2, #12]
08022dec  cbe7      b	#-106 ; -> 0x08022d86 ; branch_target=0x08022d86
08022dee  3846      mov	r0, r7
08022df0  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022df2  0220      movs	r0, #2
08022df4  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022df6  d368      ldr	r3, [r2, #12]
08022df8  23f00203  bic	r3, r3, #2
08022dfc  d360      str	r3, [r2, #12]
08022dfe  0023      movs	r3, #0
08022e00  3375      strb	r3, [r6, #20]
08022e02  e6e7      b	#-52 ; -> 0x08022dd2 ; branch_target=0x08022dd2
