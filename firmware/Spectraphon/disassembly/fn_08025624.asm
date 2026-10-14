; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08025624  38b5      push	{r3, r4, r5, lr}
08025626  437a      ldrb	r3, [r0, #9]
08025628  0446      mov	r4, r0
0802562a  03f0ff02  and	r2, r3, #255
0802562e  43b3      cbz	r3, #80 ; -> 0x08025682 ; branch_target=0x08025682
08025630  2268      ldr	r2, [r4]
08025632  0223      movs	r3, #2
08025634  6168      ldr	r1, [r4, #4]
08025636  6372      strb	r3, [r4, #9]
08025638  1368      ldr	r3, [r2]
0802563a  23f02003  bic	r3, r3, #32
0802563e  0b43      orrs	r3, r1
08025640  1360      str	r3, [r2]
08025642  2268      ldr	r2, [r4]
08025644  1368      ldr	r3, [r2]
08025646  43f00403  orr	r3, r3, #4
0802564a  1360      str	r3, [r2]
0802564c  2368      ldr	r3, [r4]
0802564e  5b68      ldr	r3, [r3, #4]
08025650  5b06      lsls	r3, r3, #25
08025652  03d5      bpl	#6 ; -> 0x0802565c ; branch_target=0x0802565c
08025654  0423      movs	r3, #4
08025656  6372      strb	r3, [r4, #9]
08025658  0120      movs	r0, #1
0802565a  38bd      pop	{r3, r4, r5, pc}
0802565c  faf79efe  bl	#-21188 ; -> 0x0802039c ; branch_target=0x0802039c
08025660  0546      mov	r5, r0
08025662  04e0      b	#8 ; -> 0x0802566e ; branch_target=0x0802566e
08025664  faf79afe  bl	#-21196 ; -> 0x0802039c ; branch_target=0x0802039c
08025668  431b      subs	r3, r0, r5
0802566a  022b      cmp	r3, #2
0802566c  0dd8      bhi	#26 ; -> 0x0802568a ; branch_target=0x0802568a
0802566e  2368      ldr	r3, [r4]
08025670  5b68      ldr	r3, [r3, #4]
08025672  13f00403  ands	r3, r3, #4
08025676  f5d1      bne	#-22 ; -> 0x08025664 ; branch_target=0x08025664
08025678  0122      movs	r2, #1
0802567a  1846      mov	r0, r3
0802567c  6272      strb	r2, [r4, #9]
0802567e  e360      str	r3, [r4, #12]
08025680  38bd      pop	{r3, r4, r5, pc}
08025682  0272      strb	r2, [r0, #8]
08025684  0ff050fd  bl	#64160 ; -> 0x08035128 ; branch_target=0x08035128
08025688  d2e7      b	#-92 ; -> 0x08025630 ; branch_target=0x08025630
0802568a  0422      movs	r2, #4
0802568c  0223      movs	r3, #2
0802568e  6272      strb	r2, [r4, #9]
08025690  e360      str	r3, [r4, #12]
08025692  e1e7      b	#-62 ; -> 0x08025658 ; branch_target=0x08025658
