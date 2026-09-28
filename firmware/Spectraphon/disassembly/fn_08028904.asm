; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028904  4f4a      ldr	r2, [pc, #316] ; [0x08028a44] = 0x20000014
08028906  8446      mov	r12, r0
08028908  4f4b      ldr	r3, [pc, #316] ; [0x08028a48] = 0x10624dd3
0802890a  1268      ldr	r2, [r2]
0802890c  ccf80810  str.w	r1, [r12, #8]
08028910  a3fb0232  umull	r3, r2, r3, r2
08028914  dcf80c10  ldr.w	r1, [r12, #12]
08028918  4c48      ldr	r0, [pc, #304] ; [0x08028a4c] = 0xfffee0c0
0802891a  41f21813  movw	r3, #4376
0802891e  520a      lsrs	r2, r2, #9
08028920  0840      ands	r0, r1
08028922  41f28831  movw	r1, #5000
08028926  0343      orrs	r3, r0
08028928  01fb02f2  mul	r2, r1, r2
0802892c  ccf80c30  str.w	r3, [r12, #12]
08028930  531e      subs	r3, r2, #1
08028932  4ab1      cbz	r2, #18 ; -> 0x08028948 ; branch_target=0x08028948
08028934  4649      ldr	r1, [pc, #280] ; [0x08028a50] = 0x00200045
08028936  dcf83420  ldr.w	r2, [r12, #52]
0802893a  013b      subs	r3, #1
0802893c  0a42      tst	r2, r1
0802893e  01d0      beq	#2 ; -> 0x08028944 ; branch_target=0x08028944
08028940  9204      lsls	r2, r2, #18
08028942  04d5      bpl	#8 ; -> 0x0802894e ; branch_target=0x0802894e
08028944  581c      adds	r0, r3, #1
08028946  f6d1      bne	#-20 ; -> 0x08028936 ; branch_target=0x08028936
08028948  4ff00040  mov.w	r0, #2147483648
0802894c  7047      bx	lr
0802894e  dcf83430  ldr.w	r3, [r12, #52]
08028952  5b07      lsls	r3, r3, #29
08028954  43d4      bmi	#134 ; -> 0x080289de ; branch_target=0x080289de
08028956  dcf83430  ldr.w	r3, [r12, #52]
0802895a  d807      lsls	r0, r3, #31
0802895c  09d4      bmi	#18 ; -> 0x08028972 ; branch_target=0x08028972
0802895e  3d4b      ldr	r3, [pc, #244] ; [0x08028a54] = 0x002000c5
08028960  ccf83830  str.w	r3, [r12, #56]
08028964  dcf81030  ldr.w	r3, [r12, #16]
08028968  dbb2      uxtb	r3, r3
0802896a  182b      cmp	r3, #24
0802896c  05d0      beq	#10 ; -> 0x0802897a ; branch_target=0x0802897a
0802896e  0120      movs	r0, #1
08028970  7047      bx	lr
08028972  0123      movs	r3, #1
08028974  ccf83830  str.w	r3, [r12, #56]
08028978  f9e7      b	#-14 ; -> 0x0802896e ; branch_target=0x0802896e
0802897a  dcf81430  ldr.w	r3, [r12, #20]
0802897e  3648      ldr	r0, [pc, #216] ; [0x08028a58] = 0xfdffe008
08028980  1840      ands	r0, r3
08028982  58b3      cbz	r0, #86 ; -> 0x080289dc ; branch_target=0x080289dc
08028984  002b      cmp	r3, #0
08028986  2fdb      blt	#94 ; -> 0x080289e8 ; branch_target=0x080289e8
08028988  5900      lsls	r1, r3, #1
0802898a  30d4      bmi	#96 ; -> 0x080289ee ; branch_target=0x080289ee
0802898c  9a00      lsls	r2, r3, #2
0802898e  30d4      bmi	#96 ; -> 0x080289f2 ; branch_target=0x080289f2
08028990  d900      lsls	r1, r3, #3
08028992  30d4      bmi	#96 ; -> 0x080289f6 ; branch_target=0x080289f6
08028994  1a01      lsls	r2, r3, #4
08028996  31d4      bmi	#98 ; -> 0x080289fc ; branch_target=0x080289fc
08028998  5901      lsls	r1, r3, #5
0802899a  32d4      bmi	#100 ; -> 0x08028a02 ; branch_target=0x08028a02
0802899c  da01      lsls	r2, r3, #7
0802899e  33d4      bmi	#102 ; -> 0x08028a08 ; branch_target=0x08028a08
080289a0  1902      lsls	r1, r3, #8
080289a2  34d4      bmi	#104 ; -> 0x08028a0e ; branch_target=0x08028a0e
080289a4  5a02      lsls	r2, r3, #9
080289a6  35d4      bmi	#106 ; -> 0x08028a14 ; branch_target=0x08028a14
080289a8  9902      lsls	r1, r3, #10
080289aa  3fd4      bmi	#126 ; -> 0x08028a2c ; branch_target=0x08028a2c
080289ac  da02      lsls	r2, r3, #11
080289ae  3ad4      bmi	#116 ; -> 0x08028a26 ; branch_target=0x08028a26
080289b0  5903      lsls	r1, r3, #13
080289b2  35d4      bmi	#106 ; -> 0x08028a20 ; branch_target=0x08028a20
080289b4  9a03      lsls	r2, r3, #14
080289b6  30d4      bmi	#96 ; -> 0x08028a1a ; branch_target=0x08028a1a
080289b8  d903      lsls	r1, r3, #15
080289ba  40d4      bmi	#128 ; -> 0x08028a3e ; branch_target=0x08028a3e
080289bc  1a04      lsls	r2, r3, #16
080289be  3bd4      bmi	#118 ; -> 0x08028a38 ; branch_target=0x08028a38
080289c0  5904      lsls	r1, r3, #17
080289c2  36d4      bmi	#108 ; -> 0x08028a32 ; branch_target=0x08028a32
080289c4  9a04      lsls	r2, r3, #18
080289c6  07d4      bmi	#14 ; -> 0x080289d8 ; branch_target=0x080289d8
080289c8  13f0080f  tst.w	r3, #8
080289cc  0cbf      ite	eq
080289ce  4ff48030  moveq.w	r0, #65536
080289d2  4ff40000  movne.w	r0, #8388608
080289d6  7047      bx	lr
080289d8  4ff48000  mov.w	r0, #4194304
080289dc  7047      bx	lr
080289de  0423      movs	r3, #4
080289e0  1846      mov	r0, r3
080289e2  ccf83830  str.w	r3, [r12, #56]
080289e6  7047      bx	lr
080289e8  4ff00070  mov.w	r0, #33554432
080289ec  7047      bx	lr
080289ee  4020      movs	r0, #64
080289f0  7047      bx	lr
080289f2  8020      movs	r0, #128
080289f4  7047      bx	lr
080289f6  4ff48070  mov.w	r0, #256
080289fa  7047      bx	lr
080289fc  4ff40070  mov.w	r0, #512
08028a00  7047      bx	lr
08028a02  4ff48060  mov.w	r0, #1024
08028a06  7047      bx	lr
08028a08  4ff40060  mov.w	r0, #2048
08028a0c  7047      bx	lr
08028a0e  4ff48050  mov.w	r0, #4096
08028a12  7047      bx	lr
08028a14  4ff40050  mov.w	r0, #8192
08028a18  7047      bx	lr
08028a1a  4ff48020  mov.w	r0, #262144
08028a1e  7047      bx	lr
08028a20  4ff40030  mov.w	r0, #131072
08028a24  7047      bx	lr
08028a26  4ff40040  mov.w	r0, #32768
08028a2a  7047      bx	lr
08028a2c  4ff48040  mov.w	r0, #16384
08028a30  7047      bx	lr
08028a32  4ff40010  mov.w	r0, #2097152
08028a36  7047      bx	lr
08028a38  4ff48010  mov.w	r0, #1048576
08028a3c  7047      bx	lr
08028a3e  4ff40020  mov.w	r0, #524288
08028a42  7047      bx	lr
