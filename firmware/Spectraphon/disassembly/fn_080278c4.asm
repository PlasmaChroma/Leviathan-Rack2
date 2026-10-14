; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080278c4  0028      cmp	r0, #0
080278c6  00f08b80  beq.w	#278 ; -> 0x080279e0 ; branch_target=0x080279e0
080278ca  70b5      push	{r4, r5, r6, lr}
080278cc  90f83d30  ldrb.w	r3, [r0, #61]
080278d0  0446      mov	r4, r0
080278d2  03f0ff02  and	r2, r3, #255
080278d6  002b      cmp	r3, #0
080278d8  70d0      beq	#224 ; -> 0x080279bc ; branch_target=0x080279bc
080278da  2268      ldr	r2, [r4]
080278dc  0223      movs	r3, #2
080278de  4148      ldr	r0, [pc, #260] ; [0x080279e4] = 0x40010000 / f32_bits_interpretation=2.015625
080278e0  414d      ldr	r5, [pc, #260] ; [0x080279e8] = 0x40010400 / f32_bits_interpretation=2.015869141
080278e2  b2f1804f  cmp.w	r2, #1073741824
080278e6  a2eb0000  sub.w	r0, r2, r0
080278ea  84f83d30  strb.w	r3, [r4, #61]
080278ee  a2eb0505  sub.w	r5, r2, r5
080278f2  1368      ldr	r3, [r2]
080278f4  b0fa80f0  clz	r0, r0
080278f8  a169      ldr	r1, [r4, #24]
080278fa  b5fa85f5  clz	r5, r5
080278fe  4fea5010  lsr.w	r0, r0, #5
08027902  4fea5515  lsr.w	r5, r5, #5
08027906  1fd0      beq	#62 ; -> 0x08027948 ; branch_target=0x08027948
08027908  f0b9      cbnz	r0, #60 ; -> 0x08027948 ; branch_target=0x08027948
0802790a  384e      ldr	r6, [pc, #224] ; [0x080279ec] = 0x40000400 / f32_bits_interpretation=2.000244141
0802790c  b242      cmp	r2, r6
0802790e  1bd0      beq	#54 ; -> 0x08027948 ; branch_target=0x08027948
08027910  06f58066  add.w	r6, r6, #1024
08027914  b242      cmp	r2, r6
08027916  17d0      beq	#46 ; -> 0x08027948 ; branch_target=0x08027948
08027918  06f58066  add.w	r6, r6, #1024
0802791c  b242      cmp	r2, r6
0802791e  13d0      beq	#38 ; -> 0x08027948 ; branch_target=0x08027948
08027920  95b9      cbnz	r5, #36 ; -> 0x08027948 ; branch_target=0x08027948
08027922  334d      ldr	r5, [pc, #204] ; [0x080279f0] = 0x40014000 / f32_bits_interpretation=2.01953125
08027924  3348      ldr	r0, [pc, #204] ; [0x080279f4] = 0x40014400 / f32_bits_interpretation=2.019775391
08027926  8242      cmp	r2, r0
08027928  18bf      it	ne
0802792a  aa42      cmpne	r2, r5
0802792c  4bd0      beq	#150 ; -> 0x080279c6 ; branch_target=0x080279c6
0802792e  00f58060  add.w	r0, r0, #1024
08027932  8242      cmp	r2, r0
08027934  47d0      beq	#142 ; -> 0x080279c6 ; branch_target=0x080279c6
08027936  23f08003  bic	r3, r3, #128
0802793a  0b43      orrs	r3, r1
0802793c  1360      str	r3, [r2]
0802793e  e368      ldr	r3, [r4, #12]
08027940  d362      str	r3, [r2, #44]
08027942  6368      ldr	r3, [r4, #4]
08027944  9362      str	r3, [r2, #40]
08027946  1de0      b	#58 ; -> 0x08027984 ; branch_target=0x08027984
08027948  a668      ldr	r6, [r4, #8]
0802794a  23f07003  bic	r3, r3, #112
0802794e  3343      orrs	r3, r6
08027950  2669      ldr	r6, [r4, #16]
08027952  23f44073  bic	r3, r3, #768
08027956  3343      orrs	r3, r6
08027958  23f08003  bic	r3, r3, #128
0802795c  0b43      orrs	r3, r1
0802795e  1360      str	r3, [r2]
08027960  e368      ldr	r3, [r4, #12]
08027962  d362      str	r3, [r2, #44]
08027964  6368      ldr	r3, [r4, #4]
08027966  9362      str	r3, [r2, #40]
08027968  50b9      cbnz	r0, #20 ; -> 0x08027980 ; branch_target=0x08027980
0802796a  4db9      cbnz	r5, #18 ; -> 0x08027980 ; branch_target=0x08027980
0802796c  2249      ldr	r1, [pc, #136] ; [0x080279f8] = 0x40014800 / f32_bits_interpretation=2.020019531
0802796e  204b      ldr	r3, [pc, #128] ; [0x080279f0] = 0x40014000 / f32_bits_interpretation=2.01953125
08027970  9a42      cmp	r2, r3
08027972  18bf      it	ne
08027974  8a42      cmpne	r2, r1
08027976  03d0      beq	#6 ; -> 0x08027980 ; branch_target=0x08027980
08027978  03f58063  add.w	r3, r3, #1024
0802797c  9a42      cmp	r2, r3
0802797e  01d1      bne	#2 ; -> 0x08027984 ; branch_target=0x08027984
08027980  6369      ldr	r3, [r4, #20]
08027982  1363      str	r3, [r2, #48]
08027984  0123      movs	r3, #1
08027986  0020      movs	r0, #0
08027988  5361      str	r3, [r2, #20]
0802798a  84f84830  strb.w	r3, [r4, #72]
0802798e  84f83e30  strb.w	r3, [r4, #62]
08027992  84f83f30  strb.w	r3, [r4, #63]
08027996  84f84030  strb.w	r3, [r4, #64]
0802799a  84f84130  strb.w	r3, [r4, #65]
0802799e  84f84230  strb.w	r3, [r4, #66]
080279a2  84f84330  strb.w	r3, [r4, #67]
080279a6  84f84430  strb.w	r3, [r4, #68]
080279aa  84f84530  strb.w	r3, [r4, #69]
080279ae  84f84630  strb.w	r3, [r4, #70]
080279b2  84f84730  strb.w	r3, [r4, #71]
080279b6  84f83d30  strb.w	r3, [r4, #61]
080279ba  70bd      pop	{r4, r5, r6, pc}
080279bc  80f83c20  strb.w	r2, [r0, #60]
080279c0  0ef024fb  bl	#58952 ; -> 0x0803600c ; branch_target=0x0803600c
080279c4  89e7      b	#-238 ; -> 0x080278da ; branch_target=0x080278da
080279c6  2069      ldr	r0, [r4, #16]
080279c8  23f44073  bic	r3, r3, #768
080279cc  0343      orrs	r3, r0
080279ce  23f08003  bic	r3, r3, #128
080279d2  0b43      orrs	r3, r1
080279d4  1360      str	r3, [r2]
080279d6  e368      ldr	r3, [r4, #12]
080279d8  d362      str	r3, [r2, #44]
080279da  6368      ldr	r3, [r4, #4]
080279dc  9362      str	r3, [r2, #40]
080279de  c5e7      b	#-118 ; -> 0x0802796c ; branch_target=0x0802796c
080279e0  0120      movs	r0, #1
080279e2  7047      bx	lr
