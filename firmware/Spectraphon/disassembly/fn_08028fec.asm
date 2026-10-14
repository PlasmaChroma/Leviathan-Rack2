; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028fec  4f4a      ldr	r2, [pc, #316] ; [0x0802912c] = 0x20000014
08028fee  8446      mov	r12, r0
08028ff0  4f4b      ldr	r3, [pc, #316] ; [0x08029130] = 0x10624dd3
08028ff2  1268      ldr	r2, [r2]
08028ff4  ccf80810  str.w	r1, [r12, #8]
08028ff8  a3fb0232  umull	r3, r2, r3, r2
08028ffc  dcf80c10  ldr.w	r1, [r12, #12]
08029000  4c48      ldr	r0, [pc, #304] ; [0x08029134] = 0xfffee0c0
08029002  41f23713  movw	r3, #4407
08029006  520a      lsrs	r2, r2, #9
08029008  0840      ands	r0, r1
0802900a  41f28831  movw	r1, #5000
0802900e  0343      orrs	r3, r0
08029010  01fb02f2  mul	r2, r1, r2
08029014  ccf80c30  str.w	r3, [r12, #12]
08029018  531e      subs	r3, r2, #1
0802901a  4ab1      cbz	r2, #18 ; -> 0x08029030 ; branch_target=0x08029030
0802901c  4649      ldr	r1, [pc, #280] ; [0x08029138] = 0x00200045
0802901e  dcf83420  ldr.w	r2, [r12, #52]
08029022  013b      subs	r3, #1
08029024  0a42      tst	r2, r1
08029026  01d0      beq	#2 ; -> 0x0802902c ; branch_target=0x0802902c
08029028  9204      lsls	r2, r2, #18
0802902a  04d5      bpl	#8 ; -> 0x08029036 ; branch_target=0x08029036
0802902c  581c      adds	r0, r3, #1
0802902e  f6d1      bne	#-20 ; -> 0x0802901e ; branch_target=0x0802901e
08029030  4ff00040  mov.w	r0, #2147483648
08029034  7047      bx	lr
08029036  dcf83430  ldr.w	r3, [r12, #52]
0802903a  5b07      lsls	r3, r3, #29
0802903c  43d4      bmi	#134 ; -> 0x080290c6 ; branch_target=0x080290c6
0802903e  dcf83430  ldr.w	r3, [r12, #52]
08029042  d807      lsls	r0, r3, #31
08029044  09d4      bmi	#18 ; -> 0x0802905a ; branch_target=0x0802905a
08029046  3d4b      ldr	r3, [pc, #244] ; [0x0802913c] = 0x002000c5
08029048  ccf83830  str.w	r3, [r12, #56]
0802904c  dcf81030  ldr.w	r3, [r12, #16]
08029050  dbb2      uxtb	r3, r3
08029052  372b      cmp	r3, #55
08029054  05d0      beq	#10 ; -> 0x08029062 ; branch_target=0x08029062
08029056  0120      movs	r0, #1
08029058  7047      bx	lr
0802905a  0123      movs	r3, #1
0802905c  ccf83830  str.w	r3, [r12, #56]
08029060  f9e7      b	#-14 ; -> 0x08029056 ; branch_target=0x08029056
08029062  dcf81430  ldr.w	r3, [r12, #20]
08029066  3648      ldr	r0, [pc, #216] ; [0x08029140] = 0xfdffe008
08029068  1840      ands	r0, r3
0802906a  58b3      cbz	r0, #86 ; -> 0x080290c4 ; branch_target=0x080290c4
0802906c  002b      cmp	r3, #0
0802906e  2fdb      blt	#94 ; -> 0x080290d0 ; branch_target=0x080290d0
08029070  5900      lsls	r1, r3, #1
08029072  30d4      bmi	#96 ; -> 0x080290d6 ; branch_target=0x080290d6
08029074  9a00      lsls	r2, r3, #2
08029076  30d4      bmi	#96 ; -> 0x080290da ; branch_target=0x080290da
08029078  d900      lsls	r1, r3, #3
0802907a  30d4      bmi	#96 ; -> 0x080290de ; branch_target=0x080290de
0802907c  1a01      lsls	r2, r3, #4
0802907e  31d4      bmi	#98 ; -> 0x080290e4 ; branch_target=0x080290e4
08029080  5901      lsls	r1, r3, #5
08029082  32d4      bmi	#100 ; -> 0x080290ea ; branch_target=0x080290ea
08029084  da01      lsls	r2, r3, #7
08029086  33d4      bmi	#102 ; -> 0x080290f0 ; branch_target=0x080290f0
08029088  1902      lsls	r1, r3, #8
0802908a  34d4      bmi	#104 ; -> 0x080290f6 ; branch_target=0x080290f6
0802908c  5a02      lsls	r2, r3, #9
0802908e  35d4      bmi	#106 ; -> 0x080290fc ; branch_target=0x080290fc
08029090  9902      lsls	r1, r3, #10
08029092  3fd4      bmi	#126 ; -> 0x08029114 ; branch_target=0x08029114
08029094  da02      lsls	r2, r3, #11
08029096  3ad4      bmi	#116 ; -> 0x0802910e ; branch_target=0x0802910e
08029098  5903      lsls	r1, r3, #13
0802909a  35d4      bmi	#106 ; -> 0x08029108 ; branch_target=0x08029108
0802909c  9a03      lsls	r2, r3, #14
0802909e  30d4      bmi	#96 ; -> 0x08029102 ; branch_target=0x08029102
080290a0  d903      lsls	r1, r3, #15
080290a2  40d4      bmi	#128 ; -> 0x08029126 ; branch_target=0x08029126
080290a4  1a04      lsls	r2, r3, #16
080290a6  3bd4      bmi	#118 ; -> 0x08029120 ; branch_target=0x08029120
080290a8  5904      lsls	r1, r3, #17
080290aa  36d4      bmi	#108 ; -> 0x0802911a ; branch_target=0x0802911a
080290ac  9a04      lsls	r2, r3, #18
080290ae  07d4      bmi	#14 ; -> 0x080290c0 ; branch_target=0x080290c0
080290b0  13f0080f  tst.w	r3, #8
080290b4  0cbf      ite	eq
080290b6  4ff48030  moveq.w	r0, #65536
080290ba  4ff40000  movne.w	r0, #8388608
080290be  7047      bx	lr
080290c0  4ff48000  mov.w	r0, #4194304
080290c4  7047      bx	lr
080290c6  0423      movs	r3, #4
080290c8  1846      mov	r0, r3
080290ca  ccf83830  str.w	r3, [r12, #56]
080290ce  7047      bx	lr
080290d0  4ff00070  mov.w	r0, #33554432
080290d4  7047      bx	lr
080290d6  4020      movs	r0, #64
080290d8  7047      bx	lr
080290da  8020      movs	r0, #128
080290dc  7047      bx	lr
080290de  4ff48070  mov.w	r0, #256
080290e2  7047      bx	lr
080290e4  4ff40070  mov.w	r0, #512
080290e8  7047      bx	lr
080290ea  4ff48060  mov.w	r0, #1024
080290ee  7047      bx	lr
080290f0  4ff40060  mov.w	r0, #2048
080290f4  7047      bx	lr
080290f6  4ff48050  mov.w	r0, #4096
080290fa  7047      bx	lr
080290fc  4ff40050  mov.w	r0, #8192
08029100  7047      bx	lr
08029102  4ff48020  mov.w	r0, #262144
08029106  7047      bx	lr
08029108  4ff40030  mov.w	r0, #131072
0802910c  7047      bx	lr
0802910e  4ff40040  mov.w	r0, #32768
08029112  7047      bx	lr
08029114  4ff48040  mov.w	r0, #16384
08029118  7047      bx	lr
0802911a  4ff40010  mov.w	r0, #2097152
0802911e  7047      bx	lr
08029120  4ff48010  mov.w	r0, #1048576
08029124  7047      bx	lr
08029126  4ff40020  mov.w	r0, #524288
0802912a  7047      bx	lr
