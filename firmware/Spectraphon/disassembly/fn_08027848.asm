; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08027848  036a      ldr	r3, [r0, #32]
0802784a  23f48073  bic	r3, r3, #256
0802784e  70b4      push	{r4, r5, r6}
08027850  0362      str	r3, [r0, #32]
08027852  036a      ldr	r3, [r0, #32]
08027854  4468      ldr	r4, [r0, #4]
08027856  c269      ldr	r2, [r0, #28]
08027858  23f40073  bic	r3, r3, #512
0802785c  0d68      ldr	r5, [r1]
0802785e  22f07302  bic	r2, r2, #115
08027862  2a43      orrs	r2, r5
08027864  8d68      ldr	r5, [r1, #8]
08027866  43ea0523  orr.w	r3, r3, r5, lsl #8
0802786a  144d      ldr	r5, [pc, #80] ; [0x080278bc] = 0x40010000 / f32_bits_interpretation=2.015625
0802786c  a842      cmp	r0, r5
0802786e  0fd0      beq	#30 ; -> 0x08027890 ; branch_target=0x08027890
08027870  05f58065  add.w	r5, r5, #1024
08027874  a842      cmp	r0, r5
08027876  0bd0      beq	#22 ; -> 0x08027890 ; branch_target=0x08027890
08027878  114e      ldr	r6, [pc, #68] ; [0x080278c0] = 0x40014000 / f32_bits_interpretation=2.01953125
0802787a  05f58045  add.w	r5, r5, #16384
0802787e  a842      cmp	r0, r5
08027880  18bf      it	ne
08027882  b042      cmpne	r0, r6
08027884  0bd0      beq	#22 ; -> 0x0802789e ; branch_target=0x0802789e
08027886  05f58065  add.w	r5, r5, #1024
0802788a  a842      cmp	r0, r5
0802788c  0fd1      bne	#30 ; -> 0x080278ae ; branch_target=0x080278ae
0802788e  06e0      b	#12 ; -> 0x0802789e ; branch_target=0x0802789e
08027890  23f40063  bic	r3, r3, #2048
08027894  cd68      ldr	r5, [r1, #12]
08027896  43ea0523  orr.w	r3, r3, r5, lsl #8
0802789a  23f48063  bic	r3, r3, #1024
0802789e  24f44054  bic	r4, r4, #12288
080278a2  d1e90565  ldrd	r6, r5, [r1, #20]
080278a6  46ea050c  orr.w	r12, r6, r5
080278aa  44ea0c14  orr.w	r4, r4, r12, lsl #4
080278ae  4460      str	r4, [r0, #4]
080278b0  c261      str	r2, [r0, #28]
080278b2  4a68      ldr	r2, [r1, #4]
080278b4  70bc      pop	{r4, r5, r6}
080278b6  c263      str	r2, [r0, #60]
080278b8  0362      str	r3, [r0, #32]
080278ba  7047      bx	lr
