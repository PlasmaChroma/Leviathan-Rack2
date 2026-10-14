; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026c54  38b5      push	{r3, r4, r5, lr}
08026c56  90f82c30  ldrb.w	r3, [r0, #44]
08026c5a  0446      mov	r4, r0
08026c5c  022b      cmp	r3, #2
08026c5e  d8b2      uxtb	r0, r3
08026c60  05d0      beq	#10 ; -> 0x08026c6e ; branch_target=0x08026c6e
08026c62  94f82c30  ldrb.w	r3, [r4, #44]
08026c66  012b      cmp	r3, #1
08026c68  ddb2      uxtb	r5, r3
08026c6a  01d0      beq	#2 ; -> 0x08026c70 ; branch_target=0x08026c70
08026c6c  0120      movs	r0, #1
08026c6e  38bd      pop	{r3, r4, r5, pc}
08026c70  0223      movs	r3, #2
08026c72  2068      ldr	r0, [r4]
08026c74  84f82c30  strb.w	r3, [r4, #44]
08026c78  01f0e6fb  bl	#6092 ; -> 0x08028448 ; branch_target=0x08028448
08026c7c  0020      movs	r0, #0
08026c7e  84f82c50  strb.w	r5, [r4, #44]
08026c82  38bd      pop	{r3, r4, r5, pc}
