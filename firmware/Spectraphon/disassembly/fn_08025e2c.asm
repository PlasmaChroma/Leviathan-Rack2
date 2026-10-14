; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: flash_pointer_candidate,prologue_heuristic
08025e2c  08b5      push	{r3, lr}
08025e2e  c369      ldr	r3, [r0, #28]
08025e30  806b      ldr	r0, [r0, #56]
08025e32  b3f5807f  cmp.w	r3, #256
08025e36  1bd0      beq	#54 ; -> 0x08025e70 ; branch_target=0x08025e70
08025e38  0268      ldr	r2, [r0]
08025e3a  0021      movs	r1, #0
08025e3c  1368      ldr	r3, [r2]
08025e3e  23f40033  bic	r3, r3, #131072
08025e42  1360      str	r3, [r2]
08025e44  436c      ldr	r3, [r0, #68]
08025e46  a0f87e10  strh.w	r1, [r0, #126]
08025e4a  082b      cmp	r3, #8
08025e4c  4368      ldr	r3, [r0, #4]
08025e4e  12d0      beq	#36 ; -> 0x08025e76 ; branch_target=0x08025e76
08025e50  6ff06102  mvn	r2, #97
08025e54  6ff0050c  mvn	r12, #5
08025e58  0168      ldr	r1, [r0]
08025e5a  023b      subs	r3, #2
08025e5c  012b      cmp	r3, #1
08025e5e  88bf      it	hi
08025e60  6246      movhi	r2, r12
08025e62  0b69      ldr	r3, [r1, #16]
08025e64  4ff0010c  mov.w	r12, #1
08025e68  1340      ands	r3, r2
08025e6a  0b61      str	r3, [r1, #16]
08025e6c  80f891c0  strb.w	r12, [r0, #145]
08025e70  0cf002fc  bl	#51204 ; -> 0x08032678 ; branch_target=0x08032678
08025e74  08bd      pop	{r3, pc}
08025e76  23f00202  bic	r2, r3, #2
08025e7a  012a      cmp	r2, #1
08025e7c  e8d1      bne	#-48 ; -> 0x08025e50 ; branch_target=0x08025e50
08025e7e  6ff07102  mvn	r2, #113
08025e82  6ff0150c  mvn	r12, #21
08025e86  e7e7      b	#-50 ; -> 0x08025e58 ; branch_target=0x08025e58
