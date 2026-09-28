; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08026388  10b4      push	{r4}
0802638a  c26d      ldr	r2, [r0, #92]
0802638c  4ff0000c  mov.w	r12, #0
08026390  0346      mov	r3, r0
08026392  920f      lsrs	r2, r2, #30
08026394  0a70      strb	r2, [r1]
08026396  c26d      ldr	r2, [r0, #92]
08026398  c2f38362  ubfx	r2, r2, #26, #4
0802639c  4a70      strb	r2, [r1, #1]
0802639e  90f85f20  ldrb.w	r2, [r0, #95]
080263a2  02f00302  and	r2, r2, #3
080263a6  8a70      strb	r2, [r1, #2]
080263a8  90f85e20  ldrb.w	r2, [r0, #94]
080263ac  ca70      strb	r2, [r1, #3]
080263ae  90f85d20  ldrb.w	r2, [r0, #93]
080263b2  0a71      strb	r2, [r1, #4]
080263b4  90f85c20  ldrb.w	r2, [r0, #92]
080263b8  4a71      strb	r2, [r1, #5]
080263ba  026e      ldr	r2, [r0, #96]
080263bc  120d      lsrs	r2, r2, #20
080263be  ca80      strh	r2, [r1, #6]
080263c0  b0f86220  ldrh.w	r2, [r0, #98]
080263c4  02f00f02  and	r2, r2, #15
080263c8  0a72      strb	r2, [r1, #8]
080263ca  026e      ldr	r2, [r0, #96]
080263cc  c2f3c032  ubfx	r2, r2, #15, #1
080263d0  4a72      strb	r2, [r1, #9]
080263d2  026e      ldr	r2, [r0, #96]
080263d4  c2f38032  ubfx	r2, r2, #14, #1
080263d8  8a72      strb	r2, [r1, #10]
080263da  026e      ldr	r2, [r0, #96]
080263dc  c2f34032  ubfx	r2, r2, #13, #1
080263e0  ca72      strb	r2, [r1, #11]
080263e2  026e      ldr	r2, [r0, #96]
080263e4  c2f30032  ubfx	r2, r2, #12, #1
080263e8  0a73      strb	r2, [r1, #12]
080263ea  81f80dc0  strb.w	r12, [r1, #13]
080263ee  826b      ldr	r2, [r0, #56]
080263f0  002a      cmp	r2, #0
080263f2  40f08a80  bne.w	#276 ; -> 0x0802650a ; branch_target=0x0802650a
080263f6  046e      ldr	r4, [r0, #96]
080263f8  40f6fc72  movw	r2, #4092
080263fc  406e      ldr	r0, [r0, #100]
080263fe  02ea8402  and.w	r2, r2, r4, lsl #2
08026402  42ea9072  orr.w	r2, r2, r0, lsr #30
08026406  0a61      str	r2, [r1, #16]
08026408  5a6e      ldr	r2, [r3, #100]
0802640a  c2f3c262  ubfx	r2, r2, #27, #3
0802640e  0a75      strb	r2, [r1, #20]
08026410  93f86720  ldrb.w	r2, [r3, #103]
08026414  02f00702  and	r2, r2, #7
08026418  4a75      strb	r2, [r1, #21]
0802641a  5a6e      ldr	r2, [r3, #100]
0802641c  c2f34252  ubfx	r2, r2, #21, #3
08026420  8a75      strb	r2, [r1, #22]
08026422  5a6e      ldr	r2, [r3, #100]
08026424  c2f38242  ubfx	r2, r2, #18, #3
08026428  ca75      strb	r2, [r1, #23]
0802642a  5a6e      ldr	r2, [r3, #100]
0802642c  c2f3c232  ubfx	r2, r2, #15, #3
08026430  0a76      strb	r2, [r1, #24]
08026432  0a69      ldr	r2, [r1, #16]
08026434  0132      adds	r2, #1
08026436  9a64      str	r2, [r3, #72]
08026438  087e      ldrb	r0, [r1, #24]
0802643a  00f00700  and	r0, r0, #7
0802643e  0230      adds	r0, #2
08026440  8240      lsls	r2, r0
08026442  0120      movs	r0, #1
08026444  9a64      str	r2, [r3, #72]
08026446  91f808c0  ldrb.w	r12, [r1, #8]
0802644a  0cf00f0c  and	r12, r12, #15
0802644e  00fa0cf0  lsl.w	r0, r0, r12
08026452  4fea502c  lsr.w	r12, r0, #9
08026456  d864      str	r0, [r3, #76]
08026458  02fb0cf2  mul	r2, r2, r12
0802645c  1a65      str	r2, [r3, #80]
0802645e  5a6e      ldr	r2, [r3, #100]
08026460  4ff40074  mov.w	r4, #512
08026464  0020      movs	r0, #0
08026466  4ff0010c  mov.w	r12, #1
0802646a  c2f38032  ubfx	r2, r2, #14, #1
0802646e  5c65      str	r4, [r3, #84]
08026470  4a76      strb	r2, [r1, #25]
08026472  5a6e      ldr	r2, [r3, #100]
08026474  c2f3c612  ubfx	r2, r2, #7, #7
08026478  8a76      strb	r2, [r1, #26]
0802647a  5a6e      ldr	r2, [r3, #100]
0802647c  02f07f02  and	r2, r2, #127
08026480  ca76      strb	r2, [r1, #27]
08026482  9a6e      ldr	r2, [r3, #104]
08026484  d20f      lsrs	r2, r2, #31
08026486  0a77      strb	r2, [r1, #28]
08026488  9a6e      ldr	r2, [r3, #104]
0802648a  c2f34172  ubfx	r2, r2, #29, #2
0802648e  4a77      strb	r2, [r1, #29]
08026490  9a6e      ldr	r2, [r3, #104]
08026492  c2f38262  ubfx	r2, r2, #26, #3
08026496  8a77      strb	r2, [r1, #30]
08026498  9a6e      ldr	r2, [r3, #104]
0802649a  c2f38352  ubfx	r2, r2, #22, #4
0802649e  ca77      strb	r2, [r1, #31]
080264a0  9a6e      ldr	r2, [r3, #104]
080264a2  c2f34052  ubfx	r2, r2, #21, #1
080264a6  81f82020  strb.w	r2, [r1, #32]
080264aa  81f82100  strb.w	r0, [r1, #33]
080264ae  b3f86a20  ldrh.w	r2, [r3, #106]
080264b2  02ea0c02  and.w	r2, r2, r12
080264b6  81f82220  strb.w	r2, [r1, #34]
080264ba  9a6e      ldr	r2, [r3, #104]
080264bc  c2f3c032  ubfx	r2, r2, #15, #1
080264c0  81f82320  strb.w	r2, [r1, #35]
080264c4  9a6e      ldr	r2, [r3, #104]
080264c6  c2f38032  ubfx	r2, r2, #14, #1
080264ca  81f82420  strb.w	r2, [r1, #36]
080264ce  9a6e      ldr	r2, [r3, #104]
080264d0  c2f34032  ubfx	r2, r2, #13, #1
080264d4  81f82520  strb.w	r2, [r1, #37]
080264d8  9a6e      ldr	r2, [r3, #104]
080264da  c2f30032  ubfx	r2, r2, #12, #1
080264de  81f82620  strb.w	r2, [r1, #38]
080264e2  9a6e      ldr	r2, [r3, #104]
080264e4  c2f38122  ubfx	r2, r2, #10, #2
080264e8  81f82720  strb.w	r2, [r1, #39]
080264ec  9a6e      ldr	r2, [r3, #104]
080264ee  c2f30122  ubfx	r2, r2, #8, #2
080264f2  81f82820  strb.w	r2, [r1, #40]
080264f6  9b6e      ldr	r3, [r3, #104]
080264f8  c3f34603  ubfx	r3, r3, #1, #7
080264fc  81f82930  strb.w	r3, [r1, #41]
08026500  81f82ac0  strb.w	r12, [r1, #42]
08026504  5df8044b  ldr	r4, [sp], #4
08026508  7047      bx	lr
0802650a  012a      cmp	r2, #1
0802650c  10d1      bne	#32 ; -> 0x08026530 ; branch_target=0x08026530
0802650e  026e      ldr	r2, [r0, #96]
08026510  b0f86640  ldrh.w	r4, [r0, #102]
08026514  4ff40070  mov.w	r0, #512
08026518  1204      lsls	r2, r2, #16
0802651a  02f47c12  and	r2, r2, #4128768
0802651e  2243      orrs	r2, r4
08026520  0a61      str	r2, [r1, #16]
08026522  0a69      ldr	r2, [r1, #16]
08026524  d864      str	r0, [r3, #76]
08026526  0132      adds	r2, #1
08026528  9202      lsls	r2, r2, #10
0802652a  9a64      str	r2, [r3, #72]
0802652c  1a65      str	r2, [r3, #80]
0802652e  96e7      b	#-212 ; -> 0x0802645e ; branch_target=0x0802645e
08026530  0268      ldr	r2, [r0]
08026532  0121      movs	r1, #1
08026534  0548      ldr	r0, [pc, #20] ; [0x0802654c] = 0x1fe00fff
08026536  9063      str	r0, [r2, #56]
08026538  0846      mov	r0, r1
0802653a  5a6b      ldr	r2, [r3, #52]
0802653c  42f08052  orr	r2, r2, #268435456
08026540  5a63      str	r2, [r3, #52]
08026542  83f83010  strb.w	r1, [r3, #48]
08026546  5df8044b  ldr	r4, [sp], #4
0802654a  7047      bx	lr
