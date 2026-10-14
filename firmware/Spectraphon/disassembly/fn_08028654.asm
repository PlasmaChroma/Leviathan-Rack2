; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028654  4f4a      ldr	r2, [pc, #316] ; [0x08028794] = 0x20000014
08028656  8446      mov	r12, r0
08028658  4f4b      ldr	r3, [pc, #316] ; [0x08028798] = 0x10624dd3
0802865a  1268      ldr	r2, [r2]
0802865c  ccf80810  str.w	r1, [r12, #8]
08028660  a3fb0232  umull	r3, r2, r3, r2
08028664  dcf80c10  ldr.w	r1, [r12, #12]
08028668  4c48      ldr	r0, [pc, #304] ; [0x0802879c] = 0xfffee0c0
0802866a  41f21113  movw	r3, #4369
0802866e  520a      lsrs	r2, r2, #9
08028670  0840      ands	r0, r1
08028672  41f28831  movw	r1, #5000
08028676  0343      orrs	r3, r0
08028678  01fb02f2  mul	r2, r1, r2
0802867c  ccf80c30  str.w	r3, [r12, #12]
08028680  531e      subs	r3, r2, #1
08028682  4ab1      cbz	r2, #18 ; -> 0x08028698 ; branch_target=0x08028698
08028684  4649      ldr	r1, [pc, #280] ; [0x080287a0] = 0x00200045
08028686  dcf83420  ldr.w	r2, [r12, #52]
0802868a  013b      subs	r3, #1
0802868c  0a42      tst	r2, r1
0802868e  01d0      beq	#2 ; -> 0x08028694 ; branch_target=0x08028694
08028690  9204      lsls	r2, r2, #18
08028692  04d5      bpl	#8 ; -> 0x0802869e ; branch_target=0x0802869e
08028694  581c      adds	r0, r3, #1
08028696  f6d1      bne	#-20 ; -> 0x08028686 ; branch_target=0x08028686
08028698  4ff00040  mov.w	r0, #2147483648
0802869c  7047      bx	lr
0802869e  dcf83430  ldr.w	r3, [r12, #52]
080286a2  5b07      lsls	r3, r3, #29
080286a4  43d4      bmi	#134 ; -> 0x0802872e ; branch_target=0x0802872e
080286a6  dcf83430  ldr.w	r3, [r12, #52]
080286aa  d807      lsls	r0, r3, #31
080286ac  09d4      bmi	#18 ; -> 0x080286c2 ; branch_target=0x080286c2
080286ae  3d4b      ldr	r3, [pc, #244] ; [0x080287a4] = 0x002000c5
080286b0  ccf83830  str.w	r3, [r12, #56]
080286b4  dcf81030  ldr.w	r3, [r12, #16]
080286b8  dbb2      uxtb	r3, r3
080286ba  112b      cmp	r3, #17
080286bc  05d0      beq	#10 ; -> 0x080286ca ; branch_target=0x080286ca
080286be  0120      movs	r0, #1
080286c0  7047      bx	lr
080286c2  0123      movs	r3, #1
080286c4  ccf83830  str.w	r3, [r12, #56]
080286c8  f9e7      b	#-14 ; -> 0x080286be ; branch_target=0x080286be
080286ca  dcf81430  ldr.w	r3, [r12, #20]
080286ce  3648      ldr	r0, [pc, #216] ; [0x080287a8] = 0xfdffe008
080286d0  1840      ands	r0, r3
080286d2  58b3      cbz	r0, #86 ; -> 0x0802872c ; branch_target=0x0802872c
080286d4  002b      cmp	r3, #0
080286d6  2fdb      blt	#94 ; -> 0x08028738 ; branch_target=0x08028738
080286d8  5900      lsls	r1, r3, #1
080286da  30d4      bmi	#96 ; -> 0x0802873e ; branch_target=0x0802873e
080286dc  9a00      lsls	r2, r3, #2
080286de  30d4      bmi	#96 ; -> 0x08028742 ; branch_target=0x08028742
080286e0  d900      lsls	r1, r3, #3
080286e2  30d4      bmi	#96 ; -> 0x08028746 ; branch_target=0x08028746
080286e4  1a01      lsls	r2, r3, #4
080286e6  31d4      bmi	#98 ; -> 0x0802874c ; branch_target=0x0802874c
080286e8  5901      lsls	r1, r3, #5
080286ea  32d4      bmi	#100 ; -> 0x08028752 ; branch_target=0x08028752
080286ec  da01      lsls	r2, r3, #7
080286ee  33d4      bmi	#102 ; -> 0x08028758 ; branch_target=0x08028758
080286f0  1902      lsls	r1, r3, #8
080286f2  34d4      bmi	#104 ; -> 0x0802875e ; branch_target=0x0802875e
080286f4  5a02      lsls	r2, r3, #9
080286f6  35d4      bmi	#106 ; -> 0x08028764 ; branch_target=0x08028764
080286f8  9902      lsls	r1, r3, #10
080286fa  3fd4      bmi	#126 ; -> 0x0802877c ; branch_target=0x0802877c
080286fc  da02      lsls	r2, r3, #11
080286fe  3ad4      bmi	#116 ; -> 0x08028776 ; branch_target=0x08028776
08028700  5903      lsls	r1, r3, #13
08028702  35d4      bmi	#106 ; -> 0x08028770 ; branch_target=0x08028770
08028704  9a03      lsls	r2, r3, #14
08028706  30d4      bmi	#96 ; -> 0x0802876a ; branch_target=0x0802876a
08028708  d903      lsls	r1, r3, #15
0802870a  40d4      bmi	#128 ; -> 0x0802878e ; branch_target=0x0802878e
0802870c  1a04      lsls	r2, r3, #16
0802870e  3bd4      bmi	#118 ; -> 0x08028788 ; branch_target=0x08028788
08028710  5904      lsls	r1, r3, #17
08028712  36d4      bmi	#108 ; -> 0x08028782 ; branch_target=0x08028782
08028714  9a04      lsls	r2, r3, #18
08028716  07d4      bmi	#14 ; -> 0x08028728 ; branch_target=0x08028728
08028718  13f0080f  tst.w	r3, #8
0802871c  0cbf      ite	eq
0802871e  4ff48030  moveq.w	r0, #65536
08028722  4ff40000  movne.w	r0, #8388608
08028726  7047      bx	lr
08028728  4ff48000  mov.w	r0, #4194304
0802872c  7047      bx	lr
0802872e  0423      movs	r3, #4
08028730  1846      mov	r0, r3
08028732  ccf83830  str.w	r3, [r12, #56]
08028736  7047      bx	lr
08028738  4ff00070  mov.w	r0, #33554432
0802873c  7047      bx	lr
0802873e  4020      movs	r0, #64
08028740  7047      bx	lr
08028742  8020      movs	r0, #128
08028744  7047      bx	lr
08028746  4ff48070  mov.w	r0, #256
0802874a  7047      bx	lr
0802874c  4ff40070  mov.w	r0, #512
08028750  7047      bx	lr
08028752  4ff48060  mov.w	r0, #1024
08028756  7047      bx	lr
08028758  4ff40060  mov.w	r0, #2048
0802875c  7047      bx	lr
0802875e  4ff48050  mov.w	r0, #4096
08028762  7047      bx	lr
08028764  4ff40050  mov.w	r0, #8192
08028768  7047      bx	lr
0802876a  4ff48020  mov.w	r0, #262144
0802876e  7047      bx	lr
08028770  4ff40030  mov.w	r0, #131072
08028774  7047      bx	lr
08028776  4ff40040  mov.w	r0, #32768
0802877a  7047      bx	lr
0802877c  4ff48040  mov.w	r0, #16384
08028780  7047      bx	lr
08028782  4ff40010  mov.w	r0, #2097152
08028786  7047      bx	lr
08028788  4ff48010  mov.w	r0, #1048576
0802878c  7047      bx	lr
0802878e  4ff40020  mov.w	r0, #524288
08028792  7047      bx	lr
