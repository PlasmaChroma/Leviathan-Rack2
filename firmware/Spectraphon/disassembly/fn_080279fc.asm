; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080279fc  90f83d30  ldrb.w	r3, [r0, #61]
08027a00  012b      cmp	r3, #1
08027a02  36d1      bne	#108 ; -> 0x08027a72 ; branch_target=0x08027a72
08027a04  1c4b      ldr	r3, [pc, #112] ; [0x08027a78] = 0x40010000 / f32_bits_interpretation=2.015625
08027a06  4ff0020c  mov.w	r12, #2
08027a0a  1c49      ldr	r1, [pc, #112] ; [0x08027a7c] = 0x40000800 / f32_bits_interpretation=2.000488281
08027a0c  10b4      push	{r4}
08027a0e  0268      ldr	r2, [r0]
08027a10  1b4c      ldr	r4, [pc, #108] ; [0x08027a80] = 0x40000400 / f32_bits_interpretation=2.000244141
08027a12  b2f1804f  cmp.w	r2, #1073741824
08027a16  18bf      it	ne
08027a18  9a42      cmpne	r2, r3
08027a1a  80f83dc0  strb.w	r12, [r0, #61]
08027a1e  1948      ldr	r0, [pc, #100] ; [0x08027a84] = 0x40000c00 / f32_bits_interpretation=2.000732422
08027a20  0cbf      ite	eq
08027a22  0123      moveq	r3, #1
08027a24  0023      movne	r3, #0
08027a26  a242      cmp	r2, r4
08027a28  08bf      it	eq
08027a2a  43f00103  orreq	r3, r3, #1
08027a2e  8a42      cmp	r2, r1
08027a30  08bf      it	eq
08027a32  43f00103  orreq	r3, r3, #1
08027a36  01f57c41  add.w	r1, r1, #64512
08027a3a  8242      cmp	r2, r0
08027a3c  08bf      it	eq
08027a3e  43f00103  orreq	r3, r3, #1
08027a42  8a42      cmp	r2, r1
08027a44  08bf      it	eq
08027a46  43f00103  orreq	r3, r3, #1
08027a4a  13b9      cbnz	r3, #4 ; -> 0x08027a52 ; branch_target=0x08027a52
08027a4c  0e4b      ldr	r3, [pc, #56] ; [0x08027a88] = 0x40001800 / f32_bits_interpretation=2.001464844
08027a4e  9a42      cmp	r2, r3
08027a50  07d1      bne	#14 ; -> 0x08027a62 ; branch_target=0x08027a62
08027a52  9168      ldr	r1, [r2, #8]
08027a54  0d4b      ldr	r3, [pc, #52] ; [0x08027a8c] = 0x00010007
08027a56  0b40      ands	r3, r1
08027a58  062b      cmp	r3, #6
08027a5a  06d0      beq	#12 ; -> 0x08027a6a ; branch_target=0x08027a6a
08027a5c  b3f5803f  cmp.w	r3, #65536
08027a60  03d0      beq	#6 ; -> 0x08027a6a ; branch_target=0x08027a6a
08027a62  1368      ldr	r3, [r2]
08027a64  43f00103  orr	r3, r3, #1
08027a68  1360      str	r3, [r2]
08027a6a  0020      movs	r0, #0
08027a6c  5df8044b  ldr	r4, [sp], #4
08027a70  7047      bx	lr
08027a72  0120      movs	r0, #1
08027a74  7047      bx	lr
