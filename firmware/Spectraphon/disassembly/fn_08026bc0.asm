; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08026bc0  18b3      cbz	r0, #70 ; -> 0x08026c0a ; branch_target=0x08026c0a
08026bc2  38b5      push	{r3, r4, r5, lr}
08026bc4  90f82c30  ldrb.w	r3, [r0, #44]
08026bc8  0446      mov	r4, r0
08026bca  0d46      mov	r5, r1
08026bcc  03f0ff02  and	r2, r3, #255
08026bd0  b3b1      cbz	r3, #44 ; -> 0x08026c00 ; branch_target=0x08026c00
08026bd2  2146      mov	r1, r4
08026bd4  0223      movs	r3, #2
08026bd6  84f82c30  strb.w	r3, [r4, #44]
08026bda  51f8040b  ldr	r0, [r1], #4
08026bde  01f09dfb  bl	#5946 ; -> 0x0802831c ; branch_target=0x0802831c
08026be2  6268      ldr	r2, [r4, #4]
08026be4  2946      mov	r1, r5
08026be6  2068      ldr	r0, [r4]
08026be8  01f0cefb  bl	#6044 ; -> 0x08028388 ; branch_target=0x08028388
08026bec  084a      ldr	r2, [pc, #32] ; [0x08026c10] = 0x52004000
08026bee  0121      movs	r1, #1
08026bf0  0020      movs	r0, #0
08026bf2  1368      ldr	r3, [r2]
08026bf4  43f00043  orr	r3, r3, #2147483648
08026bf8  1360      str	r3, [r2]
08026bfa  84f82c10  strb.w	r1, [r4, #44]
08026bfe  38bd      pop	{r3, r4, r5, pc}
08026c00  80f82d20  strb.w	r2, [r0, #45]
08026c04  0cf014f8  bl	#49192 ; -> 0x08032c30 ; branch_target=0x08032c30
08026c08  e3e7      b	#-58 ; -> 0x08026bd2 ; branch_target=0x08026bd2
08026c0a  0120      movs	r0, #1
08026c0c  7047      bx	lr
