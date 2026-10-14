; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080284fc  4f4a      ldr	r2, [pc, #316] ; [0x0802863c] = 0x20000014
080284fe  8446      mov	r12, r0
08028500  4f4b      ldr	r3, [pc, #316] ; [0x08028640] = 0x10624dd3
08028502  1268      ldr	r2, [r2]
08028504  ccf80810  str.w	r1, [r12, #8]
08028508  a3fb0232  umull	r3, r2, r3, r2
0802850c  dcf80c10  ldr.w	r1, [r12, #12]
08028510  4c48      ldr	r0, [pc, #304] ; [0x08028644] = 0xfffee0c0
08028512  41f21013  movw	r3, #4368
08028516  520a      lsrs	r2, r2, #9
08028518  0840      ands	r0, r1
0802851a  41f28831  movw	r1, #5000
0802851e  0343      orrs	r3, r0
08028520  01fb02f2  mul	r2, r1, r2
08028524  ccf80c30  str.w	r3, [r12, #12]
08028528  531e      subs	r3, r2, #1
0802852a  4ab1      cbz	r2, #18 ; -> 0x08028540 ; branch_target=0x08028540
0802852c  4649      ldr	r1, [pc, #280] ; [0x08028648] = 0x00200045
0802852e  dcf83420  ldr.w	r2, [r12, #52]
08028532  013b      subs	r3, #1
08028534  0a42      tst	r2, r1
08028536  01d0      beq	#2 ; -> 0x0802853c ; branch_target=0x0802853c
08028538  9204      lsls	r2, r2, #18
0802853a  04d5      bpl	#8 ; -> 0x08028546 ; branch_target=0x08028546
0802853c  581c      adds	r0, r3, #1
0802853e  f6d1      bne	#-20 ; -> 0x0802852e ; branch_target=0x0802852e
08028540  4ff00040  mov.w	r0, #2147483648
08028544  7047      bx	lr
08028546  dcf83430  ldr.w	r3, [r12, #52]
0802854a  5b07      lsls	r3, r3, #29
0802854c  43d4      bmi	#134 ; -> 0x080285d6 ; branch_target=0x080285d6
0802854e  dcf83430  ldr.w	r3, [r12, #52]
08028552  d807      lsls	r0, r3, #31
08028554  09d4      bmi	#18 ; -> 0x0802856a ; branch_target=0x0802856a
08028556  3d4b      ldr	r3, [pc, #244] ; [0x0802864c] = 0x002000c5
08028558  ccf83830  str.w	r3, [r12, #56]
0802855c  dcf81030  ldr.w	r3, [r12, #16]
08028560  dbb2      uxtb	r3, r3
08028562  102b      cmp	r3, #16
08028564  05d0      beq	#10 ; -> 0x08028572 ; branch_target=0x08028572
08028566  0120      movs	r0, #1
08028568  7047      bx	lr
0802856a  0123      movs	r3, #1
0802856c  ccf83830  str.w	r3, [r12, #56]
08028570  f9e7      b	#-14 ; -> 0x08028566 ; branch_target=0x08028566
08028572  dcf81430  ldr.w	r3, [r12, #20]
08028576  3648      ldr	r0, [pc, #216] ; [0x08028650] = 0xfdffe008
08028578  1840      ands	r0, r3
0802857a  58b3      cbz	r0, #86 ; -> 0x080285d4 ; branch_target=0x080285d4
0802857c  002b      cmp	r3, #0
0802857e  2fdb      blt	#94 ; -> 0x080285e0 ; branch_target=0x080285e0
08028580  5900      lsls	r1, r3, #1
08028582  30d4      bmi	#96 ; -> 0x080285e6 ; branch_target=0x080285e6
08028584  9a00      lsls	r2, r3, #2
08028586  30d4      bmi	#96 ; -> 0x080285ea ; branch_target=0x080285ea
08028588  d900      lsls	r1, r3, #3
0802858a  30d4      bmi	#96 ; -> 0x080285ee ; branch_target=0x080285ee
0802858c  1a01      lsls	r2, r3, #4
0802858e  31d4      bmi	#98 ; -> 0x080285f4 ; branch_target=0x080285f4
08028590  5901      lsls	r1, r3, #5
08028592  32d4      bmi	#100 ; -> 0x080285fa ; branch_target=0x080285fa
08028594  da01      lsls	r2, r3, #7
08028596  33d4      bmi	#102 ; -> 0x08028600 ; branch_target=0x08028600
08028598  1902      lsls	r1, r3, #8
0802859a  34d4      bmi	#104 ; -> 0x08028606 ; branch_target=0x08028606
0802859c  5a02      lsls	r2, r3, #9
0802859e  35d4      bmi	#106 ; -> 0x0802860c ; branch_target=0x0802860c
080285a0  9902      lsls	r1, r3, #10
080285a2  3fd4      bmi	#126 ; -> 0x08028624 ; branch_target=0x08028624
080285a4  da02      lsls	r2, r3, #11
080285a6  3ad4      bmi	#116 ; -> 0x0802861e ; branch_target=0x0802861e
080285a8  5903      lsls	r1, r3, #13
080285aa  35d4      bmi	#106 ; -> 0x08028618 ; branch_target=0x08028618
080285ac  9a03      lsls	r2, r3, #14
080285ae  30d4      bmi	#96 ; -> 0x08028612 ; branch_target=0x08028612
080285b0  d903      lsls	r1, r3, #15
080285b2  40d4      bmi	#128 ; -> 0x08028636 ; branch_target=0x08028636
080285b4  1a04      lsls	r2, r3, #16
080285b6  3bd4      bmi	#118 ; -> 0x08028630 ; branch_target=0x08028630
080285b8  5904      lsls	r1, r3, #17
080285ba  36d4      bmi	#108 ; -> 0x0802862a ; branch_target=0x0802862a
080285bc  9a04      lsls	r2, r3, #18
080285be  07d4      bmi	#14 ; -> 0x080285d0 ; branch_target=0x080285d0
080285c0  13f0080f  tst.w	r3, #8
080285c4  0cbf      ite	eq
080285c6  4ff48030  moveq.w	r0, #65536
080285ca  4ff40000  movne.w	r0, #8388608
080285ce  7047      bx	lr
080285d0  4ff48000  mov.w	r0, #4194304
080285d4  7047      bx	lr
080285d6  0423      movs	r3, #4
080285d8  1846      mov	r0, r3
080285da  ccf83830  str.w	r3, [r12, #56]
080285de  7047      bx	lr
080285e0  4ff00070  mov.w	r0, #33554432
080285e4  7047      bx	lr
080285e6  4020      movs	r0, #64
080285e8  7047      bx	lr
080285ea  8020      movs	r0, #128
080285ec  7047      bx	lr
080285ee  4ff48070  mov.w	r0, #256
080285f2  7047      bx	lr
080285f4  4ff40070  mov.w	r0, #512
080285f8  7047      bx	lr
080285fa  4ff48060  mov.w	r0, #1024
080285fe  7047      bx	lr
08028600  4ff40060  mov.w	r0, #2048
08028604  7047      bx	lr
08028606  4ff48050  mov.w	r0, #4096
0802860a  7047      bx	lr
0802860c  4ff40050  mov.w	r0, #8192
08028610  7047      bx	lr
08028612  4ff48020  mov.w	r0, #262144
08028616  7047      bx	lr
08028618  4ff40030  mov.w	r0, #131072
0802861c  7047      bx	lr
0802861e  4ff40040  mov.w	r0, #32768
08028622  7047      bx	lr
08028624  4ff48040  mov.w	r0, #16384
08028628  7047      bx	lr
0802862a  4ff40010  mov.w	r0, #2097152
0802862e  7047      bx	lr
08028630  4ff48010  mov.w	r0, #1048576
08028634  7047      bx	lr
08028636  4ff40020  mov.w	r0, #524288
0802863a  7047      bx	lr
