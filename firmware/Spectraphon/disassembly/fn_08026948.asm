; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026948  30b5      push	{r4, r5, lr}
0802694a  0322      movs	r2, #3
0802694c  836b      ldr	r3, [r0, #56]
0802694e  89b0      sub	sp, #36
08026950  0446      mov	r4, r0
08026952  9342      cmp	r3, r2
08026954  0d46      mov	r5, r1
08026956  80f83020  strb.w	r2, [r0, #48]
0802695a  51d0      beq	#162 ; -> 0x08026a00 ; branch_target=0x08026a00
0802695c  b1f5004f  cmp.w	r1, #32768
08026960  4ed0      beq	#156 ; -> 0x08026a00 ; branch_target=0x08026a00
08026962  b1f5804f  cmp.w	r1, #16384
08026966  7bd0      beq	#246 ; -> 0x08026a60 ; branch_target=0x08026a60
08026968  0029      cmp	r1, #0
0802696a  5ad0      beq	#180 ; -> 0x08026a22 ; branch_target=0x08026a22
0802696c  436b      ldr	r3, [r0, #52]
0802696e  43f00063  orr	r3, r3, #134217728
08026972  4363      str	r3, [r0, #52]
08026974  636b      ldr	r3, [r4, #52]
08026976  002b      cmp	r3, #0
08026978  49d1      bne	#146 ; -> 0x08026a0e ; branch_target=0x08026a0e
0802697a  4ff48030  mov.w	r0, #65536
0802697e  fef7d1fc  bl	#-5726 ; -> 0x08025324 ; branch_target=0x08025324
08026982  0028      cmp	r0, #0
08026984  48d0      beq	#144 ; -> 0x08026a18 ; branch_target=0x08026a18
08026986  2269      ldr	r2, [r4, #16]
08026988  4e4b      ldr	r3, [pc, #312] ; [0x08026ac4] = 0x55e63b89
0802698a  6168      ldr	r1, [r4, #4]
0802698c  0495      str	r5, [sp, #16]
0802698e  0592      str	r2, [sp, #20]
08026990  a3fb0053  umull	r5, r3, r3, r0
08026994  6269      ldr	r2, [r4, #20]
08026996  0291      str	r1, [sp, #8]
08026998  b2eb136f  cmp.w	r2, r3, lsr #24
0802699c  a168      ldr	r1, [r4, #8]
0802699e  4fea1365  lsr.w	r5, r3, #24
080269a2  0391      str	r1, [sp, #12]
080269a4  0ed2      bhs	#28 ; -> 0x080269c4 ; branch_target=0x080269c4
080269a6  a16d      ldr	r1, [r4, #88]
080269a8  b1f5007f  cmp.w	r1, #512
080269ac  0ad0      beq	#20 ; -> 0x080269c4 ; branch_target=0x080269c4
080269ae  b1f5807f  cmp.w	r1, #256
080269b2  77d0      beq	#238 ; -> 0x08026aa4 ; branch_target=0x08026aa4
080269b4  12b1      cbz	r2, #4 ; -> 0x080269bc ; branch_target=0x080269bc
080269b6  5300      lsls	r3, r2, #1
080269b8  b0fbf3f0  udiv	r0, r0, r3
080269bc  424b      ldr	r3, [pc, #264] ; [0x08026ac8] = 0x017d7840
080269be  9842      cmp	r0, r3
080269c0  88bf      it	hi
080269c2  2a46      movhi	r2, r5
080269c4  05a9      add	r1, sp, #20
080269c6  0692      str	r2, [sp, #24]
080269c8  02ab      add	r3, sp, #8
080269ca  0025      movs	r5, #0
080269cc  03c9      ldm	r1, {r0, r1}
080269ce  8de80300  stm.w	sp, {r0, r1}
080269d2  0ecb      ldm	r3, {r1, r2, r3}
080269d4  2068      ldr	r0, [r4]
080269d6  01f045fd  bl	#6794 ; -> 0x08028464 ; branch_target=0x08028464
080269da  4ff40071  mov.w	r1, #512
080269de  2068      ldr	r0, [r4]
080269e0  01f08cfd  bl	#6936 ; -> 0x080284fc ; branch_target=0x080284fc
080269e4  30b1      cbz	r0, #12 ; -> 0x080269f4 ; branch_target=0x080269f4
080269e6  2368      ldr	r3, [r4]
080269e8  0125      movs	r5, #1
080269ea  384a      ldr	r2, [pc, #224] ; [0x08026acc] = 0x1fe00fff
080269ec  9a63      str	r2, [r3, #56]
080269ee  636b      ldr	r3, [r4, #52]
080269f0  0343      orrs	r3, r0
080269f2  6363      str	r3, [r4, #52]
080269f4  0123      movs	r3, #1
080269f6  2846      mov	r0, r5
080269f8  84f83030  strb.w	r3, [r4, #48]
080269fc  09b0      add	sp, #36
080269fe  30bd      pop	{r4, r5, pc}
08026a00  636b      ldr	r3, [r4, #52]
08026a02  43f08053  orr	r3, r3, #268435456
08026a06  6363      str	r3, [r4, #52]
08026a08  636b      ldr	r3, [r4, #52]
08026a0a  002b      cmp	r3, #0
08026a0c  b5d0      beq	#-150 ; -> 0x0802697a ; branch_target=0x0802697a
08026a0e  2368      ldr	r3, [r4]
08026a10  2e4a      ldr	r2, [pc, #184] ; [0x08026acc] = 0x1fe00fff
08026a12  9a63      str	r2, [r3, #56]
08026a14  0125      movs	r5, #1
08026a16  e0e7      b	#-64 ; -> 0x080269da ; branch_target=0x080269da
08026a18  636b      ldr	r3, [r4, #52]
08026a1a  43f00063  orr	r3, r3, #134217728
08026a1e  6363      str	r3, [r4, #52]
08026a20  f8e7      b	#-16 ; -> 0x08026a14 ; branch_target=0x08026a14
08026a22  0022      movs	r2, #0
08026a24  0023      movs	r3, #0
08026a26  0068      ldr	r0, [r0]
08026a28  cde90223  strd	r2, r3, [sp, #8]
08026a2c  01f04cfd  bl	#6808 ; -> 0x080284c8 ; branch_target=0x080284c8
08026a30  8201      lsls	r2, r0, #6
08026a32  34d4      bmi	#104 ; -> 0x08026a9e ; branch_target=0x08026a9e
08026a34  02a9      add	r1, sp, #8
08026a36  2046      mov	r0, r4
08026a38  fff764fa  bl	#-2872 ; -> 0x08025f04 ; branch_target=0x08025f04
08026a3c  60b9      cbnz	r0, #24 ; -> 0x08026a58 ; branch_target=0x08026a58
08026a3e  039b      ldr	r3, [sp, #12]
08026a40  db03      lsls	r3, r3, #15
08026a42  35d5      bpl	#106 ; -> 0x08026ab0 ; branch_target=0x08026ab0
08026a44  616c      ldr	r1, [r4, #68]
08026a46  2068      ldr	r0, [r4]
08026a48  0904      lsls	r1, r1, #16
08026a4a  02f0cffa  bl	#9630 ; -> 0x08028fec ; branch_target=0x08028fec
08026a4e  18b9      cbnz	r0, #6 ; -> 0x08026a58 ; branch_target=0x08026a58
08026a50  0146      mov	r1, r0
08026a52  2068      ldr	r0, [r4]
08026a54  02f0b2fb  bl	#10084 ; -> 0x080291bc ; branch_target=0x080291bc
08026a58  636b      ldr	r3, [r4, #52]
08026a5a  0343      orrs	r3, r0
08026a5c  6363      str	r3, [r4, #52]
08026a5e  d3e7      b	#-90 ; -> 0x08026a08 ; branch_target=0x08026a08
08026a60  0022      movs	r2, #0
08026a62  0023      movs	r3, #0
08026a64  0021      movs	r1, #0
08026a66  0068      ldr	r0, [r0]
08026a68  cde90223  strd	r2, r3, [sp, #8]
08026a6c  01f02cfd  bl	#6744 ; -> 0x080284c8 ; branch_target=0x080284c8
08026a70  8001      lsls	r0, r0, #6
08026a72  14d4      bmi	#40 ; -> 0x08026a9e ; branch_target=0x08026a9e
08026a74  02a9      add	r1, sp, #8
08026a76  2046      mov	r0, r4
08026a78  fff744fa  bl	#-2936 ; -> 0x08025f04 ; branch_target=0x08025f04
08026a7c  0028      cmp	r0, #0
08026a7e  ebd1      bne	#-42 ; -> 0x08026a58 ; branch_target=0x08026a58
08026a80  039b      ldr	r3, [sp, #12]
08026a82  5903      lsls	r1, r3, #13
08026a84  14d5      bpl	#40 ; -> 0x08026ab0 ; branch_target=0x08026ab0
08026a86  616c      ldr	r1, [r4, #68]
08026a88  2068      ldr	r0, [r4]
08026a8a  0904      lsls	r1, r1, #16
08026a8c  02f0aefa  bl	#9564 ; -> 0x08028fec ; branch_target=0x08028fec
08026a90  0028      cmp	r0, #0
08026a92  e1d1      bne	#-62 ; -> 0x08026a58 ; branch_target=0x08026a58
08026a94  0221      movs	r1, #2
08026a96  2068      ldr	r0, [r4]
08026a98  02f090fb  bl	#10016 ; -> 0x080291bc ; branch_target=0x080291bc
08026a9c  dce7      b	#-72 ; -> 0x08026a58 ; branch_target=0x08026a58
08026a9e  4ff40060  mov.w	r0, #2048
08026aa2  d9e7      b	#-78 ; -> 0x08026a58 ; branch_target=0x08026a58
08026aa4  3ab9      cbnz	r2, #14 ; -> 0x08026ab6 ; branch_target=0x08026ab6
08026aa6  0a49      ldr	r1, [pc, #40] ; [0x08026ad0] = 0x02faf080
08026aa8  8842      cmp	r0, r1
08026aaa  8bd9      bls	#-234 ; -> 0x080269c4 ; branch_target=0x080269c4
08026aac  5a0e      lsrs	r2, r3, #25
08026aae  89e7      b	#-238 ; -> 0x080269c4 ; branch_target=0x080269c4
08026ab0  4ff08060  mov.w	r0, #67108864
08026ab4  d0e7      b	#-96 ; -> 0x08026a58 ; branch_target=0x08026a58
08026ab6  5100      lsls	r1, r2, #1
08026ab8  b0fbf1f1  udiv	r1, r0, r1
08026abc  0448      ldr	r0, [pc, #16] ; [0x08026ad0] = 0x02faf080
08026abe  8142      cmp	r1, r0
08026ac0  80d9      bls	#-256 ; -> 0x080269c4 ; branch_target=0x080269c4
08026ac2  f3e7      b	#-26 ; -> 0x08026aac ; branch_target=0x08026aac
