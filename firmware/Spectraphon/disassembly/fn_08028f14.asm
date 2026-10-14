; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028f14  114b      ldr	r3, [pc, #68] ; [0x08028f5c] = 0x20000014
08028f16  0146      mov	r1, r0
08028f18  114a      ldr	r2, [pc, #68] ; [0x08028f60] = 0xfffee0c0
08028f1a  30b4      push	{r4, r5}
08028f1c  114c      ldr	r4, [pc, #68] ; [0x08028f64] = 0x10624dd3
08028f1e  0025      movs	r5, #0
08028f20  1b68      ldr	r3, [r3]
08028f22  8560      str	r5, [r0, #8]
08028f24  a4fb0343  umull	r4, r3, r4, r3
08028f28  c468      ldr	r4, [r0, #12]
08028f2a  41f28830  movw	r0, #5000
08028f2e  2240      ands	r2, r4
08028f30  5b0a      lsrs	r3, r3, #9
08028f32  42f48052  orr	r2, r2, #4096
08028f36  00fb03f3  mul	r3, r0, r3
08028f3a  ca60      str	r2, [r1, #12]
08028f3c  02e0      b	#4 ; -> 0x08028f44 ; branch_target=0x08028f44
08028f3e  4a6b      ldr	r2, [r1, #52]
08028f40  1206      lsls	r2, r2, #24
08028f42  05d4      bmi	#10 ; -> 0x08028f50 ; branch_target=0x08028f50
08028f44  013b      subs	r3, #1
08028f46  fad2      bhs	#-12 ; -> 0x08028f3e ; branch_target=0x08028f3e
08028f48  4ff00040  mov.w	r0, #2147483648
08028f4c  30bc      pop	{r4, r5}
08028f4e  7047      bx	lr
08028f50  054b      ldr	r3, [pc, #20] ; [0x08028f68] = 0x002000c5
08028f52  0020      movs	r0, #0
08028f54  8b63      str	r3, [r1, #56]
08028f56  30bc      pop	{r4, r5}
08028f58  7047      bx	lr
