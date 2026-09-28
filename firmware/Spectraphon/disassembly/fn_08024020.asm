; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08024020  184a      ldr	r2, [pc, #96] ; [0x08024084] = 0x58024400
08024022  38b5      push	{r3, r4, r5, lr}
08024024  1369      ldr	r3, [r2, #16]
08024026  03f03803  and	r3, r3, #56
0802402a  102b      cmp	r3, #16
0802402c  19d0      beq	#50 ; -> 0x08024062 ; branch_target=0x08024062
0802402e  182b      cmp	r3, #24
08024030  22d0      beq	#68 ; -> 0x08024078 ; branch_target=0x08024078
08024032  c3b1      cbz	r3, #48 ; -> 0x08024066 ; branch_target=0x08024066
08024034  144b      ldr	r3, [pc, #80] ; [0x08024088] = 0x003d0900
08024036  1349      ldr	r1, [pc, #76] ; [0x08024084] = 0x58024400
08024038  1448      ldr	r0, [pc, #80] ; [0x0802408c] = 0x08049aac
0802403a  8a69      ldr	r2, [r1, #24]
0802403c  8969      ldr	r1, [r1, #24]
0802403e  c2f30322  ubfx	r2, r2, #8, #4
08024042  134c      ldr	r4, [pc, #76] ; [0x08024090] = 0x20000010
08024044  01f00f01  and	r1, r1, #15
08024048  124d      ldr	r5, [pc, #72] ; [0x08024094] = 0x20000014
0802404a  825c      ldrb	r2, [r0, r2]
0802404c  405c      ldrb	r0, [r0, r1]
0802404e  02f01f02  and	r2, r2, #31
08024052  00f01f00  and	r0, r0, #31
08024056  d340      lsrs	r3, r2
08024058  23fa00f0  lsr.w	r0, r3, r0
0802405c  2b60      str	r3, [r5]
0802405e  2060      str	r0, [r4]
08024060  38bd      pop	{r3, r4, r5, pc}
08024062  0d4b      ldr	r3, [pc, #52] ; [0x08024098] = 0x017d7840
08024064  e7e7      b	#-50 ; -> 0x08024036 ; branch_target=0x08024036
08024066  1368      ldr	r3, [r2]
08024068  9b06      lsls	r3, r3, #26
0802406a  09d5      bpl	#18 ; -> 0x08024080 ; branch_target=0x08024080
0802406c  1268      ldr	r2, [r2]
0802406e  0b4b      ldr	r3, [pc, #44] ; [0x0802409c] = 0x03d09000
08024070  c2f3c102  ubfx	r2, r2, #3, #2
08024074  d340      lsrs	r3, r2
08024076  dee7      b	#-68 ; -> 0x08024036 ; branch_target=0x08024036
08024078  fff7befa  bl	#-2692 ; -> 0x080235f8 ; branch_target=0x080235f8
0802407c  0346      mov	r3, r0
0802407e  dae7      b	#-76 ; -> 0x08024036 ; branch_target=0x08024036
08024080  064b      ldr	r3, [pc, #24] ; [0x0802409c] = 0x03d09000
08024082  d8e7      b	#-80 ; -> 0x08024036 ; branch_target=0x08024036
