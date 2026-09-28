; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080203a8  38b5      push	{r3, r4, r5, lr}
080203aa  0446      mov	r4, r0
080203ac  fff7f6ff  bl	#-20 ; -> 0x0802039c ; branch_target=0x0802039c
080203b0  0546      mov	r5, r0
080203b2  631c      adds	r3, r4, #1
080203b4  02d0      beq	#4 ; -> 0x080203bc ; branch_target=0x080203bc
080203b6  044b      ldr	r3, [pc, #16] ; [0x080203c8] = 0x20000000
080203b8  1b78      ldrb	r3, [r3]
080203ba  1c44      add	r4, r3
080203bc  fff7eeff  bl	#-36 ; -> 0x0802039c ; branch_target=0x0802039c
080203c0  401b      subs	r0, r0, r5
080203c2  a042      cmp	r0, r4
080203c4  fad3      blo	#-12 ; -> 0x080203bc ; branch_target=0x080203bc
080203c6  38bd      pop	{r3, r4, r5, pc}
