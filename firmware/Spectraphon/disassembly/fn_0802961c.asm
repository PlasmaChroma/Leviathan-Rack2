; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802961c  4f4a      ldr	r2, [pc, #316] ; [0x0802975c] = 0x20000014
0802961e  8446      mov	r12, r0
08029620  4f4b      ldr	r3, [pc, #316] ; [0x08029760] = 0x10624dd3
08029622  1268      ldr	r2, [r2]
08029624  ccf80810  str.w	r1, [r12, #8]
08029628  a3fb0232  umull	r3, r2, r3, r2
0802962c  dcf80c10  ldr.w	r1, [r12, #12]
08029630  4c48      ldr	r0, [pc, #304] ; [0x08029764] = 0xfffee0c0
08029632  41f20d13  movw	r3, #4365
08029636  520a      lsrs	r2, r2, #9
08029638  0840      ands	r0, r1
0802963a  41f28831  movw	r1, #5000
0802963e  0343      orrs	r3, r0
08029640  01fb02f2  mul	r2, r1, r2
08029644  ccf80c30  str.w	r3, [r12, #12]
08029648  531e      subs	r3, r2, #1
0802964a  4ab1      cbz	r2, #18 ; -> 0x08029660 ; branch_target=0x08029660
0802964c  4649      ldr	r1, [pc, #280] ; [0x08029768] = 0x00200045
0802964e  dcf83420  ldr.w	r2, [r12, #52]
08029652  013b      subs	r3, #1
08029654  0a42      tst	r2, r1
08029656  01d0      beq	#2 ; -> 0x0802965c ; branch_target=0x0802965c
08029658  9204      lsls	r2, r2, #18
0802965a  04d5      bpl	#8 ; -> 0x08029666 ; branch_target=0x08029666
0802965c  581c      adds	r0, r3, #1
0802965e  f6d1      bne	#-20 ; -> 0x0802964e ; branch_target=0x0802964e
08029660  4ff00040  mov.w	r0, #2147483648
08029664  7047      bx	lr
08029666  dcf83430  ldr.w	r3, [r12, #52]
0802966a  5b07      lsls	r3, r3, #29
0802966c  43d4      bmi	#134 ; -> 0x080296f6 ; branch_target=0x080296f6
0802966e  dcf83430  ldr.w	r3, [r12, #52]
08029672  d807      lsls	r0, r3, #31
08029674  09d4      bmi	#18 ; -> 0x0802968a ; branch_target=0x0802968a
08029676  3d4b      ldr	r3, [pc, #244] ; [0x0802976c] = 0x002000c5
08029678  ccf83830  str.w	r3, [r12, #56]
0802967c  dcf81030  ldr.w	r3, [r12, #16]
08029680  dbb2      uxtb	r3, r3
08029682  0d2b      cmp	r3, #13
08029684  05d0      beq	#10 ; -> 0x08029692 ; branch_target=0x08029692
08029686  0120      movs	r0, #1
08029688  7047      bx	lr
0802968a  0123      movs	r3, #1
0802968c  ccf83830  str.w	r3, [r12, #56]
08029690  f9e7      b	#-14 ; -> 0x08029686 ; branch_target=0x08029686
08029692  dcf81430  ldr.w	r3, [r12, #20]
08029696  3648      ldr	r0, [pc, #216] ; [0x08029770] = 0xfdffe008
08029698  1840      ands	r0, r3
0802969a  58b3      cbz	r0, #86 ; -> 0x080296f4 ; branch_target=0x080296f4
0802969c  002b      cmp	r3, #0
0802969e  2fdb      blt	#94 ; -> 0x08029700 ; branch_target=0x08029700
080296a0  5900      lsls	r1, r3, #1
080296a2  30d4      bmi	#96 ; -> 0x08029706 ; branch_target=0x08029706
080296a4  9a00      lsls	r2, r3, #2
080296a6  30d4      bmi	#96 ; -> 0x0802970a ; branch_target=0x0802970a
080296a8  d900      lsls	r1, r3, #3
080296aa  30d4      bmi	#96 ; -> 0x0802970e ; branch_target=0x0802970e
080296ac  1a01      lsls	r2, r3, #4
080296ae  31d4      bmi	#98 ; -> 0x08029714 ; branch_target=0x08029714
080296b0  5901      lsls	r1, r3, #5
080296b2  32d4      bmi	#100 ; -> 0x0802971a ; branch_target=0x0802971a
080296b4  da01      lsls	r2, r3, #7
080296b6  33d4      bmi	#102 ; -> 0x08029720 ; branch_target=0x08029720
080296b8  1902      lsls	r1, r3, #8
080296ba  34d4      bmi	#104 ; -> 0x08029726 ; branch_target=0x08029726
080296bc  5a02      lsls	r2, r3, #9
080296be  35d4      bmi	#106 ; -> 0x0802972c ; branch_target=0x0802972c
080296c0  9902      lsls	r1, r3, #10
080296c2  3fd4      bmi	#126 ; -> 0x08029744 ; branch_target=0x08029744
080296c4  da02      lsls	r2, r3, #11
080296c6  3ad4      bmi	#116 ; -> 0x0802973e ; branch_target=0x0802973e
080296c8  5903      lsls	r1, r3, #13
080296ca  35d4      bmi	#106 ; -> 0x08029738 ; branch_target=0x08029738
080296cc  9a03      lsls	r2, r3, #14
080296ce  30d4      bmi	#96 ; -> 0x08029732 ; branch_target=0x08029732
080296d0  d903      lsls	r1, r3, #15
080296d2  40d4      bmi	#128 ; -> 0x08029756 ; branch_target=0x08029756
080296d4  1a04      lsls	r2, r3, #16
080296d6  3bd4      bmi	#118 ; -> 0x08029750 ; branch_target=0x08029750
080296d8  5904      lsls	r1, r3, #17
080296da  36d4      bmi	#108 ; -> 0x0802974a ; branch_target=0x0802974a
080296dc  9a04      lsls	r2, r3, #18
080296de  07d4      bmi	#14 ; -> 0x080296f0 ; branch_target=0x080296f0
080296e0  13f0080f  tst.w	r3, #8
080296e4  0cbf      ite	eq
080296e6  4ff48030  moveq.w	r0, #65536
080296ea  4ff40000  movne.w	r0, #8388608
080296ee  7047      bx	lr
080296f0  4ff48000  mov.w	r0, #4194304
080296f4  7047      bx	lr
080296f6  0423      movs	r3, #4
080296f8  1846      mov	r0, r3
080296fa  ccf83830  str.w	r3, [r12, #56]
080296fe  7047      bx	lr
08029700  4ff00070  mov.w	r0, #33554432
08029704  7047      bx	lr
08029706  4020      movs	r0, #64
08029708  7047      bx	lr
0802970a  8020      movs	r0, #128
0802970c  7047      bx	lr
0802970e  4ff48070  mov.w	r0, #256
08029712  7047      bx	lr
08029714  4ff40070  mov.w	r0, #512
08029718  7047      bx	lr
0802971a  4ff48060  mov.w	r0, #1024
0802971e  7047      bx	lr
08029720  4ff40060  mov.w	r0, #2048
08029724  7047      bx	lr
08029726  4ff48050  mov.w	r0, #4096
0802972a  7047      bx	lr
0802972c  4ff40050  mov.w	r0, #8192
08029730  7047      bx	lr
08029732  4ff48020  mov.w	r0, #262144
08029736  7047      bx	lr
08029738  4ff40030  mov.w	r0, #131072
0802973c  7047      bx	lr
0802973e  4ff40040  mov.w	r0, #32768
08029742  7047      bx	lr
08029744  4ff48040  mov.w	r0, #16384
08029748  7047      bx	lr
0802974a  4ff40010  mov.w	r0, #2097152
0802974e  7047      bx	lr
08029750  4ff48010  mov.w	r0, #1048576
08029754  7047      bx	lr
08029756  4ff40020  mov.w	r0, #524288
0802975a  7047      bx	lr
