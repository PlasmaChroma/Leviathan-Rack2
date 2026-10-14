; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08033f50  2de9f043  push.w	{r4, r5, r6, r7, r8, r9, lr}
08033f54  2c4c      ldr	r4, [pc, #176] ; [0x08034008] = 0x200033f8
08033f56  87b0      sub	sp, #28
08033f58  0125      movs	r5, #1
08033f5a  0026      movs	r6, #0
08033f5c  1027      movs	r7, #16
08033f5e  02a9      add	r1, sp, #8
08033f60  4ff6ff72  movw	r2, #65535
08033f64  2046      mov	r0, r4
08033f66  0196      str	r6, [sp, #4]
08033f68  4ff00808  mov.w	r8, #8
08033f6c  4ff00309  mov.w	r9, #3
08033f70  cde90257  strd	r5, r7, [sp, #8]
08033f74  cde90456  strd	r5, r6, [sp, #16]
08033f78  f2f74cfe  bl	#-54120 ; -> 0x08026c14 ; branch_target=0x08026c14
08033f7c  2846      mov	r0, r5
08033f7e  ecf713fa  bl	#-80858 ; -> 0x080203a8 ; branch_target=0x080203a8
08033f82  0223      movs	r3, #2
08033f84  02a9      add	r1, sp, #8
08033f86  4ff6ff72  movw	r2, #65535
08033f8a  2046      mov	r0, r4
08033f8c  0293      str	r3, [sp, #8]
08033f8e  0596      str	r6, [sp, #20]
08033f90  cde90375  strd	r7, r5, [sp, #12]
08033f94  f2f73efe  bl	#-54148 ; -> 0x08026c14 ; branch_target=0x08026c14
08033f98  40f22223  movw	r3, #546
08033f9c  02a9      add	r1, sp, #8
08033f9e  4ff6ff72  movw	r2, #65535
08033fa2  0193      str	r3, [sp, #4]
08033fa4  2046      mov	r0, r4
08033fa6  019b      ldr	r3, [sp, #4]
08033fa8  0495      str	r5, [sp, #16]
08033faa  0593      str	r3, [sp, #20]
08033fac  9fed147b  vldr	d7, [pc, #80] ; [0x08034000] = 0x00000004 / f64_bits_interpretation=3.395193265742062e-313
08033fb0  8ded027b  vstr	d7, [sp, #8]
08033fb4  f2f72efe  bl	#-54180 ; -> 0x08026c14 ; branch_target=0x08026c14
08033fb8  0deb0801  add.w	r1, sp, r8
08033fbc  4ff6ff72  movw	r2, #65535
08033fc0  2046      mov	r0, r4
08033fc2  cde90297  strd	r9, r7, [sp, #8]
08033fc6  cde90486  strd	r8, r6, [sp, #16]
08033fca  f2f723fe  bl	#-54202 ; -> 0x08026c14 ; branch_target=0x08026c14
08033fce  2846      mov	r0, r5
08033fd0  ecf7eaf9  bl	#-80940 ; -> 0x080203a8 ; branch_target=0x080203a8
08033fd4  0deb0801  add.w	r1, sp, r8
08033fd8  4ff6ff72  movw	r2, #65535
08033fdc  2046      mov	r0, r4
08033fde  cdf80890  str.w	r9, [sp, #8]
08033fe2  0397      str	r7, [sp, #12]
08033fe4  cde90486  strd	r8, r6, [sp, #16]
08033fe8  f2f714fe  bl	#-54232 ; -> 0x08026c14 ; branch_target=0x08026c14
08033fec  40f26951  movw	r1, #1385
08033ff0  2046      mov	r0, r4
08033ff2  f2f72ffe  bl	#-54178 ; -> 0x08026c54 ; branch_target=0x08026c54
08033ff6  07b0      add	sp, #28
08033ff8  bde8f083  pop.w	{r4, r5, r6, r7, r8, r9, pc}
