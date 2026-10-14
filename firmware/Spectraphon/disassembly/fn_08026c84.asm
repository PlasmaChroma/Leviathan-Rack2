; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08026c84  0268      ldr	r2, [r0]
08026c86  10b4      push	{r4}
08026c88  5369      ldr	r3, [r2, #20]
08026c8a  9169      ldr	r1, [r2, #24]
08026c8c  41f00801  orr	r1, r1, #8
08026c90  9161      str	r1, [r2, #24]
08026c92  0168      ldr	r1, [r0]
08026c94  8a69      ldr	r2, [r1, #24]
08026c96  42f01002  orr	r2, r2, #16
08026c9a  8a61      str	r2, [r1, #24]
08026c9c  0168      ldr	r1, [r0]
08026c9e  0a68      ldr	r2, [r1]
08026ca0  22f00102  bic	r2, r2, #1
08026ca4  0a60      str	r2, [r1]
08026ca6  0168      ldr	r1, [r0]
08026ca8  274a      ldr	r2, [pc, #156] ; [0x08026d48] = 0xfffffc90
08026caa  0c69      ldr	r4, [r1, #16]
08026cac  2240      ands	r2, r4
08026cae  0a61      str	r2, [r1, #16]
08026cb0  0168      ldr	r1, [r0]
08026cb2  8a68      ldr	r2, [r1, #8]
08026cb4  22f44042  bic	r2, r2, #49152
08026cb8  8a60      str	r2, [r1, #8]
08026cba  90f88120  ldrb.w	r2, [r0, #129]
08026cbe  042a      cmp	r2, #4
08026cc0  01d0      beq	#2 ; -> 0x08026cc6 ; branch_target=0x08026cc6
08026cc2  9c06      lsls	r4, r3, #26
08026cc4  33d4      bmi	#102 ; -> 0x08026d2e ; branch_target=0x08026d2e
08026cc6  90f88120  ldrb.w	r2, [r0, #129]
08026cca  032a      cmp	r2, #3
08026ccc  01d0      beq	#2 ; -> 0x08026cd2 ; branch_target=0x08026cd2
08026cce  5906      lsls	r1, r3, #25
08026cd0  21d4      bmi	#66 ; -> 0x08026d16 ; branch_target=0x08026d16
08026cd2  9a05      lsls	r2, r3, #22
08026cd4  0ad5      bpl	#20 ; -> 0x08026cec ; branch_target=0x08026cec
08026cd6  d0f88420  ldr.w	r2, [r0, #132]
08026cda  0168      ldr	r1, [r0]
08026cdc  42f00102  orr	r2, r2, #1
08026ce0  c0f88420  str.w	r2, [r0, #132]
08026ce4  8a69      ldr	r2, [r1, #24]
08026ce6  42f40072  orr	r2, r2, #512
08026cea  8a61      str	r2, [r1, #24]
08026cec  db05      lsls	r3, r3, #23
08026cee  0ad5      bpl	#20 ; -> 0x08026d06 ; branch_target=0x08026d06
08026cf0  d0f88430  ldr.w	r3, [r0, #132]
08026cf4  0268      ldr	r2, [r0]
08026cf6  43f00803  orr	r3, r3, #8
08026cfa  c0f88430  str.w	r3, [r0, #132]
08026cfe  9369      ldr	r3, [r2, #24]
08026d00  43f48073  orr	r3, r3, #256
08026d04  9361      str	r3, [r2, #24]
08026d06  0023      movs	r3, #0
08026d08  5df8044b  ldr	r4, [sp], #4
08026d0c  a0f86230  strh.w	r3, [r0, #98]
08026d10  a0f86a30  strh.w	r3, [r0, #106]
08026d14  7047      bx	lr
08026d16  d0f88420  ldr.w	r2, [r0, #132]
08026d1a  0168      ldr	r1, [r0]
08026d1c  42f00402  orr	r2, r2, #4
08026d20  c0f88420  str.w	r2, [r0, #132]
08026d24  8a69      ldr	r2, [r1, #24]
08026d26  42f04002  orr	r2, r2, #64
08026d2a  8a61      str	r2, [r1, #24]
08026d2c  d1e7      b	#-94 ; -> 0x08026cd2 ; branch_target=0x08026cd2
08026d2e  d0f88420  ldr.w	r2, [r0, #132]
08026d32  0168      ldr	r1, [r0]
08026d34  42f08002  orr	r2, r2, #128
08026d38  c0f88420  str.w	r2, [r0, #132]
08026d3c  8a69      ldr	r2, [r1, #24]
08026d3e  42f02002  orr	r2, r2, #32
08026d42  8a61      str	r2, [r1, #24]
08026d44  bfe7      b	#-130 ; -> 0x08026cc6 ; branch_target=0x08026cc6
