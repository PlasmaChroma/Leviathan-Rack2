; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08026d54  38b5      push	{r3, r4, r5, lr}
08026d56  0446      mov	r4, r0
08026d58  0020      movs	r0, #0
08026d5a  2268      ldr	r2, [r4]
08026d5c  e368      ldr	r3, [r4, #12]
08026d5e  8a42      cmp	r2, r1
08026d60  a062      str	r0, [r4, #40]
08026d62  13d0      beq	#38 ; -> 0x08026d8c ; branch_target=0x08026d8c
08026d64  a1f57841  sub.w	r1, r1, #63488
08026d68  8a42      cmp	r2, r1
08026d6a  0fd0      beq	#30 ; -> 0x08026d8c ; branch_target=0x08026d8c
08026d6c  01f58061  add.w	r1, r1, #1024
08026d70  8a42      cmp	r2, r1
08026d72  6cd0      beq	#216 ; -> 0x08026e4e ; branch_target=0x08026e4e
08026d74  0f2b      cmp	r3, #15
08026d76  07d8      bhi	#14 ; -> 0x08026d88 ; branch_target=0x08026d88
08026d78  e16b      ldr	r1, [r4, #60]
08026d7a  0833      adds	r3, #8
08026d7c  4909      lsrs	r1, r1, #5
08026d7e  db08      lsrs	r3, r3, #3
08026d80  01fb0333  mla	r3, r1, r3, r3
08026d84  082b      cmp	r3, #8
08026d86  08d9      bls	#16 ; -> 0x08026d9a ; branch_target=0x08026d9a
08026d88  0120      movs	r0, #1
08026d8a  38bd      pop	{r3, r4, r5, pc}
08026d8c  4549      ldr	r1, [pc, #276] ; [0x08026ea4] = 0x40013000 / f32_bits_interpretation=2.018554688
08026d8e  8a42      cmp	r2, r1
08026d90  5dd0      beq	#186 ; -> 0x08026e4e ; branch_target=0x08026e4e
08026d92  a1f57841  sub.w	r1, r1, #63488
08026d96  8a42      cmp	r2, r1
08026d98  59d0      beq	#178 ; -> 0x08026e4e ; branch_target=0x08026e4e
08026d9a  94f88130  ldrb.w	r3, [r4, #129]
08026d9e  03f0ff01  and	r1, r3, #255
08026da2  002b      cmp	r3, #0
08026da4  61d0      beq	#194 ; -> 0x08026e6a ; branch_target=0x08026e6a
08026da6  0223      movs	r3, #2
08026da8  84f88130  strb.w	r3, [r4, #129]
08026dac  1368      ldr	r3, [r2]
08026dae  23f00103  bic	r3, r3, #1
08026db2  1360      str	r3, [r2]
08026db4  a369      ldr	r3, [r4, #24]
08026db6  b3f1806f  cmp.w	r3, #67108864
08026dba  5dd0      beq	#186 ; -> 0x08026e78 ; branch_target=0x08026e78
08026dbc  a26a      ldr	r2, [r4, #40]
08026dbe  e369      ldr	r3, [r4, #28]
08026dc0  e06b      ldr	r0, [r4, #60]
08026dc2  1343      orrs	r3, r2
08026dc4  e168      ldr	r1, [r4, #12]
08026dc6  2268      ldr	r2, [r4]
08026dc8  0343      orrs	r3, r0
08026dca  0b43      orrs	r3, r1
08026dcc  9360      str	r3, [r2, #8]
08026dce  656a      ldr	r5, [r4, #36]
08026dd0  a069      ldr	r0, [r4, #24]
08026dd2  2268      ldr	r2, [r4]
08026dd4  d4e90d31  ldrd	r3, r1, [r4, #52]
08026dd8  2b43      orrs	r3, r5
08026dda  0b43      orrs	r3, r1
08026ddc  2169      ldr	r1, [r4, #16]
08026dde  0343      orrs	r3, r0
08026de0  6069      ldr	r0, [r4, #20]
08026de2  0b43      orrs	r3, r1
08026de4  216a      ldr	r1, [r4, #32]
08026de6  0343      orrs	r3, r0
08026de8  6068      ldr	r0, [r4, #4]
08026dea  0b43      orrs	r3, r1
08026dec  e16c      ldr	r1, [r4, #76]
08026dee  0343      orrs	r3, r0
08026df0  a068      ldr	r0, [r4, #8]
08026df2  0b43      orrs	r3, r1
08026df4  a16c      ldr	r1, [r4, #72]
08026df6  0343      orrs	r3, r0
08026df8  0b43      orrs	r3, r1
08026dfa  a16d      ldr	r1, [r4, #88]
08026dfc  0b43      orrs	r3, r1
08026dfe  d360      str	r3, [r2, #12]
08026e00  6368      ldr	r3, [r4, #4]
08026e02  6bb9      cbnz	r3, #26 ; -> 0x08026e20 ; branch_target=0x08026e20
08026e04  2268      ldr	r2, [r4]
08026e06  9368      ldr	r3, [r2, #8]
08026e08  23f4c053  bic	r3, r3, #6144
08026e0c  43f40063  orr	r3, r3, #2048
08026e10  9360      str	r3, [r2, #8]
08026e12  2268      ldr	r2, [r4]
08026e14  9368      ldr	r3, [r2, #8]
08026e16  23f4c063  bic	r3, r3, #1536
08026e1a  43f48063  orr	r3, r3, #1024
08026e1e  9360      str	r3, [r2, #8]
08026e20  2268      ldr	r2, [r4]
08026e22  136d      ldr	r3, [r2, #80]
08026e24  23f00103  bic	r3, r3, #1
08026e28  1365      str	r3, [r2, #80]
08026e2a  6368      ldr	r3, [r4, #4]
08026e2c  5b02      lsls	r3, r3, #9
08026e2e  06d5      bpl	#12 ; -> 0x08026e3e ; branch_target=0x08026e3e
08026e30  2268      ldr	r2, [r4]
08026e32  616d      ldr	r1, [r4, #84]
08026e34  d368      ldr	r3, [r2, #12]
08026e36  23f00043  bic	r3, r3, #2147483648
08026e3a  0b43      orrs	r3, r1
08026e3c  d360      str	r3, [r2, #12]
08026e3e  0023      movs	r3, #0
08026e40  0122      movs	r2, #1
08026e42  c4f88430  str.w	r3, [r4, #132]
08026e46  1846      mov	r0, r3
08026e48  84f88120  strb.w	r2, [r4, #129]
08026e4c  38bd      pop	{r3, r4, r5, pc}
08026e4e  e16b      ldr	r1, [r4, #60]
08026e50  0833      adds	r3, #8
08026e52  4909      lsrs	r1, r1, #5
08026e54  db08      lsrs	r3, r3, #3
08026e56  01fb0333  mla	r3, r1, r3, r3
08026e5a  102b      cmp	r3, #16
08026e5c  94d8      bhi	#-216 ; -> 0x08026d88 ; branch_target=0x08026d88
08026e5e  94f88130  ldrb.w	r3, [r4, #129]
08026e62  03f0ff01  and	r1, r3, #255
08026e66  002b      cmp	r3, #0
08026e68  9dd1      bne	#-198 ; -> 0x08026da6 ; branch_target=0x08026da6
08026e6a  2046      mov	r0, r4
08026e6c  84f88010  strb.w	r1, [r4, #128]
08026e70  0ef040fd  bl	#60032 ; -> 0x080358f4 ; branch_target=0x080358f4
08026e74  2268      ldr	r2, [r4]
08026e76  96e7      b	#-212 ; -> 0x08026da6 ; branch_target=0x08026da6
08026e78  6368      ldr	r3, [r4, #4]
08026e7a  b3f5800f  cmp.w	r3, #4194304
08026e7e  0dd0      beq	#26 ; -> 0x08026e9c ; branch_target=0x08026e9c
08026e80  002b      cmp	r3, #0
08026e82  9bd1      bne	#-202 ; -> 0x08026dbc ; branch_target=0x08026dbc
08026e84  a36b      ldr	r3, [r4, #56]
08026e86  b3f1805f  cmp.w	r3, #268435456
08026e8a  97d1      bne	#-210 ; -> 0x08026dbc ; branch_target=0x08026dbc
08026e8c  2268      ldr	r2, [r4]
08026e8e  1368      ldr	r3, [r2]
08026e90  43f48053  orr	r3, r3, #4096
08026e94  1360      str	r3, [r2]
08026e96  91e7      b	#-222 ; -> 0x08026dbc ; branch_target=0x08026dbc
08026e9c  a36b      ldr	r3, [r4, #56]
08026e9e  002b      cmp	r3, #0
08026ea0  f4d0      beq	#-24 ; -> 0x08026e8c ; branch_target=0x08026e8c
08026ea2  8be7      b	#-234 ; -> 0x08026dbc ; branch_target=0x08026dbc
