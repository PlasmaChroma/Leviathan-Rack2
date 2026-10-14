; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080287ac  4f4a      ldr	r2, [pc, #316] ; [0x080288ec] = 0x20000014
080287ae  8446      mov	r12, r0
080287b0  4f4b      ldr	r3, [pc, #316] ; [0x080288f0] = 0x10624dd3
080287b2  1268      ldr	r2, [r2]
080287b4  ccf80810  str.w	r1, [r12, #8]
080287b8  a3fb0232  umull	r3, r2, r3, r2
080287bc  dcf80c10  ldr.w	r1, [r12, #12]
080287c0  4c48      ldr	r0, [pc, #304] ; [0x080288f4] = 0xfffee0c0
080287c2  41f21213  movw	r3, #4370
080287c6  520a      lsrs	r2, r2, #9
080287c8  0840      ands	r0, r1
080287ca  41f28831  movw	r1, #5000
080287ce  0343      orrs	r3, r0
080287d0  01fb02f2  mul	r2, r1, r2
080287d4  ccf80c30  str.w	r3, [r12, #12]
080287d8  531e      subs	r3, r2, #1
080287da  4ab1      cbz	r2, #18 ; -> 0x080287f0 ; branch_target=0x080287f0
080287dc  4649      ldr	r1, [pc, #280] ; [0x080288f8] = 0x00200045
080287de  dcf83420  ldr.w	r2, [r12, #52]
080287e2  013b      subs	r3, #1
080287e4  0a42      tst	r2, r1
080287e6  01d0      beq	#2 ; -> 0x080287ec ; branch_target=0x080287ec
080287e8  9204      lsls	r2, r2, #18
080287ea  04d5      bpl	#8 ; -> 0x080287f6 ; branch_target=0x080287f6
080287ec  581c      adds	r0, r3, #1
080287ee  f6d1      bne	#-20 ; -> 0x080287de ; branch_target=0x080287de
080287f0  4ff00040  mov.w	r0, #2147483648
080287f4  7047      bx	lr
080287f6  dcf83430  ldr.w	r3, [r12, #52]
080287fa  5b07      lsls	r3, r3, #29
080287fc  43d4      bmi	#134 ; -> 0x08028886 ; branch_target=0x08028886
080287fe  dcf83430  ldr.w	r3, [r12, #52]
08028802  d807      lsls	r0, r3, #31
08028804  09d4      bmi	#18 ; -> 0x0802881a ; branch_target=0x0802881a
08028806  3d4b      ldr	r3, [pc, #244] ; [0x080288fc] = 0x002000c5
08028808  ccf83830  str.w	r3, [r12, #56]
0802880c  dcf81030  ldr.w	r3, [r12, #16]
08028810  dbb2      uxtb	r3, r3
08028812  122b      cmp	r3, #18
08028814  05d0      beq	#10 ; -> 0x08028822 ; branch_target=0x08028822
08028816  0120      movs	r0, #1
08028818  7047      bx	lr
0802881a  0123      movs	r3, #1
0802881c  ccf83830  str.w	r3, [r12, #56]
08028820  f9e7      b	#-14 ; -> 0x08028816 ; branch_target=0x08028816
08028822  dcf81430  ldr.w	r3, [r12, #20]
08028826  3648      ldr	r0, [pc, #216] ; [0x08028900] = 0xfdffe008
08028828  1840      ands	r0, r3
0802882a  58b3      cbz	r0, #86 ; -> 0x08028884 ; branch_target=0x08028884
0802882c  002b      cmp	r3, #0
0802882e  2fdb      blt	#94 ; -> 0x08028890 ; branch_target=0x08028890
08028830  5900      lsls	r1, r3, #1
08028832  30d4      bmi	#96 ; -> 0x08028896 ; branch_target=0x08028896
08028834  9a00      lsls	r2, r3, #2
08028836  30d4      bmi	#96 ; -> 0x0802889a ; branch_target=0x0802889a
08028838  d900      lsls	r1, r3, #3
0802883a  30d4      bmi	#96 ; -> 0x0802889e ; branch_target=0x0802889e
0802883c  1a01      lsls	r2, r3, #4
0802883e  31d4      bmi	#98 ; -> 0x080288a4 ; branch_target=0x080288a4
08028840  5901      lsls	r1, r3, #5
08028842  32d4      bmi	#100 ; -> 0x080288aa ; branch_target=0x080288aa
08028844  da01      lsls	r2, r3, #7
08028846  33d4      bmi	#102 ; -> 0x080288b0 ; branch_target=0x080288b0
08028848  1902      lsls	r1, r3, #8
0802884a  34d4      bmi	#104 ; -> 0x080288b6 ; branch_target=0x080288b6
0802884c  5a02      lsls	r2, r3, #9
0802884e  35d4      bmi	#106 ; -> 0x080288bc ; branch_target=0x080288bc
08028850  9902      lsls	r1, r3, #10
08028852  3fd4      bmi	#126 ; -> 0x080288d4 ; branch_target=0x080288d4
08028854  da02      lsls	r2, r3, #11
08028856  3ad4      bmi	#116 ; -> 0x080288ce ; branch_target=0x080288ce
08028858  5903      lsls	r1, r3, #13
0802885a  35d4      bmi	#106 ; -> 0x080288c8 ; branch_target=0x080288c8
0802885c  9a03      lsls	r2, r3, #14
0802885e  30d4      bmi	#96 ; -> 0x080288c2 ; branch_target=0x080288c2
08028860  d903      lsls	r1, r3, #15
08028862  40d4      bmi	#128 ; -> 0x080288e6 ; branch_target=0x080288e6
08028864  1a04      lsls	r2, r3, #16
08028866  3bd4      bmi	#118 ; -> 0x080288e0 ; branch_target=0x080288e0
08028868  5904      lsls	r1, r3, #17
0802886a  36d4      bmi	#108 ; -> 0x080288da ; branch_target=0x080288da
0802886c  9a04      lsls	r2, r3, #18
0802886e  07d4      bmi	#14 ; -> 0x08028880 ; branch_target=0x08028880
08028870  13f0080f  tst.w	r3, #8
08028874  0cbf      ite	eq
08028876  4ff48030  moveq.w	r0, #65536
0802887a  4ff40000  movne.w	r0, #8388608
0802887e  7047      bx	lr
08028880  4ff48000  mov.w	r0, #4194304
08028884  7047      bx	lr
08028886  0423      movs	r3, #4
08028888  1846      mov	r0, r3
0802888a  ccf83830  str.w	r3, [r12, #56]
0802888e  7047      bx	lr
08028890  4ff00070  mov.w	r0, #33554432
08028894  7047      bx	lr
08028896  4020      movs	r0, #64
08028898  7047      bx	lr
0802889a  8020      movs	r0, #128
0802889c  7047      bx	lr
0802889e  4ff48070  mov.w	r0, #256
080288a2  7047      bx	lr
080288a4  4ff40070  mov.w	r0, #512
080288a8  7047      bx	lr
080288aa  4ff48060  mov.w	r0, #1024
080288ae  7047      bx	lr
080288b0  4ff40060  mov.w	r0, #2048
080288b4  7047      bx	lr
080288b6  4ff48050  mov.w	r0, #4096
080288ba  7047      bx	lr
080288bc  4ff40050  mov.w	r0, #8192
080288c0  7047      bx	lr
080288c2  4ff48020  mov.w	r0, #262144
080288c6  7047      bx	lr
080288c8  4ff40030  mov.w	r0, #131072
080288cc  7047      bx	lr
080288ce  4ff40040  mov.w	r0, #32768
080288d2  7047      bx	lr
080288d4  4ff48040  mov.w	r0, #16384
080288d8  7047      bx	lr
080288da  4ff40010  mov.w	r0, #2097152
080288de  7047      bx	lr
080288e0  4ff48010  mov.w	r0, #1048576
080288e4  7047      bx	lr
080288e6  4ff40020  mov.w	r0, #524288
080288ea  7047      bx	lr
