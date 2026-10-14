; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028f6c  10b4      push	{r4}
08028f6e  0146      mov	r1, r0
08028f70  4ff4d574  mov.w	r4, #426
08028f74  1a4a      ldr	r2, [pc, #104] ; [0x08028fe0] = 0x20000014
08028f76  1b4b      ldr	r3, [pc, #108] ; [0x08028fe4] = 0x10624dd3
08028f78  8c60      str	r4, [r1, #8]
08028f7a  1268      ldr	r2, [r2]
08028f7c  cc68      ldr	r4, [r1, #12]
08028f7e  1a48      ldr	r0, [pc, #104] ; [0x08028fe8] = 0xfffee0c0
08028f80  a3fb0232  umull	r3, r2, r3, r2
08028f84  41f20813  movw	r3, #4360
08028f88  2040      ands	r0, r4
08028f8a  520a      lsrs	r2, r2, #9
08028f8c  0343      orrs	r3, r0
08028f8e  41f28830  movw	r0, #5000
08028f92  00fb02f2  mul	r2, r0, r2
08028f96  cb60      str	r3, [r1, #12]
08028f98  4ab1      cbz	r2, #18 ; -> 0x08028fae ; branch_target=0x08028fae
08028f9a  531e      subs	r3, r2, #1
08028f9c  4a6b      ldr	r2, [r1, #52]
08028f9e  013b      subs	r3, #1
08028fa0  12f0450f  tst.w	r2, #69
08028fa4  01d0      beq	#2 ; -> 0x08028faa ; branch_target=0x08028faa
08028fa6  9004      lsls	r0, r2, #18
08028fa8  06d5      bpl	#12 ; -> 0x08028fb8 ; branch_target=0x08028fb8
08028faa  5c1c      adds	r4, r3, #1
08028fac  f6d1      bne	#-20 ; -> 0x08028f9c ; branch_target=0x08028f9c
08028fae  4ff00040  mov.w	r0, #2147483648
08028fb2  5df8044b  ldr	r4, [sp], #4
08028fb6  7047      bx	lr
08028fb8  4b6b      ldr	r3, [r1, #52]
08028fba  5a07      lsls	r2, r3, #29
08028fbc  0cd4      bmi	#24 ; -> 0x08028fd8 ; branch_target=0x08028fd8
08028fbe  486b      ldr	r0, [r1, #52]
08028fc0  10f00100  ands	r0, r0, #1
08028fc4  05d1      bne	#10 ; -> 0x08028fd2 ; branch_target=0x08028fd2
08028fc6  4b6b      ldr	r3, [r1, #52]
08028fc8  5b06      lsls	r3, r3, #25
08028fca  f2d5      bpl	#-28 ; -> 0x08028fb2 ; branch_target=0x08028fb2
08028fcc  4023      movs	r3, #64
08028fce  8b63      str	r3, [r1, #56]
08028fd0  efe7      b	#-34 ; -> 0x08028fb2 ; branch_target=0x08028fb2
08028fd2  0123      movs	r3, #1
08028fd4  8b63      str	r3, [r1, #56]
08028fd6  ece7      b	#-40 ; -> 0x08028fb2 ; branch_target=0x08028fb2
08028fd8  0423      movs	r3, #4
08028fda  1846      mov	r0, r3
08028fdc  8b63      str	r3, [r1, #56]
08028fde  e8e7      b	#-48 ; -> 0x08028fb2 ; branch_target=0x08028fb2
