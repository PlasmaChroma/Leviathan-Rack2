; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022c98  f8b5      push	{r3, r4, r5, r6, r7, lr}
08022c9a  0546      mov	r5, r0
08022c9c  264c      ldr	r4, [pc, #152] ; [0x08022d38] = 0x52002000
08022c9e  2769      ldr	r7, [r4, #16]
08022ca0  fdf77cfb  bl	#-10504 ; -> 0x0802039c ; branch_target=0x0802039c
08022ca4  022d      cmp	r5, #2
08022ca6  0646      mov	r6, r0
08022ca8  0bd0      beq	#22 ; -> 0x08022cc2 ; branch_target=0x08022cc2
08022caa  2369      ldr	r3, [r4, #16]
08022cac  5807      lsls	r0, r3, #29
08022cae  31d5      bpl	#98 ; -> 0x08022d14 ; branch_target=0x08022d14
08022cb0  fdf774fb  bl	#-10520 ; -> 0x0802039c ; branch_target=0x0802039c
08022cb4  4cf25033  movw	r3, #50000
08022cb8  801b      subs	r0, r0, r6
08022cba  9842      cmp	r0, r3
08022cbc  f5d9      bls	#-22 ; -> 0x08022caa ; branch_target=0x08022caa
08022cbe  0320      movs	r0, #3
08022cc0  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022cc2  d4f81051  ldr.w	r5, [r4, #272]
08022cc6  4cf25037  movw	r7, #50000
08022cca  1c4b      ldr	r3, [pc, #112] ; [0x08022d3c] = 0x17ee0000
08022ccc  1d40      ands	r5, r3
08022cce  d4f81031  ldr.w	r3, [r4, #272]
08022cd2  5b07      lsls	r3, r3, #29
08022cd4  08d5      bpl	#16 ; -> 0x08022ce8 ; branch_target=0x08022ce8
08022cd6  fdf761fb  bl	#-10558 ; -> 0x0802039c ; branch_target=0x0802039c
08022cda  831b      subs	r3, r0, r6
08022cdc  bb42      cmp	r3, r7
08022cde  eed8      bhi	#-36 ; -> 0x08022cbe ; branch_target=0x08022cbe
08022ce0  d4f81031  ldr.w	r3, [r4, #272]
08022ce4  5b07      lsls	r3, r3, #29
08022ce6  f6d4      bmi	#-20 ; -> 0x08022cd6 ; branch_target=0x08022cd6
08022ce8  55b9      cbnz	r5, #20 ; -> 0x08022d00 ; branch_target=0x08022d00
08022cea  134b      ldr	r3, [pc, #76] ; [0x08022d38] = 0x52002000
08022cec  d3f81021  ldr.w	r2, [r3, #272]
08022cf0  d203      lsls	r2, r2, #15
08022cf2  03d5      bpl	#6 ; -> 0x08022cfc ; branch_target=0x08022cfc
08022cf4  4ff48032  mov.w	r2, #65536
08022cf8  c3f81421  str.w	r2, [r3, #276]
08022cfc  0020      movs	r0, #0
08022cfe  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022d00  0f4a      ldr	r2, [pc, #60] ; [0x08022d40] = 0x20002050
08022d02  45f00043  orr	r3, r5, #2147483648
08022d06  9169      ldr	r1, [r2, #24]
08022d08  0b43      orrs	r3, r1
08022d0a  9361      str	r3, [r2, #24]
08022d0c  c4f81451  str.w	r5, [r4, #276]
08022d10  0120      movs	r0, #1
08022d12  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08022d14  094b      ldr	r3, [pc, #36] ; [0x08022d3c] = 0x17ee0000
08022d16  3b40      ands	r3, r7
08022d18  05d0      beq	#10 ; -> 0x08022d26 ; branch_target=0x08022d26
08022d1a  0949      ldr	r1, [pc, #36] ; [0x08022d40] = 0x20002050
08022d1c  8a69      ldr	r2, [r1, #24]
08022d1e  1a43      orrs	r2, r3
08022d20  8a61      str	r2, [r1, #24]
08022d22  6361      str	r3, [r4, #20]
08022d24  f4e7      b	#-24 ; -> 0x08022d10 ; branch_target=0x08022d10
08022d26  012d      cmp	r5, #1
08022d28  dfd1      bne	#-66 ; -> 0x08022cea ; branch_target=0x08022cea
08022d2a  2369      ldr	r3, [r4, #16]
08022d2c  d903      lsls	r1, r3, #15
08022d2e  e5d5      bpl	#-54 ; -> 0x08022cfc ; branch_target=0x08022cfc
08022d30  4ff48033  mov.w	r3, #65536
08022d34  6361      str	r3, [r4, #20]
08022d36  e1e7      b	#-62 ; -> 0x08022cfc ; branch_target=0x08022cfc
