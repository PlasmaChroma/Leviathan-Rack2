; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
08025dc4  08b5      push	{r3, lr}
08025dc6  c369      ldr	r3, [r0, #28]
08025dc8  806b      ldr	r0, [r0, #56]
08025dca  b3f5807f  cmp.w	r3, #256
08025dce  1bd0      beq	#54 ; -> 0x08025e08 ; branch_target=0x08025e08
08025dd0  0268      ldr	r2, [r0]
08025dd2  0023      movs	r3, #0
08025dd4  a0f87e30  strh.w	r3, [r0, #126]
08025dd8  1368      ldr	r3, [r2]
08025dda  23f40033  bic	r3, r3, #131072
08025dde  1360      str	r3, [r2]
08025de0  436c      ldr	r3, [r0, #68]
08025de2  082b      cmp	r3, #8
08025de4  4368      ldr	r3, [r0, #4]
08025de6  12d0      beq	#36 ; -> 0x08025e0e ; branch_target=0x08025e0e
08025de8  6ff06102  mvn	r2, #97
08025dec  6ff0050c  mvn	r12, #5
08025df0  0168      ldr	r1, [r0]
08025df2  023b      subs	r3, #2
08025df4  012b      cmp	r3, #1
08025df6  88bf      it	hi
08025df8  6246      movhi	r2, r12
08025dfa  0b69      ldr	r3, [r1, #16]
08025dfc  4ff0010c  mov.w	r12, #1
08025e00  1340      ands	r3, r2
08025e02  0b61      str	r3, [r1, #16]
08025e04  80f891c0  strb.w	r12, [r0, #145]
08025e08  0cf032fc  bl	#51300 ; -> 0x08032670 ; branch_target=0x08032670
08025e0c  08bd      pop	{r3, pc}
08025e0e  23f00202  bic	r2, r3, #2
08025e12  012a      cmp	r2, #1
08025e14  e8d1      bne	#-48 ; -> 0x08025de8 ; branch_target=0x08025de8
08025e16  6ff07102  mvn	r2, #113
08025e1a  6ff0150c  mvn	r12, #21
08025e1e  e7e7      b	#-50 ; -> 0x08025df0 ; branch_target=0x08025df0
