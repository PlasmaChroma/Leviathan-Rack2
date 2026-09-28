; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08023434  f8b5      push	{r3, r4, r5, r6, r7, lr}
08023436  90f83630  ldrb.w	r3, [r0, #54]
0802343a  0446      mov	r4, r0
0802343c  03f0ff02  and	r2, r3, #255
08023440  002b      cmp	r3, #0
08023442  54d0      beq	#168 ; -> 0x080234ee ; branch_target=0x080234ee
08023444  0223      movs	r3, #2
08023446  a76a      ldr	r7, [r4, #40]
08023448  d4e90060  ldrd	r6, r0, [r4]
0802344c  84f83630  strb.w	r3, [r4, #54]
08023450  0128      cmp	r0, #1
08023452  f168      ldr	r1, [r6, #12]
08023454  3fd0      beq	#126 ; -> 0x080234d6 ; branch_target=0x080234d6
08023456  b7f5000f  cmp.w	r7, #8388608
0802345a  3cd0      beq	#120 ; -> 0x080234d6 ; branch_target=0x080234d6
0802345c  6269      ldr	r2, [r4, #20]
0802345e  4ff6ff73  movw	r3, #65535
08023462  9a42      cmp	r2, r3
08023464  03d0      beq	#6 ; -> 0x0802346e ; branch_target=0x0802346e
08023466  21f46041  bic	r1, r1, #57344
0802346a  21f0c001  bic	r1, r1, #192
0802346e  a568      ldr	r5, [r4, #8]
08023470  40ea0703  orr.w	r3, r0, r7
08023474  2b43      orrs	r3, r5
08023476  256a      ldr	r5, [r4, #32]
08023478  2b43      orrs	r3, r5
0802347a  244d      ldr	r5, [pc, #144] ; [0x0802350c] = 0xff19f1fe
0802347c  0d40      ands	r5, r1
0802347e  616a      ldr	r1, [r4, #36]
08023480  0b43      orrs	r3, r1
08023482  2b43      orrs	r3, r5
08023484  f8b1      cbz	r0, #62 ; -> 0x080234c6 ; branch_target=0x080234c6
08023486  0128      cmp	r0, #1
08023488  21d1      bne	#66 ; -> 0x080234ce ; branch_target=0x080234ce
0802348a  d4e90310  ldrd	r1, r0, [r4, #12]
0802348e  0143      orrs	r1, r0
08023490  0b43      orrs	r3, r1
08023492  4ff6ff71  movw	r1, #65535
08023496  8a42      cmp	r2, r1
08023498  04d0      beq	#8 ; -> 0x080234a4 ; branch_target=0x080234a4
0802349a  a169      ldr	r1, [r4, #24]
0802349c  0a43      orrs	r2, r1
0802349e  e169      ldr	r1, [r4, #28]
080234a0  0a43      orrs	r2, r1
080234a2  1343      orrs	r3, r2
080234a4  1a4a      ldr	r2, [pc, #104] ; [0x08023510] = 0x40002400 / f32_bits_interpretation=2.002197266
080234a6  f360      str	r3, [r6, #12]
080234a8  2368      ldr	r3, [r4]
080234aa  9342      cmp	r3, r2
080234ac  16d0      beq	#44 ; -> 0x080234dc ; branch_target=0x080234dc
080234ae  02f1c052  add.w	r2, r2, #402653184
080234b2  9342      cmp	r3, r2
080234b4  12d0      beq	#36 ; -> 0x080234dc ; branch_target=0x080234dc
080234b6  174a      ldr	r2, [pc, #92] ; [0x08023514] = 0x58002800
080234b8  9342      cmp	r3, r2
080234ba  1fd0      beq	#62 ; -> 0x080234fc ; branch_target=0x080234fc
080234bc  0123      movs	r3, #1
080234be  0020      movs	r0, #0
080234c0  84f83630  strb.w	r3, [r4, #54]
080234c4  f8bd      pop	{r3, r4, r5, r6, r7, pc}
080234c6  e169      ldr	r1, [r4, #28]
080234c8  2069      ldr	r0, [r4, #16]
080234ca  0143      orrs	r1, r0
080234cc  0b43      orrs	r3, r1
080234ce  b7f5000f  cmp.w	r7, #8388608
080234d2  ded1      bne	#-68 ; -> 0x08023492 ; branch_target=0x08023492
080234d4  d9e7      b	#-78 ; -> 0x0802348a ; branch_target=0x0802348a
080234d6  21f01e01  bic	r1, r1, #30
080234da  bfe7      b	#-130 ; -> 0x0802345c ; branch_target=0x0802345c
080234dc  d4e90b21  ldrd	r2, r1, [r4, #44]
080234e0  0020      movs	r0, #0
080234e2  0a43      orrs	r2, r1
080234e4  5a62      str	r2, [r3, #36]
080234e6  0123      movs	r3, #1
080234e8  84f83630  strb.w	r3, [r4, #54]
080234ec  f8bd      pop	{r3, r4, r5, r6, r7, pc}
080234ee  80f83520  strb.w	r2, [r0, #53]
080234f2  0ff08dfd  bl	#64282 ; -> 0x08033010 ; branch_target=0x08033010
080234f6  a5e7      b	#-182 ; -> 0x08023444 ; branch_target=0x08023444
080234fc  e26a      ldr	r2, [r4, #44]
080234fe  0020      movs	r0, #0
08023500  5a62      str	r2, [r3, #36]
08023502  0123      movs	r3, #1
08023504  84f83630  strb.w	r3, [r4, #54]
08023508  f8bd      pop	{r3, r4, r5, r6, r7, pc}
