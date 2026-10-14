; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080214e0  1a4b      ldr	r3, [pc, #104] ; [0x0802154c] = 0xe000ed00
080214e2  00b5      push	{lr}
080214e4  db68      ldr	r3, [r3, #12]
080214e6  c3f30223  ubfx	r3, r3, #8, #3
080214ea  c3f1070e  rsb.w	lr, r3, #7
080214ee  03f1040c  add.w	r12, r3, #4
080214f2  bef1040f  cmp.w	lr, #4
080214f6  28bf      it	hs
080214f8  4ff0040e  movhs.w	lr, #4
080214fc  bcf1060f  cmp.w	r12, #6
08021500  1ad9      bls	#52 ; -> 0x08021538 ; branch_target=0x08021538
08021502  a3f1030c  sub.w	r12, r3, #3
08021506  4ff0ff33  mov.w	r3, #4294967295
0802150a  03fa0cf3  lsl.w	r3, r3, r12
0802150e  22ea0302  bic.w	r2, r2, r3
08021512  4ff0ff33  mov.w	r3, #4294967295
08021516  0028      cmp	r0, #0
08021518  03fa0ef3  lsl.w	r3, r3, lr
0802151c  21ea0303  bic.w	r3, r1, r3
08021520  03fa0cf3  lsl.w	r3, r3, r12
08021524  43ea0203  orr.w	r3, r3, r2
08021528  4fea0313  lsl.w	r3, r3, #4
0802152c  dbb2      uxtb	r3, r3
0802152e  06db      blt	#12 ; -> 0x0802153e ; branch_target=0x0802153e
08021530  074a      ldr	r2, [pc, #28] ; [0x08021550] = 0xe000e400
08021532  1354      strb	r3, [r2, r0]
08021534  5df804fb  ldr	pc, [sp], #4
08021538  0022      movs	r2, #0
0802153a  9446      mov	r12, r2
0802153c  e9e7      b	#-46 ; -> 0x08021512 ; branch_target=0x08021512
0802153e  00f00f00  and	r0, r0, #15
08021542  044a      ldr	r2, [pc, #16] ; [0x08021554] = 0xe000ed14
08021544  1354      strb	r3, [r2, r0]
08021546  5df804fb  ldr	pc, [sp], #4
