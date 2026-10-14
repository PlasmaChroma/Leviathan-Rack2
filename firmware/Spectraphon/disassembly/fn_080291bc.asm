; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080291bc  4f4a      ldr	r2, [pc, #316] ; [0x080292fc] = 0x20000014
080291be  8446      mov	r12, r0
080291c0  4f4b      ldr	r3, [pc, #316] ; [0x08029300] = 0x10624dd3
080291c2  1268      ldr	r2, [r2]
080291c4  ccf80810  str.w	r1, [r12, #8]
080291c8  a3fb0232  umull	r3, r2, r3, r2
080291cc  dcf80c10  ldr.w	r1, [r12, #12]
080291d0  4c48      ldr	r0, [pc, #304] ; [0x08029304] = 0xfffee0c0
080291d2  41f20613  movw	r3, #4358
080291d6  520a      lsrs	r2, r2, #9
080291d8  0840      ands	r0, r1
080291da  41f28831  movw	r1, #5000
080291de  0343      orrs	r3, r0
080291e0  01fb02f2  mul	r2, r1, r2
080291e4  ccf80c30  str.w	r3, [r12, #12]
080291e8  531e      subs	r3, r2, #1
080291ea  4ab1      cbz	r2, #18 ; -> 0x08029200 ; branch_target=0x08029200
080291ec  4649      ldr	r1, [pc, #280] ; [0x08029308] = 0x00200045
080291ee  dcf83420  ldr.w	r2, [r12, #52]
080291f2  013b      subs	r3, #1
080291f4  0a42      tst	r2, r1
080291f6  01d0      beq	#2 ; -> 0x080291fc ; branch_target=0x080291fc
080291f8  9204      lsls	r2, r2, #18
080291fa  04d5      bpl	#8 ; -> 0x08029206 ; branch_target=0x08029206
080291fc  581c      adds	r0, r3, #1
080291fe  f6d1      bne	#-20 ; -> 0x080291ee ; branch_target=0x080291ee
08029200  4ff00040  mov.w	r0, #2147483648
08029204  7047      bx	lr
08029206  dcf83430  ldr.w	r3, [r12, #52]
0802920a  5b07      lsls	r3, r3, #29
0802920c  43d4      bmi	#134 ; -> 0x08029296 ; branch_target=0x08029296
0802920e  dcf83430  ldr.w	r3, [r12, #52]
08029212  d807      lsls	r0, r3, #31
08029214  09d4      bmi	#18 ; -> 0x0802922a ; branch_target=0x0802922a
08029216  3d4b      ldr	r3, [pc, #244] ; [0x0802930c] = 0x002000c5
08029218  ccf83830  str.w	r3, [r12, #56]
0802921c  dcf81030  ldr.w	r3, [r12, #16]
08029220  dbb2      uxtb	r3, r3
08029222  062b      cmp	r3, #6
08029224  05d0      beq	#10 ; -> 0x08029232 ; branch_target=0x08029232
08029226  0120      movs	r0, #1
08029228  7047      bx	lr
0802922a  0123      movs	r3, #1
0802922c  ccf83830  str.w	r3, [r12, #56]
08029230  f9e7      b	#-14 ; -> 0x08029226 ; branch_target=0x08029226
08029232  dcf81430  ldr.w	r3, [r12, #20]
08029236  3648      ldr	r0, [pc, #216] ; [0x08029310] = 0xfdffe008
08029238  1840      ands	r0, r3
0802923a  58b3      cbz	r0, #86 ; -> 0x08029294 ; branch_target=0x08029294
0802923c  002b      cmp	r3, #0
0802923e  2fdb      blt	#94 ; -> 0x080292a0 ; branch_target=0x080292a0
08029240  5900      lsls	r1, r3, #1
08029242  30d4      bmi	#96 ; -> 0x080292a6 ; branch_target=0x080292a6
08029244  9a00      lsls	r2, r3, #2
08029246  30d4      bmi	#96 ; -> 0x080292aa ; branch_target=0x080292aa
08029248  d900      lsls	r1, r3, #3
0802924a  30d4      bmi	#96 ; -> 0x080292ae ; branch_target=0x080292ae
0802924c  1a01      lsls	r2, r3, #4
0802924e  31d4      bmi	#98 ; -> 0x080292b4 ; branch_target=0x080292b4
08029250  5901      lsls	r1, r3, #5
08029252  32d4      bmi	#100 ; -> 0x080292ba ; branch_target=0x080292ba
08029254  da01      lsls	r2, r3, #7
08029256  33d4      bmi	#102 ; -> 0x080292c0 ; branch_target=0x080292c0
08029258  1902      lsls	r1, r3, #8
0802925a  34d4      bmi	#104 ; -> 0x080292c6 ; branch_target=0x080292c6
0802925c  5a02      lsls	r2, r3, #9
0802925e  35d4      bmi	#106 ; -> 0x080292cc ; branch_target=0x080292cc
08029260  9902      lsls	r1, r3, #10
08029262  3fd4      bmi	#126 ; -> 0x080292e4 ; branch_target=0x080292e4
08029264  da02      lsls	r2, r3, #11
08029266  3ad4      bmi	#116 ; -> 0x080292de ; branch_target=0x080292de
08029268  5903      lsls	r1, r3, #13
0802926a  35d4      bmi	#106 ; -> 0x080292d8 ; branch_target=0x080292d8
0802926c  9a03      lsls	r2, r3, #14
0802926e  30d4      bmi	#96 ; -> 0x080292d2 ; branch_target=0x080292d2
08029270  d903      lsls	r1, r3, #15
08029272  40d4      bmi	#128 ; -> 0x080292f6 ; branch_target=0x080292f6
08029274  1a04      lsls	r2, r3, #16
08029276  3bd4      bmi	#118 ; -> 0x080292f0 ; branch_target=0x080292f0
08029278  5904      lsls	r1, r3, #17
0802927a  36d4      bmi	#108 ; -> 0x080292ea ; branch_target=0x080292ea
0802927c  9a04      lsls	r2, r3, #18
0802927e  07d4      bmi	#14 ; -> 0x08029290 ; branch_target=0x08029290
08029280  13f0080f  tst.w	r3, #8
08029284  0cbf      ite	eq
08029286  4ff48030  moveq.w	r0, #65536
0802928a  4ff40000  movne.w	r0, #8388608
0802928e  7047      bx	lr
08029290  4ff48000  mov.w	r0, #4194304
08029294  7047      bx	lr
08029296  0423      movs	r3, #4
08029298  1846      mov	r0, r3
0802929a  ccf83830  str.w	r3, [r12, #56]
0802929e  7047      bx	lr
080292a0  4ff00070  mov.w	r0, #33554432
080292a4  7047      bx	lr
080292a6  4020      movs	r0, #64
080292a8  7047      bx	lr
080292aa  8020      movs	r0, #128
080292ac  7047      bx	lr
080292ae  4ff48070  mov.w	r0, #256
080292b2  7047      bx	lr
080292b4  4ff40070  mov.w	r0, #512
080292b8  7047      bx	lr
080292ba  4ff48060  mov.w	r0, #1024
080292be  7047      bx	lr
080292c0  4ff40060  mov.w	r0, #2048
080292c4  7047      bx	lr
080292c6  4ff48050  mov.w	r0, #4096
080292ca  7047      bx	lr
080292cc  4ff40050  mov.w	r0, #8192
080292d0  7047      bx	lr
080292d2  4ff48020  mov.w	r0, #262144
080292d6  7047      bx	lr
080292d8  4ff40030  mov.w	r0, #131072
080292dc  7047      bx	lr
080292de  4ff40040  mov.w	r0, #32768
080292e2  7047      bx	lr
080292e4  4ff48040  mov.w	r0, #16384
080292e8  7047      bx	lr
080292ea  4ff40010  mov.w	r0, #2097152
080292ee  7047      bx	lr
080292f0  4ff48010  mov.w	r0, #1048576
080292f4  7047      bx	lr
080292f6  4ff40020  mov.w	r0, #524288
080292fa  7047      bx	lr
