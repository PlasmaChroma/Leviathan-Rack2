; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08022ffc  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
08023000  624e      ldr	r6, [pc, #392] ; [0x0802318c] = 0x20002050
08023002  82b0      sub	sp, #8
08023004  337d      ldrb	r3, [r6, #20]
08023006  012b      cmp	r3, #1
08023008  00f0ac80  beq.w	#344 ; -> 0x08023164 ; branch_target=0x08023164
0802300c  0023      movs	r3, #0
0802300e  0f46      mov	r7, r1
08023010  0121      movs	r1, #1
08023012  0446      mov	r4, r0
08023014  b361      str	r3, [r6, #24]
08023016  3175      strb	r1, [r6, #20]
08023018  4368      ldr	r3, [r0, #4]
0802301a  dd07      lsls	r5, r3, #31
0802301c  77d4      bmi	#238 ; -> 0x0802310e ; branch_target=0x0802310e
0802301e  9807      lsls	r0, r3, #30
08023020  52d4      bmi	#164 ; -> 0x080230c8 ; branch_target=0x080230c8
08023022  2568      ldr	r5, [r4]
08023024  012d      cmp	r5, #1
08023026  00f08180  beq.w	#258 ; -> 0x0802312c ; branch_target=0x0802312c
0802302a  4ff0ff33  mov.w	r3, #4294967295
0802302e  3b60      str	r3, [r7]
08023030  a068      ldr	r0, [r4, #8]
08023032  d4f80cc0  ldr.w	r12, [r4, #12]
08023036  8146      mov	r9, r0
08023038  00eb0c03  add.w	r3, r0, r12
0802303c  9842      cmp	r0, r3
0802303e  41d2      bhs	#130 ; -> 0x080230c4 ; branch_target=0x080230c4
08023040  4fea002a  lsl.w	r10, r0, #8
08023044  524d      ldr	r5, [pc, #328] ; [0x08023190] = 0x52002000
08023046  dff84c81  ldr.w	r8, [pc, #332] ; [0x08023194] = 0xfffff8fb
0802304a  6368      ldr	r3, [r4, #4]
0802304c  2269      ldr	r2, [r4, #16]
0802304e  03f00201  and	r1, r3, #2
08023052  db07      lsls	r3, r3, #31
08023054  46d5      bpl	#140 ; -> 0x080230e4 ; branch_target=0x080230e4
08023056  eb68      ldr	r3, [r5, #12]
08023058  23f4e663  bic	r3, r3, #1840
0802305c  eb60      str	r3, [r5, #12]
0802305e  eb68      ldr	r3, [r5, #12]
08023060  43ea0a03  orr.w	r3, r3, r10
08023064  1343      orrs	r3, r2
08023066  43f08403  orr	r3, r3, #132
0802306a  eb60      str	r3, [r5, #12]
0802306c  71b1      cbz	r1, #28 ; -> 0x0802308c ; branch_target=0x0802308c
0802306e  d5f80c31  ldr.w	r3, [r5, #268]
08023072  23f4e663  bic	r3, r3, #1840
08023076  c5f80c31  str.w	r3, [r5, #268]
0802307a  d5f80c31  ldr.w	r3, [r5, #268]
0802307e  43ea0a03  orr.w	r3, r3, r10
08023082  1343      orrs	r3, r2
08023084  43f08403  orr	r3, r3, #132
08023088  c5f80c31  str.w	r3, [r5, #268]
0802308c  6368      ldr	r3, [r4, #4]
0802308e  d807      lsls	r0, r3, #31
08023090  2bd4      bmi	#86 ; -> 0x080230ea ; branch_target=0x080230ea
08023092  9907      lsls	r1, r3, #30
08023094  0cd5      bpl	#24 ; -> 0x080230b0 ; branch_target=0x080230b0
08023096  0221      movs	r1, #2
08023098  4cf25030  movw	r0, #50000
0802309c  fff7fafe  bl	#-524 ; -> 0x08022e94 ; branch_target=0x08022e94
080230a0  d5f80c21  ldr.w	r2, [r5, #268]
080230a4  0346      mov	r3, r0
080230a6  02ea0802  and.w	r2, r2, r8
080230aa  c5f80c21  str.w	r2, [r5, #268]
080230ae  5bbb      cbnz	r3, #86 ; -> 0x08023108 ; branch_target=0x08023108
080230b0  d4e9020c  ldrd	r0, r12, [r4, #8]
080230b4  09f10109  add.w	r9, r9, #1
080230b8  0ceb0003  add.w	r3, r12, r0
080230bc  0af5807a  add.w	r10, r10, #256
080230c0  4b45      cmp	r3, r9
080230c2  c2d8      bhi	#-124 ; -> 0x0802304a ; branch_target=0x0802304a
080230c4  0023      movs	r3, #0
080230c6  07e0      b	#14 ; -> 0x080230d8 ; branch_target=0x080230d8
080230c8  0221      movs	r1, #2
080230ca  4cf25030  movw	r0, #50000
080230ce  fff7e1fe  bl	#-574 ; -> 0x08022e94 ; branch_target=0x08022e94
080230d2  0028      cmp	r0, #0
080230d4  a5d0      beq	#-182 ; -> 0x08023022 ; branch_target=0x08023022
080230d6  0123      movs	r3, #1
080230d8  0022      movs	r2, #0
080230da  1846      mov	r0, r3
080230dc  3275      strb	r2, [r6, #20]
080230de  02b0      add	sp, #8
080230e0  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
080230e4  0029      cmp	r1, #0
080230e6  c2d1      bne	#-124 ; -> 0x0802306e ; branch_target=0x0802306e
080230e8  e4e7      b	#-56 ; -> 0x080230b4 ; branch_target=0x080230b4
080230ea  0121      movs	r1, #1
080230ec  4cf25030  movw	r0, #50000
080230f0  fff7d0fe  bl	#-608 ; -> 0x08022e94 ; branch_target=0x08022e94
080230f4  ea68      ldr	r2, [r5, #12]
080230f6  0346      mov	r3, r0
080230f8  02ea0802  and.w	r2, r2, r8
080230fc  ea60      str	r2, [r5, #12]
080230fe  6268      ldr	r2, [r4, #4]
08023100  9207      lsls	r2, r2, #30
08023102  c8d4      bmi	#-112 ; -> 0x08023096 ; branch_target=0x08023096
08023104  002b      cmp	r3, #0
08023106  d3d0      beq	#-90 ; -> 0x080230b0 ; branch_target=0x080230b0
08023108  c7f80090  str.w	r9, [r7]
0802310c  e4e7      b	#-56 ; -> 0x080230d8 ; branch_target=0x080230d8
0802310e  4cf25030  movw	r0, #50000
08023112  fff7bffe  bl	#-642 ; -> 0x08022e94 ; branch_target=0x08022e94
08023116  6368      ldr	r3, [r4, #4]
08023118  0028      cmp	r0, #0
0802311a  80d0      beq	#-256 ; -> 0x0802301e ; branch_target=0x0802301e
0802311c  9907      lsls	r1, r3, #30
0802311e  dad5      bpl	#-76 ; -> 0x080230d6 ; branch_target=0x080230d6
08023120  0221      movs	r1, #2
08023122  4cf25030  movw	r0, #50000
08023126  fff7b5fe  bl	#-662 ; -> 0x08022e94 ; branch_target=0x08022e94
0802312a  d4e7      b	#-88 ; -> 0x080230d6 ; branch_target=0x080230d6
0802312c  6168      ldr	r1, [r4, #4]
0802312e  2069      ldr	r0, [r4, #16]
08023130  fff722ff  bl	#-444 ; -> 0x08022f78 ; branch_target=0x08022f78
08023134  6268      ldr	r2, [r4, #4]
08023136  12f00103  ands	r3, r2, #1
0802313a  18d1      bne	#48 ; -> 0x0802316e ; branch_target=0x0802316e
0802313c  9207      lsls	r2, r2, #30
0802313e  cbd5      bpl	#-106 ; -> 0x080230d8 ; branch_target=0x080230d8
08023140  0221      movs	r1, #2
08023142  4cf25030  movw	r0, #50000
08023146  0193      str	r3, [sp, #4]
08023148  fff7a4fe  bl	#-696 ; -> 0x08022e94 ; branch_target=0x08022e94
0802314c  1049      ldr	r1, [pc, #64] ; [0x08023190] = 0x52002000
0802314e  0028      cmp	r0, #0
08023150  019b      ldr	r3, [sp, #4]
08023152  d1f80c21  ldr.w	r2, [r1, #268]
08023156  18bf      it	ne
08023158  0123      movne	r3, #1
0802315a  22f00802  bic	r2, r2, #8
0802315e  c1f80c21  str.w	r2, [r1, #268]
08023162  b9e7      b	#-142 ; -> 0x080230d8 ; branch_target=0x080230d8
08023164  0223      movs	r3, #2
08023166  1846      mov	r0, r3
08023168  02b0      add	sp, #8
0802316a  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0802316e  2946      mov	r1, r5
08023170  4cf25030  movw	r0, #50000
08023174  fff78efe  bl	#-740 ; -> 0x08022e94 ; branch_target=0x08022e94
08023178  0549      ldr	r1, [pc, #20] ; [0x08023190] = 0x52002000
0802317a  031e      subs	r3, r0, #0
0802317c  ca68      ldr	r2, [r1, #12]
0802317e  18bf      it	ne
08023180  0123      movne	r3, #1
08023182  22f00802  bic	r2, r2, #8
08023186  ca60      str	r2, [r1, #12]
08023188  6268      ldr	r2, [r4, #4]
0802318a  d7e7      b	#-82 ; -> 0x0802313c ; branch_target=0x0802313c
