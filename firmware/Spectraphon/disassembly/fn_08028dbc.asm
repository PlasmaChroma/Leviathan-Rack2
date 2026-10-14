; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028dbc  4f4a      ldr	r2, [pc, #316] ; [0x08028efc] = 0x20000014
08028dbe  8446      mov	r12, r0
08028dc0  4f4b      ldr	r3, [pc, #316] ; [0x08028f00] = 0x10624dd3
08028dc2  1268      ldr	r2, [r2]
08028dc4  ccf80810  str.w	r1, [r12, #8]
08028dc8  a3fb0232  umull	r3, r2, r3, r2
08028dcc  dcf80c10  ldr.w	r1, [r12, #12]
08028dd0  4c48      ldr	r0, [pc, #304] ; [0x08028f04] = 0xfffee0c0
08028dd2  41f20713  movw	r3, #4359
08028dd6  520a      lsrs	r2, r2, #9
08028dd8  0840      ands	r0, r1
08028dda  41f28831  movw	r1, #5000
08028dde  0343      orrs	r3, r0
08028de0  01fb02f2  mul	r2, r1, r2
08028de4  ccf80c30  str.w	r3, [r12, #12]
08028de8  531e      subs	r3, r2, #1
08028dea  4ab1      cbz	r2, #18 ; -> 0x08028e00 ; branch_target=0x08028e00
08028dec  4649      ldr	r1, [pc, #280] ; [0x08028f08] = 0x00200045
08028dee  dcf83420  ldr.w	r2, [r12, #52]
08028df2  013b      subs	r3, #1
08028df4  0a42      tst	r2, r1
08028df6  01d0      beq	#2 ; -> 0x08028dfc ; branch_target=0x08028dfc
08028df8  9204      lsls	r2, r2, #18
08028dfa  04d5      bpl	#8 ; -> 0x08028e06 ; branch_target=0x08028e06
08028dfc  581c      adds	r0, r3, #1
08028dfe  f6d1      bne	#-20 ; -> 0x08028dee ; branch_target=0x08028dee
08028e00  4ff00040  mov.w	r0, #2147483648
08028e04  7047      bx	lr
08028e06  dcf83430  ldr.w	r3, [r12, #52]
08028e0a  5b07      lsls	r3, r3, #29
08028e0c  43d4      bmi	#134 ; -> 0x08028e96 ; branch_target=0x08028e96
08028e0e  dcf83430  ldr.w	r3, [r12, #52]
08028e12  d807      lsls	r0, r3, #31
08028e14  09d4      bmi	#18 ; -> 0x08028e2a ; branch_target=0x08028e2a
08028e16  3d4b      ldr	r3, [pc, #244] ; [0x08028f0c] = 0x002000c5
08028e18  ccf83830  str.w	r3, [r12, #56]
08028e1c  dcf81030  ldr.w	r3, [r12, #16]
08028e20  dbb2      uxtb	r3, r3
08028e22  072b      cmp	r3, #7
08028e24  05d0      beq	#10 ; -> 0x08028e32 ; branch_target=0x08028e32
08028e26  0120      movs	r0, #1
08028e28  7047      bx	lr
08028e2a  0123      movs	r3, #1
08028e2c  ccf83830  str.w	r3, [r12, #56]
08028e30  f9e7      b	#-14 ; -> 0x08028e26 ; branch_target=0x08028e26
08028e32  dcf81430  ldr.w	r3, [r12, #20]
08028e36  3648      ldr	r0, [pc, #216] ; [0x08028f10] = 0xfdffe008
08028e38  1840      ands	r0, r3
08028e3a  58b3      cbz	r0, #86 ; -> 0x08028e94 ; branch_target=0x08028e94
08028e3c  002b      cmp	r3, #0
08028e3e  2fdb      blt	#94 ; -> 0x08028ea0 ; branch_target=0x08028ea0
08028e40  5900      lsls	r1, r3, #1
08028e42  30d4      bmi	#96 ; -> 0x08028ea6 ; branch_target=0x08028ea6
08028e44  9a00      lsls	r2, r3, #2
08028e46  30d4      bmi	#96 ; -> 0x08028eaa ; branch_target=0x08028eaa
08028e48  d900      lsls	r1, r3, #3
08028e4a  30d4      bmi	#96 ; -> 0x08028eae ; branch_target=0x08028eae
08028e4c  1a01      lsls	r2, r3, #4
08028e4e  31d4      bmi	#98 ; -> 0x08028eb4 ; branch_target=0x08028eb4
08028e50  5901      lsls	r1, r3, #5
08028e52  32d4      bmi	#100 ; -> 0x08028eba ; branch_target=0x08028eba
08028e54  da01      lsls	r2, r3, #7
08028e56  33d4      bmi	#102 ; -> 0x08028ec0 ; branch_target=0x08028ec0
08028e58  1902      lsls	r1, r3, #8
08028e5a  34d4      bmi	#104 ; -> 0x08028ec6 ; branch_target=0x08028ec6
08028e5c  5a02      lsls	r2, r3, #9
08028e5e  35d4      bmi	#106 ; -> 0x08028ecc ; branch_target=0x08028ecc
08028e60  9902      lsls	r1, r3, #10
08028e62  3fd4      bmi	#126 ; -> 0x08028ee4 ; branch_target=0x08028ee4
08028e64  da02      lsls	r2, r3, #11
08028e66  3ad4      bmi	#116 ; -> 0x08028ede ; branch_target=0x08028ede
08028e68  5903      lsls	r1, r3, #13
08028e6a  35d4      bmi	#106 ; -> 0x08028ed8 ; branch_target=0x08028ed8
08028e6c  9a03      lsls	r2, r3, #14
08028e6e  30d4      bmi	#96 ; -> 0x08028ed2 ; branch_target=0x08028ed2
08028e70  d903      lsls	r1, r3, #15
08028e72  40d4      bmi	#128 ; -> 0x08028ef6 ; branch_target=0x08028ef6
08028e74  1a04      lsls	r2, r3, #16
08028e76  3bd4      bmi	#118 ; -> 0x08028ef0 ; branch_target=0x08028ef0
08028e78  5904      lsls	r1, r3, #17
08028e7a  36d4      bmi	#108 ; -> 0x08028eea ; branch_target=0x08028eea
08028e7c  9a04      lsls	r2, r3, #18
08028e7e  07d4      bmi	#14 ; -> 0x08028e90 ; branch_target=0x08028e90
08028e80  13f0080f  tst.w	r3, #8
08028e84  0cbf      ite	eq
08028e86  4ff48030  moveq.w	r0, #65536
08028e8a  4ff40000  movne.w	r0, #8388608
08028e8e  7047      bx	lr
08028e90  4ff48000  mov.w	r0, #4194304
08028e94  7047      bx	lr
08028e96  0423      movs	r3, #4
08028e98  1846      mov	r0, r3
08028e9a  ccf83830  str.w	r3, [r12, #56]
08028e9e  7047      bx	lr
08028ea0  4ff00070  mov.w	r0, #33554432
08028ea4  7047      bx	lr
08028ea6  4020      movs	r0, #64
08028ea8  7047      bx	lr
08028eaa  8020      movs	r0, #128
08028eac  7047      bx	lr
08028eae  4ff48070  mov.w	r0, #256
08028eb2  7047      bx	lr
08028eb4  4ff40070  mov.w	r0, #512
08028eb8  7047      bx	lr
08028eba  4ff48060  mov.w	r0, #1024
08028ebe  7047      bx	lr
08028ec0  4ff40060  mov.w	r0, #2048
08028ec4  7047      bx	lr
08028ec6  4ff48050  mov.w	r0, #4096
08028eca  7047      bx	lr
08028ecc  4ff40050  mov.w	r0, #8192
08028ed0  7047      bx	lr
08028ed2  4ff48020  mov.w	r0, #262144
08028ed6  7047      bx	lr
08028ed8  4ff40030  mov.w	r0, #131072
08028edc  7047      bx	lr
08028ede  4ff40040  mov.w	r0, #32768
08028ee2  7047      bx	lr
08028ee4  4ff48040  mov.w	r0, #16384
08028ee8  7047      bx	lr
08028eea  4ff40010  mov.w	r0, #2097152
08028eee  7047      bx	lr
08028ef0  4ff48010  mov.w	r0, #1048576
08028ef4  7047      bx	lr
08028ef6  4ff40020  mov.w	r0, #524288
08028efa  7047      bx	lr
