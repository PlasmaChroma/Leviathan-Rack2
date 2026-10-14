; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08027edc  036a      ldr	r3, [r0, #32]
08027ede  1d4a      ldr	r2, [pc, #116] ; [0x08027f54] = 0xfeff8cff
08027ee0  23f01003  bic	r3, r3, #16
08027ee4  70b4      push	{r4, r5, r6}
08027ee6  0362      str	r3, [r0, #32]
08027ee8  036a      ldr	r3, [r0, #32]
08027eea  4468      ldr	r4, [r0, #4]
08027eec  8569      ldr	r5, [r0, #24]
08027eee  23f02003  bic	r3, r3, #32
08027ef2  2a40      ands	r2, r5
08027ef4  0d68      ldr	r5, [r1]
08027ef6  42ea0522  orr.w	r2, r2, r5, lsl #8
08027efa  8d68      ldr	r5, [r1, #8]
08027efc  43ea0513  orr.w	r3, r3, r5, lsl #4
08027f00  154d      ldr	r5, [pc, #84] ; [0x08027f58] = 0x40010000 / f32_bits_interpretation=2.015625
08027f02  a842      cmp	r0, r5
08027f04  0fd0      beq	#30 ; -> 0x08027f26 ; branch_target=0x08027f26
08027f06  05f58065  add.w	r5, r5, #1024
08027f0a  a842      cmp	r0, r5
08027f0c  0bd0      beq	#22 ; -> 0x08027f26 ; branch_target=0x08027f26
08027f0e  134e      ldr	r6, [pc, #76] ; [0x08027f5c] = 0x40014000 / f32_bits_interpretation=2.01953125
08027f10  05f58045  add.w	r5, r5, #16384
08027f14  a842      cmp	r0, r5
08027f16  18bf      it	ne
08027f18  b042      cmpne	r0, r6
08027f1a  0bd0      beq	#22 ; -> 0x08027f34 ; branch_target=0x08027f34
08027f1c  05f58065  add.w	r5, r5, #1024
08027f20  a842      cmp	r0, r5
08027f22  0fd1      bne	#30 ; -> 0x08027f44 ; branch_target=0x08027f44
08027f24  06e0      b	#12 ; -> 0x08027f34 ; branch_target=0x08027f34
08027f26  23f08003  bic	r3, r3, #128
08027f2a  cd68      ldr	r5, [r1, #12]
08027f2c  43ea0513  orr.w	r3, r3, r5, lsl #4
08027f30  23f04003  bic	r3, r3, #64
08027f34  24f44064  bic	r4, r4, #3072
08027f38  d1e90565  ldrd	r6, r5, [r1, #20]
08027f3c  46ea050c  orr.w	r12, r6, r5
08027f40  44ea8c04  orr.w	r4, r4, r12, lsl #2
08027f44  4460      str	r4, [r0, #4]
08027f46  8261      str	r2, [r0, #24]
08027f48  4a68      ldr	r2, [r1, #4]
08027f4a  70bc      pop	{r4, r5, r6}
08027f4c  8263      str	r2, [r0, #56]
08027f4e  0362      str	r3, [r0, #32]
08027f50  7047      bx	lr
