; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802bf88  70b5      push	{r4, r5, r6, lr}
0802bf8a  40b1      cbz	r0, #16 ; -> 0x0802bf9e ; branch_target=0x0802bf9e
0802bf8c  0368      ldr	r3, [r0]
0802bf8e  0446      mov	r4, r0
0802bf90  2bb1      cbz	r3, #10 ; -> 0x0802bf9e ; branch_target=0x0802bf9e
0802bf92  1a78      ldrb	r2, [r3]
0802bf94  1ab1      cbz	r2, #6 ; -> 0x0802bf9e ; branch_target=0x0802bf9e
0802bf96  8188      ldrh	r1, [r0, #4]
0802bf98  da88      ldrh	r2, [r3, #6]
0802bf9a  9142      cmp	r1, r2
0802bf9c  02d0      beq	#4 ; -> 0x0802bfa4 ; branch_target=0x0802bfa4
0802bf9e  0925      movs	r5, #9
0802bfa0  2846      mov	r0, r5
0802bfa2  70bd      pop	{r4, r5, r6, pc}
0802bfa4  5878      ldrb	r0, [r3, #1]
0802bfa6  fdf78bfc  bl	#-9962 ; -> 0x080298c0 ; branch_target=0x080298c0
0802bfaa  c007      lsls	r0, r0, #31
0802bfac  f7d4      bmi	#-18 ; -> 0x0802bf9e ; branch_target=0x0802bf9e
0802bfae  657d      ldrb	r5, [r4, #21]
0802bfb0  002d      cmp	r5, #0
0802bfb2  f5d1      bne	#-22 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802bfb4  237d      ldrb	r3, [r4, #20]
0802bfb6  9907      lsls	r1, r3, #30
0802bfb8  22d5      bpl	#68 ; -> 0x0802c000 ; branch_target=0x0802c000
0802bfba  a369      ldr	r3, [r4, #24]
0802bfbc  e268      ldr	r2, [r4, #12]
0802bfbe  2668      ldr	r6, [r4]
0802bfc0  9342      cmp	r3, r2
0802bfc2  edd2      bhs	#-38 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802bfc4  f3b9      cbnz	r3, #60 ; -> 0x0802c004 ; branch_target=0x0802c004
0802bfc6  2a46      mov	r2, r5
0802bfc8  a168      ldr	r1, [r4, #8]
0802bfca  2046      mov	r0, r4
0802bfcc  fef70cfd  bl	#-5608 ; -> 0x0802a9e8 ; branch_target=0x0802a9e8
0802bfd0  a560      str	r5, [r4, #8]
0802bfd2  a369      ldr	r3, [r4, #24]
0802bfd4  e360      str	r3, [r4, #12]
0802bfd6  237d      ldrb	r3, [r4, #20]
0802bfd8  43f04003  orr	r3, r3, #64
0802bfdc  2375      strb	r3, [r4, #20]
0802bfde  0028      cmp	r0, #0
0802bfe0  3ad1      bne	#116 ; -> 0x0802c058 ; branch_target=0x0802c058
0802bfe2  1b06      lsls	r3, r3, #24
0802bfe4  dcd5      bpl	#-72 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802bfe6  0123      movs	r3, #1
0802bfe8  226a      ldr	r2, [r4, #32]
0802bfea  7078      ldrb	r0, [r6, #1]
0802bfec  04f13001  add.w	r1, r4, #48
0802bff0  fdf790fc  bl	#-9952 ; -> 0x08029914 ; branch_target=0x08029914
0802bff4  30bb      cbnz	r0, #76 ; -> 0x0802c044 ; branch_target=0x0802c044
0802bff6  237d      ldrb	r3, [r4, #20]
0802bff8  03f07f03  and	r3, r3, #127
0802bffc  2375      strb	r3, [r4, #20]
0802bffe  cfe7      b	#-98 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802c000  0725      movs	r5, #7
0802c002  cde7      b	#-102 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802c004  e169      ldr	r1, [r4, #28]
0802c006  3046      mov	r0, r6
0802c008  fef700f9  bl	#-7680 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802c00c  421c      adds	r2, r0, #1
0802c00e  13d0      beq	#38 ; -> 0x0802c038 ; branch_target=0x0802c038
0802c010  0128      cmp	r0, #1
0802c012  08d0      beq	#16 ; -> 0x0802c026 ; branch_target=0x0802c026
0802c014  7369      ldr	r3, [r6, #20]
0802c016  8342      cmp	r3, r0
0802c018  17d9      bls	#46 ; -> 0x0802c04a ; branch_target=0x0802c04a
0802c01a  0146      mov	r1, r0
0802c01c  e269      ldr	r2, [r4, #28]
0802c01e  2046      mov	r0, r4
0802c020  fef7e2fc  bl	#-5692 ; -> 0x0802a9e8 ; branch_target=0x0802a9e8
0802c024  d5e7      b	#-86 ; -> 0x0802bfd2 ; branch_target=0x0802bfd2
0802c026  237d      ldrb	r3, [r4, #20]
0802c028  0225      movs	r5, #2
0802c02a  a269      ldr	r2, [r4, #24]
0802c02c  43f04003  orr	r3, r3, #64
0802c030  6575      strb	r5, [r4, #21]
0802c032  e260      str	r2, [r4, #12]
0802c034  2375      strb	r3, [r4, #20]
0802c036  b3e7      b	#-154 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802c038  237d      ldrb	r3, [r4, #20]
0802c03a  a269      ldr	r2, [r4, #24]
0802c03c  43f04003  orr	r3, r3, #64
0802c040  e260      str	r2, [r4, #12]
0802c042  2375      strb	r3, [r4, #20]
0802c044  0125      movs	r5, #1
0802c046  6575      strb	r5, [r4, #21]
0802c048  aae7      b	#-172 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
0802c04a  237d      ldrb	r3, [r4, #20]
0802c04c  a269      ldr	r2, [r4, #24]
0802c04e  43f04003  orr	r3, r3, #64
0802c052  e260      str	r2, [r4, #12]
0802c054  2375      strb	r3, [r4, #20]
0802c056  c4e7      b	#-120 ; -> 0x0802bfe2 ; branch_target=0x0802bfe2
0802c058  0546      mov	r5, r0
0802c05a  6575      strb	r5, [r4, #21]
0802c05c  a0e7      b	#-192 ; -> 0x0802bfa0 ; branch_target=0x0802bfa0
