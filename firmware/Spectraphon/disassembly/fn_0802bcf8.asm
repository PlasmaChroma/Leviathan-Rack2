; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802bcf8  30b3      cbz	r0, #76 ; -> 0x0802bd48 ; branch_target=0x0802bd48
0802bcfa  10b5      push	{r4, lr}
0802bcfc  0368      ldr	r3, [r0]
0802bcfe  0446      mov	r4, r0
0802bd00  2bb1      cbz	r3, #10 ; -> 0x0802bd0e ; branch_target=0x0802bd0e
0802bd02  1a78      ldrb	r2, [r3]
0802bd04  1ab1      cbz	r2, #6 ; -> 0x0802bd0e ; branch_target=0x0802bd0e
0802bd06  8188      ldrh	r1, [r0, #4]
0802bd08  da88      ldrh	r2, [r3, #6]
0802bd0a  9142      cmp	r1, r2
0802bd0c  01d0      beq	#2 ; -> 0x0802bd12 ; branch_target=0x0802bd12
0802bd0e  0920      movs	r0, #9
0802bd10  10bd      pop	{r4, pc}
0802bd12  5878      ldrb	r0, [r3, #1]
0802bd14  fdf7d4fd  bl	#-9304 ; -> 0x080298c0 ; branch_target=0x080298c0
0802bd18  10f00100  ands	r0, r0, #1
0802bd1c  f7d1      bne	#-18 ; -> 0x0802bd0e ; branch_target=0x0802bd0e
0802bd1e  2369      ldr	r3, [r4, #16]
0802bd20  7bb1      cbz	r3, #30 ; -> 0x0802bd42 ; branch_target=0x0802bd42
0802bd22  013b      subs	r3, #1
0802bd24  012b      cmp	r3, #1
0802bd26  11d8      bhi	#34 ; -> 0x0802bd4c ; branch_target=0x0802bd4c
0802bd28  0d49      ldr	r1, [pc, #52] ; [0x0802bd60] = 0x2000206c
0802bd2a  4fea031c  lsl.w	r12, r3, #4
0802bd2e  01eb0313  add.w	r3, r1, r3, lsl #4
0802bd32  9a89      ldrh	r2, [r3, #12]
0802bd34  b2f5807f  cmp.w	r2, #256
0802bd38  10d0      beq	#32 ; -> 0x0802bd5c ; branch_target=0x0802bd5c
0802bd3a  4ab9      cbnz	r2, #18 ; -> 0x0802bd50 ; branch_target=0x0802bd50
0802bd3c  0023      movs	r3, #0
0802bd3e  41f80c30  str.w	r3, [r1, r12]
0802bd42  0023      movs	r3, #0
0802bd44  2360      str	r3, [r4]
0802bd46  10bd      pop	{r4, pc}
0802bd48  0920      movs	r0, #9
0802bd4a  7047      bx	lr
0802bd4c  0220      movs	r0, #2
0802bd4e  10bd      pop	{r4, pc}
0802bd50  013a      subs	r2, #1
0802bd52  92b2      uxth	r2, r2
0802bd54  9a81      strh	r2, [r3, #12]
0802bd56  002a      cmp	r2, #0
0802bd58  f3d1      bne	#-26 ; -> 0x0802bd42 ; branch_target=0x0802bd42
0802bd5a  efe7      b	#-34 ; -> 0x0802bd3c ; branch_target=0x0802bd3c
0802bd5c  9881      strh	r0, [r3, #12]
0802bd5e  ede7      b	#-38 ; -> 0x0802bd3c ; branch_target=0x0802bd3c
