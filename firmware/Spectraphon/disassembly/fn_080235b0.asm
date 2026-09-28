; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080235b0  38b5      push	{r3, r4, r5, lr}
080235b2  104c      ldr	r4, [pc, #64] ; [0x080235f4] = 0x58024800
080235b4  e368      ldr	r3, [r4, #12]
080235b6  13f0040f  tst.w	r3, #4
080235ba  e368      ldr	r3, [r4, #12]
080235bc  05d1      bne	#10 ; -> 0x080235ca ; branch_target=0x080235ca
080235be  03f00703  and	r3, r3, #7
080235c2  181a      subs	r0, r3, r0
080235c4  18bf      it	ne
080235c6  0120      movne	r0, #1
080235c8  38bd      pop	{r3, r4, r5, pc}
080235ca  23f00703  bic	r3, r3, #7
080235ce  0343      orrs	r3, r0
080235d0  e360      str	r3, [r4, #12]
080235d2  fcf7e3fe  bl	#-12858 ; -> 0x0802039c ; branch_target=0x0802039c
080235d6  0546      mov	r5, r0
080235d8  05e0      b	#10 ; -> 0x080235e6 ; branch_target=0x080235e6
080235da  fcf7dffe  bl	#-12866 ; -> 0x0802039c ; branch_target=0x0802039c
080235de  401b      subs	r0, r0, r5
080235e0  b0f57a7f  cmp.w	r0, #1000
080235e4  04d8      bhi	#8 ; -> 0x080235f0 ; branch_target=0x080235f0
080235e6  6368      ldr	r3, [r4, #4]
080235e8  9b04      lsls	r3, r3, #18
080235ea  f6d5      bpl	#-20 ; -> 0x080235da ; branch_target=0x080235da
080235ec  0020      movs	r0, #0
080235ee  38bd      pop	{r3, r4, r5, pc}
080235f0  0120      movs	r0, #1
080235f2  38bd      pop	{r3, r4, r5, pc}
