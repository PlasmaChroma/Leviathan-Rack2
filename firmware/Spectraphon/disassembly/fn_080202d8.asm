; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080202d8  0f4b      ldr	r3, [pc, #60] ; [0x08020318] = 0x20000000
080202da  1b78      ldrb	r3, [r3]
080202dc  0bb9      cbnz	r3, #2 ; -> 0x080202e2 ; branch_target=0x080202e2
080202de  0120      movs	r0, #1
080202e0  7047      bx	lr
080202e2  10b5      push	{r4, lr}
080202e4  0446      mov	r4, r0
080202e6  4ff47a70  mov.w	r0, #1000
080202ea  0c4a      ldr	r2, [pc, #48] ; [0x0802031c] = 0x20000014
080202ec  b0fbf3f3  udiv	r3, r0, r3
080202f0  1068      ldr	r0, [r2]
080202f2  b0fbf3f0  udiv	r0, r0, r3
080202f6  01f03df9  bl	#4730 ; -> 0x08021574 ; branch_target=0x08021574
080202fa  08b9      cbnz	r0, #2 ; -> 0x08020300 ; branch_target=0x08020300
080202fc  0f2c      cmp	r4, #15
080202fe  01d9      bls	#2 ; -> 0x08020304 ; branch_target=0x08020304
08020300  0120      movs	r0, #1
08020302  10bd      pop	{r4, pc}
08020304  0022      movs	r2, #0
08020306  2146      mov	r1, r4
08020308  4ff0ff30  mov.w	r0, #4294967295
0802030c  01f0e8f8  bl	#4560 ; -> 0x080214e0 ; branch_target=0x080214e0
08020310  034b      ldr	r3, [pc, #12] ; [0x08020320] = 0x20000004
08020312  0020      movs	r0, #0
08020314  1c60      str	r4, [r3]
08020316  10bd      pop	{r4, pc}
