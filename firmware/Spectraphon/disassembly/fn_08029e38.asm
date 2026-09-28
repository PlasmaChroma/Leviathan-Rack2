; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029e38  0023      movs	r3, #0
08029e3a  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08029e3e  0b60      str	r3, [r1]
08029e40  0468      ldr	r4, [r0]
08029e42  8cb1      cbz	r4, #34 ; -> 0x08029e68 ; branch_target=0x08029e68
08029e44  2578      ldrb	r5, [r4]
08029e46  202d      cmp	r5, #32
08029e48  11d9      bls	#34 ; -> 0x08029e6e ; branch_target=0x08029e6e
08029e4a  3a2d      cmp	r5, #58
08029e4c  0fd0      beq	#30 ; -> 0x08029e6e ; branch_target=0x08029e6e
08029e4e  a446      mov	r12, r4
08029e50  1cf8013f  ldrb	r3, [r12, #1]!
08029e54  202b      cmp	r3, #32
08029e56  0cd9      bls	#24 ; -> 0x08029e72 ; branch_target=0x08029e72
08029e58  3a2b      cmp	r3, #58
08029e5a  f9d1      bne	#-14 ; -> 0x08029e50 ; branch_target=0x08029e50
08029e5c  0134      adds	r4, #1
08029e5e  a445      cmp	r12, r4
08029e60  02d1      bne	#4 ; -> 0x08029e68 ; branch_target=0x08029e68
08029e62  302d      cmp	r5, #48
08029e64  00f0e180  beq.w	#450 ; -> 0x0802a02a ; branch_target=0x0802a02a
08029e68  0b20      movs	r0, #11
08029e6a  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08029e6e  2b46      mov	r3, r5
08029e70  a446      mov	r12, r4
08029e72  3a2b      cmp	r3, #58
08029e74  f2d0      beq	#-28 ; -> 0x08029e5c ; branch_target=0x08029e5c
08029e76  a14b      ldr	r3, [pc, #644] ; [0x0802a0fc] = 0x20002090
08029e78  1c68      ldr	r4, [r3]
08029e7a  002c      cmp	r4, #0
08029e7c  00f0c680  beq.w	#396 ; -> 0x0802a00c ; branch_target=0x0802a00c
08029e80  0c60      str	r4, [r1]
08029e82  02f0fe05  and	r5, r2, #254
08029e86  2378      ldrb	r3, [r4]
08029e88  53b1      cbz	r3, #20 ; -> 0x08029ea0 ; branch_target=0x08029ea0
08029e8a  6078      ldrb	r0, [r4, #1]
08029e8c  fff718fd  bl	#-1488 ; -> 0x080298c0 ; branch_target=0x080298c0
08029e90  c307      lsls	r3, r0, #31
08029e92  05d4      bmi	#10 ; -> 0x08029ea0 ; branch_target=0x08029ea0
08029e94  15b1      cbz	r5, #4 ; -> 0x08029e9c ; branch_target=0x08029e9c
08029e96  4707      lsls	r7, r0, #29
08029e98  00f1b580  bmi.w	#362 ; -> 0x0802a006 ; branch_target=0x0802a006
08029e9c  0020      movs	r0, #0
08029e9e  8de0      b	#282 ; -> 0x08029fbc ; branch_target=0x08029fbc
08029ea0  0020      movs	r0, #0
08029ea2  2080      strh	r0, [r4]
08029ea4  fff716fd  bl	#-1492 ; -> 0x080298d4 ; branch_target=0x080298d4
08029ea8  c607      lsls	r6, r0, #31
08029eaa  00f18980  bmi.w	#274 ; -> 0x08029fc0 ; branch_target=0x08029fc0
08029eae  15b1      cbz	r5, #4 ; -> 0x08029eb6 ; branch_target=0x08029eb6
08029eb0  4507      lsls	r5, r0, #29
08029eb2  00f1a880  bmi.w	#336 ; -> 0x0802a006 ; branch_target=0x0802a006
08029eb6  0021      movs	r1, #0
08029eb8  2046      mov	r0, r4
08029eba  fff7dffe  bl	#-578 ; -> 0x08029c7c ; branch_target=0x08029c7c
08029ebe  0228      cmp	r0, #2
08029ec0  00f08180  beq.w	#258 ; -> 0x08029fc6 ; branch_target=0x08029fc6
08029ec4  0428      cmp	r0, #4
08029ec6  00f0ac80  beq.w	#344 ; -> 0x0802a022 ; branch_target=0x0802a022
08029eca  0128      cmp	r0, #1
08029ecc  75d8      bhi	#234 ; -> 0x08029fba ; branch_target=0x08029fba
08029ece  0025      movs	r5, #0
08029ed0  b4f83b30  ldrh.w	r3, [r4, #59]
08029ed4  b3f5007f  cmp.w	r3, #512
08029ed8  6fd1      bne	#222 ; -> 0x08029fba ; branch_target=0x08029fba
08029eda  b4f84610  ldrh.w	r1, [r4, #70]
08029ede  01b9      cbnz	r1, #0 ; -> 0x08029ee2 ; branch_target=0x08029ee2
08029ee0  616d      ldr	r1, [r4, #84]
08029ee2  94f84020  ldrb.w	r2, [r4, #64]
08029ee6  a161      str	r1, [r4, #24]
08029ee8  531e      subs	r3, r2, #1
08029eea  a270      strb	r2, [r4, #2]
08029eec  012b      cmp	r3, #1
08029eee  64d8      bhi	#200 ; -> 0x08029fba ; branch_target=0x08029fba
08029ef0  94f83d30  ldrb.w	r3, [r4, #61]
08029ef4  6381      strh	r3, [r4, #10]
08029ef6  002b      cmp	r3, #0
08029ef8  5fd0      beq	#190 ; -> 0x08029fba ; branch_target=0x08029fba
08029efa  581e      subs	r0, r3, #1
08029efc  1842      tst	r0, r3
08029efe  5cd1      bne	#184 ; -> 0x08029fba ; branch_target=0x08029fba
08029f00  b4f84170  ldrh.w	r7, [r4, #65]
08029f04  3807      lsls	r0, r7, #28
08029f06  2781      strh	r7, [r4, #8]
08029f08  57d1      bne	#174 ; -> 0x08029fba ; branch_target=0x08029fba
08029f0a  b4f84300  ldrh.w	r0, [r4, #67]
08029f0e  00b9      cbnz	r0, #0 ; -> 0x08029f12 ; branch_target=0x08029f12
08029f10  206d      ldr	r0, [r4, #80]
08029f12  b4f83ec0  ldrh.w	r12, [r4, #62]
08029f16  bcf1000f  cmp.w	r12, #0
08029f1a  4ed0      beq	#156 ; -> 0x08029fba ; branch_target=0x08029fba
08029f1c  01fb02f2  mul	r2, r1, r2
08029f20  0ceb1716  add.w	r6, r12, r7, lsr #4
08029f24  1644      add	r6, r2
08029f26  b042      cmp	r0, r6
08029f28  47d3      blo	#142 ; -> 0x08029fba ; branch_target=0x08029fba
08029f2a  801b      subs	r0, r0, r6
08029f2c  9842      cmp	r0, r3
08029f2e  b0fbf3fe  udiv	lr, r0, r3
08029f32  42d3      blo	#132 ; -> 0x08029fba ; branch_target=0x08029fba
08029f34  0ceb0503  add.w	r3, r12, r5
08029f38  4ff6f57c  movw	r12, #65525
08029f3c  2e44      add	r6, r5
08029f3e  0ef10200  add.w	r0, lr, #2
08029f42  e645      cmp	lr, r12
08029f44  40f28f80  bls.w	#286 ; -> 0x0802a066 ; branch_target=0x0802a066
08029f48  2362      str	r3, [r4, #32]
08029f4a  b4f85a30  ldrh.w	r3, [r4, #90]
08029f4e  a662      str	r6, [r4, #40]
08029f50  3b43      orrs	r3, r7
08029f52  6061      str	r0, [r4, #20]
08029f54  e561      str	r5, [r4, #28]
08029f56  30d1      bne	#96 ; -> 0x08029fba ; branch_target=0x08029fba
08029f58  8300      lsls	r3, r0, #2
08029f5a  e26d      ldr	r2, [r4, #92]
08029f5c  03f2ff13  addw	r3, r3, #511
08029f60  6262      str	r2, [r4, #36]
08029f62  b1eb532f  cmp.w	r1, r3, lsr #9
08029f66  28d3      blo	#80 ; -> 0x08029fba ; branch_target=0x08029fba
08029f68  b4f86020  ldrh.w	r2, [r4, #96]
08029f6c  4ff0ff33  mov.w	r3, #4294967295
08029f70  8021      movs	r1, #128
08029f72  012a      cmp	r2, #1
08029f74  2171      strb	r1, [r4, #4]
08029f76  c4e90333  strd	r3, r3, [r4, #12]
08029f7a  00f09c80  beq.w	#312 ; -> 0x0802a0b6 ; branch_target=0x0802a0b6
08029f7e  0323      movs	r3, #3
08029f80  5f49      ldr	r1, [pc, #380] ; [0x0802a100] = 0x2000208c
08029f82  2370      strb	r3, [r4]
08029f84  5f4a      ldr	r2, [pc, #380] ; [0x0802a104] = 0x2000206c
08029f86  0b88      ldrh	r3, [r1]
08029f88  1068      ldr	r0, [r2]
08029f8a  0133      adds	r3, #1
08029f8c  a042      cmp	r0, r4
08029f8e  9bb2      uxth	r3, r3
08029f90  0b80      strh	r3, [r1]
08029f92  e380      strh	r3, [r4, #6]
08029f94  00f08c80  beq.w	#280 ; -> 0x0802a0b0 ; branch_target=0x0802a0b0
08029f98  1369      ldr	r3, [r2, #16]
08029f9a  9c42      cmp	r4, r3
08029f9c  7ff47eaf  bne.w	#-260 ; -> 0x08029e9c ; branch_target=0x08029e9c
08029fa0  0023      movs	r3, #0
08029fa2  1361      str	r3, [r2, #16]
08029fa4  7ae7      b	#-268 ; -> 0x08029e9c ; branch_target=0x08029e9c
08029fa6  d4f82652  ldr.w	r5, [r4, #550]
08029faa  002e      cmp	r6, #0
08029fac  4ad1      bne	#148 ; -> 0x0802a044 ; branch_target=0x0802a044
08029fae  002f      cmp	r7, #0
08029fb0  51d1      bne	#162 ; -> 0x0802a056 ; branch_target=0x0802a056
08029fb2  b8f1000f  cmp.w	r8, #0
08029fb6  3cd1      bne	#120 ; -> 0x0802a032 ; branch_target=0x0802a032
08029fb8  55bb      cbnz	r5, #84 ; -> 0x0802a010 ; branch_target=0x0802a010
08029fba  0d20      movs	r0, #13
08029fbc  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08029fc0  0320      movs	r0, #3
08029fc2  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08029fc6  94f8f261  ldrb.w	r6, [r4, #498]
08029fca  0eb1      cbz	r6, #2 ; -> 0x08029fd0 ; branch_target=0x08029fd0
08029fcc  d4f8f661  ldr.w	r6, [r4, #502]
08029fd0  94f80272  ldrb.w	r7, [r4, #514]
08029fd4  0fb1      cbz	r7, #2 ; -> 0x08029fda ; branch_target=0x08029fda
08029fd6  d4f80672  ldr.w	r7, [r4, #518]
08029fda  94f81232  ldrb.w	r3, [r4, #530]
08029fde  13b3      cbz	r3, #68 ; -> 0x0802a026 ; branch_target=0x0802a026
08029fe0  d4f81682  ldr.w	r8, [r4, #534]
08029fe4  94f82252  ldrb.w	r5, [r4, #546]
08029fe8  002d      cmp	r5, #0
08029fea  dcd1      bne	#-72 ; -> 0x08029fa6 ; branch_target=0x08029fa6
08029fec  56bb      cbnz	r6, #84 ; -> 0x0802a044 ; branch_target=0x0802a044
08029fee  8fbb      cbnz	r7, #98 ; -> 0x0802a054 ; branch_target=0x0802a054
08029ff0  b8f1000f  cmp.w	r8, #0
08029ff4  e1d0      beq	#-62 ; -> 0x08029fba ; branch_target=0x08029fba
08029ff6  4146      mov	r1, r8
08029ff8  2046      mov	r0, r4
08029ffa  fff73ffe  bl	#-898 ; -> 0x08029c7c ; branch_target=0x08029c7c
08029ffe  0128      cmp	r0, #1
0802a000  dbd8      bhi	#-74 ; -> 0x08029fba ; branch_target=0x08029fba
0802a002  4546      mov	r5, r8
0802a004  64e7      b	#-312 ; -> 0x08029ed0 ; branch_target=0x08029ed0
0802a006  0a20      movs	r0, #10
0802a008  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
0802a00c  0c20      movs	r0, #12
0802a00e  d5e7      b	#-86 ; -> 0x08029fbc ; branch_target=0x08029fbc
0802a010  2946      mov	r1, r5
0802a012  2046      mov	r0, r4
0802a014  fff732fe  bl	#-924 ; -> 0x08029c7c ; branch_target=0x08029c7c
0802a018  0128      cmp	r0, #1
0802a01a  7ff659af  bls.w	#-334 ; -> 0x08029ed0 ; branch_target=0x08029ed0
0802a01e  0428      cmp	r0, #4
0802a020  cbd1      bne	#-106 ; -> 0x08029fba ; branch_target=0x08029fba
0802a022  0120      movs	r0, #1
0802a024  cae7      b	#-108 ; -> 0x08029fbc ; branch_target=0x08029fbc
0802a026  9846      mov	r8, r3
0802a028  dce7      b	#-72 ; -> 0x08029fe4 ; branch_target=0x08029fe4
0802a02a  0cf10103  add.w	r3, r12, #1
0802a02e  0360      str	r3, [r0]
0802a030  21e7      b	#-446 ; -> 0x08029e76 ; branch_target=0x08029e76
0802a032  4146      mov	r1, r8
0802a034  2046      mov	r0, r4
0802a036  fff721fe  bl	#-958 ; -> 0x08029c7c ; branch_target=0x08029c7c
0802a03a  0128      cmp	r0, #1
0802a03c  e1d9      bls	#-62 ; -> 0x0802a002 ; branch_target=0x0802a002
0802a03e  002d      cmp	r5, #0
0802a040  e6d1      bne	#-52 ; -> 0x0802a010 ; branch_target=0x0802a010
0802a042  bae7      b	#-140 ; -> 0x08029fba ; branch_target=0x08029fba
0802a044  3146      mov	r1, r6
0802a046  2046      mov	r0, r4
0802a048  fff718fe  bl	#-976 ; -> 0x08029c7c ; branch_target=0x08029c7c
0802a04c  0128      cmp	r0, #1
0802a04e  aed8      bhi	#-164 ; -> 0x08029fae ; branch_target=0x08029fae
0802a050  3546      mov	r5, r6
0802a052  3de7      b	#-390 ; -> 0x08029ed0 ; branch_target=0x08029ed0
0802a054  3546      mov	r5, r6
0802a056  3946      mov	r1, r7
0802a058  2046      mov	r0, r4
0802a05a  fff70ffe  bl	#-994 ; -> 0x08029c7c ; branch_target=0x08029c7c
0802a05e  0128      cmp	r0, #1
0802a060  a7d8      bhi	#-178 ; -> 0x08029fb2 ; branch_target=0x08029fb2
0802a062  3d46      mov	r5, r7
0802a064  34e7      b	#-408 ; -> 0x08029ed0 ; branch_target=0x08029ed0
0802a066  40f6f57c  movw	r12, #4085
0802a06a  e561      str	r5, [r4, #28]
0802a06c  a662      str	r6, [r4, #40]
0802a06e  e645      cmp	lr, r12
0802a070  6061      str	r0, [r4, #20]
0802a072  2362      str	r3, [r4, #32]
0802a074  16d8      bhi	#44 ; -> 0x0802a0a4 ; branch_target=0x0802a0a4
0802a076  002f      cmp	r7, #0
0802a078  9fd0      beq	#-194 ; -> 0x08029fba ; branch_target=0x08029fba
0802a07a  1a44      add	r2, r3
0802a07c  00eb4003  add.w	r3, r0, r0, lsl #1
0802a080  00f00100  and	r0, r0, #1
0802a084  00eb5300  add.w	r0, r0, r3, lsr #1
0802a088  0123      movs	r3, #1
0802a08a  00f2ff10  addw	r0, r0, #511
0802a08e  6262      str	r2, [r4, #36]
0802a090  b1eb502f  cmp.w	r1, r0, lsr #9
0802a094  91d3      blo	#-222 ; -> 0x08029fba ; branch_target=0x08029fba
0802a096  4ff0ff32  mov.w	r2, #4294967295
0802a09a  8021      movs	r1, #128
0802a09c  c4e90322  strd	r2, r2, [r4, #12]
0802a0a0  2171      strb	r1, [r4, #4]
0802a0a2  6de7      b	#-294 ; -> 0x08029f80 ; branch_target=0x08029f80
0802a0a4  002f      cmp	r7, #0
0802a0a6  88d0      beq	#-240 ; -> 0x08029fba ; branch_target=0x08029fba
0802a0a8  1a44      add	r2, r3
0802a0aa  4000      lsls	r0, r0, #1
0802a0ac  0223      movs	r3, #2
0802a0ae  ece7      b	#-40 ; -> 0x0802a08a ; branch_target=0x0802a08a
0802a0b0  0023      movs	r3, #0
0802a0b2  1360      str	r3, [r2]
0802a0b4  70e7      b	#-288 ; -> 0x08029f98 ; branch_target=0x08029f98
0802a0b6  691c      adds	r1, r5, #1
0802a0b8  2046      mov	r0, r4
0802a0ba  fff77ffe  bl	#-770 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a0be  0028      cmp	r0, #0
0802a0c0  7ff45daf  bne.w	#-326 ; -> 0x08029f7e ; branch_target=0x08029f7e
0802a0c4  4af65522  movw	r2, #43605
0802a0c8  b4f82e12  ldrh.w	r1, [r4, #558]
0802a0cc  2071      strb	r0, [r4, #4]
0802a0ce  9142      cmp	r1, r2
0802a0d0  7ff455af  bne.w	#-342 ; -> 0x08029f7e ; branch_target=0x08029f7e
0802a0d4  226b      ldr	r2, [r4, #48]
0802a0d6  0c4b      ldr	r3, [pc, #48] ; [0x0802a108] = 0x41615252 / f32_bits_interpretation=14.08259773
0802a0d8  9a42      cmp	r2, r3
0802a0da  7ff450af  bne.w	#-352 ; -> 0x08029f7e ; branch_target=0x08029f7e
0802a0de  03f1ff53  add.w	r3, r3, #534773760
0802a0e2  d4f81422  ldr.w	r2, [r4, #532]
0802a0e6  03f50053  add.w	r3, r3, #8192
0802a0ea  2033      adds	r3, #32
0802a0ec  9a42      cmp	r2, r3
0802a0ee  7ff446af  bne.w	#-372 ; -> 0x08029f7e ; branch_target=0x08029f7e
0802a0f2  d4e98623  ldrd	r2, r3, [r4, #536]
0802a0f6  c4e90332  strd	r3, r2, [r4, #12]
0802a0fa  40e7      b	#-384 ; -> 0x08029f7e ; branch_target=0x08029f7e
