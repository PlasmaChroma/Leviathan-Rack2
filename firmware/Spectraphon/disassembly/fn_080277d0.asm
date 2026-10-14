; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080277d0  036a      ldr	r3, [r0, #32]
080277d2  1b4a      ldr	r2, [pc, #108] ; [0x08027840] = 0xfffeff8c
080277d4  23f00103  bic	r3, r3, #1
080277d8  70b4      push	{r4, r5, r6}
080277da  0362      str	r3, [r0, #32]
080277dc  036a      ldr	r3, [r0, #32]
080277de  4468      ldr	r4, [r0, #4]
080277e0  8569      ldr	r5, [r0, #24]
080277e2  23f00203  bic	r3, r3, #2
080277e6  2a40      ands	r2, r5
080277e8  0d68      ldr	r5, [r1]
080277ea  2a43      orrs	r2, r5
080277ec  8d68      ldr	r5, [r1, #8]
080277ee  2b43      orrs	r3, r5
080277f0  144d      ldr	r5, [pc, #80] ; [0x08027844] = 0x40010000 / f32_bits_interpretation=2.015625
080277f2  a842      cmp	r0, r5
080277f4  0fd0      beq	#30 ; -> 0x08027816 ; branch_target=0x08027816
080277f6  05f58065  add.w	r5, r5, #1024
080277fa  a842      cmp	r0, r5
080277fc  0bd0      beq	#22 ; -> 0x08027816 ; branch_target=0x08027816
080277fe  05f57055  add.w	r5, r5, #15360
08027802  a842      cmp	r0, r5
08027804  07d0      beq	#14 ; -> 0x08027816 ; branch_target=0x08027816
08027806  05f58065  add.w	r5, r5, #1024
0802780a  a842      cmp	r0, r5
0802780c  03d0      beq	#6 ; -> 0x08027816 ; branch_target=0x08027816
0802780e  05f58065  add.w	r5, r5, #1024
08027812  a842      cmp	r0, r5
08027814  0dd1      bne	#26 ; -> 0x08027832 ; branch_target=0x08027832
08027816  cd68      ldr	r5, [r1, #12]
08027818  23f00803  bic	r3, r3, #8
0802781c  24f44074  bic	r4, r4, #768
08027820  2b43      orrs	r3, r5
08027822  d1e90565  ldrd	r6, r5, [r1, #20]
08027826  23f00403  bic	r3, r3, #4
0802782a  46ea050c  orr.w	r12, r6, r5
0802782e  4cea0404  orr.w	r4, r12, r4
08027832  4460      str	r4, [r0, #4]
08027834  8261      str	r2, [r0, #24]
08027836  4a68      ldr	r2, [r1, #4]
08027838  70bc      pop	{r4, r5, r6}
0802783a  4263      str	r2, [r0, #52]
0802783c  0362      str	r3, [r0, #32]
0802783e  7047      bx	lr
