; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080261bc  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
080261c0  0d46      mov	r5, r1
080261c2  88b0      sub	sp, #32
080261c4  0446      mov	r4, r0
080261c6  9146      mov	r9, r2
080261c8  1f46      mov	r7, r3
080261ca  ddf84080  ldr.w	r8, [sp, #64]
080261ce  faf7e5f8  bl	#-24118 ; -> 0x0802039c ; branch_target=0x0802039c
080261d2  002d      cmp	r5, #0
080261d4  66d0      beq	#204 ; -> 0x080262a4 ; branch_target=0x080262a4
080261d6  94f83030  ldrb.w	r3, [r4, #48]
080261da  012b      cmp	r3, #1
080261dc  54d1      bne	#168 ; -> 0x08026288 ; branch_target=0x08026288
080261de  09eb0703  add.w	r3, r9, r7
080261e2  226d      ldr	r2, [r4, #80]
080261e4  0021      movs	r1, #0
080261e6  9342      cmp	r3, r2
080261e8  6163      str	r1, [r4, #52]
080261ea  60d8      bhi	#192 ; -> 0x080262ae ; branch_target=0x080262ae
080261ec  0323      movs	r3, #3
080261ee  0646      mov	r6, r0
080261f0  84f83030  strb.w	r3, [r4, #48]
080261f4  2368      ldr	r3, [r4]
080261f6  d962      str	r1, [r3, #44]
080261f8  02a9      add	r1, sp, #8
080261fa  a36b      ldr	r3, [r4, #56]
080261fc  2068      ldr	r0, [r4]
080261fe  012b      cmp	r3, #1
08026200  4ff00003  mov.w	r3, #0
08026204  cde90533  strd	r3, r3, [sp, #20]
08026208  0793      str	r3, [sp, #28]
0802620a  4ff0ff33  mov.w	r3, #4294967295
0802620e  18bf      it	ne
08026210  4fea4929  lslne.w	r9, r9, #9
08026214  0293      str	r3, [sp, #8]
08026216  7b02      lsls	r3, r7, #9
08026218  0393      str	r3, [sp, #12]
0802621a  9023      movs	r3, #144
0802621c  0493      str	r3, [sp, #16]
0802621e  02f057f9  bl	#8878 ; -> 0x080284d0 ; branch_target=0x080284d0
08026222  2268      ldr	r2, [r4]
08026224  012f      cmp	r7, #1
08026226  4946      mov	r1, r9
08026228  d368      ldr	r3, [r2, #12]
0802622a  43f04003  orr	r3, r3, #64
0802622e  d360      str	r3, [r2, #12]
08026230  32d9      bls	#100 ; -> 0x08026298 ; branch_target=0x08026298
08026232  2023      movs	r3, #32
08026234  2068      ldr	r0, [r4]
08026236  e362      str	r3, [r4, #44]
08026238  02f010fc  bl	#10272 ; -> 0x08028a5c ; branch_target=0x08028a5c
0802623c  2268      ldr	r2, [r4]
0802623e  0028      cmp	r0, #0
08026240  3ad1      bne	#116 ; -> 0x080262b8 ; branch_target=0x080262b8
08026242  ddf80c90  ldr.w	r9, [sp, #12]
08026246  05e0      b	#10 ; -> 0x08026254 ; branch_target=0x08026254
08026248  faf7a8f8  bl	#-24240 ; -> 0x0802039c ; branch_target=0x0802039c
0802624c  821b      subs	r2, r0, r6
0802624e  4245      cmp	r2, r8
08026250  5ed2      bhs	#188 ; -> 0x08026310 ; branch_target=0x08026310
08026252  2268      ldr	r2, [r4]
08026254  536b      ldr	r3, [r2, #52]
08026256  1046      mov	r0, r2
08026258  13f48d7f  tst.w	r3, #282
0802625c  37d1      bne	#110 ; -> 0x080262ce ; branch_target=0x080262ce
0802625e  536b      ldr	r3, [r2, #52]
08026260  5a04      lsls	r2, r3, #17
08026262  f1d5      bpl	#-30 ; -> 0x08026248 ; branch_target=0x08026248
08026264  b9f11f0f  cmp.w	r9, #31
08026268  eed9      bls	#-36 ; -> 0x08026248 ; branch_target=0x08026248
0802626a  05f1200a  add.w	r10, r5, #32
0802626e  00e0      b	#0 ; -> 0x08026272 ; branch_target=0x08026272
08026270  2068      ldr	r0, [r4]
08026272  55f8043b  ldr	r3, [r5], #4
08026276  01a9      add	r1, sp, #4
08026278  0193      str	r3, [sp, #4]
0802627a  02f013f9  bl	#8742 ; -> 0x080284a4 ; branch_target=0x080284a4
0802627e  5545      cmp	r5, r10
08026280  f6d1      bne	#-20 ; -> 0x08026270 ; branch_target=0x08026270
08026282  a9f12009  sub.w	r9, r9, #32
08026286  dfe7      b	#-66 ; -> 0x08026248 ; branch_target=0x08026248
08026288  636b      ldr	r3, [r4, #52]
0802628a  43f00053  orr	r3, r3, #536870912
0802628e  6363      str	r3, [r4, #52]
08026290  0120      movs	r0, #1
08026292  08b0      add	sp, #32
08026294  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
08026298  1023      movs	r3, #16
0802629a  2068      ldr	r0, [r4]
0802629c  e362      str	r3, [r4, #44]
0802629e  02f031fb  bl	#9826 ; -> 0x08028904 ; branch_target=0x08028904
080262a2  cbe7      b	#-106 ; -> 0x0802623c ; branch_target=0x0802623c
080262a4  636b      ldr	r3, [r4, #52]
080262a6  43f00063  orr	r3, r3, #134217728
080262aa  6363      str	r3, [r4, #52]
080262ac  f0e7      b	#-32 ; -> 0x08026290 ; branch_target=0x08026290
080262ae  636b      ldr	r3, [r4, #52]
080262b0  43f00073  orr	r3, r3, #33554432
080262b4  6363      str	r3, [r4, #52]
080262b6  ebe7      b	#-42 ; -> 0x08026290 ; branch_target=0x08026290
080262b8  314b      ldr	r3, [pc, #196] ; [0x08026380] = 0x1fe00fff
080262ba  0121      movs	r1, #1
080262bc  9363      str	r3, [r2, #56]
080262be  636b      ldr	r3, [r4, #52]
080262c0  0022      movs	r2, #0
080262c2  0343      orrs	r3, r0
080262c4  6363      str	r3, [r4, #52]
080262c6  84f83010  strb.w	r1, [r4, #48]
080262ca  e262      str	r2, [r4, #44]
080262cc  e0e7      b	#-64 ; -> 0x08026290 ; branch_target=0x08026290
080262ce  d368      ldr	r3, [r2, #12]
080262d0  23f04003  bic	r3, r3, #64
080262d4  d360      str	r3, [r2, #12]
080262d6  2268      ldr	r2, [r4]
080262d8  536b      ldr	r3, [r2, #52]
080262da  db05      lsls	r3, r3, #23
080262dc  01d5      bpl	#2 ; -> 0x080262e2 ; branch_target=0x080262e2
080262de  012f      cmp	r7, #1
080262e0  22d8      bhi	#68 ; -> 0x08026328 ; branch_target=0x08026328
080262e2  536b      ldr	r3, [r2, #52]
080262e4  13f00803  ands	r3, r3, #8
080262e8  32d1      bne	#100 ; -> 0x08026350 ; branch_target=0x08026350
080262ea  516b      ldr	r1, [r2, #52]
080262ec  11f00201  ands	r1, r1, #2
080262f0  23d1      bne	#70 ; -> 0x0802633a ; branch_target=0x0802633a
080262f2  536b      ldr	r3, [r2, #52]
080262f4  13f01003  ands	r3, r3, #16
080262f8  36d0      beq	#108 ; -> 0x08026368 ; branch_target=0x08026368
080262fa  214b      ldr	r3, [pc, #132] ; [0x08026380] = 0x1fe00fff
080262fc  0120      movs	r0, #1
080262fe  9363      str	r3, [r2, #56]
08026300  636b      ldr	r3, [r4, #52]
08026302  43f01003  orr	r3, r3, #16
08026306  6363      str	r3, [r4, #52]
08026308  84f83000  strb.w	r0, [r4, #48]
0802630c  e162      str	r1, [r4, #44]
0802630e  bfe7      b	#-130 ; -> 0x08026290 ; branch_target=0x08026290
08026310  2168      ldr	r1, [r4]
08026312  0122      movs	r2, #1
08026314  1a48      ldr	r0, [pc, #104] ; [0x08026380] = 0x1fe00fff
08026316  0023      movs	r3, #0
08026318  8863      str	r0, [r1, #56]
0802631a  0320      movs	r0, #3
0802631c  616b      ldr	r1, [r4, #52]
0802631e  6163      str	r1, [r4, #52]
08026320  84f83020  strb.w	r2, [r4, #48]
08026324  e362      str	r3, [r4, #44]
08026326  b4e7      b	#-152 ; -> 0x08026292 ; branch_target=0x08026292
08026328  a36b      ldr	r3, [r4, #56]
0802632a  032b      cmp	r3, #3
0802632c  d9d0      beq	#-78 ; -> 0x080262e2 ; branch_target=0x080262e2
0802632e  1046      mov	r0, r2
08026330  02f040fc  bl	#10368 ; -> 0x08028bb4 ; branch_target=0x08028bb4
08026334  f8b9      cbnz	r0, #62 ; -> 0x08026376 ; branch_target=0x08026376
08026336  2268      ldr	r2, [r4]
08026338  d3e7      b	#-90 ; -> 0x080262e2 ; branch_target=0x080262e2
0802633a  1148      ldr	r0, [pc, #68] ; [0x08026380] = 0x1fe00fff
0802633c  0121      movs	r1, #1
0802633e  9063      str	r0, [r2, #56]
08026340  626b      ldr	r2, [r4, #52]
08026342  42f00202  orr	r2, r2, #2
08026346  6263      str	r2, [r4, #52]
08026348  84f83010  strb.w	r1, [r4, #48]
0802634c  e362      str	r3, [r4, #44]
0802634e  9fe7      b	#-194 ; -> 0x08026290 ; branch_target=0x08026290
08026350  0b4b      ldr	r3, [pc, #44] ; [0x08026380] = 0x1fe00fff
08026352  0120      movs	r0, #1
08026354  0021      movs	r1, #0
08026356  9363      str	r3, [r2, #56]
08026358  636b      ldr	r3, [r4, #52]
0802635a  43f00803  orr	r3, r3, #8
0802635e  6363      str	r3, [r4, #52]
08026360  84f83000  strb.w	r0, [r4, #48]
08026364  e162      str	r1, [r4, #44]
08026366  93e7      b	#-218 ; -> 0x08026290 ; branch_target=0x08026290
08026368  064d      ldr	r5, [pc, #24] ; [0x08026384] = 0x18000f3a
0802636a  0121      movs	r1, #1
0802636c  1846      mov	r0, r3
0802636e  9563      str	r5, [r2, #56]
08026370  84f83010  strb.w	r1, [r4, #48]
08026374  8de7      b	#-230 ; -> 0x08026292 ; branch_target=0x08026292
08026376  2368      ldr	r3, [r4]
08026378  0121      movs	r1, #1
0802637a  014a      ldr	r2, [pc, #4] ; [0x08026380] = 0x1fe00fff
0802637c  9a63      str	r2, [r3, #56]
0802637e  9ee7      b	#-196 ; -> 0x080262be ; branch_target=0x080262be
