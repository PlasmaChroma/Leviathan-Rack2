; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802c32c  70b5      push	{r4, r5, r6, lr}
0802c32e  6149      ldr	r1, [pc, #388] ; [0x0802c4b4] = 0x40022000 / f32_bits_interpretation=2.033203125
0802c330  8cb0      sub	sp, #48
0802c332  0268      ldr	r2, [r0]
0802c334  0023      movs	r3, #0
0802c336  0546      mov	r5, r0
0802c338  8a42      cmp	r2, r1
0802c33a  0893      str	r3, [sp, #32]
0802c33c  cde90633  strd	r3, r3, [sp, #24]
0802c340  cde90933  strd	r3, r3, [sp, #36]
0802c344  62d0      beq	#196 ; -> 0x0802c40c ; branch_target=0x0802c40c
0802c346  5c4b      ldr	r3, [pc, #368] ; [0x0802c4b8] = 0x40022100 / f32_bits_interpretation=2.03326416
0802c348  9a42      cmp	r2, r3
0802c34a  01d0      beq	#2 ; -> 0x0802c350 ; branch_target=0x0802c350
0802c34c  0cb0      add	sp, #48
0802c34e  70bd      pop	{r4, r5, r6, pc}
0802c350  5a4a      ldr	r2, [pc, #360] ; [0x0802c4bc] = 0x200020a0
0802c352  1368      ldr	r3, [r2]
0802c354  0133      adds	r3, #1
0802c356  012b      cmp	r3, #1
0802c358  1360      str	r3, [r2]
0802c35a  00f09980  beq.w	#306 ; -> 0x0802c490 ; branch_target=0x0802c490
0802c35e  584b      ldr	r3, [pc, #352] ; [0x0802c4c0] = 0x58024400
0802c360  06a9      add	r1, sp, #24
0802c362  0026      movs	r6, #0
0802c364  574c      ldr	r4, [pc, #348] ; [0x0802c4c4] = 0x200020a4
0802c366  d3f8e020  ldr.w	r2, [r3, #224]
0802c36a  42f00102  orr	r2, r2, #1
0802c36e  c3f8e020  str.w	r2, [r3, #224]
0802c372  d3f8e020  ldr.w	r2, [r3, #224]
0802c376  02f00102  and	r2, r2, #1
0802c37a  0492      str	r2, [sp, #16]
0802c37c  049a      ldr	r2, [sp, #16]
0802c37e  d3f8e020  ldr.w	r2, [r3, #224]
0802c382  42f00402  orr	r2, r2, #4
0802c386  c3f8e020  str.w	r2, [r3, #224]
0802c38a  cc22      movs	r2, #204
0802c38c  d3f8e030  ldr.w	r3, [r3, #224]
0802c390  03f00403  and	r3, r3, #4
0802c394  0593      str	r3, [sp, #20]
0802c396  0323      movs	r3, #3
0802c398  0598      ldr	r0, [sp, #20]
0802c39a  4b48      ldr	r0, [pc, #300] ; [0x0802c4c8] = 0x58020000
0802c39c  cde90623  strd	r2, r3, [sp, #24]
0802c3a0  f6f7fafe  bl	#-37388 ; -> 0x08023198 ; branch_target=0x08023198
0802c3a4  3022      movs	r2, #48
0802c3a6  0323      movs	r3, #3
0802c3a8  4848      ldr	r0, [pc, #288] ; [0x0802c4cc] = 0x58020800
0802c3aa  06a9      add	r1, sp, #24
0802c3ac  0896      str	r6, [sp, #32]
0802c3ae  cde90623  strd	r2, r3, [sp, #24]
0802c3b2  f6f7f1fe  bl	#-37406 ; -> 0x08023198 ; branch_target=0x08023198
0802c3b6  464a      ldr	r2, [pc, #280] ; [0x0802c4d0] = 0x400200b8 / f32_bits_interpretation=2.031293869
0802c3b8  0a23      movs	r3, #10
0802c3ba  2046      mov	r0, r4
0802c3bc  2260      str	r2, [r4]
0802c3be  4ff48062  mov.w	r2, #1024
0802c3c2  6360      str	r3, [r4, #4]
0802c3c4  4ff40063  mov.w	r3, #2048
0802c3c8  2261      str	r2, [r4, #16]
0802c3ca  4ff40052  mov.w	r2, #8192
0802c3ce  6361      str	r3, [r4, #20]
0802c3d0  4ff48073  mov.w	r3, #256
0802c3d4  a660      str	r6, [r4, #8]
0802c3d6  e660      str	r6, [r4, #12]
0802c3d8  a662      str	r6, [r4, #40]
0802c3da  e662      str	r6, [r4, #44]
0802c3dc  2663      str	r6, [r4, #48]
0802c3de  c4e90623  strd	r2, r3, [r4, #24]
0802c3e2  4ff40032  mov.w	r2, #131072
0802c3e6  0423      movs	r3, #4
0802c3e8  c4e90823  strd	r2, r3, [r4, #32]
0802c3ec  f5f70efa  bl	#-44004 ; -> 0x0802180c ; branch_target=0x0802180c
0802c3f0  0028      cmp	r0, #0
0802c3f2  5bd1      bne	#182 ; -> 0x0802c4ac ; branch_target=0x0802c4ac
0802c3f4  0022      movs	r2, #0
0802c3f6  ec64      str	r4, [r5, #76]
0802c3f8  1220      movs	r0, #18
0802c3fa  a563      str	r5, [r4, #56]
0802c3fc  1146      mov	r1, r2
0802c3fe  f5f76ff8  bl	#-44834 ; -> 0x080214e0 ; branch_target=0x080214e0
0802c402  1220      movs	r0, #18
0802c404  f5f7a8f8  bl	#-44720 ; -> 0x08021558 ; branch_target=0x08021558
0802c408  0cb0      add	sp, #48
0802c40a  70bd      pop	{r4, r5, r6, pc}
0802c40c  2b4a      ldr	r2, [pc, #172] ; [0x0802c4bc] = 0x200020a0
0802c40e  1368      ldr	r3, [r2]
0802c410  0133      adds	r3, #1
0802c412  012b      cmp	r3, #1
0802c414  1360      str	r3, [r2]
0802c416  2dd0      beq	#90 ; -> 0x0802c474 ; branch_target=0x0802c474
0802c418  294b      ldr	r3, [pc, #164] ; [0x0802c4c0] = 0x58024400
0802c41a  06a9      add	r1, sp, #24
0802c41c  2a48      ldr	r0, [pc, #168] ; [0x0802c4c8] = 0x58020000
0802c41e  d3f8e020  ldr.w	r2, [r3, #224]
0802c422  2c4c      ldr	r4, [pc, #176] ; [0x0802c4d4] = 0x2000211c
0802c424  42f00102  orr	r2, r2, #1
0802c428  c3f8e020  str.w	r2, [r3, #224]
0802c42c  3322      movs	r2, #51
0802c42e  d3f8e030  ldr.w	r3, [r3, #224]
0802c432  03f00103  and	r3, r3, #1
0802c436  0293      str	r3, [sp, #8]
0802c438  0323      movs	r3, #3
0802c43a  029e      ldr	r6, [sp, #8]
0802c43c  cde90623  strd	r2, r3, [sp, #24]
0802c440  f6f7aafe  bl	#-37548 ; -> 0x08023198 ; branch_target=0x08023198
0802c444  2449      ldr	r1, [pc, #144] ; [0x0802c4d8] = 0x40020010 / f32_bits_interpretation=2.031253815
0802c446  0922      movs	r2, #9
0802c448  0023      movs	r3, #0
0802c44a  2046      mov	r0, r4
0802c44c  2363      str	r3, [r4, #48]
0802c44e  c4e90012  strd	r1, r2, [r4]
0802c452  4ff48061  mov.w	r1, #1024
0802c456  4ff40062  mov.w	r2, #2048
0802c45a  c4e90412  strd	r1, r2, [r4, #16]
0802c45e  4ff40051  mov.w	r1, #8192
0802c462  4ff48072  mov.w	r2, #256
0802c466  c4e90233  strd	r3, r3, [r4, #8]
0802c46a  c4e90a33  strd	r3, r3, [r4, #40]
0802c46e  c4e90612  strd	r1, r2, [r4, #24]
0802c472  b6e7      b	#-148 ; -> 0x0802c3e2 ; branch_target=0x0802c3e2
0802c474  124b      ldr	r3, [pc, #72] ; [0x0802c4c0] = 0x58024400
0802c476  d3f8d820  ldr.w	r2, [r3, #216]
0802c47a  42f02002  orr	r2, r2, #32
0802c47e  c3f8d820  str.w	r2, [r3, #216]
0802c482  d3f8d830  ldr.w	r3, [r3, #216]
0802c486  03f02003  and	r3, r3, #32
0802c48a  0193      str	r3, [sp, #4]
0802c48c  019b      ldr	r3, [sp, #4]
0802c48e  c3e7      b	#-122 ; -> 0x0802c418 ; branch_target=0x0802c418
0802c490  0b4b      ldr	r3, [pc, #44] ; [0x0802c4c0] = 0x58024400
0802c492  d3f8d820  ldr.w	r2, [r3, #216]
0802c496  42f02002  orr	r2, r2, #32
0802c49a  c3f8d820  str.w	r2, [r3, #216]
0802c49e  d3f8d830  ldr.w	r3, [r3, #216]
0802c4a2  03f02003  and	r3, r3, #32
0802c4a6  0393      str	r3, [sp, #12]
0802c4a8  039b      ldr	r3, [sp, #12]
0802c4aa  58e7      b	#-336 ; -> 0x0802c35e ; branch_target=0x0802c35e
0802c4ac  08f028fe  bl	#35920 ; -> 0x08035100 ; branch_target=0x08035100
0802c4b0  a0e7      b	#-192 ; -> 0x0802c3f4 ; branch_target=0x0802c3f4
