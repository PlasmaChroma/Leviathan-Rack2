; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08035f8c  1c4a      ldr	r2, [pc, #112] ; [0x08036000] = 0x40010000 / f32_bits_interpretation=2.015625
08035f8e  84b0      sub	sp, #16
08035f90  0368      ldr	r3, [r0]
08035f92  9342      cmp	r3, r2
08035f94  16d0      beq	#44 ; -> 0x08035fc4 ; branch_target=0x08035fc4
08035f96  b3f1804f  cmp.w	r3, #1073741824
08035f9a  04d0      beq	#8 ; -> 0x08035fa6 ; branch_target=0x08035fa6
08035f9c  194a      ldr	r2, [pc, #100] ; [0x08036004] = 0x40001800 / f32_bits_interpretation=2.001464844
08035f9e  9342      cmp	r3, r2
08035fa0  1fd0      beq	#62 ; -> 0x08035fe2 ; branch_target=0x08035fe2
08035fa2  04b0      add	sp, #16
08035fa4  7047      bx	lr
08035fa6  184b      ldr	r3, [pc, #96] ; [0x08036008] = 0x58024400
08035fa8  d3f8e820  ldr.w	r2, [r3, #232]
08035fac  42f00102  orr	r2, r2, #1
08035fb0  c3f8e820  str.w	r2, [r3, #232]
08035fb4  d3f8e830  ldr.w	r3, [r3, #232]
08035fb8  03f00103  and	r3, r3, #1
08035fbc  0293      str	r3, [sp, #8]
08035fbe  029b      ldr	r3, [sp, #8]
08035fc0  04b0      add	sp, #16
08035fc2  7047      bx	lr
08035fc4  104b      ldr	r3, [pc, #64] ; [0x08036008] = 0x58024400
08035fc6  d3f8f020  ldr.w	r2, [r3, #240]
08035fca  42f00102  orr	r2, r2, #1
08035fce  c3f8f020  str.w	r2, [r3, #240]
08035fd2  d3f8f030  ldr.w	r3, [r3, #240]
08035fd6  03f00103  and	r3, r3, #1
08035fda  0193      str	r3, [sp, #4]
08035fdc  019b      ldr	r3, [sp, #4]
08035fde  04b0      add	sp, #16
08035fe0  7047      bx	lr
08035fe2  094b      ldr	r3, [pc, #36] ; [0x08036008] = 0x58024400
08035fe4  d3f8e820  ldr.w	r2, [r3, #232]
08035fe8  42f04002  orr	r2, r2, #64
08035fec  c3f8e820  str.w	r2, [r3, #232]
08035ff0  d3f8e830  ldr.w	r3, [r3, #232]
08035ff4  03f04003  and	r3, r3, #64
08035ff8  0393      str	r3, [sp, #12]
08035ffa  039b      ldr	r3, [sp, #12]
08035ffc  04b0      add	sp, #16
08035ffe  7047      bx	lr
