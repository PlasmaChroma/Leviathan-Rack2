; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08020b78  70b5      push	{r4, r5, r6, lr}
08020b7a  234a      ldr	r2, [pc, #140] ; [0x08020c08] = 0x8000003f
08020b7c  0446      mov	r4, r0
08020b7e  9968      ldr	r1, [r3, #8]
08020b80  1142      tst	r1, r2
08020b82  2fd1      bne	#94 ; -> 0x08020be4 ; branch_target=0x08020be4
08020b84  9968      ldr	r1, [r3, #8]
08020b86  214a      ldr	r2, [pc, #132] ; [0x08020c0c] = 0x7fffffc0
08020b88  0a40      ands	r2, r1
08020b8a  42f00102  orr	r2, r2, #1
08020b8e  9a60      str	r2, [r3, #8]
08020b90  fff704fc  bl	#-2040 ; -> 0x0802039c ; branch_target=0x0802039c
08020b94  2368      ldr	r3, [r4]
08020b96  1e4a      ldr	r2, [pc, #120] ; [0x08020c10] = 0x40022000 / f32_bits_interpretation=2.033203125
08020b98  0546      mov	r5, r0
08020b9a  9342      cmp	r3, r2
08020b9c  2cd0      beq	#88 ; -> 0x08020bf8 ; branch_target=0x08020bf8
08020b9e  02f58072  add.w	r2, r2, #256
08020ba2  9342      cmp	r3, r2
08020ba4  28d0      beq	#80 ; -> 0x08020bf8 ; branch_target=0x08020bf8
08020ba6  02f1c052  add.w	r2, r2, #402653184
08020baa  02f58442  add.w	r2, r2, #16896
08020bae  9268      ldr	r2, [r2, #8]
08020bb0  1a68      ldr	r2, [r3]
08020bb2  d207      lsls	r2, r2, #31
08020bb4  14d4      bmi	#40 ; -> 0x08020be0 ; branch_target=0x08020be0
08020bb6  154e      ldr	r6, [pc, #84] ; [0x08020c0c] = 0x7fffffc0
08020bb8  9a68      ldr	r2, [r3, #8]
08020bba  d007      lsls	r0, r2, #31
08020bbc  04d4      bmi	#8 ; -> 0x08020bc8 ; branch_target=0x08020bc8
08020bbe  9a68      ldr	r2, [r3, #8]
08020bc0  3240      ands	r2, r6
08020bc2  42f00102  orr	r2, r2, #1
08020bc6  9a60      str	r2, [r3, #8]
08020bc8  fff7e8fb  bl	#-2096 ; -> 0x0802039c ; branch_target=0x0802039c
08020bcc  431b      subs	r3, r0, r5
08020bce  022b      cmp	r3, #2
08020bd0  2368      ldr	r3, [r4]
08020bd2  02d9      bls	#4 ; -> 0x08020bda ; branch_target=0x08020bda
08020bd4  1a68      ldr	r2, [r3]
08020bd6  d107      lsls	r1, r2, #31
08020bd8  04d5      bpl	#8 ; -> 0x08020be4 ; branch_target=0x08020be4
08020bda  1a68      ldr	r2, [r3]
08020bdc  d207      lsls	r2, r2, #31
08020bde  ebd5      bpl	#-42 ; -> 0x08020bb8 ; branch_target=0x08020bb8
08020be0  0020      movs	r0, #0
08020be2  70bd      pop	{r4, r5, r6, pc}
08020be4  636d      ldr	r3, [r4, #84]
08020be6  0120      movs	r0, #1
08020be8  43f01003  orr	r3, r3, #16
08020bec  6365      str	r3, [r4, #84]
08020bee  a36d      ldr	r3, [r4, #88]
08020bf0  43f00103  orr	r3, r3, #1
08020bf4  a365      str	r3, [r4, #88]
08020bf6  70bd      pop	{r4, r5, r6, pc}
08020bf8  064a      ldr	r2, [pc, #24] ; [0x08020c14] = 0x40022300 / f32_bits_interpretation=2.03338623
08020bfa  9268      ldr	r2, [r2, #8]
08020bfc  d606      lsls	r6, r2, #27
08020bfe  d7d0      beq	#-82 ; -> 0x08020bb0 ; branch_target=0x08020bb0
08020c00  054a      ldr	r2, [pc, #20] ; [0x08020c18] = 0x40022100 / f32_bits_interpretation=2.03326416
08020c02  9342      cmp	r3, r2
08020c04  d4d1      bne	#-88 ; -> 0x08020bb0 ; branch_target=0x08020bb0
08020c06  ebe7      b	#-42 ; -> 0x08020be0 ; branch_target=0x08020be0
