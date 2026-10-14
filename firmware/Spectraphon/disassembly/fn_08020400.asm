; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020400  f8b5      push	{r3, r4, r5, r6, r7, lr}
08020402  8e4a      ldr	r2, [pc, #568] ; [0x0802063c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020404  0446      mov	r4, r0
08020406  0368      ldr	r3, [r0]
08020408  9342      cmp	r3, r2
0802040a  1e68      ldr	r6, [r3]
0802040c  5f68      ldr	r7, [r3, #4]
0802040e  00f09b80  beq.w	#310 ; -> 0x08020548 ; branch_target=0x08020548
08020412  02f58072  add.w	r2, r2, #256
08020416  9342      cmp	r3, r2
08020418  00f09680  beq.w	#300 ; -> 0x08020548 ; branch_target=0x08020548
0802041c  884b      ldr	r3, [pc, #544] ; [0x08020640] = 0x58026300
0802041e  9d68      ldr	r5, [r3, #8]
08020420  b107      lsls	r1, r6, #30
08020422  05f01f05  and	r5, r5, #31
08020426  02d5      bpl	#4 ; -> 0x0802042e ; branch_target=0x0802042e
08020428  ba07      lsls	r2, r7, #30
0802042a  00f1b180  bmi.w	#354 ; -> 0x08020590 ; branch_target=0x08020590
0802042e  7007      lsls	r0, r6, #29
08020430  7ed5      bpl	#252 ; -> 0x08020530 ; branch_target=0x08020530
08020432  7907      lsls	r1, r7, #29
08020434  7cd5      bpl	#248 ; -> 0x08020530 ; branch_target=0x08020530
08020436  636d      ldr	r3, [r4, #84]
08020438  d806      lsls	r0, r3, #27
0802043a  03d4      bmi	#6 ; -> 0x08020444 ; branch_target=0x08020444
0802043c  636d      ldr	r3, [r4, #84]
0802043e  43f40073  orr	r3, r3, #512
08020442  6365      str	r3, [r4, #84]
08020444  2368      ldr	r3, [r4]
08020446  da68      ldr	r2, [r3, #12]
08020448  12f4406f  tst.w	r2, #3072
0802044c  1cd1      bne	#56 ; -> 0x08020488 ; branch_target=0x08020488
0802044e  7d4a      ldr	r2, [pc, #500] ; [0x08020644] = 0x40022100 / f32_bits_interpretation=2.03326416
08020450  9342      cmp	r3, r2
08020452  00f0e680  beq.w	#460 ; -> 0x08020622 ; branch_target=0x08020622
08020456  da68      ldr	r2, [r3, #12]
08020458  9204      lsls	r2, r2, #18
0802045a  15d4      bmi	#42 ; -> 0x08020488 ; branch_target=0x08020488
0802045c  1a68      ldr	r2, [r3]
0802045e  1007      lsls	r0, r2, #28
08020460  12d5      bpl	#36 ; -> 0x08020488 ; branch_target=0x08020488
08020462  9a68      ldr	r2, [r3, #8]
08020464  5107      lsls	r1, r2, #29
08020466  00f1fc80  bmi.w	#504 ; -> 0x08020662 ; branch_target=0x08020662
0802046a  5a68      ldr	r2, [r3, #4]
0802046c  22f00c02  bic	r2, r2, #12
08020470  5a60      str	r2, [r3, #4]
08020472  636d      ldr	r3, [r4, #84]
08020474  23f48073  bic	r3, r3, #256
08020478  6365      str	r3, [r4, #84]
0802047a  636d      ldr	r3, [r4, #84]
0802047c  da04      lsls	r2, r3, #19
0802047e  03d4      bmi	#6 ; -> 0x08020488 ; branch_target=0x08020488
08020480  636d      ldr	r3, [r4, #84]
08020482  43f00103  orr	r3, r3, #1
08020486  6365      str	r3, [r4, #84]
08020488  2046      mov	r0, r4
0802048a  12f0ebf8  bl	#74198 ; -> 0x08032664 ; branch_target=0x08032664
0802048e  2368      ldr	r3, [r4]
08020490  0c22      movs	r2, #12
08020492  1a60      str	r2, [r3]
08020494  b306      lsls	r3, r6, #26
08020496  52d5      bpl	#164 ; -> 0x0802053e ; branch_target=0x0802053e
08020498  b806      lsls	r0, r7, #26
0802049a  50d5      bpl	#160 ; -> 0x0802053e ; branch_target=0x0802053e
0802049c  636d      ldr	r3, [r4, #84]
0802049e  db06      lsls	r3, r3, #27
080204a0  03d4      bmi	#6 ; -> 0x080204aa ; branch_target=0x080204aa
080204a2  636d      ldr	r3, [r4, #84]
080204a4  43f40053  orr	r3, r3, #8192
080204a8  6365      str	r3, [r4, #84]
080204aa  2368      ldr	r3, [r4]
080204ac  6548      ldr	r0, [pc, #404] ; [0x08020644] = 0x40022100 / f32_bits_interpretation=2.03326416
080204ae  da6c      ldr	r2, [r3, #76]
080204b0  8342      cmp	r3, r0
080204b2  d968      ldr	r1, [r3, #12]
080204b4  02f4c072  and	r2, r2, #384
080204b8  78d0      beq	#240 ; -> 0x080205ac ; branch_target=0x080205ac
080204ba  d868      ldr	r0, [r3, #12]
080204bc  2ab1      cbz	r2, #10 ; -> 0x080204ca ; branch_target=0x080204ca
080204be  624a      ldr	r2, [pc, #392] ; [0x08020648] = 0x02002000
080204c0  01f44061  and	r1, r1, #3072
080204c4  0240      ands	r2, r0
080204c6  1143      orrs	r1, r2
080204c8  17d1      bne	#46 ; -> 0x080204fa ; branch_target=0x080204fa
080204ca  1a68      ldr	r2, [r3]
080204cc  5206      lsls	r2, r2, #25
080204ce  14d5      bpl	#40 ; -> 0x080204fa ; branch_target=0x080204fa
080204d0  8102      lsls	r1, r0, #10
080204d2  12d4      bmi	#36 ; -> 0x080204fa ; branch_target=0x080204fa
080204d4  9a68      ldr	r2, [r3, #8]
080204d6  1207      lsls	r2, r2, #28
080204d8  00f1ba80  bmi.w	#372 ; -> 0x08020650 ; branch_target=0x08020650
080204dc  5a68      ldr	r2, [r3, #4]
080204de  22f06002  bic	r2, r2, #96
080204e2  5a60      str	r2, [r3, #4]
080204e4  636d      ldr	r3, [r4, #84]
080204e6  23f48053  bic	r3, r3, #4096
080204ea  6365      str	r3, [r4, #84]
080204ec  636d      ldr	r3, [r4, #84]
080204ee  d805      lsls	r0, r3, #23
080204f0  03d4      bmi	#6 ; -> 0x080204fa ; branch_target=0x080204fa
080204f2  636d      ldr	r3, [r4, #84]
080204f4  43f00103  orr	r3, r3, #1
080204f8  6365      str	r3, [r4, #84]
080204fa  2046      mov	r0, r4
080204fc  00f058ff  bl	#3760 ; -> 0x080213b0 ; branch_target=0x080213b0
08020500  2368      ldr	r3, [r4]
08020502  6022      movs	r2, #96
08020504  1a60      str	r2, [r3]
08020506  3106      lsls	r1, r6, #24
08020508  01d5      bpl	#2 ; -> 0x0802050e ; branch_target=0x0802050e
0802050a  3a06      lsls	r2, r7, #24
0802050c  61d4      bmi	#194 ; -> 0x080205d2 ; branch_target=0x080205d2
0802050e  f305      lsls	r3, r6, #23
08020510  01d5      bpl	#2 ; -> 0x08020516 ; branch_target=0x08020516
08020512  f805      lsls	r0, r7, #23
08020514  68d4      bmi	#208 ; -> 0x080205e8 ; branch_target=0x080205e8
08020516  b105      lsls	r1, r6, #22
08020518  01d5      bpl	#2 ; -> 0x0802051e ; branch_target=0x0802051e
0802051a  ba05      lsls	r2, r7, #22
0802051c  4dd4      bmi	#154 ; -> 0x080205ba ; branch_target=0x080205ba
0802051e  f306      lsls	r3, r6, #27
08020520  01d5      bpl	#2 ; -> 0x08020526 ; branch_target=0x08020526
08020522  f806      lsls	r0, r7, #27
08020524  12d4      bmi	#36 ; -> 0x0802054c ; branch_target=0x0802054c
08020526  7205      lsls	r2, r6, #21
08020528  01d5      bpl	#2 ; -> 0x0802052e ; branch_target=0x0802052e
0802052a  7b05      lsls	r3, r7, #21
0802052c  68d4      bmi	#208 ; -> 0x08020600 ; branch_target=0x08020600
0802052e  f8bd      pop	{r3, r4, r5, r6, r7, pc}
08020530  3207      lsls	r2, r6, #28
08020532  afd5      bpl	#-162 ; -> 0x08020494 ; branch_target=0x08020494
08020534  3b07      lsls	r3, r7, #28
08020536  3ff57eaf  bmi.w	#-260 ; -> 0x08020436 ; branch_target=0x08020436
0802053a  b306      lsls	r3, r6, #26
0802053c  acd4      bmi	#-168 ; -> 0x08020498 ; branch_target=0x08020498
0802053e  7106      lsls	r1, r6, #25
08020540  e1d5      bpl	#-62 ; -> 0x08020506 ; branch_target=0x08020506
08020542  7a06      lsls	r2, r7, #25
08020544  dfd5      bpl	#-66 ; -> 0x08020506 ; branch_target=0x08020506
08020546  a9e7      b	#-174 ; -> 0x0802049c ; branch_target=0x0802049c
08020548  404b      ldr	r3, [pc, #256] ; [0x0802064c] = 0x40022300 / f32_bits_interpretation=2.03338623
0802054a  68e7      b	#-304 ; -> 0x0802041e ; branch_target=0x0802041e
0802054c  236b      ldr	r3, [r4, #48]
0802054e  83b1      cbz	r3, #32 ; -> 0x08020572 ; branch_target=0x08020572
08020550  2368      ldr	r3, [r4]
08020552  002d      cmp	r5, #0
08020554  6ed0      beq	#220 ; -> 0x08020634 ; branch_target=0x08020634
08020556  394a      ldr	r2, [pc, #228] ; [0x0802063c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020558  9342      cmp	r3, r2
0802055a  00f08b80  beq.w	#278 ; -> 0x08020674 ; branch_target=0x08020674
0802055e  02f58072  add.w	r2, r2, #256
08020562  9342      cmp	r3, r2
08020564  00f08680  beq.w	#268 ; -> 0x08020674 ; branch_target=0x08020674
08020568  354a      ldr	r2, [pc, #212] ; [0x08020640] = 0x58026300
0802056a  9268      ldr	r2, [r2, #8]
0802056c  12f4404f  tst.w	r2, #49152
08020570  0bd0      beq	#22 ; -> 0x0802058a ; branch_target=0x0802058a
08020572  636d      ldr	r3, [r4, #84]
08020574  2046      mov	r0, r4
08020576  43f48063  orr	r3, r3, #1024
0802057a  6365      str	r3, [r4, #84]
0802057c  a36d      ldr	r3, [r4, #88]
0802057e  43f00203  orr	r3, r3, #2
08020582  a365      str	r3, [r4, #88]
08020584  fff73aff  bl	#-396 ; -> 0x080203fc ; branch_target=0x080203fc
08020588  2368      ldr	r3, [r4]
0802058a  1022      movs	r2, #16
0802058c  1a60      str	r2, [r3]
0802058e  cae7      b	#-108 ; -> 0x08020526 ; branch_target=0x08020526
08020590  636d      ldr	r3, [r4, #84]
08020592  db06      lsls	r3, r3, #27
08020594  03d4      bmi	#6 ; -> 0x0802059e ; branch_target=0x0802059e
08020596  636d      ldr	r3, [r4, #84]
08020598  43f40063  orr	r3, r3, #2048
0802059c  6365      str	r3, [r4, #84]
0802059e  2046      mov	r0, r4
080205a0  00f00eff  bl	#3612 ; -> 0x080213c0 ; branch_target=0x080213c0
080205a4  2368      ldr	r3, [r4]
080205a6  0222      movs	r2, #2
080205a8  1a60      str	r2, [r3]
080205aa  40e7      b	#-384 ; -> 0x0802042e ; branch_target=0x0802042e
080205ac  c120      movs	r0, #193
080205ae  e840      lsrs	r0, r5
080205b0  c007      lsls	r0, r0, #31
080205b2  82d4      bmi	#-252 ; -> 0x080204ba ; branch_target=0x080204ba
080205b4  2148      ldr	r0, [pc, #132] ; [0x0802063c] = 0x40022000 / f32_bits_interpretation=2.033203125
080205b6  c068      ldr	r0, [r0, #12]
080205b8  80e7      b	#-256 ; -> 0x080204bc ; branch_target=0x080204bc
080205ba  636d      ldr	r3, [r4, #84]
080205bc  2046      mov	r0, r4
080205be  43f48023  orr	r3, r3, #262144
080205c2  6365      str	r3, [r4, #84]
080205c4  00f0fafe  bl	#3572 ; -> 0x080213bc ; branch_target=0x080213bc
080205c8  2368      ldr	r3, [r4]
080205ca  4ff40072  mov.w	r2, #512
080205ce  1a60      str	r2, [r3]
080205d0  a5e7      b	#-182 ; -> 0x0802051e ; branch_target=0x0802051e
080205d2  636d      ldr	r3, [r4, #84]
080205d4  2046      mov	r0, r4
080205d6  43f48033  orr	r3, r3, #65536
080205da  6365      str	r3, [r4, #84]
080205dc  fff70cff  bl	#-488 ; -> 0x080203f8 ; branch_target=0x080203f8
080205e0  2368      ldr	r3, [r4]
080205e2  8022      movs	r2, #128
080205e4  1a60      str	r2, [r3]
080205e6  92e7      b	#-220 ; -> 0x0802050e ; branch_target=0x0802050e
080205e8  636d      ldr	r3, [r4, #84]
080205ea  2046      mov	r0, r4
080205ec  43f40033  orr	r3, r3, #131072
080205f0  6365      str	r3, [r4, #84]
080205f2  00f0e1fe  bl	#3522 ; -> 0x080213b8 ; branch_target=0x080213b8
080205f6  2368      ldr	r3, [r4]
080205f8  4ff48072  mov.w	r2, #256
080205fc  1a60      str	r2, [r3]
080205fe  8ae7      b	#-236 ; -> 0x08020516 ; branch_target=0x08020516
08020600  636d      ldr	r3, [r4, #84]
08020602  4ff48061  mov.w	r1, #1024
08020606  2268      ldr	r2, [r4]
08020608  2046      mov	r0, r4
0802060a  43f48043  orr	r3, r3, #16384
0802060e  6365      str	r3, [r4, #84]
08020610  a36d      ldr	r3, [r4, #88]
08020612  43f00803  orr	r3, r3, #8
08020616  a365      str	r3, [r4, #88]
08020618  1160      str	r1, [r2]
0802061a  bde8f840  pop.w	{r3, r4, r5, r6, r7, lr}
0802061e  00f0c9be  b.w	#3474 ; -> 0x080213b4 ; branch_target=0x080213b4
08020622  40f22122  movw	r2, #545
08020626  ea40      lsrs	r2, r5
08020628  d107      lsls	r1, r2, #31
0802062a  3ff514af  bmi.w	#-472 ; -> 0x08020456 ; branch_target=0x08020456
0802062e  034a      ldr	r2, [pc, #12] ; [0x0802063c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020630  d268      ldr	r2, [r2, #12]
08020632  11e7      b	#-478 ; -> 0x08020458 ; branch_target=0x08020458
08020634  da68      ldr	r2, [r3, #12]
08020636  9107      lsls	r1, r2, #30
08020638  a7d0      beq	#-178 ; -> 0x0802058a ; branch_target=0x0802058a
0802063a  9ae7      b	#-204 ; -> 0x08020572 ; branch_target=0x08020572
08020650  636d      ldr	r3, [r4, #84]
08020652  43f01003  orr	r3, r3, #16
08020656  6365      str	r3, [r4, #84]
08020658  a36d      ldr	r3, [r4, #88]
0802065a  43f00103  orr	r3, r3, #1
0802065e  a365      str	r3, [r4, #88]
08020660  4be7      b	#-362 ; -> 0x080204fa ; branch_target=0x080204fa
08020662  636d      ldr	r3, [r4, #84]
08020664  43f01003  orr	r3, r3, #16
08020668  6365      str	r3, [r4, #84]
0802066a  a36d      ldr	r3, [r4, #88]
0802066c  43f00103  orr	r3, r3, #1
08020670  a365      str	r3, [r4, #88]
08020672  09e7      b	#-494 ; -> 0x08020488 ; branch_target=0x08020488
08020674  004a      ldr	r2, [pc, #0] ; [0x08020678] = 0x40022300 / f32_bits_interpretation=2.03338623
08020676  78e7      b	#-272 ; -> 0x0802056a ; branch_target=0x0802056a
080213b4  7047      bx	lr
