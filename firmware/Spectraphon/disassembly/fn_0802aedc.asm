; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802aedc  10b5      push	{r4, lr}
0802aede  82b0      sub	sp, #8
0802aee0  cde90010  strd	r1, r0, [sp]
0802aee4  a1b1      cbz	r1, #40 ; -> 0x0802af10 ; branch_target=0x0802af10
0802aee6  91f800c0  ldrb.w	r12, [r1]
0802aeea  bcf1200f  cmp.w	r12, #32
0802aeee  12d9      bls	#36 ; -> 0x0802af16 ; branch_target=0x0802af16
0802aef0  bcf13a0f  cmp.w	r12, #58
0802aef4  0fd0      beq	#30 ; -> 0x0802af16 ; branch_target=0x0802af16
0802aef6  0846      mov	r0, r1
0802aef8  10f8013f  ldrb	r3, [r0, #1]!
0802aefc  202b      cmp	r3, #32
0802aefe  0cd9      bls	#24 ; -> 0x0802af1a ; branch_target=0x0802af1a
0802af00  3a2b      cmp	r3, #58
0802af02  f9d1      bne	#-14 ; -> 0x0802aef8 ; branch_target=0x0802aef8
0802af04  0131      adds	r1, #1
0802af06  8842      cmp	r0, r1
0802af08  02d1      bne	#4 ; -> 0x0802af10 ; branch_target=0x0802af10
0802af0a  bcf1300f  cmp.w	r12, #48
0802af0e  06d0      beq	#12 ; -> 0x0802af1e ; branch_target=0x0802af1e
0802af10  0b20      movs	r0, #11
0802af12  02b0      add	sp, #8
0802af14  10bd      pop	{r4, pc}
0802af16  6346      mov	r3, r12
0802af18  0846      mov	r0, r1
0802af1a  3a2b      cmp	r3, #58
0802af1c  f2d0      beq	#-28 ; -> 0x0802af04 ; branch_target=0x0802af04
0802af1e  1349      ldr	r1, [pc, #76] ; [0x0802af6c] = 0x20002090
0802af20  0b68      ldr	r3, [r1]
0802af22  43b1      cbz	r3, #16 ; -> 0x0802af36 ; branch_target=0x0802af36
0802af24  1248      ldr	r0, [pc, #72] ; [0x0802af70] = 0x2000206c
0802af26  0468      ldr	r4, [r0]
0802af28  a342      cmp	r3, r4
0802af2a  19d0      beq	#50 ; -> 0x0802af60 ; branch_target=0x0802af60
0802af2c  0469      ldr	r4, [r0, #16]
0802af2e  9c42      cmp	r4, r3
0802af30  13d0      beq	#38 ; -> 0x0802af5a ; branch_target=0x0802af5a
0802af32  0020      movs	r0, #0
0802af34  1870      strb	r0, [r3]
0802af36  019b      ldr	r3, [sp, #4]
0802af38  5bb1      cbz	r3, #22 ; -> 0x0802af52 ; branch_target=0x0802af52
0802af3a  0020      movs	r0, #0
0802af3c  012a      cmp	r2, #1
0802af3e  0b60      str	r3, [r1]
0802af40  1870      strb	r0, [r3]
0802af42  07d1      bne	#14 ; -> 0x0802af54 ; branch_target=0x0802af54
0802af44  0246      mov	r2, r0
0802af46  01a9      add	r1, sp, #4
0802af48  6846      mov	r0, sp
0802af4a  fef775ff  bl	#-4374 ; -> 0x08029e38 ; branch_target=0x08029e38
0802af4e  02b0      add	sp, #8
0802af50  10bd      pop	{r4, pc}
0802af52  0b60      str	r3, [r1]
0802af54  0020      movs	r0, #0
0802af56  02b0      add	sp, #8
0802af58  10bd      pop	{r4, pc}
0802af5a  0024      movs	r4, #0
0802af5c  0461      str	r4, [r0, #16]
0802af5e  e8e7      b	#-48 ; -> 0x0802af32 ; branch_target=0x0802af32
0802af60  0024      movs	r4, #0
0802af62  0460      str	r4, [r0]
0802af64  0469      ldr	r4, [r0, #16]
0802af66  9c42      cmp	r4, r3
0802af68  e3d1      bne	#-58 ; -> 0x0802af32 ; branch_target=0x0802af32
0802af6a  f6e7      b	#-20 ; -> 0x0802af5a ; branch_target=0x0802af5a
