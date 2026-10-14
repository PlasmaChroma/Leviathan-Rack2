; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08032c34  d0b5      push	{r4, r6, r7, lr}
08032c36  b6b0      sub	sp, #216
08032c38  0168      ldr	r1, [r0]
08032c3a  cde90233  strd	r3, r3, [sp, #8]
08032c3e  cde90433  strd	r3, r3, [sp, #16]
08032c42  0693      str	r3, [sp, #24]
08032c44  09b1      cbz	r1, #2 ; -> 0x08032c4a ; branch_target=0x08032c4a
08032c46  36b0      add	sp, #216
08032c48  d0bd      pop	{r4, r6, r7, pc}
08032c4a  0123      movs	r3, #1
08032c4c  b822      movs	r2, #184
08032c4e  0360      str	r3, [r0]
08032c50  08a8      add	r0, sp, #32
08032c52  03f016fc  bl	#14380 ; -> 0x08036482 ; branch_target=0x08036482
08032c56  4ff08073  mov.w	r3, #16777216
08032c5a  07a8      add	r0, sp, #28
08032c5c  0793      str	r3, [sp, #28]
08032c5e  f1f703fb  bl	#-59898 ; -> 0x08024268 ; branch_target=0x08024268
08032c62  0028      cmp	r0, #0
08032c64  4cd1      bne	#152 ; -> 0x08032d00 ; branch_target=0x08032d00
08032c66  294b      ldr	r3, [pc, #164] ; [0x08032d0c] = 0x58024400
08032c68  4ff63f00  movw	r0, #63551
08032c6c  0221      movs	r1, #2
08032c6e  0c24      movs	r4, #12
08032c70  d3f8d420  ldr.w	r2, [r3, #212]
08032c74  0026      movs	r6, #0
08032c76  0327      movs	r7, #3
08032c78  42f48052  orr	r2, r2, #4096
08032c7c  c3f8d420  str.w	r2, [r3, #212]
08032c80  d3f8d430  ldr.w	r3, [r3, #212]
08032c84  0694      str	r4, [sp, #24]
08032c86  03f48053  and	r3, r3, #4096
08032c8a  cde90201  strd	r0, r1, [sp, #8]
08032c8e  0193      str	r3, [sp, #4]
08032c90  02a9      add	r1, sp, #8
08032c92  1f48      ldr	r0, [pc, #124] ; [0x08032d10] = 0x58021400
08032c94  019b      ldr	r3, [sp, #4]
08032c96  cde90467  strd	r6, r7, [sp, #16]
08032c9a  f0f77dfa  bl	#-64262 ; -> 0x08023198 ; branch_target=0x08023198
08032c9e  0d22      movs	r2, #13
08032ca0  0223      movs	r3, #2
08032ca2  02a9      add	r1, sp, #8
08032ca4  1b48      ldr	r0, [pc, #108] ; [0x08032d14] = 0x58020800
08032ca6  0694      str	r4, [sp, #24]
08032ca8  cde90223  strd	r2, r3, [sp, #8]
08032cac  cde90467  strd	r6, r7, [sp, #16]
08032cb0  f0f772fa  bl	#-64284 ; -> 0x08023198 ; branch_target=0x08023198
08032cb4  48f23712  movw	r2, #33079
08032cb8  0223      movs	r3, #2
08032cba  02a9      add	r1, sp, #8
08032cbc  1648      ldr	r0, [pc, #88] ; [0x08032d18] = 0x58021800
08032cbe  0694      str	r4, [sp, #24]
08032cc0  cde90223  strd	r2, r3, [sp, #8]
08032cc4  cde90467  strd	r6, r7, [sp, #16]
08032cc8  f0f766fa  bl	#-64308 ; -> 0x08023198 ; branch_target=0x08023198
08032ccc  4ff68372  movw	r2, #65411
08032cd0  0223      movs	r3, #2
08032cd2  02a9      add	r1, sp, #8
08032cd4  1148      ldr	r0, [pc, #68] ; [0x08032d1c] = 0x58021000
08032cd6  0694      str	r4, [sp, #24]
08032cd8  cde90223  strd	r2, r3, [sp, #8]
08032cdc  cde90467  strd	r6, r7, [sp, #16]
08032ce0  f0f75afa  bl	#-64332 ; -> 0x08023198 ; branch_target=0x08023198
08032ce4  4cf20372  movw	r2, #50947
08032ce8  0223      movs	r3, #2
08032cea  02a9      add	r1, sp, #8
08032cec  0c48      ldr	r0, [pc, #48] ; [0x08032d20] = 0x58020c00
08032cee  0694      str	r4, [sp, #24]
08032cf0  cde90467  strd	r6, r7, [sp, #16]
08032cf4  cde90223  strd	r2, r3, [sp, #8]
08032cf8  f0f74efa  bl	#-64356 ; -> 0x08023198 ; branch_target=0x08023198
08032cfc  36b0      add	sp, #216
08032cfe  d0bd      pop	{r4, r6, r7, pc}
08032d00  02f0fef9  bl	#9212 ; -> 0x08035100 ; branch_target=0x08035100
08032d04  afe7      b	#-162 ; -> 0x08032c66 ; branch_target=0x08032c66
