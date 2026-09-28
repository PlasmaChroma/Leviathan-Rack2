; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
0802755c  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08027560  0268      ldr	r2, [r0]
08027562  0446      mov	r4, r0
08027564  1169      ldr	r1, [r2, #16]
08027566  5369      ldr	r3, [r2, #20]
08027568  9768      ldr	r7, [r2, #8]
0802756a  01ea0305  and.w	r5, r1, r3
0802756e  90f88160  ldrb.w	r6, [r0, #129]
08027572  05f0640c  and	r12, r5, #100
08027576  bcf1040f  cmp.w	r12, #4
0802757a  32d0      beq	#100 ; -> 0x080275e2 ; branch_target=0x080275e2
0802757c  05f04500  and	r0, r5, #69
08027580  0128      cmp	r0, #1
08027582  37d0      beq	#110 ; -> 0x080275f4 ; branch_target=0x080275f4
08027584  05f02600  and	r0, r5, #38
08027588  0228      cmp	r0, #2
0802758a  00f0ee80  beq.w	#476 ; -> 0x0802776a ; branch_target=0x0802776a
0802758e  2807      lsls	r0, r5, #28
08027590  76d5      bpl	#236 ; -> 0x08027680 ; branch_target=0x08027680
08027592  9369      ldr	r3, [r2, #24]
08027594  17f4404f  tst.w	r7, #49152
08027598  f6b2      uxtb	r6, r6
0802759a  43f00803  orr	r3, r3, #8
0802759e  9361      str	r3, [r2, #24]
080275a0  2168      ldr	r1, [r4]
080275a2  8b69      ldr	r3, [r1, #24]
080275a4  43f01003  orr	r3, r3, #16
080275a8  8b61      str	r3, [r1, #24]
080275aa  2168      ldr	r1, [r4]
080275ac  8b69      ldr	r3, [r1, #24]
080275ae  43f40063  orr	r3, r3, #2048
080275b2  8b61      str	r3, [r1, #24]
080275b4  2168      ldr	r1, [r4]
080275b6  0b69      ldr	r3, [r1, #16]
080275b8  23f00803  bic	r3, r3, #8
080275bc  0b61      str	r3, [r1, #16]
080275be  23d0      beq	#70 ; -> 0x08027608 ; branch_target=0x08027608
080275c0  042e      cmp	r6, #4
080275c2  00f0ea80  beq.w	#468 ; -> 0x0802779a ; branch_target=0x0802779a
080275c6  a36f      ldr	r3, [r4, #120]
080275c8  db69      ldr	r3, [r3, #28]
080275ca  ebb1      cbz	r3, #58 ; -> 0x08027608 ; branch_target=0x08027608
080275cc  032e      cmp	r6, #3
080275ce  53d0      beq	#166 ; -> 0x08027678 ; branch_target=0x08027678
080275d0  e36f      ldr	r3, [r4, #124]
080275d2  db69      ldr	r3, [r3, #28]
080275d4  c3b1      cbz	r3, #48 ; -> 0x08027608 ; branch_target=0x08027608
080275d6  052e      cmp	r6, #5
080275d8  14d1      bne	#40 ; -> 0x08027604 ; branch_target=0x08027604
080275da  2046      mov	r0, r4
080275dc  fff7baff  bl	#-140 ; -> 0x08027554 ; branch_target=0x08027554
080275e0  10e0      b	#32 ; -> 0x08027604 ; branch_target=0x08027604
080275e2  436f      ldr	r3, [r0, #116]
080275e4  9847      blx	r3
080275e6  236f      ldr	r3, [r4, #112]
080275e8  2046      mov	r0, r4
080275ea  9847      blx	r3
080275ec  05f04503  and	r3, r5, #69
080275f0  012b      cmp	r3, #1
080275f2  02d1      bne	#4 ; -> 0x080275fa ; branch_target=0x080275fa
080275f4  236f      ldr	r3, [r4, #112]
080275f6  2046      mov	r0, r4
080275f8  9847      blx	r3
080275fa  05f02605  and	r5, r5, #38
080275fe  022d      cmp	r5, #2
08027600  00f0b380  beq.w	#358 ; -> 0x0802776a ; branch_target=0x0802776a
08027604  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08027608  2368      ldr	r3, [r4]
0802760a  9b68      ldr	r3, [r3, #8]
0802760c  13f4404f  tst.w	r3, #49152
08027610  20d1      bne	#64 ; -> 0x08027654 ; branch_target=0x08027654
08027612  b4f86a30  ldrh.w	r3, [r4, #106]
08027616  9bb2      uxth	r3, r3
08027618  e3b1      cbz	r3, #56 ; -> 0x08027654 ; branch_target=0x08027654
0802761a  616e      ldr	r1, [r4, #100]
0802761c  0fe0      b	#30 ; -> 0x0802763e ; branch_target=0x0802763e
0802761e  2368      ldr	r3, [r4]
08027620  1b6b      ldr	r3, [r3, #48]
08027622  0b60      str	r3, [r1]
08027624  616e      ldr	r1, [r4, #100]
08027626  0431      adds	r1, #4
08027628  b4f86a30  ldrh.w	r3, [r4, #106]
0802762c  6166      str	r1, [r4, #100]
0802762e  013b      subs	r3, #1
08027630  9bb2      uxth	r3, r3
08027632  a4f86a30  strh.w	r3, [r4, #106]
08027636  b4f86a30  ldrh.w	r3, [r4, #106]
0802763a  9bb2      uxth	r3, r3
0802763c  53b1      cbz	r3, #20 ; -> 0x08027654 ; branch_target=0x08027654
0802763e  e368      ldr	r3, [r4, #12]
08027640  0f2b      cmp	r3, #15
08027642  ecd8      bhi	#-40 ; -> 0x0802761e ; branch_target=0x0802761e
08027644  072b      cmp	r3, #7
08027646  40f29c80  bls.w	#312 ; -> 0x08027782 ; branch_target=0x08027782
0802764a  138e      ldrh	r3, [r2, #48]
0802764c  0b80      strh	r3, [r1]
0802764e  616e      ldr	r1, [r4, #100]
08027650  0231      adds	r1, #2
08027652  e9e7      b	#-46 ; -> 0x08027628 ; branch_target=0x08027628
08027654  2046      mov	r0, r4
08027656  fff715fb  bl	#-2518 ; -> 0x08026c84 ; branch_target=0x08026c84
0802765a  0123      movs	r3, #1
0802765c  84f88130  strb.w	r3, [r4, #129]
08027660  d4f88430  ldr.w	r3, [r4, #132]
08027664  002b      cmp	r3, #0
08027666  40f08980  bne.w	#274 ; -> 0x0802777c ; branch_target=0x0802777c
0802766a  052e      cmp	r6, #5
0802766c  b5d0      beq	#-150 ; -> 0x080275da ; branch_target=0x080275da
0802766e  042e      cmp	r6, #4
08027670  00f09880  beq.w	#304 ; -> 0x080277a4 ; branch_target=0x080277a4
08027674  032e      cmp	r6, #3
08027676  c5d1      bne	#-118 ; -> 0x08027604 ; branch_target=0x08027604
08027678  2046      mov	r0, r4
0802767a  fff767ff  bl	#-306 ; -> 0x0802754c ; branch_target=0x0802754c
0802767e  c1e7      b	#-126 ; -> 0x08027604 ; branch_target=0x08027604
08027680  1b05      lsls	r3, r3, #20
08027682  02d5      bpl	#4 ; -> 0x0802768a ; branch_target=0x0802768a
08027684  0e07      lsls	r6, r1, #28
08027686  00f18380  bmi.w	#262 ; -> 0x08027790 ; branch_target=0x08027790
0802768a  15f4587f  tst.w	r5, #864
0802768e  b9d0      beq	#-142 ; -> 0x08027604 ; branch_target=0x08027604
08027690  6806      lsls	r0, r5, #25
08027692  09d5      bpl	#18 ; -> 0x080276a8 ; branch_target=0x080276a8
08027694  d4f88430  ldr.w	r3, [r4, #132]
08027698  43f00403  orr	r3, r3, #4
0802769c  c4f88430  str.w	r3, [r4, #132]
080276a0  9369      ldr	r3, [r2, #24]
080276a2  43f04003  orr	r3, r3, #64
080276a6  9361      str	r3, [r2, #24]
080276a8  a905      lsls	r1, r5, #22
080276aa  0ad5      bpl	#20 ; -> 0x080276c2 ; branch_target=0x080276c2
080276ac  d4f88430  ldr.w	r3, [r4, #132]
080276b0  2268      ldr	r2, [r4]
080276b2  43f00103  orr	r3, r3, #1
080276b6  c4f88430  str.w	r3, [r4, #132]
080276ba  9369      ldr	r3, [r2, #24]
080276bc  43f40073  orr	r3, r3, #512
080276c0  9361      str	r3, [r2, #24]
080276c2  ea05      lsls	r2, r5, #23
080276c4  0ad5      bpl	#20 ; -> 0x080276dc ; branch_target=0x080276dc
080276c6  d4f88430  ldr.w	r3, [r4, #132]
080276ca  2268      ldr	r2, [r4]
080276cc  43f00803  orr	r3, r3, #8
080276d0  c4f88430  str.w	r3, [r4, #132]
080276d4  9369      ldr	r3, [r2, #24]
080276d6  43f48073  orr	r3, r3, #256
080276da  9361      str	r3, [r2, #24]
080276dc  ab06      lsls	r3, r5, #26
080276de  0ad5      bpl	#20 ; -> 0x080276f6 ; branch_target=0x080276f6
080276e0  d4f88430  ldr.w	r3, [r4, #132]
080276e4  2268      ldr	r2, [r4]
080276e6  43f08003  orr	r3, r3, #128
080276ea  c4f88430  str.w	r3, [r4, #132]
080276ee  9369      ldr	r3, [r2, #24]
080276f0  43f02003  orr	r3, r3, #32
080276f4  9361      str	r3, [r2, #24]
080276f6  d4f88430  ldr.w	r3, [r4, #132]
080276fa  002b      cmp	r3, #0
080276fc  82d0      beq	#-252 ; -> 0x08027604 ; branch_target=0x08027604
080276fe  2268      ldr	r2, [r4]
08027700  07f44047  and	r7, r7, #49152
08027704  1368      ldr	r3, [r2]
08027706  b7f5404f  cmp.w	r7, #49152
0802770a  23f00103  bic	r3, r3, #1
0802770e  1360      str	r3, [r2]
08027710  2268      ldr	r2, [r4]
08027712  264b      ldr	r3, [pc, #152] ; [0x080277ac] = 0xfffffc94
08027714  1169      ldr	r1, [r2, #16]
08027716  03ea0103  and.w	r3, r3, r1
0802771a  1361      str	r3, [r2, #16]
0802771c  2ad1      bne	#84 ; -> 0x08027774 ; branch_target=0x08027774
0802771e  2268      ldr	r2, [r4]
08027720  9368      ldr	r3, [r2, #8]
08027722  23f44043  bic	r3, r3, #49152
08027726  9360      str	r3, [r2, #8]
08027728  e36f      ldr	r3, [r4, #124]
0802772a  5bb1      cbz	r3, #22 ; -> 0x08027744 ; branch_target=0x08027744
0802772c  204a      ldr	r2, [pc, #128] ; [0x080277b0] = 0x080277b5
0802772e  1a65      str	r2, [r3, #80]
08027730  e06f      ldr	r0, [r4, #124]
08027732  faf717fd  bl	#-21970 ; -> 0x08022164 ; branch_target=0x08022164
08027736  28b1      cbz	r0, #10 ; -> 0x08027744 ; branch_target=0x08027744
08027738  d4f88430  ldr.w	r3, [r4, #132]
0802773c  43f04003  orr	r3, r3, #64
08027740  c4f88430  str.w	r3, [r4, #132]
08027744  a36f      ldr	r3, [r4, #120]
08027746  002b      cmp	r3, #0
08027748  3ff45caf  beq.w	#-328 ; -> 0x08027604 ; branch_target=0x08027604
0802774c  184a      ldr	r2, [pc, #96] ; [0x080277b0] = 0x080277b5
0802774e  1a65      str	r2, [r3, #80]
08027750  a06f      ldr	r0, [r4, #120]
08027752  faf707fd  bl	#-22002 ; -> 0x08022164 ; branch_target=0x08022164
08027756  0028      cmp	r0, #0
08027758  3ff454af  beq.w	#-344 ; -> 0x08027604 ; branch_target=0x08027604
0802775c  d4f88430  ldr.w	r3, [r4, #132]
08027760  43f04003  orr	r3, r3, #64
08027764  c4f88430  str.w	r3, [r4, #132]
08027768  4ce7      b	#-360 ; -> 0x08027604 ; branch_target=0x08027604
0802776a  636f      ldr	r3, [r4, #116]
0802776c  2046      mov	r0, r4
0802776e  bde8f041  pop.w	{r4, r5, r6, r7, r8, lr}
08027772  1847      bx	r3
08027774  0123      movs	r3, #1
08027776  2046      mov	r0, r4
08027778  84f88130  strb.w	r3, [r4, #129]
0802777c  fff7ecfe  bl	#-552 ; -> 0x08027558 ; branch_target=0x08027558
08027780  40e7      b	#-384 ; -> 0x08027604 ; branch_target=0x08027604
08027782  2368      ldr	r3, [r4]
08027784  93f83030  ldrb.w	r3, [r3, #48]
08027788  0b70      strb	r3, [r1]
0802778a  616e      ldr	r1, [r4, #100]
0802778c  0131      adds	r1, #1
0802778e  4be7      b	#-362 ; -> 0x08027628 ; branch_target=0x08027628
08027790  9369      ldr	r3, [r2, #24]
08027792  43f40063  orr	r3, r3, #2048
08027796  9361      str	r3, [r2, #24]
08027798  34e7      b	#-408 ; -> 0x08027604 ; branch_target=0x08027604
0802779a  e36f      ldr	r3, [r4, #124]
0802779c  db69      ldr	r3, [r3, #28]
0802779e  002b      cmp	r3, #0
080277a0  3ff432af  beq.w	#-412 ; -> 0x08027608 ; branch_target=0x08027608
080277a4  2046      mov	r0, r4
080277a6  fff7d3fe  bl	#-602 ; -> 0x08027550 ; branch_target=0x08027550
080277aa  2be7      b	#-426 ; -> 0x08027604 ; branch_target=0x08027604
