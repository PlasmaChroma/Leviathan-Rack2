; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022e94  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08022e98  0d46      mov	r5, r1
08022e9a  334f      ldr	r7, [pc, #204] ; [0x08022f68] = 0x52002000
08022e9c  0446      mov	r4, r0
08022e9e  3e69      ldr	r6, [r7, #16]
08022ea0  fdf77cfa  bl	#-11016 ; -> 0x0802039c ; branch_target=0x0802039c
08022ea4  022d      cmp	r5, #2
08022ea6  8246      mov	r10, r0
08022ea8  4ad0      beq	#148 ; -> 0x08022f40 ; branch_target=0x08022f40
08022eaa  304b      ldr	r3, [pc, #192] ; [0x08022f6c] = 0x17ee0000
08022eac  0027      movs	r7, #0
08022eae  4ff00409  mov.w	r9, #4
08022eb2  1e40      ands	r6, r3
08022eb4  b046      mov	r8, r6
08022eb6  b9f1040f  cmp.w	r9, #4
08022eba  17d0      beq	#46 ; -> 0x08022eec ; branch_target=0x08022eec
08022ebc  601c      adds	r0, r4, #1
08022ebe  30d1      bne	#96 ; -> 0x08022f22 ; branch_target=0x08022f22
08022ec0  294a      ldr	r2, [pc, #164] ; [0x08022f68] = 0x52002000
08022ec2  d2f81031  ldr.w	r3, [r2, #272]
08022ec6  5907      lsls	r1, r3, #29
08022ec8  fbd4      bmi	#-10 ; -> 0x08022ec2 ; branch_target=0x08022ec2
08022eca  b8f1000f  cmp.w	r8, #0
08022ece  16d1      bne	#44 ; -> 0x08022efe ; branch_target=0x08022efe
08022ed0  012d      cmp	r5, #1
08022ed2  254b      ldr	r3, [pc, #148] ; [0x08022f68] = 0x52002000
08022ed4  1ed0      beq	#60 ; -> 0x08022f14 ; branch_target=0x08022f14
08022ed6  d3f81021  ldr.w	r2, [r3, #272]
08022eda  d203      lsls	r2, r2, #15
08022edc  03d5      bpl	#6 ; -> 0x08022ee6 ; branch_target=0x08022ee6
08022ede  4ff48032  mov.w	r2, #65536
08022ee2  c3f81421  str.w	r2, [r3, #276]
08022ee6  0020      movs	r0, #0
08022ee8  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08022eec  631c      adds	r3, r4, #1
08022eee  35d1      bne	#106 ; -> 0x08022f5c ; branch_target=0x08022f5c
08022ef0  1d4a      ldr	r2, [pc, #116] ; [0x08022f68] = 0x52002000
08022ef2  1369      ldr	r3, [r2, #16]
08022ef4  5c07      lsls	r4, r3, #29
08022ef6  fcd4      bmi	#-8 ; -> 0x08022ef2 ; branch_target=0x08022ef2
08022ef8  b8f1000f  cmp.w	r8, #0
08022efc  e8d0      beq	#-48 ; -> 0x08022ed0 ; branch_target=0x08022ed0
08022efe  1c4a      ldr	r2, [pc, #112] ; [0x08022f70] = 0x20002050
08022f00  9369      ldr	r3, [r2, #24]
08022f02  3343      orrs	r3, r6
08022f04  9361      str	r3, [r2, #24]
08022f06  184b      ldr	r3, [pc, #96] ; [0x08022f68] = 0x52002000
08022f08  37b3      cbz	r7, #76 ; -> 0x08022f58 ; branch_target=0x08022f58
08022f0a  c3f81481  str.w	r8, [r3, #276]
08022f0e  0120      movs	r0, #1
08022f10  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08022f14  1a69      ldr	r2, [r3, #16]
08022f16  d103      lsls	r1, r2, #15
08022f18  e5d5      bpl	#-54 ; -> 0x08022ee6 ; branch_target=0x08022ee6
08022f1a  4ff48032  mov.w	r2, #65536
08022f1e  5a61      str	r2, [r3, #20]
08022f20  e1e7      b	#-62 ; -> 0x08022ee6 ; branch_target=0x08022ee6
08022f22  114b      ldr	r3, [pc, #68] ; [0x08022f68] = 0x52002000
08022f24  d3f81031  ldr.w	r3, [r3, #272]
08022f28  5a07      lsls	r2, r3, #29
08022f2a  ced5      bpl	#-100 ; -> 0x08022eca ; branch_target=0x08022eca
08022f2c  fdf736fa  bl	#-11156 ; -> 0x0802039c ; branch_target=0x0802039c
08022f30  a0eb0a03  sub.w	r3, r0, r10
08022f34  a342      cmp	r3, r4
08022f36  01d8      bhi	#2 ; -> 0x08022f3c ; branch_target=0x08022f3c
08022f38  002c      cmp	r4, #0
08022f3a  bcd1      bne	#-136 ; -> 0x08022eb6 ; branch_target=0x08022eb6
08022f3c  0320      movs	r0, #3
08022f3e  d3e7      b	#-90 ; -> 0x08022ee8 ; branch_target=0x08022ee8
08022f40  d7f81021  ldr.w	r2, [r7, #272]
08022f44  4ff00047  mov.w	r7, #2147483648
08022f48  084b      ldr	r3, [pc, #32] ; [0x08022f6c] = 0x17ee0000
08022f4a  dff82890  ldr.w	r9, [pc, #40] ; [0x08022f74] = 0x80000004
08022f4e  02ea0308  and.w	r8, r2, r3
08022f52  48f00046  orr	r6, r8, #2147483648
08022f56  aee7      b	#-164 ; -> 0x08022eb6 ; branch_target=0x08022eb6
08022f58  5e61      str	r6, [r3, #20]
08022f5a  d8e7      b	#-80 ; -> 0x08022f0e ; branch_target=0x08022f0e
08022f5c  024b      ldr	r3, [pc, #8] ; [0x08022f68] = 0x52002000
08022f5e  1b69      ldr	r3, [r3, #16]
08022f60  5807      lsls	r0, r3, #29
08022f62  e3d4      bmi	#-58 ; -> 0x08022f2c ; branch_target=0x08022f2c
08022f64  b1e7      b	#-158 ; -> 0x08022eca ; branch_target=0x08022eca
