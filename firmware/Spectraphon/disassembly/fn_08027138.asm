; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08027138  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
0802713c  1d46      mov	r5, r3
0802713e  90f88030  ldrb.w	r3, [r0, #128]
08027142  089e      ldr	r6, [sp, #32]
08027144  012b      cmp	r3, #1
08027146  00f0a880  beq.w	#336 ; -> 0x0802729a ; branch_target=0x0802729a
0802714a  0123      movs	r3, #1
0802714c  0446      mov	r4, r0
0802714e  8a46      mov	r10, r1
08027150  9146      mov	r9, r2
08027152  80f88030  strb.w	r3, [r0, #128]
08027156  d0f80080  ldr.w	r8, [r0]
0802715a  f9f71ff9  bl	#-28098 ; -> 0x0802039c ; branch_target=0x0802039c
0802715e  94f88130  ldrb.w	r3, [r4, #129]
08027162  0746      mov	r7, r0
08027164  6268      ldr	r2, [r4, #4]
08027166  012b      cmp	r3, #1
08027168  d9b2      uxtb	r1, r3
0802716a  0ad0      beq	#20 ; -> 0x08027182 ; branch_target=0x08027182
0802716c  b2f5800f  cmp.w	r2, #4194304
08027170  40f09080  bne.w	#288 ; -> 0x08027294 ; branch_target=0x08027294
08027174  a368      ldr	r3, [r4, #8]
08027176  002b      cmp	r3, #0
08027178  40f08c80  bne.w	#280 ; -> 0x08027294 ; branch_target=0x08027294
0802717c  0429      cmp	r1, #4
0802717e  40f08980  bne.w	#274 ; -> 0x08027294 ; branch_target=0x08027294
08027182  b9f1000f  cmp.w	r9, #0
08027186  18bf      it	ne
08027188  baf1000f  cmpne.w	r10, #0
0802718c  00f08880  beq.w	#272 ; -> 0x080272a0 ; branch_target=0x080272a0
08027190  002d      cmp	r5, #0
08027192  00f08580  beq.w	#266 ; -> 0x080272a0 ; branch_target=0x080272a0
08027196  94f88130  ldrb.w	r3, [r4, #129]
0802719a  042b      cmp	r3, #4
0802719c  02d0      beq	#4 ; -> 0x080271a4 ; branch_target=0x080271a4
0802719e  0523      movs	r3, #5
080271a0  84f88130  strb.w	r3, [r4, #129]
080271a4  0023      movs	r3, #0
080271a6  2268      ldr	r2, [r4]
080271a8  c4f86490  str.w	r9, [r4, #100]
080271ac  c4f88430  str.w	r3, [r4, #132]
080271b0  2367      str	r3, [r4, #112]
080271b2  a4f86a50  strh.w	r5, [r4, #106]
080271b6  6367      str	r3, [r4, #116]
080271b8  a3f58033  sub.w	r3, r3, #65536
080271bc  c4f85ca0  str.w	r10, [r4, #92]
080271c0  a4f86850  strh.w	r5, [r4, #104]
080271c4  a4f86050  strh.w	r5, [r4, #96]
080271c8  a4f86250  strh.w	r5, [r4, #98]
080271cc  5168      ldr	r1, [r2, #4]
080271ce  0b40      ands	r3, r1
080271d0  2b43      orrs	r3, r5
080271d2  5360      str	r3, [r2, #4]
080271d4  2268      ldr	r2, [r4]
080271d6  1368      ldr	r3, [r2]
080271d8  43f00103  orr	r3, r3, #1
080271dc  1360      str	r3, [r2]
080271de  6368      ldr	r3, [r4, #4]
080271e0  b3f5800f  cmp.w	r3, #4194304
080271e4  04d1      bne	#8 ; -> 0x080271f0 ; branch_target=0x080271f0
080271e6  2268      ldr	r2, [r4]
080271e8  1368      ldr	r3, [r2]
080271ea  43f40073  orr	r3, r3, #512
080271ee  1360      str	r3, [r2]
080271f0  e368      ldr	r3, [r4, #12]
080271f2  0f2b      cmp	r3, #15
080271f4  74d9      bls	#232 ; -> 0x080272e0 ; branch_target=0x080272e0
080271f6  002e      cmp	r6, #0
080271f8  00f07e81  beq.w	#764 ; -> 0x080274f8 ; branch_target=0x080274f8
080271fc  a846      mov	r8, r5
080271fe  48f20809  movw	r9, #32776
08027202  45ea0803  orr.w	r3, r5, r8
08027206  2268      ldr	r2, [r4]
08027208  9bb2      uxth	r3, r3
0802720a  1146      mov	r1, r2
0802720c  002b      cmp	r3, #0
0802720e  55d0      beq	#170 ; -> 0x080272bc ; branch_target=0x080272bc
08027210  5369      ldr	r3, [r2, #20]
08027212  9907      lsls	r1, r3, #30
08027214  10d5      bpl	#32 ; -> 0x08027238 ; branch_target=0x08027238
08027216  7db1      cbz	r5, #30 ; -> 0x08027238 ; branch_target=0x08027238
08027218  e36d      ldr	r3, [r4, #92]
0802721a  1b68      ldr	r3, [r3]
0802721c  1362      str	r3, [r2, #32]
0802721e  b4f86230  ldrh.w	r3, [r4, #98]
08027222  2268      ldr	r2, [r4]
08027224  013b      subs	r3, #1
08027226  9bb2      uxth	r3, r3
08027228  a4f86230  strh.w	r3, [r4, #98]
0802722c  e36d      ldr	r3, [r4, #92]
0802722e  b4f86250  ldrh.w	r5, [r4, #98]
08027232  0433      adds	r3, #4
08027234  adb2      uxth	r5, r5
08027236  e365      str	r3, [r4, #92]
08027238  5369      ldr	r3, [r2, #20]
0802723a  13ea090f  tst.w	r3, r9
0802723e  12d0      beq	#36 ; -> 0x08027266 ; branch_target=0x08027266
08027240  b8f1000f  cmp.w	r8, #0
08027244  0fd0      beq	#30 ; -> 0x08027266 ; branch_target=0x08027266
08027246  126b      ldr	r2, [r2, #48]
08027248  636e      ldr	r3, [r4, #100]
0802724a  1a60      str	r2, [r3]
0802724c  b4f86a30  ldrh.w	r3, [r4, #106]
08027250  013b      subs	r3, #1
08027252  9bb2      uxth	r3, r3
08027254  a4f86a30  strh.w	r3, [r4, #106]
08027258  636e      ldr	r3, [r4, #100]
0802725a  b4f86a80  ldrh.w	r8, [r4, #106]
0802725e  0433      adds	r3, #4
08027260  1ffa88f8  uxth.w	r8, r8
08027264  6366      str	r3, [r4, #100]
08027266  f9f799f8  bl	#-28366 ; -> 0x0802039c ; branch_target=0x0802039c
0802726a  c01b      subs	r0, r0, r7
0802726c  8642      cmp	r6, r0
0802726e  c8d8      bhi	#-112 ; -> 0x08027202 ; branch_target=0x08027202
08027270  731c      adds	r3, r6, #1
08027272  c6d0      beq	#-116 ; -> 0x08027202 ; branch_target=0x08027202
08027274  2046      mov	r0, r4
08027276  fff705fd  bl	#-1526 ; -> 0x08026c84 ; branch_target=0x08026c84
0802727a  d4f88430  ldr.w	r3, [r4, #132]
0802727e  0021      movs	r1, #0
08027280  0122      movs	r2, #1
08027282  43f48073  orr	r3, r3, #256
08027286  84f88010  strb.w	r1, [r4, #128]
0802728a  c4f88430  str.w	r3, [r4, #132]
0802728e  84f88120  strb.w	r2, [r4, #129]
08027292  08e0      b	#16 ; -> 0x080272a6 ; branch_target=0x080272a6
08027294  0023      movs	r3, #0
08027296  84f88030  strb.w	r3, [r4, #128]
0802729a  0220      movs	r0, #2
0802729c  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
080272a0  0023      movs	r3, #0
080272a2  84f88030  strb.w	r3, [r4, #128]
080272a6  0120      movs	r0, #1
080272a8  f8e7      b	#-16 ; -> 0x0802729c ; branch_target=0x0802729c
080272aa  f9f777f8  bl	#-28434 ; -> 0x0802039c ; branch_target=0x0802039c
080272ae  c01b      subs	r0, r0, r7
080272b0  b042      cmp	r0, r6
080272b2  02d3      blo	#4 ; -> 0x080272ba ; branch_target=0x080272ba
080272b4  721c      adds	r2, r6, #1
080272b6  40f01881  bne.w	#560 ; -> 0x080274ea ; branch_target=0x080274ea
080272ba  2168      ldr	r1, [r4]
080272bc  4b69      ldr	r3, [r1, #20]
080272be  1907      lsls	r1, r3, #28
080272c0  f3d5      bpl	#-26 ; -> 0x080272aa ; branch_target=0x080272aa
080272c2  2046      mov	r0, r4
080272c4  fff7defc  bl	#-1604 ; -> 0x08026c84 ; branch_target=0x08026c84
080272c8  0122      movs	r2, #1
080272ca  0023      movs	r3, #0
080272cc  84f88120  strb.w	r2, [r4, #129]
080272d0  d4f88400  ldr.w	r0, [r4, #132]
080272d4  84f88030  strb.w	r3, [r4, #128]
080272d8  c01a      subs	r0, r0, r3
080272da  18bf      it	ne
080272dc  0120      movne	r0, #1
080272de  dde7      b	#-70 ; -> 0x0802729c ; branch_target=0x0802729c
080272e0  072b      cmp	r3, #7
080272e2  a946      mov	r9, r5
080272e4  79d9      bls	#242 ; -> 0x080273da ; branch_target=0x080273da
080272e6  45ea0902  orr.w	r2, r5, r9
080272ea  2368      ldr	r3, [r4]
080272ec  92b2      uxth	r2, r2
080272ee  1946      mov	r1, r3
080272f0  002a      cmp	r2, #0
080272f2  00f0f180  beq.w	#482 ; -> 0x080274d8 ; branch_target=0x080274d8
080272f6  5a69      ldr	r2, [r3, #20]
080272f8  9007      lsls	r0, r2, #30
080272fa  17d5      bpl	#46 ; -> 0x0802732c ; branch_target=0x0802732c
080272fc  b5b1      cbz	r5, #44 ; -> 0x0802732c ; branch_target=0x0802732c
080272fe  012d      cmp	r5, #1
08027300  e26d      ldr	r2, [r4, #92]
08027302  03d0      beq	#6 ; -> 0x0802730c ; branch_target=0x0802730c
08027304  e16b      ldr	r1, [r4, #60]
08027306  0029      cmp	r1, #0
08027308  40f0c580  bne.w	#394 ; -> 0x08027496 ; branch_target=0x08027496
0802730c  1388      ldrh	r3, [r2]
0802730e  a8f82030  strh.w	r3, [r8, #32]
08027312  b4f86230  ldrh.w	r3, [r4, #98]
08027316  013b      subs	r3, #1
08027318  9bb2      uxth	r3, r3
0802731a  a4f86230  strh.w	r3, [r4, #98]
0802731e  e36d      ldr	r3, [r4, #92]
08027320  b4f86250  ldrh.w	r5, [r4, #98]
08027324  0233      adds	r3, #2
08027326  adb2      uxth	r5, r5
08027328  e365      str	r3, [r4, #92]
0802732a  2368      ldr	r3, [r4]
0802732c  5a69      ldr	r2, [r3, #20]
0802732e  12f4604f  tst.w	r2, #57344
08027332  16d0      beq	#44 ; -> 0x08027362 ; branch_target=0x08027362
08027334  b9f1000f  cmp.w	r9, #0
08027338  13d0      beq	#38 ; -> 0x08027362 ; branch_target=0x08027362
0802733a  5969      ldr	r1, [r3, #20]
0802733c  626e      ldr	r2, [r4, #100]
0802733e  0904      lsls	r1, r1, #16
08027340  40f19880  bpl.w	#304 ; -> 0x08027474 ; branch_target=0x08027474
08027344  1b6b      ldr	r3, [r3, #48]
08027346  1360      str	r3, [r2]
08027348  b4f86a30  ldrh.w	r3, [r4, #106]
0802734c  023b      subs	r3, #2
0802734e  9bb2      uxth	r3, r3
08027350  a4f86a30  strh.w	r3, [r4, #106]
08027354  636e      ldr	r3, [r4, #100]
08027356  b4f86a90  ldrh.w	r9, [r4, #106]
0802735a  0433      adds	r3, #4
0802735c  1ffa89f9  uxth.w	r9, r9
08027360  6366      str	r3, [r4, #100]
08027362  f9f71bf8  bl	#-28618 ; -> 0x0802039c ; branch_target=0x0802039c
08027366  c01b      subs	r0, r0, r7
08027368  b042      cmp	r0, r6
0802736a  bcd3      blo	#-136 ; -> 0x080272e6 ; branch_target=0x080272e6
0802736c  731c      adds	r3, r6, #1
0802736e  bad0      beq	#-140 ; -> 0x080272e6 ; branch_target=0x080272e6
08027370  80e7      b	#-256 ; -> 0x08027274 ; branch_target=0x08027274
08027372  012d      cmp	r5, #1
08027374  53d1      bne	#166 ; -> 0x0802741e ; branch_target=0x0802741e
08027376  1378      ldrb	r3, [r2]
08027378  81f82030  strb.w	r3, [r1, #32]
0802737c  b4f86230  ldrh.w	r3, [r4, #98]
08027380  013b      subs	r3, #1
08027382  9bb2      uxth	r3, r3
08027384  a4f86230  strh.w	r3, [r4, #98]
08027388  e36d      ldr	r3, [r4, #92]
0802738a  b4f86250  ldrh.w	r5, [r4, #98]
0802738e  0133      adds	r3, #1
08027390  adb2      uxth	r5, r5
08027392  e365      str	r3, [r4, #92]
08027394  2368      ldr	r3, [r4]
08027396  5a69      ldr	r2, [r3, #20]
08027398  12f4604f  tst.w	r2, #57344
0802739c  15d0      beq	#42 ; -> 0x080273ca ; branch_target=0x080273ca
0802739e  b9f1000f  cmp.w	r9, #0
080273a2  12d0      beq	#36 ; -> 0x080273ca ; branch_target=0x080273ca
080273a4  5969      ldr	r1, [r3, #20]
080273a6  626e      ldr	r2, [r4, #100]
080273a8  0904      lsls	r1, r1, #16
080273aa  4cd5      bpl	#152 ; -> 0x08027446 ; branch_target=0x08027446
080273ac  1b6b      ldr	r3, [r3, #48]
080273ae  1360      str	r3, [r2]
080273b0  b4f86a30  ldrh.w	r3, [r4, #106]
080273b4  043b      subs	r3, #4
080273b6  9bb2      uxth	r3, r3
080273b8  a4f86a30  strh.w	r3, [r4, #106]
080273bc  636e      ldr	r3, [r4, #100]
080273be  b4f86a90  ldrh.w	r9, [r4, #106]
080273c2  0433      adds	r3, #4
080273c4  1ffa89f9  uxth.w	r9, r9
080273c8  6366      str	r3, [r4, #100]
080273ca  f8f7e7ff  bl	#-28722 ; -> 0x0802039c ; branch_target=0x0802039c
080273ce  c01b      subs	r0, r0, r7
080273d0  b042      cmp	r0, r6
080273d2  02d3      blo	#4 ; -> 0x080273da ; branch_target=0x080273da
080273d4  701c      adds	r0, r6, #1
080273d6  7ff44daf  bne.w	#-358 ; -> 0x08027274 ; branch_target=0x08027274
080273da  45ea0902  orr.w	r2, r5, r9
080273de  2368      ldr	r3, [r4]
080273e0  92b2      uxth	r2, r2
080273e2  1946      mov	r1, r3
080273e4  002a      cmp	r2, #0
080273e6  77d0      beq	#238 ; -> 0x080274d8 ; branch_target=0x080274d8
080273e8  5a69      ldr	r2, [r3, #20]
080273ea  9007      lsls	r0, r2, #30
080273ec  d3d5      bpl	#-90 ; -> 0x08027396 ; branch_target=0x08027396
080273ee  002d      cmp	r5, #0
080273f0  d1d0      beq	#-94 ; -> 0x08027396 ; branch_target=0x08027396
080273f2  032d      cmp	r5, #3
080273f4  e26d      ldr	r2, [r4, #92]
080273f6  bcd9      bls	#-136 ; -> 0x08027372 ; branch_target=0x08027372
080273f8  e06b      ldr	r0, [r4, #60]
080273fa  4028      cmp	r0, #64
080273fc  10d9      bls	#32 ; -> 0x08027420 ; branch_target=0x08027420
080273fe  1268      ldr	r2, [r2]
08027400  1a62      str	r2, [r3, #32]
08027402  b4f86230  ldrh.w	r3, [r4, #98]
08027406  043b      subs	r3, #4
08027408  9bb2      uxth	r3, r3
0802740a  a4f86230  strh.w	r3, [r4, #98]
0802740e  e36d      ldr	r3, [r4, #92]
08027410  b4f86250  ldrh.w	r5, [r4, #98]
08027414  0433      adds	r3, #4
08027416  adb2      uxth	r5, r5
08027418  e365      str	r3, [r4, #92]
0802741a  2368      ldr	r3, [r4]
0802741c  bbe7      b	#-138 ; -> 0x08027396 ; branch_target=0x08027396
0802741e  e06b      ldr	r0, [r4, #60]
08027420  0028      cmp	r0, #0
08027422  a8d0      beq	#-176 ; -> 0x08027376 ; branch_target=0x08027376
08027424  1388      ldrh	r3, [r2]
08027426  a8f82030  strh.w	r3, [r8, #32]
0802742a  b4f86230  ldrh.w	r3, [r4, #98]
0802742e  023b      subs	r3, #2
08027430  9bb2      uxth	r3, r3
08027432  a4f86230  strh.w	r3, [r4, #98]
08027436  e36d      ldr	r3, [r4, #92]
08027438  b4f86250  ldrh.w	r5, [r4, #98]
0802743c  0233      adds	r3, #2
0802743e  adb2      uxth	r5, r5
08027440  e365      str	r3, [r4, #92]
08027442  2368      ldr	r3, [r4]
08027444  a7e7      b	#-178 ; -> 0x08027396 ; branch_target=0x08027396
08027446  5969      ldr	r1, [r3, #20]
08027448  01f4c041  and	r1, r1, #24576
0802744c  b1f5005f  cmp.w	r1, #8192
08027450  31d9      bls	#98 ; -> 0x080274b6 ; branch_target=0x080274b6
08027452  b8f83030  ldrh.w	r3, [r8, #48]
08027456  1380      strh	r3, [r2]
08027458  b4f86a30  ldrh.w	r3, [r4, #106]
0802745c  023b      subs	r3, #2
0802745e  9bb2      uxth	r3, r3
08027460  a4f86a30  strh.w	r3, [r4, #106]
08027464  636e      ldr	r3, [r4, #100]
08027466  b4f86a90  ldrh.w	r9, [r4, #106]
0802746a  0233      adds	r3, #2
0802746c  1ffa89f9  uxth.w	r9, r9
08027470  6366      str	r3, [r4, #100]
08027472  aae7      b	#-172 ; -> 0x080273ca ; branch_target=0x080273ca
08027474  b8f83030  ldrh.w	r3, [r8, #48]
08027478  1380      strh	r3, [r2]
0802747a  b4f86a30  ldrh.w	r3, [r4, #106]
0802747e  013b      subs	r3, #1
08027480  9bb2      uxth	r3, r3
08027482  a4f86a30  strh.w	r3, [r4, #106]
08027486  636e      ldr	r3, [r4, #100]
08027488  b4f86a90  ldrh.w	r9, [r4, #106]
0802748c  0233      adds	r3, #2
0802748e  1ffa89f9  uxth.w	r9, r9
08027492  6366      str	r3, [r4, #100]
08027494  65e7      b	#-310 ; -> 0x08027362 ; branch_target=0x08027362
08027496  1268      ldr	r2, [r2]
08027498  1a62      str	r2, [r3, #32]
0802749a  b4f86230  ldrh.w	r3, [r4, #98]
0802749e  023b      subs	r3, #2
080274a0  9bb2      uxth	r3, r3
080274a2  a4f86230  strh.w	r3, [r4, #98]
080274a6  e36d      ldr	r3, [r4, #92]
080274a8  b4f86250  ldrh.w	r5, [r4, #98]
080274ac  0433      adds	r3, #4
080274ae  adb2      uxth	r5, r5
080274b0  e365      str	r3, [r4, #92]
080274b2  2368      ldr	r3, [r4]
080274b4  3ae7      b	#-396 ; -> 0x0802732c ; branch_target=0x0802732c
080274b6  93f83030  ldrb.w	r3, [r3, #48]
080274ba  1370      strb	r3, [r2]
080274bc  b4f86a30  ldrh.w	r3, [r4, #106]
080274c0  013b      subs	r3, #1
080274c2  9bb2      uxth	r3, r3
080274c4  a4f86a30  strh.w	r3, [r4, #106]
080274c8  636e      ldr	r3, [r4, #100]
080274ca  b4f86a90  ldrh.w	r9, [r4, #106]
080274ce  0133      adds	r3, #1
080274d0  1ffa89f9  uxth.w	r9, r9
080274d4  6366      str	r3, [r4, #100]
080274d6  78e7      b	#-272 ; -> 0x080273ca ; branch_target=0x080273ca
080274d8  002e      cmp	r6, #0
080274da  7ff4efae  bne.w	#-546 ; -> 0x080272bc ; branch_target=0x080272bc
080274de  4b69      ldr	r3, [r1, #20]
080274e0  1b07      lsls	r3, r3, #28
080274e2  3ff5eeae  bmi.w	#-548 ; -> 0x080272c2 ; branch_target=0x080272c2
080274e6  f8f759ff  bl	#-29006 ; -> 0x0802039c ; branch_target=0x0802039c
080274ea  d4f88430  ldr.w	r3, [r4, #132]
080274ee  43f02003  orr	r3, r3, #32
080274f2  c4f88430  str.w	r3, [r4, #132]
080274f6  e4e6      b	#-568 ; -> 0x080272c2 ; branch_target=0x080272c2
080274f8  2368      ldr	r3, [r4]
080274fa  5a69      ldr	r2, [r3, #20]
080274fc  9507      lsls	r5, r2, #30
080274fe  0ed5      bpl	#28 ; -> 0x0802751e ; branch_target=0x0802751e
08027500  e26d      ldr	r2, [r4, #92]
08027502  1268      ldr	r2, [r2]
08027504  1a62      str	r2, [r3, #32]
08027506  b4f86220  ldrh.w	r2, [r4, #98]
0802750a  e16d      ldr	r1, [r4, #92]
0802750c  013a      subs	r2, #1
0802750e  2368      ldr	r3, [r4]
08027510  0431      adds	r1, #4
08027512  92b2      uxth	r2, r2
08027514  e165      str	r1, [r4, #92]
08027516  a4f86220  strh.w	r2, [r4, #98]
0802751a  b4f86220  ldrh.w	r2, [r4, #98]
0802751e  5969      ldr	r1, [r3, #20]
08027520  48f20802  movw	r2, #32776
08027524  1142      tst	r1, r2
08027526  0dd0      beq	#26 ; -> 0x08027544 ; branch_target=0x08027544
08027528  1a6b      ldr	r2, [r3, #48]
0802752a  636e      ldr	r3, [r4, #100]
0802752c  1a60      str	r2, [r3]
0802752e  b4f86a30  ldrh.w	r3, [r4, #106]
08027532  626e      ldr	r2, [r4, #100]
08027534  013b      subs	r3, #1
08027536  0432      adds	r2, #4
08027538  9bb2      uxth	r3, r3
0802753a  6266      str	r2, [r4, #100]
0802753c  a4f86a30  strh.w	r3, [r4, #106]
08027540  b4f86a30  ldrh.w	r3, [r4, #106]
08027544  f8f72aff  bl	#-29100 ; -> 0x0802039c ; branch_target=0x0802039c
08027548  94e6      b	#-728 ; -> 0x08027274 ; branch_target=0x08027274
