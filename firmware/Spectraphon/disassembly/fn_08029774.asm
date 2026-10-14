; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029774  10b4      push	{r4}
08029776  0146      mov	r1, r0
08029778  0024      movs	r4, #0
0802977a  4b4a      ldr	r2, [pc, #300] ; [0x080298a8] = 0x20000014
0802977c  4b4b      ldr	r3, [pc, #300] ; [0x080298ac] = 0x10624dd3
0802977e  8c60      str	r4, [r1, #8]
08029780  1268      ldr	r2, [r2]
08029782  cc68      ldr	r4, [r1, #12]
08029784  4a48      ldr	r0, [pc, #296] ; [0x080298b0] = 0xfffee0c0
08029786  a3fb0232  umull	r3, r2, r3, r2
0802978a  41f20d13  movw	r3, #4365
0802978e  2040      ands	r0, r4
08029790  520a      lsrs	r2, r2, #9
08029792  0343      orrs	r3, r0
08029794  41f28830  movw	r0, #5000
08029798  00fb02f2  mul	r2, r0, r2
0802979c  cb60      str	r3, [r1, #12]
0802979e  4ab1      cbz	r2, #18 ; -> 0x080297b4 ; branch_target=0x080297b4
080297a0  531e      subs	r3, r2, #1
080297a2  4448      ldr	r0, [pc, #272] ; [0x080298b4] = 0x00200045
080297a4  4a6b      ldr	r2, [r1, #52]
080297a6  013b      subs	r3, #1
080297a8  0242      tst	r2, r0
080297aa  01d0      beq	#2 ; -> 0x080297b0 ; branch_target=0x080297b0
080297ac  9204      lsls	r2, r2, #18
080297ae  06d5      bpl	#12 ; -> 0x080297be ; branch_target=0x080297be
080297b0  5c1c      adds	r4, r3, #1
080297b2  f7d1      bne	#-18 ; -> 0x080297a4 ; branch_target=0x080297a4
080297b4  4ff00040  mov.w	r0, #2147483648
080297b8  5df8044b  ldr	r4, [sp], #4
080297bc  7047      bx	lr
080297be  4b6b      ldr	r3, [r1, #52]
080297c0  5c07      lsls	r4, r3, #29
080297c2  3cd4      bmi	#120 ; -> 0x0802983e ; branch_target=0x0802983e
080297c4  4b6b      ldr	r3, [r1, #52]
080297c6  d807      lsls	r0, r3, #31
080297c8  07d4      bmi	#14 ; -> 0x080297da ; branch_target=0x080297da
080297ca  3b4b      ldr	r3, [pc, #236] ; [0x080298b8] = 0x002000c5
080297cc  8b63      str	r3, [r1, #56]
080297ce  0b69      ldr	r3, [r1, #16]
080297d0  dbb2      uxtb	r3, r3
080297d2  0d2b      cmp	r3, #13
080297d4  04d0      beq	#8 ; -> 0x080297e0 ; branch_target=0x080297e0
080297d6  0120      movs	r0, #1
080297d8  eee7      b	#-36 ; -> 0x080297b8 ; branch_target=0x080297b8
080297da  0123      movs	r3, #1
080297dc  8b63      str	r3, [r1, #56]
080297de  fae7      b	#-12 ; -> 0x080297d6 ; branch_target=0x080297d6
080297e0  4b69      ldr	r3, [r1, #20]
080297e2  3648      ldr	r0, [pc, #216] ; [0x080298bc] = 0xfdffe008
080297e4  1840      ands	r0, r3
080297e6  0028      cmp	r0, #0
080297e8  e6d0      beq	#-52 ; -> 0x080297b8 ; branch_target=0x080297b8
080297ea  002b      cmp	r3, #0
080297ec  2bdb      blt	#86 ; -> 0x08029846 ; branch_target=0x08029846
080297ee  5a00      lsls	r2, r3, #1
080297f0  2cd4      bmi	#88 ; -> 0x0802984c ; branch_target=0x0802984c
080297f2  9c00      lsls	r4, r3, #2
080297f4  2cd4      bmi	#88 ; -> 0x08029850 ; branch_target=0x08029850
080297f6  d900      lsls	r1, r3, #3
080297f8  2cd4      bmi	#88 ; -> 0x08029854 ; branch_target=0x08029854
080297fa  1a01      lsls	r2, r3, #4
080297fc  2dd4      bmi	#90 ; -> 0x0802985a ; branch_target=0x0802985a
080297fe  5c01      lsls	r4, r3, #5
08029800  2ed4      bmi	#92 ; -> 0x08029860 ; branch_target=0x08029860
08029802  d901      lsls	r1, r3, #7
08029804  2fd4      bmi	#94 ; -> 0x08029866 ; branch_target=0x08029866
08029806  1a02      lsls	r2, r3, #8
08029808  30d4      bmi	#96 ; -> 0x0802986c ; branch_target=0x0802986c
0802980a  5c02      lsls	r4, r3, #9
0802980c  31d4      bmi	#98 ; -> 0x08029872 ; branch_target=0x08029872
0802980e  9902      lsls	r1, r3, #10
08029810  3bd4      bmi	#118 ; -> 0x0802988a ; branch_target=0x0802988a
08029812  da02      lsls	r2, r3, #11
08029814  36d4      bmi	#108 ; -> 0x08029884 ; branch_target=0x08029884
08029816  5c03      lsls	r4, r3, #13
08029818  31d4      bmi	#98 ; -> 0x0802987e ; branch_target=0x0802987e
0802981a  9903      lsls	r1, r3, #14
0802981c  2cd4      bmi	#88 ; -> 0x08029878 ; branch_target=0x08029878
0802981e  da03      lsls	r2, r3, #15
08029820  3fd4      bmi	#126 ; -> 0x080298a2 ; branch_target=0x080298a2
08029822  1c04      lsls	r4, r3, #16
08029824  3ad4      bmi	#116 ; -> 0x0802989c ; branch_target=0x0802989c
08029826  5904      lsls	r1, r3, #17
08029828  35d4      bmi	#106 ; -> 0x08029896 ; branch_target=0x08029896
0802982a  9a04      lsls	r2, r3, #18
0802982c  30d4      bmi	#96 ; -> 0x08029890 ; branch_target=0x08029890
0802982e  13f0080f  tst.w	r3, #8
08029832  0cbf      ite	eq
08029834  4ff48030  moveq.w	r0, #65536
08029838  4ff40000  movne.w	r0, #8388608
0802983c  bce7      b	#-136 ; -> 0x080297b8 ; branch_target=0x080297b8
0802983e  0423      movs	r3, #4
08029840  1846      mov	r0, r3
08029842  8b63      str	r3, [r1, #56]
08029844  b8e7      b	#-144 ; -> 0x080297b8 ; branch_target=0x080297b8
08029846  4ff00070  mov.w	r0, #33554432
0802984a  b5e7      b	#-150 ; -> 0x080297b8 ; branch_target=0x080297b8
0802984c  4020      movs	r0, #64
0802984e  b3e7      b	#-154 ; -> 0x080297b8 ; branch_target=0x080297b8
08029850  8020      movs	r0, #128
08029852  b1e7      b	#-158 ; -> 0x080297b8 ; branch_target=0x080297b8
08029854  4ff48070  mov.w	r0, #256
08029858  aee7      b	#-164 ; -> 0x080297b8 ; branch_target=0x080297b8
0802985a  4ff40070  mov.w	r0, #512
0802985e  abe7      b	#-170 ; -> 0x080297b8 ; branch_target=0x080297b8
08029860  4ff48060  mov.w	r0, #1024
08029864  a8e7      b	#-176 ; -> 0x080297b8 ; branch_target=0x080297b8
08029866  4ff40060  mov.w	r0, #2048
0802986a  a5e7      b	#-182 ; -> 0x080297b8 ; branch_target=0x080297b8
0802986c  4ff48050  mov.w	r0, #4096
08029870  a2e7      b	#-188 ; -> 0x080297b8 ; branch_target=0x080297b8
08029872  4ff40050  mov.w	r0, #8192
08029876  9fe7      b	#-194 ; -> 0x080297b8 ; branch_target=0x080297b8
08029878  4ff48020  mov.w	r0, #262144
0802987c  9ce7      b	#-200 ; -> 0x080297b8 ; branch_target=0x080297b8
0802987e  4ff40030  mov.w	r0, #131072
08029882  99e7      b	#-206 ; -> 0x080297b8 ; branch_target=0x080297b8
08029884  4ff40040  mov.w	r0, #32768
08029888  96e7      b	#-212 ; -> 0x080297b8 ; branch_target=0x080297b8
0802988a  4ff48040  mov.w	r0, #16384
0802988e  93e7      b	#-218 ; -> 0x080297b8 ; branch_target=0x080297b8
08029890  4ff48000  mov.w	r0, #4194304
08029894  90e7      b	#-224 ; -> 0x080297b8 ; branch_target=0x080297b8
08029896  4ff40010  mov.w	r0, #2097152
0802989a  8de7      b	#-230 ; -> 0x080297b8 ; branch_target=0x080297b8
0802989c  4ff48010  mov.w	r0, #1048576
080298a0  8ae7      b	#-236 ; -> 0x080297b8 ; branch_target=0x080297b8
080298a2  4ff40020  mov.w	r0, #524288
080298a6  87e7      b	#-242 ; -> 0x080297b8 ; branch_target=0x080297b8
