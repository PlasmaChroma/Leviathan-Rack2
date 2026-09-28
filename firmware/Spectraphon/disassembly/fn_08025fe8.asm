; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08025fe8  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08025fec  0d46      mov	r5, r1
08025fee  86b0      sub	sp, #24
08025ff0  0446      mov	r4, r0
08025ff2  9146      mov	r9, r2
08025ff4  1f46      mov	r7, r3
08025ff6  ddf83880  ldr.w	r8, [sp, #56]
08025ffa  faf7cff9  bl	#-23650 ; -> 0x0802039c ; branch_target=0x0802039c
08025ffe  002d      cmp	r5, #0
08026000  67d0      beq	#206 ; -> 0x080260d2 ; branch_target=0x080260d2
08026002  94f83030  ldrb.w	r3, [r4, #48]
08026006  012b      cmp	r3, #1
08026008  54d1      bne	#168 ; -> 0x080260b4 ; branch_target=0x080260b4
0802600a  09eb0703  add.w	r3, r9, r7
0802600e  226d      ldr	r2, [r4, #80]
08026010  0021      movs	r1, #0
08026012  9342      cmp	r3, r2
08026014  6163      str	r1, [r4, #52]
08026016  61d8      bhi	#194 ; -> 0x080260dc ; branch_target=0x080260dc
08026018  0323      movs	r3, #3
0802601a  4ff0020a  mov.w	r10, #2
0802601e  0646      mov	r6, r0
08026020  84f83030  strb.w	r3, [r4, #48]
08026024  2368      ldr	r3, [r4]
08026026  d962      str	r1, [r3, #44]
08026028  6946      mov	r1, sp
0802602a  a36b      ldr	r3, [r4, #56]
0802602c  2068      ldr	r0, [r4]
0802602e  012b      cmp	r3, #1
08026030  4ff00003  mov.w	r3, #0
08026034  cde90433  strd	r3, r3, [sp, #16]
08026038  4ff0ff33  mov.w	r3, #4294967295
0802603c  18bf      it	ne
0802603e  4fea4929  lslne.w	r9, r9, #9
08026042  0093      str	r3, [sp]
08026044  7b02      lsls	r3, r7, #9
08026046  0193      str	r3, [sp, #4]
08026048  9023      movs	r3, #144
0802604a  cde9023a  strd	r3, r10, [sp, #8]
0802604e  02f03ffa  bl	#9342 ; -> 0x080284d0 ; branch_target=0x080284d0
08026052  2268      ldr	r2, [r4]
08026054  012f      cmp	r7, #1
08026056  d368      ldr	r3, [r2, #12]
08026058  43f04003  orr	r3, r3, #64
0802605c  d360      str	r3, [r2, #12]
0802605e  31d9      bls	#98 ; -> 0x080260c4 ; branch_target=0x080260c4
08026060  4946      mov	r1, r9
08026062  2068      ldr	r0, [r4]
08026064  c4f82ca0  str.w	r10, [r4, #44]
08026068  02f0a0fb  bl	#10048 ; -> 0x080287ac ; branch_target=0x080287ac
0802606c  2268      ldr	r2, [r4]
0802606e  0028      cmp	r0, #0
08026070  39d1      bne	#114 ; -> 0x080260e6 ; branch_target=0x080260e6
08026072  ddf80490  ldr.w	r9, [sp, #4]
08026076  05e0      b	#10 ; -> 0x08026084 ; branch_target=0x08026084
08026078  faf790f9  bl	#-23776 ; -> 0x0802039c ; branch_target=0x0802039c
0802607c  821b      subs	r2, r0, r6
0802607e  4245      cmp	r2, r8
08026080  5dd2      bhs	#186 ; -> 0x0802613e ; branch_target=0x0802613e
08026082  2268      ldr	r2, [r4]
08026084  536b      ldr	r3, [r2, #52]
08026086  1046      mov	r0, r2
08026088  13f4957f  tst.w	r3, #298
0802608c  36d1      bne	#108 ; -> 0x080260fc ; branch_target=0x080260fc
0802608e  536b      ldr	r3, [r2, #52]
08026090  1a04      lsls	r2, r3, #16
08026092  f1d5      bpl	#-30 ; -> 0x08026078 ; branch_target=0x08026078
08026094  b9f11f0f  cmp.w	r9, #31
08026098  eed9      bls	#-36 ; -> 0x08026078 ; branch_target=0x08026078
0802609a  05f1200a  add.w	r10, r5, #32
0802609e  00e0      b	#0 ; -> 0x080260a2 ; branch_target=0x080260a2
080260a0  2068      ldr	r0, [r4]
080260a2  02f0fbf9  bl	#9206 ; -> 0x0802849c ; branch_target=0x0802849c
080260a6  45f8040b  str	r0, [r5], #4
080260aa  aa45      cmp	r10, r5
080260ac  f8d1      bne	#-16 ; -> 0x080260a0 ; branch_target=0x080260a0
080260ae  a9f12009  sub.w	r9, r9, #32
080260b2  e1e7      b	#-62 ; -> 0x08026078 ; branch_target=0x08026078
080260b4  636b      ldr	r3, [r4, #52]
080260b6  43f00053  orr	r3, r3, #536870912
080260ba  6363      str	r3, [r4, #52]
080260bc  0120      movs	r0, #1
080260be  06b0      add	sp, #24
080260c0  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
080260c4  0123      movs	r3, #1
080260c6  4946      mov	r1, r9
080260c8  2068      ldr	r0, [r4]
080260ca  e362      str	r3, [r4, #44]
080260cc  02f0c2fa  bl	#9604 ; -> 0x08028654 ; branch_target=0x08028654
080260d0  cce7      b	#-104 ; -> 0x0802606c ; branch_target=0x0802606c
080260d2  636b      ldr	r3, [r4, #52]
080260d4  43f00063  orr	r3, r3, #134217728
080260d8  6363      str	r3, [r4, #52]
080260da  efe7      b	#-34 ; -> 0x080260bc ; branch_target=0x080260bc
080260dc  636b      ldr	r3, [r4, #52]
080260de  43f00073  orr	r3, r3, #33554432
080260e2  6363      str	r3, [r4, #52]
080260e4  eae7      b	#-44 ; -> 0x080260bc ; branch_target=0x080260bc
080260e6  334b      ldr	r3, [pc, #204] ; [0x080261b4] = 0x1fe00fff
080260e8  0121      movs	r1, #1
080260ea  9363      str	r3, [r2, #56]
080260ec  636b      ldr	r3, [r4, #52]
080260ee  0022      movs	r2, #0
080260f0  0343      orrs	r3, r0
080260f2  6363      str	r3, [r4, #52]
080260f4  84f83010  strb.w	r1, [r4, #48]
080260f8  e262      str	r2, [r4, #44]
080260fa  dfe7      b	#-66 ; -> 0x080260bc ; branch_target=0x080260bc
080260fc  d368      ldr	r3, [r2, #12]
080260fe  23f04003  bic	r3, r3, #64
08026102  d360      str	r3, [r2, #12]
08026104  2268      ldr	r2, [r4]
08026106  536b      ldr	r3, [r2, #52]
08026108  db05      lsls	r3, r3, #23
0802610a  01d5      bpl	#2 ; -> 0x08026110 ; branch_target=0x08026110
0802610c  012f      cmp	r7, #1
0802610e  24d8      bhi	#72 ; -> 0x0802615a ; branch_target=0x0802615a
08026110  536b      ldr	r3, [r2, #52]
08026112  13f00803  ands	r3, r3, #8
08026116  34d1      bne	#104 ; -> 0x08026182 ; branch_target=0x08026182
08026118  516b      ldr	r1, [r2, #52]
0802611a  11f00201  ands	r1, r1, #2
0802611e  25d1      bne	#74 ; -> 0x0802616c ; branch_target=0x0802616c
08026120  536b      ldr	r3, [r2, #52]
08026122  13f02003  ands	r3, r3, #32
08026126  38d0      beq	#112 ; -> 0x0802619a ; branch_target=0x0802619a
08026128  224b      ldr	r3, [pc, #136] ; [0x080261b4] = 0x1fe00fff
0802612a  0120      movs	r0, #1
0802612c  9363      str	r3, [r2, #56]
0802612e  636b      ldr	r3, [r4, #52]
08026130  43f02003  orr	r3, r3, #32
08026134  6363      str	r3, [r4, #52]
08026136  84f83000  strb.w	r0, [r4, #48]
0802613a  e162      str	r1, [r4, #44]
0802613c  bee7      b	#-132 ; -> 0x080260bc ; branch_target=0x080260bc
0802613e  2368      ldr	r3, [r4]
08026140  0121      movs	r1, #1
08026142  1c48      ldr	r0, [pc, #112] ; [0x080261b4] = 0x1fe00fff
08026144  0022      movs	r2, #0
08026146  9863      str	r0, [r3, #56]
08026148  0320      movs	r0, #3
0802614a  636b      ldr	r3, [r4, #52]
0802614c  43f00043  orr	r3, r3, #2147483648
08026150  6363      str	r3, [r4, #52]
08026152  84f83010  strb.w	r1, [r4, #48]
08026156  e262      str	r2, [r4, #44]
08026158  b1e7      b	#-158 ; -> 0x080260be ; branch_target=0x080260be
0802615a  a36b      ldr	r3, [r4, #56]
0802615c  032b      cmp	r3, #3
0802615e  d7d0      beq	#-82 ; -> 0x08026110 ; branch_target=0x08026110
08026160  1046      mov	r0, r2
08026162  02f027fd  bl	#10830 ; -> 0x08028bb4 ; branch_target=0x08028bb4
08026166  f8b9      cbnz	r0, #62 ; -> 0x080261a8 ; branch_target=0x080261a8
08026168  2268      ldr	r2, [r4]
0802616a  d1e7      b	#-94 ; -> 0x08026110 ; branch_target=0x08026110
0802616c  1148      ldr	r0, [pc, #68] ; [0x080261b4] = 0x1fe00fff
0802616e  0121      movs	r1, #1
08026170  9063      str	r0, [r2, #56]
08026172  626b      ldr	r2, [r4, #52]
08026174  42f00202  orr	r2, r2, #2
08026178  6263      str	r2, [r4, #52]
0802617a  84f83010  strb.w	r1, [r4, #48]
0802617e  e362      str	r3, [r4, #44]
08026180  9ce7      b	#-200 ; -> 0x080260bc ; branch_target=0x080260bc
08026182  0c4b      ldr	r3, [pc, #48] ; [0x080261b4] = 0x1fe00fff
08026184  0120      movs	r0, #1
08026186  0021      movs	r1, #0
08026188  9363      str	r3, [r2, #56]
0802618a  636b      ldr	r3, [r4, #52]
0802618c  43f00803  orr	r3, r3, #8
08026190  6363      str	r3, [r4, #52]
08026192  84f83000  strb.w	r0, [r4, #48]
08026196  e162      str	r1, [r4, #44]
08026198  90e7      b	#-224 ; -> 0x080260bc ; branch_target=0x080260bc
0802619a  074d      ldr	r5, [pc, #28] ; [0x080261b8] = 0x18000f3a
0802619c  0121      movs	r1, #1
0802619e  1846      mov	r0, r3
080261a0  9563      str	r5, [r2, #56]
080261a2  84f83010  strb.w	r1, [r4, #48]
080261a6  8ae7      b	#-236 ; -> 0x080260be ; branch_target=0x080260be
080261a8  2368      ldr	r3, [r4]
080261aa  0121      movs	r1, #1
080261ac  014a      ldr	r2, [pc, #4] ; [0x080261b4] = 0x1fe00fff
080261ae  9a63      str	r2, [r3, #56]
080261b0  9ce7      b	#-200 ; -> 0x080260ec ; branch_target=0x080260ec
