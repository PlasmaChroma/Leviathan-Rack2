; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802d330  f0b5      push	{r4, r5, r6, r7, lr}
0802d332  91b0      sub	sp, #68
0802d334  0724      movs	r4, #7
0802d336  f5f769fd  bl	#-42286 ; -> 0x08022e0c ; branch_target=0x08022e0c
0802d33a  0020      movs	r0, #0
0802d33c  0121      movs	r1, #1
0802d33e  2023      movs	r3, #32
0802d340  0125      movs	r5, #1
0802d342  4af6aa26  movw	r6, #43690
0802d346  0693      str	r3, [sp, #24]
0802d348  4af6aa27  movw	r7, #43690
0802d34c  cde90201  strd	r0, r1, [sp, #8]
0802d350  01a9      add	r1, sp, #4
0802d352  02a8      add	r0, sp, #8
0802d354  cde90445  strd	r4, r5, [sp, #16]
0802d358  f5f750fe  bl	#-41824 ; -> 0x08022ffc ; branch_target=0x08022ffc
0802d35c  40f2d540  movw	r0, #1237
0802d360  40f2d541  movw	r1, #1237
0802d364  2c4d      ldr	r5, [pc, #176] ; [0x0802d418] = 0x20001380
0802d366  08aa      add	r2, sp, #32
0802d368  2c4c      ldr	r4, [pc, #176] ; [0x0802d41c] = 0x20001200
0802d36a  cde90801  strd	r0, r1, [sp, #32]
0802d36e  cde90a01  strd	r0, r1, [sp, #40]
0802d372  cde90c01  strd	r0, r1, [sp, #48]
0802d376  2a49      ldr	r1, [pc, #168] ; [0x0802d420] = 0x080e0000
0802d378  0120      movs	r0, #1
0802d37a  cde90e67  strd	r6, r7, [sp, #56]
0802d37e  f5f7e1fc  bl	#-42558 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d382  2a46      mov	r2, r5
0802d384  2749      ldr	r1, [pc, #156] ; [0x0802d424] = 0x080e0020
0802d386  0120      movs	r0, #1
0802d388  f5f7dcfc  bl	#-42568 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d38c  05f12002  add.w	r2, r5, #32
0802d390  2549      ldr	r1, [pc, #148] ; [0x0802d428] = 0x080e0040
0802d392  0120      movs	r0, #1
0802d394  f5f7d6fc  bl	#-42580 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d398  244f      ldr	r7, [pc, #144] ; [0x0802d42c] = 0x20001300
0802d39a  2246      mov	r2, r4
0802d39c  2449      ldr	r1, [pc, #144] ; [0x0802d430] = 0x080e0060
0802d39e  0120      movs	r0, #1
0802d3a0  244e      ldr	r6, [pc, #144] ; [0x0802d434] = 0x200012c0
0802d3a2  f5f7cffc  bl	#-42594 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3a6  04f12002  add.w	r2, r4, #32
0802d3aa  2349      ldr	r1, [pc, #140] ; [0x0802d438] = 0x080e0080
0802d3ac  0120      movs	r0, #1
0802d3ae  f5f7c9fc  bl	#-42606 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3b2  224d      ldr	r5, [pc, #136] ; [0x0802d43c] = 0x20001280
0802d3b4  3a46      mov	r2, r7
0802d3b6  2249      ldr	r1, [pc, #136] ; [0x0802d440] = 0x080e00a0
0802d3b8  0120      movs	r0, #1
0802d3ba  224c      ldr	r4, [pc, #136] ; [0x0802d444] = 0x20001240
0802d3bc  f5f7c2fc  bl	#-42620 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3c0  3246      mov	r2, r6
0802d3c2  2149      ldr	r1, [pc, #132] ; [0x0802d448] = 0x080e00c0
0802d3c4  0120      movs	r0, #1
0802d3c6  f5f7bdfc  bl	#-42630 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3ca  2a46      mov	r2, r5
0802d3cc  1f49      ldr	r1, [pc, #124] ; [0x0802d44c] = 0x080e00e0
0802d3ce  0120      movs	r0, #1
0802d3d0  f5f7b8fc  bl	#-42640 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3d4  2246      mov	r2, r4
0802d3d6  1e49      ldr	r1, [pc, #120] ; [0x0802d450] = 0x080e0100
0802d3d8  0120      movs	r0, #1
0802d3da  f5f7b3fc  bl	#-42650 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3de  07f12002  add.w	r2, r7, #32
0802d3e2  1c49      ldr	r1, [pc, #112] ; [0x0802d454] = 0x080e0120
0802d3e4  0120      movs	r0, #1
0802d3e6  f5f7adfc  bl	#-42662 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3ea  06f12002  add.w	r2, r6, #32
0802d3ee  1a49      ldr	r1, [pc, #104] ; [0x0802d458] = 0x080e0140
0802d3f0  0120      movs	r0, #1
0802d3f2  f5f7a7fc  bl	#-42674 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d3f6  05f12002  add.w	r2, r5, #32
0802d3fa  1849      ldr	r1, [pc, #96] ; [0x0802d45c] = 0x080e0160
0802d3fc  0120      movs	r0, #1
0802d3fe  f5f7a1fc  bl	#-42686 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d402  04f12002  add.w	r2, r4, #32
0802d406  1649      ldr	r1, [pc, #88] ; [0x0802d460] = 0x080e0180
0802d408  0120      movs	r0, #1
0802d40a  f5f79bfc  bl	#-42698 ; -> 0x08022d44 ; branch_target=0x08022d44
0802d40e  f5f723fd  bl	#-42426 ; -> 0x08022e58 ; branch_target=0x08022e58
0802d412  11b0      add	sp, #68
0802d414  f0bd      pop	{r4, r5, r6, r7, pc}
