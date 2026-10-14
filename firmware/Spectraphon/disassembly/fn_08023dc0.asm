; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08023dc0  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08023dc4  1368      ldr	r3, [r2]
08023dc6  0446      mov	r4, r0
08023dc8  0d46      mov	r5, r1
08023dca  03f00f03  and	r3, r3, #15
08023dce  8b42      cmp	r3, r1
08023dd0  0cd2      bhs	#24 ; -> 0x08023dec ; branch_target=0x08023dec
08023dd2  1368      ldr	r3, [r2]
08023dd4  23f00f03  bic	r3, r3, #15
08023dd8  0b43      orrs	r3, r1
08023dda  1360      str	r3, [r2]
08023ddc  1368      ldr	r3, [r2]
08023dde  03f00f03  and	r3, r3, #15
08023de2  8b42      cmp	r3, r1
08023de4  02d0      beq	#4 ; -> 0x08023dec ; branch_target=0x08023dec
08023de6  0120      movs	r0, #1
08023de8  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08023dec  2368      ldr	r3, [r4]
08023dee  5f07      lsls	r7, r3, #29
08023df0  0cd5      bpl	#24 ; -> 0x08023e0c ; branch_target=0x08023e0c
08023df2  8649      ldr	r1, [pc, #536] ; [0x0802400c] = 0x58024400
08023df4  2069      ldr	r0, [r4, #16]
08023df6  8a69      ldr	r2, [r1, #24]
08023df8  02f07002  and	r2, r2, #112
08023dfc  9042      cmp	r0, r2
08023dfe  05d9      bls	#10 ; -> 0x08023e0c ; branch_target=0x08023e0c
08023e00  8b69      ldr	r3, [r1, #24]
08023e02  23f07003  bic	r3, r3, #112
08023e06  0343      orrs	r3, r0
08023e08  8b61      str	r3, [r1, #24]
08023e0a  2368      ldr	r3, [r4]
08023e0c  1e07      lsls	r6, r3, #28
08023e0e  0cd5      bpl	#24 ; -> 0x08023e2a ; branch_target=0x08023e2a
08023e10  7e49      ldr	r1, [pc, #504] ; [0x0802400c] = 0x58024400
08023e12  6069      ldr	r0, [r4, #20]
08023e14  ca69      ldr	r2, [r1, #28]
08023e16  02f07002  and	r2, r2, #112
08023e1a  9042      cmp	r0, r2
08023e1c  05d9      bls	#10 ; -> 0x08023e2a ; branch_target=0x08023e2a
08023e1e  cb69      ldr	r3, [r1, #28]
08023e20  23f07003  bic	r3, r3, #112
08023e24  0343      orrs	r3, r0
08023e26  cb61      str	r3, [r1, #28]
08023e28  2368      ldr	r3, [r4]
08023e2a  d806      lsls	r0, r3, #27
08023e2c  0cd5      bpl	#24 ; -> 0x08023e48 ; branch_target=0x08023e48
08023e2e  7749      ldr	r1, [pc, #476] ; [0x0802400c] = 0x58024400
08023e30  a069      ldr	r0, [r4, #24]
08023e32  ca69      ldr	r2, [r1, #28]
08023e34  02f4e062  and	r2, r2, #1792
08023e38  9042      cmp	r0, r2
08023e3a  05d9      bls	#10 ; -> 0x08023e48 ; branch_target=0x08023e48
08023e3c  cb69      ldr	r3, [r1, #28]
08023e3e  23f4e063  bic	r3, r3, #1792
08023e42  0343      orrs	r3, r0
08023e44  cb61      str	r3, [r1, #28]
08023e46  2368      ldr	r3, [r4]
08023e48  9906      lsls	r1, r3, #26
08023e4a  0cd5      bpl	#24 ; -> 0x08023e66 ; branch_target=0x08023e66
08023e4c  6f49      ldr	r1, [pc, #444] ; [0x0802400c] = 0x58024400
08023e4e  e069      ldr	r0, [r4, #28]
08023e50  0a6a      ldr	r2, [r1, #32]
08023e52  02f07002  and	r2, r2, #112
08023e56  9042      cmp	r0, r2
08023e58  05d9      bls	#10 ; -> 0x08023e66 ; branch_target=0x08023e66
08023e5a  0b6a      ldr	r3, [r1, #32]
08023e5c  23f07003  bic	r3, r3, #112
08023e60  0343      orrs	r3, r0
08023e62  0b62      str	r3, [r1, #32]
08023e64  2368      ldr	r3, [r4]
08023e66  9a07      lsls	r2, r3, #30
08023e68  40f18380  bpl.w	#262 ; -> 0x08023f72 ; branch_target=0x08023f72
08023e6c  6748      ldr	r0, [pc, #412] ; [0x0802400c] = 0x58024400
08023e6e  e168      ldr	r1, [r4, #12]
08023e70  8269      ldr	r2, [r0, #24]
08023e72  02f00f02  and	r2, r2, #15
08023e76  9142      cmp	r1, r2
08023e78  40f2a980  bls.w	#338 ; -> 0x08023fce ; branch_target=0x08023fce
08023e7c  8369      ldr	r3, [r0, #24]
08023e7e  23f00f03  bic	r3, r3, #15
08023e82  0b43      orrs	r3, r1
08023e84  8361      str	r3, [r0, #24]
08023e86  2368      ldr	r3, [r4]
08023e88  df07      lsls	r7, r3, #31
08023e8a  74d4      bmi	#232 ; -> 0x08023f76 ; branch_target=0x08023f76
08023e8c  9f07      lsls	r7, r3, #30
08023e8e  07d5      bpl	#14 ; -> 0x08023ea0 ; branch_target=0x08023ea0
08023e90  5e4a      ldr	r2, [pc, #376] ; [0x0802400c] = 0x58024400
08023e92  e168      ldr	r1, [r4, #12]
08023e94  9369      ldr	r3, [r2, #24]
08023e96  03f00f03  and	r3, r3, #15
08023e9a  8b42      cmp	r3, r1
08023e9c  00f2a080  bhi.w	#320 ; -> 0x08023fe0 ; branch_target=0x08023fe0
08023ea0  594a      ldr	r2, [pc, #356] ; [0x08024008] = 0x52002000
08023ea2  1368      ldr	r3, [r2]
08023ea4  03f00f03  and	r3, r3, #15
08023ea8  ab42      cmp	r3, r5
08023eaa  09d9      bls	#18 ; -> 0x08023ec0 ; branch_target=0x08023ec0
08023eac  1368      ldr	r3, [r2]
08023eae  23f00f03  bic	r3, r3, #15
08023eb2  2b43      orrs	r3, r5
08023eb4  1360      str	r3, [r2]
08023eb6  1368      ldr	r3, [r2]
08023eb8  03f00f03  and	r3, r3, #15
08023ebc  ab42      cmp	r3, r5
08023ebe  92d1      bne	#-220 ; -> 0x08023de6 ; branch_target=0x08023de6
08023ec0  2368      ldr	r3, [r4]
08023ec2  5e07      lsls	r6, r3, #29
08023ec4  0cd5      bpl	#24 ; -> 0x08023ee0 ; branch_target=0x08023ee0
08023ec6  5149      ldr	r1, [pc, #324] ; [0x0802400c] = 0x58024400
08023ec8  2069      ldr	r0, [r4, #16]
08023eca  8a69      ldr	r2, [r1, #24]
08023ecc  02f07002  and	r2, r2, #112
08023ed0  9042      cmp	r0, r2
08023ed2  05d2      bhs	#10 ; -> 0x08023ee0 ; branch_target=0x08023ee0
08023ed4  8b69      ldr	r3, [r1, #24]
08023ed6  23f07003  bic	r3, r3, #112
08023eda  0343      orrs	r3, r0
08023edc  8b61      str	r3, [r1, #24]
08023ede  2368      ldr	r3, [r4]
08023ee0  1d07      lsls	r5, r3, #28
08023ee2  0cd5      bpl	#24 ; -> 0x08023efe ; branch_target=0x08023efe
08023ee4  4949      ldr	r1, [pc, #292] ; [0x0802400c] = 0x58024400
08023ee6  6069      ldr	r0, [r4, #20]
08023ee8  ca69      ldr	r2, [r1, #28]
08023eea  02f07002  and	r2, r2, #112
08023eee  9042      cmp	r0, r2
08023ef0  05d2      bhs	#10 ; -> 0x08023efe ; branch_target=0x08023efe
08023ef2  cb69      ldr	r3, [r1, #28]
08023ef4  23f07003  bic	r3, r3, #112
08023ef8  0343      orrs	r3, r0
08023efa  cb61      str	r3, [r1, #28]
08023efc  2368      ldr	r3, [r4]
08023efe  d806      lsls	r0, r3, #27
08023f00  0cd5      bpl	#24 ; -> 0x08023f1c ; branch_target=0x08023f1c
08023f02  4249      ldr	r1, [pc, #264] ; [0x0802400c] = 0x58024400
08023f04  a069      ldr	r0, [r4, #24]
08023f06  ca69      ldr	r2, [r1, #28]
08023f08  02f4e062  and	r2, r2, #1792
08023f0c  9042      cmp	r0, r2
08023f0e  05d2      bhs	#10 ; -> 0x08023f1c ; branch_target=0x08023f1c
08023f10  cb69      ldr	r3, [r1, #28]
08023f12  23f4e063  bic	r3, r3, #1792
08023f16  0343      orrs	r3, r0
08023f18  cb61      str	r3, [r1, #28]
08023f1a  2368      ldr	r3, [r4]
08023f1c  9906      lsls	r1, r3, #26
08023f1e  0bd5      bpl	#22 ; -> 0x08023f38 ; branch_target=0x08023f38
08023f20  3a4a      ldr	r2, [pc, #232] ; [0x0802400c] = 0x58024400
08023f22  e169      ldr	r1, [r4, #28]
08023f24  136a      ldr	r3, [r2, #32]
08023f26  03f07003  and	r3, r3, #112
08023f2a  9942      cmp	r1, r3
08023f2c  04d2      bhs	#8 ; -> 0x08023f38 ; branch_target=0x08023f38
08023f2e  136a      ldr	r3, [r2, #32]
08023f30  23f07003  bic	r3, r3, #112
08023f34  0b43      orrs	r3, r1
08023f36  1362      str	r3, [r2, #32]
08023f38  fff7b0fe  bl	#-672 ; -> 0x08023c9c ; branch_target=0x08023c9c
08023f3c  334a      ldr	r2, [pc, #204] ; [0x0802400c] = 0x58024400
08023f3e  0346      mov	r3, r0
08023f40  3348      ldr	r0, [pc, #204] ; [0x08024010] = 0x08049aac
08023f42  9169      ldr	r1, [r2, #24]
08023f44  9269      ldr	r2, [r2, #24]
08023f46  c1f30321  ubfx	r1, r1, #8, #4
08023f4a  324d      ldr	r5, [pc, #200] ; [0x08024014] = 0x20000014
08023f4c  02f00f02  and	r2, r2, #15
08023f50  314c      ldr	r4, [pc, #196] ; [0x08024018] = 0x20000010
08023f52  415c      ldrb	r1, [r0, r1]
08023f54  825c      ldrb	r2, [r0, r2]
08023f56  01f01f01  and	r1, r1, #31
08023f5a  3048      ldr	r0, [pc, #192] ; [0x0802401c] = 0x20000004
08023f5c  02f01f02  and	r2, r2, #31
08023f60  cb40      lsrs	r3, r1
08023f62  0068      ldr	r0, [r0]
08023f64  2b60      str	r3, [r5]
08023f66  d340      lsrs	r3, r2
08023f68  2360      str	r3, [r4]
08023f6a  bde8f041  pop.w	{r4, r5, r6, r7, r8, lr}
08023f6e  fcf7b3b9  b.w	#-15514 ; -> 0x080202d8 ; branch_target=0x080202d8
08023f72  db07      lsls	r3, r3, #31
08023f74  94d5      bpl	#-216 ; -> 0x08023ea0 ; branch_target=0x08023ea0
08023f76  254a      ldr	r2, [pc, #148] ; [0x0802400c] = 0x58024400
08023f78  a168      ldr	r1, [r4, #8]
08023f7a  9369      ldr	r3, [r2, #24]
08023f7c  23f47063  bic	r3, r3, #3840
08023f80  0b43      orrs	r3, r1
08023f82  9361      str	r3, [r2, #24]
08023f84  6168      ldr	r1, [r4, #4]
08023f86  1368      ldr	r3, [r2]
08023f88  0229      cmp	r1, #2
08023f8a  34d0      beq	#104 ; -> 0x08023ff6 ; branch_target=0x08023ff6
08023f8c  0329      cmp	r1, #3
08023f8e  2dd0      beq	#90 ; -> 0x08023fec ; branch_target=0x08023fec
08023f90  0129      cmp	r1, #1
08023f92  35d0      beq	#106 ; -> 0x08024000 ; branch_target=0x08024000
08023f94  5b07      lsls	r3, r3, #29
08023f96  7ff526af  bpl.w	#-436 ; -> 0x08023de6 ; branch_target=0x08023de6
08023f9a  1c4e      ldr	r6, [pc, #112] ; [0x0802400c] = 0x58024400
08023f9c  41f28838  movw	r8, #5000
08023fa0  3369      ldr	r3, [r6, #16]
08023fa2  23f00703  bic	r3, r3, #7
08023fa6  0b43      orrs	r3, r1
08023fa8  3361      str	r3, [r6, #16]
08023faa  fcf7f7f9  bl	#-15378 ; -> 0x0802039c ; branch_target=0x0802039c
08023fae  0746      mov	r7, r0
08023fb0  04e0      b	#8 ; -> 0x08023fbc ; branch_target=0x08023fbc
08023fb2  fcf7f3f9  bl	#-15386 ; -> 0x0802039c ; branch_target=0x0802039c
08023fb6  c01b      subs	r0, r0, r7
08023fb8  4045      cmp	r0, r8
08023fba  1fd8      bhi	#62 ; -> 0x08023ffc ; branch_target=0x08023ffc
08023fbc  3369      ldr	r3, [r6, #16]
08023fbe  6268      ldr	r2, [r4, #4]
08023fc0  03f03803  and	r3, r3, #56
08023fc4  b3ebc20f  cmp.w	r3, r2, lsl #3
08023fc8  f3d1      bne	#-26 ; -> 0x08023fb2 ; branch_target=0x08023fb2
08023fca  2368      ldr	r3, [r4]
08023fcc  5ee7      b	#-324 ; -> 0x08023e8c ; branch_target=0x08023e8c
08023fce  da07      lsls	r2, r3, #31
08023fd0  d1d4      bmi	#-94 ; -> 0x08023f76 ; branch_target=0x08023f76
08023fd2  0e4a      ldr	r2, [pc, #56] ; [0x0802400c] = 0x58024400
08023fd4  9369      ldr	r3, [r2, #24]
08023fd6  03f00f03  and	r3, r3, #15
08023fda  8b42      cmp	r3, r1
08023fdc  7ff660af  bls.w	#-320 ; -> 0x08023ea0 ; branch_target=0x08023ea0
08023fe0  9369      ldr	r3, [r2, #24]
08023fe2  23f00f03  bic	r3, r3, #15
08023fe6  0b43      orrs	r3, r1
08023fe8  9361      str	r3, [r2, #24]
08023fea  59e7      b	#-334 ; -> 0x08023ea0 ; branch_target=0x08023ea0
08023fec  9801      lsls	r0, r3, #6
08023fee  d4d4      bmi	#-88 ; -> 0x08023f9a ; branch_target=0x08023f9a
08023ff0  f9e6      b	#-526 ; -> 0x08023de6 ; branch_target=0x08023de6
08023ff6  9e03      lsls	r6, r3, #14
08023ff8  cfd4      bmi	#-98 ; -> 0x08023f9a ; branch_target=0x08023f9a
08023ffa  f4e6      b	#-536 ; -> 0x08023de6 ; branch_target=0x08023de6
08023ffc  0320      movs	r0, #3
08023ffe  f3e6      b	#-538 ; -> 0x08023de8 ; branch_target=0x08023de8
08024000  da05      lsls	r2, r3, #23
08024002  cad4      bmi	#-108 ; -> 0x08023f9a ; branch_target=0x08023f9a
08024004  efe6      b	#-546 ; -> 0x08023de6 ; branch_target=0x08023de6
