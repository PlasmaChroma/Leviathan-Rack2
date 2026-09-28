; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08028bb4  30b4      push	{r4, r5}
08028bb6  0146      mov	r1, r0
08028bb8  794b      ldr	r3, [pc, #484] ; [0x08028da0] = 0x20000014
08028bba  c068      ldr	r0, [r0, #12]
08028bbc  0025      movs	r5, #0
08028bbe  794c      ldr	r4, [pc, #484] ; [0x08028da4] = 0x10624dd3
08028bc0  1a68      ldr	r2, [r3]
08028bc2  40f08000  orr	r0, r0, #128
08028bc6  784b      ldr	r3, [pc, #480] ; [0x08028da8] = 0xfffee0c0
08028bc8  c860      str	r0, [r1, #12]
08028bca  a4fb0242  umull	r4, r2, r4, r2
08028bce  c868      ldr	r0, [r1, #12]
08028bd0  764c      ldr	r4, [pc, #472] ; [0x08028dac] = 0x05f5e100
08028bd2  20f04000  bic	r0, r0, #64
08028bd6  520a      lsrs	r2, r2, #9
08028bd8  c860      str	r0, [r1, #12]
08028bda  41f20c10  movw	r0, #4364
08028bde  8d60      str	r5, [r1, #8]
08028be0  04fb02f2  mul	r2, r4, r2
08028be4  cc68      ldr	r4, [r1, #12]
08028be6  2340      ands	r3, r4
08028be8  1843      orrs	r0, r3
08028bea  c860      str	r0, [r1, #12]
08028bec  4ab1      cbz	r2, #18 ; -> 0x08028c02 ; branch_target=0x08028c02
08028bee  531e      subs	r3, r2, #1
08028bf0  6f48      ldr	r0, [pc, #444] ; [0x08028db0] = 0x00200045
08028bf2  4a6b      ldr	r2, [r1, #52]
08028bf4  013b      subs	r3, #1
08028bf6  0242      tst	r2, r0
08028bf8  01d0      beq	#2 ; -> 0x08028bfe ; branch_target=0x08028bfe
08028bfa  9204      lsls	r2, r2, #18
08028bfc  09d5      bpl	#18 ; -> 0x08028c12 ; branch_target=0x08028c12
08028bfe  5c1c      adds	r4, r3, #1
08028c00  f7d1      bne	#-18 ; -> 0x08028bf2 ; branch_target=0x08028bf2
08028c02  cb68      ldr	r3, [r1, #12]
08028c04  4ff00040  mov.w	r0, #2147483648
08028c08  23f08003  bic	r3, r3, #128
08028c0c  cb60      str	r3, [r1, #12]
08028c0e  30bc      pop	{r4, r5}
08028c10  7047      bx	lr
08028c12  4b6b      ldr	r3, [r1, #52]
08028c14  5b07      lsls	r3, r3, #29
08028c16  4dd4      bmi	#154 ; -> 0x08028cb4 ; branch_target=0x08028cb4
08028c18  486b      ldr	r0, [r1, #52]
08028c1a  10f00100  ands	r0, r0, #1
08028c1e  3ad1      bne	#116 ; -> 0x08028c96 ; branch_target=0x08028c96
08028c20  644b      ldr	r3, [pc, #400] ; [0x08028db4] = 0x002000c5
08028c22  8b63      str	r3, [r1, #56]
08028c24  0b69      ldr	r3, [r1, #16]
08028c26  dbb2      uxtb	r3, r3
08028c28  0c2b      cmp	r3, #12
08028c2a  36d1      bne	#108 ; -> 0x08028c9a ; branch_target=0x08028c9a
08028c2c  4b69      ldr	r3, [r1, #20]
08028c2e  624a      ldr	r2, [pc, #392] ; [0x08028db8] = 0xfdffe008
08028c30  1a40      ands	r2, r3
08028c32  002a      cmp	r2, #0
08028c34  38d0      beq	#112 ; -> 0x08028ca8 ; branch_target=0x08028ca8
08028c36  002b      cmp	r3, #0
08028c38  36db      blt	#108 ; -> 0x08028ca8 ; branch_target=0x08028ca8
08028c3a  5d00      lsls	r5, r3, #1
08028c3c  41d4      bmi	#130 ; -> 0x08028cc2 ; branch_target=0x08028cc2
08028c3e  9c00      lsls	r4, r3, #2
08028c40  45d4      bmi	#138 ; -> 0x08028cce ; branch_target=0x08028cce
08028c42  d800      lsls	r0, r3, #3
08028c44  49d4      bmi	#146 ; -> 0x08028cda ; branch_target=0x08028cda
08028c46  1a01      lsls	r2, r3, #4
08028c48  4ed4      bmi	#156 ; -> 0x08028ce8 ; branch_target=0x08028ce8
08028c4a  5d01      lsls	r5, r3, #5
08028c4c  53d4      bmi	#166 ; -> 0x08028cf6 ; branch_target=0x08028cf6
08028c4e  dc01      lsls	r4, r3, #7
08028c50  5fd4      bmi	#190 ; -> 0x08028d12 ; branch_target=0x08028d12
08028c52  1802      lsls	r0, r3, #8
08028c54  56d4      bmi	#172 ; -> 0x08028d04 ; branch_target=0x08028d04
08028c56  5a02      lsls	r2, r3, #9
08028c58  69d4      bmi	#210 ; -> 0x08028d2e ; branch_target=0x08028d2e
08028c5a  9d02      lsls	r5, r3, #10
08028c5c  60d4      bmi	#192 ; -> 0x08028d20 ; branch_target=0x08028d20
08028c5e  dc02      lsls	r4, r3, #11
08028c60  7ad4      bmi	#244 ; -> 0x08028d58 ; branch_target=0x08028d58
08028c62  5803      lsls	r0, r3, #13
08028c64  71d4      bmi	#226 ; -> 0x08028d4a ; branch_target=0x08028d4a
08028c66  9a03      lsls	r2, r3, #14
08028c68  68d4      bmi	#208 ; -> 0x08028d3c ; branch_target=0x08028d3c
08028c6a  dd03      lsls	r5, r3, #15
08028c6c  00f19080  bmi.w	#288 ; -> 0x08028d90 ; branch_target=0x08028d90
08028c70  1c04      lsls	r4, r3, #16
08028c72  7fd4      bmi	#254 ; -> 0x08028d74 ; branch_target=0x08028d74
08028c74  5804      lsls	r0, r3, #17
08028c76  76d4      bmi	#236 ; -> 0x08028d66 ; branch_target=0x08028d66
08028c78  9a04      lsls	r2, r3, #18
08028c7a  00f18280  bmi.w	#260 ; -> 0x08028d82 ; branch_target=0x08028d82
08028c7e  13f0080f  tst.w	r3, #8
08028c82  cb68      ldr	r3, [r1, #12]
08028c84  23f08003  bic	r3, r3, #128
08028c88  14bf      ite	ne
08028c8a  4ff40000  movne.w	r0, #8388608
08028c8e  4ff48030  moveq.w	r0, #65536
08028c92  cb60      str	r3, [r1, #12]
08028c94  bbe7      b	#-138 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028c96  0123      movs	r3, #1
08028c98  8b63      str	r3, [r1, #56]
08028c9a  cb68      ldr	r3, [r1, #12]
08028c9c  0120      movs	r0, #1
08028c9e  23f08003  bic	r3, r3, #128
08028ca2  cb60      str	r3, [r1, #12]
08028ca4  30bc      pop	{r4, r5}
08028ca6  7047      bx	lr
08028ca8  cb68      ldr	r3, [r1, #12]
08028caa  23f08003  bic	r3, r3, #128
08028cae  cb60      str	r3, [r1, #12]
08028cb0  30bc      pop	{r4, r5}
08028cb2  7047      bx	lr
08028cb4  0420      movs	r0, #4
08028cb6  8863      str	r0, [r1, #56]
08028cb8  cb68      ldr	r3, [r1, #12]
08028cba  23f08003  bic	r3, r3, #128
08028cbe  cb60      str	r3, [r1, #12]
08028cc0  a5e7      b	#-182 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028cc2  cb68      ldr	r3, [r1, #12]
08028cc4  4020      movs	r0, #64
08028cc6  23f08003  bic	r3, r3, #128
08028cca  cb60      str	r3, [r1, #12]
08028ccc  9fe7      b	#-194 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028cce  cb68      ldr	r3, [r1, #12]
08028cd0  8020      movs	r0, #128
08028cd2  23f08003  bic	r3, r3, #128
08028cd6  cb60      str	r3, [r1, #12]
08028cd8  99e7      b	#-206 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028cda  cb68      ldr	r3, [r1, #12]
08028cdc  4ff48070  mov.w	r0, #256
08028ce0  23f08003  bic	r3, r3, #128
08028ce4  cb60      str	r3, [r1, #12]
08028ce6  92e7      b	#-220 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028ce8  cb68      ldr	r3, [r1, #12]
08028cea  4ff40070  mov.w	r0, #512
08028cee  23f08003  bic	r3, r3, #128
08028cf2  cb60      str	r3, [r1, #12]
08028cf4  8be7      b	#-234 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028cf6  cb68      ldr	r3, [r1, #12]
08028cf8  4ff48060  mov.w	r0, #1024
08028cfc  23f08003  bic	r3, r3, #128
08028d00  cb60      str	r3, [r1, #12]
08028d02  84e7      b	#-248 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d04  cb68      ldr	r3, [r1, #12]
08028d06  4ff48050  mov.w	r0, #4096
08028d0a  23f08003  bic	r3, r3, #128
08028d0e  cb60      str	r3, [r1, #12]
08028d10  7de7      b	#-262 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d12  cb68      ldr	r3, [r1, #12]
08028d14  4ff40060  mov.w	r0, #2048
08028d18  23f08003  bic	r3, r3, #128
08028d1c  cb60      str	r3, [r1, #12]
08028d1e  76e7      b	#-276 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d20  cb68      ldr	r3, [r1, #12]
08028d22  4ff48040  mov.w	r0, #16384
08028d26  23f08003  bic	r3, r3, #128
08028d2a  cb60      str	r3, [r1, #12]
08028d2c  6fe7      b	#-290 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d2e  cb68      ldr	r3, [r1, #12]
08028d30  4ff40050  mov.w	r0, #8192
08028d34  23f08003  bic	r3, r3, #128
08028d38  cb60      str	r3, [r1, #12]
08028d3a  68e7      b	#-304 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d3c  cb68      ldr	r3, [r1, #12]
08028d3e  4ff48020  mov.w	r0, #262144
08028d42  23f08003  bic	r3, r3, #128
08028d46  cb60      str	r3, [r1, #12]
08028d48  61e7      b	#-318 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d4a  cb68      ldr	r3, [r1, #12]
08028d4c  4ff40030  mov.w	r0, #131072
08028d50  23f08003  bic	r3, r3, #128
08028d54  cb60      str	r3, [r1, #12]
08028d56  5ae7      b	#-332 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d58  cb68      ldr	r3, [r1, #12]
08028d5a  4ff40040  mov.w	r0, #32768
08028d5e  23f08003  bic	r3, r3, #128
08028d62  cb60      str	r3, [r1, #12]
08028d64  53e7      b	#-346 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d66  cb68      ldr	r3, [r1, #12]
08028d68  4ff40010  mov.w	r0, #2097152
08028d6c  23f08003  bic	r3, r3, #128
08028d70  cb60      str	r3, [r1, #12]
08028d72  4ce7      b	#-360 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d74  cb68      ldr	r3, [r1, #12]
08028d76  4ff48010  mov.w	r0, #1048576
08028d7a  23f08003  bic	r3, r3, #128
08028d7e  cb60      str	r3, [r1, #12]
08028d80  45e7      b	#-374 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d82  cb68      ldr	r3, [r1, #12]
08028d84  4ff48000  mov.w	r0, #4194304
08028d88  23f08003  bic	r3, r3, #128
08028d8c  cb60      str	r3, [r1, #12]
08028d8e  3ee7      b	#-388 ; -> 0x08028c0e ; branch_target=0x08028c0e
08028d90  cb68      ldr	r3, [r1, #12]
08028d92  4ff40020  mov.w	r0, #524288
08028d96  23f08003  bic	r3, r3, #128
08028d9a  cb60      str	r3, [r1, #12]
08028d9c  37e7      b	#-402 ; -> 0x08028c0e ; branch_target=0x08028c0e
