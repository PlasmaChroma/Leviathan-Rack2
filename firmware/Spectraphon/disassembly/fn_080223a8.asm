; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
080223a8  f0b5      push	{r4, r5, r6, r7, lr}
080223aa  0022      movs	r2, #0
080223ac  3c4b      ldr	r3, [pc, #240] ; [0x080224a0] = 0x20000014
080223ae  83b0      sub	sp, #12
080223b0  856d      ldr	r5, [r0, #88]
080223b2  1e68      ldr	r6, [r3]
080223b4  0446      mov	r4, r0
080223b6  0192      str	r2, [sp, #4]
080223b8  0368      ldr	r3, [r0]
080223ba  3a4a      ldr	r2, [pc, #232] ; [0x080224a4] = 0x40020010 / f32_bits_interpretation=2.031253815
080223bc  2f68      ldr	r7, [r5]
080223be  9342      cmp	r3, r2
080223c0  2968      ldr	r1, [r5]
080223c2  77d0      beq	#238 ; -> 0x080224b4 ; branch_target=0x080224b4
080223c4  1832      adds	r2, #24
080223c6  9342      cmp	r3, r2
080223c8  74d0      beq	#232 ; -> 0x080224b4 ; branch_target=0x080224b4
080223ca  1832      adds	r2, #24
080223cc  9342      cmp	r3, r2
080223ce  71d0      beq	#226 ; -> 0x080224b4 ; branch_target=0x080224b4
080223d0  1832      adds	r2, #24
080223d2  9342      cmp	r3, r2
080223d4  6ed0      beq	#220 ; -> 0x080224b4 ; branch_target=0x080224b4
080223d6  1832      adds	r2, #24
080223d8  9342      cmp	r3, r2
080223da  6bd0      beq	#214 ; -> 0x080224b4 ; branch_target=0x080224b4
080223dc  1832      adds	r2, #24
080223de  9342      cmp	r3, r2
080223e0  68d0      beq	#208 ; -> 0x080224b4 ; branch_target=0x080224b4
080223e2  1832      adds	r2, #24
080223e4  9342      cmp	r3, r2
080223e6  65d0      beq	#202 ; -> 0x080224b4 ; branch_target=0x080224b4
080223e8  1832      adds	r2, #24
080223ea  9342      cmp	r3, r2
080223ec  62d0      beq	#196 ; -> 0x080224b4 ; branch_target=0x080224b4
080223ee  02f55672  add.w	r2, r2, #856
080223f2  9342      cmp	r3, r2
080223f4  5ed0      beq	#188 ; -> 0x080224b4 ; branch_target=0x080224b4
080223f6  1832      adds	r2, #24
080223f8  9342      cmp	r3, r2
080223fa  5bd0      beq	#182 ; -> 0x080224b4 ; branch_target=0x080224b4
080223fc  1832      adds	r2, #24
080223fe  9342      cmp	r3, r2
08022400  58d0      beq	#176 ; -> 0x080224b4 ; branch_target=0x080224b4
08022402  1832      adds	r2, #24
08022404  9342      cmp	r3, r2
08022406  55d0      beq	#170 ; -> 0x080224b4 ; branch_target=0x080224b4
08022408  1832      adds	r2, #24
0802240a  9342      cmp	r3, r2
0802240c  52d0      beq	#164 ; -> 0x080224b4 ; branch_target=0x080224b4
0802240e  1832      adds	r2, #24
08022410  9342      cmp	r3, r2
08022412  4fd0      beq	#158 ; -> 0x080224b4 ; branch_target=0x080224b4
08022414  1832      adds	r2, #24
08022416  9342      cmp	r3, r2
08022418  4cd0      beq	#152 ; -> 0x080224b4 ; branch_target=0x080224b4
0802241a  1832      adds	r2, #24
0802241c  9342      cmp	r3, r2
0802241e  49d0      beq	#146 ; -> 0x080224b4 ; branch_target=0x080224b4
08022420  214a      ldr	r2, [pc, #132] ; [0x080224a8] = 0x58025408
08022422  2248      ldr	r0, [pc, #136] ; [0x080224ac] = 0x5802541c
08022424  8342      cmp	r3, r0
08022426  18bf      it	ne
08022428  9342      cmpne	r3, r2
0802242a  00f11400  add.w	r0, r0, #20
0802242e  0cbf      ite	eq
08022430  0122      moveq	r2, #1
08022432  0022      movne	r2, #0
08022434  8342      cmp	r3, r0
08022436  08bf      it	eq
08022438  42f00102  orreq	r2, r2, #1
0802243c  1430      adds	r0, #20
0802243e  8342      cmp	r3, r0
08022440  08bf      it	eq
08022442  42f00102  orreq	r2, r2, #1
08022446  1430      adds	r0, #20
08022448  8342      cmp	r3, r0
0802244a  08bf      it	eq
0802244c  42f00102  orreq	r2, r2, #1
08022450  1430      adds	r0, #20
08022452  8342      cmp	r3, r0
08022454  08bf      it	eq
08022456  42f00102  orreq	r2, r2, #1
0802245a  1430      adds	r0, #20
0802245c  8342      cmp	r3, r0
0802245e  08bf      it	eq
08022460  42f00102  orreq	r2, r2, #1
08022464  12b9      cbnz	r2, #4 ; -> 0x0802246c ; branch_target=0x0802246c
08022466  124a      ldr	r2, [pc, #72] ; [0x080224b0] = 0x58025494
08022468  9342      cmp	r3, r2
0802246a  16d1      bne	#44 ; -> 0x0802249a ; branch_target=0x0802249a
0802246c  e06d      ldr	r0, [r4, #92]
0802246e  0426      movs	r6, #4
08022470  1a68      ldr	r2, [r3]
08022472  00f01f00  and	r0, r0, #31
08022476  8640      lsls	r6, r0
08022478  0e42      tst	r6, r1
0802247a  00f09282  beq.w	#1316 ; -> 0x080229a2 ; branch_target=0x080229a2
0802247e  5707      lsls	r7, r2, #29
08022480  40f18f82  bpl.w	#1310 ; -> 0x080229a2 ; branch_target=0x080229a2
08022484  1104      lsls	r1, r2, #16
08022486  6e60      str	r6, [r5, #4]
08022488  40f1ca82  bpl.w	#1428 ; -> 0x08022a20 ; branch_target=0x08022a20
0802248c  d203      lsls	r2, r2, #15
0802248e  00f11e83  bmi.w	#1596 ; -> 0x08022ace ; branch_target=0x08022ace
08022492  a36c      ldr	r3, [r4, #72]
08022494  002b      cmp	r3, #0
08022496  40f0a381  bne.w	#838 ; -> 0x080227e0 ; branch_target=0x080227e0
0802249a  03b0      add	sp, #12
0802249c  f0bd      pop	{r4, r5, r6, r7, pc}
080224b4  d4f85cc0  ldr.w	r12, [r4, #92]
080224b8  0822      movs	r2, #8
080224ba  0cf01f00  and	r0, r12, #31
080224be  8240      lsls	r2, r0
080224c0  3a42      tst	r2, r7
080224c2  40f09281  bne.w	#804 ; -> 0x080227ea ; branch_target=0x080227ea
080224c6  27fa00f2  lsr.w	r2, r7, r0
080224ca  d207      lsls	r2, r2, #31
080224cc  0ed5      bpl	#28 ; -> 0x080224ec ; branch_target=0x080224ec
080224ce  5a69      ldr	r2, [r3, #20]
080224d0  1106      lsls	r1, r2, #24
080224d2  40f10982  bpl.w	#1042 ; -> 0x080228e8 ; branch_target=0x080228e8
080224d6  0123      movs	r3, #1
080224d8  8340      lsls	r3, r0
080224da  ab60      str	r3, [r5, #8]
080224dc  636d      ldr	r3, [r4, #84]
080224de  d4f85cc0  ldr.w	r12, [r4, #92]
080224e2  43f00203  orr	r3, r3, #2
080224e6  0cf01f00  and	r0, r12, #31
080224ea  6365      str	r3, [r4, #84]
080224ec  0421      movs	r1, #4
080224ee  8140      lsls	r1, r0
080224f0  3942      tst	r1, r7
080224f2  5fd0      beq	#190 ; -> 0x080225b4 ; branch_target=0x080225b4
080224f4  d4f800e0  ldr.w	lr, [r4]
080224f8  974a      ldr	r2, [pc, #604] ; [0x08022758] = 0x40020010 / f32_bits_interpretation=2.031253815
080224fa  984b      ldr	r3, [pc, #608] ; [0x0802275c] = 0x40020028 / f32_bits_interpretation=2.031259537
080224fc  9e45      cmp	lr, r3
080224fe  18bf      it	ne
08022500  9645      cmpne	lr, r2
08022502  03f11803  add.w	r3, r3, #24
08022506  0cbf      ite	eq
08022508  0122      moveq	r2, #1
0802250a  0022      movne	r2, #0
0802250c  9e45      cmp	lr, r3
0802250e  08bf      it	eq
08022510  42f00102  orreq	r2, r2, #1
08022514  1833      adds	r3, #24
08022516  9e45      cmp	lr, r3
08022518  08bf      it	eq
0802251a  42f00102  orreq	r2, r2, #1
0802251e  1833      adds	r3, #24
08022520  9e45      cmp	lr, r3
08022522  08bf      it	eq
08022524  42f00102  orreq	r2, r2, #1
08022528  1833      adds	r3, #24
0802252a  9e45      cmp	lr, r3
0802252c  08bf      it	eq
0802252e  42f00102  orreq	r2, r2, #1
08022532  1833      adds	r3, #24
08022534  9e45      cmp	lr, r3
08022536  08bf      it	eq
08022538  42f00102  orreq	r2, r2, #1
0802253c  1833      adds	r3, #24
0802253e  9e45      cmp	lr, r3
08022540  08bf      it	eq
08022542  42f00102  orreq	r2, r2, #1
08022546  03f55673  add.w	r3, r3, #856
0802254a  9e45      cmp	lr, r3
0802254c  08bf      it	eq
0802254e  42f00102  orreq	r2, r2, #1
08022552  1833      adds	r3, #24
08022554  9e45      cmp	lr, r3
08022556  08bf      it	eq
08022558  42f00102  orreq	r2, r2, #1
0802255c  1833      adds	r3, #24
0802255e  9e45      cmp	lr, r3
08022560  08bf      it	eq
08022562  42f00102  orreq	r2, r2, #1
08022566  1833      adds	r3, #24
08022568  9e45      cmp	lr, r3
0802256a  08bf      it	eq
0802256c  42f00102  orreq	r2, r2, #1
08022570  1833      adds	r3, #24
08022572  9e45      cmp	lr, r3
08022574  08bf      it	eq
08022576  42f00102  orreq	r2, r2, #1
0802257a  1833      adds	r3, #24
0802257c  9e45      cmp	lr, r3
0802257e  08bf      it	eq
08022580  42f00102  orreq	r2, r2, #1
08022584  1833      adds	r3, #24
08022586  9e45      cmp	lr, r3
08022588  08bf      it	eq
0802258a  42f00102  orreq	r2, r2, #1
0802258e  7346      mov	r3, lr
08022590  1ab9      cbnz	r2, #6 ; -> 0x0802259a ; branch_target=0x0802259a
08022592  734a      ldr	r2, [pc, #460] ; [0x08022760] = 0x400204b8 / f32_bits_interpretation=2.03153801
08022594  9645      cmp	lr, r2
08022596  40f0f981  bne.w	#1010 ; -> 0x0802298c ; branch_target=0x0802298c
0802259a  1a68      ldr	r2, [r3]
0802259c  9207      lsls	r2, r2, #30
0802259e  40f1ac81  bpl.w	#856 ; -> 0x080228fa ; branch_target=0x080228fa
080225a2  a960      str	r1, [r5, #8]
080225a4  636d      ldr	r3, [r4, #84]
080225a6  d4f85cc0  ldr.w	r12, [r4, #92]
080225aa  43f00403  orr	r3, r3, #4
080225ae  0cf01f00  and	r0, r12, #31
080225b2  6365      str	r3, [r4, #84]
080225b4  1021      movs	r1, #16
080225b6  8140      lsls	r1, r0
080225b8  3942      tst	r1, r7
080225ba  73d0      beq	#230 ; -> 0x080226a4 ; branch_target=0x080226a4
080225bc  2368      ldr	r3, [r4]
080225be  664a      ldr	r2, [pc, #408] ; [0x08022758] = 0x40020010 / f32_bits_interpretation=2.031253815
080225c0  dff898e1  ldr.w	lr, [pc, #408] ; [0x0802275c] = 0x40020028 / f32_bits_interpretation=2.031259537
080225c4  7345      cmp	r3, lr
080225c6  18bf      it	ne
080225c8  9342      cmpne	r3, r2
080225ca  0ef1180e  add.w	lr, lr, #24
080225ce  0cbf      ite	eq
080225d0  0122      moveq	r2, #1
080225d2  0022      movne	r2, #0
080225d4  7345      cmp	r3, lr
080225d6  08bf      it	eq
080225d8  42f00102  orreq	r2, r2, #1
080225dc  0ef1180e  add.w	lr, lr, #24
080225e0  7345      cmp	r3, lr
080225e2  08bf      it	eq
080225e4  42f00102  orreq	r2, r2, #1
080225e8  0ef1180e  add.w	lr, lr, #24
080225ec  7345      cmp	r3, lr
080225ee  08bf      it	eq
080225f0  42f00102  orreq	r2, r2, #1
080225f4  0ef1180e  add.w	lr, lr, #24
080225f8  7345      cmp	r3, lr
080225fa  08bf      it	eq
080225fc  42f00102  orreq	r2, r2, #1
08022600  0ef1180e  add.w	lr, lr, #24
08022604  7345      cmp	r3, lr
08022606  08bf      it	eq
08022608  42f00102  orreq	r2, r2, #1
0802260c  0ef1180e  add.w	lr, lr, #24
08022610  7345      cmp	r3, lr
08022612  08bf      it	eq
08022614  42f00102  orreq	r2, r2, #1
08022618  0ef5567e  add.w	lr, lr, #856
0802261c  7345      cmp	r3, lr
0802261e  08bf      it	eq
08022620  42f00102  orreq	r2, r2, #1
08022624  0ef1180e  add.w	lr, lr, #24
08022628  7345      cmp	r3, lr
0802262a  08bf      it	eq
0802262c  42f00102  orreq	r2, r2, #1
08022630  0ef1180e  add.w	lr, lr, #24
08022634  7345      cmp	r3, lr
08022636  08bf      it	eq
08022638  42f00102  orreq	r2, r2, #1
0802263c  0ef1180e  add.w	lr, lr, #24
08022640  7345      cmp	r3, lr
08022642  08bf      it	eq
08022644  42f00102  orreq	r2, r2, #1
08022648  0ef1180e  add.w	lr, lr, #24
0802264c  7345      cmp	r3, lr
0802264e  08bf      it	eq
08022650  42f00102  orreq	r2, r2, #1
08022654  0ef1180e  add.w	lr, lr, #24
08022658  7345      cmp	r3, lr
0802265a  08bf      it	eq
0802265c  42f00102  orreq	r2, r2, #1
08022660  0ef1180e  add.w	lr, lr, #24
08022664  7345      cmp	r3, lr
08022666  08bf      it	eq
08022668  42f00102  orreq	r2, r2, #1
0802266c  1ab9      cbnz	r2, #6 ; -> 0x08022676 ; branch_target=0x08022676
0802266e  3c4a      ldr	r2, [pc, #240] ; [0x08022760] = 0x400204b8 / f32_bits_interpretation=2.03153801
08022670  9342      cmp	r3, r2
08022672  40f03281  bne.w	#612 ; -> 0x080228da ; branch_target=0x080228da
08022676  1b68      ldr	r3, [r3]
08022678  1a07      lsls	r2, r3, #28
0802267a  13d5      bpl	#38 ; -> 0x080226a4 ; branch_target=0x080226a4
0802267c  a960      str	r1, [r5, #8]
0802267e  2368      ldr	r3, [r4]
08022680  1a68      ldr	r2, [r3]
08022682  5003      lsls	r0, r2, #13
08022684  00f13f81  bmi.w	#638 ; -> 0x08022906 ; branch_target=0x08022906
08022688  1a68      ldr	r2, [r3]
0802268a  d205      lsls	r2, r2, #23
0802268c  03d4      bmi	#6 ; -> 0x08022696 ; branch_target=0x08022696
0802268e  1a68      ldr	r2, [r3]
08022690  22f00802  bic	r2, r2, #8
08022694  1a60      str	r2, [r3]
08022696  236c      ldr	r3, [r4, #64]
08022698  0bb1      cbz	r3, #2 ; -> 0x0802269e ; branch_target=0x0802269e
0802269a  2046      mov	r0, r4
0802269c  9847      blx	r3
0802269e  e06d      ldr	r0, [r4, #92]
080226a0  00f01f00  and	r0, r0, #31
080226a4  2021      movs	r1, #32
080226a6  8140      lsls	r1, r0
080226a8  3942      tst	r1, r7
080226aa  6ed0      beq	#220 ; -> 0x0802278a ; branch_target=0x0802278a
080226ac  2268      ldr	r2, [r4]
080226ae  2a4b      ldr	r3, [pc, #168] ; [0x08022758] = 0x40020010 / f32_bits_interpretation=2.031253815
080226b0  2a48      ldr	r0, [pc, #168] ; [0x0802275c] = 0x40020028 / f32_bits_interpretation=2.031259537
080226b2  8242      cmp	r2, r0
080226b4  18bf      it	ne
080226b6  9a42      cmpne	r2, r3
080226b8  00f11800  add.w	r0, r0, #24
080226bc  0cbf      ite	eq
080226be  0123      moveq	r3, #1
080226c0  0023      movne	r3, #0
080226c2  8242      cmp	r2, r0
080226c4  08bf      it	eq
080226c6  43f00103  orreq	r3, r3, #1
080226ca  1830      adds	r0, #24
080226cc  8242      cmp	r2, r0
080226ce  08bf      it	eq
080226d0  43f00103  orreq	r3, r3, #1
080226d4  1830      adds	r0, #24
080226d6  8242      cmp	r2, r0
080226d8  08bf      it	eq
080226da  43f00103  orreq	r3, r3, #1
080226de  1830      adds	r0, #24
080226e0  8242      cmp	r2, r0
080226e2  08bf      it	eq
080226e4  43f00103  orreq	r3, r3, #1
080226e8  1830      adds	r0, #24
080226ea  8242      cmp	r2, r0
080226ec  08bf      it	eq
080226ee  43f00103  orreq	r3, r3, #1
080226f2  1830      adds	r0, #24
080226f4  8242      cmp	r2, r0
080226f6  08bf      it	eq
080226f8  43f00103  orreq	r3, r3, #1
080226fc  00f55670  add.w	r0, r0, #856
08022700  8242      cmp	r2, r0
08022702  08bf      it	eq
08022704  43f00103  orreq	r3, r3, #1
08022708  1830      adds	r0, #24
0802270a  8242      cmp	r2, r0
0802270c  08bf      it	eq
0802270e  43f00103  orreq	r3, r3, #1
08022712  1830      adds	r0, #24
08022714  8242      cmp	r2, r0
08022716  08bf      it	eq
08022718  43f00103  orreq	r3, r3, #1
0802271c  1830      adds	r0, #24
0802271e  8242      cmp	r2, r0
08022720  08bf      it	eq
08022722  43f00103  orreq	r3, r3, #1
08022726  1830      adds	r0, #24
08022728  8242      cmp	r2, r0
0802272a  08bf      it	eq
0802272c  43f00103  orreq	r3, r3, #1
08022730  1830      adds	r0, #24
08022732  8242      cmp	r2, r0
08022734  08bf      it	eq
08022736  43f00103  orreq	r3, r3, #1
0802273a  1830      adds	r0, #24
0802273c  8242      cmp	r2, r0
0802273e  08bf      it	eq
08022740  43f00103  orreq	r3, r3, #1
08022744  1bb9      cbnz	r3, #6 ; -> 0x0802274e ; branch_target=0x0802274e
08022746  064b      ldr	r3, [pc, #24] ; [0x08022760] = 0x400204b8 / f32_bits_interpretation=2.03153801
08022748  9a42      cmp	r2, r3
0802274a  40f03c81  bne.w	#632 ; -> 0x080229c6 ; branch_target=0x080229c6
0802274e  1368      ldr	r3, [r2]
08022750  df06      lsls	r7, r3, #27
08022752  1ad5      bpl	#52 ; -> 0x0802278a ; branch_target=0x0802278a
08022754  06e0      b	#12 ; -> 0x08022764 ; branch_target=0x08022764
08022764  a960      str	r1, [r5, #8]
08022766  94f83530  ldrb.w	r3, [r4, #53]
0802276a  042b      cmp	r3, #4
0802276c  00f0d480  beq.w	#424 ; -> 0x08022918 ; branch_target=0x08022918
08022770  2368      ldr	r3, [r4]
08022772  1a68      ldr	r2, [r3]
08022774  5203      lsls	r2, r2, #13
08022776  40f1f180  bpl.w	#482 ; -> 0x0802295c ; branch_target=0x0802295c
0802277a  1b68      ldr	r3, [r3]
0802277c  1f03      lsls	r7, r3, #12
0802277e  40f1fc80  bpl.w	#504 ; -> 0x0802297a ; branch_target=0x0802297a
08022782  e36b      ldr	r3, [r4, #60]
08022784  0bb1      cbz	r3, #2 ; -> 0x0802278a ; branch_target=0x0802278a
08022786  2046      mov	r0, r4
08022788  9847      blx	r3
0802278a  636d      ldr	r3, [r4, #84]
0802278c  002b      cmp	r3, #0
0802278e  3ff484ae  beq.w	#-760 ; -> 0x0802249a ; branch_target=0x0802249a
08022792  636d      ldr	r3, [r4, #84]
08022794  dd07      lsls	r5, r3, #31
08022796  1fd5      bpl	#62 ; -> 0x080227d8 ; branch_target=0x080227d8
08022798  2268      ldr	r2, [r4]
0802279a  0421      movs	r1, #4
0802279c  84f83510  strb.w	r1, [r4, #53]
080227a0  1368      ldr	r3, [r2]
080227a2  23f00103  bic	r3, r3, #1
080227a6  1360      str	r3, [r2]
080227a8  994a      ldr	r2, [pc, #612] ; [0x08022a10] = 0x1b4e81b5
080227aa  2168      ldr	r1, [r4]
080227ac  a2fb0662  umull	r6, r2, r2, r6
080227b0  920a      lsrs	r2, r2, #10
080227b2  02e0      b	#4 ; -> 0x080227ba ; branch_target=0x080227ba
080227b4  0b68      ldr	r3, [r1]
080227b6  d807      lsls	r0, r3, #31
080227b8  04d5      bpl	#8 ; -> 0x080227c4 ; branch_target=0x080227c4
080227ba  019b      ldr	r3, [sp, #4]
080227bc  0133      adds	r3, #1
080227be  9342      cmp	r3, r2
080227c0  0193      str	r3, [sp, #4]
080227c2  f7d9      bls	#-18 ; -> 0x080227b4 ; branch_target=0x080227b4
080227c4  0023      movs	r3, #0
080227c6  84f83430  strb.w	r3, [r4, #52]
080227ca  0b68      ldr	r3, [r1]
080227cc  db07      lsls	r3, r3, #31
080227ce  4cbf      ite	mi
080227d0  0323      movmi	r3, #3
080227d2  0123      movpl	r3, #1
080227d4  84f83530  strb.w	r3, [r4, #53]
080227d8  e36c      ldr	r3, [r4, #76]
080227da  002b      cmp	r3, #0
080227dc  3ff45dae  beq.w	#-838 ; -> 0x0802249a ; branch_target=0x0802249a
080227e0  2046      mov	r0, r4
080227e2  03b0      add	sp, #12
080227e4  bde8f040  pop.w	{r4, r5, r6, r7, lr}
080227e8  1847      bx	r3
080227ea  1a68      ldr	r2, [r3]
080227ec  5207      lsls	r2, r2, #29
080227ee  11d5      bpl	#34 ; -> 0x08022814 ; branch_target=0x08022814
080227f0  1a68      ldr	r2, [r3]
080227f2  22f00402  bic	r2, r2, #4
080227f6  1a60      str	r2, [r3]
080227f8  e36d      ldr	r3, [r4, #92]
080227fa  03f01f02  and	r2, r3, #31
080227fe  0823      movs	r3, #8
08022800  9340      lsls	r3, r2
08022802  ab60      str	r3, [r5, #8]
08022804  636d      ldr	r3, [r4, #84]
08022806  d4f85cc0  ldr.w	r12, [r4, #92]
0802280a  43f00103  orr	r3, r3, #1
0802280e  0cf01f00  and	r0, r12, #31
08022812  6365      str	r3, [r4, #84]
08022814  27fa00f3  lsr.w	r3, r7, r0
08022818  db07      lsls	r3, r3, #31
0802281a  7ff567ae  bpl.w	#-818 ; -> 0x080224ec ; branch_target=0x080224ec
0802281e  2368      ldr	r3, [r4]
08022820  7c4a      ldr	r2, [pc, #496] ; [0x08022a14] = 0x40020010 / f32_bits_interpretation=2.031253815
08022822  7d49      ldr	r1, [pc, #500] ; [0x08022a18] = 0x40020028 / f32_bits_interpretation=2.031259537
08022824  9e46      mov	lr, r3
08022826  8b42      cmp	r3, r1
08022828  18bf      it	ne
0802282a  9342      cmpne	r3, r2
0802282c  01f11801  add.w	r1, r1, #24
08022830  0cbf      ite	eq
08022832  0122      moveq	r2, #1
08022834  0022      movne	r2, #0
08022836  8b42      cmp	r3, r1
08022838  08bf      it	eq
0802283a  42f00102  orreq	r2, r2, #1
0802283e  1831      adds	r1, #24
08022840  8b42      cmp	r3, r1
08022842  08bf      it	eq
08022844  42f00102  orreq	r2, r2, #1
08022848  1831      adds	r1, #24
0802284a  8b42      cmp	r3, r1
0802284c  08bf      it	eq
0802284e  42f00102  orreq	r2, r2, #1
08022852  1831      adds	r1, #24
08022854  8b42      cmp	r3, r1
08022856  08bf      it	eq
08022858  42f00102  orreq	r2, r2, #1
0802285c  1831      adds	r1, #24
0802285e  8b42      cmp	r3, r1
08022860  08bf      it	eq
08022862  42f00102  orreq	r2, r2, #1
08022866  1831      adds	r1, #24
08022868  8b42      cmp	r3, r1
0802286a  08bf      it	eq
0802286c  42f00102  orreq	r2, r2, #1
08022870  01f55671  add.w	r1, r1, #856
08022874  8b42      cmp	r3, r1
08022876  08bf      it	eq
08022878  42f00102  orreq	r2, r2, #1
0802287c  1831      adds	r1, #24
0802287e  8b42      cmp	r3, r1
08022880  08bf      it	eq
08022882  42f00102  orreq	r2, r2, #1
08022886  1831      adds	r1, #24
08022888  8b42      cmp	r3, r1
0802288a  08bf      it	eq
0802288c  42f00102  orreq	r2, r2, #1
08022890  1831      adds	r1, #24
08022892  8b42      cmp	r3, r1
08022894  08bf      it	eq
08022896  42f00102  orreq	r2, r2, #1
0802289a  1831      adds	r1, #24
0802289c  8b42      cmp	r3, r1
0802289e  08bf      it	eq
080228a0  42f00102  orreq	r2, r2, #1
080228a4  1831      adds	r1, #24
080228a6  8b42      cmp	r3, r1
080228a8  08bf      it	eq
080228aa  42f00102  orreq	r2, r2, #1
080228ae  1831      adds	r1, #24
080228b0  8b42      cmp	r3, r1
080228b2  08bf      it	eq
080228b4  42f00102  orreq	r2, r2, #1
080228b8  002a      cmp	r2, #0
080228ba  7ff408ae  bne.w	#-1008 ; -> 0x080224ce ; branch_target=0x080224ce
080228be  574a      ldr	r2, [pc, #348] ; [0x08022a1c] = 0x400204b8 / f32_bits_interpretation=2.03153801
080228c0  9342      cmp	r3, r2
080228c2  3ff404ae  beq.w	#-1016 ; -> 0x080224ce ; branch_target=0x080224ce
080228c6  1a68      ldr	r2, [r3]
080228c8  0422      movs	r2, #4
080228ca  8240      lsls	r2, r0
080228cc  3a42      tst	r2, r7
080228ce  5dd1      bne	#186 ; -> 0x0802298c ; branch_target=0x0802298c
080228d0  1021      movs	r1, #16
080228d2  8140      lsls	r1, r0
080228d4  0f42      tst	r7, r1
080228d6  3ff4e5ae  beq.w	#-566 ; -> 0x080226a4 ; branch_target=0x080226a4
080228da  1b68      ldr	r3, [r3]
080228dc  5b07      lsls	r3, r3, #29
080228de  3ff5cdae  bmi.w	#-614 ; -> 0x0802267c ; branch_target=0x0802267c
080228e2  0cf01f00  and	r0, r12, #31
080228e6  dde6      b	#-582 ; -> 0x080226a4 ; branch_target=0x080226a4
080228e8  0421      movs	r1, #4
080228ea  8140      lsls	r1, r0
080228ec  0f42      tst	r7, r1
080228ee  3ff461ae  beq.w	#-830 ; -> 0x080225b4 ; branch_target=0x080225b4
080228f2  1a68      ldr	r2, [r3]
080228f4  9207      lsls	r2, r2, #30
080228f6  3ff554ae  bmi.w	#-856 ; -> 0x080225a2 ; branch_target=0x080225a2
080228fa  1021      movs	r1, #16
080228fc  8140      lsls	r1, r0
080228fe  0f42      tst	r7, r1
08022900  7ff4b9ae  bne.w	#-654 ; -> 0x08022676 ; branch_target=0x08022676
08022904  cee6      b	#-612 ; -> 0x080226a4 ; branch_target=0x080226a4
08022906  1b68      ldr	r3, [r3]
08022908  1903      lsls	r1, r3, #12
0802290a  7ff5c4ae  bpl.w	#-632 ; -> 0x08022696 ; branch_target=0x08022696
0802290e  a36c      ldr	r3, [r4, #72]
08022910  002b      cmp	r3, #0
08022912  7ff4c2ae  bne.w	#-636 ; -> 0x0802269a ; branch_target=0x0802269a
08022916  c2e6      b	#-636 ; -> 0x0802269e ; branch_target=0x0802269e
08022918  2268      ldr	r2, [r4]
0802291a  1368      ldr	r3, [r2]
0802291c  23f01603  bic	r3, r3, #22
08022920  1360      str	r3, [r2]
08022922  2268      ldr	r2, [r4]
08022924  5369      ldr	r3, [r2, #20]
08022926  23f08003  bic	r3, r3, #128
0802292a  5361      str	r3, [r2, #20]
0802292c  236c      ldr	r3, [r4, #64]
0802292e  4bb3      cbz	r3, #82 ; -> 0x08022984 ; branch_target=0x08022984
08022930  2268      ldr	r2, [r4]
08022932  1368      ldr	r3, [r2]
08022934  23f00803  bic	r3, r3, #8
08022938  1360      str	r3, [r2]
0802293a  e26d      ldr	r2, [r4, #92]
0802293c  3f23      movs	r3, #63
0802293e  0021      movs	r1, #0
08022940  02f01f02  and	r2, r2, #31
08022944  9340      lsls	r3, r2
08022946  0122      movs	r2, #1
08022948  ab60      str	r3, [r5, #8]
0802294a  236d      ldr	r3, [r4, #80]
0802294c  84f83410  strb.w	r1, [r4, #52]
08022950  84f83520  strb.w	r2, [r4, #53]
08022954  002b      cmp	r3, #0
08022956  7ff443af  bne.w	#-378 ; -> 0x080227e0 ; branch_target=0x080227e0
0802295a  9ee5      b	#-1220 ; -> 0x0802249a ; branch_target=0x0802249a
0802295c  1a68      ldr	r2, [r3]
0802295e  12f48072  ands	r2, r2, #256
08022962  7ff40eaf  bne.w	#-484 ; -> 0x08022782 ; branch_target=0x08022782
08022966  1968      ldr	r1, [r3]
08022968  21f01001  bic	r1, r1, #16
0802296c  1960      str	r1, [r3]
0802296e  0123      movs	r3, #1
08022970  84f83420  strb.w	r2, [r4, #52]
08022974  84f83530  strb.w	r3, [r4, #53]
08022978  03e7      b	#-506 ; -> 0x08022782 ; branch_target=0x08022782
0802297a  636c      ldr	r3, [r4, #68]
0802297c  002b      cmp	r3, #0
0802297e  7ff402af  bne.w	#-508 ; -> 0x08022786 ; branch_target=0x08022786
08022982  02e7      b	#-508 ; -> 0x0802278a ; branch_target=0x0802278a
08022984  a36c      ldr	r3, [r4, #72]
08022986  002b      cmp	r3, #0
08022988  d2d1      bne	#-92 ; -> 0x08022930 ; branch_target=0x08022930
0802298a  d6e7      b	#-84 ; -> 0x0802293a ; branch_target=0x0802293a
0802298c  0cf01f00  and	r0, r12, #31
08022990  1021      movs	r1, #16
08022992  def80030  ldr.w	r3, [lr]
08022996  8140      lsls	r1, r0
08022998  0f42      tst	r7, r1
0802299a  3ff483ae  beq.w	#-762 ; -> 0x080226a4 ; branch_target=0x080226a4
0802299e  7346      mov	r3, lr
080229a0  9be7      b	#-202 ; -> 0x080228da ; branch_target=0x080228da
080229a2  0226      movs	r6, #2
080229a4  8640      lsls	r6, r0
080229a6  0e42      tst	r6, r1
080229a8  12d0      beq	#36 ; -> 0x080229d0 ; branch_target=0x080229d0
080229aa  9707      lsls	r7, r2, #30
080229ac  10d5      bpl	#32 ; -> 0x080229d0 ; branch_target=0x080229d0
080229ae  6e60      str	r6, [r5, #4]
080229b0  1604      lsls	r6, r2, #16
080229b2  40f19180  bpl.w	#290 ; -> 0x08022ad8 ; branch_target=0x08022ad8
080229b6  d503      lsls	r5, r2, #15
080229b8  00f1eb80  bmi.w	#470 ; -> 0x08022b92 ; branch_target=0x08022b92
080229bc  636c      ldr	r3, [r4, #68]
080229be  002b      cmp	r3, #0
080229c0  7ff40eaf  bne.w	#-484 ; -> 0x080227e0 ; branch_target=0x080227e0
080229c4  69e5      b	#-1326 ; -> 0x0802249a ; branch_target=0x0802249a
080229c6  1368      ldr	r3, [r2]
080229c8  9807      lsls	r0, r3, #30
080229ca  7ff5deae  bpl.w	#-580 ; -> 0x0802278a ; branch_target=0x0802278a
080229ce  c9e6      b	#-622 ; -> 0x08022764 ; branch_target=0x08022764
080229d0  0826      movs	r6, #8
080229d2  06fa00f0  lsl.w	r0, r6, r0
080229d6  0842      tst	r0, r1
080229d8  3ff45fad  beq.w	#-1346 ; -> 0x0802249a ; branch_target=0x0802249a
080229dc  1107      lsls	r1, r2, #28
080229de  7ff55cad  bpl.w	#-1352 ; -> 0x0802249a ; branch_target=0x0802249a
080229e2  1968      ldr	r1, [r3]
080229e4  0122      movs	r2, #1
080229e6  21f00e01  bic	r1, r1, #14
080229ea  1960      str	r1, [r3]
080229ec  0021      movs	r1, #0
080229ee  e36d      ldr	r3, [r4, #92]
080229f0  03f01f03  and	r3, r3, #31
080229f4  02fa03f3  lsl.w	r3, r2, r3
080229f8  6b60      str	r3, [r5, #4]
080229fa  e36c      ldr	r3, [r4, #76]
080229fc  6265      str	r2, [r4, #84]
080229fe  84f83410  strb.w	r1, [r4, #52]
08022a02  84f83520  strb.w	r2, [r4, #53]
08022a06  002b      cmp	r3, #0
08022a08  7ff4eaae  bne.w	#-556 ; -> 0x080227e0 ; branch_target=0x080227e0
08022a0c  45e5      b	#-1398 ; -> 0x0802249a ; branch_target=0x0802249a
08022a20  9306      lsls	r3, r2, #26
08022a22  54d4      bmi	#168 ; -> 0x08022ace ; branch_target=0x08022ace
08022a24  2268      ldr	r2, [r4]
08022a26  624b      ldr	r3, [pc, #392] ; [0x08022bb0] = 0x40020010 / f32_bits_interpretation=2.031253815
08022a28  6248      ldr	r0, [pc, #392] ; [0x08022bb4] = 0x40020028 / f32_bits_interpretation=2.031259537
08022a2a  6349      ldr	r1, [pc, #396] ; [0x08022bb8] = 0x40020040 / f32_bits_interpretation=2.031265259
08022a2c  8242      cmp	r2, r0
08022a2e  18bf      it	ne
08022a30  9a42      cmpne	r2, r3
08022a32  00f13000  add.w	r0, r0, #48
08022a36  0cbf      ite	eq
08022a38  0123      moveq	r3, #1
08022a3a  0023      movne	r3, #0
08022a3c  8a42      cmp	r2, r1
08022a3e  08bf      it	eq
08022a40  43f00103  orreq	r3, r3, #1
08022a44  3031      adds	r1, #48
08022a46  8242      cmp	r2, r0
08022a48  08bf      it	eq
08022a4a  43f00103  orreq	r3, r3, #1
08022a4e  3030      adds	r0, #48
08022a50  8a42      cmp	r2, r1
08022a52  08bf      it	eq
08022a54  43f00103  orreq	r3, r3, #1
08022a58  3031      adds	r1, #48
08022a5a  8242      cmp	r2, r0
08022a5c  08bf      it	eq
08022a5e  43f00103  orreq	r3, r3, #1
08022a62  3030      adds	r0, #48
08022a64  8a42      cmp	r2, r1
08022a66  08bf      it	eq
08022a68  43f00103  orreq	r3, r3, #1
08022a6c  01f55c71  add.w	r1, r1, #880
08022a70  8242      cmp	r2, r0
08022a72  08bf      it	eq
08022a74  43f00103  orreq	r3, r3, #1
08022a78  00f55c70  add.w	r0, r0, #880
08022a7c  8a42      cmp	r2, r1
08022a7e  08bf      it	eq
08022a80  43f00103  orreq	r3, r3, #1
08022a84  3031      adds	r1, #48
08022a86  8242      cmp	r2, r0
08022a88  08bf      it	eq
08022a8a  43f00103  orreq	r3, r3, #1
08022a8e  3030      adds	r0, #48
08022a90  8a42      cmp	r2, r1
08022a92  08bf      it	eq
08022a94  43f00103  orreq	r3, r3, #1
08022a98  3031      adds	r1, #48
08022a9a  8242      cmp	r2, r0
08022a9c  08bf      it	eq
08022a9e  43f00103  orreq	r3, r3, #1
08022aa2  3030      adds	r0, #48
08022aa4  8a42      cmp	r2, r1
08022aa6  08bf      it	eq
08022aa8  43f00103  orreq	r3, r3, #1
08022aac  3031      adds	r1, #48
08022aae  8242      cmp	r2, r0
08022ab0  08bf      it	eq
08022ab2  43f00103  orreq	r3, r3, #1
08022ab6  8a42      cmp	r2, r1
08022ab8  08bf      it	eq
08022aba  43f00103  orreq	r3, r3, #1
08022abe  13b9      cbnz	r3, #4 ; -> 0x08022ac6 ; branch_target=0x08022ac6
08022ac0  3e4b      ldr	r3, [pc, #248] ; [0x08022bbc] = 0x400204b8 / f32_bits_interpretation=2.03153801
08022ac2  9a42      cmp	r2, r3
08022ac4  6ad1      bne	#212 ; -> 0x08022b9c ; branch_target=0x08022b9c
08022ac6  1368      ldr	r3, [r2]
08022ac8  23f00803  bic	r3, r3, #8
08022acc  1360      str	r3, [r2]
08022ace  236c      ldr	r3, [r4, #64]
08022ad0  002b      cmp	r3, #0
08022ad2  7ff485ae  bne.w	#-758 ; -> 0x080227e0 ; branch_target=0x080227e0
08022ad6  e0e4      b	#-1600 ; -> 0x0802249a ; branch_target=0x0802249a
08022ad8  9006      lsls	r0, r2, #26
08022ada  5ad4      bmi	#180 ; -> 0x08022b92 ; branch_target=0x08022b92
08022adc  2268      ldr	r2, [r4]
08022ade  344b      ldr	r3, [pc, #208] ; [0x08022bb0] = 0x40020010 / f32_bits_interpretation=2.031253815
08022ae0  3448      ldr	r0, [pc, #208] ; [0x08022bb4] = 0x40020028 / f32_bits_interpretation=2.031259537
08022ae2  3549      ldr	r1, [pc, #212] ; [0x08022bb8] = 0x40020040 / f32_bits_interpretation=2.031265259
08022ae4  8242      cmp	r2, r0
08022ae6  18bf      it	ne
08022ae8  9a42      cmpne	r2, r3
08022aea  00f13000  add.w	r0, r0, #48
08022aee  0cbf      ite	eq
08022af0  0123      moveq	r3, #1
08022af2  0023      movne	r3, #0
08022af4  8a42      cmp	r2, r1
08022af6  08bf      it	eq
08022af8  43f00103  orreq	r3, r3, #1
08022afc  3031      adds	r1, #48
08022afe  8242      cmp	r2, r0
08022b00  08bf      it	eq
08022b02  43f00103  orreq	r3, r3, #1
08022b06  3030      adds	r0, #48
08022b08  8a42      cmp	r2, r1
08022b0a  08bf      it	eq
08022b0c  43f00103  orreq	r3, r3, #1
08022b10  3031      adds	r1, #48
08022b12  8242      cmp	r2, r0
08022b14  08bf      it	eq
08022b16  43f00103  orreq	r3, r3, #1
08022b1a  3030      adds	r0, #48
08022b1c  8a42      cmp	r2, r1
08022b1e  08bf      it	eq
08022b20  43f00103  orreq	r3, r3, #1
08022b24  01f55c71  add.w	r1, r1, #880
08022b28  8242      cmp	r2, r0
08022b2a  08bf      it	eq
08022b2c  43f00103  orreq	r3, r3, #1
08022b30  00f55c70  add.w	r0, r0, #880
08022b34  8a42      cmp	r2, r1
08022b36  08bf      it	eq
08022b38  43f00103  orreq	r3, r3, #1
08022b3c  3031      adds	r1, #48
08022b3e  8242      cmp	r2, r0
08022b40  08bf      it	eq
08022b42  43f00103  orreq	r3, r3, #1
08022b46  3030      adds	r0, #48
08022b48  8a42      cmp	r2, r1
08022b4a  08bf      it	eq
08022b4c  43f00103  orreq	r3, r3, #1
08022b50  3031      adds	r1, #48
08022b52  8242      cmp	r2, r0
08022b54  08bf      it	eq
08022b56  43f00103  orreq	r3, r3, #1
08022b5a  3030      adds	r0, #48
08022b5c  8a42      cmp	r2, r1
08022b5e  08bf      it	eq
08022b60  43f00103  orreq	r3, r3, #1
08022b64  3031      adds	r1, #48
08022b66  8242      cmp	r2, r0
08022b68  08bf      it	eq
08022b6a  43f00103  orreq	r3, r3, #1
08022b6e  8a42      cmp	r2, r1
08022b70  08bf      it	eq
08022b72  43f00103  orreq	r3, r3, #1
08022b76  13b9      cbnz	r3, #4 ; -> 0x08022b7e ; branch_target=0x08022b7e
08022b78  104b      ldr	r3, [pc, #64] ; [0x08022bbc] = 0x400204b8 / f32_bits_interpretation=2.03153801
08022b7a  9a42      cmp	r2, r3
08022b7c  13d1      bne	#38 ; -> 0x08022ba6 ; branch_target=0x08022ba6
08022b7e  1368      ldr	r3, [r2]
08022b80  23f01403  bic	r3, r3, #20
08022b84  1360      str	r3, [r2]
08022b86  0022      movs	r2, #0
08022b88  0123      movs	r3, #1
08022b8a  84f83420  strb.w	r2, [r4, #52]
08022b8e  84f83530  strb.w	r3, [r4, #53]
08022b92  e36b      ldr	r3, [r4, #60]
08022b94  002b      cmp	r3, #0
08022b96  7ff423ae  bne.w	#-954 ; -> 0x080227e0 ; branch_target=0x080227e0
08022b9a  7ee4      b	#-1796 ; -> 0x0802249a ; branch_target=0x0802249a
08022b9c  1368      ldr	r3, [r2]
08022b9e  23f00403  bic	r3, r3, #4
08022ba2  1360      str	r3, [r2]
08022ba4  93e7      b	#-218 ; -> 0x08022ace ; branch_target=0x08022ace
08022ba6  1368      ldr	r3, [r2]
08022ba8  23f00a03  bic	r3, r3, #10
08022bac  1360      str	r3, [r2]
08022bae  eae7      b	#-44 ; -> 0x08022b86 ; branch_target=0x08022b86
