; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028a5c  4f4a      ldr	r2, [pc, #316] ; [0x08028b9c] = 0x20000014
08028a5e  8446      mov	r12, r0
08028a60  4f4b      ldr	r3, [pc, #316] ; [0x08028ba0] = 0x10624dd3
08028a62  1268      ldr	r2, [r2]
08028a64  ccf80810  str.w	r1, [r12, #8]
08028a68  a3fb0232  umull	r3, r2, r3, r2
08028a6c  dcf80c10  ldr.w	r1, [r12, #12]
08028a70  4c48      ldr	r0, [pc, #304] ; [0x08028ba4] = 0xfffee0c0
08028a72  41f21913  movw	r3, #4377
08028a76  520a      lsrs	r2, r2, #9
08028a78  0840      ands	r0, r1
08028a7a  41f28831  movw	r1, #5000
08028a7e  0343      orrs	r3, r0
08028a80  01fb02f2  mul	r2, r1, r2
08028a84  ccf80c30  str.w	r3, [r12, #12]
08028a88  531e      subs	r3, r2, #1
08028a8a  4ab1      cbz	r2, #18 ; -> 0x08028aa0 ; branch_target=0x08028aa0
08028a8c  4649      ldr	r1, [pc, #280] ; [0x08028ba8] = 0x00200045
08028a8e  dcf83420  ldr.w	r2, [r12, #52]
08028a92  013b      subs	r3, #1
08028a94  0a42      tst	r2, r1
08028a96  01d0      beq	#2 ; -> 0x08028a9c ; branch_target=0x08028a9c
08028a98  9204      lsls	r2, r2, #18
08028a9a  04d5      bpl	#8 ; -> 0x08028aa6 ; branch_target=0x08028aa6
08028a9c  581c      adds	r0, r3, #1
08028a9e  f6d1      bne	#-20 ; -> 0x08028a8e ; branch_target=0x08028a8e
08028aa0  4ff00040  mov.w	r0, #2147483648
08028aa4  7047      bx	lr
08028aa6  dcf83430  ldr.w	r3, [r12, #52]
08028aaa  5b07      lsls	r3, r3, #29
08028aac  43d4      bmi	#134 ; -> 0x08028b36 ; branch_target=0x08028b36
08028aae  dcf83430  ldr.w	r3, [r12, #52]
08028ab2  d807      lsls	r0, r3, #31
08028ab4  09d4      bmi	#18 ; -> 0x08028aca ; branch_target=0x08028aca
08028ab6  3d4b      ldr	r3, [pc, #244] ; [0x08028bac] = 0x002000c5
08028ab8  ccf83830  str.w	r3, [r12, #56]
08028abc  dcf81030  ldr.w	r3, [r12, #16]
08028ac0  dbb2      uxtb	r3, r3
08028ac2  192b      cmp	r3, #25
08028ac4  05d0      beq	#10 ; -> 0x08028ad2 ; branch_target=0x08028ad2
08028ac6  0120      movs	r0, #1
08028ac8  7047      bx	lr
08028aca  0123      movs	r3, #1
08028acc  ccf83830  str.w	r3, [r12, #56]
08028ad0  f9e7      b	#-14 ; -> 0x08028ac6 ; branch_target=0x08028ac6
08028ad2  dcf81430  ldr.w	r3, [r12, #20]
08028ad6  3648      ldr	r0, [pc, #216] ; [0x08028bb0] = 0xfdffe008
08028ad8  1840      ands	r0, r3
08028ada  58b3      cbz	r0, #86 ; -> 0x08028b34 ; branch_target=0x08028b34
08028adc  002b      cmp	r3, #0
08028ade  2fdb      blt	#94 ; -> 0x08028b40 ; branch_target=0x08028b40
08028ae0  5900      lsls	r1, r3, #1
08028ae2  30d4      bmi	#96 ; -> 0x08028b46 ; branch_target=0x08028b46
08028ae4  9a00      lsls	r2, r3, #2
08028ae6  30d4      bmi	#96 ; -> 0x08028b4a ; branch_target=0x08028b4a
08028ae8  d900      lsls	r1, r3, #3
08028aea  30d4      bmi	#96 ; -> 0x08028b4e ; branch_target=0x08028b4e
08028aec  1a01      lsls	r2, r3, #4
08028aee  31d4      bmi	#98 ; -> 0x08028b54 ; branch_target=0x08028b54
08028af0  5901      lsls	r1, r3, #5
08028af2  32d4      bmi	#100 ; -> 0x08028b5a ; branch_target=0x08028b5a
08028af4  da01      lsls	r2, r3, #7
08028af6  33d4      bmi	#102 ; -> 0x08028b60 ; branch_target=0x08028b60
08028af8  1902      lsls	r1, r3, #8
08028afa  34d4      bmi	#104 ; -> 0x08028b66 ; branch_target=0x08028b66
08028afc  5a02      lsls	r2, r3, #9
08028afe  35d4      bmi	#106 ; -> 0x08028b6c ; branch_target=0x08028b6c
08028b00  9902      lsls	r1, r3, #10
08028b02  3fd4      bmi	#126 ; -> 0x08028b84 ; branch_target=0x08028b84
08028b04  da02      lsls	r2, r3, #11
08028b06  3ad4      bmi	#116 ; -> 0x08028b7e ; branch_target=0x08028b7e
08028b08  5903      lsls	r1, r3, #13
08028b0a  35d4      bmi	#106 ; -> 0x08028b78 ; branch_target=0x08028b78
08028b0c  9a03      lsls	r2, r3, #14
08028b0e  30d4      bmi	#96 ; -> 0x08028b72 ; branch_target=0x08028b72
08028b10  d903      lsls	r1, r3, #15
08028b12  40d4      bmi	#128 ; -> 0x08028b96 ; branch_target=0x08028b96
08028b14  1a04      lsls	r2, r3, #16
08028b16  3bd4      bmi	#118 ; -> 0x08028b90 ; branch_target=0x08028b90
08028b18  5904      lsls	r1, r3, #17
08028b1a  36d4      bmi	#108 ; -> 0x08028b8a ; branch_target=0x08028b8a
08028b1c  9a04      lsls	r2, r3, #18
08028b1e  07d4      bmi	#14 ; -> 0x08028b30 ; branch_target=0x08028b30
08028b20  13f0080f  tst.w	r3, #8
08028b24  0cbf      ite	eq
08028b26  4ff48030  moveq.w	r0, #65536
08028b2a  4ff40000  movne.w	r0, #8388608
08028b2e  7047      bx	lr
08028b30  4ff48000  mov.w	r0, #4194304
08028b34  7047      bx	lr
08028b36  0423      movs	r3, #4
08028b38  1846      mov	r0, r3
08028b3a  ccf83830  str.w	r3, [r12, #56]
08028b3e  7047      bx	lr
08028b40  4ff00070  mov.w	r0, #33554432
08028b44  7047      bx	lr
08028b46  4020      movs	r0, #64
08028b48  7047      bx	lr
08028b4a  8020      movs	r0, #128
08028b4c  7047      bx	lr
08028b4e  4ff48070  mov.w	r0, #256
08028b52  7047      bx	lr
08028b54  4ff40070  mov.w	r0, #512
08028b58  7047      bx	lr
08028b5a  4ff48060  mov.w	r0, #1024
08028b5e  7047      bx	lr
08028b60  4ff40060  mov.w	r0, #2048
08028b64  7047      bx	lr
08028b66  4ff48050  mov.w	r0, #4096
08028b6a  7047      bx	lr
08028b6c  4ff40050  mov.w	r0, #8192
08028b70  7047      bx	lr
08028b72  4ff48020  mov.w	r0, #262144
08028b76  7047      bx	lr
08028b78  4ff40030  mov.w	r0, #131072
08028b7c  7047      bx	lr
08028b7e  4ff40040  mov.w	r0, #32768
08028b82  7047      bx	lr
08028b84  4ff48040  mov.w	r0, #16384
08028b88  7047      bx	lr
08028b8a  4ff40010  mov.w	r0, #2097152
08028b8e  7047      bx	lr
08028b90  4ff48010  mov.w	r0, #1048576
08028b94  7047      bx	lr
08028b96  4ff40020  mov.w	r0, #524288
08028b9a  7047      bx	lr
