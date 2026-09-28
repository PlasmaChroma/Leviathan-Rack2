; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08029d00  f8b5      push	{r3, r4, r5, r6, r7, lr}
08029d02  c378      ldrb	r3, [r0, #3]
08029d04  0446      mov	r4, r0
08029d06  5bb9      cbnz	r3, #22 ; -> 0x08029d20 ; branch_target=0x08029d20
08029d08  2378      ldrb	r3, [r4]
08029d0a  032b      cmp	r3, #3
08029d0c  2ad0      beq	#84 ; -> 0x08029d64 ; branch_target=0x08029d64
08029d0e  0022      movs	r2, #0
08029d10  6078      ldrb	r0, [r4, #1]
08029d12  1146      mov	r1, r2
08029d14  fff70cfe  bl	#-1000 ; -> 0x08029930 ; branch_target=0x08029930
08029d18  0038      subs	r0, #0
08029d1a  18bf      it	ne
08029d1c  0120      movne	r0, #1
08029d1e  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08029d20  c56a      ldr	r5, [r0, #44]
08029d22  00f13007  add.w	r7, r0, #48
08029d26  0123      movs	r3, #1
08029d28  4078      ldrb	r0, [r0, #1]
08029d2a  2a46      mov	r2, r5
08029d2c  3946      mov	r1, r7
08029d2e  fff7f1fd  bl	#-1054 ; -> 0x08029914 ; branch_target=0x08029914
08029d32  a8b9      cbnz	r0, #42 ; -> 0x08029d60 ; branch_target=0x08029d60
08029d34  226a      ldr	r2, [r4, #32]
08029d36  a369      ldr	r3, [r4, #24]
08029d38  aa1a      subs	r2, r5, r2
08029d3a  e070      strb	r0, [r4, #3]
08029d3c  9a42      cmp	r2, r3
08029d3e  e3d2      bhs	#-58 ; -> 0x08029d08 ; branch_target=0x08029d08
08029d40  a678      ldrb	r6, [r4, #2]
08029d42  012e      cmp	r6, #1
08029d44  01d8      bhi	#2 ; -> 0x08029d4a ; branch_target=0x08029d4a
08029d46  dfe7      b	#-66 ; -> 0x08029d08 ; branch_target=0x08029d08
08029d48  a369      ldr	r3, [r4, #24]
08029d4a  1d44      add	r5, r3
08029d4c  013e      subs	r6, #1
08029d4e  0123      movs	r3, #1
08029d50  3946      mov	r1, r7
08029d52  2a46      mov	r2, r5
08029d54  6078      ldrb	r0, [r4, #1]
08029d56  fff7ddfd  bl	#-1094 ; -> 0x08029914 ; branch_target=0x08029914
08029d5a  012e      cmp	r6, #1
08029d5c  f4d1      bne	#-24 ; -> 0x08029d48 ; branch_target=0x08029d48
08029d5e  d3e7      b	#-90 ; -> 0x08029d08 ; branch_target=0x08029d08
08029d60  0120      movs	r0, #1
08029d62  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08029d64  2579      ldrb	r5, [r4, #4]
08029d66  012d      cmp	r5, #1
08029d68  d1d1      bne	#-94 ; -> 0x08029d0e ; branch_target=0x08029d0e
08029d6a  04f13006  add.w	r6, r4, #48
08029d6e  4ff40072  mov.w	r2, #512
08029d72  0021      movs	r1, #0
08029d74  3046      mov	r0, r6
08029d76  0cf084fb  bl	#50952 ; -> 0x08036482 ; branch_target=0x08036482
08029d7a  0f48      ldr	r0, [pc, #60] ; [0x08029db8] = 0x41615252 / f32_bits_interpretation=14.08259773
08029d7c  4af65523  movw	r3, #43605
08029d80  2063      str	r0, [r4, #48]
08029d82  00f1ff50  add.w	r0, r0, #534773760
08029d86  a4f82e32  strh.w	r3, [r4, #558]
08029d8a  2b46      mov	r3, r5
08029d8c  00f50050  add.w	r0, r0, #8192
08029d90  d4e90312  ldrd	r1, r2, [r4, #12]
08029d94  2030      adds	r0, #32
08029d96  c4f81822  str.w	r2, [r4, #536]
08029d9a  e269      ldr	r2, [r4, #28]
08029d9c  c4f81c12  str.w	r1, [r4, #540]
08029da0  3146      mov	r1, r6
08029da2  0132      adds	r2, #1
08029da4  c4f81402  str.w	r0, [r4, #532]
08029da8  6078      ldrb	r0, [r4, #1]
08029daa  e262      str	r2, [r4, #44]
08029dac  fff7b2fd  bl	#-1180 ; -> 0x08029914 ; branch_target=0x08029914
08029db0  0023      movs	r3, #0
08029db2  2371      strb	r3, [r4, #4]
08029db4  abe7      b	#-170 ; -> 0x08029d0e ; branch_target=0x08029d0e
