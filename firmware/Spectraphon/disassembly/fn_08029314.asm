; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029314  10b4      push	{r4}
08029316  0146      mov	r1, r0
08029318  0024      movs	r4, #0
0802931a  4b4a      ldr	r2, [pc, #300] ; [0x08029448] = 0x20000014
0802931c  4b4b      ldr	r3, [pc, #300] ; [0x0802944c] = 0x10624dd3
0802931e  8c60      str	r4, [r1, #8]
08029320  1268      ldr	r2, [r2]
08029322  cc68      ldr	r4, [r1, #12]
08029324  4a48      ldr	r0, [pc, #296] ; [0x08029450] = 0xfffee0c0
08029326  a3fb0232  umull	r3, r2, r3, r2
0802932a  41f23313  movw	r3, #4403
0802932e  2040      ands	r0, r4
08029330  520a      lsrs	r2, r2, #9
08029332  0343      orrs	r3, r0
08029334  41f28830  movw	r0, #5000
08029338  00fb02f2  mul	r2, r0, r2
0802933c  cb60      str	r3, [r1, #12]
0802933e  4ab1      cbz	r2, #18 ; -> 0x08029354 ; branch_target=0x08029354
08029340  531e      subs	r3, r2, #1
08029342  4448      ldr	r0, [pc, #272] ; [0x08029454] = 0x00200045
08029344  4a6b      ldr	r2, [r1, #52]
08029346  013b      subs	r3, #1
08029348  0242      tst	r2, r0
0802934a  01d0      beq	#2 ; -> 0x08029350 ; branch_target=0x08029350
0802934c  9204      lsls	r2, r2, #18
0802934e  06d5      bpl	#12 ; -> 0x0802935e ; branch_target=0x0802935e
08029350  5c1c      adds	r4, r3, #1
08029352  f7d1      bne	#-18 ; -> 0x08029344 ; branch_target=0x08029344
08029354  4ff00040  mov.w	r0, #2147483648
08029358  5df8044b  ldr	r4, [sp], #4
0802935c  7047      bx	lr
0802935e  4b6b      ldr	r3, [r1, #52]
08029360  5c07      lsls	r4, r3, #29
08029362  3cd4      bmi	#120 ; -> 0x080293de ; branch_target=0x080293de
08029364  4b6b      ldr	r3, [r1, #52]
08029366  d807      lsls	r0, r3, #31
08029368  07d4      bmi	#14 ; -> 0x0802937a ; branch_target=0x0802937a
0802936a  3b4b      ldr	r3, [pc, #236] ; [0x08029458] = 0x002000c5
0802936c  8b63      str	r3, [r1, #56]
0802936e  0b69      ldr	r3, [r1, #16]
08029370  dbb2      uxtb	r3, r3
08029372  332b      cmp	r3, #51
08029374  04d0      beq	#8 ; -> 0x08029380 ; branch_target=0x08029380
08029376  0120      movs	r0, #1
08029378  eee7      b	#-36 ; -> 0x08029358 ; branch_target=0x08029358
0802937a  0123      movs	r3, #1
0802937c  8b63      str	r3, [r1, #56]
0802937e  fae7      b	#-12 ; -> 0x08029376 ; branch_target=0x08029376
08029380  4b69      ldr	r3, [r1, #20]
08029382  3648      ldr	r0, [pc, #216] ; [0x0802945c] = 0xfdffe008
08029384  1840      ands	r0, r3
08029386  0028      cmp	r0, #0
08029388  e6d0      beq	#-52 ; -> 0x08029358 ; branch_target=0x08029358
0802938a  002b      cmp	r3, #0
0802938c  2bdb      blt	#86 ; -> 0x080293e6 ; branch_target=0x080293e6
0802938e  5a00      lsls	r2, r3, #1
08029390  2cd4      bmi	#88 ; -> 0x080293ec ; branch_target=0x080293ec
08029392  9c00      lsls	r4, r3, #2
08029394  2cd4      bmi	#88 ; -> 0x080293f0 ; branch_target=0x080293f0
08029396  d900      lsls	r1, r3, #3
08029398  2cd4      bmi	#88 ; -> 0x080293f4 ; branch_target=0x080293f4
0802939a  1a01      lsls	r2, r3, #4
0802939c  2dd4      bmi	#90 ; -> 0x080293fa ; branch_target=0x080293fa
0802939e  5c01      lsls	r4, r3, #5
080293a0  2ed4      bmi	#92 ; -> 0x08029400 ; branch_target=0x08029400
080293a2  d901      lsls	r1, r3, #7
080293a4  2fd4      bmi	#94 ; -> 0x08029406 ; branch_target=0x08029406
080293a6  1a02      lsls	r2, r3, #8
080293a8  30d4      bmi	#96 ; -> 0x0802940c ; branch_target=0x0802940c
080293aa  5c02      lsls	r4, r3, #9
080293ac  31d4      bmi	#98 ; -> 0x08029412 ; branch_target=0x08029412
080293ae  9902      lsls	r1, r3, #10
080293b0  3bd4      bmi	#118 ; -> 0x0802942a ; branch_target=0x0802942a
080293b2  da02      lsls	r2, r3, #11
080293b4  36d4      bmi	#108 ; -> 0x08029424 ; branch_target=0x08029424
080293b6  5c03      lsls	r4, r3, #13
080293b8  31d4      bmi	#98 ; -> 0x0802941e ; branch_target=0x0802941e
080293ba  9903      lsls	r1, r3, #14
080293bc  2cd4      bmi	#88 ; -> 0x08029418 ; branch_target=0x08029418
080293be  da03      lsls	r2, r3, #15
080293c0  3fd4      bmi	#126 ; -> 0x08029442 ; branch_target=0x08029442
080293c2  1c04      lsls	r4, r3, #16
080293c4  3ad4      bmi	#116 ; -> 0x0802943c ; branch_target=0x0802943c
080293c6  5904      lsls	r1, r3, #17
080293c8  35d4      bmi	#106 ; -> 0x08029436 ; branch_target=0x08029436
080293ca  9a04      lsls	r2, r3, #18
080293cc  30d4      bmi	#96 ; -> 0x08029430 ; branch_target=0x08029430
080293ce  13f0080f  tst.w	r3, #8
080293d2  0cbf      ite	eq
080293d4  4ff48030  moveq.w	r0, #65536
080293d8  4ff40000  movne.w	r0, #8388608
080293dc  bce7      b	#-136 ; -> 0x08029358 ; branch_target=0x08029358
080293de  0423      movs	r3, #4
080293e0  1846      mov	r0, r3
080293e2  8b63      str	r3, [r1, #56]
080293e4  b8e7      b	#-144 ; -> 0x08029358 ; branch_target=0x08029358
080293e6  4ff00070  mov.w	r0, #33554432
080293ea  b5e7      b	#-150 ; -> 0x08029358 ; branch_target=0x08029358
080293ec  4020      movs	r0, #64
080293ee  b3e7      b	#-154 ; -> 0x08029358 ; branch_target=0x08029358
080293f0  8020      movs	r0, #128
080293f2  b1e7      b	#-158 ; -> 0x08029358 ; branch_target=0x08029358
080293f4  4ff48070  mov.w	r0, #256
080293f8  aee7      b	#-164 ; -> 0x08029358 ; branch_target=0x08029358
080293fa  4ff40070  mov.w	r0, #512
080293fe  abe7      b	#-170 ; -> 0x08029358 ; branch_target=0x08029358
08029400  4ff48060  mov.w	r0, #1024
08029404  a8e7      b	#-176 ; -> 0x08029358 ; branch_target=0x08029358
08029406  4ff40060  mov.w	r0, #2048
0802940a  a5e7      b	#-182 ; -> 0x08029358 ; branch_target=0x08029358
0802940c  4ff48050  mov.w	r0, #4096
08029410  a2e7      b	#-188 ; -> 0x08029358 ; branch_target=0x08029358
08029412  4ff40050  mov.w	r0, #8192
08029416  9fe7      b	#-194 ; -> 0x08029358 ; branch_target=0x08029358
08029418  4ff48020  mov.w	r0, #262144
0802941c  9ce7      b	#-200 ; -> 0x08029358 ; branch_target=0x08029358
0802941e  4ff40030  mov.w	r0, #131072
08029422  99e7      b	#-206 ; -> 0x08029358 ; branch_target=0x08029358
08029424  4ff40040  mov.w	r0, #32768
08029428  96e7      b	#-212 ; -> 0x08029358 ; branch_target=0x08029358
0802942a  4ff48040  mov.w	r0, #16384
0802942e  93e7      b	#-218 ; -> 0x08029358 ; branch_target=0x08029358
08029430  4ff48000  mov.w	r0, #4194304
08029434  90e7      b	#-224 ; -> 0x08029358 ; branch_target=0x08029358
08029436  4ff40010  mov.w	r0, #2097152
0802943a  8de7      b	#-230 ; -> 0x08029358 ; branch_target=0x08029358
0802943c  4ff48010  mov.w	r0, #1048576
08029440  8ae7      b	#-236 ; -> 0x08029358 ; branch_target=0x08029358
08029442  4ff40020  mov.w	r0, #524288
08029446  87e7      b	#-242 ; -> 0x08029358 ; branch_target=0x08029358
